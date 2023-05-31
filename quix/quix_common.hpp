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

template <typename Type, typename... Args>
NODISCARD static inline constexpr auto create_auto_array(Args&&... args)
{
    return std::array<Type, sizeof...(Args)> { std::forward<Args>(args)... };
}

// concept that requires it has allocate and deallocate functions
template <typename Type>
concept is_allocator = requires(Type a, size_t size, size_t alignment) {
    {
        a.allocate(size, alignment)
    } -> std::same_as<void*>;
    {
        a.deallocate(nullptr, size, alignment)
    } -> std::same_as<void>;
};

template <typename Type, typename Allocator, typename... Args>
NODISCARD constexpr inline Type* allocate_ptr(Allocator& allocator, Args... args)
    requires is_allocator<Allocator>
{
    void* allocation = allocator.allocate(sizeof(Type), alignof(Type));
    allocation = new Type { args... };
    return (Type*)allocation;
}

} // namespace quix

#endif // _QUIX_COMMON_HPP