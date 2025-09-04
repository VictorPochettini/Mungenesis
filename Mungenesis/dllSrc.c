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
typedef struct {
    float x;
    float y;
} Vector2;

// Function implementations
EXPORT float DotGrid(Vector2* gradients, int gridWidth, int i, int j, float dx, float dy)
{
    // Placeholder implementation
    int index = i * gridWidth + j;
    
    Vector2 gradient = gradients[index];
    
    float dotProduct = gradient.x * dx + gradient.y * dy;
    
    return dotProduct;
}

EXPORT void generateSeed(Vector2* seed, int cornerNumber)
{
    srand((unsigned int)time(NULL)); // Seed the random number generator
    for (int i = 0; i < cornerNumber; i++) 
    {
        seed[i].x = (float)(rand() % 1000) / 1000.0f; // Random float between 0 and 1
        seed[i].y = (float)(rand() % 1000) / 1000.0f; // Random float between 0 and 1

        //Creates the possibility of negative values
        int negative1 = rand() % 2;
        int negative2 = rand() % 2;

        // 50% chance to make the values negative
        seed[i].x = (negative1 == 0) ? seed[i].x : -seed[i].x;
        seed[i].y = (negative2 == 0) ? seed[i].y : -seed[i].y;
    }
}

EXPORT void freeSeed(void* ptr)
{
    // Placeholder implementation for freeing memory
    if (ptr != NULL) {
        free(ptr);
    }
}