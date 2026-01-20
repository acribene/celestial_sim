#ifndef QUADTREE_H
#define QUADTREE_H
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <limits>
#include "constants.h"
#include "Vec.h"
#include "raylib.h"

class Body;

// Represents a quadrant in 2D space
struct Quad {
    Vec2 center;
    double size;

    Quad() : center(Vec2(0, 0)), size(0) {}
    Quad(Vec2 center, double size) : center(center), size(size) {}

    // Create a quad containing all bodies
    static Quad newContaining(const std::vector<Body>& bodies);

    // Find which quadrant (0-3) a position belongs to
    // 0: bottom-left, 1: bottom-right, 2: top-left, 3: top-right
    size_t findQuadrant(Vec2 pos) const;

    // Get the quad for a specific quadrant
    Quad intoQuadrant(size_t quadrant) const;

    // Subdivide into 4 child quads
    std::array<Quad, 4> subdivide() const;
};

// Node in the quadtree
struct Node {
    size_t children;  // Index of first child (0 if leaf)
    size_t next;      // Index of next node at same level
    Vec2 pos;         // Center of mass position
    double mass;      // Total mass
    Quad quad;        // Spatial region

    Node(size_t next, Quad quad) 
        : children(0), next(next), pos(Vec2(0, 0)), mass(0.0), quad(quad) {}

    bool isLeaf() const { return children == 0; }
    bool isBranch() const { return children != 0; }
    bool isEmpty() const { return mass == 0.0; }
};

// Barnes-Hut Quadtree for efficient force calculation
class Quadtree {
private:
    double m_thetasq;    // Theta squared (accuracy parameter)
    double m_epsilonsq;    // Epsilon squared (softening parameter)
    std::vector<Node> m_nodes;
    std::vector<size_t> m_parents;

    static constexpr size_t m_root = 0;

    // Subdivide a node into 4 children
    size_t subdivide(size_t node);

public:
    Quadtree(double theta, double epsilon);

    // Clear the tree and set root quad
    void clear(Quad quad);

    // Reserve space to reduce allocations before inserting bodies
    void reserve(size_t bodyCount);

    // Insert a body into the tree
    void insert(Vec2 pos, double mass);

    // Propagate mass and center of mass up the tree
    void propagate();

    // Calculate acceleration at a position
    Vec2 acc(Vec2 pos) const;

    // Render the quadtree (for debugging)
    void render() const;
};

#endif // QUADTREE_H
