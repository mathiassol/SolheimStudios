#include "../engine/Application.h"
#include "../engine/InputManager.h"
#include "../engine/FlyCamera.h"
#include "../engine/Box.h"
#include <vector>
#include <random>
#include <ostream>
#include <iostream>

int main() {
    Application engine(1280, 720, "OpenGL");

    InputManager input;
    input.bindKey("forward", GLFW_KEY_W);
    input.bindKey("backward", GLFW_KEY_S);
    input.bindKey("left", GLFW_KEY_A);
    input.bindKey("right", GLFW_KEY_D);
    input.bindKey("up", GLFW_KEY_SPACE);
    input.bindKey("down", GLFW_KEY_LEFT_SHIFT);

    FlyCamera camera(&input);
    camera.setPosition({0, 5, 30});
    camera.setSpeed(0.2f);
    camera.setSensitivity(0.1f);

    engine.setInputManager(&input);
    engine.setCamera(&camera);

    // Configure LOD settings
    LODSettings lodSettings;
    lodSettings.highDistance = 15.0f;
    lodSettings.mediumDistance = 30.0f;
    lodSettings.lowDistance = 60.0f;
    lodSettings.cullDistance = 100.0f;
    engine.setLODSettings(lodSettings);

    // 3 diffrent optimizations i created.
    engine.enableFrustumCulling(true); // only renders whats actualy in view.
    engine.enableBatchRendering(false); // groups objects tougheter based on LOD
    engine.enableOctree(true); // a data structure to speed up frustum


    // create a sceen with x amout of objects
    std::vector<Box*> boxes;
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.0f);


    // objects added for preformance benchmarking
    int count = 100000;

    // the area boxes spawn in
    glm::vec3 minBound(-20.0f, -5.0f, -20.0f);
    glm::vec3 maxBound( 20.0f,  5.0f,  20.0f);

    std::uniform_real_distribution<float> distX(minBound.x, maxBound.x);
    std::uniform_real_distribution<float> distY(minBound.y, maxBound.y);
    std::uniform_real_distribution<float> distZ(minBound.z, maxBound.z);

    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);

        glm::vec3 position(
            distX(rng),
            distY(rng),
            distZ(rng)
        );

        Box* box = new Box(position, glm::vec3(size, size, size));
        boxes.push_back(box);
        engine.addEntity(box);
    }

    std::cout << "Created " << boxes.size() << " boxes in the scene" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  W/A/S/D - Move camera" << std::endl;
    std::cout << "  Space/Shift - Move up/down" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << std::endl;
    std::cout << "Features enabled:" << std::endl;
    std::cout << "  - Frustum Culling (only renders visible objects)" << std::endl;
    std::cout << "  - LOD System (detail based on distance)" << std::endl;
    std::cout << "  - Batch Rendering (efficient GPU submission)" << std::endl;
    std::cout << "  - Octree Spatial Partitioning (fast queries)" << std::endl;
    std::cout << std::endl;

    engine.run();

    // Cleanup
    for (Box* box : boxes) {
        delete box;
    }

    return 0;
}