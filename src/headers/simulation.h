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

    // Barnes-Hut approximation parameter (lower = more accurate) : 0 becomes brute force, .3 - .5 accurate, 1.0 max value for accuracy,
    // 1.5 if accuracy doesn't matter, any greater than that significant errors occur.
    double m_theta;

    size_t m_threadCount;            // Number of threads for parallelization
    ThreadPool m_threadPool;         // Thread pool for parallel calculations
    bool m_toggleWF;                 // A toggle for the wireframe rendering.

    public:
    Simulation( double theta = 0.5 );
    ~Simulation() = default;

    // Will not support copying of simulations
    Simulation(const Simulation& sim) = delete;
    Simulation(Simulation&& sim) = delete;
    Simulation& operator=(const Simulation& rhs) = delete;
    Simulation& operator=(Simulation&& rhs) = delete;

    void update(years_t deltaT);
    Body* addBody(Body body);
    void render();
    void reset();
    void generateProPlanetaryDisk(int count, Vec2 centerPoint = Vec2(0, 0), Vec2 velocity = Vec2(0, 0), bool centralMass = true);

    void DeleteBodyAt(Vec2 worldPos);
    Body* getBodyAt(Vec2 worldPos);

    //  TODO: Saving and loading simulation state.
    void saveState(const std::string& filename);
    void loadState(const std::string& filename);

    void setTheta(double theta);
    double getTheta() const { return m_theta; }
    void toggleWF() { m_toggleWF = !m_toggleWF; }
};

#endif // SIMULATION_H
