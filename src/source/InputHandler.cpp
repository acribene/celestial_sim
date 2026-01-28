#include "../headers/InputHandler.h"
#include <iostream>

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
    if( IsKeyPressed(KEY_T) ) { simulation.toggleWF(); }
    // Add simulation-specific input handling here
    // For example: adding bodies, resetting, etc.
}
void InputHandler::handleSelection(Sidebar &sidebar, Simulation &sim, const Camera2D &camera)
{
    // Mouse Left Click
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Check if we clicked the UI first
        if (sidebar.isMouseOver()) {
            return; // Don't select the world if we are dragging a slider
        }

        // Convert Screen Space -> World Space
        Vector2 mouseScreen = GetMousePosition();
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
        
        // Raycast / Hit Test
        Vec2 simPos(mouseWorld.x, mouseWorld.y);
        
        Body* clickedBody = sim.getBodyAt(simPos);

        if (clickedBody) {
            sidebar.selectBody(clickedBody);
        } else {
            // Clicked empty space
            sidebar.deselect();
        }
    }
}