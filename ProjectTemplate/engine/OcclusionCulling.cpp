#include "OcclusionCulling.h"
#include <iostream>

OcclusionCulling::OcclusionCulling()
    : m_enabled(true), m_queriesIssued(0), m_objectsCulled(0), m_frameCount(0) {
}

OcclusionCulling::~OcclusionCulling() {
    clear();
}

void OcclusionCulling::initialize() {
    // OpenGL occlusion queries are created on-demand
    std::cout << "Occlusion culling initialized" << std::endl;
}

void OcclusionCulling::beginQueryPass() {
    if (!m_enabled) return;

    m_queriesIssued = 0;
    m_objectsCulled = 0;
    m_queryQueue.clear();
    m_frameCount++;

    // Disable color and depth writes for the query pass
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
}

void OcclusionCulling::issueQuery(Box* box) {
    if (!m_enabled) return;

    OcclusionQuery& query = getQuery(box);

    // Only issue new queries every few frames to reduce overhead
    // Or if we don't have a result yet
    if (query.frameDelay <= 0 || !query.resultAvailable) {
        // Begin the occlusion query
        glBeginQuery(GL_SAMPLES_PASSED, query.queryID);

        // Draw a simple bounding box
        drawBoundingBox(box);

        // End the query
        glEndQuery(GL_SAMPLES_PASSED);

        query.resultAvailable = false;
        query.frameDelay = 3; // Re-query every 3 frames
        m_queriesIssued++;
    } else {
        query.frameDelay--;
    }

    m_queryQueue.push_back(box);
}

void OcclusionCulling::endQueryPass() {
    if (!m_enabled) return;

    // Re-enable color and depth writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
}

bool OcclusionCulling::isVisible(Box* box) {
    if (!m_enabled) return true;

    auto it = m_queries.find(box);
    if (it == m_queries.end()) {
        // If no query exists yet, assume visible
        return true;
    }

    OcclusionQuery& query = it->second;

    // If we don't have results yet, assume visible to be safe
    if (!query.resultAvailable) {
        return true;
    }

    // Object is visible if any samples passed the depth test
    bool visible = query.samplesPassed > 0;

    if (!visible) {
        m_objectsCulled++;
    }

    return visible;
}

void OcclusionCulling::updateQueryResults() {
    if (!m_enabled) return;

    // Check for available query results
    for (auto& pair : m_queries) {
        OcclusionQuery& query = pair.second;

        if (!query.resultAvailable) {
            GLuint available = 0;
            glGetQueryObjectuiv(query.queryID, GL_QUERY_RESULT_AVAILABLE, &available);

            if (available) {
                glGetQueryObjectuiv(query.queryID, GL_QUERY_RESULT, &query.samplesPassed);
                query.resultAvailable = true;
            }
        }
    }
}

void OcclusionCulling::clear() {
    // Delete all query objects
    for (auto& pair : m_queries) {
        glDeleteQueries(1, &pair.second.queryID);
    }
    m_queries.clear();
    m_queryQueue.clear();
}

void OcclusionCulling::drawBoundingBox(Box* box) {
    float x = box->position.x;
    float y = box->position.y;
    float z = box->position.z;

    float hx = box->size.x / 2.0f;
    float hy = box->size.y / 2.0f;
    float hz = box->size.z / 2.0f;

    // Draw a simple box outline (just the vertices, minimal geometry)
    glBegin(GL_QUADS);

    // Front face
    glVertex3f(x - hx, y - hy, z + hz);
    glVertex3f(x + hx, y - hy, z + hz);
    glVertex3f(x + hx, y + hy, z + hz);
    glVertex3f(x - hx, y + hy, z + hz);

    // Back face
    glVertex3f(x - hx, y - hy, z - hz);
    glVertex3f(x - hx, y + hy, z - hz);
    glVertex3f(x + hx, y + hy, z - hz);
    glVertex3f(x + hx, y - hy, z - hz);

    // Left face
    glVertex3f(x - hx, y - hy, z - hz);
    glVertex3f(x - hx, y - hy, z + hz);
    glVertex3f(x - hx, y + hy, z + hz);
    glVertex3f(x - hx, y + hy, z - hz);

    // Right face
    glVertex3f(x + hx, y - hy, z - hz);
    glVertex3f(x + hx, y + hy, z - hz);
    glVertex3f(x + hx, y + hy, z + hz);
    glVertex3f(x + hx, y - hy, z + hz);

    // Top face
    glVertex3f(x - hx, y + hy, z - hz);
    glVertex3f(x - hx, y + hy, z + hz);
    glVertex3f(x + hx, y + hy, z + hz);
    glVertex3f(x + hx, y + hy, z - hz);

    // Bottom face
    glVertex3f(x - hx, y - hy, z - hz);
    glVertex3f(x + hx, y - hy, z - hz);
    glVertex3f(x + hx, y - hy, z + hz);
    glVertex3f(x - hx, y - hy, z + hz);

    glEnd();
}

OcclusionQuery& OcclusionCulling::getQuery(Box* box) {
    auto it = m_queries.find(box);
    if (it == m_queries.end()) {
        // Create a new query
        OcclusionQuery query;
        glGenQueries(1, &query.queryID);
        query.box = box;
        query.resultAvailable = false;
        query.samplesPassed = 0;
        query.frameDelay = 0;

        m_queries[box] = query;
        return m_queries[box];
    }
    return it->second;
}