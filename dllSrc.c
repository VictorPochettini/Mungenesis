#include <stdio.h>
#include <stdlib.h>
#include <time.h>

__declspec(dllexport) int[][] generateSeed(int corners) {
    srand(time(NULL));
    int[2][corners] seed;
    for(int i = 0; i < corners; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            seed[i][j] = rand() % 8500;
        }
    }
    return seed;
}