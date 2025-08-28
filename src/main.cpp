#include "raylib.h"
#include "simulation.h"

int main() {
    InitWindow(800, 600, "Celestial Simulation");

    SetTargetFPS(60);
    Simulation();
    
    while (!WindowShouldClose()) {
        
    }

    CloseWindow();
    return 0;
}
