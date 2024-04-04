#include <iostream>
#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "logger.h"
#include "resourceManager.h"
#include "window.h"
#include "buffers.h"

void processInput(Window& window)
{
    if(glfwGetKey(window.window(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        window.close();
    }
}

int main()
{
    const GLuint width = 800;
    const GLuint height = 600;

    Window window{width, height, "Ocean Simulation"};

    std::vector<GLfloat> positions = {
        // positions            colors
         0.5f, -0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // bottom left
        -0.5f,  0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // upp left
        -0.5f,  0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // upp left
         0.5f,  0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // upp right
         0.5f, -0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // bottom right
    };

    glEnable(GL_DEPTH);

    VArray vao;
    VBuffer vbo{positions.data(), positions.size() * sizeof(GLfloat)};
    VBufferLayout layout;
    layout.addElement<GLfloat>(3);
    layout.addElement<GLfloat>(3);
    vao.addBuffer(vbo, layout);
    
    Shader shader = ResourceManager::get().loadShader("basic", "shaders/triangle/vert.vs", "shaders/triangle/frag.fs");

    constexpr int N = 64; // MUST BE A POWER OF 2
    constexpr float amplitute = 0.00005f;
    constexpr float windSpeed = 32.0f;
    constexpr float length = 64;




    while(window.isOpen())
    {

        glClearColor(.1f, .2f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vao.bind();
        shader.use();
        glDrawArrays(GL_TRIANGLES, 0, 6);


        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}