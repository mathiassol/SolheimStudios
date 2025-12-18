message(STATUS "Configuring for Linux")

# -------------------------------------------------
# OpenGL
# -------------------------------------------------
find_package(OpenGL REQUIRED)

# -------------------------------------------------
# pkg-config
# -------------------------------------------------
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

# -------------------------------------------------
# Libraries to link
# -------------------------------------------------
set(PLATFORM_LIBS
        OpenGL::GL
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
)

# -------------------------------------------------
# Include directories
# -------------------------------------------------
set(PLATFORM_INCLUDES
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
)
