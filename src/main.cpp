#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "logger.h"
#include "window.h"

int main()
{
    const GLuint width = 800;
    const GLuint height = 600;

    Window window{width, height, "Ocean Simulation"};

    int version = gladLoadGL(glfwGetProcAddress);
    if(!version)
    {
        Logger::get().log("Failed to initialize OpenGL context", true);
        return -1;
    }
    Logger::get().log("Loaded OpenGL " + std::to_string(GLAD_VERSION_MAJOR(version)) + '.' + std::to_string(GLAD_VERSION_MINOR(version)));

    window.setViewport();

    while(window.isOpen())
    {
        glfwPollEvents();

        glClearColor(.1f, .2f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window.window());
    }

    return 0;
}