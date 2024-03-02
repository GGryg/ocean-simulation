cmake_minimum_required(VERSION 3.29)

project(SomkeSimulation)
set(CMAKE_CXX_STANDARD 20)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
add_subdirectory(libs/glfw)

include_directories(SYSTEM "libs/glad/include")
include_directories(SYSTEM "include")

add_executable(${CMAKE_PROJECT_NAME}
                                    "src/main.cpp"
                                    "libs/glad/src/gl.c"
                                    "src/logger/logger.cpp"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
                                    glfw
)