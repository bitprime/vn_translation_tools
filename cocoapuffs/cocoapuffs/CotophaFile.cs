using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace cocoapuffs
{
    class CotophaFile
    {
        BinaryReader file;

        public CotophaFile(string inputFilename)
        {
            if (File.Exists(inputFilename))
            {
                try
                {
                    file = new BinaryReader(File.Open(inputFilename, FileMode.Open));
                }
                catch (Exception e)
                {
                    throw e;
                }
            }
            else
            {
                Console.WriteLine("Could not open {0}", inputFilename);
            }
        }

        public byte[] ReadBytes(Int32 readLength)
        {
            return file.ReadBytes(readLength);
        }

        public Byte ReadByte()
        {
            return file.ReadByte();
        }

        public Char ReadChar()
        {
            return file.ReadChar();
        }

        public Int16 ReadInt16()
        {
            return file.ReadInt16();
        }

        public UInt16 ReadUInt16()
        {
            return file.ReadUInt16();
        }

        public Int32 ReadInt32()
        {
            return file.ReadInt32();
        }

        public UInt32 ReadUInt32()
        {
            return file.ReadUInt32();
        }

        public Double ReadDouble()
        {
            return file.ReadDouble();
        }

        public string ReadString()
        {
            int stringLen = ReadInt32();
            byte[] stringData = ReadBytes(stringLen * 2);

            return Encoding.Unicode.GetString(stringData);
        }

        public byte PeekByte()
        {
            byte b = file.ReadByte();
            file.BaseStream.Seek(-1, SeekOrigin.Current);
            return b;
        }

        public Int64 SeekToSection(string sectionName)
        {
            Int64 sectionSize = -1;
            Int64 currentPosition = file.BaseStream.Position;

            file.BaseStream.Seek(0x40, SeekOrigin.Begin); // set to first section

            while (file.BaseStream.Position + 8 < file.BaseStream.Length &&
                sectionSize == -1)
            {
                byte[] tempSectionName = file.ReadBytes(8);

                if (Encoding.ASCII.GetString(tempSectionName).TrimEnd() == sectionName)
                {
                    sectionSize = file.ReadInt64();
                }
                else
                {
                    Int64 _sectionSize = file.ReadInt64();
                    file.BaseStream.Seek(_sectionSize, SeekOrigin.Current);
                }
            }

            if (sectionSize == -1)
            {
                file.BaseStream.Seek(currentPosition, SeekOrigin.Begin);
            }

            return sectionSize;
        }

        public Int64 GetPosition()
        {
            return file.BaseStream.Position;
        }

        private bool _Match(bool skipBytes, byte[] toBeMatched, int numOfBytes = 0)
        {
            bool isMatch = false;

            if (numOfBytes == 0)
                numOfBytes = toBeMatched.Length;

            byte[] data = file.ReadBytes(numOfBytes);

            if (data.SequenceEqual(toBeMatched))
                isMatch = true;

            if (!isMatch || !skipBytes)
                file.BaseStream.Seek(-numOfBytes, SeekOrigin.Current);

            return isMatch;
        }

        public bool IsMatch(byte[] toBeMatched, int numOfBytes = 0)
        {
            return _Match(false, toBeMatched, numOfBytes);
        }

        public bool Match(byte[] toBeMatched, int numOfBytes = 0)
        {
            return _Match(true, toBeMatched, numOfBytes);
        }

        public void SeekCurrent(Int32 bytesToSeek)
        {
            file.BaseStream.Seek(bytesToSeek, SeekOrigin.Current);
        }
    }
}
