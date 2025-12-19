# Windows.cmake

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

set(PLATFORM_LIBS
        OpenGL::GL
        glfw
        GLEW::GLEW
        glm::glm
)
