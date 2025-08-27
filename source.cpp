#include "raylib.h"

int main() {
    InitWindow(800, 600, "Raylib Example - Ball");

    Vector2 ballPosition = {400, 300};
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT)) ballPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT))  ballPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP))    ballPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN))  ballPosition.y += 2.0f;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(ballPosition, 30, MAROON);
        DrawText("Move the ball with arrow keys!", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
