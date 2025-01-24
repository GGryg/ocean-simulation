#ifndef WINDOW_H
#define WINDOW_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
	Window(int width, int height, std::string title);

	~Window();

	GLFWwindow *window() const;

	GLuint width() const;

	GLuint height() const;

	const std::string &title() const;

	void setViewport() const;

	bool isOpen() const;

	void close() const;

	void swapBuffers() const;

	void pollEvents() const;

private:
	void init();

	GLFWwindow *m_window = nullptr;
	GLuint m_width{};
	GLuint m_height{};
	std::string m_title;
};

#endif //WINDOW_H
