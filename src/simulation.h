#include "body.h"
#include "utils/vec.h"  
#include <vector>
#include "utils/constants.h"

class Simulation
{
    std::vector<Body> m_bodies;

    public:
    Simulation() = default;
    ~Simulation() = default;

    // Will not support copying of simulations
    Simulation(const Simulation& sim) = delete;
    Simulation(const Simulation&& sim) = delete;

    void step();
    void addBody(const Body& body);
    void render();
    void reset();
};
