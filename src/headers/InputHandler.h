#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "CameraController.h"
#include "simulation.h"
#include "TimeManager.h"
#include "Sidebar.h"
#include <raylib.h>


class InputHandler {
public:
    static void handleTimeScaleInput(TimeManager& timeManager);
    static void handleCameraInput(CameraController& cameraController);
    static void handleSimulationInput(Simulation& simulation);
    static void handleSelection(Sidebar& sidebar, Simulation& sim, const Camera2D& camera, TimeManager& timeMgr);
};

#endif // INPUT_HANDLER_H
