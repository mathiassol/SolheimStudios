
#include "../engine/Application.h"
#include "../engine/InputManager.h"
#include "../engine/FlyCamera.h"
#include "../engine/Box.h"

int main() {
    Application app(1280, 720, "OpenGL FlyCam Example");

    InputManager input;
    FlyCamera camera(&input);
    app.setCamera(&camera);

    Box box({0,0,0}, {1,1,1});
    app.addEntity(&box);

    app.run();
}
