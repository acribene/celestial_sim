#include "../headers/InputHandler.h"
#include "../headers/CameraController.h"
#include "../headers/simulation.h"
#include "../headers/TimeManager.h"
#include <raylib.h>

void InputHandler::handleTimeScaleInput(TimeManager& time_manager) {
    if (IsKeyPressed(KEY_SPACE)) { time_manager.togglePause(); }
    if( time_manager.getPauseState() && IsKeyDown(KEY_ENTER) ) { time_manager.step(); }
    if (IsKeyPressed(KEY_EQUAL)) { time_manager.increaseTimeScale(1.5f); }
    if (IsKeyPressed(KEY_MINUS)) { time_manager.decreaseTimeScale(1.5f); }
}

void InputHandler::handleCameraInput(CameraController& cameraController) {
    cameraController.update();
}

void InputHandler::handleSimulationInput(Simulation& simulation) {
    // Add simulation-specific input handling here
    // For example: adding bodies, resetting, etc.
}