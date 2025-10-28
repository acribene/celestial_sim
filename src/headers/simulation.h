#ifndef SIMULATION_H
#define SIMULATION_H

#include "body.h"
#include "../utils/vec.h"  
#include <vector>
#include "../utils/constants.h"
#include "../utils/QuadTree.h"

class Simulation
{
    private:
    std::vector<Body> m_bodies;
    double m_timeScale;
    Quadtree m_quadtree;
    double m_theta;
    
    public:
    Simulation(double theta = 1.0);
    ~Simulation() = default;

    // Will not support copying of simulations
    Simulation(const Simulation& sim) = delete;
    Simulation(Simulation&& sim) = delete;

    void update(years_t deltaT);
    void addBody(Body body);
    void render();
    void reset();
    void generateRandomSystem(int count, bool centralMass = true );
    
    // Theta parameter management
    void setTheta(double theta);
    double getTheta() const { return m_theta; }
};

#endif // SIMULATION_H
