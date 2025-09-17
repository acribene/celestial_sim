#include "../headers/simulation.h"

// Default ctor sets bodies to stl vector default and puts timescale at 1 (real time)
Simulation::Simulation() : m_bodies(std::vector<Body>()), m_timeScale(1.0)
{}

// Updates all the forces on all bodies in the simulation.
void Simulation::update(years_t deltaT)
{
    // Clear accelerations
    for (auto& body : m_bodies) {
        body.setAcc(Vec2(0, 0));
    }
    
    // Calculate gravitational forces between all body pairs
    for (size_t i = 0; i < m_bodies.size(); i++) {
        for (size_t j = i + 1; j < m_bodies.size(); j++) {
            Vec2 r = m_bodies[j].getPos() - m_bodies[i].getPos();
            double dist_squared = r.magSqrd() + SOFTENING;
            double dist = sqrt(dist_squared);
            
            // F = G * m1 * m2 / r^2
            // Since we're using AU-M☉-year units, GC = 4π²
            double force_magnitude = GC * m_bodies[i].getMass() * m_bodies[j].getMass() / dist_squared;
            Vec2 force_direction = r / dist;
            Vec2 force = force_direction * force_magnitude;
            
            // Apply equal and opposite forces
            m_bodies[i].applyForce(force);
            m_bodies[j].applyForce(force * -1);
        }
    }
    
    // Update positions and velocities
    years_t dt_years = std::chrono::duration_cast<years_t>(deltaT);
    for (auto& body : m_bodies) {
        body.update(dt_years);
    }
}

// Adds body to simulation
void Simulation::addBody(const Body &body)
{
    m_bodies.push_back(body);
}

void Simulation::render()
{
    // Renders bodies
    for (Body &body : m_bodies)
    {
        body.draw();
    }
}

// Removes all bodies from current simulation
void Simulation::reset()
{
    m_bodies.clear();
    m_timeScale = 1.0;
}
