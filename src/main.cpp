#include <iostream>
#include <memory>
#include <vector>
#include <cstddef>

#include <glad/gl.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

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
#include "skybox.h"

Camera camera(glm::vec3(0.0f, 16.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float lastX;
float lastY;
bool firstMouse{true};
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool cameraMode = false;

void processInput(Window& window)
{
    if(glfwGetKey(window.window(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        window.close();
    }
    if(glfwGetKey(window.window(), GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.processKeyboard(deltaTime, CameraMoveDirection::front);
    }
    if(glfwGetKey(window.window(), GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.processKeyboard(deltaTime, CameraMoveDirection::back);
    }
    if(glfwGetKey(window.window(), GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.processKeyboard(deltaTime, CameraMoveDirection::right);
    }
    if(glfwGetKey(window.window(), GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.processKeyboard(deltaTime, CameraMoveDirection::left);
    }
    if(glfwGetKey(window.window(), GLFW_KEY_F) == GLFW_PRESS)
    {
        cameraMode = true;
        glfwSetInputMode(window.window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if(glfwGetKey(window.window(), GLFW_KEY_G) == GLFW_PRESS)
    {
        cameraMode = false;
        firstMouse = true;
        glfwSetInputMode(window.window(), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
    }
}

void mosueCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
    if(cameraMode)
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
}

int main()
{
    const GLuint width = 1280;
    const GLuint height = 720;

    Window window{width, height, "Ocean Simulation"};
    glfwSetCursorPosCallback(window.window(), mosueCallback);
    lastX = window.width() / 2.0f;
    lastY = window.height() / 2.0f;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    std::unique_ptr<Shader> shader = ResourceLoader::get().loadShader("shaders/ocean_vs.glsl", "shaders/ocean_fs.glsl");
    if(!shader->isValid())
    {
        return 1;
    }
    shader->use();

    constexpr int N = 256; // MUST BE A POWER OF 2
    float amplitute = 5.0f;
    float windSpeed = 500000.0f;
    glm::vec2 windDirection{500.0f, 1000.0f};
    float length = 1000;
    float l = 1;

    Ocean ocean{N, amplitute, windSpeed, windDirection, length, l};//
    Skybox skybox;

    std::unique_ptr<Shader> tShader = ResourceLoader::get().loadShader("shaders/testTex.vs", "shaders/testTex.fs");

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
    tShader->use();
    VArray va;
    VBuffer vb;
    va.bind();
    vb.addData(v, sizeof(v), GL_STATIC_DRAW);
    VBufferLayout e;
    e.emplace_back(3, GL_FLOAT, GL_FALSE, 0);
    e.emplace_back(2, GL_FLOAT, GL_FALSE, 3*sizeof(float));
    va.addBuffer(&vb, 5 * sizeof(float), e);
    EBuffer eb = EBuffer{in, std::size(in), GL_STATIC_DRAW};

    tShader->use();
    tShader->setInt("texture1", 0);

    glm::vec3 lightPosition{1.2f, 100.0f, 2.0f};

    glm::vec3 sunDirection(-1.29f, -1.0f, 4.86f);
    glm::vec3 sunIrradiance(1.0f, 0.694f, 0.32f);
    glm::vec3 scatterColor(0.016000003f, 0.07359998f, 0.16f);
    glm::vec3 bubbleColor(0.0f, 0.02f, 0.015999999f);
    float bubbleDensity = 1.0f;
    float wavePeakScatterStrength = 1.0f;
    float scatterStrength = 1.0f;
    float scatterShadowStrength = 0.5f;
    float heightWave = 1.0f;
    float roughness = 0.08f;
    float envLightStrength = 1.0f;
    float choppiness = 19.7f;
    float displacement = 15.5f;

    int tiling = 1;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.window(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while(window.isOpen())
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        ocean.waving(deltaTime);
        glClearColor(.4f, .8f, .9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();
        //vao.bind();
        ocean.draw(deltaTime, glm::vec3(1.0), glm::vec3(1.0), glm::mat4(1.0), glm::mat4(1.0), glm::mat4(1.0));
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100000.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = camera.getViewMat();
        shader->setMat4("u_proj", projection);
        shader->setMat4("u_view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{0.0f, 0.0f, 0.0f});
        //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader->setMat4("u_model", model);
        shader->setInt("u_dx", 0);
        ocean.m_dx->bindActive(0);
        shader->setInt("u_dy", 1);
        ocean.m_dy->bindActive(1);
        shader->setInt("u_dz", 2);
        ocean.m_dz->bindActive(2);
        shader->setInt("u_normalMap", 3);
        ocean.m_normalMap->bindActive(3);
        shader->setInt("skybox", 4);
        skybox.texture()->bindActive(4);
        shader->setFloat("u_displacement", displacement);
        shader->setFloat("u_choppiness", choppiness);
        shader->setVec3("u_cameraPosition", camera.position());

        // lightining/shadows
        shader->setVec3("u_viewPosition", camera.position());

        shader->setFloat("u_roughness", roughness);

        shader->setVec3("u_sunDirection", sunDirection);
        shader->setVec3("u_sunIrradiance", sunIrradiance);
        shader->setVec3("u_scatterColor", scatterColor);
        shader->setVec3("u_bubbleColor", bubbleColor);
        shader->setFloat("u_bubbleDensity", bubbleDensity);
        shader->setFloat("u_wavePeakScatterStrength", wavePeakScatterStrength);
        shader->setFloat("u_scatterStrength", scatterStrength);
        shader->setFloat("u_scatterShadowStrength", scatterShadowStrength);
        shader->setFloat("u_waveHeight", heightWave);
        shader->setFloat("u_envLightStrength", envLightStrength);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for(int j = 0; j < tiling; ++j)
        {
            for(int i = 0; i < tiling; ++i)
            {
                model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
                model = glm::translate(model, glm::vec3(1024 * i, 0, 1024 * -j));
                shader->setMat4("u_model", model);
                glDrawElements(GL_TRIANGLES, ocean.m_ebo->count(), GL_UNSIGNED_INT, nullptr);
            }
        }
        //
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        //tShader->use();
        //testTex->bind(0);
        //tilde_h0k->bindActive(0);
        //ocean.m_twiddleFactors->bindActive(0);
        //ocean.m_twiddleFactors->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_dy->bindActive(0);
        //ocean.m_dy->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_pingPong->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_tilde_hkt_dy->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_normalMap->bindImage(0, 0, 0, GL_READ_ONLY);
        //ocean.m_tilde_h0k->bindImage(0, 0, 0, GL_READ_ONLY);
        //va.bind();
        //glDrawElements(GL_TRIANGLES, eb.count(), GL_UNSIGNED_INT, nullptr);
        //break;dfddfdf

        view = glm::mat4(glm::mat3(camera.getViewMat()));
        skybox.draw(view, projection);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Ocean simulaton!");
            ImGui::Text("Press to F to disable cursor and move camera");

            if(ImGui::BeginTabBar("Parameters", ImGuiTableFlags_None))
            {
                if(ImGui::BeginTabItem("Spectrum"))
                {
                    if(ImGui::InputFloat("Amplitude", &amplitute))
                    {
                        ocean.setAmplitude(amplitute);
                    }
                    if(ImGui::InputFloat("Wind speed", &windSpeed))
                    {
                        ocean.setWindSpeed(windSpeed);
                    }
                    if(ImGui::SliderFloat2("Wind direction", glm::value_ptr(windDirection), 0.0f, 1.0f))
                    {
                        ocean.setwindDirection(windDirection);
                    }
                    if(ImGui::SliderFloat("Length", &length, 100.0f, 1000.0f))
                    {
                        ocean.setLength(length);
                    }
                    if(ImGui::SliderFloat("Supressor factor", &l, 0.0f, 50.0f))
                    {
                        ocean.setL(l);
                    }
                    ImGui::InputFloat("Displacement", &displacement);
                    ImGui::InputFloat("Choppiness", &choppiness);
                    ImGui::Image((void*)ocean.m_dy->id(), ImVec2(256, 256));
                    ImGui::Image((void*)ocean.m_tilde_hkt_dy->id(), ImVec2(256, 256));
                    ImGui::Image((void*)ocean.m_tilde_h0k->id(), ImVec2(256, 256));//
                    ImGui::EndTabItem();
                }
            }
            if(ImGui::BeginTabItem("PBR"))
            {
                ImGui::SliderFloat("Roughness", &roughness, 0.01f, 1.0f);
                ImGui::InputFloat3("Sun direction", glm::value_ptr(sunDirection));
                ImGui::InputFloat3("Sun Irradiance", glm::value_ptr(sunIrradiance));
                ImGui::InputFloat("Wave height", &heightWave);
                ImGui::ColorEdit3("Scatter color", glm::value_ptr(scatterColor), ImGuiColorEditFlags_Float);
                ImGui::ColorEdit3("Bubble color", glm::value_ptr(bubbleColor), ImGuiColorEditFlags_Float);
                ImGui::InputFloat("Bubble density", &bubbleDensity);
                ImGui::InputFloat("Wave peak scatter strength", &wavePeakScatterStrength);
                ImGui::InputFloat("Scatter strength", &scatterStrength);
                ImGui::InputFloat("Scatter shadow strength", &scatterShadowStrength);
                ImGui::InputFloat("Enviroment Light Strength", &envLightStrength);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
            ImGui::Separator();
            ImGui::InputInt("Tiling", &tiling);


            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);

            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}