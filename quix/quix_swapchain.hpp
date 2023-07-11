#ifndef _QUIX_SWAPCHAIN_HPP
#define _QUIX_SWAPCHAIN_HPP

namespace quix {

class instance;
class window;
class device;
struct queue_family_indices;
class render_target;
class image_handle;

class swapchain {
    friend class instance;
    friend class device;
    friend class render_target;

public:
    swapchain(weakref<instance> p_instance, weakref<window> p_window, weakref<device> p_device, const int32_t frames_in_flight, const VkPresentModeKHR present_mode, const bool depth_buffer);
    ~swapchain();

    swapchain(const swapchain&) = delete;
    swapchain& operator=(const swapchain&) = delete;
    swapchain(swapchain&&) = delete;
    swapchain& operator=(swapchain&&) = delete;

    NODISCARD inline VkSwapchainKHR get_swapchain() const noexcept { return m_swapchain; }
    NODISCARD inline int32_t get_frames_in_flight() const noexcept { return m_frames_in_flight; }
    NODISCARD inline VkSurfaceFormatKHR get_surface_format() const noexcept { return m_swapchain_surface_format; }
    NODISCARD inline VkExtent2D get_extent() const noexcept { return m_swapchain_extent; }
    NODISCARD const inline std::vector<VkImageView>& get_image_views() const noexcept { return m_swapchain_image_views; }

private:
    void recreate_swapchain();

    void create_swapchain(VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);
    void destroy_swapchain();
    void create_image_views();
    void destroy_image_views();

    void create_depth_image();
    void destroy_depth_image();

    NODISCARD VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) const noexcept;
    NODISCARD VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) const noexcept;
    NODISCARD VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) const noexcept;

    // Depth buffering
    NODISCARD VkFormat find_supported_format(const std::initializer_list<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    NODISCARD VkFormat find_depth_format();
    NODISCARD static bool has_stencil_component(VkFormat format);

    weakref<instance> m_instance;
    weakref<window> m_window;
    weakref<device> m_device;

    int32_t m_frames_in_flight;
    VkPresentModeKHR m_present_mode;

    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapchain_images {};
    std::vector<VkImageView> m_swapchain_image_views {};
    VkSurfaceFormatKHR m_swapchain_surface_format {};
    VkExtent2D m_swapchain_extent {};

    bool depth_buffer_enabled;
    std::optional<VkFormat> depth_format;
    image_handle* depth_image{nullptr};

};

} // namespace quix

#endif // _QUIX_SWAPCHAIN_HPP
