#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "Box.h"
#include "Frustum.h"

struct OcclusionQuery {
    GLuint queryID;
    Box* box;
    bool resultAvailable;
    GLuint samplesPassed;
    int frameDelay;
};

class OcclusionCulling {
public:
    OcclusionCulling();
    ~OcclusionCulling();

    // Initialize the occlusion culling system
    void initialize();

    // Begin occlusion query pass (renders bounding boxes to test visibility)
    void beginQueryPass();

    // Issue an occlusion query for a box
    void issueQuery(Box* box);

    // End occlusion query pass
    void endQueryPass();

    // Check if a box is visible based on previous queries
    bool isVisible(Box* box);

    // Update and retrieve query results
    void updateQueryResults();

    // Clear all queries
    void clear();

    // Enable/disable occlusion culling
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    // Get statistics
    int getQueriesIssued() const { return m_queriesIssued; }
    int getObjectsCulled() const { return m_objectsCulled; }

private:
    bool m_enabled;
    std::unordered_map<Box*, OcclusionQuery> m_queries;
    std::vector<Box*> m_queryQueue;
    int m_queriesIssued;
    int m_objectsCulled;
    int m_frameCount;

    // Draw a simple bounding box for occlusion testing
    void drawBoundingBox(Box* box);

    // Get or create a query for a box
    OcclusionQuery& getQuery(Box* box);
};