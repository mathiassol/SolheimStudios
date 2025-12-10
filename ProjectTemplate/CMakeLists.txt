cmake_minimum_required(VERSION 3.10)
project(OpenGLExample)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")

# ---- Dependencies ----
find_package(OpenGL REQUIRED)

set(GLFW3_LIB_DIR "C:/msys64/mingw64/lib")
set(GLFW3_INCLUDE_DIR "C:/msys64/mingw64/include")

include_directories(${GLFW3_INCLUDE_DIR})
link_directories(${GLFW3_LIB_DIR})



# ---- Executable ----
add_executable(app
        src/main.cpp
)

# ---- Link libraries ----
target_link_libraries(app
        OpenGL32
        glew32
        glfw3
)
