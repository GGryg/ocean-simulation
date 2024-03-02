#include "window.h"

#include "logger.h"

Window::Window(int width, int height)
{
    if(!glfwInit())
    {
        Logger::get().log("Failed to initialized GLFW", true);
        std::exit(EXIT_FAILURE);
    }
    Logger::get().log("Initialized GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, "Smoke Simulation", NULL, NULL);
    if(!m_window)
    {
        Logger::get().log("Failed to create GLFW window");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
}