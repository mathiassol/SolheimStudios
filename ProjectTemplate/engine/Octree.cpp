#include "Octree.h"

Octree::Octree(const glm::vec3& center, float halfSize, int maxDepth, int maxObjectsPerNode)
    : m_maxDepth(maxDepth), m_maxObjectsPerNode(maxObjectsPerNode) {
    m_root = new OctreeNode(center, halfSize);
}

Octree::~Octree() {
    delete m_root;
}

void Octree::insert(Box* box) {
    if (m_root->contains(box)) {
        insertRecursive(m_root, box, 0);
    }
}

void Octree::insertRecursive(OctreeNode* node, Box* box, int depth) {
    if (node->isLeaf) {
        node->objects.push_back(box);

        if (node->objects.size() > m_maxObjectsPerNode && depth < m_maxDepth) {
            subdivide(node);
        }
    } else {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]->contains(box)) {
                insertRecursive(node->children[i], box, depth + 1);
                return;
            }
        }
        node->objects.push_back(box);
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
    node->objects = remaining;
}

void Octree::clear() {
    delete m_root;
    m_root = new OctreeNode(glm::vec3(0.0f), 50.0f);
}

void Octree::rebuild(const std::vector<Box*>& boxes) {
    clear();
    for (Box* box : boxes) {
        insert(box);
    }
}

void Octree::query(const Frustum& frustum, std::vector<Box*>& result) const {
    queryRecursive(m_root, frustum, result);
}

void Octree::queryRecursive(OctreeNode* node, const Frustum& frustum, std::vector<Box*>& result) const {
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
                queryRecursive(node->children[i], frustum, result);
            }
        }
    }
}

void Octree::queryRange(const glm::vec3& center, float radius, std::vector<Box*>& result) const {
    queryRangeRecursive(m_root, center, radius, result);
}

void Octree::queryRangeRecursive(OctreeNode* node, const glm::vec3& center, float radius, std::vector<Box*>& result) const {
    float nodeRadius = node->halfSize * 1.732f;
    if (glm::distance(node->center, center) > radius + nodeRadius) {
        return;
    }

    for (Box* box : node->objects) {
        if (glm::distance(box->position, center) <= radius) {
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