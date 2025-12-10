cmake_minimum_required(VERSION 3.10)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

# Use bundled toolchain
set(TOOLCHAIN_ROOT "${TOOLCHAIN_PATH}")

# Platform-specific settings
if(WIN32)
    set(GLFW3_INCLUDE_DIR "${TOOLCHAIN_ROOT}/libs/include")
    set(GLFW3_LIB_DIR "${TOOLCHAIN_ROOT}/libs/lib")
    set(GLEW_INCLUDE_DIR "${TOOLCHAIN_ROOT}/libs/include")
    set(GLEW_LIB_DIR "${TOOLCHAIN_ROOT}/libs/lib")

    include_directories(${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
    link_directories(${GLFW3_LIB_DIR} ${GLEW_LIB_DIR})
else()
    # Linux: use system libraries or bundled static libs
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
endif()

# ---- Executable ----
add_executable(app
        src/main.cpp
)

# ---- Link libraries ----
if(WIN32)
    target_link_libraries(app
            opengl32
            glew32
            glfw3
            gdi32
    )
else()
    target_link_libraries(app
            OpenGL::GL
            glfw
            GLEW::GLEW
    )
endif()