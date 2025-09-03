using System;
using System.Runtime.InteropServices;

namespace PerlinWorld
{
    class World
    {
        public int[,] map = new int[1080, 1080];
        public Vector2[,] grid;
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
        public static extern IntPtr generateSeed(int corners);

        [DllImport("MyCLibrary.dll")]
        public static extern void freeSeed(IntPtr ptr);

        static Cell[] initializeGrid(int squares, int cornerNumber, out Corner[] corners)
        {
            int length = Math.Sqrt(squares);
            int cornerPerLength = length + 1;
            Corner[] corners = new Corner[cornerNumber];
            Cell[] cells = new Cell[squares];

            for (int i = 0; i < cornerNumber; i++)
            {
                corners[i] = new Corner(i);
            }

            for (int i = 0; i < length; i++)
            {
                for (int j = 0; j < cornerPerLength; j++)
                {
                    Cell cell = new Cell(i);
                    cell.c[0] = corners[i * cornerPerLength + j];
                    cell.c[1] = corners[i * cornerPerLength + j + 1];
                    cell.c[2] = corners[(i + 1) * cornerPerLength + j];
                    cell.c[3] = corners[(i + 1) * cornerPerLength + j + 1];
                    cells[i * cornerPerLength + j] = cell;
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

            int squares = (int)Math.Pow(variation + 1, 2);
            int cornerNumber = (int)Math.Sqrt(squares + 1);
            Vector2 seed = new Vector2[cornerNumber];


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
                        seed = generateSeed(corners);
                        flag = true;
                        break;

                    default:
                        Console.WriteLine("Choose a valid option, please. This is adult talking.");
                        break;
                }

            } while (!flag);

            World world = new World();
            Corner[] corners = new Corner[cornerNumber];
            Cell[] cells = initializeGrid(squares, cornerNumber, out corners);

            foreach (Corner corner in corners)
            {
                corner.gradientV.X = seed[i, 0];
                corner.gradientV.Y = seed[i, 1];
                i++;
            }

            
        }
    }
}
