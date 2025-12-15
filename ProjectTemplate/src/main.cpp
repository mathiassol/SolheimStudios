#include "../engine/Application.h"
#include "../engine/InputManager.h"
#include "../engine/FlyCamera.h"
#include "../engine/Engine.h"
#include <iostream>
#include <random>

int main() {
    // Initialize application
    Application engine(1920, 1080, "My 3D Game");

    // Setup input system
    InputManager input;
    input.bindKey("forward", GLFW_KEY_W);
    input.bindKey("backward", GLFW_KEY_S);
    input.bindKey("left", GLFW_KEY_A);
    input.bindKey("right", GLFW_KEY_D);
    input.bindKey("up", GLFW_KEY_SPACE);
    input.bindKey("down", GLFW_KEY_LEFT_SHIFT);

    // Setup camera
    FlyCamera camera(&input);
    camera.setPosition({0, 10, 30});
    camera.setSpeed(0.3f);
    camera.setSensitivity(0.1f);

    // Connect systems
    engine.setInputManager(&input);
    engine.setCamera(&camera);
    Engine::initialize(&engine, &input);

    // Configure global optimization settings
    engine.enableFrustumCulling(true);
    engine.enableBatchRendering(true);
    engine.enableOctree(true);
    engine.enableOcclusionCulling(false);

    LODSettings lodSettings;
    lodSettings.highDistance = 20.0f;
    lodSettings.mediumDistance = 40.0f;
    lodSettings.lowDistance = 80.0f;
    lodSettings.cullDistance = 100.0f;
    engine.setLODSettings(lodSettings);

    Scene* worldScene = Engine::createScene("world");

    // Add terrain
    for (int x = -50; x <= 50; x += 5) {
        for (int z = -50; z <= 50; z += 5) {
            worldScene->createRect(
                glm::vec3(x, -2, z),
                glm::vec3(5, 0.5f, 5)
            );
        }
    }

    // Add random objects
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> xDist(-40.0f, 40.0f);
    std::uniform_real_distribution<float> zDist(-40.0f, 40.0f);
    std::uniform_real_distribution<float> yDist(0.0f, 10.0f);
    std::uniform_real_distribution<float> sizeDist(0.5f, 3.0f);

    for (int i = 0; i < 5000; i++) {
        glm::vec3 pos(xDist(rng), yDist(rng), zDist(rng));
        float size = sizeDist(rng);
        worldScene->createRect(pos, size);
    }

    // Create test scene with occlusion
    Scene* occlusionTest = Engine::createScene("occlusion_test");
    occlusionTest->enableOcclusionCulling(true);

    // Add walls
    for (int x = -20; x <= 20; x += 2) {
        for (int y = -10; y <= 10; y += 2) {
            occlusionTest->createRect(
                glm::vec3(x, y, 0),
                glm::vec3(2, 2, 0.5f)
            );
        }
    }

    // Add objects behind wall
    for (int i = 0; i < 1000; i++) {
        glm::vec3 pos(xDist(rng), yDist(rng), -20.0f);
        occlusionTest->createRect(pos, sizeDist(rng));
    }

    // Set initial scene
    Engine::setActiveScene("world");

    // Print info
    std::cout << "\n=== Game Initialized ===" << std::endl;
    std::cout << "Scenes:" << std::endl;
    std::cout << "  1. World - " << worldScene->getEntityCount() << " objects" << std::endl;
    std::cout << "  2. Occlusion Test - " << occlusionTest->getEntityCount() << " objects" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  Mouse - Look" << std::endl;
    std::cout << "  1/2 - Switch Scenes" << std::endl;
    std::cout << std::endl;

    // Run game loop
    engine.run();

    return 0;
}