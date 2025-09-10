#include <utility>
#include "raylib.h"
#include "../utils/Vec.h"
#include "../utils/constants.h"

// Celestial Body
class Simulation;

class Body {
    friend Simulation;
    private:
    Vec2 m_position; // in Astronomical Units (AU)
    Vec2 m_velocity; // in AU per Year (AU/yr)
    Vec2 m_acceleration; // in AU/yr²
    double m_mass; // in Solar Masses (M☉)
    Color m_color;
    double m_radius;
    
    public:
    
    Body(double mass = 0);
    Body(const Body& rhs);
    Body(Body&& rhs) noexcept;
    Body& operator=(const Body& rhs);
    Body& operator=(Body&& rhs) noexcept;
    ~Body() = default;

    void update(years_t dt);
    void draw() const;
    void applyForce(Vec2 force);
};
