#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "raymath.h"

#define MAX_COLS 26
#define MAX_ROWS 15
#define RADIUS 15

typedef struct {
    Vector2 center;
    bool selected;
} Hex;

Hex grid[MAX_COLS][MAX_ROWS];

Vector2 startHex = { -1, -1 };
Vector2 finishHex = { -1, -1 };

typedef struct Node {
    int col, row;
    int gCost, hCost, fCost;
    int parentCol, parentRow; // store parent as coordinates
    bool inOpen;
} Node;

void InitHexGrid()
{
    const float height = sqrtf(3.0f) * RADIUS;

    for (int col = 0; col < MAX_COLS; col++)
    {
        for (int row = 0; row < MAX_ROWS; row++)
        {
            float x = (float)col * (RADIUS * 1.5f);
            float y = (float)row * height + (float)(col % 2) * (height / 2.0f);

            grid[col][row].center = (Vector2){ x + 100, y + 100 };
            grid[col][row].selected = false;
        }
    }
}

bool IsMouseInHexagon(const Vector2 center, const float radius)
{
    const Vector2 mouse = GetMousePosition();
    return Vector2Distance(mouse, center) <= radius;
}

void UpdateHexSelection()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        for (int col = 0; col < MAX_COLS; col++)
        {
            for (int row = 0; row < MAX_ROWS; row++)
            {
                Vector2 center = grid[col][row].center;
                if (IsMouseInHexagon(center, RADIUS))
                {
                    if (startHex.x < 0)
                    {
                        startHex = center;
                        grid[col][row].selected = true;
                    }
                    else if (finishHex.x < 0)
                    {
                        finishHex = center;
                        grid[col][row].selected = true;
                    }
                    return; // stop after selecting one
                }
            }
        }
    }
}

void ResetHexSelection()
{
    startHex = (Vector2){ -1, -1 };
    finishHex = (Vector2){ -1, -1 };

    for (int col = 0; col < MAX_COLS; col++)
    {
        for (int row = 0; row < MAX_ROWS; row++)
        {
            grid[col][row].selected = false;
        }
    }
}

void DrawHexagon(const Vector2 center, const float radius, const bool selected)
{
    Vector2 points[6];

    for (int i = 0; i < 6; i++)
    {
        const float angle = DEG2RAD * (60.0f * (float)i);
        points[i] = (Vector2){
            center.x + cosf(angle) * radius,
            center.y + sinf(angle) * radius
        };
    }

    for (int i = 0; i < 6; i++)
    {
        const Vector2 p1 = points[i];
        const Vector2 p2 = points[(i + 1) % 6];

        const Color color = selected ? RED : BLACK;
        (selected) ? DrawTriangle(center, p2, p1, color) : DrawLineV(p2, p1, color);
    }
}

void DrawHexGrid()
{
    for (int col = 0; col < MAX_COLS; col++)
    {
        for (int row = 0; row < MAX_ROWS; row++)
        {
            const Vector2 center = grid[col][row].center;
            bool selected = grid[col][row].selected;

            if (Vector2Equals(center, startHex)) selected = true;
            if (Vector2Equals(center, finishHex)) selected = true;

            DrawHexagon(center, RADIUS, selected);
        }
    }
}

typedef struct Cube {
    int x, y, z;
} Cube;

Cube OffsetToCube(int col, int row)
{
    int x = col;
    int z = row - (col - (col & 1)) / 2;
    int y = -x - z;
    return (Cube){x, y, z};
}

int CubeDistance(int col1, int row1, int col2, int row2)
{
    Cube a = OffsetToCube(col1, row1);
    Cube b = OffsetToCube(col2, row2);
    return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z)) / 2;
}

