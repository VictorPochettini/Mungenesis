using System;
using System.Runtime.InteropServices;

namespace PerlinWorld
{
    class World
    {
        public int[,] map = new int[1080, 1080];
        public Vector2[,] grid;
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
        public static extern string generateSeed(int corners);

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

            int squares = (int)Math.Sqrt(variation + 1);
            int corners = (int)Math.Sqrt(squares + 1);
            int[, ] seed = new int[corners, 2];
            
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

            // Placeholder: here you would continue with world generation using `variation` and `seed`
            Console.WriteLine($"World will be generated with variation {variation} and seed {seed}");
        }
    }
}
