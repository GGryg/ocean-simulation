#include <iostream>
#include <memory>
#include <vector>
#include <cstddef>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "logger.h"
#include "resourceLoader.h"
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

struct Uniform
{
    glm::mat4 view_proj;
};

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
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glDisable(GL_BLEND);

    
    Shader shader = ResourceLoader::get().loadShader("shaders/ocean_vs.glsl", "shaders/ocean_fs.glsl");
    shader.use();
    
    //VArray vao;
    //VBuffer vbo;
    
    //VBuffer vbo{positions.data(), positions.size() * sizeof(GLfloat), GL_STATIC_DRAW};
    //VBufferLayout layout;
    //layout.addElement<GLfloat>(3);
    //layout.addElement<GLfloat>(3);
    //vao.addBuffer(vbo, layout);
    
    //Shader shader = ResourceManager::get().loadShader("basic", "shaders/triangle/vert.vs", "shaders/triangle/frag.fs");
    
    //vbo.unbind();
    //vao.unbind();
    
    constexpr int N = 256; // MUST BE A POWER OF 2
    constexpr float amplitute = 5.0f;
    constexpr float windSpeed = 32.0f;
    glm::vec2 windDirection{1.0f, 1.0f};
    constexpr float length = 1024;

    Ocean ocean{N, amplitute, windSpeed, windDirection, length};

    //vao.bind();
    //vbo.addData(ocean.m_vertices.data(), ocean.m_vertices.size() * sizeof(OceanVertex), GL_STATIC_DRAW);
    //VBufferLayout layout;
    //layout.addElement<GLfloat>(3);
    //layout.addElement<GLfloat>(2);
    //m_vao.bind();
    //vbo.bind();
    //VBufferLayout elements;
    //elements.emplace_back(3, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, vertexPosition));
    //elements.emplace_back(2, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, texCoord));
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (void*)0);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (void*)sizeof(glm::vec3));
    //vao.addBuffer(vbo, sizeof(OceanVertex), elements);
    //EBuffer ebo = EBuffer{ocean.m_indices.data(), ocean.m_indices.size(), GL_STATIC_DRAW};

    
    Shader tShader = ResourceLoader::get().loadShader("shaders/testTex.vs", "shaders/testTex.fs");

    float v[] = {
         0.35f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.35f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.35f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.35f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
    };

    GLuint in[] = {
        0, 1, 3,
        1, 2, 3
    };
    tShader.use();
    VArray va;
    VBuffer vb;
    va.bind();
    vb.addData(v, sizeof(v), GL_STATIC_DRAW);
    VBufferLayout e;
    e.emplace_back(3, GL_FLOAT, GL_FALSE, 0);
    e.emplace_back(2, GL_FLOAT, GL_FALSE, 3*sizeof(float));
    va.addBuffer(&vb, 5 * sizeof(float), e);
    EBuffer eb = EBuffer{in, std::size(in), GL_STATIC_DRAW};

    std::unique_ptr<UBuffer> ubo = std::make_unique<UBuffer>(sizeof(Uniform), GL_DYNAMIC_DRAW);

    tShader.use();
    tShader.setInt("texture1", 0);

    glm::vec3 lightPosition{1.2f, 2.0f, 2.0f};

    while(window.isOpen())
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        ocean.waving(deltaTime);
        glClearColor(.1f, .2f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //vao.bind();
        //shader.use();
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        /*
        shader.use();
        //vao.bind();
        ocean.draw(deltaTime, glm::vec3(1.0), glm::vec3(1.0), glm::mat4(1.0), glm::mat4(1.0), glm::mat4(1.0));
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = camera.getViewMat();
        shader.setMat4("u_proj", projection);
        shader.setMat4("u_view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{0.0f, -2.0f, 0.0f});
        shader.setMat4("u_model", model);
        shader.setInt("u_dx", 0);
        ocean.m_dx->bindActive(0);
        shader.setInt("u_dy", 1);
        ocean.m_dy->bindActive(1);
        shader.setInt("u_dz", 2);
        ocean.m_dz->bindActive(2);
        shader.setFloat("u_displacement", 0.5f);
        shader.setFloat("u_choppiness", 0.7f);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, ocean.m_ebo->count(), GL_UNSIGNED_INT, nullptr);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        */
        tShader.use();
        //testTex->bind(0);
        //tilde_h0k->bindActive(0);
        //ocean.m_twiddleFactors->bindActive(0);
        //ocean.m_twiddleFactors->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_dy->bindActive(0);
        //ocean.m_dz->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_pingPong->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_tilde_hkt_dx->bindImage(0, 0, 0, GL_READ_ONLY);
        ocean.m_normalMap->bindImage(0, 0, 0, GL_READ_ONLY);
        va.bind();
        glDrawElements(GL_TRIANGLES, eb.count(), GL_UNSIGNED_INT, nullptr);
        //break;
        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}