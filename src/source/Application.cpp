#include "../headers/Application.h"
#include "raylib.h"

Application::Application(int width, int height) : isRunning_(false), cameraController_(width, height), timeManager_(TimeManager()), simulation_(Simulation()), sidebar_(simulation_, timeManager_) {
    initialize();
    sidebar_.applyTheme();
}

void Application::initialize() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "2D Physics Simulator");
    SetTargetFPS(60);
    
    simulation_.loadPreset(0); // Load solar system preset
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

    sidebar_.render();

    const char* helpText = "Press TAB for help";
    int fontSize = 20;
    int textWidth = MeasureText(helpText, fontSize);
    
    int paddingRight = 20;
    int paddingTop = 20;
    
    int xPos = GetScreenWidth() - textWidth - paddingRight;
    int yPos = paddingTop;

    // Draw text with 50% opacity
    DrawText(helpText, xPos, yPos, fontSize, Fade(LIGHTGRAY, 0.5f));
    
    EndDrawing();
}

void Application::shutdown()
{
    CloseWindow();
}

void Application::processInput()
{
    if(!sidebar_.isEditing())
    {
        InputHandler::handleTimeScaleInput(timeManager_);
        InputHandler::handleCameraInput(cameraController_);
        InputHandler::handleSimulationInput(simulation_);
    }
    InputHandler::handleSelection(sidebar_, simulation_, cameraController_.getCamera(), timeManager_);
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