#include "simulation.h"

// Default ctor sets bodies to stl vector default and puts timescale at 1 (real time)
Simulation::Simulation() : m_bodies(std::vector<Body>()), m_timeScale(1.0)
{}

// Updates all the forces on all bodies in the simulation.
void Simulation::step()
{
    for (Body i : m_bodies)
    {
        Vec2 pos1 = i.m_position;
        double mass1 = i.m_mass;
        for(auto j = m_bodies.begin() + 1; j !=  m_bodies.end(); ++j)
        {
            Vec2 pos2 = j->m_position;
            double mass2 = j->m_mass;

        }
    }
}

// Adds body to simulation
void Simulation::addBody(const Body &body)
{
    m_bodies.push_back(body);
}

// Removes all bodies from current simulation
void Simulation::reset()
{
    m_bodies.clear();
    m_timeScale = 1.0;
}
