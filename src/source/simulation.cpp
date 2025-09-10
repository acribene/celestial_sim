#include "../headers/simulation.h"

// Default ctor sets bodies to stl vector default and puts timescale at 1 (real time)
Simulation::Simulation() : m_bodies(std::vector<Body>()), m_timeScale(1.0)
{}

// Updates all the forces on all bodies in the simulation.
void Simulation::update(years_t deltaT)
{
    for (int i = 0; i < m_bodies.size(); ++i)
    {
        Body& body1 = m_bodies[i];
        for(int j = i + 1; j < m_bodies.size(); ++j)
        {
            Body& body2 = m_bodies[j];

            Vec2 delta = body2.m_position - body1.m_position;
            double distanceSqrd = delta.magSqrd();
            
            // Softens distance to keep crazy forces when distances get close 
            double softeningDistanceSqrd = distanceSqrd + SOFTENING * SOFTENING;

            double forceMag = (GC * body1.m_mass * body2.m_mass) / softeningDistanceSqrd;

            // Get unit vector for directon of force (not softened one)
            Vec2 forceDir = delta.normalized();
            // Apply forces to both bodies
            body1.applyForce(forceDir * forceMag);
            body2.applyForce(forceDir * -forceMag);
        }
    }

    for (Body& body : m_bodies)
    {
        body.update(deltaT);
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

    // Possibly additional info.
}

// Removes all bodies from current simulation
void Simulation::reset()
{
    m_bodies.clear();
    m_timeScale = 1.0;
}
