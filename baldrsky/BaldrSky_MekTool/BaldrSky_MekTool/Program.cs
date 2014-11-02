using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace BaldrSky_MekTool
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 3 || args[0][0] != '-')
            {
                Console.WriteLine("usage:\n\t{0} -x infile.mek outfile.txt\n\t{0} -i infile.txt outfile.mek", System.AppDomain.CurrentDomain.FriendlyName);
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

            BinaryReader mek = new BinaryReader(File.Open(input, FileMode.Open));
            StreamWriter txt = new StreamWriter(File.Open(output, FileMode.Create));

            List<int> sections = new List<int>(6);

            for(int i = 0; i < 6; i++)
                sections.Add(mek.ReadInt32());

            txt.WriteLine("[Section]");
            for (int i = 0; i < 5; i++)
                txt.WriteLine("<S> {0}", ReadString(mek));
            for (int i = 0; i < 22; i++)
                txt.WriteLine("<I> {0:x8}", mek.ReadInt32());
            txt.WriteLine("");

            txt.WriteLine("[Section]");
            for (int i = 0; i < 28 * 2; i++)
                txt.WriteLine("<I> {0:x8}", mek.ReadInt32());
            txt.WriteLine("");

            txt.WriteLine("[Section]");
            int loops = mek.ReadInt32();
            txt.WriteLine("<I> {0:x8}", loops);
            txt.WriteLine("");

            for (int x = 0; x < loops; x++)
            {
                txt.WriteLine("<I> {0:x8}", mek.ReadInt32());
                for (int i = 0; i < 3; i++)
                    txt.WriteLine("<S> {0}", ReadString(mek));
                for (int i = 0; i < 19; i++)
                    txt.WriteLine("<I> {0:x8}", mek.ReadInt32());
                txt.WriteLine("");
            }


            txt.WriteLine("[Section]");
            var end = mek.BaseStream.Length;
            if (sections[4] != 0)
                end = sections[4];

            while (mek.BaseStream.Position + 4 <= end)
                txt.WriteLine("<I> {0:x8}", mek.ReadInt32());
            while (mek.BaseStream.Position < end)
                txt.WriteLine("<B> {0:x2}", mek.ReadByte());
            txt.WriteLine("");

            txt.WriteLine("[Section]");
            end = mek.BaseStream.Length;
            if (sections[5] != 0)
                end = sections[5];

            while (mek.BaseStream.Position + 4 <= end)
                txt.WriteLine("<I> {0:x8}", mek.ReadInt32());
            while (mek.BaseStream.Position < end)
                txt.WriteLine("<B> {0:x2}", mek.ReadByte());
            txt.WriteLine("");

            txt.WriteLine("[Section]");
            while (mek.BaseStream.Position + 4 <= mek.BaseStream.Length)
                txt.WriteLine("<I> {0:x8}", mek.ReadInt32());
            while (mek.BaseStream.Position < mek.BaseStream.Length)
                txt.WriteLine("<B> {0:x2}", mek.ReadByte());
            txt.WriteLine("");

            mek.Close();
            txt.Close();
        }

        static void Assemble(string input, string output)
        {
            StreamReader txt = new StreamReader(File.Open(input, FileMode.Open));
            BinaryWriter mek = new BinaryWriter(File.Open(output, FileMode.Create));
            int curSection = 0;

            mek.Write((int)0);
            mek.Write((int)0);
            mek.Write((int)0);
            mek.Write((int)0);
            mek.Write((int)0);
            mek.Write((int)0);

            while (txt.Peek() != -1)
            {
                string line = txt.ReadLine().Trim();

                if (line.StartsWith("[Section]"))
                {
                    var curOffset = mek.BaseStream.Position;

                    mek.BaseStream.Seek(4 * curSection, SeekOrigin.Begin);
                    mek.Write(Convert.ToInt32(curOffset));
                    mek.BaseStream.Seek(curOffset, SeekOrigin.Begin);

                    curSection++;
                }

                if (line.StartsWith("//") ||
                    !(line.Contains("<") && line.Contains(">")))
                    continue;

                string type = line.Substring(line.IndexOf('<') + 1, line.IndexOf('>') - line.IndexOf('<') - 1);
                string data = line.Substring(line.IndexOf('>') + 1).Trim();

                if (type == "S")
                {
                    byte[] d = Encoding.GetEncoding(932).GetBytes(data);
                    mek.Write(d);
                    mek.Write((byte)0);
                }
                else if (type == "I")
                {
                    uint d = UInt32.Parse(data, System.Globalization.NumberStyles.HexNumber);
                    mek.Write(d);
                }
                else if (type == "B")
                {
                    byte d = Convert.ToByte(data);
                    mek.Write(d);
                }
            }

            txt.Close();
            mek.Close();
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
