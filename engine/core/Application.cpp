#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Application.h"
#include "Engine.h"
#include <iostream>
#include <iomanip>
#include <glm/gtc/matrix_transform.hpp>

Application::Application(int width, int height, const char* title)
    : m_width(width), m_height(height), m_title(title),
      m_camera(nullptr), m_input(nullptr), m_activeScene(nullptr),
      m_defaultFrustumCulling(true), m_defaultBatchRendering(true),
      m_defaultOctree(true)
{
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        exit(-1);
    }

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to init GLEW\n";
        exit(-1);
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    m_renderer = new Renderer(window);
}

Application::~Application() {
    for (auto& pair : m_scenes) {
        delete pair.second;
    }

    delete m_renderer;
    glfwTerminate();
}

Scene* Application::createScene(const std::string& name) {
    if (m_scenes.find(name) != m_scenes.end()) {
        std::cerr << "Scene '" << name << "' already exists!\n";
        return m_scenes[name];
    }

    Scene* scene = new Scene(name);
    scene->inheritSettings(m_defaultFrustumCulling, m_defaultBatchRendering, m_defaultOctree);
    scene->setLODSettings(m_defaultLODSettings);
    m_scenes[name] = scene;

    if (!m_activeScene) {
        m_activeScene = scene;
    }

    return scene;
}

Scene* Application::getScene(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it != m_scenes.end()) {
        return it->second;
    }
    return nullptr;
}

void Application::setActiveScene(const std::string& name) {
    Scene* scene = getScene(name);
    if (scene) {
        m_activeScene = scene;
        std::cout << "Switched to scene: " << name << std::endl;
    } else {
        std::cerr << "Scene '" << name << "' not found!\n";
    }
}

void Application::deleteScene(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it != m_scenes.end()) {
        if (m_activeScene == it->second) {
            m_activeScene = nullptr;
        }
        delete it->second;
        m_scenes.erase(it);
        std::cout << "Deleted scene: " << name << std::endl;
    }
}

void Application::setCamera(FlyCamera* camera) {
    m_camera = camera;
}

void Application::setInputManager(InputManager* input) {
    m_input = input;
    if (m_input && m_renderer) {
        m_input->setMouseCallback(m_renderer->getWindow());
    }
}

void Application::setLODSettings(const LODSettings& settings) {
    m_defaultLODSettings = settings;
    for (auto& pair : m_scenes) {
        pair.second->setLODSettings(settings);
    }
}

void Application::updateSceneDefaults() {
    for (auto& pair : m_scenes) {
        pair.second->inheritSettings(m_defaultFrustumCulling, m_defaultBatchRendering, m_defaultOctree);
    }
}

void Application::checkSceneSwitching() {
    static bool key1WasPressed = false;
    static bool key2WasPressed = false;
    static bool key3WasPressed = false;

    bool key1Pressed = Engine::isKeyPressed(GLFW_KEY_1);
    bool key2Pressed = Engine::isKeyPressed(GLFW_KEY_2);
    bool key3Pressed = Engine::isKeyPressed(GLFW_KEY_3);

    if (key1Pressed && !key1WasPressed) {
        setActiveScene("main");
    }
    if (key2Pressed && !key2WasPressed) {
        setActiveScene("test");
    }
    if (key3Pressed && !key3WasPressed) {
        setActiveScene("performance");
    }

    key1WasPressed = key1Pressed;
    key2WasPressed = key2Pressed;
    key3WasPressed = key3Pressed;
}

void Application::run() {
    GLFWwindow* window = m_renderer->getWindow();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    float aspect = (float)width / (float)height;
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&m_projectionMatrix[0][0]);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = {5.0f, 5.0f, 5.0f, 1.0f};
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    int frameCount = 0;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        if (m_input) {
            m_input->update(window);
        }

        checkSceneSwitching();

        if (m_camera) {
            m_camera->update();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if (m_camera) {
            glm::mat4 view = m_camera->getViewMatrix();
            glMultMatrixf(&view[0][0]);
        }

        if (m_activeScene) {
            m_activeScene->update(m_camera, m_projectionMatrix);
            m_activeScene->render(m_renderer, m_camera);
        }

        frameCount++;
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= 1.0) {
            if (m_activeScene) {
                const CullingStats& stats = m_activeScene->getCullingStats();

                std::cout << "FPS: " << std::setw(4) << frameCount
                          << " | Scene: " << std::setw(12) << std::left << m_activeScene->getName()
                          << " | Total: " << std::setw(6) << std::right << stats.totalEntities
                          << " | Rendered: " << std::setw(6) << stats.rendered
                          << " | Culled: " << std::setw(6) << stats.frustumCulled
                          << std::endl;
            }
            frameCount = 0;
            lastTime = currentTime;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}