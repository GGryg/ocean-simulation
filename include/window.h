#ifndef WINDOW_H
#define WINDOW_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
    Window(int width, int heigth, const std::string& title);
    ~Window();

    GLFWwindow* window() const;
    GLuint width() const;
    GLuint height() const;
    void setViewport();
    bool isOpen() const;
    void close();
    
private:
    GLFWwindow* m_window = nullptr;
    GLuint m_width{};
    GLuint m_height{};
};

#endif