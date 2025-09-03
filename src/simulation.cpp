#include "simulation.h"

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
