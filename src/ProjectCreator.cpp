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
        result.replace(pos, 14, projectName);
        pos += projectName.size();
    }
    return result;
}

bool ProjectCreator::CreateProject(
    const std::string& basePath,
    const std::string& projectName,
    const std::string& templatePath
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
                }

                std::ofstream outFile(destPath, std::ios::binary);
                outFile << content;
                outFile.close();
            }
        }

        std::ofstream engineFile(projectRoot / (projectName + ".engine"));
        engineFile << "project_name: " << projectName << "\n";
        engineFile << "engine_version: 0.0.1\n";

        engineFile << "cmake_cmd: cmake ..\n";
        engineFile << "build_cmd: cmake --build .\n";
        engineFile.close();


        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
}
