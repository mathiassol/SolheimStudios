#pragma once
#include <GL/glew.h>
#include "Box.h"
#include <GLFW/glfw3.h>

class Renderer {
public:
    Renderer(GLFWwindow* window);
    void drawBox(Box* box);
    GLFWwindow* getWindow() { return m_window; }

private:
    GLFWwindow* m_window;
};
