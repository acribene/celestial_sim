#include "../headers/InputHandler.h"
#include "../headers/CameraController.h"
#include "../headers/simulation.h"
#include "../headers/TimeManager.h"
#include <raylib.h>

void InputHandler::handleTimeScaleInput(TimeManager& timeManager) {
    if (IsKeyPressed(KEY_EQUAL)) timeManager.increaseTimeScale(1.5f);
    if (IsKeyPressed(KEY_MINUS)) timeManager.decreaseTimeScale(1.5f);
}

void InputHandler::handleCameraInput(CameraController& cameraController) {
    cameraController.update();
}

void InputHandler::handleSimulationInput(Simulation& simulation) {
    // Add simulation-specific input handling here
    // For example: adding bodies, resetting, etc.
}