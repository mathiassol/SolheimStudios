#include "ProjectCreator.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

static std::string replaceProjectName(const std::string& content, const std::string& projectName) {
    std::string result = content;
    size_t pos = 0;
    while ((pos = result.find("${PROJECT_NAME}", pos)) != std::string::npos) {
        result.replace(pos, 15, projectName);
        pos += projectName.size();
    }
    return result;
}

static std::string replaceToolchainPath(const std::string& content, const std::string& toolchainPath) {
    std::string result = content;
    std::string normalized = toolchainPath;

    // Normalize path separators for CMake (use forward slashes)
    for (auto& c : normalized) {
        if (c == '\\') c = '/';
    }

    size_t pos = 0;
    while ((pos = result.find("${TOOLCHAIN_PATH}", pos)) != std::string::npos) {
        result.replace(pos, 17, normalized);
        pos += normalized.size();
    }
    return result;
}

std::string ProjectCreator::GetToolchainPath() {
    // Get the executable's directory
    fs::path exePath = fs::current_path();

    #ifdef _WIN32
        return (exePath / "toolchain" / "windows").string();
    #else
        return (exePath / "toolchain" / "linux").string();
    #endif
}

bool ProjectCreator::CreateProject(
    const std::string& basePath,
    const std::string& projectName,
    const std::string& templatePath,
    const std::string& toolchainPath
) {
    try {
        fs::path projectRoot = fs::path(basePath) / projectName;
        fs::path templateRoot = fs::path(templatePath);

        if (!fs::exists(templateRoot)) {
            std::cerr << "Template folder does not exist: " << templatePath << "\n";
            return false;
        }

        fs::create_directories(projectRoot);

        for (auto& p : fs::recursive_directory_iterator(templateRoot)) {
            const auto& relPath = fs::relative(p.path(), templateRoot);
            const auto& destPath = projectRoot / relPath;

            if (fs::is_directory(p.path())) {
                fs::create_directories(destPath);
            } else if (fs::is_regular_file(p.path())) {
                std::ifstream inFile(p.path(), std::ios::binary);
                std::ostringstream ss;
                ss << inFile.rdbuf();
                std::string content = ss.str();
                inFile.close();

                if (destPath.filename() == "CMakeLists.txt") {
                    content = replaceProjectName(content, projectName);
                    content = replaceToolchainPath(content, toolchainPath);
                }

                std::ofstream outFile(destPath, std::ios::binary);
                outFile << content;
                outFile.close();
            }
        }

        std::ofstream engineFile(projectRoot / (projectName + ".engine"));
        engineFile << "project_name: " << projectName << "\n";
        engineFile << "engine_version: 0.0.1\n";

        #ifdef _WIN32
            engineFile << "cmake_cmd: cmake -G \"MinGW Makefiles\" -DCMAKE_C_COMPILER=" << toolchainPath << "/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=" << toolchainPath << "/mingw64/bin/g++.exe ..\n";
            engineFile << "build_cmd: " << toolchainPath << "/mingw64/bin/mingw32-make.exe\n";
        #else
            engineFile << "cmake_cmd: cmake ..\n";
            engineFile << "build_cmd: make\n";
        #endif

        engineFile.close();

        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
}