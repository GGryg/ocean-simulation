#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

namespace gfx
{
	// Implementation of camera using Euler's angles
	class Camera
	{
	public:
		enum class MoveDirection
		{
			front,
			back,
			right,
			left
		};

		Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

		glm::mat4 viewMat() const;

		void processMouse(float deltaTime, float xoffset, float yoffset);

		void processKeyboard(float deltaTime, MoveDirection direction);

		const glm::vec3 &position() const;

	private:
		void updateVectors();

		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldUp;
		float m_movementSpeed;
		float m_mouseSensitivity;

		// angles
		float m_yaw;
		float m_pitch;

	};
}

#endif //CAMERA_H
