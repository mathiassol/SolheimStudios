cmake_minimum_required(VERSION 3.16)

project(SolheimStudios2 LANGUAGES CXX)

# -------------------------------------------------
# C++ standard
# -------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -------------------------------------------------
# Sources
# -------------------------------------------------
file(GLOB_RECURSE ENGINE_SRC
        engine/*.cpp
)

file(GLOB_RECURSE APP_SRC
        src/*.cpp
)

add_executable(app
        ${ENGINE_SRC}
        ${APP_SRC}
)

# -------------------------------------------------
# Base include directories
# -------------------------------------------------
target_include_directories(app
        PRIVATE
        engine
        src
)

# -------------------------------------------------
# OS detection
# -------------------------------------------------
message(STATUS "Detected OS: ${CMAKE_SYSTEM_NAME}")

if (WIN32)
    include(cmake/Windows.cmake)
elseif (APPLE)
    include(cmake/MacOS.cmake)
elseif (UNIX)
    include(cmake/Linux.cmake)
else()
    message(FATAL_ERROR "Unsupported operating system")
endif()

# -------------------------------------------------
# Platform includes (from OS files)
# -------------------------------------------------
if (DEFINED PLATFORM_INCLUDES)
    target_include_directories(app PRIVATE ${PLATFORM_INCLUDES})
endif()

# -------------------------------------------------
# GLEW requirement (Linux)
# -------------------------------------------------
# Required to avoid __glew* linker issues
target_compile_definitions(app PRIVATE GLEW_STATIC)

# -------------------------------------------------
# Link libraries
# -------------------------------------------------
target_link_libraries(app
        PRIVATE
        ${PLATFORM_LIBS}
)

# -------------------------------------------------
# Compiler warnings (optional but recommended)
# -------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
    target_compile_options(app PRIVATE /W4)
endif()
