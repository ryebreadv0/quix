#ifndef _QUIX_DEVICE_CPP
#define _QUIX_DEVICE_CPP

#include "quix_device.hpp"

#include "quix_window.hpp"

namespace quix {

device::device(weakref<window> p_window,
    const char* app_name,
    uint32_t app_version,
    const char* engine_name,
    uint32_t engine_version)
    : m_window(p_window)
{

    glslang::InitializeProcess();

    // auto result = glfwInit();
    // quix_assert(result != GLFW_FALSE, "failed to initialize GLFW");

    create_instance(app_name, app_version, engine_name, engine_version);

    // create_window(app_name, width, height);
}

device::~device()
{
#ifdef _DEBUG
    if (initialized != true) {
        spdlog::error("error device not initialized");
    }
#endif

    for (auto& pool : m_command_pools) {
        vkDestroyCommandPool(m_logical_device, pool, nullptr);
    }

    vmaDestroyAllocator(m_allocator);

    vkDestroyDevice(m_logical_device, nullptr);

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    vkDestroyInstance(m_instance, nullptr);

    glslang::FinalizeProcess();
}

void device::init(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features)
{
#ifdef _DEBUG
    quix_assert(initialized == false, "device already initialized");
    initialized = true;
#endif

    this->requested_extensions = std::move(requested_extensions);
    this->requested_features = requested_features;

    spdlog::info("requested extensions: {}", requested_extensions.size());

    create_surface();

    pick_physical_device();

    create_logical_device();

    create_allocator();
}

NODISCARD VkCommandPool device::get_command_pool()
{
    if (m_command_pools.empty()) {
        quix_assert(m_queue_family_indices.has_value(), "queue family indices not initialized");
        VkCommandPoolCreateInfo pool_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = m_queue_family_indices->graphics_family.value()
        };

        VkCommandPool pool = VK_NULL_HANDLE;
        VK_CHECK(vkCreateCommandPool(m_logical_device, &pool_info, nullptr, &pool), "failed to create command pool");

        return pool;
    }

    std::lock_guard<std::mutex> lock(m_command_pool_mutex);
    VkCommandPool pool = m_command_pools.front();
    m_command_pools.pop_front();

    return pool;
}

void device::return_command_pool(VkCommandPool command_pool)
{
    {
        std::lock_guard<std::mutex> lock(m_command_pool_mutex); // TODO determine if it is worth it to release resources
        vkResetCommandPool(m_logical_device, command_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        m_command_pools.push_back(command_pool);
    }
}

void device::create_instance(const char* app_name,
    uint32_t app_version,
    const char* engine_name,
    uint32_t engine_version)
{
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = app_name,
        .applicationVersion = app_version,
        .pEngineName = engine_name,
        .engineVersion = engine_version,
        .apiVersion = vk_api_version
    };

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions
    };

    VK_CHECK(vkCreateInstance(&create_info, nullptr, &m_instance), "failed to create vulkan instance");
}

void device::create_surface()
{
    m_window->get_surface(m_instance, &m_surface);
}

queue_family_indices
device::find_queue_families(VkPhysicalDevice physical_device)
{
    if (m_queue_family_indices.has_value()) {
        return m_queue_family_indices.value();
    }
    queue_family_indices indices {};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamiliesProperties(
        queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device, &queueFamilyCount, queueFamiliesProperties.data());

    int iterator = 0;
    for (const auto& queueFamily : queueFamiliesProperties) {
        if (((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0U) && ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0U)) {
            indices.graphics_family = iterator;
        }

        VkBool32 presentSupport = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device, iterator, m_surface, &presentSupport), "failed to check physical device surface support");
        if (presentSupport != 0U) {
            indices.present_family = iterator;
        }

        if (indices.is_complete()) {
            break;
        }

        iterator++;
    }

    quix_assert(indices.is_complete(), "failed to find queue families");

    return indices;
}

