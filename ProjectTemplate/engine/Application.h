#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "Renderer.h"
#include "FlyCamera.h"
#include "InputManager.h"
#include "Scene.h"
#include "LODSystem.h"

class Application {
public:
    Application(int width, int height, const char* title);
    ~Application();

    Scene* createScene(const std::string& name);
    Scene* getScene(const std::string& name);
    void setActiveScene(const std::string& name);
    Scene* getActiveScene() { return m_activeScene; }
    void deleteScene(const std::string& name);

    void setCamera(FlyCamera* camera);
    void setInputManager(InputManager* input);
    FlyCamera* getCamera() { return m_camera; }
    InputManager* getInput() { return m_input; }

    void enableFrustumCulling(bool enable) { m_defaultFrustumCulling = enable; updateSceneDefaults(); }
    void enableBatchRendering(bool enable) { m_defaultBatchRendering = enable; updateSceneDefaults(); }
    void enableOctree(bool enable) { m_defaultOctree = enable; updateSceneDefaults(); }
    void setLODSettings(const LODSettings& settings);

    bool getFrustumCullingEnabled() const { return m_defaultFrustumCulling; }
    bool getBatchRenderingEnabled() const { return m_defaultBatchRendering; }
    bool getOctreeEnabled() const { return m_defaultOctree; }

    void run();

    Renderer* getRenderer() { return m_renderer; }
    glm::mat4 getProjectionMatrix() const { return m_projectionMatrix; }

    void checkSceneSwitching();

private:
    int m_width, m_height;
    const char* m_title;
    Renderer* m_renderer;
    FlyCamera* m_camera;
    InputManager* m_input;

    std::unordered_map<std::string, Scene*> m_scenes;
    Scene* m_activeScene;

    bool m_defaultFrustumCulling;
    bool m_defaultBatchRendering;
    bool m_defaultOctree;
    LODSettings m_defaultLODSettings;

    glm::mat4 m_projectionMatrix;

    void updateSceneDefaults();
};