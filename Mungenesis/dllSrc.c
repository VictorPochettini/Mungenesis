#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cJSON.h"

// Export macro for DLL
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

// Structure to match C# Vector2
typedef struct
{
    float x;
    float y;
    float value;
} Vector2;

typedef struct
{
    BlittableCorner corner0;
    BlittableCorner corner1;
    BlittableCorner corner2;
    BlittableCorner corner3;
    int layer;
} BlittableCell;

typedef struct
{
    int id;
    Vector2 gradientV;
    Vector2 position;
} BlittableCorner;

Vector2 d(float x, float y, float x0, float y0)
{
    Vector2 result;
    result.x = x - x0;
    result.y = y - y0;
    result.value = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
    return result;
}

int fade(int t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

int interpolation(float corner1Value, float corner2Value, float corner3Value, float corner4Value, int currentPosition, int cellIndex, int cellLine, int cellColumn, int line, int worldSize, int linePerCell)
{
    int x1 = currentPosition % worldSize;
    int x0 = cellColumn * (linePerCell + 1);
    int y1 = currentPosition / worldSize;
    int y0 = cellLine * (linePerCell + 1);

    int u = fade(x1 - x0);
    int v = fade(y1 - y0);

    float ix1 = lerp(corner1Value, corner2Value, u);
    float ix2 = lerp(corner3Value, corner4Value, u);
    return (int)lerp(ix1, ix2, v);
}

// Function implementations
EXPORT void DotGrid(BlittableCell *cells, int worldSize, int variation)
{
    int linePerCell = worldSize / variation;
    int line;
    int column;
    int currentPosition;
    int cellLine;
    int cellColumn;
    int cellIndex;
    float corner1Value, corner2Value, corner3Value, corner4Value;
    Vector2 map[worldSize][worldSize];
    BlittableCell *cell;

    for (int i = 0; i < worldSize * worldSize; i++)
    {
        line = i / worldSize;
        column = i % worldSize;
        cellLine = line / linePerCell;
        cellColumn = column / linePerCell;
        cellIndex = cellLine * variation + cellColumn;

        map[line][column].x = column;
        map[line][column].y = line;

        corner1Value = cells[cellIndex].corner0.gradientV.x * d(column, line, cells[cellIndex].corner0.position.x, cells[cellIndex].corner0.position.y).x +
                       cells[cellIndex].corner0.gradientV.y * d(column, line, cells[cellIndex].corner0.position.x, cells[cellIndex].corner0.position.y).y;
        corner2Value = cells[cellIndex].corner1.gradientV.x * d(column, line, cells[cellIndex].corner1.position.x, cells[cellIndex].corner1.position.y).x +
                       cells[cellIndex].corner1.gradientV.y * d(column, line, cells[cellIndex].corner1.position.x, cells[cellIndex].corner1.position.y).y;
        corner3Value = cells[cellIndex].corner2.gradientV.x * d(column, line, cells[cellIndex].corner2.position.x, cells[cellIndex].corner2.position.y).x +
                       cells[cellIndex].corner2.gradientV.y * d(column, line, cells[cellIndex].corner2.position.x, cells[cellIndex].corner2.position.y).y;
        corner4Value = cells[cellIndex].corner3.gradientV.x * d(column, line, cells[cellIndex].corner3.position.x, cells[cellIndex].corner3.position.y).x +
                       cells[cellIndex].corner3.gradientV.y * d(column, line, cells[cellIndex].corner3.position.x, cells[cellIndex].corner3.position.y).y;

        map[line][column].value = interpolation(corner1Value, corner2Value, corner3Value, corner4Value, i, cellIndex, cellLine, cellColumn, line, worldSize, linePerCell);
    }
}

EXPORT void generateSeed(Vector2 *seed, int cornerNumber)
{
    srand((unsigned int)time(NULL)); // Seed the random number generator
    for (int i = 0; i < cornerNumber; i++)
    {
        seed[i].x = (float)(rand() % 1000) / 1000.0f; // Random float between 0 and 1
        seed[i].y = (float)(rand() % 1000) / 1000.0f; // Random float between 0 and 1

        // Creates the possibility of negative values
        int negative1 = rand() % 2;
        int negative2 = rand() % 2;

        // 50% chance to make the values negative
        seed[i].x = (negative1 == 0) ? seed[i].x : -seed[i].x;
        seed[i].y = (negative2 == 0) ? seed[i].y : -seed[i].y;

        // Normalize the vector
        float norm = sqrt(seed[i].x * seed[i].x + seed[i].y * seed[i].y);
        seed[i].x /= norm;
        seed[i].y /= norm;
    }
}

EXPORT void freeSeed(void *ptr)
{
    // Placeholder implementation for freeing memory
    if (ptr != NULL)
    {
        free(ptr);
    }
}