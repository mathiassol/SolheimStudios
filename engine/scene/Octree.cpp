#include "Octree.h"
#include <algorithm>
#include <cmath>

Octree::Octree(const glm::vec3& center, float halfSize, int maxDepth, int maxObjectsPerNode)
    : m_maxDepth(maxDepth), m_maxObjectsPerNode(maxObjectsPerNode), m_objectCount(0) {
    m_root = new OctreeNode(center, halfSize);
}

Octree::~Octree() {
    delete m_root;
}

void Octree::insert(Box* box) {
    if (m_root->intersects(box)) {
        insertRecursive(m_root, box, 0);
        m_objectCount++;
    }
}

void Octree::insertRecursive(OctreeNode* node, Box* box, int depth) {
    if (node->isLeaf) {
        node->objects.push_back(box);

        if (node->objects.size() > (size_t)m_maxObjectsPerNode && depth < m_maxDepth) {
            subdivide(node);
        }
    } else {
        bool placed = false;
        for (int i = 0; i < 8; i++) {
            if (node->children[i]->contains(box)) {
                insertRecursive(node->children[i], box, depth + 1);
                placed = true;
                break;
            }
        }

        if (!placed) {
            node->objects.push_back(box);
        }
    }
}

void Octree::subdivide(OctreeNode* node) {
    float quarter = node->halfSize * 0.5f;
    node->isLeaf = false;

    for (int i = 0; i < 8; i++) {
        glm::vec3 offset;
        offset.x = (i & 1) ? quarter : -quarter;
        offset.y = (i & 2) ? quarter : -quarter;
        offset.z = (i & 4) ? quarter : -quarter;

        node->children[i] = new OctreeNode(node->center + offset, quarter);
    }

    std::vector<Box*> remaining;
    remaining.reserve(node->objects.size());

    for (Box* box : node->objects) {
        bool placed = false;
        for (int i = 0; i < 8; i++) {
            if (node->children[i]->contains(box)) {
                node->children[i]->objects.push_back(box);
                placed = true;
                break;
            }
        }
        if (!placed) {
            remaining.push_back(box);
        }
    }

    node->objects = std::move(remaining);
}

bool Octree::removeRecursive(OctreeNode* node, Box* box) {
    auto it = std::find(node->objects.begin(), node->objects.end(), box);
    if (it != node->objects.end()) {
        node->objects.erase(it);
        return true;
    }

    if (!node->isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]->intersects(box)) {
                if (removeRecursive(node->children[i], box)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Octree::remove(Box* box) {
    if (removeRecursive(m_root, box)) {
        m_objectCount--;
    }
}

void Octree::clear() {
    glm::vec3 center = m_root->center;
    float halfSize = m_root->halfSize;
    delete m_root;
    m_root = new OctreeNode(center, halfSize);
    m_objectCount = 0;
}

void Octree::rebuild(const std::vector<Box*>& boxes) {
    clear();
    for (Box* box : boxes) {
        insert(box);
    }
}

void Octree::queryFrustum(const Frustum& frustum, std::vector<Box*>& result) const {
    result.clear();
    result.reserve(m_objectCount / 4);
    queryFrustumRecursive(m_root, frustum, result);
}

void Octree::queryFrustumRecursive(OctreeNode* node, const Frustum& frustum, std::vector<Box*>& result) const {
    if (!node->intersectsFrustum(frustum)) {
        return;
    }

    for (Box* box : node->objects) {
        if (frustum.isBoxVisible(box)) {
            result.push_back(box);
        }
    }

    if (!node->isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                queryFrustumRecursive(node->children[i], frustum, result);
            }
        }
    }
}

void Octree::queryRange(const glm::vec3& center, float radius, std::vector<Box*>& result) const {
    result.clear();
    queryRangeRecursive(m_root, center, radius, result);
}

