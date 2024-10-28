using Cosmos.System.Graphics.Fonts;
using Cosmos.System.Graphics;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sys = Cosmos.System;
using Cosmos.System;
using Cosmos.Debug.Kernel.Plugs.Asm;

namespace LebirunDev.GUI
{
    public class VGA
    {
        public static bool menuOpen = false;
        public static bool isLeftMouseButtonPressed = false;
        public static Canvas canvas;

        public static void VGAgui()
        {
            canvas = FullScreenCanvas.GetFullScreenCanvas(new Mode(640, 480, ColorDepth.ColorDepth32));

            canvas.DrawFilledRectangle(Color.MediumPurple, 0, 0, 640, 480);

            canvas.DrawFilledRectangle(Color.GhostWhite, 0, 445, 640, 35);

            canvas.DrawFilledRectangle(Color.FromArgb(40, 0, 13, 54), 4, 450, 70, 25);

            if (menuOpen)
            {
                canvas.DrawFilledRectangle(Color.Firebrick, 4, 280, 200, 165);
            }

            MouseManager.ScreenWidth = 633;
            MouseManager.ScreenHeight = 480;
            int X = (int)MouseManager.X;
            int Y = (int)MouseManager.Y;

            canvas.DrawFilledRectangle(Color.Brown, (int)MouseManager.X, (int)MouseManager.Y, 6, 14);

            if (MouseManager.MouseState == MouseState.Left)
            {
                if (!isLeftMouseButtonPressed)
                {
                    if (MouseManager.X >= 4 && MouseManager.X <= 74 && MouseManager.Y >= 450 && MouseManager.Y <= 475)
                    {
                        menuOpen = !menuOpen;
                    }
                    isLeftMouseButtonPressed = true;
                }
            }
            else
            {
                isLeftMouseButtonPressed = false;
            }

            canvas.Display();

            canvas.Clear();
        }
    }
}
