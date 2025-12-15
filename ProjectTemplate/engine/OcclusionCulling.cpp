#include "OcclusionCulling.h"
#include <iostream>

OcclusionCulling::OcclusionCulling()
    : m_enabled(true), m_queriesIssued(0), m_objectsCulled(0), m_frameCount(0) {
}

OcclusionCulling::~OcclusionCulling() {
    clear();
}

void OcclusionCulling::initialize() {
    std::cout << "Occlusion culling initialized" << std::endl;
}

void OcclusionCulling::beginQueryPass() {
    if (!m_enabled) return;

    m_queriesIssued = 0;
    m_objectsCulled = 0;
    m_frameCount++;

    // Disable color writes but keep depth test enabled
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    // Ensure depth test is ON so queries work properly
    glEnable(GL_DEPTH_TEST);
}

void OcclusionCulling::issueQuery(Box* box) {
    if (!m_enabled) return;

    OcclusionQuery& query = getQuery(box);

    // Issue queries every frame for accurate results
    // Or space them out for performance (every N frames)
    if (query.frameDelay <= 0) {
        glBeginQuery(GL_SAMPLES_PASSED, query.queryID);
        drawBoundingBox(box);
        glEndQuery(GL_SAMPLES_PASSED);

        query.resultAvailable = false;
        query.frameDelay = 1; // Query every frame for accuracy
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
        // First frame, assume visible
        return true;
    }

    OcclusionQuery& query = it->second;

    // Check if result is available
    if (!query.resultAvailable) {
        GLuint available = 0;
        glGetQueryObjectuiv(query.queryID, GL_QUERY_RESULT_AVAILABLE, &available);

        if (available) {
            glGetQueryObjectuiv(query.queryID, GL_QUERY_RESULT, &query.samplesPassed);
            query.resultAvailable = true;
        } else {
            // Result not ready, assume visible to be safe
            return true;
        }
    }

    // Object is visible if any samples passed
    bool visible = query.samplesPassed > 0;

    if (!visible) {
        m_objectsCulled++;
    }

    return visible;
}

void OcclusionCulling::updateQueryResults() {
    if (!m_enabled) return;

    // Update all pending queries
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
    for (auto& pair : m_queries) {
        glDeleteQueries(1, &pair.second.queryID);
    }
    m_queries.clear();
    m_queryQueue.clear();
}

void OcclusionCulling::drawBoundingBox(Box* box) {
    glPushMatrix();
    glTranslatef(box->position.x, box->position.y, box->position.z);

    float hx = box->size.x / 2.0f;
    float hy = box->size.y / 2.0f;
    float hz = box->size.z / 2.0f;

    glBegin(GL_QUADS);

    // Front face
    glVertex3f(-hx, -hy, hz);
    glVertex3f(hx, -hy, hz);
    glVertex3f(hx, hy, hz);
    glVertex3f(-hx, hy, hz);

    // Back face
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx, hy, -hz);
    glVertex3f(hx, hy, -hz);
    glVertex3f(hx, -hy, -hz);

    // Left face
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx, -hy, hz);
    glVertex3f(-hx, hy, hz);
    glVertex3f(-hx, hy, -hz);

    // Right face
    glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, hy, -hz);
    glVertex3f(hx, hy, hz);
    glVertex3f(hx, -hy, hz);

    // Top face
    glVertex3f(-hx, hy, -hz);
    glVertex3f(-hx, hy, hz);
    glVertex3f(hx, hy, hz);
    glVertex3f(hx, hy, -hz);

    // Bottom face
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, -hy, hz);
    glVertex3f(-hx, -hy, hz);

    glEnd();
    glPopMatrix();
}

OcclusionQuery& OcclusionCulling::getQuery(Box* box) {
    auto it = m_queries.find(box);
    if (it == m_queries.end()) {
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