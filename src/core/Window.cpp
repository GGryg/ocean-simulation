#include "core/Window.h"

#include <sstream>

#include "logger/Logger.h"


void error_callback(int error, const char *description)
{
	Logger::get().log(description, true);
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                            const char *message, const void *userParam)
{
	std::stringstream ss;
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
	ss << "OPENGL ERROR:\n";
	ss << "---------------\n";
	ss << "Debug message (" << id << "): " << message << '\n';

	switch (source)
	{
		case GL_DEBUG_SOURCE_API: ss << "Source: API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: ss << "Source: Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: ss << "Source: Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: ss << "Source: Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION: ss << "Source: Application";
			break;
		case GL_DEBUG_SOURCE_OTHER: ss << "Source: Other";
			break;
		default: break;
	}
	ss << '\n';

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR: ss << "Type: Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ss << "Type: Deprecated Behaviour";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: ss << "Type: Undefined Behaviour";
			break;
		case GL_DEBUG_TYPE_PORTABILITY: ss << "Type: Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE: ss << "Type: Performance";
			break;
		case GL_DEBUG_TYPE_MARKER: ss << "Type: Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP: ss << "Type: Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP: ss << "Type: Pop Group";
			break;
		case GL_DEBUG_TYPE_OTHER: ss << "Type: Other";
			break;
		default: break;
	}
	ss << '\n';

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH: ss << "Severity: high";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM: ss << "Severity: medium";
			break;
		case GL_DEBUG_SEVERITY_LOW: ss << "Severity: low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: ss << "Severity: notification";
			break;
		default: break;
	}
	ss << '\n';

	Logger::get().log(ss.str(), true);
}

Window::Window(int width, int height, std::string title)
	: m_width(width)
	, m_height(height)
	, m_title(std::move(title))
{
	init();
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Window::init()
{
	if (!glfwInit())
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

	m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
	// NOLINT(*-narrowing-conversions)
	if (!m_window)
	{
		Logger::get().log("Failed to create GLFW window");
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(m_window);

	int version = gladLoadGL(glfwGetProcAddress);
	if (!version)
	{
		Logger::get().log("Failed to initialize OpenGL context", true);
		glfwDestroyWindow(m_window);
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	Logger::get().log(
		"Loaded OpenGL " + std::to_string(GLAD_VERSION_MAJOR(version)) + '.' + std::to_string(
			GLAD_VERSION_MINOR(version)));

	setViewport();

	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}

GLFWwindow *Window::window() const
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

const std::string &Window::title() const
{
	return m_title;
}

void Window::setViewport() const
{
	glViewport(0, 0, m_width, m_height); // NOLINT(*-narrowing-conversions)
}

bool Window::isOpen() const
{
	return !glfwWindowShouldClose(m_window);
}

void Window::close() const
{
	glfwSetWindowShouldClose(m_window, true);
}

void Window::swapBuffers() const
{
	glfwSwapBuffers(m_window);
}

void Window::pollEvents() const // NOLINT(*-convert-member-functions-to-static)
{
	glfwPollEvents();
}
