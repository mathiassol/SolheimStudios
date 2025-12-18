#include "Scene.h"
#include "../graphics/Renderer.h"
#include <iostream>
#include <algorithm>

Scene::Scene(const std::string& name)
    : m_name(name),
      m_useFrustumCulling(true),
      m_useBatchRendering(true),
      m_useOctree(true),
      m_overrideFrustumCulling(false),
      m_overrideBatchRendering(false),
      m_overrideOctree(false)
{
    m_octree = new Octree(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, 5, 8);
    m_batchRenderer = new BatchRenderer();
}

Scene::~Scene() {
    delete m_octree;
    delete m_batchRenderer;

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

void Scene::removeEntity(Box* box) {
    auto it = std::find(m_boxes.begin(), m_boxes.end(), box);
    if (it != m_boxes.end()) {
        m_boxes.erase(it);
        if (m_useOctree) {
            m_octree->remove(box);
        }
    }

    auto ownedIt = std::find(m_ownedBoxes.begin(), m_ownedBoxes.end(), box);
    if (ownedIt != m_ownedBoxes.end()) {
        m_ownedBoxes.erase(ownedIt);
        delete box;
    }
}

void Scene::clear() {
    m_boxes.clear();
    m_octree->clear();

    for (Box* box : m_ownedBoxes) {
        delete box;
    }
    m_ownedBoxes.clear();
}

void Scene::inheritSettings(bool frustumCulling, bool batchRendering, bool octree) {
    if (!m_overrideFrustumCulling) {
        m_useFrustumCulling = frustumCulling;
    }
    if (!m_overrideBatchRendering) {
        m_useBatchRendering = batchRendering;
    }
    if (!m_overrideOctree) {
        m_useOctree = octree;
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

    if (m_useOctree && m_useFrustumCulling) {
        m_octree->queryFrustum(m_frustum, visibleBoxes);
        m_stats.frustumCulled = m_stats.totalEntities - visibleBoxes.size();
    } else if (m_useFrustumCulling) {
        visibleBoxes.reserve(m_boxes.size() / 2);
        for (auto box : m_boxes) {
            if (m_frustum.isBoxVisible(box)) {
                visibleBoxes.push_back(box);
            }
        }
        m_stats.frustumCulled = m_stats.totalEntities - visibleBoxes.size();
    } else {
        // No culling
        visibleBoxes = m_boxes;
        m_stats.frustumCulled = 0;
    }

    m_stats.rendered = visibleBoxes.size();

    if (m_useBatchRendering) {
        m_batchRenderer->beginBatch();

        glm::vec3 cameraPos = glm::vec3(0.0f);
        if (camera) {
            cameraPos = camera->getPosition();
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