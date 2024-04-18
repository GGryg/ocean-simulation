#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position_t, glm::vec3 worldUp_t, float yaw_t, float pitch_t)
    : m_position{position_t}
    , m_front{glm::vec3{0.0f, 0.0f, -1.0f}} 
    , m_worldUp{worldUp_t}
    , yaw{yaw_t}
    , pitch{pitch_t}
    , movementSpeed{200.0f}
    , mouseSensitivity{0.025f}
{
    updateVectors();
}

glm::mat4 Camera::getViewMat() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processMouse(float xoffset, float yoffset)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    if(pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    updateVectors();
}

void Camera::processKeyboard(float deltaTime, CameraMoveDirection direction)
{
    float vel = movementSpeed * deltaTime;
    if(direction == CameraMoveDirection::front)
    {
        m_position += m_front * vel;
    }
    if(direction == CameraMoveDirection::back)
    {
        m_position -= m_front * vel;
    }
    if(direction == CameraMoveDirection::right)
    {
        m_position += m_right * vel;
    }
    if(direction == CameraMoveDirection::left)
    {
        m_position -= m_right * vel;
    }
}

float Camera::zoom() const
{
    return m_zoom;
}

const glm::vec3& Camera::position() const
{
    return m_position;
}

void Camera::updateVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}