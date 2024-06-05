#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"
#include "graphics/camera.h"
#include "Input.h"
#include "graphics/skybox.h"
#include "ocean.h"

#include "imgui.h"

class Application
{
public:
	Application(const std::string &windowTitle, const SpectrumParams &spectrumParams, const PBR &material,
	            const OtherOptions &otherOptions);

	~Application() = default;

	void run();

private:
	void init();

	void mainLoop();

	void drawUI();

	void processInput();

	Window m_window;
	gfx::Camera m_camera;
	Ocean m_ocean;
	gfx::Skybox m_skybox;
	Input m_input;
	float m_deltaTime = 0.0f;
	float m_lastFrame = 0.0f;
	bool m_wireframeMode = false;
	bool m_visualize = false;
	int m_textureSelection;
	const char *m_textureNames;
	SpectrumParams m_spectrumGui;
	PBR m_materialGui;
	OtherOptions m_otherGui;
	ImGuiIO *m_io;
};

#endif //APPLICATION_H
