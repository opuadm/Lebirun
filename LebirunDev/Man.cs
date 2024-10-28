using System;
using System.Collections.Generic;

namespace LebirunDev
{
    public class Man
    {
        public static bool isRunning = true;
        private static int currentPageIndex = 0;
        private static int scrollOffset = 0;
        private static int visibleLines = Console.WindowHeight - 2;
        private static List<List<string>> pages = new List<List<string>>();
        public static string CurCommand = null;
        public enum DisplayMode
        {
            PageView,
            CommandEntry,
            KeybindsLocked
        }

        private static DisplayMode mode = DisplayMode.PageView;
        private static string commandEntryText = "";

        private static Dictionary<string, int> commandToPageMap = new Dictionary<string, int>
        {
            { "help", 0 },
            { "clear", 1 },
            { "grub", 2 },
            { "grubrescue", 3 },
            { "game1", 4 },
            { "productman", 5 },
            { "su", 6 }
        };

        public static void InitializePages()
        {
            pages.Add(new List<string>
            {
                "About the Help / Cmds command",
                "",
                "Nothing much to explain, Help / Cmds command shows just the list of commands."
            });

            pages.Add(new List<string>
            {
                "About the Clear / Minireboot Command",
                "",
                "Clear (aka Minireboot) is a command which just simply clears your console history, literally just that people cant see what commands you typed before."
            });

            pages.Add(new List<string>
            {
                "The GRUB Command",
                "",
                "This command was inspired by the Welcome to GRUB! text in Debian.",
                "",
                "This command has arguments:",
                "--noclearafter just doesnt clear the Welcome to GRUB! text after some time has passed, unless you do Clear (aka Minireboot) command."
            });

            pages.Add(new List<string>
            {
                "GRUBRescue",
                "",
                "This is a command inspired by GRUB itself, it enters a fake Grub Rescue enviornment.",
                "",
                "You can exit grubrescue enviornment by typing exit."
            });

            pages.Add(new List<string>
            {
                "About Game1",
                "",
                "I made this game because i had no ideas on what to do."
            });

            pages.Add(new List<string>
            {
                "Productman - The Activator for Lebirun",
                "",
                "First of all, i made this activator for fun. You can check source code for available keys.",
                "",
                "Productman has the following arguments:",
                "--deactivate, which removes the current Product Key assigned.",
                "Also, there's a very secret argument which instantly activates Lebirun."
            });

            pages.Add(new List<string>
            {
                "The SU Command",
                "",
                "This command is inspired by Linux, it basically just switches the user in the current terminal without logging out of the user fully.",
                "",
                "This command has no arguments."
            });
        }

        public static void ManPagesHandler()
        {
            isRunning = true;
            if (CurCommand != null && commandToPageMap.ContainsKey(CurCommand))
            {
                currentPageIndex = commandToPageMap[CurCommand];
            }
            scrollOffset = 0;
            while (isRunning)
            {
                Console.Clear();
                DisplayCurrentPage();
                HandleInput();
            }
        }

        private static void ExecuteCommand(string command)
        {
            if (command == "exit")
            {
                ExitMan();
            }
            else
            {
                Console.WriteLine("Unknown command.");
            }
        }

        private static void DisplayCurrentPage()
        {
            Console.Clear();
            var pageContent = pages[currentPageIndex];
            int totalLines = pageContent.Count;
            int linesToDisplay = Math.Min(visibleLines, totalLines - scrollOffset);

            for (int i = 0; i < linesToDisplay; i++)
            {
                Console.WriteLine(pageContent[scrollOffset + i]);
            }

            if (mode == DisplayMode.CommandEntry)
            {
                Console.SetCursorPosition(0, Console.WindowHeight - 1);
                Console.BackgroundColor = ConsoleColor.Gray;
                Console.ForegroundColor = ConsoleColor.Black;
                Console.Write("Command: " + commandEntryText.PadRight(Console.WindowWidth - 9));
                Console.ResetColor();
            }
        }

        private static void HandleInput()
        {
            var key = Console.ReadKey(true);

            if (mode == DisplayMode.PageView)
            {
                switch (key.Key)
                {
                    case ConsoleKey.UpArrow:
                        ScrollUp();
                        break;
                    case ConsoleKey.DownArrow:
                        ScrollDown();
                        break;
                    case ConsoleKey.LeftArrow:
                        SwitchPage(-1);
                        break;
                    case ConsoleKey.RightArrow:
                        SwitchPage(1);
                        break;
                    case ConsoleKey.V:
                        mode = DisplayMode.KeybindsLocked;
                        break;
                    case ConsoleKey.C:
                        mode = DisplayMode.CommandEntry;
                        break;
                    case ConsoleKey.Q:
                        ExitMan();
                        break;
                }
            }
            else if (mode == DisplayMode.CommandEntry)
            {
                if (key.Key == ConsoleKey.Enter)
                {
                    ExecuteCommand(commandEntryText);
                    commandEntryText = "";
                    Console.Clear();
                }
                else if (key.Key == ConsoleKey.Backspace && commandEntryText.Length > 0)
                {
                    commandEntryText = commandEntryText.Remove(commandEntryText.Length - 1);
                }
                else if (key.Key == ConsoleKey.C)
                {
                    mode = DisplayMode.PageView;
                }
                else
                {
                    commandEntryText += key.KeyChar;
                }
            }
            else if (mode == DisplayMode.KeybindsLocked)
            {
                if (key.Key == ConsoleKey.V)
                {
                    mode = DisplayMode.PageView;
                }
            }
        }

        private static void ScrollUp()
        {
            if (scrollOffset > 0)
            {
                scrollOffset--;
            }
        }

        private static void ScrollDown()
        {
            var currentPageContent = pages[currentPageIndex];
            if (scrollOffset + visibleLines < currentPageContent.Count)
            {
                scrollOffset++;
            }
        }

        private static void SwitchPage(int direction)
        {
            currentPageIndex += direction;
            if (currentPageIndex < 0) currentPageIndex = pages.Count - 1;
            if (currentPageIndex >= pages.Count) currentPageIndex = 0;
            scrollOffset = 0;
        }

        private static void ExitMan()
        {
            Console.WriteLine("Are you sure you want to quit? (y/n)");
            var confirmKey = Console.ReadKey(true);
            if (confirmKey.Key == ConsoleKey.Y)
            {
                isRunning = false;
                Kernel.ManOn = false;
            }
        }
    }
}
