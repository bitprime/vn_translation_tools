using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace BaldrSky_ScriptTool
{
    class Program
    {
        static List<string> opcodes = new List<string>();

        static void Main(string[] args)
        {
            if (args.Length != 3 || args[0][0] != '-')
            {
                Console.WriteLine("usage:\n\t{0} -x infile.dat outfile.txt\n\t{0} -i infile.txt outfile.dat", System.AppDomain.CurrentDomain.FriendlyName);
                return;
            }

            InitializeOpcodes();

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

        static void InitializeOpcodes()
        {
            for (int i = 0; i < 0x3c; i++)
                opcodes.Add("");

            opcodes[0x00] = "PUSH";
            opcodes[0x02] = "JUMP7";
            opcodes[0x03] = "LABEL7";
            opcodes[0x05] = "PUSHT";
            opcodes[0x07] = "CALL";
            opcodes[0x08] = "LD";
            opcodes[0x09] = "ADD";
            opcodes[0x0a] = "SUB";
            opcodes[0x0b] = "MUL";
            opcodes[0x0c] = "DIV";
            opcodes[0x0d] = "MOD";
            opcodes[0x0e] = "STR";
            opcodes[0x11] = "OR";
            opcodes[0x12] = "AND";
            opcodes[0x13] = "XOR";
            opcodes[0x14] = "NOT";
            opcodes[0x15] = "LE";
            opcodes[0x16] = "GE";
            opcodes[0x17] = "LT";
            opcodes[0x18] = "GT";
            opcodes[0x19] = "EQ";
            opcodes[0x1a] = "NE";
            opcodes[0x1e] = "LABEL";
            opcodes[0x1f] = "JUMP";
            opcodes[0x20] = "LABEL2";
            opcodes[0x21] = "JUMP2";
            opcodes[0x22] = "INC";
            opcodes[0x23] = "DEC";
            opcodes[0x24] = "LABEL3";
            opcodes[0x25] = "JUMP3";
            opcodes[0x26] = "LABEL4";
            opcodes[0x27] = "JUMP4";
            opcodes[0x29] = "LABEL5";
            opcodes[0x2a] = "JUMP5";
            opcodes[0x39] = "LABEL6";
            opcodes[0x3a] = "JUMP6";
            opcodes[0x2d] = "SHR";
            opcodes[0x2e] = "SHL";
            opcodes[0x2f] = "LDADD";
            opcodes[0x30] = "LDSUB";
            opcodes[0x31] = "LDMUL";
            opcodes[0x32] = "LDDIV";
            opcodes[0x33] = "LDMOD";
            opcodes[0x34] = "LDOR";
            opcodes[0x35] = "LDAND";
            opcodes[0x36] = "LDXOR";
            opcodes[0x37] = "LDSHR";
            opcodes[0x38] = "LDSHL";
        }

        static void Disassemble(string input, string output)
        {
            BinaryReader bin = new BinaryReader(File.Open(input, FileMode.Open));

            StreamWriter outScript = new StreamWriter(File.Open(output + ".asm", FileMode.Create));
            StreamWriter outText = new StreamWriter(File.Open(output + ".utf", FileMode.Create));

            int maxOpcodes = bin.ReadInt32();
            int sectSize = maxOpcodes * 8;
            //bin.BaseStream.Position = 0x36ec;

            List<string> stringTable = new List<string>();
            List<string> sysStringTable = new List<string>();
            List<byte[]> sysDataTable = new List<byte[]>();

            bin.BaseStream.Position = sectSize + 4;

            // read string table
            sectSize = bin.ReadInt32();
            for (int i = 0; i < sectSize; i++)
            {
                List<byte> curString = new List<byte>();

                byte c = bin.ReadByte();

                while (c != '\0')
                {
                    curString.Add(c);
                    c = bin.ReadByte();
                }

                curString.Add(c);

                stringTable.Add(Encoding.GetEncoding(932).GetString(curString.ToArray()));
            }

            bin.BaseStream.Position = 0;
            sectSize = bin.ReadInt32() * 8;
            //bin.BaseStream.Position = 0x36ec;

            // parse bytecode
            for (int curOpcode = 0; curOpcode < maxOpcodes; curOpcode++)
            {
                int opcode = bin.ReadInt32();
                int d = bin.ReadInt32();

                //outScript.Write("@{0:x8}: ", (bin.BaseStream.Position - 4) / 8);

                if (opcode < 0 || opcode > opcodes.Count)
                {
                    Console.WriteLine("Invalid opcode @ {1:x8}: {0:x8}", opcode, bin.BaseStream.Position - 4);

                    //Environment.Exit(1);
                    break;
                }

                string opcodeName = opcodes[opcode];

                if (opcodeName == "")
                    opcodeName = String.Format("OP_{0:x2}", opcode);

                outScript.Write("<{0}> ", opcodeName);

                if (opcode == 0x05)
                {
                    if (d == 0)
                        outScript.WriteLine("{1:x8} // Int", opcodeName, d);
                    else if (d == 1)
                        outScript.WriteLine("{1:x8} // String", opcodeName, d);
                    else
                        outScript.WriteLine("{1:x8}", opcodeName, d);
                }
                else if (opcode == 0x07)
                {
                    //Console.WriteLine("{1:x4} {2:x4}", opcodeName, (d >> 16), d & 0xffff);
                    //outScript.WriteLine("{1}", opcodeName, stringTable[d]);
                    //outScript.WriteLine("{1:x4} {2:x4}", opcodeName, (d >> 16), d & 0xffff);
                    //outScript.WriteLine("{2:x4} {1:x4}", opcodeName, (d >> 16), d & 0xffff);

                    if(d == 0x00040159)
                        outScript.WriteLine("{1:x8} // TextCmd", opcodeName, d);
                    else
                        outScript.WriteLine("{1:x8}", opcodeName, d);

                    outScript.WriteLine();
                }
                else
                {
                    outScript.WriteLine("{1:x8}", opcodeName, d);
                    //Console.WriteLine(opcodeName);
                }
            }
            outScript.Write("{EndSection}");
            outScript.WriteLine("\n");

            int maxStrings = bin.ReadInt32();
            for (int curString = 0; curString < maxStrings; curString++)
            {
                string str = ReadString(bin);

                outText.WriteLine("// <{0:x8}> {1}", curString, str);
                outText.WriteLine("<{0:x8}> {1}", curString, str);
                outText.WriteLine("");
            }

            maxStrings = bin.ReadInt32();
            for (int curString = 0; curString < maxStrings; curString++)
            {
                string str = ReadString(bin);
                outScript.WriteLine("<{0:x8}> {1}", curString, str);
            }
            outScript.Write("{EndSection}");
            outScript.WriteLine("\n");

            int maxChunks = bin.ReadInt32();
            for (int curChunk = 0; curChunk < maxChunks; curChunk++)
            {
                byte[] data = bin.ReadBytes(0x44);

                outScript.Write("{");
                for (int i = 0; i < data.Length; i++)
                {
                    outScript.Write("{0:x2}", data[i]);

                    if (i + 1 < data.Length)
                        outScript.Write(" ");
                }
                outScript.Write("}");

                outScript.WriteLine();
            }
            outScript.Write("{EndSection}");

            bin.Close();

            outScript.Close();
            outText.Close();
        }

        static void Assemble(string input, string output)
        {
            StreamReader asm = new StreamReader(File.Open(input + ".asm", FileMode.Open));
            StreamReader utf = new StreamReader(File.Open(input + ".utf", FileMode.Open));
            BinaryWriter bin = new BinaryWriter(File.Open(output, FileMode.Create));
            int sectionLen = 0;
            var sectionStart = bin.BaseStream.Position;

            bin.Write(sectionLen);

            while (asm.Peek() != -1)
            {
                string line = asm.ReadLine().TrimStart();

                if (!(line.StartsWith("<") && line.Contains(">")))
                {
                    if (line.Trim() == "{EndSection}")
                        break;

                    continue;
                }

                if (line.Contains("//"))
                    line = line.Substring(0, line.IndexOf("//") - 1);

                string cmdStr = line.Substring(1, line.IndexOf('>') - 1);
                string dataStr = line.Remove(0, line.IndexOf('>') + 2);

                uint opcode = GetOpcode(cmdStr);
                uint data = UInt32.Parse(dataStr, System.Globalization.NumberStyles.HexNumber);

                if (opcode == 0xffffffff)
                {
                    Console.WriteLine("Could not parse following line:\n{0}", line);
                    Environment.Exit(1);
                }

                bin.Write(opcode);
                bin.Write(data);

                sectionLen++;
            }

            var curPosition = bin.BaseStream.Position;
            bin.BaseStream.Seek(sectionStart, SeekOrigin.Begin);
            bin.Write(sectionLen);

            sectionLen = 0;
            sectionStart = curPosition;
            bin.BaseStream.Seek(sectionStart, SeekOrigin.Begin);
            bin.Write(sectionLen);

            while (utf.Peek() != -1)
            {
                string line = utf.ReadLine().TrimStart();

                if (!(line.StartsWith("<") && line.Contains(">")))
                {
                    if (line.Trim() == "{EndSection}")
                        break;

                    continue;
                }

                if (line.Contains("//"))
                    line = line.Substring(0, line.IndexOf("//") - 1);

                string id = line.Substring(1, line.IndexOf('>') - 1);
                string text = line.Remove(0, line.IndexOf('>') + 2);

                byte[] d = Encoding.GetEncoding(932).GetBytes(text);
                bin.Write(d);
                bin.Write((byte)0);

                sectionLen++;
            }

            curPosition = bin.BaseStream.Position;
            bin.BaseStream.Seek(sectionStart, SeekOrigin.Begin);
            bin.Write(sectionLen);

            sectionLen = 0;
            sectionStart = curPosition;
            bin.BaseStream.Seek(sectionStart, SeekOrigin.Begin);
            bin.Write(sectionLen);

            while (asm.Peek() != -1)
            {
                string line = asm.ReadLine().TrimStart();

                if (!(line.StartsWith("<") && line.Contains(">")))
                {
                    if (line.Trim() == "{EndSection}")
                        break;

                    continue;
                }

                if (line.Contains("//"))
                    line = line.Substring(0, line.IndexOf("//") - 1);

                string id = line.Substring(1, line.IndexOf('>') - 1);
                string text = line.Remove(0, line.IndexOf('>') + 2);

                byte[] d = Encoding.GetEncoding(932).GetBytes(text);
                bin.Write(d);
                bin.Write((byte)0);

                sectionLen++;
            }

            curPosition = bin.BaseStream.Position;
            bin.BaseStream.Seek(sectionStart, SeekOrigin.Begin);
            bin.Write(sectionLen);

            sectionLen = 0;
            sectionStart = curPosition;
            bin.BaseStream.Seek(sectionStart, SeekOrigin.Begin);
            bin.Write(sectionLen);

            while (asm.Peek() != -1)
            {
                string line = asm.ReadLine().Trim();

                if (line == "{EndSection}")
                    break;

                if (line.Contains("//"))
                    line = line.Substring(0, line.IndexOf("//") - 1);

                if (!(line.StartsWith("{") && line.EndsWith("}")))
                    continue;

                string data = line.Trim('{').Trim('}');

                foreach (string byteStr in data.Split(' '))
                {
                    bin.Write(Byte.Parse(byteStr, System.Globalization.NumberStyles.HexNumber));
                }

                sectionLen++;
            }

            bin.BaseStream.Seek(sectionStart, SeekOrigin.Begin);
            bin.Write(sectionLen);

            asm.Close();
            utf.Close();
            bin.Close();
        }

        static uint GetOpcode(string cmd)
        {
            if (cmd.StartsWith("OP_"))
            {
                return UInt32.Parse(cmd.Remove(0, 3), System.Globalization.NumberStyles.HexNumber);
            }
            else
            {
                for(uint i = 0; i < opcodes.Count; i++)
                {
                    if (opcodes[(int)i] == cmd)
                    {
                        return i;
                    }
                }
            }

            return 0xffffffff;
        }

        static string ReadString(BinaryReader input)
        {
            List<byte> output = new List<byte>();

            while (input.BaseStream.Position < input.BaseStream.Length)
            {
                byte c = input.ReadByte();

                if (c == '\0')
                    break;

                output.Add(c);
            }

            return Encoding.GetEncoding(932).GetString(output.ToArray());
        }
    }
}
