cmake_minimum_required(VERSION 3.10)
project(OpenGLApp)

set(CMAKE_CXX_STANDARD 17)
set(TOOLCHAIN_ROOT "${TOOLCHAIN_PATH}")

if(WIN32)
    set(GLFW3_INCLUDE_DIR "${TOOLCHAIN_ROOT}/libs/include")
    set(GLFW3_LIB_DIR "${TOOLCHAIN_ROOT}/libs/lib")
    set(GLEW_INCLUDE_DIR "${TOOLCHAIN_ROOT}/libs/include")
    set(GLEW_LIB_DIR "${TOOLCHAIN_ROOT}/libs/lib")

    include_directories(${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
    link_directories(${GLFW3_LIB_DIR} ${GLEW_LIB_DIR})
else()
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
endif()

# Find GLM
find_package(glm REQUIRED)
if(glm_FOUND)
    include_directories(${GLM_INCLUDE_DIRS})
endif()

include_directories(
        ${PROJECT_SOURCE_DIR}/engine
        ${PROJECT_SOURCE_DIR}/src
)

# Gather all .cpp files
file(GLOB_RECURSE ENGINE_SRC "engine/*.cpp")
file(GLOB_RECURSE SRC_SRC "src/*.cpp")

add_executable(app
        ${ENGINE_SRC}
        ${SRC_SRC}
)

# Link libraries
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
            GLU
            glfw
            GLEW::GLEW
    )
endif()