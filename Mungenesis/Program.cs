using System;
using System.Runtime.InteropServices;
using System.Text.Json;

namespace PerlinWorld
{
    class World
    {
        public int[,] map = new int[1080, 1080];
        public Vector2[,]? grid;
    }

    class Cell
    {
        public Corner[] c = new Corner[4];
        int layer;
        public Cell(int layer)
        {
            this.layer = layer;
        }
    }

    class Corner
    {
        public int id;
        public Vector2 gradientV;
        public Corner(int id)
        {
            this.id = id;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float X;
        public float Y;
    }

    class Program
    {
        [DllImport("MyCLibrary.dll")]
        public static extern float DotGrid([In] Vector2[] gradients, int gridWidth, int i, int j, float dx, float dy);

        [DllImport("MyCLibrary.dll")]
        public static extern void generateSeed([Out] Vector2[] seed, int cornerNumber);

        [DllImport("MyCLibrary.dll")]
        public static extern void freeSeed(IntPtr ptr);

        static Cell[] initializeGrid(int squares, int cornerNumber, out Corner[] corners)
        {
            int length = (int)Math.Sqrt(squares);
            int cornerPerLength = length + 1;
            corners = new Corner[cornerNumber];
            Cell[] cells = new Cell[squares];

            for (int i = 0; i < cornerNumber; i++)
            {
                corners[i] = new Corner(i);
            }

            for (int i = 0; i < length; i++)
            {
                for (int j = 0; j < length; j++)
                {
                    Cell cell = new Cell(i);
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
            int variation = 0;
            bool flag = false;

            // Ask user for smoothness level
            do
            {
                Console.WriteLine("In a scale of 1 to 6, how smooth do you want your world to be?\n");
                Console.WriteLine("1 is a smooth world with subtle terrain variations. 6 is a varied world, with sudden transitions");
                Console.Write("Insert answer: ");

                if (!int.TryParse(Console.ReadLine(), out variation) || variation < 1 || variation > 6)
                {
                    Console.WriteLine("As much as I like new ideas, our developer (myself) hasn't yet figured out how to support this option. Press enter and try again!");
                    Console.ReadLine();
                    continue;
                }

                flag = true;

            } while (!flag);

            flag = false;

            //Adding +1 do variation so if a user chooses 1, the world will be 2x2 squares
            variation++;
            int squares = (int)Math.Pow(variation, 2);
            int cornerNumber = (int)Math.Pow(variation + 1, 2);
            Vector2[] seed = new Vector2[cornerNumber];

            World world = new World();
            Corner[] corners = new Corner[cornerNumber];
            Cell[] cells = initializeGrid(squares, cornerNumber, out corners);


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

            foreach(Corner corner in corners)
                {
                    corner.gradientV = seed[corner.id];
                }

            } while (!flag);
            for (int i = 0; i < cornerNumber; i++)
            {
                Console.WriteLine("X: " + seed[i].X);
                Console.WriteLine("Y: " + seed[i].Y);
                Console.WriteLine("Angle: " + Math.Atan2(seed[i].Y, seed[i].X) * (180.0 / Math.PI));
                Console.WriteLine("Length: " + Math.Sqrt(seed[i].X * seed[i].X + seed[i].Y * seed[i].Y));
                Console.WriteLine("===============================");
            }

            string json = JsonSerializer.Serialize(cells);
            string path = "cells.json";

            File.WriteAllText(path, json);

            foreach (Cell cell in cells)
            {
                for (int i = 0; i < 1080 / squares; i++)
                {
                    for (int j = 0; j < 1080 / squares; j++)
                    {

                    }
                }
            }


        }
    }
}