bool device::check_device_extension_support(VkPhysicalDevice physical_device)
{
    uint32_t extensionCount {};
    vkEnumerateDeviceExtensionProperties(
        physical_device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        physical_device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions {};

    for (const auto& ext : requested_extensions) {
        requiredExtensions.insert(ext);
    }

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

swapchain_support_details
device::query_swapchain_support(VkPhysicalDevice physical_device)
{
    swapchain_support_details details {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device, m_surface, &details.capabilities);

    uint32_t formatCount {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device, m_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.present_modes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
            m_surface,
            &presentModeCount,
            details.present_modes.data());
    }

    return details;
}

bool device::is_physical_device_suitable(VkPhysicalDevice physical_device)
{
    queue_family_indices indices = this->find_queue_families(physical_device);

    bool extensions_supported = check_device_extension_support(physical_device);

    bool swapchain_adequate = false;
    if (extensions_supported) {
        swapchain_support_details swap_chain_support = query_swapchain_support(physical_device);
        swapchain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
    }

    return indices.is_complete() && extensions_supported && swapchain_adequate;
}

#define CHECK_VKDEVICE_FEATURE(feature)                                                  \
    {                                                                                    \
        if (supported_features.feature == false && requested_features.feature == true) { \
            return 0;                                                                    \
        } else {                                                                         \
            if (requested_features.feature == true)                                      \
                features_score += 100;                                                   \
            else {                                                                       \
                features_score += 10;                                                    \
            }                                                                            \
        }                                                                                \
    }

int device::get_supported_feature_score(VkPhysicalDevice physical_device)
{
    int features_score = 0;

    // if (requested_features == nullptr) {
    //     return features_score;
    // }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(physical_device, &supported_features);

    CHECK_VKDEVICE_FEATURE(robustBufferAccess);
    CHECK_VKDEVICE_FEATURE(fullDrawIndexUint32);
    CHECK_VKDEVICE_FEATURE(imageCubeArray);
    CHECK_VKDEVICE_FEATURE(independentBlend);
    CHECK_VKDEVICE_FEATURE(geometryShader);
    CHECK_VKDEVICE_FEATURE(tessellationShader);
    CHECK_VKDEVICE_FEATURE(sampleRateShading);
    CHECK_VKDEVICE_FEATURE(dualSrcBlend);
    CHECK_VKDEVICE_FEATURE(logicOp);
    CHECK_VKDEVICE_FEATURE(multiDrawIndirect);
    CHECK_VKDEVICE_FEATURE(drawIndirectFirstInstance);
    CHECK_VKDEVICE_FEATURE(depthClamp);
    CHECK_VKDEVICE_FEATURE(depthBiasClamp);
    CHECK_VKDEVICE_FEATURE(fillModeNonSolid);
    CHECK_VKDEVICE_FEATURE(depthBounds);
    CHECK_VKDEVICE_FEATURE(wideLines);
    CHECK_VKDEVICE_FEATURE(largePoints);
    CHECK_VKDEVICE_FEATURE(alphaToOne);
    CHECK_VKDEVICE_FEATURE(multiViewport);
    CHECK_VKDEVICE_FEATURE(samplerAnisotropy);
    CHECK_VKDEVICE_FEATURE(textureCompressionETC2);
    CHECK_VKDEVICE_FEATURE(textureCompressionASTC_LDR);
    CHECK_VKDEVICE_FEATURE(textureCompressionBC);
    CHECK_VKDEVICE_FEATURE(occlusionQueryPrecise);
    CHECK_VKDEVICE_FEATURE(pipelineStatisticsQuery);
    CHECK_VKDEVICE_FEATURE(vertexPipelineStoresAndAtomics);
    CHECK_VKDEVICE_FEATURE(fragmentStoresAndAtomics);
    CHECK_VKDEVICE_FEATURE(shaderTessellationAndGeometryPointSize);
    CHECK_VKDEVICE_FEATURE(shaderImageGatherExtended);
    CHECK_VKDEVICE_FEATURE(shaderStorageImageExtendedFormats);
    CHECK_VKDEVICE_FEATURE(shaderStorageImageMultisample);
    CHECK_VKDEVICE_FEATURE(shaderStorageImageReadWithoutFormat);
    CHECK_VKDEVICE_FEATURE(shaderStorageImageWriteWithoutFormat);
    CHECK_VKDEVICE_FEATURE(shaderUniformBufferArrayDynamicIndexing);
    CHECK_VKDEVICE_FEATURE(shaderSampledImageArrayDynamicIndexing);
    CHECK_VKDEVICE_FEATURE(shaderStorageBufferArrayDynamicIndexing);
    CHECK_VKDEVICE_FEATURE(shaderStorageImageArrayDynamicIndexing);
    CHECK_VKDEVICE_FEATURE(shaderClipDistance);
    CHECK_VKDEVICE_FEATURE(shaderCullDistance);
    CHECK_VKDEVICE_FEATURE(shaderFloat64);
    CHECK_VKDEVICE_FEATURE(shaderInt64);
    CHECK_VKDEVICE_FEATURE(shaderInt16);
    CHECK_VKDEVICE_FEATURE(shaderResourceResidency);
    CHECK_VKDEVICE_FEATURE(shaderResourceMinLod);
    CHECK_VKDEVICE_FEATURE(sparseBinding);
    CHECK_VKDEVICE_FEATURE(sparseResidencyBuffer);
    CHECK_VKDEVICE_FEATURE(sparseResidencyImage2D);
    CHECK_VKDEVICE_FEATURE(sparseResidencyImage3D);
    CHECK_VKDEVICE_FEATURE(sparseResidency2Samples);
    CHECK_VKDEVICE_FEATURE(sparseResidency4Samples);
    CHECK_VKDEVICE_FEATURE(sparseResidency8Samples);
    CHECK_VKDEVICE_FEATURE(sparseResidency16Samples);
    CHECK_VKDEVICE_FEATURE(sparseResidencyAliased);
    CHECK_VKDEVICE_FEATURE(variableMultisampleRate);
    CHECK_VKDEVICE_FEATURE(inheritedQueries);

    return features_score;
}

int device::rate_physical_device(VkPhysicalDevice physical_device)
{
    int score = get_supported_feature_score(physical_device);

    if (score == 0) {
        return 0;
    }

    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_props);

    VkDeviceSize deviceMemory = 0;
    for (uint32_t i = 0; i < mem_props.memoryHeapCount; ++i) {
        // const auto& heap = mem_props.memoryHeaps[i];
        if ((mem_props.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0U) {
            deviceMemory += mem_props.memoryHeaps[i].size;
        }
    }

    static constexpr double memory_score_multiplier = 20.0;
    static constexpr double memory_score_divider = 1000000000.0;

    score += static_cast<int>((static_cast<double>(deviceMemory) / memory_score_divider * memory_score_multiplier) + 0.5);

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    switch (properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        score += 1200;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        score += 600;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        score += 300;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        score += 10;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        score += 1;
        break;
    default:
        spdlog::warn("Unknown device type: {}", (size_t)properties.deviceType);
        score = 0;
        return score;
        break;
    }

    spdlog::trace("Device: {} score: {}", properties.deviceName, score);

    return score;
}

void device::pick_physical_device()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    quix_assert(deviceCount != 0, "failed to find GPUs with Vulkan support");

    auto* devices = static_cast<VkPhysicalDevice*>(
        alloca(sizeof(VkPhysicalDevice) * deviceCount));

    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices);

    std::multimap<int, VkPhysicalDevice> deviceRatings;

    spdlog::info("Found {} devices", deviceCount);

    // rate physicalDevice
    for (uint32_t i = 0; i < deviceCount; i++) {
        int score = rate_physical_device(devices[i]);
        deviceRatings.insert(std::make_pair(score, devices[i]));
    }

    for (auto it = deviceRatings.rbegin(); it != deviceRatings.rend(); ++it) {
        auto& deviceRating = *it;
        if (is_physical_device_suitable(deviceRating.second)) {
            if (deviceRating.first <= 0) {
                spdlog::warn("[WARNING] Device has a score of {} which means it does not support requested features", deviceRating.first);
                continue;
            }

            m_physical_device = deviceRating.second;
            m_queue_family_indices = find_queue_families(m_physical_device);
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(m_physical_device, &properties);
            max_sampler_anisotropy = properties.limits.maxSamplerAnisotropy;
            spdlog::info("Using device: {} with a score of {}", properties.deviceName, deviceRating.first);

            // maxMsaa = getMaxUsableSampleCount(); // TODO
            break;
        }
    }

    quix_assert(m_physical_device != VK_NULL_HANDLE, "failed to find a suitable GPU");
}

void device::create_logical_device()
{
    queue_family_indices indices = find_queue_families(m_physical_device);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphics_family.value(), indices.present_family.value() };

    float queuePriority = 1.0f;
    for (const uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // areAllFeaturesSupported(physicalDevice);
    // should be checked by the device scoring system (returns 0 if a requested feature/extension is not supported)

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &requested_features;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requested_extensions.size());
    createInfo.ppEnabledExtensionNames = requested_extensions.data();

    VK_CHECK(vkCreateDevice(m_physical_device, &createInfo, nullptr, &m_logical_device), "failed to create a logical device");

    vkGetDeviceQueue(m_logical_device, indices.graphics_family.value(), 0, &m_graphics_queue);
    vkGetDeviceQueue(m_logical_device, indices.present_family.value(), 0, &m_present_queue);
}

void device::create_allocator()
{
    VmaAllocatorCreateInfo allocatorInfo {};
    allocatorInfo.instance = m_instance,
    allocatorInfo.device = m_logical_device,
    allocatorInfo.physicalDevice = m_physical_device,
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3,

    VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator), "failed to create VMA allocator");
}

} // namespace quix

#endif // _QUIX_DEVICE_CPP
