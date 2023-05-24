#ifndef _QUIX_SWAPCHAIN_HPP
#define _QUIX_SWAPCHAIN_HPP

#include "quix_instance.hpp"

namespace quix {

struct queue_family_indices;

using swapchain = class instance::swapchain {
    friend class instance::device;
public:

    swapchain(std::shared_ptr<instance::device> p_device, const int32_t frames_in_flight, const VkPresentModeKHR present_mode);

    ~swapchain();
    swapchain(const swapchain&) = delete;
    swapchain& operator=(const swapchain&) = delete;
    swapchain(swapchain&&) = delete;
    swapchain& operator=(swapchain&&) = delete;

private:
    void create_swapchain();
    void destroy_swapchain();
    void create_image_views();
    void destroy_image_views();

    NODISCARD VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) const noexcept;
    NODISCARD VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) const noexcept;
    NODISCARD VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) const noexcept;

    std::shared_ptr<instance::device> m_device; 
    const int32_t m_frames_in_flight;
    const VkPresentModeKHR m_present_mode;
    logger m_logger;

    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_swapchain_images;
    std::vector<VkImageView> m_swapchain_image_views;
    VkSurfaceFormatKHR m_swapchain_surface_format;
    VkExtent2D m_swapchain_extent;

};

} // namespace quix

#endif // _QUIX_SWAPCHAIN_HPP