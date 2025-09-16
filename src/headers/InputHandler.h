#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class TimeManager;
class Simulation;
class CameraController;

class InputHandler {
public:
    static void handleTimeScaleInput(TimeManager& timeManager);
    static void handleCameraInput(CameraController& cameraController);
    static void handleSimulationInput(Simulation& simulation);
};

#endif // INPUT_HANDLER_H
