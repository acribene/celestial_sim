#include "../headers/CameraController.h"
#include <algorithm>

CameraController::CameraController(int screenWidth, int screenHeight) 
    : panSpeed_(10.0f), zoomSpeed_(0.02f) {
    camera_.target = { screenWidth/2.0f, screenHeight/2.0f };
    camera_.offset = { screenWidth/2.0f, screenHeight/2.0f };
    camera_.rotation = 0.0f;
    camera_.zoom = 1.0f;
}

void CameraController::update() {
    // Camera controls
    if (IsKeyDown(KEY_RIGHT)) camera_.target.x += panSpeed_ / camera_.zoom;
    if (IsKeyDown(KEY_LEFT)) camera_.target.x -= panSpeed_ / camera_.zoom;
    if (IsKeyDown(KEY_UP)) camera_.target.y -= panSpeed_ / camera_.zoom;
    if (IsKeyDown(KEY_DOWN)) camera_.target.y += panSpeed_ / camera_.zoom;
    
    // if (IsKeyDown(KEY_A)) camera.zoom += zoomSpeed;
    // if (IsKeyDown(KEY_Z)) camera.zoom -= zoomSpeed;

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera_);

        // Set the offset to where the mouse is
        camera_.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        camera_.target = mouseWorldPos;
        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        camera_.zoom += zoomSpeed_ * wheel;
    }
    
    // Clamp zoom
    camera_.zoom = std::max(0.1f, std::min(10.0f, camera_.zoom));
}

Camera2D CameraController::getCamera() const {
    return camera_;
}

void CameraController::setTarget(Vector2 target) {
    camera_.target = target;
}