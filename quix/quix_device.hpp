#ifndef _QUIX_DEVICE_HPP
#define _QUIX_DEVICE_HPP

namespace quix {

class device_impl;
class swapchain;

class device {
public:
    device(const char* app_name, uint32_t app_version, uint32_t width, uint32_t height, 
        std::vector<const char*>&& requested_extensions,
        VkPhysicalDeviceFeatures&& requested_features);

    ~device();

    device(const device&) = delete;
    device& operator=(const device&) = delete;
    device(device&&) = delete;
    device& operator=(device&&) = delete;

    NODISCARD GLFWwindow* window() const noexcept;
    void create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode);

private:
    NODISCARD std::shared_ptr<device_impl> impl() const noexcept;

    std::shared_ptr<device_impl> m_impl = nullptr;
    std::shared_ptr<swapchain> m_swapchain = nullptr;
};

} // namespace quix

#endif // _QUIX_DEVICE_HPP