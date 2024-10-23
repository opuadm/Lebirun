using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LebirunDev
{
    public class BootManagerInput
    {
        public static void BootManagerInputClass()
        {
            ConsoleKeyInfo custominput = Console.ReadKey();

            if (custominput.Key == ConsoleKey.UpArrow)
            {
                if (BootManager.SelectedItem > 1)
                {
                    BootManager.SelectedItem -= 1;
                }
            }

            if (custominput.Key == ConsoleKey.DownArrow)
            {
                if (BootManager.SelectedItem < 2)
                {
                    BootManager.SelectedItem += 1;
                }
            }
            if (custominput.Key == ConsoleKey.Enter)
            {
                if (BootManager.SelectedItem == 1)
                {
                    Kernel.BootMgrOn = false;
                    Console.Clear();
                    Console.WriteLine("Starting Lebirun 0.1.0 Alpha");
                    Thread.Sleep(BootManager.rnd.Next(0, 670));
                    Console.ForegroundColor = ConsoleColor.White;
                    Console.BackgroundColor = ConsoleColor.Black;
                    Console.Clear();
                    LebirunDev.LoginMgr.login();
                }
                else if (BootManager.SelectedItem == 2)
                {
                    Kernel.BootMgrOn = false;
                    Console.Clear();
                    Console.WriteLine("Starting Lebirun 0.1.0 Alpha - GUI Mode");
                    LebirunDev.Kernel.guiON = true;
                    Thread.Sleep(BootManager.rnd.Next(0, 2300));
                    Console.ForegroundColor = ConsoleColor.White;
                    Console.BackgroundColor = ConsoleColor.Black;
                    Console.Clear();
                    LebirunDev.LoginMgr.login();
                }
            }
        }
    }
}
