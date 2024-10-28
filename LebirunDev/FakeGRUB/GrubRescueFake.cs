using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LebirunDev.FakeGRUB
{
    class GrubRescueFake
    {
        public static void grubRescueFake()
        {
            Console.Clear();
            Console.WriteLine("GRUB Rescue Enviornment (Fake)");
            Console.WriteLine();
            Console.WriteLine();
            Kernel.grubRescueOn = true;
        }

        public static void grubRescueFake1()
        {
            Console.Write("grub rescue >> ");

            var input2 = Console.ReadLine();

            if (input2 == "grub2cmds")
            {
                Console.WriteLine("Current GRUB Commands:");
                Console.WriteLine("exit - Exits the grubrescue enviornment");
            }
            else if (input2 == "exit")
            {
                Console.WriteLine("Goodbye!");
                Kernel.grubRescueOn = false;
            }
            else
            {
                Console.WriteLine("Invalid GRUB Command");
            }
        }
    }
}
