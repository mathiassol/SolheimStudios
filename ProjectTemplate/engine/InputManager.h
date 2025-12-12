#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>
#include <functional>

class InputManager {
public:
    void update(GLFWwindow* window);
    bool isKeyPressed(int key);

    void getMouseDelta(double& deltaX, double& deltaY);
    void setMouseCallback(GLFWwindow* window);

    void bindKey(const std::string& action, int key);
    bool isActionPressed(const std::string& action);

private:
    std::unordered_map<int, bool> m_keys;
    std::unordered_map<std::string, int> m_keyBindings;

    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    double m_mouseX = 0.0;
    double m_mouseY = 0.0;
    bool m_firstMouse = true;

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static InputManager* s_instance;
};