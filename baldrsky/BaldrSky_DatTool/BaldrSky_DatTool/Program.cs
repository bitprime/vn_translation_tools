using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace BaldrSky_DatTool
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
            StreamWriter outfile = new StreamWriter(File.Open(output, FileMode.Create));

            outfile.WriteLine("// Original input file: {0}", input);

            int numOfEntries = datFile.ReadInt32();

            List<int> types = new List<int>();

            outfile.WriteLine("// Do not modify the <Def> line unless you know what you are doing.");
            outfile.Write("<Def> ");

            string[] typeString = new string[4] { "U", "S", "I", "B" };

            for (int i = 0; i < numOfEntries; i++)
            {
                int type = datFile.ReadInt32();

                types.Add(type);

                if(type < typeString.Length)
                    outfile.Write(typeString[type]);
                else
                    outfile.Write(typeString[0]);                    
            }

            outfile.WriteLine("\r\n");

            int curEntry = 0;
            while (datFile.BaseStream.Position < datFile.BaseStream.Length)
            {
                outfile.WriteLine("// Entry {0}", curEntry);
                curEntry++;

                for (int i = 0; i < numOfEntries; i++)
                {
                    if (types[i] < typeString.Length)
                        outfile.Write("<" + typeString[types[i]] + "> ");
                    else
                        outfile.Write("<" + typeString[0] + "> ");

                    switch (types[i])
                    {
                        case 1: // String
                            {
                                List<Byte> sjisString = new List<Byte>();

                                byte c = datFile.ReadByte();

                                while (c != 0x00)
                                {
                                    sjisString.Add(c);
                                    c = datFile.ReadByte();
                                }

                                outfile.WriteLine("{0}", Encoding.GetEncoding(932).GetString(sjisString.ToArray()));
                            }
                            break;

                        case 2: // Dword
                            outfile.WriteLine("{0:x8}", datFile.ReadInt32());
                            break;

                        case 3: // Byte
                            outfile.WriteLine("{0:x2}", (int)datFile.ReadByte());
                            break;

                        case 0:
                        default:
                            break;
                    }
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
            
            while(infile.Peek() != -1)
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
