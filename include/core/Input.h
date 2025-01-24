#ifndef INPUT_H
#define INPUT_H

#include <array>

#include "GLFW/glfw3.h"

class Input
{
public:
	Input() = default;

	static void mouseCallback(GLFWwindow *window, double xPosIn, double yPosIn);

	static void keyboardCallback(GLFWwindow *window, int key, int scanCode, int action, int mods);

	bool key(int key) const;

	bool keyUp(int key);

	bool keyChanged(int key);

	bool keyDown(int key);

	float mouseX() const;

	float mouseY() const;

	float mouseXOffset();

	float mouseYOffset();

	bool cameraMode() const;

	void changeCameraMode();

	void changeFirstMouse();

private:
	static std::array<bool, GLFW_KEY_LAST> m_keyboardState;
	static std::array<bool, GLFW_KEY_LAST> m_keyboardStateChanged;
	static float m_mouseX;
	static float m_mouseY;
	static float m_mouseLastX;
	static float m_mouseLastY;
	static float m_mouseXOffset;
	static float m_mouseYOffset;
	static bool m_firstMouse;
	static bool m_cameraMode;
};

#endif //INPUT_H
