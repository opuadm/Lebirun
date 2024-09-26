using Cosmos.System.FileSystem;
using Cosmos.System.Graphics;
using Cosmos.System.Graphics.Fonts;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Threading;
using Sys = Cosmos.System;
using FileSystem = Cosmos.System.FileSystem;
using System.IO;

namespace LebirunDev
{
    public class Kernel : Sys.Kernel
    {
        bool grubRescueOn = false;
        float blankSpacesUsed = 0;
        string grubMode = "normal";
        bool game1On = false;
        private float game1GetCoinsMulti = 1;
        private float game1GetCoinsValue = 1;
        private float game1Coins = 0;
        bool guiON = false;
        string currentLoggedInUser = "none";
        string currentSelectUserLogin = "none";
        string previousLoggedInUser = "none";
        string CurrentVersion = "0.1.0 Alpha 2";
        Canvas canvas;

        protected override void BeforeRun()
        {
            Console.Clear();
            login();
        }

        protected void login()
        {
            Console.Write("Write the user here: ");
            string input = Console.ReadLine();
            if (input == "root")
            {
                Console.Clear();
                currentSelectUserLogin = "root";
                LoginPassword();
            }
            else if (input == "user")
            {
                Console.Clear();
                Console.WriteLine("Lebirun " + CurrentVersion);
                Thread.Sleep(83);
                Console.WriteLine("Type help / cmds for the list of commands available!");
                currentLoggedInUser = "user";
                previousLoggedInUser = "user";
                Run();
            }
            else
            {
                Console.WriteLine("Invalid User!");
                login();
            }
        }

        protected void LoginPassword()
        {
            Console.Write("Write the user password here: ");
            string inputlogin1 = Console.ReadLine();
            if (inputlogin1 == "admin")
            {
                if (currentSelectUserLogin == "root")
                {
                    Console.Clear();
                    Console.WriteLine("Lebirun " + CurrentVersion);
                    Thread.Sleep(83);
                    Console.WriteLine("Type help / cmds for the list of commands available!");
                    currentLoggedInUser = "root";
                    previousLoggedInUser = "root";
                    Run();
                }
            }
            else
            {
                if (currentSelectUserLogin == "root")
                {
                    Console.WriteLine("Wrong Password!");
                    LoginPassword();
                }
            }
        }

