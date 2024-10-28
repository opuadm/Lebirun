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
using Cosmos.Core;
using LebirunDev.GUI;
using LebirunDev.FakeGRUB;

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
        public static bool ManOn = false;
        Canvas canvas;

        protected override void BeforeRun()
        {
            FileManager.FSLoad();
            Console.Clear();
            BootManager.SetRes();
            BootManager.BootMenu();
            BootMgrOn = true;
            Man.InitializePages();
        }

        protected override void Run()
        {

            if (grubRescueOn)
            {
                GrubRescueFake.grubRescueFake1();
            }
            else if (game1On)
            {
                Game1.TheGameItself();
            }
            else if (guiON)
            {
                VGA.VGAgui();
            }
            else if (BootMgrOn)
            {
                BootManagerInput.BootManagerInputClass();
            }
            else if (ManOn)
            {

            }
            else
            {
                MainCMD.CMD();
            }
        }
    }
}
