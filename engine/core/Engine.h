#pragma once
#include "Application.h"
#include "InputManager.h"
#include "../components/FlyCamera.h"
#include <glm/glm.hpp>

class Engine {
public:
    static void initialize(Application* app, InputManager* input) {
        s_application = app;
        s_inputManager = input;
    }

    static Application* getApp() {
        return s_application;
    }

    static InputManager* getInput() {
        return s_inputManager;
    }

    static Scene* createScene(const std::string& name) {
        return s_application ? s_application->createScene(name) : nullptr;
    }

    static Scene* getScene(const std::string& name) {
        return s_application ? s_application->getScene(name) : nullptr;
    }

    static void setActiveScene(const std::string& name) {
        if (s_application) s_application->setActiveScene(name);
    }

    static Scene* getActiveScene() {
        return s_application ? s_application->getActiveScene() : nullptr;
    }

    static void deleteScene(const std::string& name) {
        if (s_application) s_application->deleteScene(name);
    }

    static bool isKeyPressed(int key) {
        return s_inputManager ? s_inputManager->isKeyPressed(key) : false;
    }

    static bool isActionPressed(const std::string& action) {
        return s_inputManager ? s_inputManager->isActionPressed(action) : false;
    }

    static void bindKey(const std::string& action, int key) {
        if (s_inputManager) s_inputManager->bindKey(action, key);
    }

    static FlyCamera* getCamera() {
        return s_application ? s_application->getCamera() : nullptr;
    }

    static glm::vec3 getCameraPosition() {
        FlyCamera* cam = getCamera();
        return cam ? cam->getPosition() : glm::vec3(0.0f);
    }

    static glm::vec3 getCameraFront() {
        FlyCamera* cam = getCamera();
        return cam ? cam->getFront() : glm::vec3(0.0f, 0.0f, -1.0f);
    }

    static glm::vec3 getCameraUp() {
        FlyCamera* cam = getCamera();
        return cam ? cam->getUp() : glm::vec3(0.0f, 1.0f, 0.0f);
    }

    static glm::vec3 getCameraRight() {
        FlyCamera* cam = getCamera();
        return cam ? cam->getRight() : glm::vec3(1.0f, 0.0f, 0.0f);
    }

    static void enableFrustumCulling(bool enable) {
        if (s_application) s_application->enableFrustumCulling(enable);
    }

    static void enableBatchRendering(bool enable) {
        if (s_application) s_application->enableBatchRendering(enable);
    }

    static void enableOctree(bool enable) {
        if (s_application) s_application->enableOctree(enable);
    }

    static void setLODSettings(const LODSettings& settings) {
        if (s_application) s_application->setLODSettings(settings);
    }

    static Box* createRect(const glm::vec3& position, const glm::vec3& size) {
        Scene* scene = getActiveScene();
        return scene ? scene->createRect(position, size) : nullptr;
    }

    static Box* createRect(const glm::vec3& position, float size = 1.0f) {
        Scene* scene = getActiveScene();
        return scene ? scene->createRect(position, size) : nullptr;
    }

    static void queryRange(const glm::vec3& center, float radius, std::vector<Box*>& result) {
        Scene* scene = getActiveScene();
        if (scene && scene->getOctree()) {
            scene->getOctree()->queryRange(center, radius, result);
        }
    }

    static void queryAABB(const glm::vec3& min, const glm::vec3& max, std::vector<Box*>& result) {
        Scene* scene = getActiveScene();
        if (scene && scene->getOctree()) {
            scene->getOctree()->queryAABB(min, max, result);
        }
    }

    static bool raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Box** hitBox) {
        Scene* scene = getActiveScene();
        if (scene && scene->getOctree()) {
            return scene->getOctree()->raycast(origin, direction, maxDistance, hitBox);
        }
        return false;
    }

private:
    static Application* s_application;
    static InputManager* s_inputManager;
};