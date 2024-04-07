cmake_minimum_required(VERSION 3.29)

project(OceanSimulation)
set(CMAKE_CXX_STANDARD 20)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
add_subdirectory(libs/glfw)

add_subdirectory(libs/glm)

include_directories(SYSTEM "libs/glad/include")
include_directories(SYSTEM "include")

add_executable(${CMAKE_PROJECT_NAME}
                                    "src/main.cpp"
                                    "libs/glad/src/gl.c"
                                    "src/logger/logger.cpp"
                                    "src/window.cpp"
                                    "src/buffers.cpp"
                                    "src/resourceManager.cpp"
                                    "src/shader.cpp"
                                    "src/ocean.cpp"
                                    "src/fft.cpp"
                                    "src/camera.cpp"
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