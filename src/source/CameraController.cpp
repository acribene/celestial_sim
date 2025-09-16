#include "../headers/CameraController.h"
#include <algorithm>

CameraController::CameraController(int screenWidth, int screenHeight) 
    : panSpeed(10.0f), zoomSpeed(0.01f) {
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
    
    if (IsKeyDown(KEY_A)) camera.zoom += zoomSpeed;
    if (IsKeyDown(KEY_Z)) camera.zoom -= zoomSpeed;
    
    // Clamp zoom
    camera.zoom = std::max(0.1f, std::min(10.0f, camera.zoom));
}

Camera2D CameraController::getCamera() const {
    return camera;
}

void CameraController::setTarget(Vector2 target) {
    camera.target = target;
}