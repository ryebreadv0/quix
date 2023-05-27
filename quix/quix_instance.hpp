#ifndef _QUIX_INSTANCE_HPP
#define _QUIX_INSTANCE_HPP

namespace quix {

class device;
class swapchain;

namespace descriptor {
    class allocator;
    class layout_cache;
    class builder;
    struct allocator_pool;
}

class instance {
public:

    instance(const char* app_name, uint32_t app_version, uint32_t width, uint32_t height);
    ~instance();

    instance(const instance&) = delete;
    instance& operator=(const instance&) = delete;
    instance(instance&&) = delete;
    instance& operator=(instance&&) = delete;

    void create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features);
    void create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode);

    NODISCARD GLFWwindow* window() const noexcept;
    NODISCARD VkDevice get_logical_device() const noexcept;
    NODISCARD VkSurfaceFormatKHR get_surface_format() const noexcept;

    NODISCARD descriptor::allocator_pool get_descriptor_allocator_pool() const noexcept; 
    NODISCARD descriptor::builder get_descriptor_builder(descriptor::allocator_pool* allocator_pool) const noexcept;

private:

    friend class swapchain;
    
    NODISCARD std::shared_ptr<device> get_device() const noexcept;


    std::shared_ptr<device> m_device = nullptr;
    std::shared_ptr<swapchain> m_swapchain = nullptr;
    
    std::unique_ptr<descriptor::allocator> m_descriptor_allocator = nullptr;
    std::unique_ptr<descriptor::layout_cache> m_descriptor_layout_cache = nullptr;

};

} // namespace quix

#endif // _QUIX_DEVICE_HPP