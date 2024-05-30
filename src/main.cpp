#include <constants.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cstddef>
#include <sstream>

#include <glad/gl.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TOML_EXCEPTIONS 0
#include <filesystem>
#include <toml++/toml.hpp>

#include "camera.h"
#include "logger.h"
#include "window.h"
#include "buffers.h"
#include "ocean.h"
#include "skybox.h"
#include "shaderException.h"

Camera camera(glm::vec3(0.0f, 16.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float lastX;
float lastY;
bool firstMouse{true};
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool cameraMode = false;
bool wireframeMode = false;

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
    if(glfwGetKey(window.window(), GLFW_KEY_R) == GLFW_PRESS)
    {
        wireframeMode = !wireframeMode;
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
    if(!std::filesystem::exists(constants::config)) [[unlikely]]
    {
        Logger::get().log("Config file doesn't exist", true);
        return 1;
    }
    const GLuint width = 1600;
    const GLuint height = 900;

    Window window{width, height, "Ocean Simulation"};
    glfwSetCursorPosCallback(window.window(), mosueCallback);
    lastX = window.width() / 2.0f;
    lastY = window.height() / 2.0f;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    toml::parse_result config = toml::parse_file(constants::config);
    if(!config)
    {
        std::stringstream ss;
        ss << "Parsing config failed: " << config.error();
        Logger::get().log(ss.str(), true);
        return 1;
    }

    SpectrumParams spectrumParams;
    spectrumParams.N = constants::N;
    if(config["spectrum"].is_table())
    {

        spectrumParams.amplitude = config.at_path("spectrum.amplitude").as_floating_point()->get();
        spectrumParams.length = config.at_path("spectrum.length").as_floating_point()->get();
        spectrumParams.choppinessScale = glm::vec2(
            config.at_path("spectrum.choppinessScale[0]").as_floating_point()->get(),
            config.at_path("spectrum.choppinessScale[1]").as_floating_point()->get());
        spectrumParams.displacementScale = config.at_path("spectrum.displacementScale").as_floating_point()->get();
        spectrumParams.suppresorFactor = config.at_path("spectrum.suppresorFactor").as_floating_point()->get();
        spectrumParams.windDirection = glm::vec2(config.at_path("spectrum.windDirection[0]").as_floating_point()->get(),
                                                 config.at_path(
                                                     "spectrum.windDirection[1]").as_floating_point()->get());
        spectrumParams.windSpeed = config.at_path("spectrum.windSpeed").as_floating_point()->get();
    }

    PBR material;
    if(config["pbr"].is_table())
    {
        material.roughness = config.at_path("pbr.roughness").as_floating_point()->get();
        material.bubbleColor = glm::vec3(config.at_path("pbr.bubbleColor[0]").as_floating_point()->get(),
                                         config.at_path("pbr.bubbleColor[1]").as_floating_point()->get(),
                                         config.at_path("pbr.bubbleColor[2]").as_floating_point()->get());
        material.bubbleDensity = config.at_path("pbr.bubbleDensity").as_floating_point()->get();
        material.heightWave = config.at_path("pbr.heightWave").as_floating_point()->get();
        material.scatterColor = glm::vec3(config.at_path("pbr.scatterColor[0]").as_floating_point()->get(),
                                          config.at_path("pbr.scatterColor[1]").as_floating_point()->get(),
                                          config.at_path("pbr.scatterColor[2]").as_floating_point()->get());
        material.scatterStrength = config.at_path("pbr.scatterStrength").as_floating_point()->get();
        material.sunDirection = glm::vec3(config.at_path("pbr.sunDirection[0]").as_floating_point()->get(),
                                          config.at_path("pbr.sunDirection[1]").as_floating_point()->get(),
                                          config.at_path("pbr.sunDirection[2]").as_floating_point()->get());
        material.sunIrradiance = glm::vec3(config.at_path("pbr.sunIrradiance[0]").as_floating_point()->get(),
                                           config.at_path("pbr.sunIrradiance[1]").as_floating_point()->get(),
                                           config.at_path("pbr.sunIrradiance[2]").as_floating_point()->get());
        material.envLightStrength = config.at_path("pbr.envLightStrength").as_floating_point()->get();
        material.scatterShadowStrength = config.at_path("pbr.scatterShadowStrength").as_floating_point()->get();
        material.wavePeakScatterStrength = config.at_path("pbr.wavePeakScatterStrength").as_floating_point()->get();
    }


    Ocean ocean{spectrumParams, material};
    try
    {
        ocean.prepareResources();
    }
    catch (const ShaderException& ex)
    {
        Logger::get().log(ex.what(), true);
        return -1;
    }
    Skybox skybox;

    int tiling = 1;
    bool visualize = false;
    int textureSelection = Ocean::HIILDE0K;
    const char* textureNames = "Phillips spectrum k\0Phillips spectrum -k\0tilde h dx\0tilde h dy\0tilde h dz\0dx\0dy\0dz\0Normal map\0";
    float timeSpeed = 1.0f;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.window(), true);
    ImGui_ImplOpenGL3_Init("#version 430");
    while(window.isOpen())
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        ocean.waving(deltaTime, timeSpeed);
        glClearColor(.4f, .8f, .9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.texture()->bindActive(4);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100000.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = camera.getViewMat();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{0.0f, 0.0f, 0.0f});
        //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        if(wireframeMode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        ocean.draw(deltaTime, camera.position(), projection, view, model, tiling, wireframeMode);

        if(wireframeMode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

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
                    if(ImGui::InputFloat("Amplitude", &spectrumParams.amplitude))
                    {
                        ocean.setAmplitude(spectrumParams.amplitude);
                    }
                    if(ImGui::InputFloat("Wind speed", &spectrumParams.windSpeed))
                    {
                        ocean.setWindSpeed(spectrumParams.windSpeed);
                    }
                    if(ImGui::SliderFloat2("Wind direction", glm::value_ptr(spectrumParams.windDirection), -1.0f, 1.0f))
                    {
                        ocean.setwindDirection(spectrumParams.windDirection);
                    }
                    if(ImGui::SliderFloat("Length", &spectrumParams.length, 100.0f, 3000.0f))
                    {
                        ocean.setLength(spectrumParams.length);
                    }
                    if(ImGui::SliderFloat("Supressor factor", &spectrumParams.suppresorFactor, 0.0f, 30.0f))
                    {
                        ocean.setSuppresorFactor(spectrumParams.suppresorFactor);
                    }
                    if(ImGui::InputFloat("Displacement", &spectrumParams.displacementScale))
                    {
                        ocean.setDisplacementScale(spectrumParams.displacementScale);
                    }
                    if(ImGui::InputFloat2("Choppiness", glm::value_ptr(spectrumParams.choppinessScale)))
                    {
                        ocean.setChoppinessScale(spectrumParams.choppinessScale);
                    }
                    ImGui::EndTabItem();
                }
            }
            if(ImGui::BeginTabItem("PBR"))
            {
                if(ImGui::SliderFloat("Roughness", &material.roughness, 0.01f, 1.0f))
                {
                    ocean.setRoughness(material.roughness);
                }
                if(ImGui::InputFloat3("Sun direction", glm::value_ptr(material.sunDirection)))
                {
                    ocean.setSunDirection(material.sunDirection);
                }
                if(ImGui::InputFloat3("Sun Irradiance", glm::value_ptr(material.sunIrradiance)))
                {
                    ocean.setSunIrradiance(material.sunIrradiance);
                }
                if(ImGui::InputFloat("Wave height", &material.heightWave))
                {
                    ocean.setHeightWave(material.heightWave);
                }
                if(ImGui::ColorEdit3("Scatter color", glm::value_ptr(material.scatterColor), ImGuiColorEditFlags_Float))
                {
                    ocean.setScatterColor(material.scatterColor);
                }
                if(ImGui::ColorEdit3("Bubble color", glm::value_ptr(material.bubbleColor), ImGuiColorEditFlags_Float))
                {
                    ocean.setBubbleColor(material.bubbleColor);
                }
                if(ImGui::InputFloat("Bubble density", &material.bubbleDensity))
                {
                    ocean.setBubbleDensity(material.bubbleDensity);
                }
                if(ImGui::InputFloat("Wave peak scatter strength", &material.wavePeakScatterStrength))
                {
                    ocean.setWavePeakScatterStrength(material.wavePeakScatterStrength);
                }
                if(ImGui::InputFloat("Scatter strength", &material.scatterStrength))
                {
                    ocean.setScatterStrength(material.scatterStrength);
                }
                if(ImGui::InputFloat("Scatter shadow strength", &material.scatterShadowStrength))
                {
                    ocean.setScatterShadowStrength(material.scatterShadowStrength);
                }
                if(ImGui::InputFloat("Enviroment Light Strength", &material.envLightStrength))
                {
                    ocean.setEnvLightStrength(material.envLightStrength);
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
            ImGui::Separator();
            ImGui::Text("Other options");
            ImGui::InputInt("Tiling", &tiling);
            ImGui::InputFloat("Speed", &timeSpeed);
            ImGui::Checkbox("Visualize textures",&visualize);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();

            if(visualize)
            {
                ImGui::Begin("Visualize texture");
                ImGui::Combo("Select texture", &textureSelection, textureNames);
                ImGui::Image(reinterpret_cast<void*>(ocean.texture(static_cast<Ocean::TextureVis>(textureSelection))), ImVec2(256, 256));
                ImGui::End();
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //break;//

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}