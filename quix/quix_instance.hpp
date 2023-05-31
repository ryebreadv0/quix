#ifndef _QUIX_INSTANCE_HPP
#define _QUIX_INSTANCE_HPP

namespace quix {

class device;
class swapchain;
class render_target;

namespace graphics {
    class pipeline_manager;
}

namespace descriptor {
    class allocator;
    class layout_cache;
    class builder;
    struct allocator_pool;
}

class sync;
class command_pool;

class instance {
public:

    instance(const char* app_name, uint32_t app_version, int width, int height);
    ~instance();

    instance(const instance&) = delete;
    instance& operator=(const instance&) = delete;
    instance(instance&&) = delete;
    instance& operator=(instance&&) = delete;

    void create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features);
    void create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode);
    void create_pipeline_manager();
    NODISCARD std::shared_ptr<render_target> create_render_target(const VkRenderPassCreateInfo&& render_pass_create_info) const noexcept;
    NODISCARD std::shared_ptr<sync> create_sync_objects() const noexcept;

    void wait_idle();

    NODISCARD GLFWwindow* window() const noexcept;
    NODISCARD VkDevice get_logical_device() const noexcept;
    NODISCARD VkSurfaceFormatKHR get_surface_format() const noexcept;

    NODISCARD std::shared_ptr<graphics::pipeline_manager> get_pipeline_manager() const noexcept;
    NODISCARD std::shared_ptr<command_pool> get_command_pool() const;

    NODISCARD descriptor::allocator_pool get_descriptor_allocator_pool() const noexcept; 
    NODISCARD descriptor::builder get_descriptor_builder(descriptor::allocator_pool* allocator_pool) const noexcept;

private:

    friend class swapchain;
    
    NODISCARD std::shared_ptr<device> get_device() const noexcept;


    std::shared_ptr<device> m_device;
    std::shared_ptr<swapchain> m_swapchain;
    std::shared_ptr<graphics::pipeline_manager> m_pipeline_manager;
    
    std::unique_ptr<descriptor::allocator> m_descriptor_allocator;
    std::unique_ptr<descriptor::layout_cache> m_descriptor_layout_cache;

};

} // namespace quix

#endif // _QUIX_DEVICE_HPP