#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Application.h"
#include <iostream>

Application::Application(int width, int height, const char* title)
    : m_width(width), m_height(height), m_title(title), m_camera(nullptr), m_input(nullptr)
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
}

Application::~Application() {
    delete m_renderer;
    glfwTerminate();
}

void Application::addEntity(Box* box) {
    m_boxes.push_back(box);
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

void Application::run() {
    GLFWwindow* window = m_renderer->getWindow();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        if (m_input) {
            m_input->update(window);
        }

        if (m_camera) {
            m_camera->update();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if (m_camera) {
            glm::mat4 view = m_camera->getViewMatrix();
            glLoadMatrixf(&view[0][0]);
        }

        for (auto box : m_boxes) {
            m_renderer->drawBox(box);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}