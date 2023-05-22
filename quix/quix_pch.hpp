#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <GLFW/glfw3native.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <optional>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <ranges>


#include <cmath>

#include "quix_logger.hpp"

#define NODISCARD [[nodiscard]]