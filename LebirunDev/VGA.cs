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

            canvas.DrawFilledRectangle(Color.MediumPurple, 0, 0, 640, 480);

            canvas.DrawFilledRectangle(Color.GhostWhite, 0, 465, 640, 15);

            Sys.MouseManager.ScreenWidth = (uint)635;
            Sys.MouseManager.ScreenHeight = (uint)480;
            int X = (int)Sys.MouseManager.X;
            int Y = (int)Sys.MouseManager.Y;

            canvas.DrawFilledRectangle(Color.Brown, (int)Sys.MouseManager.X, (int)Sys.MouseManager.Y, 5, 8);

            canvas.Display();

            canvas.Clear();
        }
    }
}
