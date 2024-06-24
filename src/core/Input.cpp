#include "core/Input.h"

#include "core/Constants.h"

std::array<bool, GLFW_KEY_LAST> Input::m_keyboardState{};
std::array<bool, GLFW_KEY_LAST> Input::m_keyboardStateChanged{};

float Input::m_mouseX = 0.0f;
float Input::m_mouseY = 0.0f;

float Input::m_mouseLastX = static_cast<float>(constants::windowWidth) / 2.0f;
float Input::m_mouseLastY = static_cast<float>(constants::windowHeight) / 2.0f;

float Input::m_mouseXOffset = 0.0f;
float Input::m_mouseYOffset = 0.0f;

bool Input::m_cameraMode = false;
bool Input::m_firstMouse = true;

void Input::mosueCallback(GLFWwindow *window, double xPosIn, double yPosIn)
{
	if (m_cameraMode)
	{
		m_mouseX = static_cast<float>(xPosIn);
		m_mouseY = static_cast<float>(yPosIn);

		if (m_firstMouse)
		{
			m_mouseLastX = m_mouseX;
			m_mouseLastY = m_mouseY;
			m_firstMouse = false;
		}

		m_mouseXOffset = m_mouseX - m_mouseLastX;
		m_mouseYOffset = m_mouseLastY - m_mouseY;

		m_mouseLastX = m_mouseX;
		m_mouseLastY = m_mouseY;
	}
}

void Input::keyboardCallback(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	if (action != GLFW_RELEASE)
	{
		if (!m_keyboardState[key])
		{
			m_keyboardState[key] = true;
		}
	}
	else
	{
		m_keyboardState[key] = false;
	}
	m_keyboardStateChanged[key] = action != GLFW_REPEAT;
}

bool Input::key(int key) const
{
	return m_keyboardState[key];
}

bool Input::keyUp(int key)
{
	return m_keyboardState[key] && keyChanged(key);
}

bool Input::keyChanged(int key)
{
	bool ret = m_keyboardStateChanged[key];
	m_keyboardStateChanged[key] = false;
	return ret;
}

bool Input::keyDown(int key)
{
	return !m_keyboardState[key] && keyChanged(key);
}

float Input::mouseX() const
{
	return m_mouseX;
}

float Input::mouseY() const
{
	return m_mouseY;
}

float Input::mouseXOffset()
{
	float ret = m_mouseXOffset;
	m_mouseXOffset = 0.0f;
	return ret;
}

float Input::mouseYOffset()
{
	float ret = m_mouseYOffset;
	m_mouseYOffset = 0.0f;
	return ret;
}

bool Input::cameraMode() const
{
	return m_cameraMode;
}

void Input::changeCameraMode()
{
	m_cameraMode = !m_cameraMode;
}

void Input::changeFirstMouse()
{
	m_firstMouse = !m_firstMouse;
}