        protected override void Run()
        {
            if (grubRescueOn)
            {
                grubRescueFake1();
            }
            else if (game1On)
            {
                game1();
            }
            else if (guiON)
            {
                canvas = FullScreenCanvas.GetFullScreenCanvas(new Mode(640, 480, ColorDepth.ColorDepth32));
                canvas.Clear();

                for (uint w = 0; w < 640; w++)
                {
                    for (uint h = 0; h < 50; h++)
                    {
                        int x = (int)w;
                        int y = (int)(500 - h);
                        canvas.DrawPoint(Color.White, x, y);
                    }
                }

                for (uint w = 0; w < 80; w++)
                {
                    for (uint h = 0; h < 25; h++)
                    {
                        int x = (int)(80 - w);
                        int y = (int)(477 - h);
                        canvas.DrawPoint(Color.Gray, x, y);
                    }
                }

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
            else
            {
                Console.ForegroundColor = ConsoleColor.Gray;
                Console.Write(currentLoggedInUser + "@lebirun");
                Console.Write(" >> ");
                Console.ForegroundColor = ConsoleColor.White;

                var input = Console.ReadLine();

                if (input == "iptv")
                {
                    Console.WriteLine("i need IPTV ASAP!!!");
                }
                else if (input.StartsWith("help") || input.StartsWith("cmds"))
                {
                    if (input == "help 1" || input == "cmds 1")
                    {
                        Console.WriteLine("List of Commands:");
                        Console.WriteLine("help / cmds <page> = Shows the Commands");
                        Console.WriteLine("iptv = i need IPTV");
                        Console.WriteLine("metamask = Metamask wallet got hacked");
                        Console.WriteLine("echo <text> = Print the same thing you sent in the second argument");
                        Console.WriteLine("clear / minireboot = Clears the terminal");
                        Console.WriteLine("grub = Displays the fake Welcome to GRUB! screen");
                        Console.WriteLine("grubrescue = Displays the fake grub rescue screen (interactive)");
                        Console.WriteLine("root = R00t");
                        Console.WriteLine("blankspaces = The Amount of Blankspaces commands you have used on this current session");
                        Console.WriteLine("46 = The Number 46");
                        Console.WriteLine("92 = The Number 92");
                        Console.WriteLine("93 = The Number 93");
                        Console.WriteLine("game1 = Game 1");
                        Console.WriteLine("vga = Display GUI");
                        Console.WriteLine("logout / logoff - Logs out the user");
                        Console.WriteLine("whoami = Sends the username of current logged in user");
                        Console.WriteLine("su <user> = Switches to a other user without having to fully logout (use su prv to go back to the previous user)");
                        Console.WriteLine("website = Outputs the lebirun website link");
                        Console.WriteLine("infofetch = Outputs some of the OS Info");
                    }
                    else if (input == "help grub" || input == "cmds grub")
                    {
                        Console.WriteLine("Current GRUB Arguments:");
                        Console.WriteLine("--noclearafter = Terminal wont clear after finishing displaying the Welcome to GRUB! text.");
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Invalid Help/Cmds Page. The Current Pages in Help/Cmds are: 1. You can also type help <command> but its only for some commands.");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input == "metamask")
                {
                    Console.WriteLine("Metamask Wallet got Hacked");
                }
                else if (input.StartsWith("echo "))
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine(input.Remove(0, 5));
                    Console.ForegroundColor = ConsoleColor.White;
                }
                else if (input == "clear" || input == "minireboot")
                {
                    Console.Clear();
                }
                else if (input.StartsWith("grub"))
                {
                    if (input == "grub --noclearafter")
                    {
                        grubMode = "noclearafter";
                        grubText(grubMode: grubMode);
                    }
                    else if (input == "grubrescue")
                    {
                        grubRescueFake();
                    }
                    else
                    {
                        grubMode = "normal";
                        grubText(grubMode: grubMode);
                    }
                }
                else if (input == "root")
                {
                    Console.Write("R00T@K4L1");
                    Console.Write(" R00T@UBUNTU");
                    Console.Write(" R00T@D3B14N");
                    Console.WriteLine();
                }
                else if (input.StartsWith(" "))
                {
                    blankSpacesUsed++;
                }
                else if (input == "")
                {
                    blankSpacesUsed++;
                }
                else if (input == "blankspaces")
                {
                    Console.WriteLine(blankSpacesUsed + " is the Amount of blankspaces you used in this session!");
                }
                else if (input == "unknown1")
                {
                    for (int i = 0; i < 15; i++)
                    {
                        Console.WriteLine(".");
                        Console.WriteLine(".");
                        Console.Write(".");
                    }
                }
                else if (input == "46")
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine("The Number 46");
                    Console.ForegroundColor = ConsoleColor.White;
                }
                else if (input == "92")
                {
                    Console.ForegroundColor = ConsoleColor.Blue;
                    Console.WriteLine("The Number 92");
                    Console.ForegroundColor = ConsoleColor.White;
                }
                else if (input == "93")
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("The Number 93");
                    Console.ForegroundColor = ConsoleColor.White;
                }
                else if (input == "game1")
                {
                    game1On = true;
                    Console.WriteLine("Entered Game 1! (Data will be lost after a reboot/shutdown)");
                }
                else if (input == "rm -rf /* --no-preserve-root")
                {
                    Console.WriteLine("rm: could not remove /tmp: Device or resource busy");
                    Console.WriteLine("rm: could not remove /dev/sda: Device or resource busy")
                }
                else if (input == "rm -rf /*")
                {
                    Console.WriteLine("rm: it is dangerous to operate recursively on '/'");
                    Console.WriteLine("rm: use --no-preserve-root to override this failsafe")
                }
                else if (input == "rm -rvf /* --no-preserve-root")
                {
                    Console.WriteLine("Deleted /boot succesfully");
                    Console.WriteLine("rm: could not remove /dev/sda: Device or resource busy")
                    Console.WriteLine("Deleted /home succesfully")
                }
                else if (input == "vga")
                {
                    guiON = true;
                }
                else if (input == "whoami")
                {
                    Console.WriteLine(currentLoggedInUser);
                }
                else if (input == "logout" || input == "logoff")
                {
                    Console.Clear();
                    Console.WriteLine("Logging off...");
                    Thread.Sleep(250);
                    Console.Clear();
                    login();
                }
                else if (input.StartsWith("su"))
                {
                    if (input == "su root")
                    {
                    loginsuroot:
                        if (currentLoggedInUser == "root")
                        {
                            return;
                        }
                        else
                        {
                            Console.Write("Password for root: ");
                        }
                        var inputsu1 = Console.ReadLine();
                        if (currentLoggedInUser == "root")
                        {
                            Console.WriteLine("You are already root!");
                        }
                        else
                        {
                            if (inputsu1 == "admin")
                            {
                                currentLoggedInUser = "root";
                                Console.WriteLine("Successfully switched to root! Use su prv to switch to the previous user.");
                            }
                            else if (inputsu1 == "n")
                            {
                                Run();
                            }
                            else
                            {
                                Console.WriteLine("Retry. If you dont want to continue then type n");
                                goto loginsuroot;
                            }
                        }
                    }
                    else if (input == "su prv")
                    {
                        if (currentLoggedInUser == previousLoggedInUser)
                        {
                            Console.WriteLine("You are already the previous user!");
                        }
                        else
                        {
                            currentLoggedInUser = previousLoggedInUser;
                            Console.WriteLine("Successfully switched to the previous user!");
                        }
                    }
                }
                else if (input == "website")
                {
                    Console.WriteLine("-> lebirun.000.pe");
                }
                else if (input == "infofetch")
                {
                    Console.WriteLine("Version: Lebirun " + CurrentVersion);
                    Console.WriteLine("Game 1 Coins: " + game1Coins);
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Invalid IPTV Command. Type help or cmds for the list of commands.");
                    Console.ForegroundColor = ConsoleColor.White;
                }
            }
        }
        protected void grubText(string grubMode)
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
                Run();
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
                Run();
            }
        }

        protected void grubRescueFake()
        {
            Console.Clear();
            Console.WriteLine("GRUB Rescue Enviornment (Fake)");
            Console.WriteLine();
            Console.WriteLine();
            grubRescueOn = true;
        }

        protected void grubRescueFake1()
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
                grubRescueOn = false;
            }
            else
            {
                Console.WriteLine("Invalid GRUB Command");
            }
        }

        protected void game1()
        {
            Console.Write("game1 > ");

            var input2 = Console.ReadLine();

            if (input2 == "" || input2.StartsWith(" "))
            {
                Console.WriteLine("+" + (game1GetCoinsValue * game1GetCoinsMulti) + " Coins");
                game1Coins += game1GetCoinsValue * game1GetCoinsMulti;
            }
            else if (input2 == "bal" || input2 == "balance")
            {
                Console.WriteLine("You have " + game1Coins + " Coins.");
            }
            else if (input2.StartsWith("shop"))
            {
                if (input2 == "shop 1")
                {
                    Console.WriteLine("+1.03x coin multiplier - 25 coins (ID: 1)");
                    Console.WriteLine("+1.18x coin multiplier - 230 coins (ID: 2)");
                    Console.WriteLine("+4 Coins Addition - 450 coins (ID: 3)");
                    Console.WriteLine("+12 Coins Addition with +1.4x coin multiplier - 800 coins (ID: 4)");
                    Console.WriteLine("+2x Coin Multiplier - 4000 Coins (ID: 5)");
                    Console.WriteLine("Mod Menu - 120000 Coins (ID: mod)");
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Invalid Shop Pages / Shop Argument. Current Pages in shop: 1");
                    Console.ForegroundColor = ConsoleColor.White;
                }
            }
            else if (input2.StartsWith("buy"))
            {
                if (input2 == "buy 1")
                {
                    if (game1Coins > 25)
                    {
                        Console.WriteLine("Successful!");
                        game1GetCoinsMulti += 1.03f;
                        game1Coins += -25;
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Not Enough Coins to buy this!");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input2 == "buy 2")
                {
                    if (game1Coins > 230)
                    {
                        Console.WriteLine("Successful!");
                        game1GetCoinsMulti += 1.18f;
                        game1Coins += -230;
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Not Enough Coins to buy this!");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input2 == "buy 3")
                {
                    if (game1Coins > 450)
                    {
                        Console.WriteLine("Successful!");
                        game1GetCoinsValue += 4;
                        game1Coins += -450;
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Not Enough Coins to buy this!");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input2 == "buy 4")
                {
                    if (game1Coins > 800)
                    {
                        Console.WriteLine("Successful!");
                        game1GetCoinsValue += 12;
                        game1GetCoinsMulti += 1.4f;
                        game1Coins += -800;
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Not Enough Coins to buy this!");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input2 == "buy 5")
                {
                    if (game1Coins > 4000)
                    {
                        Console.WriteLine("Successful!");
                        game1GetCoinsMulti += 2;
                        game1Coins += -4000;
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Not Enough Coins to buy this!");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input2 == "buy mod")
                {
                    Console.WriteLine("In Progress...");
                    game1Coins += game1GetCoinsValue * game1GetCoinsMulti + 2.35f * 4f;
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Invalid ID");
                    Console.ForegroundColor = ConsoleColor.White;
                }
            }
            else if (input2 == "exit")
            {
                Console.WriteLine("Exiting the game (Your data will be erased after a session end)");
                game1On = false;
            }
            else if (input2.StartsWith("tutorial") || input2.StartsWith("guides"))
            {
                if (input2 == "tutorial 1")
                {
                    Console.WriteLine("If you write nothingness then you will gain 1 coin");
                    Console.WriteLine("The command bal or balance is to display the current amount of coins you have.");
                    Console.WriteLine("The shop command shows things to buy, and Buy command buys a thing (ID)");
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Failed. Current Pages in tutorial: 1, Current Pages in guides: 0");
                    game1Coins += game1GetCoinsValue * game1GetCoinsMulti;
                    Console.ForegroundColor = ConsoleColor.White;
                }
            }
            else
            {
                Console.WriteLine("Invalid Command, Use the command tutorial for commands (or guides for some guides)");
            }
        }
    }
}
