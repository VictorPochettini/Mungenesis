#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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
    int id;
    Vector2 gradientV;
    Vector2 position;
} BlittableCorner;

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
    int plateId;
    int plateType; // 0 for oceanic, 1 for continental
    int coolDown;
} BlittablePlate;

Vector2 d(float x, float y, float x0, float y0)
{
    Vector2 result;
    result.x = x - x0;
    result.y = y - y0;
    result.value = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
    return result;
}

float fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float interpolation(
    float corner1Value, float corner2Value, float corner3Value, float corner4Value,
    int line, int column, int cellLine, int cellColumn,
    int worldSize, int variation)
{
    // Calculate position within the current cell (0.0 to 1.0)
    float cellWidth = (float)worldSize / variation;
    float cellHeight = (float)worldSize / variation;

    float localX = ((float)column - (cellColumn * cellWidth)) / cellWidth;
    float localY = ((float)line - (cellLine * cellHeight)) / cellHeight;

    if (localX < 0.0f)
        localX = 0.0f;
    if (localX > 1.0f)
        localX = 1.0f;
    if (localY < 0.0f)
        localY = 0.0f;
    if (localY > 1.0f)
        localY = 1.0f;

    // Apply fade function for smooth interpolation
    float u = fade(localX);
    float v = fade(localY);

    // Bilinear interpolation
    float ix1 = lerp(corner1Value, corner2Value, u); // Top edge
    float ix2 = lerp(corner3Value, corner4Value, u); // Bottom edge

    return lerp(ix1, ix2, v); // Final interpolation between top and bottom
}

