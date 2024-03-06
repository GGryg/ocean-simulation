#include "window.h"

#include "logger.h"

Window::Window(int width, int height, const std::string& title)
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

    m_window = glfwCreateWindow(width, height, title.data(), NULL, NULL);
    glfwMakeContextCurrent(m_window);
    if(!m_window)
    {
        Logger::get().log("Failed to create GLFW window");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    Logger::get().log("Window destroyed");
    glfwTerminate();
}

GLFWwindow* Window::window() const
{
    return m_window;
}

GLuint Window::width() const
{
    return m_width;
}

GLuint Window::height() const
{
    return m_height;
}

void Window::setViewport()
{
    glViewport(0, 0, m_width, m_height);
}

bool Window::isOpen() const
{
    return !glfwWindowShouldClose(m_window);
}

void Window::close()
{
    glfwSetWindowShouldClose(m_window, true);
}

void error_callback(int error, const char* description)
{
    Logger::get().log(description, true);
}