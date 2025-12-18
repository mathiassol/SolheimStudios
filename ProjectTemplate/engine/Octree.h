#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Box.h"
#include "Frustum.h"

struct OctreeNode {
    glm::vec3 center;
    float halfSize;
    std::vector<Box*> objects;
    OctreeNode* children[8];
    bool isLeaf;

    OctreeNode(const glm::vec3& c, float hs)
        : center(c), halfSize(hs), isLeaf(true) {
        for (int i = 0; i < 8; i++) children[i] = nullptr;
        objects.reserve(8); // Pre-allocate for better performance
    }

    ~OctreeNode() {
        for (int i = 0; i < 8; i++) {
            delete children[i];
        }
    }

    bool contains(const Box* box) const {
        glm::vec3 halfBoxSize = box->size * 0.5f;
        glm::vec3 min = center - glm::vec3(halfSize);
        glm::vec3 max = center + glm::vec3(halfSize);
        glm::vec3 boxMin = box->position - halfBoxSize;
        glm::vec3 boxMax = box->position + halfBoxSize;

        return (boxMin.x >= min.x && boxMax.x <= max.x &&
                boxMin.y >= min.y && boxMax.y <= max.y &&
                boxMin.z >= min.z && boxMax.z <= max.z);
    }

    bool intersects(const Box* box) const {
        glm::vec3 halfBoxSize = box->size * 0.5f;
        glm::vec3 nodeMin = center - glm::vec3(halfSize);
        glm::vec3 nodeMax = center + glm::vec3(halfSize);
        glm::vec3 boxMin = box->position - halfBoxSize;
        glm::vec3 boxMax = box->position + halfBoxSize;

        return (boxMin.x <= nodeMax.x && boxMax.x >= nodeMin.x &&
                boxMin.y <= nodeMax.y && boxMax.y >= nodeMin.y &&
                boxMin.z <= nodeMax.z && boxMax.z >= nodeMin.z);
    }

    bool intersectsFrustum(const Frustum& frustum) const {
        Box nodeBox(center, glm::vec3(halfSize * 2.0f));
        return frustum.isBoxVisible(&nodeBox);
    }
};

class Octree {
public:
    Octree(const glm::vec3& center = glm::vec3(0.0f),
           float halfSize = 100.0f,
           int maxDepth = 5,
           int maxObjectsPerNode = 8);
    ~Octree();

    void insert(Box* box);
    void remove(Box* box);
    void clear();
    void rebuild(const std::vector<Box*>& boxes);

    void queryFrustum(const Frustum& frustum, std::vector<Box*>& result) const;
    void queryRange(const glm::vec3& center, float radius, std::vector<Box*>& result) const;
    void queryAABB(const glm::vec3& min, const glm::vec3& max, std::vector<Box*>& result) const;

    // Ray casting
    bool raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Box** hitBox) const;

    // Statistics
    int getObjectCount() const { return m_objectCount; }
    int getNodeCount() const;
    int getMaxDepth() const { return m_maxDepth; }

    // Configuration
    void setMaxDepth(int depth) { m_maxDepth = depth; }
    void setMaxObjectsPerNode(int count) { m_maxObjectsPerNode = count; }

private:
    OctreeNode* m_root;
    int m_maxDepth;
    int m_maxObjectsPerNode;
    int m_objectCount;

    void insertRecursive(OctreeNode* node, Box* box, int depth);
    bool removeRecursive(OctreeNode* node, Box* box);
    void subdivide(OctreeNode* node);
    void queryFrustumRecursive(OctreeNode* node, const Frustum& frustum, std::vector<Box*>& result) const;
    void queryRangeRecursive(OctreeNode* node, const glm::vec3& center, float radius, std::vector<Box*>& result) const;
    void queryAABBRecursive(OctreeNode* node, const glm::vec3& min, const glm::vec3& max, std::vector<Box*>& result) const;
    bool raycastRecursive(OctreeNode* node, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Box** hitBox, float& closestDist) const;
    int getNodeCountRecursive(OctreeNode* node) const;
};