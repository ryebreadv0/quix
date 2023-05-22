#ifndef _QUIX_DEVICE_CPP
#define _QUIX_DEVICE_CPP

#include "quix_device.hpp"

#include "quix_device_impl.hpp"
#include "quix_swapchain.hpp"
#include "quix_common.hpp"

namespace quix {

// device class
device::device(const char* app_name,
    uint32_t app_version,
    uint32_t width,
    uint32_t height,
    std::vector<const char*>&& requested_extensions,
    VkPhysicalDeviceFeatures&& requested_features)
    : m_impl(std::make_shared<device_impl>(app_name,
        app_version,
        "quix",
        VK_MAKE_VERSION(1, 0, 0),
        this,
        width,
        height, 
        std::move(requested_extensions), 
        std::move(requested_features)))
{
}

device::~device() = default;

NODISCARD GLFWwindow*
device::window() const noexcept
{
    return m_impl->get_window();
}

NODISCARD std::shared_ptr<device_impl> device::impl() const noexcept {
    return m_impl;
}

void device::create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode)
{
    m_swapchain = std::make_shared<swapchain>(m_impl, frames_in_flight, present_mode);
}
 

} // namespace quix

#endif // _QUIX_DEVICE_CPP