// Function implementations
EXPORT void PerlinNoise(BlittableCell *cells, int worldSize, int *map, int variation, int octave)
{

    int linePerCell = worldSize / variation;

    int line;
    int column;
    int currentLine;
    int currentColumn;
    int cellIndex;
    int currCellLine;
    int currCellColumn;
    int currCellIndex;
    int total;
    float cellLine;
    float cellColumn;
    float frequency;
    float maxValue;
    float amplitude;
    float value;
    float persistence = 0.5;
    float corner1Value, corner2Value, corner3Value, corner4Value;
    BlittableCell *cell;

    for (int i = 0; i < worldSize * worldSize; i++)
    {
        value = 0.0;
        maxValue = 0.0;
        frequency = 2.0f;
        line = i / worldSize;
        column = i % worldSize;
        cellLine = line / linePerCell;
        cellColumn = column / linePerCell;
        cellIndex = cellLine * variation + cellColumn;

        for (int j = 0; j < octave; j++)
        {
            frequency = powf(2.0, (float)j);
            amplitude = powf(persistence, (float)j);

            int a = (int)(line * frequency);
            int b = (int)(column * frequency);
            int l = line;
            int c = column;
            /*
            do
            {

                if(a % worldSize == 0)
                {
                    l -= 10;
                    a = l * frequency;
                }
                if(b % worldSize == 0)
                {
                    c -= 10;
                    b = c * frequency;
                }

            } while (a % worldSize == 0 || b % worldSize == 0);
            */

            currentLine = a % worldSize;
            currentColumn = b % worldSize;
            currCellLine = currentLine / linePerCell;
            currCellColumn = currentColumn / linePerCell;

            // Clamp to valid cell range
            if (currCellLine >= variation)
                currCellLine = variation - 1;
            if (currCellColumn >= variation)
                currCellColumn = variation - 1;

            currCellIndex = (currCellLine * variation) + currCellColumn;

            Vector2 d0 = d((float)currentColumn, (float)currentLine, cells[currCellIndex].corner0.position.x, cells[currCellIndex].corner0.position.y);
            Vector2 d1 = d((float)currentColumn, (float)currentLine, cells[currCellIndex].corner1.position.x, cells[currCellIndex].corner1.position.y);
            Vector2 d2 = d((float)currentColumn, (float)currentLine, cells[currCellIndex].corner2.position.x, cells[currCellIndex].corner2.position.y);
            Vector2 d3 = d((float)currentColumn, (float)currentLine, cells[currCellIndex].corner3.position.x, cells[currCellIndex].corner3.position.y);

            corner1Value = cells[currCellIndex].corner0.gradientV.x * d0.x + cells[currCellIndex].corner0.gradientV.y * d0.y;
            corner2Value = cells[currCellIndex].corner1.gradientV.x * d1.x + cells[currCellIndex].corner1.gradientV.y * d1.y;
            corner3Value = cells[currCellIndex].corner2.gradientV.x * d2.x + cells[currCellIndex].corner2.gradientV.y * d2.y;
            corner4Value = cells[currCellIndex].corner3.gradientV.x * d3.x + cells[currCellIndex].corner3.gradientV.y * d3.y;

            maxValue += amplitude;
            value += interpolation(
                         corner1Value, corner2Value, corner3Value, corner4Value,
                         currentLine, currentColumn, currCellLine, currCellColumn,
                         worldSize, variation) *
                     amplitude;
        }

        int resultado = (int)(value / maxValue); //< 0 ? value + 12 : (variation * 3) * value;
        map[i] = resultado;
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

// POCHETTINI ALGORITHM ==========================================================================================================================

struct spreadQueue
{
    BlittablePlate** init;
    BlittablePlate** end;

    struct queueItem* firstItem;
};

struct queueItem
{
    struct queueItem** prev;
    struct queueItem** pos;
    BlittablePlate plate;
};

struct spreadQueue createQueue(BlittablePlate plate)
{
    struct spreadQueue queue;
    queue.init = &plate;
    queue.end = &plate;

    struct queueItem* item = malloc(sizeof(struct queueItem));
    item->prev = item;
    item->pos = item;
    item->plate = plate;
    queue.firstItem = item;
    

    return queue;
}

void addQueue(struct spreadQueue queue, BlittablePlate plate)
{
    if(queue.init == NULL)
    {
        createQueue(plate);
    }
    else
    {
        struct queueItem* item = queue.firstItem;
        while(item->pos != NULL)
        {
            item = item->pos;
        }

        struct queueItem* post = malloc(sizeof(struct queueItem));
        post->prev = item;
        post->plate = plate;
        queue.end = &plate;
        item->pos = post;
        
    }

}

void initializeMap(int *map, int n, int worldArea, BlittablePlate *plates)
{
    int initialIndex = 0;
    struct spreadQueue queue;

    for(int i = 1; i < n; i++)
    {
        initialIndex = rand() % worldArea;
        plates[initialIndex].plateId = i;

        if(rand() % 10 < 3)
        {
            plates[initialIndex].plateType = 0;
        }
        else
        {
            plates[initialIndex].plateType = 1;
        }

        if(i == 1)
            queue = createQueue(plates[initialIndex]);
        else
        ;
    }
}

EXPORT void PochettiniAlgorithm(BlittablePlate *plates, int worldSize, int *map)
{
    int platesNumber = 9;
    int line, column;
    int flag = 1;
    int worldArea = worldSize * worldSize;
    float percentage = 0.0f;
    BlittablePlate *adjacents[4];
    srand(time(NULL));

    struct spreadQueue queue;

    // Defines where the plates will be initially positioned
    for (int i = 1; i < 10; i++)
    {
        int initialIndex = rand() % (worldArea);
        plates[initialIndex].plateId = i;
        if (rand() % 10 < 3) // 30% chance to be type 0
        {
            plates[initialIndex].plateType = 0;
        }
        else
        {
            plates[initialIndex].plateType = 1;
        }
    }

    // Fills the rest of the map with plates
    while (flag)
    {
        flag = 0;
        for (int i = 0; i < worldArea; i++)
        {
            if (plates[i].plateId == 0)
            {
                flag = 1;
                continue;
            }

            line = i / worldSize;
            column = i % worldSize;
            percentage = (float)i / (float)(worldArea) * 100.0f;
            if (percentage < 20 || percentage >= 80)
            {
                adjacents[0] = &plates[((line - 1 < 0 ? line : line - 1) % worldSize) * worldSize + column];
                adjacents[1] = &plates[((line + 1 > worldSize ? line : line + 1) % worldSize) * worldSize + column];
                adjacents[2] = &plates[line * worldSize + ((column - 1 + worldSize) % worldSize)];
                adjacents[3] = &plates[line * worldSize + ((column + 1) % worldSize)];
            }
            else
            {
                adjacents[0] = &plates[line * worldSize + ((column - 1 + worldSize) % worldSize)];
                adjacents[1] = &plates[line * worldSize + ((column + 1) % worldSize)];
                adjacents[2] = &plates[((line - 1 + worldSize) % worldSize) * worldSize + column];
                adjacents[3] = &plates[((line + 1) % worldSize) * worldSize + column];
            }

            checkAdjecent(&plates[i], plates, worldSize, adjacents);
            plates[i].coolDown = 0;
            map[i] = plates[i].plateId;
        }
    }   
}
