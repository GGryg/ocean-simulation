#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

class Window
{
public:
    Window(int width, int heigth);
    ~Window();
private:

    GLFWwindow* m_window = nullptr;
    GLuint m_width{};
    GLuint m_height{};
};

#endif