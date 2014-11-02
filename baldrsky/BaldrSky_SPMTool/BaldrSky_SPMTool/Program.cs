using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace BaldrSky_SPMTool
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 3 || args[0][0] != '-')
            {
                Console.WriteLine("usage:\n\t{0} -x infile.dat outfile.txt\n\t{0} -i infile.txt outfile.dat", System.AppDomain.CurrentDomain.FriendlyName);
                return;
            }

            switch (args[0][1])
            {
                case 'x':
                    Disassemble(args[1], args[2]);
                    break;
                case 'i':
                    Assemble(args[1], args[2]);
                    break;
            }
        }

        static void Disassemble(string input, string output)
        {
            if (!File.Exists(input))
            {
                Console.WriteLine("Could not find {0}", input);
                return;
            }

            BinaryReader datFile = new BinaryReader(File.Open(input, FileMode.Open));

            string spmString = Encoding.ASCII.GetString(datFile.ReadBytes(0x0d));
            if (spmString != "SPM VER-2.00\0")
            {
                Console.WriteLine("Not a valid SPM file");
                Environment.Exit(1);
            }

            StreamWriter outfile = new StreamWriter(File.Open(output, FileMode.Create));

            outfile.WriteLine("// Original input file: {0}", input);

            int numOfEntries = datFile.ReadInt32();

            for (int i = 0; i < numOfEntries; i++)
            {
                for (int x = 0; x < 0x64 / 4; x++)
                {
                    int data = datFile.ReadInt32();
                    outfile.Write("<{0:x8}> {1:x8}", x, data);
                    
                    if(x == 0x0c)
                        outfile.Write(" // width");
                    else if(x == 0x0d)
                        outfile.Write(" // height");
                    else if(x == 0x0e)
                        outfile.Write(" // bottom left");
                    else if(x == 0x0e)
                        outfile.Write(" // top left");
                    else if(x == 0x0f)
                        outfile.Write(" // bottom right");
                    else if(x == 0x10)
                        outfile.Write(" // top right");

                    outfile.WriteLine();
                }

                outfile.WriteLine();
            }

            datFile.Close();
            outfile.Close();
        }


        static void Assemble(string input, string output)
        {
            if (!File.Exists(input))
            {
                Console.WriteLine("Could not find {0}", input);
                return;
            }

            StreamReader infile = new StreamReader(File.Open(input, FileMode.Open));
            BinaryWriter outfile = new BinaryWriter(File.Open(output, FileMode.Create));

            bool foundDef = false;

            while (infile.Peek() != -1)
            {
                string line = infile.ReadLine().TrimStart();

                if (line.StartsWith("//") ||
                    !(line.Contains("<") && line.Contains(">")))
                    continue;

                string type = line.Substring(line.IndexOf('<') + 1, line.IndexOf('>') - line.IndexOf('<') - 1);
                string data = line.Substring(line.IndexOf('>') + 2);

                if (type == "Def")
                {
                    data = data.Trim();
                    foundDef = true;

                    // the defintion contains the structure of each entry that will follow
                    // S (type 1) = String
                    // I (type 2) = Dword
                    // B (type 3) = Byte
                    // U (type 0) = Unknown
                    outfile.Write(data.Length);

                    foreach (char c in data)
                    {
                        if (c == 'S')
                            outfile.Write(1);
                        else if (c == 'I')
                            outfile.Write(2);
                        else if (c == 'B')
                            outfile.Write(3);
                        else
                        {
                            Console.WriteLine("Unknown type: {0}", c);
                            Environment.Exit(1);
                        }
                    }
                }
                else
                {
                    if (!foundDef)
                        continue;

                    if (type == "S")
                    {
                        byte[] d = Encoding.GetEncoding(932).GetBytes(data);
                        outfile.Write(d);
                        outfile.Write((byte)0);
                    }
                    else if (type == "I")
                    {
                        uint d = UInt32.Parse(data, System.Globalization.NumberStyles.HexNumber);
                        outfile.Write(d);
                    }
                    else if (type == "B")
                    {
                        byte d = Convert.ToByte(data);
                        outfile.Write(d);
                    }
                    else
                    {
                        Console.WriteLine("Unknown type: {0}", type);
                        Environment.Exit(1);
                    }
                }
            }

            infile.Close();
            outfile.Close();
        }
    }
}
