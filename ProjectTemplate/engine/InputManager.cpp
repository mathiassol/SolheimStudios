#include "InputManager.h"

InputManager* InputManager::s_instance = nullptr;

void InputManager::update(GLFWwindow* window) {
    for (int key = 0; key < GLFW_KEY_LAST; ++key) {
        m_keys[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }
}

bool InputManager::isKeyPressed(int key) {
    return m_keys[key];
}

void InputManager::bindKey(const std::string& action, int key) {
    m_keyBindings[action] = key;
}

bool InputManager::isActionPressed(const std::string& action) {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        bool pressed = isKeyPressed(it->second);
        return pressed;
    }
    printf("Action '%s' not bound!\n", action.c_str());
    return false;
}

void InputManager::setMouseCallback(GLFWwindow* window) {
    s_instance = this;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
}

void InputManager::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (s_instance) {
        if (s_instance->m_firstMouse) {
            s_instance->m_lastMouseX = xpos;
            s_instance->m_lastMouseY = ypos;
            s_instance->m_firstMouse = false;
        }

        s_instance->m_mouseX = xpos;
        s_instance->m_mouseY = ypos;
    }
}

void InputManager::getMouseDelta(double& deltaX, double& deltaY) {
    deltaX = m_mouseX - m_lastMouseX;
    deltaY = m_lastMouseY - m_mouseY;

    m_lastMouseX = m_mouseX;
    m_lastMouseY = m_mouseY;
}