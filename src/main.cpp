#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include "ProjectCreator.h"

namespace fs = std::filesystem;

std::map<std::string, std::string> readEngineFile(const std::string& path) {
    std::map<std::string, std::string> config;
    fs::path enginePath;

    for (auto& p : fs::directory_iterator(path)) {
        if (p.path().extension() == ".engine") {
            enginePath = p.path();
            break;
        }
    }

    if (!fs::exists(enginePath))
        return config;

    std::ifstream in(enginePath);
    std::string line;

    while (std::getline(in, line)) {
        auto delimPos = line.find(": ");
        if (delimPos != std::string::npos) {
            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 2);
            config[key] = value;
        }
    }

    return config;
}

bool compileProject(const std::string& path) {
    fs::path projectPath(path);

    if (!fs::exists(projectPath / "CMakeLists.txt")) {
        std::cout << "No CMakeLists.txt found. Not a valid project.\n";
        return false;
    }

    fs::path buildPath = projectPath / "build";
    fs::create_directories(buildPath);

    auto config = readEngineFile(projectPath.string());
    std::string cmakeCmd = config.count("cmake_cmd") ? config["cmake_cmd"] : "cmake ..";
    std::string buildCmd = config.count("build_cmd") ? config["build_cmd"] : "cmake --build .";

    std::cout << "Running CMake command: " << cmakeCmd << "\n";

    #ifdef _WIN32
        std::string fullCmakeCmd = "cd /d \"" + buildPath.string() + "\" && " + cmakeCmd;
    #else
        std::string fullCmakeCmd = "cd \"" + buildPath.string() + "\" && " + cmakeCmd;
    #endif

    if (system(fullCmakeCmd.c_str()) != 0)
        return false;

    std::cout << "Running Build command: " << buildCmd << "\n";

    #ifdef _WIN32
        std::string fullBuildCmd = "cd /d \"" + buildPath.string() + "\" && " + buildCmd;
    #else
        std::string fullBuildCmd = "cd \"" + buildPath.string() + "\" && " + buildCmd;
    #endif

    if (system(fullBuildCmd.c_str()) != 0)
        return false;

    return true;
}

bool runProject(const std::string& path) {
    fs::path executable;

#ifdef _WIN32
    executable = fs::path(path) / "build" / "app.exe";
#else
    executable = fs::path(path) / "build" / "app";
#endif

    if (!fs::exists(executable)) {
        std::cout << "Executable not found. Did you compile first?\n";
        return false;
    }

    #ifdef _WIN32
        std::string cmd = "\"" + executable.string() + "\"";
    #else
        std::string cmd = executable.string();
    #endif

    return system(cmd.c_str()) == 0;
}

void setBuildCommands(const std::string& path) {
    auto config = readEngineFile(path);
    if (config.empty()) {
        std::cout << "Project not found or missing .engine file.\n";
        return;
    }

    std::string cmakeCmd, buildCmd;
    std::cout << "Enter CMake command: ";
    std::getline(std::cin, cmakeCmd);

    std::cout << "Enter Build command: ";
    std::getline(std::cin, buildCmd);

    fs::path enginePath;
    for (auto& p : fs::directory_iterator(path)) {
        if (p.path().extension() == ".engine") {
            enginePath = p.path();
            break;
        }
    }

    std::ofstream out(enginePath);
    out << "project_name: " << config["project_name"] << "\n";
    out << "engine_version: " << config["engine_version"] << "\n";
    out << "cmake_cmd: " << cmakeCmd << "\n";
    out << "build_cmd: " << buildCmd << "\n";

    std::cout << "Build commands updated.\n";
}

void checkToolchain() {
    std::string toolchainPath = ProjectCreator::GetToolchainPath();

    #ifdef _WIN32
        fs::path gccPath = fs::path(toolchainPath) / "mingw64" / "bin" / "gcc.exe";
        fs::path glewPath = fs::path(toolchainPath) / "libs" / "include" / "GL" / "glew.h";
        fs::path glfwPath = fs::path(toolchainPath) / "libs" / "include" / "GLFW" / "glfw3.h";

        std::cout << "\n=== Toolchain Check (Windows) ===\n";
        std::cout << "Toolchain path: " << toolchainPath << "\n";
        std::cout << "GCC: " << (fs::exists(gccPath) ? "✓ Found" : "✗ Missing") << "\n";
        std::cout << "GLEW: " << (fs::exists(glewPath) ? "✓ Found" : "✗ Missing") << "\n";
        std::cout << "GLFW: " << (fs::exists(glfwPath) ? "✓ Found" : "✗ Missing") << "\n";

        if (!fs::exists(gccPath)) {
            std::cout << "\n⚠ MinGW-w64 not found! Download from: https://github.com/niXman/mingw-builds-binaries/releases\n";
            std::cout << "   Extract to: " << toolchainPath << "/mingw64/\n";
        }
        if (!fs::exists(glewPath) || !fs::exists(glfwPath)) {
            std::cout << "\n⚠ OpenGL libraries missing! See setup instructions.\n";
        }
    #else
        std::cout << "\n=== Toolchain Check (Linux) ===\n";
        std::cout << "System GCC will be used.\n";
        std::cout << "Make sure to install: sudo apt install build-essential libglfw3-dev libglew-dev\n";
    #endif

    std::cout << "=================================\n\n";
}

// Main CLI
int main() {
    std::cout << "Engine Editor CLI (prototype)\n";
    std::cout << "Bundled compiler version - works out of the box!\n";

    checkToolchain();

    std::cout << "Commands: create <path> <name>, compile <path>, run <path>, set_build <path>, check, clear, exit\n";

    while (true) {
        std::cout << "> ";

        std::string line;
        std::getline(std::cin, line);

        if (line.empty())
            continue;

        std::string cmd;
        std::istringstream iss(line);
        iss >> cmd;

        if (cmd == "create") {
            std::string path, name;
            iss >> path >> name;

            if (path.empty() || name.empty()) {
                std::cout << "Usage: create <path> <name>\n";
                continue;
            }

            std::string templatePath = "ProjectTemplate";
            std::string toolchainPath = ProjectCreator::GetToolchainPath();

            if (ProjectCreator::CreateProject(path, name, templatePath, toolchainPath))
                std::cout << "Project created successfully.\n";
            else
                std::cout << "Failed to create project.\n";
        }

        else if (cmd == "compile") {
            std::string path;
            iss >> path;

            if (path.empty()) {
                std::cout << "Usage: compile <path>\n";
                continue;
            }

            if (compileProject(path))
                std::cout << "Compilation successful.\n";
            else
                std::cout << "Compilation failed.\n";
        }

        else if (cmd == "run") {
            std::string path;
            iss >> path;

            if (path.empty()) {
                std::cout << "Usage: run <path>\n";
                continue;
            }

            runProject(path);
        }

        else if (cmd == "set_build") {
            std::string path;
            iss >> path;

            if (path.empty()) {
                std::cout << "Usage: set_build <path>\n";
                continue;
            }

            setBuildCommands(path);
        }

        else if (cmd == "check") {
            checkToolchain();
        }

        else if (cmd == "clear" || cmd == "cls") {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
        }

        else if (cmd == "exit") {
            break;
        }

        else {
            std::cout << "Unknown command.\n";
        }
    }

    return 0;
}