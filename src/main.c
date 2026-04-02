#include <raylib.h>
#include <math.h>
#include <stdio.h>

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "GABHEXPATH");
    SetTargetFPS(60);

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV((Vector2){50,50}, 10, RED);

        EndDrawing();
    }

    CloseWindow();
}