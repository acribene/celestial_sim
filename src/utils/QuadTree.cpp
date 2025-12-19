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

    // Add some padding and clamp to a minimum so degenerate cases do not spin subdivides
    size = std::max(size * 1.1, 1e-6);

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
    : m_thetasq(theta * theta), m_epsilonsq(epsilon * epsilon) {
}

void Quadtree::reserve(size_t bodyCount) {
    // Barnes-Hut produces up to ~4n nodes; reserve to limit per-frame allocations
    size_t expectedNodes = bodyCount > 0 ? bodyCount * 4 + 1 : 0;
    if (expectedNodes > m_nodes.capacity()) {
        m_nodes.reserve(expectedNodes);
    }

    if (bodyCount > m_parents.capacity()) {
        m_parents.reserve(bodyCount);
    }
}

void Quadtree::clear(Quad quad) {
    m_nodes.clear();
    m_parents.clear();
    m_nodes.push_back(Node(0, quad));
}

size_t Quadtree::subdivide(size_t node) {
    m_parents.push_back(node);
    size_t children = m_nodes.size();
    m_nodes[node].children = children;

    std::array<size_t, 4> nexts = {
        children + 1,
        children + 2,
        children + 3,
        m_nodes[node].next
    };
    
    std::array<Quad, 4> quads = m_nodes[node].quad.subdivide();
    
    for (size_t i = 0; i < 4; i++) {
        m_nodes.push_back(Node(nexts[i], quads[i]));
    }

    return children;
}

void Quadtree::insert(Vec2 pos, double mass) {
    size_t node = m_root;

    // Navigate to appropriate leaf
    while (m_nodes[node].isBranch()) {
        size_t quadrant = m_nodes[node].quad.findQuadrant(pos);
        node = m_nodes[node].children + quadrant;
    }

    // If leaf is empty, just insert here
    if (m_nodes[node].isEmpty()) {
        m_nodes[node].pos = pos;
        m_nodes[node].mass = mass;
        return;
    }

    // If same position, add mass
    Vec2 existingPos = m_nodes[node].pos;
    double existingMass = m_nodes[node].mass;
    
    if (pos.getX() == existingPos.getX() && pos.getY() == existingPos.getY()) {
        m_nodes[node].mass += mass;
        return;
    }

    // Need to subdivide
    while (true) {
        size_t children = subdivide(node);

        size_t q1 = m_nodes[node].quad.findQuadrant(existingPos);
        size_t q2 = m_nodes[node].quad.findQuadrant(pos);

        if (q1 == q2) {
            // Both go in same quadrant, continue subdividing
            node = children + q1;
        } else {
            // Different quadrants, insert both
            size_t n1 = children + q1;
            size_t n2 = children + q2;

            m_nodes[n1].pos = existingPos;
            m_nodes[n1].mass = existingMass;
            m_nodes[n2].pos = pos;
            m_nodes[n2].mass = mass;
            return;
        }
    }
}

void Quadtree::propagate() {
    // Iterate through m_parents in reverse order (bottom-up)
    for (auto it = m_parents.rbegin(); it != m_parents.rend(); ++it) {
        size_t node = *it;
        size_t i = m_nodes[node].children;

        // Calculate total mass
        m_nodes[node].mass = m_nodes[i].mass 
                         + m_nodes[i + 1].mass 
                         + m_nodes[i + 2].mass 
                         + m_nodes[i + 3].mass;

        // Calculate weighted center of mass
        if (m_nodes[node].mass > 0) {
            m_nodes[node].pos = Vec2(
                (m_nodes[i].pos.getX() * m_nodes[i].mass +
                 m_nodes[i + 1].pos.getX() * m_nodes[i + 1].mass +
                 m_nodes[i + 2].pos.getX() * m_nodes[i + 2].mass +
                 m_nodes[i + 3].pos.getX() * m_nodes[i + 3].mass) / m_nodes[node].mass,
                (m_nodes[i].pos.getY() * m_nodes[i].mass +
                 m_nodes[i + 1].pos.getY() * m_nodes[i + 1].mass +
                 m_nodes[i + 2].pos.getY() * m_nodes[i + 2].mass +
                 m_nodes[i + 3].pos.getY() * m_nodes[i + 3].mass) / m_nodes[node].mass
            );
        }
    }
}

Vec2 Quadtree::acc(Vec2 pos) const {
    Vec2 acceleration(0, 0);

    size_t node = m_root;
    while (true) {
        const Node& n = m_nodes[node];

        if (n.mass == 0.0) {
            if (n.next == 0) {
                break;
            }
            node = n.next;
            continue;
        }

        Vec2 d = Vec2(
            n.pos.getX() - pos.getX(),
            n.pos.getY() - pos.getY()
        );
        double d_sq = d.magSqrd();

        // Check if we can treat this node as a single body (leaf or far enough)
        if (n.isLeaf() || n.quad.size * n.quad.size < d_sq * m_thetasq) {
            // Skip if this is the body itself (d_sq very small)
            if (d_sq > m_epsilonsq) {  // Only calculate if not too close
                double denom = (d_sq + m_epsilonsq) * std::sqrt(d_sq + m_epsilonsq);
                
                if (denom > 0) {
                    double forceMag = GC * n.mass / denom;
                    forceMag = std::min(forceMag, 1e10);
                    acceleration += d * forceMag;
                }
            }

            if (n.next == 0) {
                break;
            }
            node = n.next;
        } else {
            node = n.children;
        }
    }

    return acceleration;
}

// Renders the quadtree wireframe.
void Quadtree::render() const
{
    for (const auto& node : m_nodes) {
        if( !node.isEmpty() ) {
            // Convert from world space (AU) to screen space (pixels)
            // Same transformation as Body::draw()
            float screenX = WINDOW_WIDTH / 2.0f + static_cast<float>(node.quad.center.getX() * SCALE);
            float screenY = WINDOW_HEIGHT / 2.0f + static_cast<float>(node.quad.center.getY() * SCALE);
            float screenSize = static_cast<float>(node.quad.size * SCALE);
            
            DrawRectangleLinesEx(
                Rectangle{
                    screenX - screenSize / 2.0f,
                    screenY - screenSize / 2.0f,
                    screenSize,
                    screenSize
                },
                2.0f,
                RED
            );
        }
    }
}
