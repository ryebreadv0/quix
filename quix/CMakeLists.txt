cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror") # -fsanitize=address
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

project(quix C CXX)

add_library(${PROJECT_NAME}
    quix_instance.cpp
    quix_window.cpp
    quix_device.cpp
    quix_logger.cpp
    quix_swapchain.cpp
    quix_shader.cpp
    quix_pipeline.cpp
    quix_descriptor.cpp
    quix_render_target.cpp
    quix_commands.cpp
    quix_resource.cpp
)

# set_target_properties(${PROJECT_NAME} PROPERTIES
#     COMPILE_OPTIONS -fsanitize=address
#     LINK_OPTIONS -fsanitize=address
# )

target_precompile_headers(${PROJECT_NAME} 
    PUBLIC quix_pch.hpp
)

target_link_libraries(${PROJECT_NAME} 
    spdlog::spdlog
    Vulkan::Vulkan
    glfw
    quix_impl
    glslang::glslang
    glslang::glslang-default-resource-limits
    glslang::SPIRV
    glslang::SPVRemapper
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR}/quix
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${spdlog_SOURCE_DIRS}/include
    PUBLIC ${Vulkan_INCLUDE_DIRS}
    PUBLIC ${glfw_SOURCE_DIRS}/include
    PUBLIC ${stb_SOURCE_DIR}
)
