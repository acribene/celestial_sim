#ifndef APPLICATION_H
#define APPLICATION_H

#include "simulation.h"
#include "TimeManager.h"
#include "CameraController.h"
#include "InputHandler.h"
#include "../utils/constants.h"

class Application
{
    private:
    bool isRunning_;
    CameraController cameraController_;
    TimeManager timeManager_;
    Simulation simulation_;

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

    void run();
};

#endif // APPLICATION_H