#include <GL/glew.h>
#include "FlyCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

FlyCamera::FlyCamera(InputManager* input)
    : m_input(input), m_position(0.0f, 0.0f, 3.0f), m_front(0.0f, 0.0f, -1.0f), m_up(0.0f, 1.0f, 0.0f), m_speed(0.05f) {}

void FlyCamera::update() {
    GLFWwindow* window = glfwGetCurrentContext();

    if (m_input->isKeyPressed(GLFW_KEY_W)) m_position += m_speed * m_front;
    if (m_input->isKeyPressed(GLFW_KEY_S)) m_position -= m_speed * m_front;
    if (m_input->isKeyPressed(GLFW_KEY_A)) m_position -= glm::normalize(glm::cross(m_front, m_up)) * m_speed;
    if (m_input->isKeyPressed(GLFW_KEY_D)) m_position += glm::normalize(glm::cross(m_front, m_up)) * m_speed;
}

glm::mat4 FlyCamera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}
