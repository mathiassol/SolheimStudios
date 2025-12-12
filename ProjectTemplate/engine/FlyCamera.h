#pragma once

#include "InputManager.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class FlyCamera {
public:
    FlyCamera(InputManager* input);
    void update();
    glm::mat4 getViewMatrix() const;

    void setPosition(const glm::vec3& pos) { m_position = pos; }
    void setSpeed(float speed) { m_speed = speed; }
    void setSensitivity(float sensitivity) { m_sensitivity = sensitivity; }

private:
    InputManager* m_input;
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;
    float m_speed;
    float m_sensitivity;

    void updateCameraVectors();
};