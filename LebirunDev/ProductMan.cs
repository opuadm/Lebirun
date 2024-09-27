using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LebirunDev
{
    class ProductMan
    {
        Kernel kernelMain = new Kernel();
        public string[] productKeys = {"2K9M4S-M52QP2-J6K3P2-R5TTKL-P1OLD4", "SK39K2-KS9-M39L", "STINGRAY-SPECIAL_HACK", "SLOS-SOPK-LOPS-ZNMI-IDDS", "#_2K28M"};
        public bool isActivated = false;
        public static int ProductKeyNumber = 0;
        public void ProductManWizard()
        {
            Console.Write("Write the product key here: ");
            var input = Console.ReadLine();

            if (input == productKeys[0])
            {
                isActivated = true;
                ProductKeyNumber = 1;
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[0]);
            }
            else if (input == productKeys[1])
            {
                isActivated = true;
                ProductKeyNumber = 2;
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[1]);
            }
            else if (input == productKeys[2])
            {
                isActivated = true;
                ProductKeyNumber = 3;
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[2]);
            }
            else if (input == productKeys[3])
            {
                isActivated = true;
                ProductKeyNumber = 4;
                Console.WriteLine("Activated successfully! Your current product key is: " + productKeys[3]);
            }
            else if (input == productKeys[4])
            {
                isActivated = true;
                ProductKeyNumber = 5;
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
    }
}