#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Box.h"
#include "Frustum.h"
#include "LODSystem.h"
#include "Octree.h"
#include "BatchRenderer.h"
#include "OcclusionCulling.h"
#include "FlyCamera.h"

struct CullingStats {
    int totalEntities = 0;
    int frustumCulled = 0;
    int occlusionCulled = 0;
    int rendered = 0;

    void reset() {
        totalEntities = 0;
        frustumCulled = 0;
        occlusionCulled = 0;
        rendered = 0;
    }
};

class Scene {
public:
    Scene(const std::string& name);
    ~Scene();

    // Entity management
    void addEntity(Box* box);
    Box* createRect(const glm::vec3& position, const glm::vec3& size);
    Box* createRect(const glm::vec3& position, float size = 1.0f);
    void clear();

    void enableFrustumCulling(bool enable) { m_useFrustumCulling = enable; m_overrideFrustumCulling = true; }
    void enableBatchRendering(bool enable) { m_useBatchRendering = enable; m_overrideBatchRendering = true; }
    void enableOctree(bool enable) { m_useOctree = enable; m_overrideOctree = true; }
    void enableOcclusionCulling(bool enable) { m_useOcclusionCulling = enable; m_overrideOcclusionCulling = true; }
    void setLODSettings(const LODSettings& settings) { m_lodSystem.setSettings(settings); }

    // Rendering
    void update(FlyCamera* camera, const glm::mat4& projectionMatrix);
    void render(class Renderer* renderer, FlyCamera* camera);

    // Getters
    const std::string& getName() const { return m_name; }
    size_t getEntityCount() const { return m_boxes.size(); }
    const CullingStats& getCullingStats() const { return m_stats; }

    // Internal settings access (used by Application)
    void inheritSettings(bool frustumCulling, bool batchRendering, bool octree, bool occlusionCulling);
    bool usesFrustumCulling() const { return m_useFrustumCulling; }
    bool usesBatchRendering() const { return m_useBatchRendering; }
    bool usesOctree() const { return m_useOctree; }
    bool usesOcclusionCulling() const { return m_useOcclusionCulling; }

    void rebuildOctree();

private:
    std::string m_name;
    std::vector<Box*> m_boxes;
    std::vector<Box*> m_ownedBoxes; // Boxes created by this scene

    Frustum m_frustum;
    LODSystem m_lodSystem;
    Octree* m_octree;
    BatchRenderer* m_batchRenderer;
    OcclusionCulling* m_occlusionCulling;

    CullingStats m_stats;

    // Settings and overrides
    bool m_useFrustumCulling;
    bool m_useBatchRendering;
    bool m_useOctree;
    bool m_useOcclusionCulling;

    bool m_overrideFrustumCulling;
    bool m_overrideBatchRendering;
    bool m_overrideOctree;
    bool m_overrideOcclusionCulling;

    void updateFrustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
    void renderScene(Renderer* renderer, FlyCamera* camera);
};