#include "LODSystem.h"

LODLevel LODSystem::calculateLOD(const glm::vec3& objectPos, const glm::vec3& cameraPos) const {
    float distance = glm::distance(objectPos, cameraPos);

    if (distance > m_settings.cullDistance) {
        return LODLevel::CULLED;
    } else if (distance > m_settings.lowDistance) {
        return LODLevel::LOW;
    } else if (distance > m_settings.mediumDistance) {
        return LODLevel::MEDIUM;
    } else {
        return LODLevel::HIGH;
    }
}

int LODSystem::getSubdivisionLevel(LODLevel level) const {
    switch (level) {
    case LODLevel::HIGH:   return 3;
    case LODLevel::MEDIUM: return 2;
    case LODLevel::LOW:    return 1;
    case LODLevel::CULLED: return 0;
    default: return 1;
    }
}