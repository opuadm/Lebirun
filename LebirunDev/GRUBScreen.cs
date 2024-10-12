using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LebirunDev
{
    class GRUBScreen
    {
        public static void grubText(string grubMode)
        {
            if (grubMode == "normal")
            {
                Console.Clear();
                Console.BackgroundColor = ConsoleColor.White;
                Console.ForegroundColor = ConsoleColor.Black;
                Console.WriteLine("Welcome to GRUB!");
                Console.BackgroundColor = ConsoleColor.Black;
                Console.ForegroundColor = ConsoleColor.White;
                Thread.Sleep(3500);
                Console.Clear();
            }
            else if (grubMode == "noclearafter")
            {
                Console.Clear();
                Console.BackgroundColor = ConsoleColor.White;
                Console.ForegroundColor = ConsoleColor.Black;
                Console.WriteLine("Welcome to GRUB!");
                Console.BackgroundColor = ConsoleColor.Black;
                Console.ForegroundColor = ConsoleColor.White;
                Thread.Sleep(3500);
            }
        }
    }
}
