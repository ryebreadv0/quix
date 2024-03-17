cmake_minimum_required(VERSION 3.24.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

include(cmake/CPM.cmake)

CPMAddPackage("gh:nothings/stb#master") 
# CPMAddPackage("gh:gabime/spdlog@1.11.0")

project(p C CXX)

find_package(stb REQUIRED)
find_package(Vulkan REQUIRED)

find_package(Threads REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(Vulkan REQUIRED)


add_subdirectory(quix_impl)
add_subdirectory(quix)

set(BUILD_QUIX_EXAMPLE ON CACHE BOOL "Build quix example")

if (BUILD_QUIX_EXAMPLE)
    CPMAddPackage("gh:g-truc/glm#0.9.9.8")

    project(quix_example)

    add_executable(quix_example
        examples/main.cpp
    )

    target_link_libraries(quix_example
        quix
    )

    target_include_directories(quix_example 
        PUBLIC ${PROJECT_SOURCE_DIR}/quix
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PUBLIC ${glm_SOURCE_DIR}
    )

    # set_target_properties(${PROJECT_NAME} PROPERTIES
    #     COMPILE_OPTIONS -fsanitize=address
    #     LINK_OPTIONS -fsanitize=address
    # )

endif()
