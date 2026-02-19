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
void InputHandler::handleSelection(Sidebar &sidebar, Simulation &sim, const Camera2D &camera, TimeManager& timeMgr)
{
    if (IsKeyPressed(KEY_TAB)) {
        sidebar.toggleInfo();
    }
    
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
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // Don't click through the sidebar
        if (sidebar.isMouseOver()) return;

        // 1. Convert Screen Space -> World Space
        Vector2 mouseScreen = GetMousePosition();
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
        double halfWidth = WINDOW_WIDTH / 2.0;
        double halfHeight = WINDOW_HEIGHT / 2.0;

        // Reverse the offset (subtract center)
        double tempX = mouseWorld.x - halfWidth;
        double tempY = mouseWorld.y - halfHeight;

        // Reverse the scaling (divide by SCALE)
        double simX = tempX / SCALE;
        double simY = tempY / SCALE;
        Vec2 simPos(simX, simY);

        
        // 2. Pause the simulation if it isn't already
        if (!timeMgr.getPauseState()) {
            timeMgr.togglePause();
        }

        // 3. Open Sidebar in Creation Mode
        sidebar.openCreationMenu(simPos);
    }
}