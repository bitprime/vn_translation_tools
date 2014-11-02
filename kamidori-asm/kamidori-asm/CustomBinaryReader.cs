using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace kamidori_asm
{
    class CustomBinaryReader
    {
        BinaryReader file;
        Char[] data;
        bool atEOF = false;

        public long Length = 0;

        public long Position = 0;

        public CustomBinaryReader(string input)
        {
            file = new BinaryReader(File.OpenRead(input));

            Length = file.BaseStream.Length;
            data = new Char[Length];
            data = file.ReadChars((int)Length);

            file.Close();
        }

        public char ReadChar()
        {
            if (Position + 1 >= Length)
                atEOF = true;

            return data[Position++];
        }

        public char PeekChar()
        {
            return data[Position];
        }

        public bool IsEOF()
        {
            return atEOF;
        }

        public void Seek(long offset)
        {
            Position = offset;

            if (Position + 1 >= Length)
                atEOF = true;
            else if (atEOF == true)
                atEOF = false;
        }

        public void Close()
        {
            // any cleanup needed goes here
        }
    }
}
