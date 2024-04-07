#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>


// Implementation of camera using Euler's angles
class Camera
{
public:
    Camera(glm::vec3 position_t, glm::vec3 up_t, float yaw_t, float pitch_t);

    glm::mat4 getViewMat() const;

    void processMouse(float xoffset, float yoffset);

    float zoom() const;
private:
    void updateVectors();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float m_zoom;
};

#endif