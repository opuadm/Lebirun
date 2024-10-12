using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LebirunDev
{
    class LoginMgr
    {
        public static string currentLoggedInUser = "none";
        public static string currentSelectUserLogin = "none";
        public static string previousLoggedInUser = "none";
        public static void login()
        {
            Console.Write("Write the user here: ");
            string input = Console.ReadLine();
            if (input == "root")
            {
                Console.Clear();
                currentSelectUserLogin = "root";
                LoginPassword();
            }
            else if (input == "toor")
            {
                Console.Clear();
                Console.WriteLine("Lebirun " + LebirunDev.Kernel.CurrentVersion);
                Thread.Sleep(83);
                Console.WriteLine("Type help / cmds for the list of commands available!");
                currentLoggedInUser = "toor";
                previousLoggedInUser = "toor";
            }
            else if (input == "user")
            {
                Console.Clear();
                Console.WriteLine("Lebirun " + LebirunDev.Kernel.CurrentVersion);
                Thread.Sleep(83);
                Console.WriteLine("Type help / cmds for the list of commands available!");
                currentLoggedInUser = "user";
                previousLoggedInUser = "user";
            }
            else
            {
                Console.WriteLine("Invalid User!");
                login();
            }
        }

        public static void LoginPassword()
        {
            Console.Write("Write the user password here: ");
            string inputlogin1 = Console.ReadLine();
            if (inputlogin1 == "admin")
            {
                if (currentSelectUserLogin == "root")
                {
                    Console.Clear();
                    Console.WriteLine("Lebirun " + LebirunDev.Kernel.CurrentVersion);
                    Thread.Sleep(83);
                    Console.WriteLine("Type help / cmds for the list of commands available!");
                    currentLoggedInUser = "root";
                    previousLoggedInUser = "root";
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
    }
}
