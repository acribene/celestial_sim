#ifndef SIMULATION_H
#define SIMULATION_H

#include "body.h"
#include "../utils/vec.h"  
#include <vector>
#include "../utils/constants.h"
#include "../utils/QuadTree.h"
#include "ThreadPool.h"

class Simulation
{
    private:
    std::vector<Body> m_bodies;      // Collection of all celestial bodies in the simulation
    double m_timeScale;              // Time scaling factor for simulation speed
    Quadtree m_quadtree;             // Barnes-Hut quadtree for efficient force calculations
    double m_theta;                  // Barnes-Hut approximation parameter (lower = more accurate)
    size_t m_threadCount;            // Number of threads for parallelization
    ThreadPool m_threadPool;         // Thread pool for parallel acceleration calculations

    public:
    Simulation(double theta = 0.5);
    ~Simulation() = default;

    // Will not support copying of simulations
    Simulation(const Simulation& sim) = delete;
    Simulation(Simulation&& sim) = delete;
    Simulation& operator=(const Simulation& rhs) = delete;
    Simulation& operator=(Simulation&& rhs) = delete;

    void update(years_t deltaT);
    void addBody(Body body);
    void render();
    void reset();
    void generateRandomSystem(int count, bool centralMass = true );

    //  TODO: Saving and loading simulation state.
    void saveState(const std::string& filename);
    void loadState(const std::string& filename);

    // Theta parameter management
    void setTheta(double theta);
    double getTheta() const { return m_theta; }
};

#endif // SIMULATION_H
