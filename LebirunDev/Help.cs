using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LebirunDev
{
    class Help
    {
        public static void ShowHelpPage1()
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
            Console.WriteLine("productman = A wizard for activating your current Lebirun Session");
            Console.WriteLine("logout / logoff - Logs out the user");
            Console.WriteLine("whoami = Sends the username of current logged in user");
            Console.WriteLine("su <user> = Switches to a other user without having to fully logout (use su prv to go back to the previous user)");
            Console.WriteLine("website = Outputs the lebirun website link");
            Console.WriteLine("infofetch = Outputs some of the OS Info");
        }

        public static void ShowHelpPage2()
        {
            Console.WriteLine("List of commands (Page 2):");
            Console.WriteLine("man <command> = Get info about a specific command");
            Console.WriteLine("shutdown / poweroff = Shuts down the computer");
            Console.WriteLine("reboot / restart = Shuts down the computer, and then starts it again.");
        }

        public static void HelpGrub()
        {
            Console.WriteLine("Current GRUB Arguments:");
            Console.WriteLine("--noclearafter = Terminal wont clear after finishing displaying the Welcome to GRUB! text.");
        }

        public static void HelpProductman()
        {
            Console.WriteLine("Current Productman Arguments:");
            Console.WriteLine("--deactivate = Makes the system be no longer activated, which sets isActivated bool to false and ProductKey to none.");
        }
    }
}
