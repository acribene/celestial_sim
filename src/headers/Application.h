#ifndef APPLICATION_H
#define APPLICATION_H

#include "simulation.h"
#include "TimeManager.h"
#include "CameraController.h"
#include "InputHandler.h"
#include "Sidebar.h"
#include "../utils/constants.h"

class Application
{
    private:
    bool isRunning_; // Main application loop flag
    CameraController cameraController_; // Manages camera movement and zoom
    TimeManager timeManager_; // Manages discrepencies between simulation and real time
    Simulation simulation_; // The main simulation instance
    Sidebar sidebar_; // UI Sidebar for controls and info

    void initialize();
    void update();
    void render();
    void shutdown();
    void processInput();

    public:
    // Takes the window width and height
    Application(int width, int height);
    ~Application() = default;

    // Wont support copying of Applications
    Application(const Application& app) = delete;
    Application(Application&& app) = delete;
    Application& operator=(const Application& rhs) = delete;
    Application& operator=(Application&& rhs) = delete;

    void run(); // Main application loop
};

#endif // APPLICATION_H