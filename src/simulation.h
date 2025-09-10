#ifndef SIMULATION_H
#define SIMULATION_H

#include "body.h"
#include "utils/vec.h"  
#include <vector>
#include "utils/constants.h"

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
    Simulation(const Simulation&& sim) = delete;

    void update(seconds_t deltaT);
    void addBody(const Body& body);
    void render();
    void reset();
};

#endif // SIMULATION_H
