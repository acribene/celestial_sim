#include "../headers/Application.h"
#include "raylib.h"

Application::Application(int width, int height) : isRunning(false), cameraController(width, height), timeManager(TimeManager()), simulation(Simulation()) {
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
    simulation.generateRandomSystem(25);
    
    isRunning = true;
}

void Application::update()
{
    timeManager.update();
    while(timeManager.shouldUpdatePhysics())
    {
        simulation.update(timeManager.getFixedDeltaTime());
        timeManager.consumePhysicsTime();
    }
}

void Application::render()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode2D(cameraController.getCamera());
    simulation.render();
    EndMode2D();

    // Display information
    DrawText("2D Physics Simulator", 10, 10, 20, WHITE);
    DrawText(TextFormat("Time Scale: %.2f", timeManager.getTimeScale()), 10, 40, 20, WHITE);
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
    InputHandler::handleTimeScaleInput(timeManager);
    InputHandler::handleCameraInput(cameraController);
}

void Application::run()
{
    while (isRunning && !WindowShouldClose())
    {
        processInput();
        update();
        render();
    }
    shutdown();
}