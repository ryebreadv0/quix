cmake_minimum_required(VERSION 3.22.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

project(quix)

add_library(${PROJECT_NAME} 
    quix/quix_device.cpp
    quix/quix_device_impl.cpp
    quix/quix_logger.cpp
    quix/quix_swapchain.cpp
)

target_precompile_headers(${PROJECT_NAME} 
    PUBLIC quix/quix_pch.hpp
)

include(cmake/CPM.cmake)

CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage("gh:glfw/glfw#3.3.7")

find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw REQUIRED)

add_library(VulkanMemoryAllocator
    quix/quix_vma.cpp
)

target_include_directories(VulkanMemoryAllocator
    PUBLIC ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
    spdlog::spdlog
    Vulkan::Vulkan
    glfw
    VulkanMemoryAllocator
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR}/quix
    PUBLIC ${spdlog_SOURCE_DIRS}/include
    PUBLIC ${Vulkan_INCLUDE_DIRS}
    PUBLIC ${glfw_SOURCE_DIRS}/include
)




add_executable(quix_example 
    examples/main.cpp
)

target_link_libraries(quix_example 
    ${PROJECT_NAME}
)
target_include_directories(quix_example 
    PUBLIC ${PROJECT_SOURCE_DIR}/quix
    PUBLIC ${spdlog_SOURCE_DIRS}/include
)

# copy compile_commands.json from build dir to ../compile_commands.json

add_custom_command(TARGET quix_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/build/compile_commands.json
)
