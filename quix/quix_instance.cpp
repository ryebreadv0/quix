#ifndef _QUIX_INSTANCE_CPP
#define _QUIX_INSTANCE_CPP

#include "quix_instance.hpp"

#include "quix_command_list.hpp"
#include "quix_common.hpp"
#include "quix_descriptor.hpp"
#include "quix_device.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"
#include "quix_swapchain.hpp"
#include "quix_window.hpp"
#include "quix_resource.hpp"

namespace quix {

consteval std::size_t get_allocation_size()
{
    return sizeof(window) + 
           sizeof(device) + 
           sizeof(swapchain) + 
           sizeof(graphics::pipeline_manager) + 
           sizeof(descriptor::allocator) + 
           sizeof(descriptor::layout_cache);
}

instance::instance(const char* app_name,
    uint32_t app_version,
    int width,
    int height)
    : m_window(allocate_unique<window>(&m_allocator, app_name, width, height))
    , m_device(allocate_unique<device>(&m_allocator,
          make_weakref<window>(m_window),
          app_name,
          app_version,
          "quix",
          VK_MAKE_VERSION(1, 0, 0)))
    , m_swapchain(nullptr)
    , m_pipeline_manager(nullptr)
    , m_descriptor_allocator(nullptr)
    , m_descriptor_layout_cache(nullptr)
{ 
    static_assert(get_allocation_size()
                   <= m_buffer_size, 
                   "instance buffer size is too small");
}

instance::~instance() = default;

void instance::create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features)
{
    m_device->init(std::move(requested_extensions), requested_features);

    m_descriptor_allocator = allocate_unique<descriptor::allocator>(&m_allocator, m_device->get_logical_device());
    m_descriptor_layout_cache = allocate_unique<descriptor::layout_cache>(&m_allocator, m_device->get_logical_device());
}

void instance::create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode)
{
    m_swapchain = allocate_unique<swapchain>(&m_allocator, make_weakref<window>(m_window), make_weakref<device>(m_device), frames_in_flight, present_mode);
}

void instance::create_pipeline_manager()
{
    // TODO : make pipeline manager threadsafe (probably)
    m_pipeline_manager = allocate_unique<graphics::pipeline_manager>(&m_allocator, make_weakref<device>(m_device));
}

NODISCARD command_pool instance::get_command_pool()
{
    return command_pool{
        make_weakref<device>(m_device), 
        m_device->get_command_pool()};
}

NODISCARD render_target instance::create_render_target(const VkRenderPassCreateInfo&& render_pass_create_info) noexcept
{
    return render_target{
        make_weakref<window>(m_window),
        make_weakref<device>(m_device),
        make_weakref<swapchain>(m_swapchain),
        &render_pass_create_info};
}

NODISCARD sync instance::create_sync_objects() noexcept
{
    return sync {
        make_weakref<device>(m_device),
        make_weakref<swapchain>(m_swapchain)};
}

NODISCARD buffer_handle instance::create_buffer_handle() const noexcept
{
    return buffer_handle {
        make_weakref<device>(m_device),
        };
}

void instance::wait_idle()
{
    m_device->wait_idle();
}

NODISCARD weakref<window>
instance::get_window() const noexcept
{
    return make_weakref<window>(m_window);
}

NODISCARD VkDevice
instance::get_logical_device() const noexcept
{
    return m_device->get_logical_device();
}

NODISCARD VkSurfaceFormatKHR instance::get_surface_format() const noexcept
{
    return m_swapchain->get_surface_format();
}

NODISCARD weakref<graphics::pipeline_manager> instance::get_pipeline_manager() const noexcept
{
    return make_weakref<graphics::pipeline_manager>(m_pipeline_manager);
}

NODISCARD descriptor::allocator_pool instance::get_descriptor_allocator_pool() const noexcept
{
    return m_descriptor_allocator->getPool();
}

NODISCARD descriptor::builder instance::get_descriptor_builder(descriptor::allocator_pool* allocator_pool) const noexcept
{
    return descriptor::builder { m_descriptor_layout_cache.get(), allocator_pool };
}

NODISCARD weakref<device> instance::get_device() const noexcept
{
    return weakref<device> { m_device };
}

} // namespace quix

#endif // _QUIX_DEVICE_CPP