#ifndef _QUIX_SWAPCHAIN_CPP
#define _QUIX_SWAPCHAIN_CPP

#include "quix_swapchain.hpp"

#include "quix_device.hpp"
#include "quix_instance.hpp"
#include "quix_common.hpp"

namespace quix {

swapchain::swapchain(std::shared_ptr<device> device, const int32_t frames_in_flight, const VkPresentModeKHR present_mode)
    : m_device(device), m_frames_in_flight(frames_in_flight), m_present_mode(present_mode), m_logger("swapchain")
{
    m_logger.set_level(spdlog::level::trace);
    m_logger.add_sink(m_device->get_sink());
    m_logger.trace("Created swapchain class");

    create_swapchain();
    create_image_views();
}

swapchain::~swapchain() 
{
    destroy_image_views();
    destroy_swapchain();

    m_logger.trace("Destroyed swapchain class");
}

void swapchain::create_swapchain() 
{
    swapchain_support_details swapchain_support = m_device->query_swapchain_support(m_device->get_physical_device());

    m_swapchain_surface_format = choose_swap_surface_format(swapchain_support.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(swapchain_support.present_modes);
    m_swapchain_extent = choose_swap_extent(swapchain_support.capabilities);

    uint32_t imageCount = swapchain_support.capabilities.minImageCount + 1;
    if (swapchain_support.capabilities.maxImageCount > 0 && imageCount > swapchain_support.capabilities.maxImageCount) {
        imageCount = swapchain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_device->get_surface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_swapchain_surface_format.format;
    createInfo.imageColorSpace = m_swapchain_surface_format.colorSpace;
    createInfo.imageExtent = m_swapchain_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    queue_family_indices indices = m_device->find_queue_families(m_device->get_physical_device());
    uint32_t queueFamilyIndices[] = {indices.graphics_family.value(), indices.present_family.value()};

    if (indices.graphics_family != indices.present_family) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapchain_support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = present_mode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_device->get_logical_device(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
        m_logger.error("failed to create swap chain!");
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_device->get_logical_device(), m_swapchain, &imageCount, nullptr);
    m_swapchain_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device->get_logical_device(), m_swapchain, &imageCount, m_swapchain_images.data());

    m_logger.trace("Created swapchain");
}

void swapchain::destroy_swapchain() 
{
    vkDestroySwapchainKHR(m_device->get_logical_device(), m_swapchain, nullptr);
    m_logger.trace("Destroyed swapchain");
}

void swapchain::create_image_views()
{
    m_swapchain_image_views.resize(m_swapchain_images.size());

    for (int i = 0; i < m_swapchain_images.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapchain_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapchain_surface_format.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device->get_logical_device(), &createInfo, nullptr, &m_swapchain_image_views[i]) != VK_SUCCESS) {
            m_logger.error("failed to create image views!");
            throw std::runtime_error("failed to create image views!");
        }
    }

    m_logger.trace("Created image views");
}

void swapchain::destroy_image_views()
{
    for (auto& image_view : m_swapchain_image_views) {
        vkDestroyImageView(m_device->get_logical_device(), image_view, nullptr);
    }
}



VkSurfaceFormatKHR swapchain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) const noexcept
{
    for (const auto& format : available_formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    m_logger.warn("preferred surface format was not found, using first available");
    return available_formats[0];
}

VkPresentModeKHR swapchain::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) const noexcept 
{
    for (const auto& availablePresentMode : available_present_modes) {
        if (availablePresentMode == m_present_mode) {
            return availablePresentMode;
        }
    }

    m_logger.warn("preferred present mode was not found, using FIFO");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swapchain::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) const noexcept
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(m_device->get_window(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

} // namespace quix

#endif // _QUIX_SWAPCHAIN_CPP