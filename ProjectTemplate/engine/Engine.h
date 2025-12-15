#pragma once
#include "Application.h"
#include "InputManager.h"

// Global engine access singleton
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

    // Convenience wrappers for common operations
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

    static bool isKeyPressed(int key) {
        return s_inputManager ? s_inputManager->isKeyPressed(key) : false;
    }

    static bool isActionPressed(const std::string& action) {
        return s_inputManager ? s_inputManager->isActionPressed(action) : false;
    }

private:
    static Application* s_application;
    static InputManager* s_inputManager;
};