using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace kamidori_asm
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("usage: " + Process.GetCurrentProcess().ProcessName + " input.txt output.bin");
            }
            else
            {
                CompileScript.Compile(args[0], args[1]);
            }
        }
    }
}
