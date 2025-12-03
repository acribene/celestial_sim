#include "../headers/Application.h"
#include "raylib.h"

Application::Application(int width, int height) : isRunning_(false), cameraController_(width, height), timeManager_(TimeManager()), simulation_(Simulation()) {
    initialize();
}

void Application::initialize() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "2D Physics Simulator");
    SetTargetFPS(60);
    
    // For testing
    // Body sun(1.0, 0.05, Vec2(0, 0), Vec2(0, 0), WHITE);
    // Body earth(0.000003, 0.01, Vec2(1.0, 0), Vec2(0, 2.0 * M_PI), WHITE);
    // Body mars(0.00000032, 0.005, Vec2(1.52, 0), Vec2(0, 5.08), WHITE);
    
    // simulation.addBody(sun);
    // simulation.addBody(earth);
    // simulation.addBody(mars);
    simulation_.generateRandomSystem(200);
    
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
}

void Application::render()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode2D(cameraController_.getCamera());
    simulation_.render();
    EndMode2D();

    // Display information
    DrawText("2D Physics Simulator", 10, 10, 20, WHITE);
    DrawText(TextFormat("Time Scale: %.2f", timeManager_.getTimeScale()), 10, 40, 20, WHITE);
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 70, 20, WHITE);
    DrawText("Controls: Arrow keys to pan, A/Z to zoom, +/- to change time scale", 10, 100, 20, WHITE);

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