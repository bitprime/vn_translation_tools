using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;

namespace TWINS_DatPacker
{
    class Program
    {
        // data taken from Riatre's Twins Engine Archive Extractor
        class GameConfig
        {
            public string id;
            public string name;
            public int nameLength;
            public bool encrypted;
            public int key1;
            public int key2;
            public byte[] key;
            public byte keya;
            public uint mulkey;
            public bool isXmlEncrypted;
            public byte[] xmlkey;

            public GameConfig(string _id, string _name, int _nameLength, bool _encrypted, int _key1, int _key2, byte[] _key, byte _keya, uint _mulkey, bool _isXmlEncrypted, byte[] _scriptkey)
            {
                id = _id;
                name = _name;
                nameLength = _nameLength;
                encrypted = _encrypted;
                key1 = _key1;
                key2 = _key2;
                key = _key;
                keya = _keya;
                mulkey = _mulkey;
                isXmlEncrypted = _isXmlEncrypted;
                xmlkey = _scriptkey;
            }

            public GameConfig()
            {
                id = "";
            }
        }

        class IndexEntry
        {
            public string fullpath;
            public string filename;
            public uint offset;
            public uint filesize;

            public byte[] GetBuffer()
            {
                List<byte> buffer = new List<byte>();
                byte[] filenameBytes = new byte[gameConfig.nameLength];

                int curByte = 0;
                foreach (byte b in Encoding.GetEncoding(932).GetBytes(filename))
                {
                    filenameBytes[curByte++] = b;

                    if (curByte > gameConfig.nameLength)
                        break;
                }

                uint offsetEnc = offset;
                uint filesizeEnc = filesize;

                if (gameConfig.encrypted)
                {
                    filenameBytes = EncryptData(filenameBytes, gameConfig.nameLength);
                    offsetEnc ^= 0xADC3FEA9;
                    filesizeEnc ^= 0xC3FEA9BA;
                }

                for (int i = 0; i < gameConfig.nameLength; i++)
                {
                    if (i >= filenameBytes.Length)
                        buffer.Add(0);
                    else
                        buffer.Add(filenameBytes[i]);
                }

                foreach (byte b in BitConverter.GetBytes(offsetEnc))
                    buffer.Add(b);
                foreach (byte b in BitConverter.GetBytes(filesizeEnc))
                    buffer.Add(b);

                return buffer.ToArray();
            }
        }

        static List<GameConfig> gameConfigs = new List<GameConfig>();
        static GameConfig gameConfig = new GameConfig();
        static List<IndexEntry> entries = new List<IndexEntry>();

        static void Main(string[] args)
        {
            gameConfigs.Add(new GameConfig("ALTSPHERE", "Alternative Sphere", 64, true, 0x77c523f9, 0xde, new byte[5] { 0x45, 0xa1, 0xca, 0x23, 0x29 }, 0x17, 2, false, new byte[4] { 0xba, 0xad, 0xca, 0xfe }));
            gameConfigs.Add(new GameConfig("ALTSPHERE-EN", "Alternative Sphere English", 64, true, 0x77c523f9, 0xde, new byte[5] { 0x45, 0xa1, 0xca, 0x23, 0x29 }, 0x17, 2, false, new byte[4] { 0xba, 0xad, 0xca, 0xfe }));
            gameConfigs.Add(new GameConfig("TWINS", "TWilight INSanity", 56, false, 0, 0, new byte[5] { 0, 0, 0, 0, 0 }, 0, 0, true, new byte[4] { 0xba, 0xad, 0xca, 0xfe }));
            gameConfigs.Add(new GameConfig("TWAIN", "TWilight refrAIN", 32, true, 0, 0, new byte[5] { 0xba, 0xad, 0xc3, 0xfe, 0xa9 }, 0x71, 3, false, new byte[4] { 0xba, 0xad, 0xca, 0xfe }));
            gameConfigs.Add(new GameConfig("TWAIN-EN", "TWilight refrAIN English", 32, true, 0, 0, new byte[5] { 0xba, 0xad, 0xc3, 0xfe, 0xa9 }, 0x71, 3, false, new byte[4] { 0xba, 0xad, 0xca, 0xfe }));
            gameConfigs.Add(new GameConfig("WOTWAIN", "WORLD OF TWAIN", 256, true, 0x77c523f9, 0xde, new byte[5] { 0x45, 0xa1, 0xca, 0x23, 0x29 }, 0x17, 2, false, new byte[4] { 0xba, 0xad, 0xca, 0xfe }));

            if (args.Length != 2)
            {
                Console.WriteLine("usage: {0} game inputFolder\nGame list:", AppDomain.CurrentDomain.FriendlyName);

                foreach (GameConfig config in gameConfigs)
                {
                    Console.WriteLine("\t{1} - {0}", config.name, config.id);
                }

                return;
            }

            foreach (GameConfig config in gameConfigs)
            {
                if (config.id == args[0])
                {
                    gameConfig = config;
                    break;
                }
            }

            if (gameConfig.id == "")
            {
                Console.WriteLine("Could not find game id '{0}'", args[0]);
                return;
            }

            BuildIndex(args[1]);
            BuildArchive();
        }

