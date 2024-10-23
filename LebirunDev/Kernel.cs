using Cosmos.System.FileSystem;
using Cosmos.System.Graphics;
using Cosmos.System.Graphics.Fonts;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Threading;
using Sys = Cosmos.System;
using System.IO;
using Cosmos.Core_Plugs.System;
using Cosmos.System.FileSystem.VFS;
using System.Security.Cryptography.X509Certificates;

namespace LebirunDev
{
    public class Kernel : Sys.Kernel
    {
        public static bool grubRescueOn = false;
        float blankSpacesUsed = 0;
        string grubMode = "normal";
        public static bool game1On = false;
        public static bool guiON = false;
        public static string CurrentVersion = "0.1.0 Beta 2";
        public static bool BootMgrOn = false;
        Canvas canvas;

        protected override void BeforeRun()
        {
            LebirunDev.FileManager.FSLoad();
            Console.Clear();
            LebirunDev.BootManager.SetRes();
            LebirunDev.BootManager.BootMenu();
            BootMgrOn = true;
        }

        protected override void Run()
        {

            if (grubRescueOn)
            {
                LebirunDev.GrubRescueFake.grubRescueFake1();
            }
            else if (game1On)
            {
                LebirunDev.Game1.TheGameItself();
            }
            else if (guiON)
            {
                LebirunDev.VGA.VGAgui();
            }
            else if (BootMgrOn)
            {
                BootManagerInput.BootManagerInputClass();
            }
            else
            {
                MainCMD.CMD();
            }
        }
    }
}
