#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Vector2 layout must match the C# struct (Sequential, two floats)
typedef struct
{
    float X;
    float Y;
} Vector2;

// Generate 'corners' unit vectors (gradients). Returns heap pointer; caller must free with freeSeed.
__declspec(dllexport) Vector2 *generateSeed(int corners)
{
    if (corners <= 0)
        return NULL;
    Vector2 *seed = (Vector2 *)malloc(sizeof(Vector2) * (size_t)corners);
    if (!seed)
        return NULL;
    /* seed RNG; for deterministic behavior, seed elsewhere */
    srand((unsigned int)time(NULL));
    for (int i = 0; i < corners; ++i)
    {
        float t = ((float)rand() / (float)RAND_MAX) * 2.0f * (float)M_PI;
        seed[i].X = cosf(t);
        seed[i].Y = sinf(t);
    }
    return seed;
}

// Free a pointer returned by generateSeed
__declspec(dllexport) void freeSeed(Vector2 *ptr)
{
    free(ptr);
}

// Helper functions for Perlin interpolation
static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
static float lerp(float a, float b, float t) { return a + t * (b - a); }

// Compute Perlin-style contribution for a point inside the cell (i,j).
// gradients: flattened array of Vector2 with width = gridWidth (rows x cols),
// i,j: integer cell coordinates (row i, col j) of the top-left corner,
// dx,dy: fractional offsets within the cell in [0,1].
__declspec(dllexport) int DotGrid(Vector2 *gradients, int gridWidth, int squares)
{
    if (!gradients || gridWidth <= 0)
        return 0;

    int cellSize = 1080 / squares;

    for (int i = 0; i < 1080; i++)
    {
        for (int j = 0; j < 1080; j++)
        {
            Vector2* currentGrad[4] = {
                &gradients[(i / cellSize) + (j / cellSize) * sqrt(squares)],
                &gradients[(i / cellSize) + ((j + 1) / cellSize) * sqrt(squares)],
                &gradients[((i + 1) / cellSize) + (j / cellSize) * sqrt(squares)],
                &gradients[((i + 1) / cellSize) + ((j + 1) / cellSize) * sqrt(squares)]
            };
        }
    }
}

float distance(x0, y0, x1, y1)
{
    return sqrtf((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}