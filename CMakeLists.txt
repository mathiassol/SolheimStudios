cmake_minimum_required(VERSION 3.20)
project(EngineEditor)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(EngineEditor
        src/main.cpp
        src/ProjectCreator.cpp
        src/ProjectCreator.h
)

add_custom_command(TARGET EngineEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ProjectTemplate
        $<TARGET_FILE_DIR:EngineEditor>/ProjectTemplate
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/toolchain
        $<TARGET_FILE_DIR:EngineEditor>/toolchain
        COMMENT "Copying ProjectTemplate and toolchain to build directory"
)