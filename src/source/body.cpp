#include "../headers/body.h"
#include "raylib.h"

Body::Body(double mass) : m_mass(mass), m_radius(0), m_position(Vec2()), m_velocity(Vec2()), m_acceleration(Vec2()),  m_color(WHITE)
{}

Body::Body(double mass, double radius, Vec2 position, Vec2 velocity, Color color) : m_mass(mass), m_radius(radius), m_position(position), m_velocity(velocity), m_acceleration(Vec2()), m_color(color)
{}

// Leapfrog: velocity half-step (kick)
void Body::kick(years_t dt) {
    m_velocity += m_acceleration * dt.count();
}

// Leapfrog: position full-step (drift)
void Body::drift(years_t dt) {
    m_position += m_velocity * dt.count();
}

void Body::draw() const
{
    // Convert AU to pixels for rendering
    double screenX = WINDOW_WIDTH / 2.0 + m_position.getX() * SCALE;
    double screenY = WINDOW_HEIGHT / 2.0 + m_position.getY() * SCALE;
    double screenRadius = m_radius * SCALE;
    
    if (screenRadius < 3.0) {
        screenRadius = 3.0;
    }
    
    // Draw the celestial body
    DrawCircle(screenX, screenY, screenRadius, m_color);
    //DrawCircle(screenX, screenY, screenRadius, WHITE);
}

void Body::applyForce(Vec2 force)
{
    m_acceleration += force / m_mass;
}

void Body::setPos(Vec2 pos)
{
    m_position = pos;
}

void Body::setVel(Vec2 vel)
{
    m_velocity = vel;
}

void Body::setAcc(Vec2 acc)
{
    m_acceleration = acc;
}

void Body::setMass(double mass)
{
    m_mass = mass;
}

Vec2 Body::getPos() const
{
    return m_position;
}

Vec2 Body::getVel() const
{
    return m_velocity;
}

Vec2 Body::getAcc() const
{
    return m_acceleration;
}

double Body::getMass() const
{
    return m_mass;
}
