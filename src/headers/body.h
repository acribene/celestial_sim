#ifndef BODY_H
#define BODY_H

#include <utility>
#include "raylib.h"
#include "../utils/Vec.h"
#include "../utils/constants.h"

// Celestial Body
class Simulation;

class Body {
    friend Simulation;
    private:
    double m_mass; // in Solar Masses (M☉)
    double m_radius;
    Vec2 m_position; // in Astronomical Units (AU)
    Vec2 m_velocity; // in AU per Year (AU/yr)
    Vec2 m_acceleration; // in AU/yr²
    Color m_color;
    
    public:
    
    Body(double mass = 0);
    Body(double mass, double radius, Vec2 position, Vec2 velocity, Color color);    
    Body(const Body& rhs);
    Body(Body&& rhs) noexcept;
    Body& operator=(const Body& rhs);
    Body& operator=(Body&& rhs) noexcept;
    ~Body() = default;

    // Leapfrog integration steps
    void kick(years_t dt); // update velocity by dt using current acceleration
    void drift(years_t dt); // update position by dt using current velocity
    void draw() const;
    void applyForce(Vec2 force);

    // Setters
    void setPos(Vec2 pos);
    void setVel(Vec2 vel);
    void setAcc(Vec2 acc);
    void setMass(double mass);

    // Getters
    Vec2 getPos() const;
    Vec2 getVel() const;
    Vec2 getAcc() const;
    double getMass() const;
};
#endif // BODY_H