void GetHexNeighbors(int col, int row, int* outCols, int* outRows, int* count)
{
    *count = 0;

    const int dirs_even[6][2] = {
        {+1, -1}, {0, -1}, {-1, -1},
        {-1, 0}, {0, +1}, {+1, 0}
    };

    const int dirs_odd[6][2] = {
        {+1, 0}, {0, -1}, {-1, 0},
        {-1, +1}, {0, +1}, {+1, +1}
    };

    const int (*dirs)[2] = (col % 2 == 1) ? dirs_odd : dirs_even;

    for (int i = 0; i < 6; i++)
    {
        int nc = col + dirs[i][0];
        int nr = row + dirs[i][1];

        if (nc >= 0 && nc < MAX_COLS && nr >= 0 && nr < MAX_ROWS)
        {
            outCols[*count] = nc;
            outRows[*count] = nr;
            (*count)++;
        }
    }
}

float Heuristic(const Vector2 a, const Vector2 b)
{
    return Vector2Distance(a, b);
}

void FindPath()
{
    if (startHex.x < 0 || finishHex.x < 0) return;

    int startCol=-1, startRow=-1, finishCol=-1, finishRow=-1;
    for (int col=0; col<MAX_COLS; col++)
    {
        for (int row=0; row<MAX_ROWS; row++)
        {
            if (Vector2Equals(grid[col][row].center, startHex)) { startCol=col; startRow=row; }
            if (Vector2Equals(grid[col][row].center, finishHex)) { finishCol=col; finishRow=row; }
        }
    }

    if (startCol<0 || finishCol<0) return;

    Node nodes[MAX_COLS][MAX_ROWS] = {0};
    bool closed[MAX_COLS][MAX_ROWS] = {0};

    // Initialize nodes
    for(int c=0;c<MAX_COLS;c++)
    {
        for(int r=0;r<MAX_ROWS;r++)
        {
            nodes[c][r].col = c;
            nodes[c][r].row = r;
            nodes[c][r].gCost = 9999;
            nodes[c][r].hCost = CubeDistance(c,r,finishCol,finishRow);
            nodes[c][r].fCost = 9999;
            nodes[c][r].inOpen = false;
            nodes[c][r].parentCol = -1;
            nodes[c][r].parentRow = -1;
        }
    }

    nodes[startCol][startRow].gCost = 0;
    nodes[startCol][startRow].fCost = nodes[startCol][startRow].hCost;
    nodes[startCol][startRow].inOpen = true;

    bool pathFound = false;

    while(true)
    {
        // Find node in open list with lowest fCost
        Node* current = NULL;
        for(int c=0;c<MAX_COLS;c++)
        {
            for(int r=0;r<MAX_ROWS;r++)
            {
                if(nodes[c][r].inOpen && (!current || nodes[c][r].fCost < current->fCost))
                    current = &nodes[c][r];
            }
        }

        if(!current) break; // no path

        if(current->col == finishCol && current->row == finishRow)
        {
            pathFound = true;
            break;
        }

        current->inOpen = false;
        closed[current->col][current->row] = true;

        int nCols[6], nRows[6], nCount;
        GetHexNeighbors(current->col, current->row, nCols, nRows, &nCount);
        for(int i=0;i<nCount;i++)
        {
            const int nc = nCols[i];
            const int nr = nRows[i];
            if(closed[nc][nr]) continue;

            const int gCostNew = current->gCost + 1; // neighbor cost = 1
            if(gCostNew < nodes[nc][nr].gCost)
            {
                nodes[nc][nr].gCost = gCostNew;
                nodes[nc][nr].fCost = gCostNew + nodes[nc][nr].hCost;
                nodes[nc][nr].parentCol = current->col;
                nodes[nc][nr].parentRow = current->row;
                nodes[nc][nr].inOpen = true;
            }
        }
    }

    if(pathFound)
    {
        int col = finishCol, row = finishRow;
        while(col != -1 && row != -1)
        {
            grid[col][row].selected = true;
            const int parentCol = nodes[col][row].parentCol;
            const int parentRow = nodes[col][row].parentRow;
            col = parentCol; row = parentRow;
        }
    }
}


int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "GABHEXPATH");
    SetTargetFPS(60);

    InitHexGrid();

    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_R)) ResetHexSelection();

        UpdateHexSelection();

        BeginDrawing();
        ClearBackground(GRAY);

        if (IsKeyPressed(KEY_SPACE)) FindPath();

        DrawHexGrid();

        EndDrawing();
    }

    CloseWindow();
}