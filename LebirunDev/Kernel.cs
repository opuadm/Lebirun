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
        Canvas canvas;

        protected override void BeforeRun()
        {
            LebirunDev.FileManager.FSLoad();
            Console.Clear();
            LebirunDev.BootManager.SetRes();
            LebirunDev.BootManager.BootMenu();
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
            else
            {
                Console.ForegroundColor = ConsoleColor.Gray;
                Console.Write(LebirunDev.LoginMgr.currentLoggedInUser + "@lebirun");
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
                        LebirunDev.Help.ShowHelpPage1();
                    }
                    else if (input == "help 2" || input == "cmds 2")
                    {
                        LebirunDev.Help.ShowHelpPage2();
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Invalid Help/Cmds Page. The Current Pages in Help/Cmds are: 2. If you want info about a specific command, then type man <command> (not all commands have arguments)");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input.StartsWith("man"))
                {
                    if (input == "man grub")
                    {
                        LebirunDev.Help.HelpGrub();
                    }
                    else if (input == "man productman")
                    {
                        LebirunDev.Help.HelpProductman();
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("The command you provided is not available on man command, please try a different command (and if you just typed nothing on man then you need to do man <command>");
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
                        LebirunDev.GRUBScreen.grubText(grubMode: grubMode);
                    }
                    else if (input == "grubrescue")
                    {
                        LebirunDev.GrubRescueFake.grubRescueFake();
                    }
                    else
                    {
                        grubMode = "normal";
                        LebirunDev.GRUBScreen.grubText(grubMode: grubMode);
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
                    LebirunDev.Game1.FSInit();
                }
                else if (input == "vga")
                {
                    guiON = true;
                }
                else if (input == "whoami")
                {
                    Console.WriteLine(LebirunDev.LoginMgr.currentLoggedInUser);
                }
                else if (input == "logout" || input == "logoff")
                {
                    Console.Clear();
                    Console.WriteLine("Logging off...");
                    Thread.Sleep(250);
                    Console.Clear();
                    LebirunDev.LoginMgr.login();
                }
                else if (input.StartsWith("su"))
                {
                    if (input == "su root")
                    {
                    loginsuroot:
                        if (LebirunDev.LoginMgr.currentLoggedInUser == "root")
                        {
                            return;
                        }
                        else
                        {
                            Console.Write("Password for root: ");
                        }
                        var inputsu1 = Console.ReadLine();
                        if (LebirunDev.LoginMgr.currentLoggedInUser == "root")
                        {
                            Console.WriteLine("You are already root!");
                        }
                        else
                        {
                            if (inputsu1 == "admin")
                            {
                                LebirunDev.LoginMgr.currentLoggedInUser = "root";
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
                        if (LebirunDev.LoginMgr.currentLoggedInUser == LebirunDev.LoginMgr.previousLoggedInUser)
                        {
                            Console.WriteLine("You are already the previous user!");
                        }
                        else
                        {
                            LebirunDev.LoginMgr.currentLoggedInUser = LebirunDev.LoginMgr.previousLoggedInUser;
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
                    Console.WriteLine("Game 1 Coins: " + LebirunDev.Game1.Coins);
                    Console.WriteLine("Product Key: " + LebirunDev.ProductMan.CurrentProductKey);
                }
                else if (input == "productman")
                {
                    if (LebirunDev.ProductMan.isActivated == false)
                    {
                        LebirunDev.ProductMan.ProductManWizard();
                    }
                    else
                    {
                        Console.WriteLine("You already have a product key!");
                    }
                }
                else if (input == "productman --crack")
                {
                    if (LebirunDev.ProductMan.isActivated == false)
                    {
                        LebirunDev.ProductMan.InstantUnlock();
                    }
                    else
                    {
                        Console.WriteLine("You already have a product key!");
                    }
                }
                else if (input == "productman --deactivate")
                {
                    if (LebirunDev.ProductMan.isActivated == true)
                    {
                        LebirunDev.ProductMan.DeUnlock();
                    }
                    else
                    {
                        Console.WriteLine("You dont have any product key! So you cannot deactivate the system right now.");
                    }

                }
                else if (input == "reboot" || input == "restart")
                {
                    Sys.Power.Reboot();
                }
                else if (input == "shutdown" || input == "poweroff")
                {
                    Sys.Power.Shutdown();
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Invalid IPTV Command. Type help or cmds for the list of commands.");
                    Console.ForegroundColor = ConsoleColor.White;
                }
            }
        }
    }
}
