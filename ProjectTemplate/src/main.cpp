#include "../engine/Application.h"
#include "../engine/InputManager.h"
#include "../engine/FlyCamera.h"
#include "../engine/Engine.h"
#include <iostream>
#include <iomanip>
#include <random>

void createTerrainGrid(Scene* scene, int size, float spacing) {
    for (int x = -size; x <= size; x++) {
        for (int z = -size; z <= size; z++) {
            scene->createRect(
                glm::vec3(x * spacing, -2.0f, z * spacing),
                glm::vec3(spacing * 0.9f, 0.5f, spacing * 0.9f)
            );
        }
    }
}

void createRandomObjects(Scene* scene, int count, float rangeMin, float rangeMax) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> posDist(rangeMin, rangeMax);
    std::uniform_real_distribution<float> yDist(0.0f, 15.0f);
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.5f);

    for (int i = 0; i < count; i++) {
        scene->createRect(
            glm::vec3(posDist(rng), yDist(rng), posDist(rng)),
            sizeDist(rng)
        );
    }
}

// Create a grid of markers at specific positions to test LOD/Frustum
void createTestMarkers(Scene* scene) {
    // Create markers every 50 units in a grid pattern
    for (int x = -200; x <= 200; x += 50) {
        for (int z = -200; z <= 200; z += 50) {
            // Create a tall marker so it's easy to see
            scene->createRect(
                glm::vec3(x, 5.0f, z),
                glm::vec3(2.0f, 10.0f, 2.0f)
            );
        }
    }
}

int main() {
    // Initialize application
    Application app(1920, 1080, "3D Engine - Camera Fix Demo");

    // Setup input
    InputManager input;
    input.bindKey("forward", GLFW_KEY_W);
    input.bindKey("backward", GLFW_KEY_S);
    input.bindKey("left", GLFW_KEY_A);
    input.bindKey("right", GLFW_KEY_D);
    input.bindKey("up", GLFW_KEY_SPACE);
    input.bindKey("down", GLFW_KEY_LEFT_SHIFT);

    // Setup camera with initial position
    FlyCamera camera(&input);
    camera.setPosition({0, 10, 30});
    camera.setSpeed(0.5f); // Increased speed to move around faster
    camera.setSensitivity(0.1f);

    // Connect systems
    app.setInputManager(&input);
    app.setCamera(&camera);
    Engine::initialize(&app, &input);

    // Configure optimizations
    Engine::enableFrustumCulling(true);
    Engine::enableBatchRendering(true);
    Engine::enableOctree(true);

    // LOD settings that should work correctly now
    LODSettings lod;
    lod.highDistance = 30.0f;
    lod.mediumDistance = 60.0f;
    lod.lowDistance = 100.0f;
    lod.cullDistance = 150.0f;
    Engine::setLODSettings(lod);

    // === Create test scene with markers ===
    Scene* testScene = Engine::createScene("main");

    // Add terrain
    createTerrainGrid(testScene, 20, 5.0f);

    // Add test markers to verify LOD/frustum works at any position
    createTestMarkers(testScene);

    // Add random objects
    createRandomObjects(testScene, 3000, -100.0f, 100.0f);

    // === Create a large world to test far distances ===
    Scene* largeWorld = Engine::createScene("test");

    // Terrain extending very far
    for (int x = -100; x <= 100; x += 5) {
        for (int z = -100; z <= 100; z += 5) {
            largeWorld->createRect(
                glm::vec3(x * 5.0f, -2.0f, z * 5.0f),
                glm::vec3(4.5f, 0.5f, 4.5f)
            );
        }
    }

    // Markers at extreme distances
    for (int i = -500; i <= 500; i += 100) {
        largeWorld->createRect(glm::vec3(i, 10, 0), glm::vec3(5, 20, 5));
        largeWorld->createRect(glm::vec3(0, 10, i), glm::vec3(5, 20, 5));
    }

    // === Performance test scene ===
    Scene* perfTest = Engine::createScene("performance");
    createRandomObjects(perfTest, 100000, -25.0f, 25.0f);

    // Set initial scene
    Engine::setActiveScene("main");

    // Print info
    std::cout << "\n=== 3D Engine - Camera Position Fix Demo ===" << std::endl;
    std::cout << "\nThis demo verifies that frustum culling and LOD work" << std::endl;
    std::cout << "correctly regardless of camera position." << std::endl;

    std::cout << "\nScenes:" << std::endl;
    std::cout << "  [1] Test Scene - " << testScene->getEntityCount() << " objects (with distance markers)" << std::endl;
    std::cout << "  [2] Large World - " << largeWorld->getEntityCount() << " objects (extreme distances)" << std::endl;
    std::cout << "  [3] Performance Test - " << perfTest->getEntityCount() << " objects" << std::endl;

    std::cout << "\nOptimizations:" << std::endl;
    std::cout << "  Frustum Culling: ON" << std::endl;
    std::cout << "  Octree: ON" << std::endl;
    std::cout << "  Batch Rendering: ON" << std::endl;
    std::cout << "  LOD System: ON" << std::endl;

    std::cout << "\nLOD Distances:" << std::endl;
    std::cout << "  High: < " << lod.highDistance << " units" << std::endl;
    std::cout << "  Medium: " << lod.highDistance << " - " << lod.mediumDistance << " units" << std::endl;
    std::cout << "  Low: " << lod.mediumDistance << " - " << lod.lowDistance << " units" << std::endl;
    std::cout << "  Culled: > " << lod.cullDistance << " units" << std::endl;

    std::cout << "\nControls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  Mouse - Look" << std::endl;
    std::cout << "  1/2/3 - Switch Scenes" << std::endl;

    std::cout << "\nTest Instructions:" << std::endl;
    std::cout << "  1. Move far from origin (e.g., 200+ units away)" << std::endl;
    std::cout << "  2. Verify tall markers are still visible/culled correctly" << std::endl;
    std::cout << "  3. Check FPS stats show proper culling numbers" << std::endl;
    std::cout << "  4. LOD should transition smoothly based on distance from camera" << std::endl;
    std::cout << "\n" << std::endl;

    // Run
    app.run();

    return 0;
}