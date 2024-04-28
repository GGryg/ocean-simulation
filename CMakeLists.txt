cmake_minimum_required(VERSION 3.27)

project(OceanSimulation)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
add_subdirectory(libs/glfw)

add_subdirectory(libs/glm)

set(STB_DIRS ${PROJECT_SOURCE_DIR}/libs/stb)

include_directories(SYSTEM "libs/glad/include")
include_directories(SYSTEM ${STB_DIRS})
include_directories(SYSTEM "include")

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

add_executable(${CMAKE_PROJECT_NAME}
                                    "src/main.cpp"
                                    "libs/glad/src/gl.c"
                                    "src/logger/logger.cpp"
                                    "src/window.cpp"
                                    "src/buffers.cpp"
                                    "src/resourceLoader.cpp"
                                    "src/shader.cpp"
                                    "src/ocean.cpp"
                                    "src/camera.cpp"
                                    "src/texture.cpp"
                                    "src/skybox.cpp"
                                    ${IMGUI_GLOB}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
                                    glfw
                                    glm
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            ${CMAKE_BINARY_DIR}/resources
)