#ifndef _QUIX_INSTANCE_CPP
#define _QUIX_INSTANCE_CPP

#include "quix_instance.hpp"

#include "quix_common.hpp"
#include "quix_descriptor.hpp"
#include "quix_device.hpp"
#include "quix_swapchain.hpp"

namespace quix {

instance::instance(const char* app_name,
    uint32_t app_version,
    uint32_t width,
    uint32_t height)
    : m_device(std::make_shared<device>(app_name,
        app_version,
        "quix",
        VK_MAKE_VERSION(1, 0, 0),
        width,
        height))
{}

instance::~instance() = default;

void instance::create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features)
{
    m_device->init(std::move(requested_extensions), requested_features);

    m_descriptor_allocator.reset(descriptor::allocator::init(m_device->get_logical_device()));
    m_descriptor_layout_cache.reset(new descriptor::layout_cache);
    m_descriptor_layout_cache->init(m_device->get_logical_device());
}

void instance::create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode)
{
    m_swapchain = std::make_shared<swapchain>(m_device, frames_in_flight, present_mode);
}

NODISCARD GLFWwindow*
instance::window() const noexcept
{
    return m_device->get_window();
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

NODISCARD descriptor::allocator_pool instance::get_descriptor_allocator_pool() const noexcept
{
    return m_descriptor_allocator->getPool();
}

NODISCARD descriptor::builder instance::get_descriptor_builder(descriptor::allocator_pool* allocator_pool) const noexcept
{
    return descriptor::builder::begin(m_descriptor_layout_cache.get(), allocator_pool);
}

NODISCARD std::shared_ptr<device> instance::get_device() const noexcept
{
    return m_device;
}

} // namespace quix

#endif // _QUIX_DEVICE_CPP