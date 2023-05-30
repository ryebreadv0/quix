#ifndef _QUIX_DEVICE_HPP
#define _QUIX_DEVICE_HPP

#include "quix_common.hpp"

namespace quix {

class swapchain;

class device {
    friend class swapchain;

public:
    device(const char* app_name,
        uint32_t app_version,
        const char* engine_name,
        uint32_t engine_version,
        uint32_t width,
        uint32_t height);

    ~device();

    void init(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features);

    device(const device&) = delete;
    device& operator=(const device&) = delete;
    device(device&&) = delete;
    device& operator=(device&&) = delete;

    NODISCARD inline GLFWwindow* get_window() const noexcept { return window; }
    NODISCARD inline spdlog::sink_ptr get_sink() const noexcept { return m_logger.get_sink(0); }
    NODISCARD inline VkPhysicalDevice get_physical_device() const noexcept { return m_physical_device; }
    NODISCARD inline VkSurfaceKHR get_surface() const noexcept { return m_surface; }
    NODISCARD inline VkDevice get_logical_device() const noexcept { return m_logical_device; }
    NODISCARD inline VmaAllocator get_allocator() const noexcept { return m_allocator; }
    NODISCARD inline queue_family_indices get_queue_family_indices() const noexcept { return m_queue_family_indices.value(); }
    NODISCARD inline VkQueue get_graphics_queue() const noexcept { return m_graphics_queue; }
    NODISCARD inline VkQueue get_present_queue() const noexcept { return m_present_queue; }

    NODISCARD VkCommandPool get_command_pool();
    void return_command_pool(VkCommandPool command_pool);

    inline void wait_idle() { vkDeviceWaitIdle(m_logical_device); }

private:
    void create_instance(const char* app_name,
        uint32_t app_version,
        const char* engine_name,
        uint32_t engine_version);
    void create_window(const char* app_name, uint32_t width, uint32_t height);
    void create_surface();

    queue_family_indices find_queue_families(VkPhysicalDevice physical_device);
    bool check_device_extension_support(VkPhysicalDevice physical_device);
    swapchain_support_details query_swapchain_support(
        VkPhysicalDevice physical_device);

    bool is_physical_device_suitable(VkPhysicalDevice physical_device);
    int get_supported_feature_score(VkPhysicalDevice physical_device);
    int rate_physical_device(VkPhysicalDevice physical_device);
    void pick_physical_device();
    void create_logical_device();
    void create_allocator();

    // instance variables
    GLFWwindow* window;
    logger m_logger;

#ifdef _DEBUG
    bool initialized = false;
#endif

    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkDevice m_logical_device = VK_NULL_HANDLE;

    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkQueue m_graphics_queue = VK_NULL_HANDLE;
    VkQueue m_present_queue = VK_NULL_HANDLE;

    static constexpr uint32_t vk_api_version = VK_API_VERSION_1_3;

    std::vector<const char*> requested_extensions {};
    VkPhysicalDeviceFeatures requested_features {};

    std::optional<queue_family_indices> m_queue_family_indices;

    std::deque<VkCommandPool> m_command_pools;
    std::mutex m_command_pool_mutex;
};

} // namespace quix

#endif // _QUIX_DEVICE_HPP