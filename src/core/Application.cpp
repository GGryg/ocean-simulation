#include "core/Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "core/Constants.h"
#include "logger/Logger.h"
#include "graphics/Skybox.h"


Application::Application(const std::string &windowTitle, const SpectrumParams &spectrumParams, const PBR &material,
                         const OtherOptions &otherOptions)
	: m_window(constants::windowWidth, constants::windowHeight, windowTitle)
	, m_camera(glm::vec3(0.0f, 16.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f)
	, m_ocean(spectrumParams, material, otherOptions)
	, m_skybox()
	, m_textureSelection(static_cast<int>(Ocean::TextureVis::hildeh0k))
	, m_textureNames(
		"Phillips spectrum k\0Phillips spectrum -k\0tilde h dx\0tilde h dy\0tilde h dz\0dx\0dy\0dz\0Normal map\0")
	, m_spectrumGui(spectrumParams)
	, m_materialGui(material)
	, m_otherGui(otherOptions)
	, m_io(nullptr)
{
}

void Application::init()
{
	glfwSetCursorPosCallback(m_window.window(), Input::mouseCallback);
	glfwSetKeyCallback(m_window.window(), Input::keyboardCallback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = &ImGui::GetIO();;
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window.window(), true);
	ImGui_ImplOpenGL3_Init("#version 430");

	m_ocean.prepareResources();
}

void Application::run()
{
	init();
	mainLoop();
}

void Application::mainLoop()
{
	while (m_window.isOpen())
	{
		auto currentFrame = static_cast<float>(glfwGetTime());
		m_deltaTime = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;

		processInput();
		m_ocean.waving();

		glClearColor(.4f, .8f, .9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_skybox.texture()->bindActive(4);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		                                        static_cast<float>(constants::windowWidth) / static_cast<float>(
			                                        constants::windowHeight), 0.1f, 100000.0f);
		glm::mat4 view = m_camera.viewMat();

		if (m_wireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		m_ocean.draw(m_camera.position(), projection, view, m_wireframeMode);

		if (m_wireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		view = glm::mat4(glm::mat3(m_camera.viewMat()));
		m_skybox.draw(view, projection);

		drawUI();

		m_window.swapBuffers();
		m_window.pollEvents();
	}
}

void Application::processInput()
{
	if (m_input.key(GLFW_KEY_ESCAPE))
	{
		m_window.close();
	}
	if (m_input.key(GLFW_KEY_W))
	{
		m_camera.processKeyboard(m_deltaTime, gfx::Camera::MoveDirection::front);
	}
	if (m_input.key(GLFW_KEY_S))
	{
		m_camera.processKeyboard(m_deltaTime, gfx::Camera::MoveDirection::back);
	}
	if (m_input.key(GLFW_KEY_D))
	{
		m_camera.processKeyboard(m_deltaTime, gfx::Camera::MoveDirection::right);
	}
	if (m_input.key(GLFW_KEY_A))
	{
		m_camera.processKeyboard(m_deltaTime, gfx::Camera::MoveDirection::left);
	}
	if (m_input.keyDown(GLFW_KEY_F))
	{
		m_input.changeCameraMode();
		if (m_input.cameraMode())
		{
			glfwSetInputMode(m_window.window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			m_input.changeFirstMouse();
			glfwSetInputMode(m_window.window(), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
		}
	}
	if (m_input.keyDown(GLFW_KEY_R))
	{
		m_wireframeMode = !m_wireframeMode;
	}

	m_camera.processMouse(m_deltaTime, m_input.mouseXOffset(), m_input.mouseYOffset());
}

void Application::drawUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame(); {
		ImGui::Begin("Ocean simulation!", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Press to F to catch/uncatch cursor");

		if (ImGui::BeginTabBar("Parameters", ImGuiTableFlags_None))
		{
			if (ImGui::BeginTabItem("Spectrum"))
			{
				if (ImGui::SliderFloat("Amplitude", &m_spectrumGui.amplitude, constants::minAmplitude,
				                       constants::maxAmplitude))
				{
					m_ocean.setAmplitude(m_spectrumGui.amplitude);
				}
				if (ImGui::SliderFloat("Wind speed", &m_spectrumGui.windSpeed, constants::minWindSpeed,
				                       constants::maxWindSpeed))
				{
					m_ocean.setWindSpeed(m_spectrumGui.windSpeed);
				}
				if (ImGui::SliderFloat2("Wind direction", glm::value_ptr(m_spectrumGui.windDirection),
				                        constants::minWindDirection, constants::maxWindDirection))
				{
					m_ocean.setWindDirection(m_spectrumGui.windDirection);
				}
				if (ImGui::SliderFloat("Length", &m_spectrumGui.length, constants::minLength, constants::maxLength))
				{
					m_ocean.setLength(m_spectrumGui.length);
				}
				if (ImGui::SliderFloat("Suppressor factor", &m_spectrumGui.suppressorFactor,
				                       constants::minSuppressorFactor, constants::maxSuppressorFactor))
				{
					m_ocean.setSuppressorFactor(m_spectrumGui.suppressorFactor);
				}
				if (ImGui::SliderFloat("Displacement scale", &m_spectrumGui.displacementScale,
				                       constants::minDisplacementScale, constants::maxDisplacementScale))
				{
					m_ocean.setDisplacementScale(m_spectrumGui.displacementScale);
				}
				if (ImGui::SliderFloat2("Choppiness scale", glm::value_ptr(m_spectrumGui.choppinessScale),
				                        constants::minChoppinessScale, constants::maxChoppinessScale))
				{
					m_ocean.setChoppinessScale(m_spectrumGui.choppinessScale);
				}
				ImGui::EndTabItem();
			}
		}
		if (ImGui::BeginTabItem("PBR"))
		{
			if (ImGui::SliderFloat("Roughness", &m_materialGui.roughness, constants::minRoughness,
			                       constants::maxRoughness))
			{
				m_ocean.setRoughness(m_materialGui.roughness);
			}
			if (ImGui::InputFloat3("Sun direction", glm::value_ptr(m_materialGui.sunDirection)))
			{
				m_ocean.setSunDirection(m_materialGui.sunDirection);
			}
			if (ImGui::ColorEdit3("Sun color", glm::value_ptr(m_materialGui.sunColor), ImGuiColorEditFlags_Float))
			{
				m_ocean.setSunColor(m_materialGui.sunColor);
			}
			if (ImGui::SliderFloat("Wave height", &m_materialGui.waveHeight, constants::minWaveHeight,
			                       constants::maxWaveHeight))
			{
				m_ocean.setWaveHeight(m_materialGui.waveHeight);
			}
			if (ImGui::ColorEdit3("Scatter color", glm::value_ptr(m_materialGui.scatterColor),
			                      ImGuiColorEditFlags_Float))
			{
				m_ocean.setScatterColor(m_materialGui.scatterColor);
			}
			if (ImGui::ColorEdit3("Bubble color", glm::value_ptr(m_materialGui.bubbleColor), ImGuiColorEditFlags_Float))
			{
				m_ocean.setBubbleColor(m_materialGui.bubbleColor);
			}
			if (ImGui::SliderFloat("Bubble density", &m_materialGui.bubbleDensity, constants::minBubbleDensity,
			                       constants::maxBubbleDensity))
			{
				m_ocean.setBubbleDensity(m_materialGui.bubbleDensity);
			}
			if (ImGui::SliderFloat("Wave peak scatter strength", &m_materialGui.wavePeakScatterStrength,
			                       constants::minWavePeakScatterStrength, constants::maxWavePeakScatterStrength))
			{
				m_ocean.setWavePeakScatterStrength(m_materialGui.wavePeakScatterStrength);
			}
			if (ImGui::SliderFloat("Scatter strength", &m_materialGui.scatterStrength, constants::minScatterStrength,
			                       constants::maxScatterStrength))
			{
				m_ocean.setScatterStrength(m_materialGui.scatterStrength);
			}
			if (ImGui::SliderFloat("Scatter shadow strength", &m_materialGui.scatterShadowStrength,
			                       constants::minScatterShadowStrength, constants::maxScatterShadowStrength))
			{
				m_ocean.setScatterShadowStrength(m_materialGui.scatterShadowStrength);
			}
			if (ImGui::SliderFloat("Environment Light Strength", &m_materialGui.envLightStrength,
			                       constants::minEnvironmentLightStrength, constants::maxEnvironmentLightStrength))
			{
				m_ocean.setEnvLightStrength(m_materialGui.envLightStrength);
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
		ImGui::Separator();
		ImGui::Text("Other options");
		if (ImGui::InputInt("Tiling", &m_otherGui.tiling))
		{
			m_ocean.setTiling(m_otherGui.tiling);
		}
		if (ImGui::SliderFloat("Speed", &m_otherGui.speed, constants::minSpeed, constants::maxSpeed))
		{
			m_ocean.setSpeed(m_otherGui.speed);
		}
		ImGui::Checkbox("Visualize textures", &m_visualize);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_io->Framerate, m_io->Framerate);
		ImGui::End();

		if (m_visualize)
		{
			ImGui::Begin("Visualize texture", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Combo("Select texture", &m_textureSelection, m_textureNames);
			ImGui::Image(reinterpret_cast<void *>(m_ocean.texture(static_cast<Ocean::TextureVis>(m_textureSelection))),
			             ImVec2(256, 256));
			ImGui::End();
		}
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
