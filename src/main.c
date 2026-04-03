#include <raylib.h>
#include <math.h>
#include <stdio.h>

void DrawHexagon(Vector2 center, float radius, Color color)
{
    Vector2 points[6];

    for (int i = 0; i < 6; i++)
    {
        float angle = DEG2RAD * (60.0f * (float)i);
        points[i] = (Vector2){
            center.x + cosf(angle) * radius,
            center.y + sinf(angle) * radius
        };
    }

    for (int i = 0; i < 6; i++)
    {
        Vector2 p1 = points[i];
        Vector2 p2 = points[(i + 1) % 6];

        DrawTriangle(center, p2, p1, color);
    }
}

void DrawHexGrid(int cols, int rows, float radius)
{
    float height = sqrtf(3.0f) * radius;

    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            float x = (float)col * (radius * 1.5f);
            float y = (float)row * height + (float)(col % 2) * (height / 2.0f);

            DrawHexagon((Vector2){ x + 100, y + 100 }, radius, BLACK);
        }
    }
}

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

        DrawHexGrid(10, 10, 15.0f);

        EndDrawing();
    }

    CloseWindow();
}