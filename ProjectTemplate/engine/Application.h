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

    // Scene management
    Scene* createScene(const std::string& name);
    Scene* getScene(const std::string& name);
    void setActiveScene(const std::string& name);
    Scene* getActiveScene() { return m_activeScene; }

    // Camera and input
    void setCamera(FlyCamera* camera);
    void setInputManager(InputManager* input);

    // Global engine settings (inherited by new scenes)
    void enableFrustumCulling(bool enable) { m_defaultFrustumCulling = enable; updateSceneDefaults(); }
    void enableBatchRendering(bool enable) { m_defaultBatchRendering = enable; updateSceneDefaults(); }
    void enableOctree(bool enable) { m_defaultOctree = enable; updateSceneDefaults(); }
    void setLODSettings(const LODSettings& settings);

    void run();

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