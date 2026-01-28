#include "../headers/Application.h"
#include "raylib.h"

Application::Application(int width, int height) : isRunning_(false), cameraController_(width, height), timeManager_(TimeManager()), simulation_(Simulation()), sidebar_(Sidebar()) {
    initialize();
}

void Application::initialize() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "2D Physics Simulator");
    SetTargetFPS(60);
    
    // // For testing
    // // Sun
    // Body sun(1.0, 0.08, Vec2(0, 0), Vec2(0, 0), YELLOW);
    
    // // Mercury
    // // Mass: 0.000000166 solar masses, Distance: 0.387 AU
    // double mercury_v = sqrt(1.0 / 0.387) * 2.0 * M_PI;
    // Body mercury(0.000000166, 0.004, Vec2(0.387, 0), Vec2(0, mercury_v), GRAY);
    
    // // Venus
    // // Mass: 0.00000244 solar masses, Distance: 0.723 AU
    // double venus_v = sqrt(1.0 / 0.723) * 2.0 * M_PI;
    // Body venus(0.00000244, 0.009, Vec2(0.723, 0), Vec2(0, venus_v), Color{255, 200, 100, 255});
    
    // // Earth
    // // Mass: 0.000003 solar masses, Distance: 1.0 AU
    // double earth_v = sqrt(1.0 / 1.0) * 2.0 * M_PI;
    // Body earth(0.000003, 0.01, Vec2(1.0, 0), Vec2(0, earth_v), BLUE);
    
    // // Mars
    // // Mass: 0.000000323 solar masses, Distance: 1.524 AU
    // double mars_v = sqrt(1.0 / 1.524) * 2.0 * M_PI;
    // Body mars(0.000000323, 0.006, Vec2(1.524, 0), Vec2(0, mars_v), RED);
    
    // // Jupiter
    // // Mass: 0.000954 solar masses, Distance: 5.203 AU
    // double jupiter_v = sqrt(1.0 / 5.203) * 2.0 * M_PI;
    // Body jupiter(0.000954, 0.05, Vec2(5.203, 0), Vec2(0, jupiter_v), ORANGE);
    
    // // Saturn
    // // Mass: 0.000285 solar masses, Distance: 9.537 AU
    // double saturn_v = sqrt(1.0 / 9.537) * 2.0 * M_PI;
    // Body saturn(0.000285, 0.045, Vec2(9.537, 0), Vec2(0, saturn_v), Color{210, 180, 140, 255});
    
    // // Uranus
    // // Mass: 0.0000436 solar masses, Distance: 19.191 AU
    // double uranus_v = sqrt(1.0 / 19.191) * 2.0 * M_PI;
    // Body uranus(0.0000436, 0.035, Vec2(19.191, 0), Vec2(0, uranus_v), SKYBLUE);
    
    // // Neptune
    // // Mass: 0.0000515 solar masses, Distance: 30.069 AU
    // double neptune_v = sqrt(1.0 / 30.069) * 2.0 * M_PI;
    // Body neptune(0.0000515, 0.035, Vec2(30.069, 0), Vec2(0, neptune_v), DARKBLUE);
    
    // simulation_.addBody(sun);
    // simulation_.addBody(mercury);
    // simulation_.addBody(venus);
    // simulation_.addBody(earth);
    // simulation_.addBody(mars);
    // simulation_.addBody(jupiter);
    // simulation_.addBody(saturn);
    // simulation_.addBody(uranus);
    // simulation_.addBody(neptune);
    simulation_.generateProPlanetaryDisk(50, Vec2(-10, 0), Vec2(0, 1.49), true);  // Left disk moving up
    simulation_.generateProPlanetaryDisk(50, Vec2(10, 0), Vec2(0, -1.49), true);  // Right disk moving down
    
    isRunning_ = true;
}

void Application::update()
{
    timeManager_.update();
    while(timeManager_.shouldUpdatePhysics())
    {
        simulation_.update(timeManager_.getFixedDeltaTime());
        timeManager_.consumePhysicsTime();
    }
    sidebar_.update(GetFrameTime());
}

void Application::render()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode2D(cameraController_.getCamera());
    simulation_.render();
    EndMode2D();

    sidebar_.render(timeManager_);

    EndDrawing();
}

void Application::shutdown()
{
    CloseWindow();
}

void Application::processInput()
{
    InputHandler::handleTimeScaleInput(timeManager_);
    InputHandler::handleCameraInput(cameraController_);
    InputHandler::handleSimulationInput(simulation_);
    InputHandler::handleSelection(sidebar_, simulation_, cameraController_.getCamera());
}

void Application::run()
{
    while (isRunning_ && !WindowShouldClose())
    {
        processInput();
        update();
        render();
    }
    shutdown();
}