#pragma once

#include "InputManager.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class FlyCamera {
public:
    FlyCamera(InputManager* input);
    void update();
    glm::mat4 getViewMatrix() const;

private:
    InputManager* m_input;
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    float m_speed;
};
