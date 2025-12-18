#pragma once
#include <glm/glm.hpp>

enum class LODLevel {
    HIGH = 0,
    MEDIUM = 1,
    LOW = 2,
    CULLED = 3
};

struct LODSettings {
    float highDistance = 10.0f;
    float mediumDistance = 25.0f;
    float lowDistance = 50.0f;
    float cullDistance = 100.0f;
};

class LODSystem {
public:
    LODSystem() = default;

    void setSettings(const LODSettings& settings) { m_settings = settings; }
    LODLevel calculateLOD(const glm::vec3& objectPos, const glm::vec3& cameraPos) const;

    int getSubdivisionLevel(LODLevel level) const;

private:
    LODSettings m_settings;
};