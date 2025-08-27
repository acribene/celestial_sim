#include "body.h"

Body::Body(double mass) : m_position(Vec2()), m_velocity(Vec2()), m_acceleration(Vec2()), m_mass(mass)
{}

Body::Body(const Body &rhs) : m_position(rhs.m_position), m_velocity(rhs.m_velocity), m_acceleration(rhs.m_acceleration), m_mass(rhs.m_mass) 
{}

Body::Body(Body &&rhs) noexcept : m_position(std::move(rhs.m_position)), m_velocity(std::move(rhs.m_velocity)), m_acceleration(std::move(rhs.m_acceleration)), m_mass(rhs.m_mass) 
{}

inline Body &Body::operator=(const Body &rhs)
{
    m_position = Vec2();
    m_velocity = Vec2();
    m_acceleration = Vec2();
    m_mass = 0;

    return *this;
}

inline Body &Body::operator=(Body &&rhs) noexcept
{
    m_position = Vec2();
    m_velocity = Vec2();
    m_acceleration = Vec2();
    m_mass = 0;

    return *this;
}

void Body::update(double dt)
{
    m_position += m_position * dt;
    m_velocity += m_velocity * dt;
    m_acceleration.zero();
}
