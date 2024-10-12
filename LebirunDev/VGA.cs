using Cosmos.System.Graphics.Fonts;
using Cosmos.System.Graphics;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sys = Cosmos.System;

namespace LebirunDev
{
    public class VGA
    {
        public static Canvas canvas;

        public static void VGAgui()
        {
            canvas = FullScreenCanvas.GetFullScreenCanvas(new Mode(640, 480, ColorDepth.ColorDepth32));
            canvas.Clear();

            canvas.DrawFilledRectangle(Color.White, 0, 460, 640, 20);

            Sys.MouseManager.ScreenWidth = (uint)632;
            Sys.MouseManager.ScreenHeight = (uint)480;
            int X = (int)Sys.MouseManager.X;
            int Y = (int)Sys.MouseManager.Y;
            canvas.DrawString("Reboot", PCScreenFont.Default, Color.Orange, 18, 460);

            canvas.DrawFilledRectangle(Color.FromArgb(125, 0, 25, 255), (int)Sys.MouseManager.X, (int)Sys.MouseManager.Y, 5, 12);

            canvas.Display();

            if (Sys.MouseManager.MouseState == Sys.MouseState.Left && Sys.MouseManager.Y >= 441 && Sys.MouseManager.Y <= 477 && Sys.MouseManager.X >= 10 && Sys.MouseManager.X <= 100)
            {
                Sys.Power.Reboot();
            }

            canvas.Clear();
        }
    }
}
