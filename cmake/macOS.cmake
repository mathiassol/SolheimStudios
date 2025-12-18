message(STATUS "Configuring for macOS")

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(app
        PRIVATE
        OpenGL::GL
)
