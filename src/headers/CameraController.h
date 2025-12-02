#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "raylib.h"

class CameraController 
{
    private:
    Camera2D camera_;
    float panSpeed_;
    float zoomSpeed_;
    
    public:
    CameraController(int screenWidth, int screenHeight);
    ~CameraController() = default;
    
    // Camera controller should not be copied or moved.
    CameraController(const CameraController& other) = delete;
    CameraController(CameraController&& other) = delete;
    CameraController& operator=(const CameraController& rhs) = delete;
    CameraController& operator=(CameraController&& rhs) = delete;

    void update();
    Camera2D getCamera() const;
    void setTarget(Vector2 target);
};

#endif // CAMERA_CONTROLLER_H
