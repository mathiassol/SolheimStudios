#pragma once
#include <vector>
#include "Renderer.h"
#include "FlyCamera.h"
#include "InputManager.h"
#include "Box.h"
#include "Frustum.h"
#include "LODSystem.h"
#include "Octree.h"
#include "BatchRenderer.h"

class Application {
public:
    Application(int width, int height, const char* title);
    ~Application();

    void addEntity(Box* box);
    void setCamera(FlyCamera* camera);
    void setInputManager(InputManager* input);
    void run();

    void setLODSettings(const LODSettings& settings);
    void rebuildOctree();
    void enableFrustumCulling(bool enable) { m_useFrustumCulling = enable; }
    void enableBatchRendering(bool enable) { m_useBatchRendering = enable; }
    void enableOctree(bool enable) { m_useOctree = enable; }

private:
    int m_width, m_height;
    const char* m_title;
    Renderer* m_renderer;
    FlyCamera* m_camera;
    InputManager* m_input;
    std::vector<Box*> m_boxes;

    Frustum m_frustum;
    LODSystem m_lodSystem;
    Octree* m_octree;
    BatchRenderer* m_batchRenderer;

    bool m_useFrustumCulling;
    bool m_useBatchRendering;
    bool m_useOctree;

    glm::mat4 m_projectionMatrix;

    void updateFrustum();
    void renderScene();
};