#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

int main()
{
    const GLuint width = 800;
    const GLuint height = 600;

    if(!glfwInit())
    {
        std::cerr << "ERROR: Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Somke Simulation", NULL, NULL);
    glfwMakeContextCurrent(window);

    if(!window)
    {
        std::cerr << "ERROR: Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    int version = gladLoadGL(glfwGetProcAddress);
    if(!version)
    {
        std::cerr << "ERROR: Failed to initialize OpenGL context\n";
        glfwTerminate();
        return -1;
    }

    std::cout<<"Loaded OpenGL " << GLAD_VERSION_MAJOR(version)<< '.' << GLAD_VERSION_MINOR(version) << '\n';

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