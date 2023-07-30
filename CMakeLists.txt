cmake_minimum_required(VERSION 3.24.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

message("IPO enabled? ${CMAKE_INTERPROCEDURAL_OPTIMIZATION}")

include(cmake/CPM.cmake)

CPMAddPackage("gh:nothings/stb#master")
CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage(
	NAME glfw
	GITHUB_REPOSITORY glfw/glfw
	GIT_TAG 3.3.8
	VERSION 3.3.8
	DOWNLOAD_ONLY True
)

if (UNIX)
	execute_process(
		COMMAND /usr/bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/iswayland.sh
		RESULT_VARIABLE WAYLAND_CHECK
		OUTPUT_QUIET
		ERROR_QUIET
		)
	message("Wayland check result ${WAYLAND_CHECK}")
	if (WAYLAND_CHECK EQUAL 1)
		message("Wayland session found, enabling glfw option for wayland")
		option(GLFW_USE_WAYLAND "Use Wayland for window creation" ON)
	endif()
endif()

add_subdirectory(${glfw_SOURCE_DIR})

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

    add_custom_command(TARGET quix_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()


