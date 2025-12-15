#include "BatchRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

BatchRenderer::BatchRenderer() : m_maxInstances(1000) {
    setupBuffers();
}

BatchRenderer::~BatchRenderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_instanceVBO);
}

void BatchRenderer::setupBuffers() {
    // In a real implementation, you'd set up VBOs and instance buffers here
}

void BatchRenderer::beginBatch() {
    m_instances.clear();
}

void BatchRenderer::addInstance(const Box* box, LODLevel lod, const glm::vec3& cameraPos) {
    if (lod == LODLevel::CULLED) return;

    InstanceData data;
    data.modelMatrix = glm::translate(glm::mat4(1.0f), box->position);
    data.modelMatrix = glm::scale(data.modelMatrix, box->size);
    data.lodLevel = static_cast<int>(lod);

    m_instances.push_back(data);
}

void BatchRenderer::endBatch() {
    flush();
}

void BatchRenderer::flush() {
    std::vector<InstanceData> highLOD, mediumLOD, lowLOD;

    for (const auto& instance : m_instances) {
        switch (static_cast<LODLevel>(instance.lodLevel)) {
            case LODLevel::HIGH:   highLOD.push_back(instance); break;
            case LODLevel::MEDIUM: mediumLOD.push_back(instance); break;
            case LODLevel::LOW:    lowLOD.push_back(instance); break;
            default: break;
        }
    }

    auto renderGroup = [this](const std::vector<InstanceData>& group, LODLevel lod) {
        for (const auto& instance : group) {
            glPushMatrix();
            glMultMatrixf(&instance.modelMatrix[0][0]);
            drawBoxGeometry(lod);
            glPopMatrix();
        }
    };

    renderGroup(highLOD, LODLevel::HIGH);
    renderGroup(mediumLOD, LODLevel::MEDIUM);
    renderGroup(lowLOD, LODLevel::LOW);
}

void BatchRenderer::drawBoxGeometry(LODLevel lod) {
    int subdivisions = 1;
    switch (lod) {
        case LODLevel::HIGH:   subdivisions = 4; break;
        case LODLevel::MEDIUM: subdivisions = 2; break;
        case LODLevel::LOW:    subdivisions = 1; break;
        default: return;
    }

    float h = 0.5f;

    glBegin(GL_QUADS);

    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-h, -h, h);
    glVertex3f(h, -h, h);
    glVertex3f(h, h, h);
    glVertex3f(-h, h, h);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-h, -h, -h);
    glVertex3f(-h, h, -h);
    glVertex3f(h, h, -h);
    glVertex3f(h, -h, -h);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-h, -h, -h);
    glVertex3f(-h, -h, h);
    glVertex3f(-h, h, h);
    glVertex3f(-h, h, -h);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(h, -h, -h);
    glVertex3f(h, h, -h);
    glVertex3f(h, h, h);
    glVertex3f(h, -h, h);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-h, h, -h);
    glVertex3f(-h, h, h);
    glVertex3f(h, h, h);
    glVertex3f(h, h, -h);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-h, -h, -h);
    glVertex3f(h, -h, -h);
    glVertex3f(h, -h, h);
    glVertex3f(-h, -h, h);

    glEnd();
}