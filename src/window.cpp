#include "window.h"

#include "logger.h"


void error_callback(int error, const char* description)
{
    Logger::get().log(description, true);
}

Window::Window(int width, int height, const std::string& title)
    : m_width(width)
    , m_height(height)
    , m_title(title)
{
    init();
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    Logger::get().log("Window destroyed");
    glfwTerminate();
}

void Window::init()
{
    if(!glfwInit())
    {
        Logger::get().log("Failed to initialized GLFW", true);
        std::exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);

    Logger::get().log("Initialized GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
    if(!m_window)
    {
        Logger::get().log("Failed to create GLFW window");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);

    

    int version = gladLoadGL(glfwGetProcAddress);
    if(!version)
    {
        Logger::get().log("Failed to initialize OpenGL context", true);
        glfwDestroyWindow(m_window);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    Logger::get().log("Loaded OpenGL " + std::to_string(GLAD_VERSION_MAJOR(version)) + '.' + std::to_string(GLAD_VERSION_MINOR(version)));

    setViewport();
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

const std::string& Window::title() const
{
    return m_title;
}

void Window::setViewport()
{
    glViewport(0, 0, m_width, m_height);
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << glGetString(error) << '\n';
    }
}

bool Window::isOpen() const
{
    return !glfwWindowShouldClose(m_window);
}

void Window::close()
{
    glfwSetWindowShouldClose(m_window, true);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}