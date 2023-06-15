cmake_minimum_required(VERSION 3.24.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

message("IPO enabled? ${CMAKE_INTERPROCEDURAL_OPTIMIZATION}")

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

add_subdirectory(quix_vma)
add_subdirectory(quix)


set(BUILD_QUIX_EXAMPLE ON CACHE BOOL "Build quix example")

if (BUILD_QUIX_EXAMPLE)
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
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_OPTIONS -fsanitize=address
        LINK_OPTIONS -fsanitize=address
    )

    # copy compile_commands.json from build dir to ../compile_commands.json

    add_custom_command(TARGET quix_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/build/compile_commands.json
    )
endif()


