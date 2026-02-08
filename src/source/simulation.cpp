#include "../headers/simulation.h"
#include "raylib.h"
#include <cmath>
#include <random>
#include <thread>
#include <iostream>

// Default ctor sets bodies to stl vector default and puts timescale at 1 (real time)
// Initialize quadtree with theta (default 0.5) and epsilon from constants
Simulation::Simulation(double theta) 
    : m_bodies(std::vector<Body>()), 
      m_timeScale(1.0),
      m_quadtree(Quadtree(theta, SOFTENING)),
      m_theta(theta),
      m_threadCount(std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4),
      m_threadPool(m_threadCount),
      m_toggleWF(false)
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
    m_quadtree.reserve(m_bodies.size());
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
Body* Simulation::addBody(Body body)
{
    m_bodies.push_back(body);
    return &m_bodies.back();
}

void Simulation::render()
{
    // Renders bodies
    for (const Body &body : m_bodies)
    {
        body.draw();
    }
    if( m_toggleWF ) { m_quadtree.render(); } 
}

// Removes all bodies from current simulation
void Simulation::reset()
{
    m_bodies.clear();
    m_timeScale = 1.0;
}

void Simulation::saveSimulation(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to save to " << filename << std::endl;
        return;
    }

    // 1. Write the number of bodies (size_t) header
    size_t count = m_bodies.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(size_t));

    // 2. Write the entire vector data block directly
    // This assumes Body contains no pointers or std::string
    if (count > 0) {
        file.write(reinterpret_cast<const char*>(m_bodies.data()), count * sizeof(Body));
    }

    file.close();
}

void Simulation::loadSimulation(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to load from " << filename << std::endl;
        return;
    }

    reset(); // Clear current simulation

    // 1. Read the header (number of bodies)
    size_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(size_t));

    if (count > 0) {
        // 2. Resize vector to allocate the exact memory needed
        // This requires Body to have a default constructor (which your code implies it does)
        m_bodies.resize(count);

        // 3. Read the data block directly into the vector's memory
        file.read(reinterpret_cast<char*>(m_bodies.data()), count * sizeof(Body));
    }

    file.close();
    
    // Rebuild quadtree to prevent visual glitches on the first frame
    Quad boundingQuad = Quad::newContaining(m_bodies);
    m_quadtree.clear(boundingQuad);
}

void Simulation::loadPreset(int presetID) 
{
    reset(); // Clear existing

    if (presetID == 0) {
        // PRESET: Solar System (Moved from Application.cpp)
        
        // Sun
        addBody(Body(1.0, 0.08, Vec2(0, 0), Vec2(0, 0), YELLOW));
        
        // Mercury
        double mercury_v = sqrt(1.0 / 0.387) * 2.0 * M_PI;
        addBody(Body(0.000000166, 0.004, Vec2(0.387, 0), Vec2(0, mercury_v), GRAY));
        
        // Venus
        double venus_v = sqrt(1.0 / 0.723) * 2.0 * M_PI;
        addBody(Body(0.00000244, 0.009, Vec2(0.723, 0), Vec2(0, venus_v), Color{255, 200, 100, 255}));
        
        // Earth
        double earth_v = sqrt(1.0 / 1.0) * 2.0 * M_PI;
        addBody(Body(0.000003, 0.01, Vec2(1.0, 0), Vec2(0, earth_v), BLUE));
        
        // Mars
        double mars_v = sqrt(1.0 / 1.524) * 2.0 * M_PI;
        addBody(Body(0.00000323, 0.006, Vec2(1.524, 0), Vec2(0, mars_v), RED));

        // Jupiter
        double jupiter_v = sqrt(1.0 / 5.203) * 2.0 * M_PI;
        addBody(Body(0.000954, 0.05, Vec2(5.203, 0), Vec2(0, jupiter_v), ORANGE));

        // Saturn
        double saturn_v = sqrt(1.0 / 9.537) * 2.0 * M_PI;
        addBody(Body(0.000285, 0.045, Vec2(9.537, 0), Vec2(0, saturn_v), Color{210, 180, 140, 255}));

    } else if (presetID == 1) {
        // PRESET: Protoplanetary Collision
        generateProPlanetaryDisk(100, Vec2(-10, 0), Vec2(0, 1.49), true);
        generateProPlanetaryDisk(100, Vec2(10, 0), Vec2(0, -1.49), true);
    } else if (presetID == 2) {
        // PRESET: Random Stable Disk
        generateProPlanetaryDisk(200, Vec2(0,0), Vec2(0,0), true);
    }
}

// Set the theta parameter for Barnes-Hut algorithm
// Lower values = more accurate but slower (typical range: 0.3 - 1.0)
void Simulation::setTheta(double theta)
{
    m_theta = theta;
    // Reinitialize quadtree with new theta
    m_quadtree = Quadtree(m_theta, SOFTENING);
}

// Generates a protoplanetary disk of bodies around a central point
void Simulation::generateProPlanetaryDisk(int count, Vec2 centerPoint, Vec2 velocity, bool centralMass)
{
    // Random number generation setup
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (centralMass && count > 0) {
        // Add a central star (like our Sun)
        double starMass = 1.0;
        // Visual size for central star (0.06 AU * 150 scale = 9 pixels)
        double starRadius = 0.06; // Fixed visual size for central star
        Body star(starMass, starRadius, centerPoint, velocity, YELLOW);
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
        // Position relative to centerPoint
        Vec2 position(
            centerPoint.getX() + std::cos(angle) * distance,
            centerPoint.getY() + std::sin(angle) * distance + z_displacement
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
        
        // Add the body
        addBody(Body(mass, radius, position, velocity, WHITE));
    }    
}

void Simulation::DeleteBodyAt(Vec2 worldPos) {
    for( auto body = m_bodies.rbegin(); body != m_bodies.rend(); ++body ) {
        if( body->getPos() == worldPos ) {
            m_bodies.erase( std::next(body).base() );
            break;
        }
    }
}

Body *Simulation::getBodyAt(Vec2 worldPos)
{
    // Setup the same constants used in Body::draw
    double halfWidth = WINDOW_WIDTH / 2.0;
    double halfHeight = WINDOW_HEIGHT / 2.0;

    for( auto body = m_bodies.rbegin(); body != m_bodies.rend(); ++body ) {
        
        // Convert the Body's Physics Position to "Visual Position"
        double visualX = halfWidth + body->getPos().getX() * SCALE;
        double visualY = halfHeight + body->getPos().getY() * SCALE;

        // 3. Calculate Distance in Visual/Pixel Space
        double dx = worldPos.getX() - visualX;
        double dy = worldPos.getY() - visualY;
        double distSq = dx*dx + dy*dy;
        
        // Calculate Visual Radius (including the minimum 3.0 pixel clamp)
        // If we don't do this, small planets (radius < 1 pixel) will be unclickable
        double visualRadius = body->getRadius() * SCALE;
        if (visualRadius < 3.0) {
            visualRadius = 3.0;
        }

        // Check collision
        if (distSq <= (visualRadius * visualRadius)) {
            return &(*body);
        }
    }
    return nullptr;
}
