#include "Scene.h"
#include "Renderer.h"
#include <iostream>

Scene::Scene(const std::string& name)
    : m_name(name),
      m_useFrustumCulling(true),
      m_useBatchRendering(true),
      m_useOctree(true),
      m_useOcclusionCulling(false),
      m_overrideFrustumCulling(false),
      m_overrideBatchRendering(false),
      m_overrideOctree(false),
      m_overrideOcclusionCulling(false)
{
    m_octree = new Octree(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, 5, 8);
    m_batchRenderer = new BatchRenderer();
    m_occlusionCulling = new OcclusionCulling();
    m_occlusionCulling->initialize();
}

Scene::~Scene() {
    delete m_octree;
    delete m_batchRenderer;
    delete m_occlusionCulling;

    // Delete owned boxes
    for (Box* box : m_ownedBoxes) {
        delete box;
    }
}

void Scene::addEntity(Box* box) {
    m_boxes.push_back(box);
    if (m_useOctree) {
        m_octree->insert(box);
    }
}

Box* Scene::createRect(const glm::vec3& position, const glm::vec3& size) {
    Box* box = new Box(position, size);
    m_ownedBoxes.push_back(box);
    addEntity(box);
    return box;
}

Box* Scene::createRect(const glm::vec3& position, float size) {
    return createRect(position, glm::vec3(size, size, size));
}

void Scene::clear() {
    m_boxes.clear();
    m_octree->clear();
    m_occlusionCulling->clear();

    for (Box* box : m_ownedBoxes) {
        delete box;
    }
    m_ownedBoxes.clear();
}

void Scene::inheritSettings(bool frustumCulling, bool batchRendering, bool octree, bool occlusionCulling) {
    if (!m_overrideFrustumCulling) {
        m_useFrustumCulling = frustumCulling;
    }
    if (!m_overrideBatchRendering) {
        m_useBatchRendering = batchRendering;
    }
    if (!m_overrideOctree) {
        m_useOctree = octree;
    }
    if (!m_overrideOcclusionCulling) {
        m_useOcclusionCulling = occlusionCulling;
    }
}

void Scene::rebuildOctree() {
    m_octree->rebuild(m_boxes);
}

void Scene::updateFrustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    m_frustum.update(projectionMatrix, viewMatrix);
}

void Scene::update(FlyCamera* camera, const glm::mat4& projectionMatrix) {
    if (camera) {
        updateFrustum(projectionMatrix, camera->getViewMatrix());
    }
}

void Scene::renderScene(Renderer* renderer, FlyCamera* camera) {
    m_stats.reset();
    m_stats.totalEntities = m_boxes.size();

    std::vector<Box*> visibleBoxes;

    // Step 1: Frustum culling
    if (m_useOctree && m_useFrustumCulling) {
        m_octree->query(m_frustum, visibleBoxes);
        m_stats.frustumCulled = m_stats.totalEntities - visibleBoxes.size();
    } else if (m_useFrustumCulling) {
        for (auto box : m_boxes) {
            if (m_frustum.isBoxVisible(box)) {
                visibleBoxes.push_back(box);
            }
        }
        m_stats.frustumCulled = m_stats.totalEntities - visibleBoxes.size();
    } else {
        visibleBoxes = m_boxes;
        m_stats.frustumCulled = 0;
    }

    // Step 2: Occlusion culling query pass
    if (m_useOcclusionCulling) {
        m_occlusionCulling->beginQueryPass();
        for (auto box : visibleBoxes) {
            m_occlusionCulling->issueQuery(box);
        }
        m_occlusionCulling->endQueryPass();
        m_occlusionCulling->updateQueryResults();

        // Filter based on occlusion results
        std::vector<Box*> occlusionFiltered;
        for (auto box : visibleBoxes) {
            if (m_occlusionCulling->isVisible(box)) {
                occlusionFiltered.push_back(box);
            }
        }
        m_stats.occlusionCulled = visibleBoxes.size() - occlusionFiltered.size();
        visibleBoxes = occlusionFiltered;
    } else {
        m_stats.occlusionCulled = 0;
    }

    m_stats.rendered = visibleBoxes.size();

    // Step 3: Render visible objects
    if (m_useBatchRendering) {
        m_batchRenderer->beginBatch();

        glm::vec3 cameraPos = glm::vec3(0.0f);
        if (camera) {
            glm::mat4 view = camera->getViewMatrix();
            cameraPos = -glm::vec3(view[3][0], view[3][1], view[3][2]);
        }

        for (auto box : visibleBoxes) {
            LODLevel lod = LODLevel::HIGH;
            if (camera) {
                lod = m_lodSystem.calculateLOD(box->position, cameraPos);
            }
            m_batchRenderer->addInstance(box, lod, cameraPos);
        }

        m_batchRenderer->endBatch();
    } else {
        for (auto box : visibleBoxes) {
            renderer->drawBox(box);
        }
    }
}

void Scene::render(Renderer* renderer, FlyCamera* camera) {
    renderScene(renderer, camera);
}