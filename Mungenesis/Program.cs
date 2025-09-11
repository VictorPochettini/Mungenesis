using System;
using System.Data;
using System.Runtime.InteropServices;
using System.Text.Json;
using System.IO;

namespace PerlinWorld
{
    public class World
    {
        public Vector2[,]? grid;
    }

    public class Cell
    {
        public Corner[] c = new Corner[4];
        public int layer;
        public int id;
        public Cell(int layer, int id)
        {
            this.layer = layer;
            this.id = id;
        }
    }

    public class Corner
    {
        public int id;
        public Vector2 gradientV;
        public Vector2 position;
        public Corner(int id)
        {
            this.id = id;
        }
    }

    public static class ConversionExtensions

    {
        public static BlittableCell ToBlittable(this Cell cell)
        {
            return new BlittableCell
            {
                corner0 = cell.c[0].ToBlittable(),
                corner1 = cell.c[1].ToBlittable(),
                corner2 = cell.c[2].ToBlittable(),
                corner3 = cell.c[3].ToBlittable(),
                layer = cell.layer // You'll need to make this field public in Cell class
            };
        }

        public static BlittableCorner ToBlittable(this Corner corner)
        {
            return new BlittableCorner
            {
                id = corner.id,
                gradientV = corner.gradientV,
                position = corner.position
            };
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float X;
        public float Y;
        public float value;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BlittableCell
    {
        public BlittableCorner corner0;
        public BlittableCorner corner1;
        public BlittableCorner corner2;
        public BlittableCorner corner3;
        public int layer;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BlittableCorner
    {
        public int id;
        public Vector2 gradientV;
        public Vector2 position;
    }



    class Program
    {
        [DllImport("MyCLibrary.dll")]
        public static extern unsafe void PerlinNoise(BlittableCell* cells, int worldSize, int* map, int layer, int octave);

        [DllImport("MyCLibrary.dll")]
        public static extern void generateSeed([Out] Vector2[] seed, int cornerNumber);

        [DllImport("MyCLibrary.dll")]
        public static extern void freeSeed(IntPtr ptr);

        static Cell[] initializeGrid(int squares, int cornerNumber, int worldSize, out Corner[] corners)
        {
            int length = (int)Math.Sqrt(squares);
            int cornerPerLength = length + 1;
            corners = new Corner[cornerNumber];
            Cell[] cells = new Cell[squares];

            for (int i = 0; i < cornerNumber; i++)
            {
                Corner c = new Corner(i);
                c.position = new Vector2
                {
                    X = (float)(i % cornerPerLength) * ((float)worldSize / (cornerPerLength - 1)),
                    Y = (float)(i / cornerPerLength) * ((float)worldSize / (cornerPerLength - 1))
                };
                corners[i] = c;
            }

            for (int i = 0; i < length; i++)
            {
                for (int j = 0; j < length; j++)
                {
                    int cellIndex = i * length + j;
                    Cell cell = new Cell(i, cellIndex);
                    cell.c[0] = corners[i * cornerPerLength + j];
                    cell.c[1] = corners[i * cornerPerLength + j + 1];
                    cell.c[2] = corners[(i + 1) * cornerPerLength + j];
                    cell.c[3] = corners[(i + 1) * cornerPerLength + j + 1];
                    cells[i * length + j] = cell;
                }
            }
            return cells;
        }

        static void Main(string[] args)
        {
            int variation = 15;
            int octave = 9;
            bool flag = false;
            int worldSize = 4320;
            int[] mapFromC = new int[worldSize * worldSize];
            int[,] map = new int[worldSize, worldSize];
            int algorithmChoice;

            int squares = (int)Math.Pow(variation, 2);
            int cornerNumber = (int)Math.Pow(variation + 1, 2);
            Vector2[] seed = new Vector2[cornerNumber];

            World world = new World();
            Corner[] corners = new Corner[cornerNumber];
            Cell[] cells = initializeGrid(squares, cornerNumber, worldSize, out corners);
            while (true)
            {
                Console.WriteLine("What algorithm would you like to use?");
                Console.WriteLine("1 - Perlin Noise");
                Console.WriteLine("2 - Pochettini Algorithm (Not available yet)");

                if (int.TryParse(Console.ReadLine(), out algorithmChoice) && algorithmChoice == 1)
                {
                    break;
                }
                else if (int.TryParse(Console.ReadLine(), out algorithmChoice) && algorithmChoice == 2)
                {
                    Console.WriteLine("Are you illiterate? I just told you this algorithm isn't available yet!");
                }

                Console.WriteLine("Invalid input. Please enter 1 or 2.");
            }

            // Ask user to insert or generate seed
            do
            {
                Console.WriteLine("Would you like to insert a seed (1) or generate one (2)?");
                int choice;
                if (!int.TryParse(Console.ReadLine(), out choice))
                {
                    Console.WriteLine("Invalid input. Please enter 1 or 2.");
                    continue;
                }

                switch (choice)
                {
                    case 1:
                        Console.WriteLine("Insert seed:");
                        //I have to see what I can do here
                        flag = true;
                        break;

                    case 2:
                        generateSeed(seed, cornerNumber);
                        flag = true;
                        break;

                    default:
                        Console.WriteLine("Choose a valid option, please. This is adult talking.");
                        break;
                }

                foreach (Corner corner in corners)
                {
                    corner.gradientV = seed[corner.id];
                }

            } while (!flag);

            BlittableCell[] blittableCells = new BlittableCell[cells.Length];

            for (int i = 0; i < cells.Length; i++)
            {
                BlittableCell blittableCell = cells[i].ToBlittable();
                blittableCells[i] = blittableCell;
            }

            unsafe
            {

                fixed (BlittableCell* ptr = blittableCells)
                fixed (int* mapPtr = mapFromC)
                {
                    Console.WriteLine("Calling DotGrid...");
                    PerlinNoise(ptr, worldSize, mapPtr, variation, octave);
                    Console.WriteLine("DotGrid completed.");
                }
            }

            // Debug: Check if map has values
            Console.WriteLine($"First map values: {mapFromC[0]}, {mapFromC[1]}, {mapFromC[2]}");
            Console.WriteLine($"Some middle values: {mapFromC[1000]}, {mapFromC[2000]}, {mapFromC[3000]}");

            var objec = new { Seed = seed, Map = mapFromC, Variation = variation - 1, WorldSize = worldSize };
            Console.WriteLine("Creating JSON...");

            try
            {
                string json = JsonSerializer.Serialize(objec);

                Console.WriteLine($"JSON created. Length: {json.Length}");
                //Console.WriteLine($"First 100 chars: {json.Substring(0, Math.Min(100, json.Length))}");

                Console.WriteLine("Writing to file...");
                File.WriteAllText("Front-end/world.json", json);
                Console.WriteLine("File written successfully!");

                // Verify file was written
                if (File.Exists("Front-end/world.json"))
                {
                    long fileSize = new FileInfo("Front-end/world.json").Length;
                    Console.WriteLine($"File exists and is {fileSize} bytes");
                }
                else
                {
                    Console.WriteLine("ERROR: File was not created!");
                }

                string jsContent = $"const worldData = {json};";
                File.WriteAllText("Front-end/world.js", jsContent);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"ERROR: {ex.Message}");
                Console.WriteLine($"Stack trace: {ex.StackTrace}");
            }

            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }
    }
}
