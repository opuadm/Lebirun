using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace LebirunDev
{
    class ProductMan
    {
        public static string[] productKeys = { "2K9M4S-M52QP2-J6K3P2-R5TTKL-P1OLD4", "SK39K2-KS9-M39L", "STINGRAY-SPECIAL_HACK", "SLOS-SOPK-LOPS-ZNMI-IDDS", "#_2K28M", "SMSIO-DKS-MEM-DLSPO-SMKAL", "FJI___SKKI", "HELLO-WORLD29478-34743-48934", "4873-2388-1822-3831-4884" };
        public static bool isActivated = false;
        public static string CurrentProductKey = "none";

        private static readonly Random random = new Random();

        public static void ProductManWizard()
        {
            Console.Write("Write the product key here: ");
            var input = Console.ReadLine();

            if (input == productKeys[0])
            {
                isActivated = true;
                CurrentProductKey = productKeys[0];
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[0]);
            }
            else if (input == productKeys[1])
            {
                isActivated = true;
                CurrentProductKey = productKeys[1];
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[1]);
            }
            else if (input == productKeys[2])
            {
                isActivated = true;
                CurrentProductKey = productKeys[2];
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[2]);
            }
            else if (input == productKeys[3])
            {
                isActivated = true;
                CurrentProductKey = productKeys[3];
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[3]);
            }
            else if (input == productKeys[4])
            {
                isActivated = true;
                CurrentProductKey = productKeys[4];
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[4]);
            }
            else if (input == "N")
            {
                return;
            }
            else
            {
                Console.WriteLine("Activation was unsuccessful. Press N to exit the product Key wizard.");
                ProductManWizard();
            }
        }

        public static void InstantUnlock()
        {
            var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            var stringLength = 5;

            Console.WriteLine("Activating...");

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

            CurrentProductKey = finalProductKeyResult;

            isActivated = true;

            Console.WriteLine("Activated! Current Product Key is: " + finalProductKeyResult);
        }

        public static void DeUnlock()
        {
            CurrentProductKey = "none";
            isActivated = false;
            Console.WriteLine("Successfully deactivated the system!");
        }
    }
}
