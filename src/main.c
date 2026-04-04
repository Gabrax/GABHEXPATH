#include <raylib.h>
#include <math.h>
#include <stdio.h>

#include "raymath.h"

#define MAX_COLS 10
#define MAX_ROWS 10

typedef struct {
    Vector2 center;
    bool selected;
} Hex;

Hex grid[MAX_COLS][MAX_ROWS];

void InitHexGrid(int cols, int rows, float radius)
{
    float height = sqrtf(3.0f) * radius;

    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            float x = col * (radius * 1.5f);
            float y = row * height + (col % 2) * (height / 2.0f);

            grid[col][row].center = (Vector2){ x + 100, y + 100 };
            grid[col][row].selected = false;
        }
    }
}

bool IsPointInHex(Vector2 p, Vector2 center, float radius)
{
    return Vector2Distance(p, center) < radius;
}

void HandleInput(int cols, int rows, float radius)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();

        for (int col = 0; col < cols; col++)
        {
            for (int row = 0; row < rows; row++)
            {
                if (IsPointInHex(mouse, grid[col][row].center, radius))
                {
                    grid[col][row].selected = !grid[col][row].selected;
                }
            }
        }
    }
}

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
    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            Color color = grid[col][row].selected ? RED : BLACK;

            DrawHexagon(grid[col][row].center, radius, color);
        }
    }
}

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "GABHEXPATH");
    SetTargetFPS(60);

    InitHexGrid(MAX_COLS, MAX_ROWS, 15.0f);

    while (!WindowShouldClose()) 
    {
        HandleInput(MAX_COLS, MAX_ROWS, 15.0f);

        BeginDrawing();
        ClearBackground(GRAY);

        DrawHexGrid(MAX_COLS, MAX_ROWS, 15.0f);

        EndDrawing();
    }

    CloseWindow();
}