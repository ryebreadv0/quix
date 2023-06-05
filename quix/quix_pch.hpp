#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vma/vk_mem_alloc.h>

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cassert>
#undef assert

#include <cmath>

#define NODISCARD [[nodiscard]]
#define MAYBEUNUSED [[maybe_unused]]
#define UNLIKELY [[unlikely]]

template <typename StrType>
constexpr inline void VK_CHECK(VkResult result, StrType error, const char* file, int line) noexcept(true)
{
    UNLIKELY if (result != VK_SUCCESS)
    {
        fmt::print("VK_ERROR [{}:{}] {}\n", file, line, error);
        exit(EXIT_FAILURE);
    }
}

#define VK_CHECK(result, error) VK_CHECK(result, error, __FILE__, __LINE__)

template <typename BoolType>
concept EvaluatableBoolean = requires(BoolType cond) {
    {
        cond
    } -> std::convertible_to<bool>;
};

template <typename BoolType, typename StrType>
constexpr inline void quix_assert(BoolType cond, StrType error, const char* cond_string, const char* file, int line) noexcept(true)
    requires EvaluatableBoolean<BoolType>
{
    UNLIKELY if (!cond)
    {
        fmt::print("Assert failed: [{}] [{}:{}] {}\n", cond_string, file, line, error);
        exit(EXIT_FAILURE);
    }
}

#define quix_assert(cond, msg) quix_assert(cond, msg, #cond, __FILE__, __LINE__)

template <typename StrType>
constexpr inline void quix_error(StrType error, const char* file, int line) noexcept(true)
{
    fmt::print("Error [{}:{}] {}", file, line, error);
    exit(EXIT_FAILURE);
}

#define quix_error(error) quix_error(error, __FILE__, __LINE__)

#include "quix_logger.hpp"
