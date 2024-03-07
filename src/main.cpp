#include <iostream>
#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "logger.h"
#include "resourceManager.h"
#include "window.h"
#include "buffers.h"

int main()
{
    const GLuint width = 800;
    const GLuint height = 600;

    Window window{width, height, "Ocean Simulation"};

    std::vector<GLfloat> positions = {
        // positions            colors
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
    };

    VArray vao;
    VBuffer vbo{positions.data(), positions.size() * sizeof(GLfloat)};
    VBufferLayout layout;
    layout.addElement<GLfloat>(3);
    layout.addElement<GLfloat>(3);
    vao.addBuffer(vbo, layout);
    
    Shader shader = ResourceManager::get().loadShader("basic", "shaders/triangle/vert.vs", "shaders/triangle/frag.fs");
    
    while(window.isOpen())
    {

        glClearColor(.1f, .2f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vao.bind();
        shader.use();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}