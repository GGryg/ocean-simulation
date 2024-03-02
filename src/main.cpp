#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "logger.h"

int main()
{
    const GLuint width = 800;
    const GLuint height = 600;

    if(!glfwInit())
    {
        Logger::get().log("Failed to initialized GLFW", true);
        return -1;
    }
    Logger::get().log("Initialized GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Somke Simulation", NULL, NULL);
    glfwMakeContextCurrent(window);

    if(!window)
    {
        Logger::get().log("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    Logger::get().log("Created GLFW window");

    int version = gladLoadGL(glfwGetProcAddress);
    if(!version)
    {
        Logger::get().log("Failed to initialize OpenGL context", true);
        glfwTerminate();
        return -1;
    }
    Logger::get().log("Loaded OpenGL " + std::to_string(GLAD_VERSION_MAJOR(version)) + '.' + std::to_string(GLAD_VERSION_MINOR(version)));

    glViewport(0, 0, width, height);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(.1f, .2f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    
    return 0;
}