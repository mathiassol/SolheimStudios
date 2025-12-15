#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Box.h"

struct Plane {
    glm::vec3 normal;
    float distance;

    Plane() : normal(0.0f), distance(0.0f) {}

    float distanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

class Frustum {
public:
    enum Side { LEFT = 0, RIGHT, BOTTOM, TOP, NEAR, FAR, COUNT };

    Frustum() = default;

    void update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
    bool isBoxVisible(const Box* box) const;
    bool isSphereVisible(const glm::vec3& center, float radius) const;

private:
    Plane m_planes[COUNT];
};