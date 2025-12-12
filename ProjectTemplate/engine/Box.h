#pragma once
#include <glm/glm.hpp>

struct Box {
    glm::vec3 position;
    glm::vec3 size;
    Box(glm::vec3 pos, glm::vec3 s) : position(pos), size(s) {}
};
