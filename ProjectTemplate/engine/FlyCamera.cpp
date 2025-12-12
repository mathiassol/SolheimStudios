#include <GL/glew.h>
#include "FlyCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cstdio>

FlyCamera::FlyCamera(InputManager* input)
    : m_input(input),
      m_position(0.0f, 0.0f, 3.0f),
      m_worldUp(0.0f, 1.0f, 0.0f),
      m_yaw(-90.0f),
      m_pitch(0.0f),
      m_speed(0.05f),
      m_sensitivity(0.1f)
{
    updateCameraVectors();
}

void FlyCamera::update() {
    double deltaX, deltaY;
    m_input->getMouseDelta(deltaX, deltaY);

    m_yaw += deltaX * m_sensitivity;
    m_pitch += deltaY * m_sensitivity;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateCameraVectors();

    bool moved = false;
    if (m_input->isActionPressed("forward")) {
        m_position += m_speed * m_front;
        moved = true;
    }
    if (m_input->isActionPressed("backward")) {
        m_position -= m_speed * m_front;
        moved = true;
    }
    if (m_input->isActionPressed("left")) {
        m_position -= m_right * m_speed;
        moved = true;
    }
    if (m_input->isActionPressed("right")) {
        m_position += m_right * m_speed;
        moved = true;
    }
    if (m_input->isActionPressed("up")) {
        m_position += m_up * m_speed;
        moved = true;
    }
    if (m_input->isActionPressed("down")) {
        m_position -= m_up * m_speed;
        moved = true;
    }
}

void FlyCamera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

glm::mat4 FlyCamera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}