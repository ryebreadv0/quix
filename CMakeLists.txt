cmake_minimum_required(VERSION 3.24.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

message("IPO enabled? ${CMAKE_INTERPROCEDURAL_OPTIMIZATION}")

project(quix)

add_library(${PROJECT_NAME} 
    quix/quix_instance.cpp
    quix/quix_window.cpp
    quix/quix_device.cpp
    quix/quix_logger.cpp
    quix/quix_swapchain.cpp
    quix/quix_shader.cpp
    quix/quix_pipeline.cpp
    quix/quix_descriptor.cpp
    quix/quix_render_target.cpp
    quix/quix_command_list.cpp
    quix/quix_resource.cpp
)

target_precompile_headers(${PROJECT_NAME} 
    PUBLIC quix/quix_pch.hpp
)

include(cmake/CPM.cmake)

CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage("gh:glfw/glfw#3.3.8")

CPMAddPackage(
    NAME glslang
    GITHUB_REPOSITORY KhronosGroup/glslang
    GIT_TAG 12.2.0
    VERSION 12.2.0
    DOWNLOAD_ONLY True
    OPTION BUILD_TESTING OFF
    OPTION ENABLE_CTEST OFF
)

# execute python program to get glslang externals

find_package(Python3 COMPONENTS Interpreter REQUIRED)

execute_process(
    COMMAND ${Python3_EXECUTABLE} ${glslang_SOURCE_DIR}/update_glslang_sources.py
    WORKING_DIRECTORY ${glslang_SOURCE_DIR}
)

add_subdirectory(${glslang_SOURCE_DIR} ${glslang_BINARY_DIR})



find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw REQUIRED)
find_package(glslang REQUIRED)

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
    glslang
    glslang-default-resource-limits
    OGLCompiler
    OSDependent
    HLSL
    SPIRV
    SPIRV-Tools-opt
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR}/quix
    PUBLIC ${PROJECT_SOURCE_DIR}/include
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
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${spdlog_SOURCE_DIRS}/include
)

# copy compile_commands.json from build dir to ../compile_commands.json

add_custom_command(TARGET quix_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/build/compile_commands.json
)


