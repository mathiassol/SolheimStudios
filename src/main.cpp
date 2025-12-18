#include "../engine/core/Application.h"
#include "../engine/core/InputManager.h"
#include "../engine/components/FlyCamera.h"
#include "../engine/core/Engine.h"
#include <random>

/*
 * This is an example file to showcase how to use different engine tools.
 * if you want further info see to docs/HAND-GUIDE.md
 */


// these 2 are just to generate objects in to the scenes
void createTerrainGrid(Scene* scene, int size, float spacing) {
    for (int x = -size; x <= size; x++) {
        for (int z = -size; z <= size; z++) {

            // this is the actuall logic. you chose a scene in this case scene is a var you can just "myScene->"
            // then you chose the function in this case createRect();
            // then inside you create 2 vec3's one for pos and one for size if you want to hard code in vaules make sure its a float so "1.0f", so a use of this can be:
            // myScene->createRect(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
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




int main() {
    // this explains its self mostly, its the window size and text.
    Application app(1920, 1080, "3D Engine");

    // Setup input
    // here we init the input manager.
    // it works by listning to inputs and replacing them with a text var, then you can look for input "forward" in any file.
    // if you want to se how its used you can open up flycamera.cpp inside engine/
    InputManager input;
    input.bindKey("forward", GLFW_KEY_W);
    input.bindKey("backward", GLFW_KEY_S);
    input.bindKey("left", GLFW_KEY_A);
    input.bindKey("right", GLFW_KEY_D);
    input.bindKey("up", GLFW_KEY_SPACE);
    input.bindKey("down", GLFW_KEY_LEFT_SHIFT);

    // init the defaut enigine camera and give it accses to the input manager

    FlyCamera camera(&input);
    camera.setPosition({0, 10, 30});
    camera.setSpeed(0.5f);
    camera.setSensitivity(0.1f);

    app.setInputManager(&input);
    app.setCamera(&camera);

    // init the Engine singleton, this will allow you to use engine:: calls.
    Engine::initialize(&app, &input);

    // here is our first engine:: call. we use it to enable or disable optimizations, octree, batchrendering, and frustum culling.
    Engine::enableFrustumCulling(true);
    Engine::enableBatchRendering(true);
    Engine::enableOctree(true);


    // LOD is currently not that greate but it can be used to set a max render distance
    LODSettings lod;
    lod.highDistance = 30.0f;
    lod.mediumDistance = 60.0f;
    lod.lowDistance = 100.0f;
    lod.cullDistance = 150.0f; // this is the dist that we cull / completly hide objects
    Engine::setLODSettings(lod); // and load in the settings via Engine::


    // we have a scene system, and this is how we init it.
    // testScene is the name we call it by in main.cpp and "main" is what the engine calls it.
    Scene* testScene = Engine::createScene("main");

    // here we use the fuction from earlyer to create a grid of blocks.
    createTerrainGrid(testScene, 20, 5.0f);

    // create another scene with wayyy more objects to test renderer preformace.
    Scene* perfTest = Engine::createScene("performance");
    createRandomObjects(perfTest, 10000, -25.0f, 25.0f);


    // setActiveScene is used to chose scene it can be used like this or at runtime to change our scene.
    Engine::setActiveScene("main");


    // start the app
    app.run();

    return 0;
}