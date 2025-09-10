#include "raylib.h"
#include "../headers/simulation.h"

int main() {
    InitWindow(800, 800, "Celestial Simulation");

    SetTargetFPS(60);
    Simulation sim;
    
    while (!WindowShouldClose()) {
    }

    CloseWindow();
    return 0;
}
