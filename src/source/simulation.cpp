#include "../headers/simulation.h"
#include "raylib.h"
#include <cmath>
#include <random>

// Default ctor sets bodies to stl vector default and puts timescale at 1 (real time)
// Initialize quadtree with theta (default 0.5) and epsilon from constants
Simulation::Simulation(double theta) 
    : m_bodies(std::vector<Body>()), 
      m_timeScale(1.0),
      m_quadtree(Quadtree(theta, SOFTENING)),
      m_theta(theta)
{}

// Updates all the forces on all bodies in the simulation using Barnes-Hut algorithm
void Simulation::update(years_t deltaT)
{
    if (m_bodies.empty()) return;

    // Leapfrog (kick-drift-kick) integrator with Barnes-Hut force calculation
    years_t half_dt = deltaT / 2.0;

    // 1. Kick: update velocity by half-step using current acceleration
    for (auto& body : m_bodies) {
        body.kick(half_dt);
    }

    // 2. Drift: update position by full-step using new velocity
    for (auto& body : m_bodies) {
        body.drift(deltaT);
    }

    // 3. Build Barnes-Hut quadtree and compute accelerations
    
    // Create quad containing all bodies
    Quad boundingQuad = Quad::newContaining(m_bodies);
    m_quadtree.clear(boundingQuad);
    
    // Insert all bodies into quadtree
    for (const auto& body : m_bodies) {
        m_quadtree.insert(body.getPos(), body.getMass());
    }
    
    // Propagate mass and center of mass up the tree
    m_quadtree.propagate();
    
    // Calculate accelerations using quadtree
    for (auto& body : m_bodies) {
        body.setAcc(Vec2(0, 0));
        Vec2 acceleration = m_quadtree.acc(body.getPos());
        body.setAcc(acceleration);
    }

    // 4. Kick: update velocity by another half-step using new acceleration
    for (auto& body : m_bodies) {
        body.kick(half_dt);
    }
}

// Adds body to simulation
void Simulation::addBody(Body body)
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
}

// Removes all bodies from current simulation
void Simulation::reset()
{
    m_bodies.clear();
    m_timeScale = 1.0;
}

// Set the theta parameter for Barnes-Hut algorithm
// Lower values = more accurate but slower (typical range: 0.3 - 1.0)
void Simulation::setTheta(double theta)
{
    m_theta = theta;
    // Reinitialize quadtree with new theta
    m_quadtree = Quadtree(m_theta, SOFTENING);
}

// Generates a "random" system of bodies
void Simulation::generateRandomSystem(int count, bool centralMass)
{
    // Clear existing bodies
    reset();
    
    // Random number generation setup
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (centralMass && count > 0) {
        // Add a central star (like our Sun)
        Body star(1.0, 0.05, Vec2(0, 0), Vec2(0, 0), YELLOW);
        addBody(star);
        count--;
    }
    
    if (count <= 0) return;
    
    // Distributions for random properties
    std::uniform_real_distribution<> massDist(-7, -3); // Log scale: 10^-7 to 10^-3 solar masses
    std::uniform_real_distribution<> distDist(0.3, 5.0); // Distance from center in AU
    std::uniform_real_distribution<> angleDist(0, 2 * M_PI); // Random angle
    std::uniform_real_distribution<> eccDist(0, 0.2); // Orbital eccentricity
    //std::uniform_real_distribution<> colorDist(0, 255); // Color components
    
    for (int i = 0; i < count; i++) {
        // Generate random mass (log scale)
        double mass = std::pow(10, massDist(gen));
        
        // Generate random distance from center with more probability for inner orbits
        double distance = distDist(gen);
        if (i < count / 2) {
            // First half of bodies are more likely to be closer
            distance *= 0.5;
        }
        
        // Random angle
        double angle = angleDist(gen);
        
        // Calculate position
        Vec2 position(std::cos(angle) * distance, std::sin(angle) * distance);
        
        // Calculate velocity for stable orbit with some randomness
        double centralMass = centralMass ? 1.0 : (i > 0 ? m_bodies[0].getMass() : 1.0);
        double circularVelocity = std::sqrt(GC * centralMass / distance);
        
        // Add some eccentricity
        double eccentricity = eccDist(gen);
        double velocityMag = circularVelocity * (1.0 - eccentricity);
        
        // Velocity is perpendicular to position vector with some random direction
        Vec2 velocityDir(-std::sin(angle), std::cos(angle));
        if (gen() % 2 == 0) velocityDir = velocityDir * -1; // Random direction
        
        Vec2 velocity = velocityDir * velocityMag;
        
        // Calculate reasonable radius for rendering (proportional to mass^(1/3))
        double radius = 0.01 * std::pow(mass * 1e6, 1.0/3.0); // Scale to make visible
        
        // Generate random color
        // Color color = {
        //     static_cast<unsigned char>(colorDist(gen)),
        //     static_cast<unsigned char>(colorDist(gen)),
        //     static_cast<unsigned char>(colorDist(gen)),
        //     255
        // };
        
        // Create and add the body
        Body body(mass, radius, position, velocity, WHITE);
        addBody(body);
    }    
}
