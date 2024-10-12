using System;
using System.Text;
using Sys = Cosmos.System;
using Cosmos.System.FileSystem;
using Cosmos.System.FileSystem.VFS;

namespace LebirunDev
{
    class FileManager
    {
        // Use the VFS to initialize the file system
        public static CosmosVFS fs = new Sys.FileSystem.CosmosVFS();

        // Method to load the VFS (initialize the file system)
        public static void FSLoad()
        {
            Sys.FileSystem.VFS.VFSManager.RegisterVFS(fs);
            Console.WriteLine("File system initialized.");
        }

        // Method to write data to a file
        public static void WriteToFile(string filePath, string content)
        {
            try
            {
                // Create the file if it doesn't exist
                Sys.FileSystem.VFS.VFSManager.CreateFile(filePath);

                // Get the file stream
                var file = Sys.FileSystem.VFS.VFSManager.GetFile(filePath);
                var fileStream = file.GetFileStream();

                // Check if we can write to the file
                if (fileStream.CanWrite)
                {
                    byte[] textToWrite = Encoding.UTF8.GetBytes(content);
                    fileStream.Write(textToWrite, 0, textToWrite.Length);
                    fileStream.Flush(); // Ensure data is written immediately
                    Console.WriteLine($"File '{filePath}' written successfully.");
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"Error writing to file '{filePath}': {e.Message}");
            }
        }

        // Method to read data from a file
        public static string ReadFromFile(string filePath)
        {
            try
            {
                var file = Sys.FileSystem.VFS.VFSManager.GetFile(filePath);
                var fileStream = file.GetFileStream();

                if (fileStream.CanRead)
                {
                    byte[] textToRead = new byte[fileStream.Length];
                    fileStream.Read(textToRead, 0, (int)fileStream.Length);
                    string fileContent = Encoding.UTF8.GetString(textToRead);
                    Console.WriteLine($"File '{filePath}' read successfully.");
                    return fileContent;
                }
                else
                {
                    Console.WriteLine($"File '{filePath}' cannot be read.");
                    return null;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"Error reading file '{filePath}': {e.Message}");
                return null;
            }
        }

        // Save Game1 data to a file
        public static void SaveGame1Data(float coins, float getCoinsValue, float getCoinsMulti)
        {
            string filePath = @"0:\game1data.txt";
            string content = $"Coins:{coins}\nGetCoinsValue:{getCoinsValue}\nGetCoinsMulti:{getCoinsMulti}";

            WriteToFile(filePath, content);
        }

        // Load Game1 data from a file
        public static (float coins, float getCoinsValue, float getCoinsMulti) LoadGame1Data()
        {
            string filePath = @"0:\game1data.txt";
            string content = ReadFromFile(filePath);

            if (content != null)
            {
                try
                {
                    // Parse the content into separate values
                    var lines = content.Split('\n');
                    float coins = float.Parse(lines[0].Split(':')[1]);
                    float getCoinsValue = float.Parse(lines[1].Split(':')[1]);
                    float getCoinsMulti = float.Parse(lines[2].Split(':')[1]);

                    return (coins, getCoinsValue, getCoinsMulti);
                }
                catch (Exception e)
                {
                    Console.WriteLine($"Error parsing game data: {e.Message}");
                    return (0, 1, 1); // Return default values in case of error
                }
            }

            return (0, 1, 1); // Return default values if file content is null
        }
    }
}
