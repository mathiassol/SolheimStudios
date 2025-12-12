#include "InputManager.h"

void InputManager::update(GLFWwindow* window) {
    for (int key = 32; key < 350; ++key) {
        m_keys[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }
}

bool InputManager::isKeyPressed(int key) {
    return m_keys[key];
}
