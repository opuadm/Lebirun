using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Sys = Cosmos.System;
using Cosmos.System.FileSystem;
using Cosmos.System.FileSystem.VFS;
using System.Net.Security;

namespace LebirunDev
{
    class Game1
    {
        public static float GetCoinsMulti = 1;
        public static float GetCoinsValue = 1;
        public static float Coins = 0;
        private static readonly Random random = new Random();

        public static void FSInit()
        {
            Console.WriteLine("Entered Game 1! (Data will be lost after a reboot/shutdown)");
            TheGameItself();
        }

        public static void TheGameItself()
        {
            Console.Write("game1 > ");

            var input2 = Console.ReadLine();

            if (input2 == "" || input2.StartsWith(" "))
            {
                Console.WriteLine("+" + (GetCoinsValue * GetCoinsMulti) + " Coins");
                Coins += GetCoinsValue * GetCoinsMulti;
            }
            else if (input2 == "bal" || input2 == "balance")
            {
                Console.WriteLine("You have " + Coins + " Coins.");
            }
            else if (input2 == "data save")
            {
                FileManager.SaveGame1Data(Coins, GetCoinsValue, GetCoinsMulti);
                Console.WriteLine("Game data saved successfully.");
            }
            else if (input2 == "data load")
            {
                (Coins, GetCoinsValue, GetCoinsMulti) = FileManager.LoadGame1Data();
                Console.WriteLine($"Data Loaded: Coins={Coins}, GetCoinsValue={GetCoinsValue}, GetCoinsMulti={GetCoinsMulti}");
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
                    Console.WriteLine("Product Key - 20000 Coins (ID: 6)");
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
                    if (Coins > 25)
                    {
                        Console.WriteLine("Successful!");
                        GetCoinsMulti += 1.03f;
                        Coins += -25;
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
                    if (Coins > 230)
                    {
                        Console.WriteLine("Successful!");
                        GetCoinsMulti += 1.18f;
                        Coins += -230;
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
                    if (Coins > 450)
                    {
                        Console.WriteLine("Successful!");
                        GetCoinsValue += 4;
                        Coins += -450;
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
                    if (Coins > 800)
                    {
                        Console.WriteLine("Successful!");
                        GetCoinsValue += 12;
                        GetCoinsMulti += 1.4f;
                        Coins += -800;
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
                    if (Coins > 4000)
                    {
                        Console.WriteLine("Successful!");
                        GetCoinsMulti += 2;
                        Coins += -4000;
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Not Enough Coins to buy this!");
                        Console.ForegroundColor = ConsoleColor.White;
                    }
                }
                else if (input2 == "buy 6")
                {
                    if (Coins > 20000 && LebirunDev.ProductMan.isActivated == false)
                    {
                        Console.WriteLine("Successful!");
                        LebirunDev.ProductMan.isActivated = true;

                        var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
                        var stringLength = 5;

                        string GenerateRandomString(int length)
                        {
                            var stringChars = new char[length];
                            for (int i = 0; i < length; i++)
                            {
                                stringChars[i] = chars[random.Next(chars.Length)];
                            }
                            return new String(stringChars);
                        }

                        var finalString1 = GenerateRandomString(stringLength);
                        var finalString2 = GenerateRandomString(stringLength);
                        var finalString3 = GenerateRandomString(stringLength);
                        var finalString4 = GenerateRandomString(stringLength);
                        var finalString5 = GenerateRandomString(stringLength);

                        var finalProductKeyResult = (finalString1 + "-" + finalString2 + "-" + finalString3 + "-" + finalString4 + "-" + finalString5);

                        LebirunDev.ProductMan.CurrentProductKey = finalProductKeyResult;

                        Coins += -20000;
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
                    Coins += GetCoinsValue * GetCoinsMulti + 2.35f * 4f;
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
                LebirunDev.Kernel.game1On = false;
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
                    Coins += GetCoinsValue * GetCoinsMulti;
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
