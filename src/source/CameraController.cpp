#include "../headers/CameraController.h"
#include <algorithm>

CameraController::CameraController(int screenWidth, int screenHeight) 
    : panSpeed(10.0f), zoomSpeed(0.02f) {
    camera.target = { screenWidth/2.0f, screenHeight/2.0f };
    camera.offset = { screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void CameraController::update() {
    // Camera controls
    if (IsKeyDown(KEY_RIGHT)) camera.target.x += panSpeed / camera.zoom;
    if (IsKeyDown(KEY_LEFT)) camera.target.x -= panSpeed / camera.zoom;
    if (IsKeyDown(KEY_UP)) camera.target.y -= panSpeed / camera.zoom;
    if (IsKeyDown(KEY_DOWN)) camera.target.y += panSpeed / camera.zoom;
    
    // if (IsKeyDown(KEY_A)) camera.zoom += zoomSpeed;
    // if (IsKeyDown(KEY_Z)) camera.zoom -= zoomSpeed;

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        // Set the offset to where the mouse is
        camera.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        camera.target = mouseWorldPos;

        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        camera.zoom += zoomSpeed * wheel;
    }
    
    // Clamp zoom
    camera.zoom = std::max(0.1f, std::min(10.0f, camera.zoom));
}

Camera2D CameraController::getCamera() const {
    return camera;
}

void CameraController::setTarget(Vector2 target) {
    camera.target = target;
}