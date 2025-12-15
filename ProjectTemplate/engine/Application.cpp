#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Application.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Application::Application(int width, int height, const char* title)
    : m_width(width), m_height(height), m_title(title), m_camera(nullptr), m_input(nullptr),
      m_useFrustumCulling(true), m_useBatchRendering(true), m_useOctree(true)
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

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to init GLEW\n";
        exit(-1);
    }

    m_renderer = new Renderer(window);
    m_octree = new Octree(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, 5, 8);
    m_batchRenderer = new BatchRenderer();
}

Application::~Application() {
    delete m_renderer;
    delete m_octree;
    delete m_batchRenderer;
    glfwTerminate();
}

void Application::addEntity(Box* box) {
    m_boxes.push_back(box);
    if (m_useOctree) {
        m_octree->insert(box);
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
    m_lodSystem.setSettings(settings);
}

void Application::rebuildOctree() {
    m_octree->rebuild(m_boxes);
}

void Application::updateFrustum() {
    if (m_camera) {
        m_frustum.update(m_projectionMatrix, m_camera->getViewMatrix());
    }
}

void Application::renderScene() {
    std::vector<Box*> visibleBoxes;

    if (m_useOctree && m_useFrustumCulling) {
        m_octree->query(m_frustum, visibleBoxes);
    } else if (m_useFrustumCulling) {
        for (auto box : m_boxes) {
            if (m_frustum.isBoxVisible(box)) {
                visibleBoxes.push_back(box);
            }
        }
    } else {
        visibleBoxes = m_boxes;
    }

    if (m_useBatchRendering) {
        m_batchRenderer->beginBatch();

        glm::vec3 cameraPos = m_camera ? m_camera->getViewMatrix()[3] : glm::vec3(0.0f);

        for (auto box : visibleBoxes) {
            LODLevel lod = LODLevel::HIGH;
            if (m_camera) {
                // Extract camera position from view matrix
                glm::mat4 view = m_camera->getViewMatrix();
                glm::vec3 camPos = glm::vec3(view[3][0], view[3][1], view[3][2]);
                lod = m_lodSystem.calculateLOD(box->position, camPos);
            }
            m_batchRenderer->addInstance(box, lod, cameraPos);
        }

        m_batchRenderer->endBatch();
    } else {
        for (auto box : visibleBoxes) {
            m_renderer->drawBox(box);
        }
    }
}

void Application::run() {
    GLFWwindow* window = m_renderer->getWindow();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    float aspect = (float)width / (float)height;
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

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

    // Set material properties
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

        if (m_camera) {
            m_camera->update();
        }

        updateFrustum();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if (m_camera) {
            glm::mat4 view = m_camera->getViewMatrix();
            glMultMatrixf(&view[0][0]);
        }

        renderScene();

        frameCount++;
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= 1.0) {
            std::cout << "FPS: " << frameCount << " | Boxes: " << m_boxes.size() << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}