        static void BuildIndex(string inputFolder)
        {
            var files = Directory.GetFiles(@inputFolder, "*.*");

            foreach (string fullpath in files)
            {
                IndexEntry entry = new IndexEntry();

                string filename = Path.GetFileName(fullpath);

                entry.fullpath = fullpath;
                entry.filename = filename;
                entry.offset = 0;
                entry.filesize = 0;

                entries.Add(entry);
            }
        }

        static void BuildArchive()
        {
            int idx = 0;

            BinaryWriter outarc = new BinaryWriter(File.Open("arc.dat", FileMode.Create));
            BinaryWriter outidx = new BinaryWriter(File.Open("idx.dat", FileMode.Create));

            foreach (IndexEntry entry in entries)
            {
                BinaryReader infile = new BinaryReader(File.Open(entry.fullpath, FileMode.Open));
                string[] compressable = new string[] { ".dds", ".bmp", ".ttf" };
                string ext = Path.GetExtension(entry.filename);

                byte[] data = infile.ReadBytes(Convert.ToInt32(infile.BaseStream.Length));

                if (ext == ".raw")
                {
                    entries[idx].filename = Path.GetFileNameWithoutExtension(entry.filename);
                }
                else
                {
                    if (ext == ".xml")
                    {
                        data = FixStrings(Encoding.GetEncoding(932).GetString(data));
                    }

                    if (compressable.Contains(ext))
                    {
                        data = CompressData(data);
                    }
                    else
                    {
                        if ((gameConfig.id == "TWAIN-EN" || gameConfig.id == "ALTSPHERE-EN") &&
                            ext == ".xml")
                        {
                            data = BitConverter.GetBytes(data.Length).Concat(data).ToArray();
                        }

                        if (gameConfig.encrypted)
                            data = EncryptData(data);
                    }

                    // requires special encryption for TWINS
                    if (gameConfig.id == "TWINS" &&
                        ext == ".xml")
                    {
                        for (int i = 0; i < data.Length; i++)
                        {
                            data[i] ^= gameConfig.xmlkey[i % 4];
                        }
                    }
                }

                entries[idx].offset = Convert.ToUInt32(outarc.BaseStream.Position);
                entries[idx].filesize = Convert.ToUInt32(data.Length);

                Console.WriteLine("[{0:d4}] {1,-32} offset[{2:x8}] size[{3:x8}]", idx, entries[idx].filename, entries[idx].offset, entries[idx].filesize);

                outarc.Write(data);
                outidx.Write(entry.GetBuffer());

                infile.Close();
                idx++;
            }

            outarc.Close();
            outidx.Close();
        }

        static byte[] CompressData(byte[] indata)
        {
            int fileSize = indata.Length;
            //byte[] output = new byte[fileSize];
            List<byte> output = new List<byte>(Convert.ToInt32(fileSize));
            int pos = 0;

            // add filesize
            foreach (byte b in BitConverter.GetBytes(fileSize).Reverse())
                output.Add(b);

            while (pos + 4 <= fileSize)
            {
                int reps = 1;
                uint data = (uint)(((uint)indata[pos + 3] << 24) + ((uint)indata[pos + 2] << 16) + ((uint)indata[pos + 1] << 8) + (uint)indata[pos]);
                pos += 4;

                while (reps < 0xfe &&
                       pos + 4 <= fileSize &&
                       data == (uint)(((uint)indata[pos + 3] << 24) + ((uint)indata[pos + 2] << 16) + ((uint)indata[pos + 1] << 8) + (uint)indata[pos]))
                {
                    reps++;
                    pos += 4;
                }

                output.Add(Convert.ToByte(reps & 0xff));
                foreach (byte b in BitConverter.GetBytes(data))
                    output.Add(b);
            }

            while (pos < fileSize)
            {
                output.Add(indata[pos]);
                pos++;
            }

            // something, not sure what it's for. junk data though?
            output.Add(0);
            output.Add(0);
            output.Add(0);
            output.Add(0);

            return output.ToArray();
        }

        static byte[] EncryptData(byte[] input)
        {
            return EncryptData(input, input.Length);
        }

        static byte[] EncryptData(byte[] input, int len)
        {
            byte[] output = input;

            for (uint i = 0; i < len; i++)
            {
                output[i] ^= Convert.ToByte((gameConfig.key[i % 5] + gameConfig.mulkey * i + gameConfig.keya) & 0xff);
            }

            return output;
        }

        static byte[] FixStrings(string input)
        {
            XmlDocument xml = new XmlDocument();
            xml.PreserveWhitespace = true;
            xml.LoadXml(input);

            FixNodes(xml.DocumentElement);

            StringWriter str = new StringWriter();
            XmlTextWriter xml2 = new XmlTextWriter(str);
            xml.WriteTo(xml2);

            xml2.Close();

            return Encoding.GetEncoding(932).GetBytes(str.ToString().Replace(" />","/>"));
        }

        static void FixNodes(XmlNode node)
        {
            for (int i = 0; i < node.ChildNodes.Count; i++)
            {
                if (node.ChildNodes[i].Name == "string")
                {
                    StringBuilder doubled = new StringBuilder();

                    foreach (char c in node.ChildNodes[i].InnerText)
                    {
                        doubled.Append(c);

                        if (c != '*' &&
                            c <= 0x7f)
                        {
                            doubled.Append(c);
                        }
                    }

                    node.ChildNodes[i].InnerText = doubled.ToString();
                }

                FixNodes(node.ChildNodes[i]);
            }
        }
    }
}
