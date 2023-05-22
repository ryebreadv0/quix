#ifndef _QUIX_COMMON_HPP
#define _QUIX_COMMON_HPP

namespace quix {

struct queue_family_indices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    NODISCARD bool is_complete() const
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct swapchain_support_details {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};


} // namespace quix


#endif // _QUIX_COMMON_HPP