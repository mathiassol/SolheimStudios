#pragma once
#include <vector>
#include "Renderer.h"
#include "FlyCamera.h"
#include "InputManager.h"
#include "Box.h"

class Application {
public:
    Application(int width, int height, const char* title);
    ~Application();

    void addEntity(Box* box);
    void setCamera(FlyCamera* camera);
    void setInputManager(InputManager* input);
    void run();

private:
    int m_width, m_height;
    const char* m_title;
    Renderer* m_renderer;
    FlyCamera* m_camera;
    InputManager* m_input;
    std::vector<Box*> m_boxes;
};