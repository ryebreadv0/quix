#ifndef _QUIX_COMMON_HPP
#define _QUIX_COMMON_HPP

namespace quix {

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
NODISCARD constexpr inline Type* allocate_ptr(Allocator& allocator, Args&&... args)
    requires is_allocator<Allocator>
{
    void* allocation = allocator.allocate(sizeof(Type), alignof(Type));
    allocation = new Type { std::forward<Args>(args)... };
    return (Type*)allocation;
}

template <typename Type, typename Allocator, typename... Args>
NODISCARD constexpr inline std::shared_ptr<Type> allocate_shared(Allocator* allocator, Args&&... args)
    requires is_allocator<Allocator>
{
    return std::allocate_shared<Type, std::pmr::polymorphic_allocator<Type>>(allocator, std::forward<Args>(args)...);
}

template <typename Type>
using allocated_unique_ptr = std::unique_ptr<Type, std::function<void(Type*)>>;

template <typename Type, typename Allocator, typename... Args>
NODISCARD constexpr inline allocated_unique_ptr<Type> allocate_unique(Allocator* allocator, Args&&... args)
    requires is_allocator<Allocator>
{
    void* allocation = allocator->allocate(sizeof(Type), alignof(Type));
    Type* object = new (allocation) Type { std::forward<Args>(args)... };
    return allocated_unique_ptr<Type> {
        object, [allocator](Type* ptr) {
            ptr->~Type();
            allocator->deallocate(ptr, sizeof(Type), alignof(Type));
        }
    };
}

template <typename StrType>
constexpr inline void VK_CHECK(VkResult result, StrType error, const char* file, int line) noexcept(true)
{
    UNLIKELY if (result != VK_SUCCESS)
    {
        fmt::print("VK_ERROR [{}:{}] {}\n", file, line, error);
        // exit(EXIT_FAILURE);
        abort();
    }
}

#define VK_CHECK(result, error) quix::VK_CHECK(result, error, __FILE__, __LINE__)

template <typename BoolType>
concept EvaluatableBoolean = requires(BoolType cond) {
    {
        cond
    } -> std::convertible_to<bool>;
};

template <typename BoolType, typename StrType>
constexpr inline void quix_assert(BoolType cond, StrType error, const char* cond_string, const char* file, int line) noexcept
    requires EvaluatableBoolean<BoolType>
{
    UNLIKELY if (!cond)
    {
        fmt::print("Assert failed: [{}] [{}:{}] {}\n", cond_string, file, line, error);
        // exit(EXIT_FAILURE);
        abort();
    }
}

#define quix_assert(cond, msg) quix::quix_assert(cond, msg, #cond, __FILE__, __LINE__)

template <typename StrType>
constexpr inline void quix_error(StrType error, const char* file, int line) noexcept
{
    fmt::print("Error [{}:{}] {}", file, line, error);
    // exit(EXIT_FAILURE);
    abort();
}

#define quix_error(error) quix::quix_error(error, __FILE__, __LINE__)

// a non-owning pointer
template <typename Type>
struct weakref {
    constexpr explicit weakref(Type* ptr)
        : ptr(ptr)
    {
    }
    constexpr explicit weakref(const std::shared_ptr<Type>& ptr)
        : ptr(ptr.get())
    {
    }
    constexpr explicit weakref(const std::unique_ptr<Type>& ptr)
        : ptr(ptr.get())
    {
    }
    constexpr explicit weakref(const allocated_unique_ptr<Type>& ptr)
        : ptr(ptr.get())
    {
    }

    ~weakref() = default;
    weakref(const weakref& other)
        : ptr(other.ptr)
    {
    }
    weakref& operator=(const weakref& other) = default;
    weakref(weakref&& other) noexcept = default;
    weakref& operator=(weakref&& other) noexcept = default;

    NODISCARD constexpr inline Type* get() const noexcept { return ptr; }
    NODISCARD constexpr inline Type* operator->() const noexcept { return ptr; }

private:
    Type* ptr;
};

template <typename Type, typename PtrType>
constexpr inline auto make_weakref(const PtrType& ptr) -> weakref<Type>
{
    static_assert(std::is_constructible<weakref<Type>, PtrType>(), "Cannot make weakref from this pointer type");
    return weakref<Type>(ptr);
}

} // namespace quix

#endif // _QUIX_COMMON_HPP