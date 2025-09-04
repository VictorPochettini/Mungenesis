#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cJSON.h"

// Structure to match C# Vector2
typedef struct {
    float x;
    float y;
} Vector2;

// Function signatures
float DotGrid(Vector2* gradients, int gridWidth, int i, int j, float dx, float dy);

void generateSeed(Vector2* seed, int cornerNumber)
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

void freeSeed(void* ptr);