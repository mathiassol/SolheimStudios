#include "Frustum.h"

void Frustum::update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    glm::mat4 clip = projectionMatrix * viewMatrix;

    // Left plane
    m_planes[LEFT].normal.x = clip[0][3] + clip[0][0];
    m_planes[LEFT].normal.y = clip[1][3] + clip[1][0];
    m_planes[LEFT].normal.z = clip[2][3] + clip[2][0];
    m_planes[LEFT].distance = clip[3][3] + clip[3][0];

    // Right plane
    m_planes[RIGHT].normal.x = clip[0][3] - clip[0][0];
    m_planes[RIGHT].normal.y = clip[1][3] - clip[1][0];
    m_planes[RIGHT].normal.z = clip[2][3] - clip[2][0];
    m_planes[RIGHT].distance = clip[3][3] - clip[3][0];

    // Bottom plane
    m_planes[BOTTOM].normal.x = clip[0][3] + clip[0][1];
    m_planes[BOTTOM].normal.y = clip[1][3] + clip[1][1];
    m_planes[BOTTOM].normal.z = clip[2][3] + clip[2][1];
    m_planes[BOTTOM].distance = clip[3][3] + clip[3][1];

    // Top plane
    m_planes[TOP].normal.x = clip[0][3] - clip[0][1];
    m_planes[TOP].normal.y = clip[1][3] - clip[1][1];
    m_planes[TOP].normal.z = clip[2][3] - clip[2][1];
    m_planes[TOP].distance = clip[3][3] - clip[3][1];

    // Near plane
    m_planes[NEAR].normal.x = clip[0][3] + clip[0][2];
    m_planes[NEAR].normal.y = clip[1][3] + clip[1][2];
    m_planes[NEAR].normal.z = clip[2][3] + clip[2][2];
    m_planes[NEAR].distance = clip[3][3] + clip[3][2];

    // Far plane
    m_planes[FAR].normal.x = clip[0][3] - clip[0][2];
    m_planes[FAR].normal.y = clip[1][3] - clip[1][2];
    m_planes[FAR].normal.z = clip[2][3] - clip[2][2];
    m_planes[FAR].distance = clip[3][3] - clip[3][2];

    for (int i = 0; i < COUNT; i++) {
        float length = glm::length(m_planes[i].normal);
        m_planes[i].normal /= length;
        m_planes[i].distance /= length;
    }
}

bool Frustum::isBoxVisible(const Box* box) const {
    glm::vec3 center = box->position;
    glm::vec3 halfSize = box->size * 0.5f;

    for (int i = 0; i < COUNT; i++) {
        glm::vec3 positiveVertex = center;
        if (m_planes[i].normal.x >= 0) positiveVertex.x += halfSize.x;
        else positiveVertex.x -= halfSize.x;
        if (m_planes[i].normal.y >= 0) positiveVertex.y += halfSize.y;
        else positiveVertex.y -= halfSize.y;
        if (m_planes[i].normal.z >= 0) positiveVertex.z += halfSize.z;
        else positiveVertex.z -= halfSize.z;

        if (m_planes[i].distanceToPoint(positiveVertex) < 0) {
            return false;
        }
    }

    return true;
}

bool Frustum::isSphereVisible(const glm::vec3& center, float radius) const {
    for (int i = 0; i < COUNT; i++) {
        if (m_planes[i].distanceToPoint(center) < -radius) {
            return false;
        }
    }
    return true;
}