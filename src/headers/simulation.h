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
    
    // Pre-allocated buffers for O(N) allocation-free spatial hashing
    std::vector<int> m_hashHead;
    std::vector<int> m_hashNext;

    // Barnes-Hut approximation parameter (lower = more accurate) : 0 becomes brute force, .3 - .5 accurate, 1.0 max value for accuracy,
    // 1.5 if accuracy doesn't matter, any greater than that significant errors occur.
    double m_theta;

    size_t m_threadCount;            // Number of threads for parallelization
    ThreadPool m_threadPool;         // Thread pool for parallel calculations
    bool m_toggleWF;                 // A toggle for the wireframe rendering.

    // For energy logging
    // std::ofstream m_energyLog;
    // double m_totalTime = 0.0;
    // double m_timeSinceLastLog = 0.0;
    // double m_totalHeatEnergy = 0.0;
    
    // // 0.1 means it logs once every 0.1 simulated years.
    // const double LOG_INTERVAL = .1;

    public:
    Simulation( double theta = 0.5 );
    ~Simulation() = default;

    // Will not support copying of simulations
    Simulation(const Simulation& sim) = delete;
    Simulation(Simulation&& sim) = delete;
    Simulation& operator=(const Simulation& rhs) = delete;
    Simulation& operator=(Simulation&& rhs) = delete;

    // Methods for collisions
    void handleCollisions();
    void resolveCollision(Body& b1, Body& b2, double restitution = 0.5);
    double calculateTotalEnergy() const;

    // General Physics
    void update(years_t deltaT, bool enableCollisions = true);
    Body* addBody(Body body);
    void render();
    void reset();
    void generateProPlanetaryDisk(int count, Vec2 centerPoint = Vec2(0, 0), Vec2 velocity = Vec2(0, 0), bool centralMass = true);

    void DeleteBodyAt(Vec2 worldPos);
    Body* getBodyAt(Vec2 worldPos);

    // Saving and loading simulation state.
    void saveSimulation(const std::string& filename);
    void loadSimulation(const std::string& filename);
    void loadPreset(int preset, int numBodies = -1);

    void setTheta(double theta);
    double getTheta() const { return m_theta; }
    void toggleWF() { m_toggleWF = !m_toggleWF; }
    Quadtree& getQuadtree() { return m_quadtree; }
};

#endif // SIMULATION_H
