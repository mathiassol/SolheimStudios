#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>

class InputManager {
public:
    void update(GLFWwindow* window);
    bool isKeyPressed(int key);

private:
    std::unordered_map<int, bool> m_keys;
};