void Octree::queryRangeRecursive(OctreeNode* node, const glm::vec3& center, float radius, std::vector<Box*>& result) const {
    float nodeRadius = node->halfSize * 1.732f; // sqrt(3)
    if (glm::distance(node->center, center) > radius + nodeRadius) {
        return;
    }

    float radiusSq = radius * radius;
    for (Box* box : node->objects) {
        if (glm::distance(box->position, center) <= radiusSq) {
            result.push_back(box);
        }
    }

    if (!node->isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                queryRangeRecursive(node->children[i], center, radius, result);
            }
        }
    }
}

void Octree::queryAABB(const glm::vec3& min, const glm::vec3& max, std::vector<Box*>& result) const {
    result.clear();
    queryAABBRecursive(m_root, min, max, result);
}

void Octree::queryAABBRecursive(OctreeNode* node, const glm::vec3& min, const glm::vec3& max, std::vector<Box*>& result) const {
    // AABB intersection test
    glm::vec3 nodeMin = node->center - glm::vec3(node->halfSize);
    glm::vec3 nodeMax = node->center + glm::vec3(node->halfSize);

    if (max.x < nodeMin.x || min.x > nodeMax.x ||
        max.y < nodeMin.y || min.y > nodeMax.y ||
        max.z < nodeMin.z || min.z > nodeMax.z) {
        return;
    }

    // Test objects
    for (Box* box : node->objects) {
        glm::vec3 halfSize = box->size * 0.5f;
        glm::vec3 boxMin = box->position - halfSize;
        glm::vec3 boxMax = box->position + halfSize;

        if (!(max.x < boxMin.x || min.x > boxMax.x ||
              max.y < boxMin.y || min.y > boxMax.y ||
              max.z < boxMin.z || min.z > boxMax.z)) {
            result.push_back(box);
        }
    }

    if (!node->isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                queryAABBRecursive(node->children[i], min, max, result);
            }
        }
    }
}

bool Octree::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Box** hitBox) const {
    float closestDist = maxDistance;
    *hitBox = nullptr;
    return raycastRecursive(m_root, origin, direction, maxDistance, hitBox, closestDist);
}

bool Octree::raycastRecursive(OctreeNode* node, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Box** hitBox, float& closestDist) const {
    // Simple ray-box intersection for octree node
    glm::vec3 nodeMin = node->center - glm::vec3(node->halfSize);
    glm::vec3 nodeMax = node->center + glm::vec3(node->halfSize);

    float tmin = 0.0f;
    float tmax = maxDistance;

    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / direction[i];
        float t0 = (nodeMin[i] - origin[i]) * invD;
        float t1 = (nodeMax[i] - origin[i]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);
        if (tmax < tmin) return false;
    }

    bool hit = false;

    for (Box* box : node->objects) {
        glm::vec3 boxMin = box->position - box->size * 0.5f;
        glm::vec3 boxMax = box->position + box->size * 0.5f;

        float t0 = 0.0f;
        float t1 = maxDistance;

        for (int i = 0; i < 3; i++) {
            float invD = 1.0f / direction[i];
            float ta = (boxMin[i] - origin[i]) * invD;
            float tb = (boxMax[i] - origin[i]) * invD;
            if (invD < 0.0f) std::swap(ta, tb);
            t0 = std::max(t0, ta);
            t1 = std::min(t1, tb);
            if (t1 < t0) break;
        }

        if (t1 >= t0 && t0 < closestDist) {
            closestDist = t0;
            *hitBox = box;
            hit = true;
        }
    }

    if (!node->isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                if (raycastRecursive(node->children[i], origin, direction, closestDist, hitBox, closestDist)) {
                    hit = true;
                }
            }
        }
    }

    return hit;
}

int Octree::getNodeCount() const {
    return getNodeCountRecursive(m_root);
}

int Octree::getNodeCountRecursive(OctreeNode* node) const {
    int count = 1;
    if (!node->isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                count += getNodeCountRecursive(node->children[i]);
            }
        }
    }
    return count;
}