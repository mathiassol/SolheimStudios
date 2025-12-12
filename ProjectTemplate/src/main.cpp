#include "../engine/Application.h"
#include "../engine/InputManager.h"
#include "../engine/FlyCamera.h"
#include "../engine/Box.h"

int main() {
    Application app(1280, 720, "OpenGL FlyCam Example");

    InputManager input;
    input.bindKey("forward", GLFW_KEY_W);
    input.bindKey("backward", GLFW_KEY_S);
    input.bindKey("left", GLFW_KEY_A);
    input.bindKey("right", GLFW_KEY_D);
    input.bindKey("up", GLFW_KEY_SPACE);
    input.bindKey("down", GLFW_KEY_LEFT_SHIFT);
    
    FlyCamera camera(&input);
    camera.setPosition({0, 0, 5});
    camera.setSpeed(0.1f);
    camera.setSensitivity(0.1f);
    
    app.setInputManager(&input);
    app.setCamera(&camera);

    Box box1({0, 0, 0}, {1, 1, 1});
    Box box2({3, 0, 0}, {1, 1, 1});
    Box box3({-3, 0, 0}, {1, 1, 1});
    Box box4({0, 2, 0}, {1, 1, 1});
    
    app.addEntity(&box1);
    app.addEntity(&box2);
    app.addEntity(&box3);
    app.addEntity(&box4);

    app.run();
    
    return 0;
}