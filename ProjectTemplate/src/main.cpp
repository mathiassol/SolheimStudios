#include "../engine/Application.h"
#include "../engine/InputManager.h"
#include "../engine/FlyCamera.h"
#include <iostream>
#include <random>

int main() {
    Application engine(1280, 720, "OpenGL Scene System");

    // Setup input
    InputManager input;
    input.bindKey("forward", GLFW_KEY_W);
    input.bindKey("backward", GLFW_KEY_S);
    input.bindKey("left", GLFW_KEY_A);
    input.bindKey("right", GLFW_KEY_D);
    input.bindKey("up", GLFW_KEY_SPACE);
    input.bindKey("down", GLFW_KEY_LEFT_SHIFT);
    input.bindKey("scene", GLFW_KEY_KP_1);

    // Setup camera
    FlyCamera camera(&input);
    camera.setPosition({0, 5, 30});
    camera.setSpeed(0.2f);
    camera.setSensitivity(0.1f);

    engine.setInputManager(&input);
    engine.setCamera(&camera);

    LODSettings lodSettings;
    lodSettings.highDistance = 15.0f;
    lodSettings.mediumDistance = 30.0f;
    lodSettings.lowDistance = 60.0f;
    lodSettings.cullDistance = 100.0f;
    engine.setLODSettings(lodSettings);

    engine.enableFrustumCulling(true);
    engine.enableBatchRendering(true);
    engine.enableOctree(true);

    Scene* mainScene = engine.createScene("main");

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.0f);
    std::uniform_real_distribution<float> distX(-20.0f, 20.0f);
    std::uniform_real_distribution<float> distY(-5.0f, 5.0f);
    std::uniform_real_distribution<float> distZ(-20.0f, 20.0f);

    int count = 10000;
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        glm::vec3 position(distX(rng), distY(rng), distZ(rng));

        mainScene->createRect(position, size);
    }

    Scene* testScene = engine.createScene("test");

    testScene->enableFrustumCulling(false);
    testScene->enableBatchRendering(false);

    for (int x = -5; x <= 5; x += 2) {
        for (int z = -5; z <= 5; z += 2) {
            testScene->createRect(glm::vec3(x, 0, z), 1.0f);
        }
    }

    Scene* perfScene = engine.createScene("performance");

    for (int i = 0; i < 100000; ++i) {
        float size = sizeDist(rng);
        glm::vec3 position(distX(rng), distY(rng), distZ(rng));
        perfScene->createRect(position, size);
    }

    // Set the starting scene
    engine.setActiveScene("main");

    std::cout << "\n=== Scene System Demo ===" << std::endl;
    std::cout << "Created " << mainScene->getEntityCount() << " boxes in 'main' scene" << std::endl;
    std::cout << "Created " << testScene->getEntityCount() << " boxes in 'test' scene" << std::endl;
    std::cout << "Created " << perfScene->getEntityCount() << " boxes in 'performance' scene" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  W/A/S/D - Move camera" << std::endl;
    std::cout << "  Space/Shift - Move up/down" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "\nActive Scene: " << engine.getActiveScene()->getName() << std::endl;
    std::cout << "\nFeatures:" << std::endl;
    std::cout << "  - Scene System (multiple independent worlds)" << std::endl;
    std::cout << "  - Easy entity creation (createRect)" << std::endl;
    std::cout << "  - Per-scene optimization overrides" << std::endl;
    std::cout << "  - Automatic memory management" << std::endl;
    std::cout << std::endl;

    engine.run();

    return 0;
}