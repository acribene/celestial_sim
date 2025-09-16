#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "raylib.h"

class CameraController 
{
    private:
    Camera2D camera;
    float panSpeed;
    float zoomSpeed;
    
    public:
    CameraController(int screenWidth, int screenHeight);
    
    void update();
    Camera2D getCamera() const;
    void setTarget(Vector2 target);
};

#endif // CAMERA_CONTROLLER_H
