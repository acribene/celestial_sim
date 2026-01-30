#ifndef BODY_H
#define BODY_H

#include <utility>
#include <fstream>
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
    Body(const Body& other) = default;
    Body(Body&& other) noexcept = default;
    Body& operator=(const Body& rhs) = default;
    Body& operator=(Body&& rhs) noexcept = default;
    ~Body() = default;

    // Leapfrog integration steps
    void kick(years_t dt); // update velocity by dt using current acceleration
    void drift(years_t dt); // update position by dt using current velocity
    void draw() const;
    void drawTrail() const; // TODO: Implement trail drawing. May or may not be const.
    void applyForce(Vec2 force); // TODO: Might not be needed.

    //  TODO: Saving and loading body state.
    void saveState(std::ofstream& file) const;
    void loadState(std::ifstream& file);
    
    // Setters
    void setPos(Vec2 pos);
    void setVel(Vec2 vel);
    void setAcc(Vec2 acc);
    void setMass(double mass);
    void setRadius(double radius);

    // Getters
    Vec2 getPos() const;
    Vec2 getVel() const;
    Vec2 getAcc() const;
    double getMass() const;
    double getRadius() const;

    // For debugging
    friend std::ostream& operator<<(std::ostream& os, const Body& body) {
        os << "Body(Mass: " << body.m_mass << " M☉, Radius: " << body.m_radius << " AU, Position: (" 
           << body.m_position.getX() << ", " << body.m_position.getY() << ") AU, Velocity: ("
           << body.m_velocity.getX() << ", " << body.m_velocity.getY() << ") AU/yr)";
        return os;
    }
};
#endif // BODY_H
