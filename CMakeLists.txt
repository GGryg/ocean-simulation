cmake_minimum_required(VERSION 3.27)

project(OceanSimulation)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# config file
configure_file(${PROJECT_SOURCE_DIR}/config.toml ${CMAKE_BINARY_DIR}/config.toml COPYONLY)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
add_subdirectory(libs/glfw)

# GLM
add_subdirectory(libs/glm)

# STB IMAGE
set(STB_DIRS ${PROJECT_SOURCE_DIR}/libs/stb)

# GLAD
include_directories(SYSTEM "libs/glad/include")
include_directories(SYSTEM ${STB_DIRS})
include_directories(SYSTEM "include")

# Dear ImGui
set(IMGUI_SOURCES
    "libs/imgui/imconfig.h"
    "libs/imgui/imgui_internal.h"
    "libs/imgui/imgui_draw.cpp"
    "libs/imgui/imgui_tables.cpp"
    "libs/imgui/imgui_widgets.cpp"
    "libs/imgui/imgui.cpp"
    "libs/imgui/imgui.h"
    "libs/imgui/imstb_rectpack.h"
    "libs/imgui/imstb_textedit.h"
    "libs/imgui/imstb_truetype.h"
    "libs/imgui/backends/imgui_impl_glfw.h"
    "libs/imgui/backends/imgui_impl_glfw.cpp"
    "libs/imgui/backends/imgui_impl_opengl3.h"
    "libs/imgui/backends/imgui_impl_opengl3.cpp"
)

set(IMGUI_PATH ${PROJECT_SOURCE_DIR}/libs/imgui)
file(GLOB IMGUI_GLOB
    ${IMGUI_PATH}/imgui.h
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imconfig.h
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_internal.h
    ${IMGUI_PATH}/imstb_rectpack.h
    ${IMGUI_PATH}/imstb_textedit.h
    ${IMGUI_PATH}/imstb_truetype.h
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp

    # specific bindings...
    ${IMGUI_PATH}/backends/imgui_impl_glfw.h
    ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.cpp)

include_directories(SYSTEM ${IMGUI_PATH})

# Toml++
add_subdirectory(libs/tomlplusplus)

# additional includes for the libraries to work
include_directories(${PROJECT_SOURCE_DIR}/libs/include)

# GLFFT
set(GLFFT_PATH ${PROJECT_SOURCE_DIR}/libs/glfft)
file(GLOB GLFFT_GLOB
        ${GLFFT_PATH}/*.cpp
)
add_library(GLFFT ${GLFFT_GLOB})
include_directories(${PROJECT_SOURCE_DIR}/libs/GLFFT)
add_custom_command(
        OUTPUT libs/GLFFT/glsl
        WORKING_DIR libs/GLFFT/glsl
        COMMAND make
)

add_executable(${CMAKE_PROJECT_NAME}
                                    "src/main.cpp"
                                    "libs/glad/src/gl.c"
                                    "src/logger/logger.cpp"
                                    "src/core/Window.cpp"
                                    "src/core/ResourceLoader.cpp"
                                    "src/graphics/Shader.cpp"
                                    "src/graphics/VArray.cpp"
                                    "src/graphics/VBuffer.cpp"
                                    "src/graphics/EBuffer.cpp"
                                    "src/core/Ocean.cpp"
                                    "src/graphics/Camera.cpp"
                                    "src/graphics/Texture.cpp"
                                    "src/graphics/Skybox.cpp"
                                    "src/core/Application.cpp"
                                    "src/core/Input.cpp"
                                    ${IMGUI_GLOB}
                                    ${GLFFT_GLOB}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
                                    glfw
                                    glm
                                    tomlplusplus::tomlplusplus
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            ${CMAKE_SOURCE_DIR}/shaders
            ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            ${CMAKE_SOURCE_DIR}/resources
            ${CMAKE_BINARY_DIR}/resources
)