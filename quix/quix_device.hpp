#ifndef _QUIX_DEVICE_HPP
#define _QUIX_DEVICE_HPP

namespace quix {

class device_impl;

class device {
public:
    device(const char* app_name, uint32_t app_version);
    ~device();

    device(const device&) = delete;
    device& operator=(const device&) = delete;
    device(device&&) = delete;
    device& operator=(device&&) = delete;

    NODISCARD GLFWwindow* window() const noexcept;

private:

    std::unique_ptr<device_impl> m_impl;



};


} // namespace quix

#endif // _QUIX_DEVICE_HPP