#include "window.h"

#include "logger.h"


void error_callback(int error, const char* description)
{
    Logger::get().log(description, true);
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{

    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    Logger::get().log("OPENGL ERROR: ", true);
    std::cout << "---------------" << '\n';
    std::cout << "Debug message (" << id << "): " <<  message << '\n';

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    }
    std::cout << '\n';

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } 
    std::cout << '\n';
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    }
    std::cout << '\n';
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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // TODO remove for release build

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

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
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