#include <utility>
#include "raylib.h"
#include "utils/Vec.h"

// Celestial Body

class Body {
    private:
    Vec2 m_position;
    Vec2 m_velocity;
    Vec2 m_acceleration;
    double m_mass;

    public:
    
    Body(double mass = 0);
    Body(const Body& rhs);
    Body(Body&& rhs) noexcept;
    Body& operator=(const Body& rhs);
    Body& operator=(Body&& rhs) noexcept;
    ~Body() = default;

    void update(double dt);
};
