#include "../headers/simulation.h"
#include "raylib.h"
#include <cmath>
#include <random>
#include <thread>

// Default ctor sets bodies to stl vector default and puts timescale at 1 (real time)
// Initialize quadtree with theta (default 0.5) and epsilon from constants
Simulation::Simulation(double theta) 
    : m_bodies(std::vector<Body>()), 
      m_timeScale(1.0),
      m_quadtree(Quadtree(theta, SOFTENING)),
      m_theta(theta),
      m_threadCount(std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4),
      m_threadPool(m_threadCount)
{
}

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
    
    // Calculate accelerations using quadtree with thread pool
    size_t bodiesPerThread = (m_bodies.size() + m_threadCount - 1) / m_threadCount;
    
    for (size_t t = 0; t < m_threadCount; ++t) {
        size_t start = t * bodiesPerThread;
        size_t end = std::min(start + bodiesPerThread, m_bodies.size());
        
        if (start >= m_bodies.size()) break;
        
        m_threadPool.enqueue([this, start, end]() {
            for (size_t i = start; i < end; ++i) {
                m_bodies[i].setAcc(Vec2(0, 0));
                Vec2 acceleration = m_quadtree.acc(m_bodies[i].getPos());
                m_bodies[i].setAcc(acceleration);
            }
        });
    }
    
    // Wait for all tasks to complete
    m_threadPool.wait();

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
    m_quadtree.render();
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

// Generates a "random" system of bodies simulating a protoplanetary disk
void Simulation::generateRandomSystem(int count, bool centralMass)
{
    // Clear existing bodies
    reset();
    
    // Random number generation setup
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (centralMass && count > 0) {
        // Add a central star (like our Sun)
        double starMass = 1.0;
        // Visual size for central star (0.06 AU * 150 scale = 9 pixels)
        double starRadius = 0.06; // Fixed visual size for central star
        Body star(starMass, starRadius, Vec2(0, 0), Vec2(0, 0), YELLOW);
        addBody(star);
        count--;
    }
    
    if (count <= 0) return;
    
    // Distributions for protoplanetary disk properties
    std::uniform_real_distribution<> massDist(-8, -4); // Log scale: 10^-8 to 10^-4 solar masses (planetesimal range)
    std::uniform_real_distribution<> radiusDist(0, 1.0); // For surface density sampling
    std::uniform_real_distribution<> angleDist(0, 2 * M_PI); // Random angle around disk
    std::uniform_real_distribution<> eccDist(0, 0.05); // Low eccentricity (circular orbits)
    std::normal_distribution<> inclinationDist(0, 0.01); // Small vertical displacement (flattened disk)
    
    for (int i = 0; i < count; i++) {
        // Generate random mass (log scale)
        double mass = std::pow(10, massDist(gen));
        
        // Generate distance using surface density profile (∝ r^-1.5)
        // Use inverse transform sampling for r^-1.5 distribution
        double r_min = 0.5;  // Inner edge of disk in AU
        double r_max = 8.0;  // Outer edge of disk in AU
        double u = radiusDist(gen);
        double distance = std::pow(
            std::pow(r_min, -0.5) + u * (std::pow(r_max, -0.5) - std::pow(r_min, -0.5)),
            -2.0
        );
        
        // Random angle for position in disk
        double angle = angleDist(gen);
        
        // Add small vertical displacement for disk thickness (Gaussian distribution)
        double z_displacement = inclinationDist(gen) * distance; // Scale with distance
        
        // Calculate position (mostly in plane with small z-component)
        Vec2 position(
            std::cos(angle) * distance,
            std::sin(angle) * distance + z_displacement
        );
        
        // Calculate velocity for stable circular orbit
        double centralMassValue = centralMass ? 1.0 : (i > 0 ? m_bodies[0].getMass() : 1.0);
        double circularVelocity = std::sqrt(GC * centralMassValue / distance);
        
        // Add small eccentricity
        double eccentricity = eccDist(gen);
        double velocityMag = circularVelocity * (1.0 + eccentricity);
        
        // Velocity is perpendicular to radial direction (prograde orbit only)
        Vec2 velocityDir(-std::sin(angle), std::cos(angle));
        Vec2 velocity = velocityDir * velocityMag;
        
        // Logarithmic radius scaling for visual representation
        // Dramatic size variation to clearly show mass differences
        // For mass 10^-8: log10(10^-8) = -8, scaled radius = 0.02 AU (3 pixels)
        // For mass 10^-4: log10(10^-4) = -4, scaled radius = 0.04 AU (6 pixels)
        // This creates a 2x visible size range with clear mass differences
        double log_mass = std::log10(mass);
        double radius = 0.02 + 0.005 * (log_mass + 8.0); // Scales from 0.02 to 0.04 AU
        
        // Create and add the body
        Body body(mass, radius, position, velocity, WHITE);
        addBody(body);
    }    
}
