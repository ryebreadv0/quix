#ifndef _QUIX_INSTANCE_CPP
#define _QUIX_INSTANCE_CPP

#include "quix_instance.hpp"

#include "quix_device.hpp"
#include "quix_swapchain.hpp"
#include "quix_common.hpp"

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
{
}

instance::~instance() = default;

NODISCARD GLFWwindow*
instance::window() const noexcept
{
    return m_device->get_window();
}

NODISCARD std::shared_ptr<device> instance::impl() const noexcept {
    return m_device;
}

void instance::create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features)
{
    m_device->init(std::move(requested_extensions), requested_features);
}

void instance::create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode)
{
    m_swapchain = std::make_shared<swapchain>(m_device, frames_in_flight, present_mode);
}
 

} // namespace quix

#endif // _QUIX_DEVICE_CPP