#include "raylib.h"

typedef enum {
    Scene_Menu,
    Scene_Game
} Scene;

int main(void)
{
    const int screen_width = 800;
    const int screen_height = 800;

    Scene scene = Scene.Scene_Menu;

    InitWindow(screen_width, screen_height, "Raylib Template");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("TEXT HERE", 190, 200, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}