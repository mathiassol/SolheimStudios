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
    }

    ~OctreeNode() {
        for (int i = 0; i < 8; i++) {
            delete children[i];
        }
    }

    bool contains(const Box* box) const {
        glm::vec3 halfBoxSize = box->size * 0.5f;
        return (box->position.x - halfBoxSize.x >= center.x - halfSize &&
                box->position.x + halfBoxSize.x <= center.x + halfSize &&
                box->position.y - halfBoxSize.y >= center.y - halfSize &&
                box->position.y + halfBoxSize.y <= center.y + halfSize &&
                box->position.z - halfBoxSize.z >= center.z - halfSize &&
                box->position.z + halfBoxSize.z <= center.z + halfSize);
    }

    bool intersectsFrustum(const Frustum& frustum) const {
        Box nodeBox(center, glm::vec3(halfSize * 2.0f));
        return frustum.isBoxVisible(&nodeBox);
    }
};

class Octree {
public:
    Octree(const glm::vec3& center, float halfSize, int maxDepth = 5, int maxObjectsPerNode = 8);
    ~Octree();

    void insert(Box* box);
    void clear();
    void rebuild(const std::vector<Box*>& boxes);

    void query(const Frustum& frustum, std::vector<Box*>& result) const;
    void queryRange(const glm::vec3& center, float radius, std::vector<Box*>& result) const;

private:
    OctreeNode* m_root;
    int m_maxDepth;
    int m_maxObjectsPerNode;

    void insertRecursive(OctreeNode* node, Box* box, int depth);
    void subdivide(OctreeNode* node);
    void queryRecursive(OctreeNode* node, const Frustum& frustum, std::vector<Box*>& result) const;
    void queryRangeRecursive(OctreeNode* node, const glm::vec3& center, float radius, std::vector<Box*>& result) const;
};