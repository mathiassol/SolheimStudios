#pragma once

#include "../core/InputManager.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class FlyCamera {
public:
    FlyCamera(InputManager* input);
    void update();
    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getFront() const { return m_front; }
    glm::vec3 getUp() const { return m_up; }
    glm::vec3 getRight() const { return m_right; }

    void setPosition(const glm::vec3& pos) { m_position = pos; }
    void setSpeed(float speed) { m_speed = speed; }
    void setSensitivity(float sensitivity) { m_sensitivity = sensitivity; }

    float getYaw() const { return m_yaw; }
    float getPitch() const { return m_pitch; }

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