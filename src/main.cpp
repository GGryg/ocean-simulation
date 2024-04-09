#include <iostream>
#include <vector>
#include <cstddef>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "logger.h"
#include "resourceManager.h"
#include "window.h"
#include "buffers.h"
#include "ocean.h"

void processInput(Window& window)
{
    if(glfwGetKey(window.window(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        window.close();
    }
}

Camera camera(glm::vec3(0.0f, 12.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float lastX;
float lastY;
bool firstMouse{true};
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void mosueCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

    if(firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.processMouse(xOffset, yOffset);
}

int main()
{
    const GLuint width = 1280;
    const GLuint height = 720;

    Window window{width, height, "Ocean Simulation"};
    glfwSetCursorPosCallback(window.window(), mosueCallback);
    lastX = window.width() / 2.0f;
    lastY = window.height() / 2.0f;

    /*
    std::vector<GLfloat> positions = {
        // positions            colors
         0.5f, -0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // bottom left
        -0.5f,  0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // upp left
        -0.5f,  0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // upp left
         0.5f,  0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // upp right
         0.5f, -0.5f, 0.0f,     0.3f, 0.6f, 0.9f, // bottom right
    };
    */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    
    Shader shader = ResourceManager::get().loadShader("ocean", "shaders/ocean.vs", "shaders/ocean.fs");
    shader.use();
    
    VArray vao;
    VBuffer vbo;
    /*
    VBuffer vbo{positions.data(), positions.size() * sizeof(GLfloat), GL_STATIC_DRAW};
    VBufferLayout layout;
    layout.addElement<GLfloat>(3);
    layout.addElement<GLfloat>(3);
    vao.addBuffer(vbo, layout);
    
    Shader shader = ResourceManager::get().loadShader("basic", "shaders/triangle/vert.vs", "shaders/triangle/frag.fs");
    
    vbo.unbind();
    vao.unbind();
    */
    constexpr int N = 64; // MUST BE A POWER OF 2
    constexpr float amplitute = 0.00005f;
    constexpr float windSpeed = 32.0f;
    constexpr float length = 64;

    Ocean ocean{N, amplitute, windSpeed, glm::vec2{1.0f, 1.0f}, length};



    vao.bind();
    vbo.addData(ocean.m_vertices.data(), ocean.m_vertices.size() * sizeof(OceanVertex), GL_STATIC_DRAW);
    //VBufferLayout layout;
    //layout.addElement<GLfloat>(3);
    //layout.addElement<GLfloat>(2);
    //m_vao.bind();
    //vbo.bind();
    VBufferLayout elements;
    elements.emplace_back(3, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, vertexPosition));
    elements.emplace_back(2, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, texCoord));
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (void*)0);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (void*)sizeof(glm::vec3));
    vao.addBuffer(vbo, sizeof(OceanVertex), elements);
    EBuffer ebo = EBuffer{ocean.m_indices.data(), ocean.m_indices.size(), GL_STATIC_DRAW};



    while(window.isOpen())
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(.1f, .2f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //vao.bind();
        //shader.use();
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        shader.use();

        vao.bind();

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = camera.getViewMat();
        shader.setMat4("proj", projection);
        shader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{0.0f, -2.0f, 0.0f});
        shader.setMat4("model", model);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, ebo.count(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}