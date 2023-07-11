#ifndef _QUIX_INSTANCE_HPP
#define _QUIX_INSTANCE_HPP

#include "quix_resource.hpp"
namespace quix {

class window;
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

class buffer_handle;

class instance {
public:
    instance(const char* app_name, uint32_t app_version, int width, int height);
    ~instance();

    instance(const instance&) = delete;
    instance& operator=(const instance&) = delete;
    instance(instance&&) = delete;
    instance& operator=(instance&&) = delete;

    void create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features);
    void create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode, const bool depth_buffer);

    NODISCARD render_target create_single_pass_render_target() noexcept;
    NODISCARD render_target create_single_pass_depth_render_target() noexcept;
    NODISCARD render_target create_render_target(const VkRenderPassCreateInfo&& render_pass_create_info) noexcept;
    NODISCARD sync create_sync_objects() noexcept;
    
    NODISCARD buffer_handle create_buffer_handle() const noexcept;
    NODISCARD image_handle create_image_handle() const noexcept;

    void wait_idle();

    NODISCARD weakref<window> get_window() const noexcept;
    NODISCARD VkDevice get_logical_device() const noexcept;
    NODISCARD VkSurfaceFormatKHR get_surface_format() const noexcept;

    NODISCARD weakref<graphics::pipeline_manager> get_pipeline_manager() noexcept;
    NODISCARD command_pool get_command_pool();

    NODISCARD descriptor::allocator_pool get_descriptor_allocator_pool() const noexcept;
    NODISCARD descriptor::builder get_descriptor_builder(descriptor::allocator_pool* allocator_pool) const noexcept;

    NODISCARD VkFence create_fence(VkFenceCreateFlags flags = 0);

private:
    friend class swapchain;

    NODISCARD weakref<device> get_device() const noexcept;
    void create_pipeline_manager();

    static constexpr std::size_t m_buffer_size = 2048;
    std::array<char, m_buffer_size> m_buffer{};
    std::pmr::monotonic_buffer_resource m_allocator{m_buffer.data(), m_buffer_size};

    allocated_unique_ptr<window> m_window;
    allocated_unique_ptr<device> m_device;
    allocated_unique_ptr<swapchain> m_swapchain;
    allocated_unique_ptr<graphics::pipeline_manager> m_pipeline_manager;
    allocated_unique_ptr<descriptor::allocator> m_descriptor_allocator;
    allocated_unique_ptr<descriptor::layout_cache> m_descriptor_layout_cache;
};

} // namespace quix

#endif // _QUIX_DEVICE_HPP
