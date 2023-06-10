cmake_minimum_required(VERSION 3.24)

project(quix_vma)

find_package(Vulkan REQUIRED)

add_library(${PROJECT_NAME} STATIC quix_vma.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
