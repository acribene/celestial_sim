#ifndef SIMULATION_H
#define SIMULATION_H

#include "body.h"
#include "../utils/vec.h"  
#include <vector>
#include "../utils/constants.h"

class Simulation
{
    private:
    std::vector<Body> m_bodies;
    double m_timeScale;
    
    public:
    Simulation();
    ~Simulation() = default;

    // Will not support copying of simulations
    Simulation(const Simulation& sim) = delete;
    Simulation(Simulation&& sim) = delete;

    void update(years_t deltaT);
    void addBody(const Body& body);
    void render();
    void reset();
    void generateRandomSystem(int count, bool centralMass = true );
};

#endif // SIMULATION_H
