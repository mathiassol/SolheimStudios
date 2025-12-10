cmake_minimum_required(VERSION 3.20)
project(EngineEditor)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(EngineEditor
        src/main.cpp
        src/ProjectCreator.cpp
        src/ProjectCreator.h
)