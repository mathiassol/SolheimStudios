#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "Box.h"
#include "LODSystem.h"

struct InstanceData {
    glm::mat4 modelMatrix;
    int lodLevel;
};

class BatchRenderer {
public:
    BatchRenderer();
    ~BatchRenderer();

    void beginBatch();
    void addInstance(const Box* box, LODLevel lod, const glm::vec3& cameraPos);
    void endBatch();
    void flush();

private:
    std::vector<InstanceData> m_instances;
    GLuint m_vao, m_vbo, m_instanceVBO;
    int m_maxInstances;

    void setupBuffers();
    void drawBoxGeometry(LODLevel lod);
};