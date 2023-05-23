#ifndef _QUIX_INSTANCE_HPP
#define _QUIX_INSTANCE_HPP

namespace quix {

class device;
class swapchain;

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

private:
    friend class swapchain;
    
    
    NODISCARD std::shared_ptr<device> impl() const noexcept;

    std::shared_ptr<device> m_device = nullptr;
    std::shared_ptr<swapchain> m_swapchain = nullptr;
};

} // namespace quix

#endif // _QUIX_DEVICE_HPP