#include "QuadTree.h"
#include "../headers/body.h"

// Quad implementation
Quad Quad::newContaining(const std::vector<Body>& bodies) {
    if (bodies.empty()) {
        return Quad(Vec2(0, 0), 10.0); // Default size if no bodies
    }

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double max_y = std::numeric_limits<double>::lowest();

    for (const auto& body : bodies) {
        Vec2 pos = body.getPos();
        min_x = std::min(min_x, pos.getX());
        min_y = std::min(min_y, pos.getY());
        max_x = std::max(max_x, pos.getX());
        max_y = std::max(max_y, pos.getY());
    }

    Vec2 center((min_x + max_x) * 0.5, (min_y + max_y) * 0.5);
    double size = std::max(max_x - min_x, max_y - min_y);
    
    // Add some padding to ensure all bodies fit comfortably
    size *= 1.1;

    return Quad(center, size);
}

size_t Quad::findQuadrant(Vec2 pos) const {
    return ((pos.getY() > center.getY()) ? 2 : 0) | ((pos.getX() > center.getX()) ? 1 : 0);
}

Quad Quad::intoQuadrant(size_t quadrant) const {
    Quad result = *this;
    result.size *= 0.5;
    result.center = Vec2(
        center.getX() + ((quadrant & 1) ? 0.5 : -0.5) * result.size,
        center.getY() + ((quadrant & 2) ? 0.5 : -0.5) * result.size
    );
    return result;
}

std::array<Quad, 4> Quad::subdivide() const {
    return {
        intoQuadrant(0),
        intoQuadrant(1),
        intoQuadrant(2),
        intoQuadrant(3)
    };
}

// Quadtree implementation
Quadtree::Quadtree(double theta, double epsilon) 
    : t_sq(theta * theta), e_sq(epsilon * epsilon) {
}

void Quadtree::clear(Quad quad) {
    nodes.clear();
    parents.clear();
    nodes.push_back(Node(0, quad));
}

size_t Quadtree::subdivide(size_t node) {
    parents.push_back(node);
    size_t children = nodes.size();
    nodes[node].children = children;

    std::array<size_t, 4> nexts = {
        children + 1,
        children + 2,
        children + 3,
        nodes[node].next
    };
    
    std::array<Quad, 4> quads = nodes[node].quad.subdivide();
    
    for (size_t i = 0; i < 4; i++) {
        nodes.push_back(Node(nexts[i], quads[i]));
    }

    return children;
}

void Quadtree::insert(Vec2 pos, double mass) {
    size_t node = ROOT;

    // Navigate to appropriate leaf
    while (nodes[node].isBranch()) {
        size_t quadrant = nodes[node].quad.findQuadrant(pos);
        node = nodes[node].children + quadrant;
    }

    // If leaf is empty, just insert here
    if (nodes[node].isEmpty()) {
        nodes[node].pos = pos;
        nodes[node].mass = mass;
        return;
    }

    // If same position, add mass
    Vec2 existingPos = nodes[node].pos;
    double existingMass = nodes[node].mass;
    
    if (pos.getX() == existingPos.getX() && pos.getY() == existingPos.getY()) {
        nodes[node].mass += mass;
        return;
    }

    // Need to subdivide
    while (true) {
        size_t children = subdivide(node);

        size_t q1 = nodes[node].quad.findQuadrant(existingPos);
        size_t q2 = nodes[node].quad.findQuadrant(pos);

        if (q1 == q2) {
            // Both go in same quadrant, continue subdividing
            node = children + q1;
        } else {
            // Different quadrants, insert both
            size_t n1 = children + q1;
            size_t n2 = children + q2;

            nodes[n1].pos = existingPos;
            nodes[n1].mass = existingMass;
            nodes[n2].pos = pos;
            nodes[n2].mass = mass;
            return;
        }
    }
}

void Quadtree::propagate() {
    // Iterate through parents in reverse order (bottom-up)
    for (auto it = parents.rbegin(); it != parents.rend(); ++it) {
        size_t node = *it;
        size_t i = nodes[node].children;

        // Calculate total mass
        nodes[node].mass = nodes[i].mass 
                         + nodes[i + 1].mass 
                         + nodes[i + 2].mass 
                         + nodes[i + 3].mass;

        // Calculate weighted center of mass
        if (nodes[node].mass > 0) {
            nodes[node].pos = Vec2(
                (nodes[i].pos.getX() * nodes[i].mass +
                 nodes[i + 1].pos.getX() * nodes[i + 1].mass +
                 nodes[i + 2].pos.getX() * nodes[i + 2].mass +
                 nodes[i + 3].pos.getX() * nodes[i + 3].mass) / nodes[node].mass,
                (nodes[i].pos.getY() * nodes[i].mass +
                 nodes[i + 1].pos.getY() * nodes[i + 1].mass +
                 nodes[i + 2].pos.getY() * nodes[i + 2].mass +
                 nodes[i + 3].pos.getY() * nodes[i + 3].mass) / nodes[node].mass
            );
        }
    }
}

Vec2 Quadtree::acc(Vec2 pos) const {
    Vec2 acceleration(0, 0);

    size_t node = ROOT;
    while (true) {
        const Node& n = nodes[node];

        Vec2 d = Vec2(
            n.pos.getX() - pos.getX(),
            n.pos.getY() - pos.getY()
        );
        double d_sq = d.magSqrd();

        // Check if we can treat this node as a single body (leaf or far enough)
        if (n.isLeaf() || n.quad.size * n.quad.size < d_sq * t_sq) {
            // Calculate gravitational acceleration: a = GM/r^2 * r_hat
            double denom = (d_sq + e_sq) * std::sqrt(d_sq + e_sq);
            
            if (denom > 0) {
                double forceMag = GC * n.mass / denom;
                // Clamp to prevent extremely large forces
                forceMag = std::min(forceMag, 1e10);
                acceleration += d * forceMag;
            }

            // Move to next node
            if (n.next == 0) {
                break;
            }
            node = n.next;
        } else {
            // Need to recurse into children
            node = n.children;
        }
    }

    return acceleration;
}
