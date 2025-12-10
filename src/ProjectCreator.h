#ifndef PROJECT_CREATOR_H
#define PROJECT_CREATOR_H

#include <string>

class ProjectCreator {
public:
    static bool CreateProject(
        const std::string& basePath,
        const std::string& projectName,
        const std::string& templatePath
    );
};

#endif
