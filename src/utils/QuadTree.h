#ifndef QUADTREE_H
#define QUADTREE_H
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include "constants.h"
#include "Vec.h"


/**
 * @brief Axis-Aligned Bounding Box (AABB) for 2D space.
 *
 * Represents a rectangle whose sides are always parallel to the axes.
 * Used for fast spatial queries and collision checks.
 */
struct AABB {
    float x, y, w, h;

    /**
     * @brief Check if a point is inside this AABB.
     * @param px X coordinate of the point
     * @param py Y coordinate of the point
     * @return true if the point is inside the box
     */
    bool contains(float px, float py) const {
        // Check if px, py is within the bounds of this box
        return px >= x && px < x + w && py >= y && py < y + h;
    }

    /**
     * @brief Check if this AABB intersects with another AABB.
     * @param other The other AABB
     * @return true if the two boxes overlap
     */
    bool intersects(const AABB& other) const {
        // If one box is to the left/right or above/below the other, they do not intersect
        return !(x + w < other.x || x > other.x + other.w ||
                 y + h < other.y || y > other.y + other.h);
    }
};

/**
 * @brief QuadTree for N-body simulations.
 *
 * This QuadTree is optimized for spatial partitioning and force calculations
 * using the Barnes-Hut approximation.
 */
template<typename T>
class QuadTree {
public:
    /**
     * @brief Body representation in the QuadTree.
     *
     * Contains position and mass of an astronomical body.
     */
    struct Body {
        Vec2 position; ///< Position of the body
        double mass;   ///< Mass of the body
    };

    /**
     * @brief Construct a QuadTree covering a given region.
     * @param boundary The region covered by the root node
     * @param capacity Max bodies per node before subdivision
     * @param theta Opening angle parameter for Barnes-Hut approximation
     */
    QuadTree(const AABB& boundary, int capacity = 4, double theta = 0.5)
        : m_boundary(boundary), m_capacity(capacity), m_theta(theta) {
        m_nodes.reserve(1024); // Pre-allocate space for nodes
        m_nodes.push_back(Node{boundary});
    }

    /**
     * @brief Remove all bodies from the QuadTree.
     * Resets to a single root node.
     */
    void clear() {
        m_nodes.clear();
        m_nodes.push_back(Node{m_boundary});
    }

    /**
     * @brief Insert a body into the QuadTree.
     * @param position Position of the body
     * @param mass Mass of the body
     */
    void insert(const Vec2& position, double mass) {
        insert(0, position, mass);
    }

    /**
     * @brief Compute the force on a body at a given position.
     * @param position Position of the body
     * @param softening Softening parameter to prevent singularities
     * @return Gravitational force vector
     */
    Vec2 computeForce(const Vec2& position, double softening = 1e-10) const {
        return computeForce(0, position, softening);
    }

private:
    /**
     * @brief Node in the QuadTree.
     * Each node covers a region and may contain a body or be subdivided into 4 children.
     */
    struct Node {
        AABB boundary; ///< Region covered by this node
        Vec2 centerOfMass = {0, 0}; ///< Center of mass of all bodies in this node
        double totalMass = 0; ///< Total mass of all bodies in this node
        std::array<int, 4> children = {-1, -1, -1, -1}; ///< Indices of child nodes
        bool isLeaf = true; ///< True if this node is a leaf
        Body body; ///< Body stored in this node (valid only if isLeaf)
    };

    std::vector<Node> m_nodes; ///< All nodes in the tree
    AABB m_boundary; ///< Boundary of the root node
    int m_capacity; ///< Max bodies per node before subdivision
    double m_theta; ///< Opening angle parameter for Barnes-Hut approximation

    /**
     * @brief Recursive insert helper.
     * @param idx Index of current node
     * @param position Position of the body
     * @param mass Mass of the body
     */
    void insert(int idx, const Vec2& position, double mass) {
        Node& node = m_nodes[idx];

        if (node.isLeaf) {
            if (node.totalMass == 0) {
                // Empty leaf node, store the body here
                node.body = {position, mass};
                node.totalMass = mass;
                node.centerOfMass = position;
                return;
            } else {
                // Subdivide the node and reinsert the existing body
                subdivide(idx);
                insertIntoChildren(idx, node.body.position, node.body.mass);
                node.isLeaf = false;
            }
        }

        // Insert the new body into the appropriate child
        insertIntoChildren(idx, position, mass);

        // Update the center of mass and total mass
        node.totalMass += mass;
        node.centerOfMass = (node.centerOfMass * (node.totalMass - mass) + position * mass) / node.totalMass;
    }

    /**
     * @brief Insert a body into the correct child node.
     * @param idx Index of current node
     * @param position Position of the body
     * @param mass Mass of the body
     */
    void insertIntoChildren(int idx, const Vec2& position, double mass) {
        Node& node = m_nodes[idx];
        for (int i = 0; i < 4; ++i) {
            int childIdx = node.children[i];
            if (childIdx == -1) continue;
            if (m_nodes[childIdx].boundary.contains(position.getX(), position.getY())) {
                insert(childIdx, position, mass);
                return;
            }
        }
    }

    /**
     * @brief Subdivide a node into 4 children (NW, NE, SW, SE).
     * @param idx Index of the node to subdivide
     */
    void subdivide(int idx) {
        Node& node = m_nodes[idx];
        float x = node.boundary.x, y = node.boundary.y;
        float hw = node.boundary.w / 2.0f, hh = node.boundary.h / 2.0f;
        for (int i = 0; i < 4; ++i) {
            float nx = x + (i & 1) * hw;
            float ny = y + (i >> 1) * hh;
            AABB childBoundary{nx, ny, hw, hh};
            m_nodes.push_back(Node{childBoundary});
            node.children[i] = m_nodes.size() - 1;
        }
    }

    /**
     * @brief Recursive force computation helper.
     * @param idx Index of current node
     * @param position Position of the body
     * @param softening Softening parameter to prevent singularities
     * @return Gravitational force vector
     */
    Vec2 computeForce(int idx, const Vec2& position, double softening) const {
        const Node& node = m_nodes[idx];
        if (node.totalMass == 0) return {0, 0};

        Vec2 r = node.centerOfMass - position;
        double distSq = r.magSqrd() + softening;
        double dist = std::sqrt(distSq);

        if (node.isLeaf || (node.boundary.w / dist) < m_theta) {
            // Treat as a single body
            double forceMag = GC * node.totalMass / distSq;
            return r.normalized() * forceMag;
        }

        // Otherwise, recurse into children
        Vec2 force = {0, 0};
        for (int i = 0; i < 4; ++i) {
            int childIdx = node.children[i];
            if (childIdx != -1) {
                force += computeForce(childIdx, position, softening);
            }
        }
        return force;
    }
};

#endif // QUADTREE_H
