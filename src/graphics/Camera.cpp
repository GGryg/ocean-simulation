#include "graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace gfx
{
	Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
		: m_position{position}
		, m_front{glm::vec3{0.0f, 0.0f, -1.0f}}
		, m_up(0.0f)
		, m_right(0.0f)
		, m_worldUp{worldUp}
		, m_yaw{yaw}
		, m_pitch{pitch}
		, m_movementSpeed{200.0f}
		, m_mouseSensitivity{0.5f}
	{
		updateVectors();
	}

	glm::mat4 Camera::viewMat() const
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void Camera::processMouse(float deltaTime, float xoffset, float yoffset)
	{
		xoffset *= m_mouseSensitivity * deltaTime;
		yoffset *= m_mouseSensitivity * deltaTime;
		m_yaw += xoffset;
		m_pitch += yoffset;

		if (m_pitch > 89.0f)
		{
			m_pitch = 89.0f;
		}
		if (m_pitch < -89.0f)
		{
			m_pitch = -89.0f;
		}

		updateVectors();
	}

	void Camera::processKeyboard(float deltaTime, MoveDirection direction)
	{
		const float vel = m_movementSpeed * deltaTime;
		using enum MoveDirection;
		if (direction == front)
		{
			m_position += m_front * vel;
		}
		if (direction == back)
		{
			m_position -= m_front * vel;
		}
		if (direction == right)
		{
			m_position += m_right * vel;
		}
		if (direction == left)
		{
			m_position -= m_right * vel;
		}
	}

	const glm::vec3 &Camera::position() const
	{
		return m_position;
	}

	void Camera::updateVectors()
	{
		glm::vec3 front;
		front.x = static_cast<float>(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
		front.y = static_cast<float>(sin(glm::radians(m_pitch)));
		front.z = static_cast<float>(sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
		m_front = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_front, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}
}
