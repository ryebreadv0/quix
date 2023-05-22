cmake_minimum_required(VERSION 3.22.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

project(quix)

add_library(${PROJECT_NAME} 
    quix/quix_device.cpp
    quix/quix_logger.cpp
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


target_link_libraries(${PROJECT_NAME} 
    spdlog::spdlog
    Vulkan::Vulkan
    glfw
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