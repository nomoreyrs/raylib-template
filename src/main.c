#include "raylib.h"

int main() {
    InitWindow(800, 600, "App");

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);
        EndDrawing();
    }

    CloseWindow();
}