using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace imgtool
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 3)
            {
                Console.WriteLine("{0} [-x/-c] inputfile outputfolder", AppDomain.CurrentDomain.FriendlyName);
                Environment.Exit(1);
            }

            if(args[0] == "-x")
                Extract(args[1], args[2]);
            else if (args[0] == "-c")
                Create(args[1], args[2]);
            else
            {
                Console.WriteLine("{0} [-x/-c] inputfile outputfolder", AppDomain.CurrentDomain.FriendlyName);
                Environment.Exit(1);
            }
        }

        static void Extract(string input, string output)
        {
            using (BinaryReader reader = new BinaryReader(File.Open(input, FileMode.Open)))
            {
                byte[] hdr = new byte[] { 0x47, 0x52, 0x50, 0x00 }; // GRP\0
                int bmpDataSize = (int)reader.BaseStream.Length - 0x0c;
                int totalSize = 0x36 + bmpDataSize;

                byte[] filehdr = reader.ReadBytes(4);

                if (!hdr.SequenceEqual(filehdr))
                {
                    Console.WriteLine("Not a valid GRP file");
                    return;
                }
                else
                {
                    Int32 w = 0, h = 0;

                    input = Path.Combine(new String[] { output, Path.GetFileName(input) });
                    using (BinaryWriter writer = new BinaryWriter(File.Create(input + ".hdr")))
                    {
                        writer.Write(hdr);
                        writer.Write(reader.ReadBytes(4));

                        w = reader.ReadInt16();
                        h = reader.ReadInt16();

                        writer.Write((Int16)w);
                        writer.Write((Int16)h);
                    }

                    using (BinaryWriter writer = new BinaryWriter(File.Create(input + ".bmp")))
                    {
                        byte[] bmphdr1 = new byte[] { 0x42, 0x4d }; // BM
                        // total file size
                        byte[] bmphdr2 = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00 };
                        // width
                        // height
                        byte[] bmphdr3 = new byte[] { 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 };
                        // bmp data size
                        byte[] bmphdr4 = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

                        writer.Write(bmphdr1);
                        writer.Write(totalSize);
                        writer.Write(bmphdr2);
                        writer.Write(w);
                        writer.Write(h);
                        writer.Write(bmphdr3);
                        writer.Write(bmpDataSize);
                        writer.Write(bmphdr4);

                        writer.Write(reader.ReadBytes(bmpDataSize));
                    }
                }
            }
        }

        static void Create(string input, string output)
        {
            byte[] hdrData = new byte[0x0c];
            Int16 hdrw = 0, hdrh = 0;

            using (BinaryReader reader = new BinaryReader(File.Open(input, FileMode.Open)))
            {
                hdrData = reader.ReadBytes((int)reader.BaseStream.Length);

                reader.BaseStream.Seek(0x08, SeekOrigin.Begin);
                hdrw = reader.ReadInt16();
                hdrh = reader.ReadInt16();
            }

            using (BinaryReader reader = new BinaryReader(File.Open(input.Replace(".hdr", ".bmp"), FileMode.Open)))
            {
                byte[] hdr = new byte[] { 0x42, 0x4d }; // BM
                byte[] bmphdr = reader.ReadBytes(2);


                if (!hdr.SequenceEqual(bmphdr))
                {
                    Console.WriteLine("Not a valid BMP file");
                    return;
                }
                else
                {
                    Int32 w = 0, h = 0, bpp = 0;
                    Int32 dataOffset = 0;

                    reader.BaseStream.Seek(0x0a, SeekOrigin.Begin);
                    dataOffset = reader.ReadInt32();
                    reader.BaseStream.Seek(0x12, SeekOrigin.Begin);
                    w = reader.ReadInt32();
                    reader.BaseStream.Seek(0x16, SeekOrigin.Begin);
                    h = reader.ReadInt32();
                    reader.BaseStream.Seek(0x1c, SeekOrigin.Begin);
                    bpp = reader.ReadByte();

                    if (w != hdrw || h != hdrh)
                    {
                        Console.WriteLine("Image must be the same size as the original: [original] {0}x{1}, [new] {2}x{3}", hdrw, hdrh, w, h);
                        return;
                    }

                    if (bpp != 32)
                    {
                        Console.WriteLine("Image must be a 32bit BMP");
                        return;
                    }

                    input = Path.Combine(new String[] { output, Path.GetFileName(input) });
                    using (BinaryWriter writer = new BinaryWriter(File.Create(input.Replace(".hdr", ""))))
                    {
                        writer.Write(hdrData);

                        reader.BaseStream.Seek(dataOffset, SeekOrigin.Begin);
                        writer.Write(reader.ReadBytes((int)(reader.BaseStream.Length - reader.BaseStream.Position)));
                    }
                }
            }
        }
    }
}
