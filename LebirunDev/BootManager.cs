using Cosmos.HAL.Drivers.Video;
using Cosmos.System.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Security;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LebirunDev
{
    public class BootManager
    {
        public static float BootPage = 1;
        public static Random rnd = new Random();

        public static void BootMenu()
        {
            if (BootPage == 1)
            {
                Console.ForegroundColor = ConsoleColor.White;
                Console.BackgroundColor = ConsoleColor.Blue;
                Console.Clear();
                Console.WriteLine("Lebirun Boot Menu");
                Console.WriteLine("Type page:<number> for different page | Current Page: 1");
                Console.WriteLine("Normal Boot | 1");
                Console.WriteLine("GUI Boot | 2");
            }
            BootMenuInput();
        }

        public static void SetRes()
        {
            VGAScreen.SetGraphicsMode(Cosmos.HAL.Drivers.Video.VGADriver.ScreenSize.Size720x480, ColorDepth.ColorDepth4);
            VGAScreen.SetTextMode(Cosmos.HAL.Drivers.Video.VGADriver.TextSize.Size80x25);
        }

        public static void BootMenuInput()
        {
            var input = Console.ReadLine();
            if (BootPage == 1)
            {
                if (input == "1")
                {
                    Console.Clear();
                    Console.WriteLine("Starting Lebirun 0.1.0 Alpha");
                    Thread.Sleep(rnd.Next(0, 670));
                    Console.ForegroundColor = ConsoleColor.White;
                    Console.BackgroundColor = ConsoleColor.Black;
                    Console.Clear();
                    LebirunDev.LoginMgr.login();
                }
                else if (input == "2")
                {
                    Console.Clear();
                    Console.WriteLine("Starting Lebirun 0.1.0 Alpha - GUI Mode");
                    LebirunDev.Kernel.guiON = true;
                    Thread.Sleep(rnd.Next(0, 2300));
                    Console.ForegroundColor = ConsoleColor.White;
                    Console.BackgroundColor = ConsoleColor.Black;
                    Console.Clear();
                    LebirunDev.LoginMgr.login();
                }
                else
                {
                    Console.WriteLine("Try Again");
                    BootMenuInput();
                }
            }
        }
    }
}
