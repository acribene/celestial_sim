#include "body.h"

Body::Body(double mass) : m_position(Vec2()), m_velocity(Vec2()), m_acceleration(Vec2()), m_mass(mass)
{}

Body::Body(const Body &rhs) : m_position(rhs.m_position), m_velocity(rhs.m_velocity), m_acceleration(rhs.m_acceleration), m_mass(rhs.m_mass) 
{}

Body::Body(Body &&rhs) noexcept : m_position(std::move(rhs.m_position)), m_velocity(std::move(rhs.m_velocity)), m_acceleration(std::move(rhs.m_acceleration)), m_mass(rhs.m_mass) 
{}

inline Body &Body::operator=(const Body &rhs)
{
    m_position = rhs.m_position;
    m_velocity = rhs.m_velocity;
    m_acceleration = rhs.m_acceleration;
    m_mass = rhs.m_mass;

    return *this;
}

inline Body &Body::operator=(Body &&rhs) noexcept
{
    m_position = rhs.m_position;
    m_velocity = rhs.m_velocity;
    m_acceleration = rhs.m_acceleration;
    m_mass = rhs.m_mass;

    return *this;
}

void Body::update(years_t dt)
{
    m_position += m_position * dt.count();
    m_velocity += m_velocity * dt.count();
    m_acceleration.zero();
}

void Body::draw() const
{
    // Convert AU to pixels for rendering
    double screenX = WINDOW_WIDTH / 2.0 + m_position.getX() * SCALE;
    double screenY = WINDOW_HEIGHT / 2.0 + m_position.getY() * SCALE;
    double screenRadius = m_radius * SCALE;
    
    // Ensure the body is at least 2 pixels visible
    if (screenRadius < 2.0) {
        screenRadius = 2.0;
    }
    
    // Draw the celestial body
    DrawCircle(screenX, screenY, screenRadius, m_color);
}

void Body::applyForce(Vec2 force)
{
    m_acceleration += force / m_mass;
}
