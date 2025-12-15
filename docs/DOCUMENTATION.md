# Solheim Studios - Engine Documentation

## Table of Contents
1. [Getting Started](#getting-started)
2. [Engine API Reference](#engine-api-reference)
3. [Scene System](#scene-system)
4. [Camera & Input](#camera--input)
5. [Optimization Systems](#optimization-systems)
6. [Complete Example](#complete-example)

---

## Getting Started

### Basic Setup Template

Every project needs these essential components:

```cpp
#include "../engine/Application.h"
#include "../engine/InputManager.h"
#include "../engine/FlyCamera.h"
#include "../engine/Engine.h"

int main() {
    // 1. Create the application
    Application engine(1280, 720, "My Game");
    
    // 2. Setup input system
    InputManager input;
    input.bindKey("forward", GLFW_KEY_W);
    input.bindKey("backward", GLFW_KEY_S);
    input.bindKey("left", GLFW_KEY_A);
    input.bindKey("right", GLFW_KEY_D);
    input.bindKey("up", GLFW_KEY_SPACE);
    input.bindKey("down", GLFW_KEY_LEFT_SHIFT);
    
    // 3. Setup camera
    FlyCamera camera(&input);
    camera.setPosition({0, 5, 10});
    camera.setSpeed(0.2f);
    camera.setSensitivity(0.1f);
    
    // 4. Connect everything
    engine.setInputManager(&input);
    engine.setCamera(&camera);
    
    // 5. Initialize global engine access
    Engine::initialize(&engine, &input);
    
    // 6. Create your scene and add objects
    Scene* myScene = Engine::createScene("main");
    myScene->createRect(glm::vec3(0, 0, 0), 1.0f);
    
    // 7. Run the engine
    engine.run();
    
    return 0;
}
```

---

## Engine API Reference

The `Engine` class provides global access to engine features from anywhere in your code.

### Scene Management

#### `Engine::createScene(name)`
Creates and returns a new scene.
```cpp
Scene* mainScene = Engine::createScene("main");
Scene* menuScene = Engine::createScene("menu");
```

#### `Engine::getScene(name)`
Retrieves an existing scene by name.
```cpp
Scene* scene = Engine::getScene("main");
if (scene) {
    // Do something with scene
}
```

#### `Engine::setActiveScene(name)`
Switches to a different scene.
```cpp
Engine::setActiveScene("menu");
```

#### `Engine::getActiveScene()`
Gets the currently active scene.
```cpp
Scene* current = Engine::getActiveScene();
std::cout << "Current scene: " << current->getName() << std::endl;
```

### Input Management

#### `Engine::isKeyPressed(key)`
Checks if a specific key is currently pressed.
```cpp
if (Engine::isKeyPressed(GLFW_KEY_ESCAPE)) {
    // Handle escape key
}
```

#### `Engine::isActionPressed(action)`
Checks if a bound action is pressed.
```cpp
if (Engine::isActionPressed("forward")) {
    // Move forward
}
```

### Global Access

#### `Engine::getApp()`
Returns the Application instance.
```cpp
Application* app = Engine::getApp();
```

#### `Engine::getInput()`
Returns the InputManager instance.
```cpp
InputManager* input = Engine::getInput();
```

---

## Scene System

### Creating Scenes

```cpp
Scene* gameScene = Engine::createScene("game");
```

### Adding Objects to Scenes

#### `createRect(position, size)`
Creates a rectangular box in the scene.

```cpp
// Single size for all dimensions
myScene->createRect(glm::vec3(0, 0, 0), 1.0f);

// Individual dimensions
myScene->createRect(glm::vec3(5, 2, -3), glm::vec3(2.0f, 1.0f, 3.0f));
```

#### Adding Many Objects

```cpp
// Create a grid of boxes
for (int x = -10; x <= 10; x += 2) {
    for (int z = -10; z <= 10; z += 2) {
        myScene->createRect(glm::vec3(x, 0, z), 1.0f);
    }
}

// Random placement with distributions
std::mt19937 rng(42);
std::uniform_real_distribution<float> posDist(-20.0f, 20.0f);
std::uniform_real_distribution<float> sizeDist(0.5f, 2.0f);

for (int i = 0; i < 1000; i++) {
    glm::vec3 pos(posDist(rng), posDist(rng), posDist(rng));
    float size = sizeDist(rng);
    myScene->createRect(pos, size);
}
```

### Scene Properties

#### `getEntityCount()`
Returns the number of entities in the scene.
```cpp
size_t count = myScene->getEntityCount();
std::cout << "Objects: " << count << std::endl;
```

#### `getName()`
Returns the scene's name.
```cpp
std::string name = myScene->getName();
```

#### `getCullingStats()`
Gets culling statistics for the scene.
```cpp
const CullingStats& stats = myScene->getCullingStats();
std::cout << "Total: " << stats.totalEntities << std::endl;
std::cout << "Rendered: " << stats.rendered << std::endl;
std::cout << "Frustum Culled: " << stats.frustumCulled << std::endl;
std::cout << "Occlusion Culled: " << stats.occlusionCulled << std::endl;
```

---

## Camera & Input

### Camera Setup

```cpp
FlyCamera camera(&input);
camera.setPosition(glm::vec3(0, 10, 20));  // Starting position
camera.setSpeed(0.2f);                      // Movement speed
camera.setSensitivity(0.1f);                // Mouse sensitivity
```

### Input Binding

Map game actions to keyboard keys:

```cpp
InputManager input;

// Movement
input.bindKey("forward", GLFW_KEY_W);
input.bindKey("backward", GLFW_KEY_S);
input.bindKey("left", GLFW_KEY_A);
input.bindKey("right", GLFW_KEY_D);
input.bindKey("up", GLFW_KEY_SPACE);
input.bindKey("down", GLFW_KEY_LEFT_SHIFT);

// Custom actions
input.bindKey("jump", GLFW_KEY_SPACE);
input.bindKey("crouch", GLFW_KEY_C);
input.bindKey("interact", GLFW_KEY_E);
```

### Checking Input

```cpp
// Direct key check
if (Engine::isKeyPressed(GLFW_KEY_ESCAPE)) {
    // Exit game
}

// Action check
if (Engine::isActionPressed("interact")) {
    // Perform interaction
}
```

---

## Optimization Systems

### Global Settings (Application Level)

These settings are inherited by all new scenes:

```cpp
// Enable/disable for all scenes
engine.enableFrustumCulling(true);    // Cull objects outside camera view
engine.enableBatchRendering(true);     // Batch similar objects together
engine.enableOctree(true);             // Use spatial partitioning
engine.enableOcclusionCulling(false);  // Cull objects behind other objects

// LOD (Level of Detail) settings
LODSettings lodSettings;
lodSettings.highDistance = 15.0f;      // High detail up to 15 units
lodSettings.mediumDistance = 30.0f;    // Medium detail up to 30 units
lodSettings.lowDistance = 60.0f;       // Low detail up to 60 units
lodSettings.cullDistance = 100.0f;     // Don't render beyond 100 units
engine.setLODSettings(lodSettings);
```

### Per-Scene Settings (Override)

Individual scenes can override global settings:

```cpp
Scene* testScene = Engine::createScene("test");

// Override specific settings for this scene
testScene->enableFrustumCulling(true);
testScene->enableBatchRendering(false);
testScene->enableOctree(true);
testScene->enableOcclusionCulling(true);  // Special: enable occlusion for this scene

// Custom LOD for this scene
LODSettings customLOD;
customLOD.highDistance = 20.0f;
customLOD.mediumDistance = 40.0f;
customLOD.lowDistance = 80.0f;
customLOD.cullDistance = 150.0f;
testScene->setLODSettings(customLOD);
```

### Optimization Techniques Explained

#### **Frustum Culling** ★★★★☆
- **Type:** CPU
- **What it does:** Only renders objects within the camera's view
- **When to use:** Always (very low overhead, high benefit)
- **Impact:** Massive performance gain in large worlds

#### **Occlusion Culling** ★★★★☆
- **Type:** CPU + GPU
- **What it does:** Skips objects hidden behind other objects
- **When to use:** Dense environments with lots of overlap (cities, buildings)
- **Impact:** High gain in occluded scenes, but adds overhead
- **Note:** Can be counterproductive in sparse/open environments

#### **Octree Spatial Partitioning** ★★★☆☆
- **Type:** CPU
- **What it does:** Divides space into 8 sections recursively for faster queries
- **When to use:** Scenes with 1000+ objects
- **Impact:** Speeds up frustum culling, collision, and spatial queries

#### **Batch Rendering** ★★★★★
- **Type:** CPU + GPU
- **What it does:** Combines similar objects into single draw calls
- **When to use:** Always (critical for performance)
- **Impact:** Massive reduction in draw call overhead

#### **LOD (Level of Detail)** ★★★★☆
- **Type:** CPU + GPU
- **What it does:** Reduces detail of distant objects
- **When to use:** Always in 3D games
- **Impact:** Significant GPU savings on distant objects

---

## Complete Example

Here's a complete, production-ready example:

```cpp
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
    lodSettings.cullDistance = 150.0f;
    engine.setLODSettings(lodSettings);
    
    // Create game world scene
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
```

---

## Tips & Best Practices

### Performance

1. **Always enable frustum culling and batch rendering** - These have minimal overhead
2. **Use octrees for 1000+ objects** - Below that, the overhead isn't worth it
3. **Only enable occlusion culling in dense scenes** - Open worlds don't benefit
4. **Tune LOD distances for your scene** - Balance visual quality with performance

### Scene Organization

1. **Use separate scenes for different game states**
    - Menu scene, gameplay scene, loading scene
2. **Override optimization settings per scene**
    - Menu doesn't need occlusion culling
    - Gameplay might benefit from it

### Debugging

1. **Watch the FPS output** - Shows culling statistics every second
2. **Compare scenes with different settings** - Use test scenes to measure impact
3. **Start simple** - Add complexity gradually to identify bottlenecks

---

## Troubleshooting

**Low FPS with few objects?**
- Disable occlusion culling - it may be adding overhead

**Objects disappearing?**
- Check frustum culling - might be too aggressive
- Verify camera position and direction

**Scene not switching?**
- Check if scene name matches exactly
- Verify Engine::initialize() was called

**Input not working?**
- Ensure input.bindKey() was called for all actions
- Check Engine::initialize() included InputManager

---

## Advanced Usage

### Custom Scene Switching Logic

```cpp
// In your update loop or custom function
if (Engine::isKeyPressed(GLFW_KEY_1)) {
    Engine::setActiveScene("main");
}
if (Engine::isKeyPressed(GLFW_KEY_2)) {
    Engine::setActiveScene("test");
}
if (Engine::isKeyPressed(GLFW_KEY_ESCAPE)) {
    Engine::setActiveScene("menu");
}
```

### Dynamic Object Creation

```cpp
Scene* scene = Engine::getActiveScene();

// Create object at camera position
FlyCamera* camera = /* get camera */;
glm::vec3 spawnPos = camera->getPosition();
scene->createRect(spawnPos, 1.0f);
```

### Performance Monitoring

```cpp
const CullingStats& stats = scene->getCullingStats();
float cullPercentage = (float)stats.frustumCulled / stats.totalEntities * 100.0f;
std::cout << "Culling " << cullPercentage << "% of objects" << std::endl;
```

---

## Summary

You now have everything you need to create a 3D application:

1. ✅ Basic application setup
2. ✅ Scene creation and management
3. ✅ Object placement
4. ✅ Camera controls
5. ✅ Input handling
6. ✅ Performance optimization
7. ✅ Scene switching

Start with the basic template, add your objects, and tune the optimization settings for your specific use case. Happy coding!


# Rendering

# Optimization
So the engine used a bunch of different Optimization technics, mostly to improve rendering, tick speed, or memory. bellow is a more detailed view of how they work and what they actually do. The list below contains many features not all are yet or ever to be implemented.


### Frustum culling
**type: CPU**

**Impact: ★★★★☆**

Frustum culling is used to prevent rendering of everything outside the camera view. Effectively reducing draw calls and GPU workload.

### <Occlusion> culling
**type: CPU / GPU**

**Impact: ★★★★☆**

Skips hidden objects. If an object is hidden behind a wall, we skip its rendering. Center as it adds CPU/GPU overhead for visibility checks, so if this culling is really needed, it might be counterproductive.

### Portal culling 
**Tag: CPU**

**Impact: ★★★☆☆**

Culling objects based on segmented environments like caves or indoors. Visibility is determined by recursively rendering through connected portals. Objects outside of portals are then culled.

### Octree
**Tag: CPU**

**Impact: ★★★☆☆**

Octree is a form of spatial partitioning that subdivides 3d pace in to cubes. It's used to speed up Frustum culling, collision, ray cast, and visibility checks. It does this by limiting the actions operation to its relevant spatial region. Picture splitting the world in to section each section being split in 8 (an octagon), hens the name OC tree. OC for octagon.

### Batch rendering
**Tag: CPU / GPU**

**Impact: ★★★★★**

batch rendering is super important in larger renders. Its a technic used to combind objects into a single draw call when they share render state, shaders, and materials. Minimizes CPU to GPU overhead from excessive draw calls.

### Map Persistent buffers
**Tag: CPU / GPU**

**Impact: ★★★☆☆**


### Mipmapping

### Texture compression

### Anisotropic filtering

### CPU Multithreading / asynchrony

### GPU Asynchrony

### LOD Usage

### shader cache

### UBOs

### Forward+

### GPU Instancing

### Depth Pre-Pass

### Meshlet

### Temporal Upscaling
