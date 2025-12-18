# Engine Hand Guide

This short hand guide explains how to use the core engine features shown in the provided files.  
It is intended to be **simple, practical, and easy to follow**.

---

## 1. Engine

The engine exposes a **static `Engine` interface** that works as a global access point to:

- Application
- Input
- Camera
- Scenes
- Rendering optimizations
- World queries (Octree)

You must initialize it **once** at startup.

```cpp
Engine::initialize(&app, &input);
```

After this, `Engine::` calls can be used anywhere.

---

## 2. Application

Create the application window:

```cpp
Application app(1920, 1080, "3D Engine");
```

This controls:
- Window size
- Rendering loop
- Scene management

Start the engine loop with:

```cpp
app.run();
```

---

## 3. Input System

The input system maps **keys to named actions**.

### Binding Keys
```cpp
InputManager input;
input.bindKey("forward", GLFW_KEY_W);
input.bindKey("backward", GLFW_KEY_S);
```

### Checking Input Anywhere
```cpp
if (Engine::isActionPressed("forward")) {
    // move forward
}
```

You can also check raw keys:
```cpp
Engine::isKeyPressed(GLFW_KEY_SPACE);
```

---

## 4. Camera 

The default camera is a **FlyCamera**, ideal for editor-style movement.

```cpp
FlyCamera camera(&input);
camera.setPosition({0, 10, 30});
camera.setSpeed(0.5f);
camera.setSensitivity(0.1f);
```

Attach it to the application:

```cpp
app.setCamera(&camera);
```

### Access Camera Data Anywhere
```cpp
glm::vec3 pos = Engine::getCameraPosition();
glm::vec3 forward = Engine::getCameraFront();
```

---

## 5. Scene System

Scenes allow you to separate worlds or tests.

### Create a Scene
```cpp
Scene* scene = Engine::createScene("main");
```

### Switch Active Scene
```cpp
Engine::setActiveScene("main");
```

### Get Active Scene
```cpp
Scene* active = Engine::getActiveScene();
```

### Delete a Scene
```cpp
Engine::deleteScene("performance");
```

---

## 6. Creating Objects (Boxes)

Boxes are simple 3D primitives used for testing and terrain.

### Create a Box with Size
```cpp
Engine::createRect(
    glm::vec3(0.0f, 5.0f, 0.0f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);
```

### Create a Box with Uniform Size
```cpp
Engine::createRect(glm::vec3(0, 0, 0), 1.0f);
```

> Objects are always created in the **active scene**.

---

## 7. Rendering Optimizations

These can be toggled at runtime.

### Frustum Culling
Hides objects outside the camera view.

```cpp
Engine::enableFrustumCulling(true);
```

### Batch Rendering
Groups draw calls for better performance.

```cpp
Engine::enableBatchRendering(true);
```

### Octree Spatial Partitioning
Required for fast queries and raycasting.

```cpp
Engine::enableOctree(true);
```

---

## 8. Level of Detail (LOD)

Controls render distance and object quality.

```cpp
LODSettings lod;
lod.highDistance = 30.0f;
lod.mediumDistance = 60.0f;
lod.lowDistance = 100.0f;
lod.cullDistance = 150.0f;

Engine::setLODSettings(lod);
```

Objects beyond `cullDistance` will not render.

---

## 9. Octree Queries

Octree must be enabled.

### Range Query
```cpp
std::vector<Box*> results;
Engine::queryRange(center, radius, results);
```

### AABB Query
```cpp
Engine::queryAABB(min, max, results);
```

### Raycasting
```cpp
Box* hit = nullptr;
bool didHit = Engine::raycast(
    origin,
    direction,
    maxDistance,
    &hit
);
```

Useful for:
- Mouse picking
- Shooting
- Visibility tests

---

## 10. Example Scene Generation

### Terrain Grid
Creates a flat grid of boxes.

```cpp
scene->createRect(position, size);
```

### Random Object Stress Test
Used to test performance with thousands of objects.

```cpp
createRandomObjects(scene, 10000, -25.0f, 25.0f);
```

---

## 11. Typical Engine Flow

1. Create `Application`
2. Setup `InputManager`
3. Create `FlyCamera`
4. Initialize `Engine`
5. Enable optimizations
6. Create scenes and objects
7. Set active scene
8. Run application

---

## 12. Notes

- `Engine` is a **global helper**, not a full ECS
- Octree features require `enableOctree(true)`
- Objects always belong to the active scene
- This setup is ideal for prototypes, tools, and experiments

---

**End of Hand Guide**