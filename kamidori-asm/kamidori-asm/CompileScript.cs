using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace kamidori_asm
{
    class CompileScript
    {
        struct Label
        {
            public string label;
            public int id; // so i can reuse this struct for the string labels. being lazy a bit here
            public long offset;
        };

        static CustomBinaryReader curScript;
        static BinaryWriter outScript;
        static string headerString = "", textFile = "";
        static long headerSize = 0;
        static long curBaseLength = 0;

        static Dictionary<string, long> labels = new Dictionary<string, long>();
        static List<Label> fixLabels = new List<Label>();
        static List<Label> fixStaticInts = new List<Label>();
        static List<Label> strings = new List<Label>();

        // we must index these 3 calls and write them at the end of the script, so make a list for them
        static List<int> staticIntList = new List<int>();
        static List<long> u0041a7b0List = new List<long>();
        static List<long> u00417d60List = new List<long>();
        static List<long> callList = new List<long>();

        static int checkText = 0, newLines = 0, newLinesID = 0;
        public static void Compile(string sourceScript, string outputScript)
        {
            InitCommands();

            curScript = new CustomBinaryReader(sourceScript);
            outScript = new BinaryWriter(File.Create(outputScript));


            if (curBaseLength == 0) // uninitialized
                curBaseLength = curScript.Length;

            ReadHeader();
            ReadStrings(textFile);

            while (!curScript.IsEOF())
            {
                if (curScript.PeekChar() == '(') // command
                {
                    Match('(');

                    int command = GetCommand();

                    if (command == commandList["show-text"])
                    {
                        checkText = 1;
                    }
                    else if (command == commandList["u004252d0"])
                    {
                        checkText = 2;
                    }
                    else
                    {
                        checkText = 0;
                        newLines = 0;
                    }

                    //Console.WriteLine(command);

                    while (!curScript.IsEOF() && 
                        curScript.PeekChar() != ')')
                    {
                        SkipWhitespace();

                        if (curScript.PeekChar() == '(')
                        {
                            ParseParens();
                        }
                        else if (curScript.PeekChar() == '#')
                        {
                            int num = GetNumber();

                            outScript.Write(0);
                            outScript.Write(num);
                        }
                        else
                        {
                            Label l = new Label();
                            l.label = GetNext();
                            l.offset = outScript.BaseStream.Position + 4; // skip type
                            fixLabels.Add(l);

                            outScript.Write(0);
                            outScript.Write(0);
                        }
                    }

                    Match(')');
                }
                else // label
                {
                    string label = GetNext();

                    if (!String.IsNullOrWhiteSpace(label))
                    {
                        /*
                        Label l = new Label();

                        l.label = label.TrimEnd(':');
                        l.offset = outScript.BaseStream.Position;
                        */

                        /*
                        Console.WriteLine("Label: " + label);
                        Environment.Exit(1);
                         */

                        labels.Add(label.TrimEnd(':'), outScript.BaseStream.Position);
                    }
                }

                if (checkText == 1 && newLines > 0) // show-text
                {
                    //Console.WriteLine("New lines 1: " + newLines);

                    // simulate a \n by writing an end-text-line for the linebreak and then show-text to write the remaining text lines
                    int id = newLinesID;
                    for (int i = 0; i < newLines; i++)
                    {
                        // (wait-for-input #x0)
                        outScript.Write(commandList["end-text-line"]);
                        outScript.Write((int)0);
                        outScript.Write((int)0);

                        // (show-text #x0 (external-string (string num)))
                        outScript.Write(commandList["show-text"]);
                        outScript.Write((int)0);
                        outScript.Write((int)0);

                        Label l = new Label();
                        l.id = id;
                        l.offset = outScript.BaseStream.Position + 4; // +4 to skip to the actual address we need to write
                        strings.Add(l);

                        outScript.Write(GetType("external-string"));
                        outScript.Write((int)0);

                        id++;
                    }

                    checkText = 0;
                    newLines = 0;
                }
                else if (checkText == 2 && newLines > 0) // u004252d0
                {
                    //Console.WriteLine("New lines 2: " + newLines);

                    // simulate a \n by writing an end-text-line for the linebreak and then show-text to write the remaining text lines
                    int id = newLinesID;
                    for (int i = 0; i < newLines; i++)
                    {
                        // (show-text #x0 (external-string (string num)))
                        outScript.Write(commandList["u004252d0"]);
                        outScript.Write(GetType("global-string-1"));
                        outScript.Write(0x6A39);

                        Label l = new Label();
                        l.id = id;
                        l.offset = outScript.BaseStream.Position + 4; // +4 to skip to the actual address we need to write
                        strings.Add(l);

                        outScript.Write(GetType("external-string"));
                        outScript.Write((int)0);

                        id++;
                    }

                    checkText = 0;
                    newLines = 0;
                }

                //Environment.Exit(1);
            } // end while

            for (int i = 0; i < fixLabels.Count; i++)
            {
                Label l = fixLabels[i];

                if (labels.ContainsKey(l.label))
                {
                    outScript.BaseStream.Seek(l.offset, SeekOrigin.Begin);
                    outScript.Write((int)(labels[l.label] - headerSize) / 4);
                }
                else
                {
                    Console.WriteLine("Unknown label: " + l.label + " " + l.offset);
                    Environment.Exit(1);
                }
            }

            WriteStrings();

            outScript.Seek(0, SeekOrigin.End);
            long baseOffset = outScript.BaseStream.Position - 0x3c;
            for (int i = 0; i < fixStaticInts.Count; i++)
            {
                outScript.BaseStream.Seek(fixStaticInts[i].offset, SeekOrigin.Begin);
                outScript.Write(((fixStaticInts[i].id * 4) + (int)baseOffset) / 4); // fix static integer reference
            }

            outScript.Seek(0, SeekOrigin.End);
            for (int i = 0; i < staticIntList.Count; i++)
            {
                outScript.Write(staticIntList[i]);
            }

            outScript.Seek(0x24, SeekOrigin.Begin);
            outScript.Write(u0041a7b0List.Count);
            outScript.Write((int)((outScript.BaseStream.Length - headerSize) / 4));
            outScript.Seek(0, SeekOrigin.End);
            for (int i = 0; i < u0041a7b0List.Count; i++)
            {
                outScript.Write((int)((u0041a7b0List[i] - headerSize) / 4));
            }

            outScript.Seek(0x2c, SeekOrigin.Begin);
            outScript.Write(u00417d60List.Count);
            outScript.Write((int)((outScript.BaseStream.Length - headerSize) / 4));
            outScript.Seek(0, SeekOrigin.End);
            for (int i = 0; i < u00417d60List.Count; i++)
            {
                outScript.Write((int)((u00417d60List[i] - headerSize) / 4));
            }

            outScript.Seek(0x34, SeekOrigin.Begin);
            outScript.Write(callList.Count);
            outScript.Write((int)((outScript.BaseStream.Length - headerSize) / 4));
            outScript.Seek(0, SeekOrigin.End);
            for (int i = 0; i < callList.Count; i++)
            {
                outScript.Write((int)((callList[i] - headerSize) / 4));
            }

            curScript.Close();
            outScript.Close();
        }

        /* From ViruX's code:
            (defstruct header
                signature ; char[8] - 'SYS4415'
                local-variable-counts ; int[6]
                ; ^
                ; 0 - local integers 1 ? +523A4h ( inited to encrypted null - or decrypted, if one disables it ) - table used in case 9 - int-table-3
                ; 1 - local floats ? +523A8h - same as 0 - case 10 - float-table2
                ; 2 - local strings 1 ? +523ACh ( 3-4 string pointers are inited? hardcoded.) - case 11 - dyn-string-table3
                ; 3 - local integers 2 ? +523B0 - same as other ints - table used in case 12 - same number is used again, to init : +523E8+i*4 - int-table-4
                ; 4 - ints ? +523B4 - same - unused or different table - again, used again ^
                ; 5 - local strings 2 ? +523B8 - same - case 14 - used again ^ - dyn-string-table-4
  
                ;static-variable-header-size ; int, unsure about name, 1C
                static-data/code-offsets ; size/4, or int[6]
                ; ^ - probably offsets to those 3 special instruction types (calls and ...)
                ; 0 - size of offset table 1
                ; 1 - offset table 1 offset
                ; 2 - size of offset table 2
                ; 3 - offset table 2 offset
                ; 4 - size of offset table 3
                ; 5 - offset table 3 offset
                static-integers ; not part of the header, but actually the footer, but since this structure here is not a file header, but a general script header...
                external-string-file ; not part of the real header at all, but present in case if *store-strings-in-external-file* is set
            )
         */
        static void ReadHeader()
        {
            string s = GetNext();

            if (s == "#S")
            {
                Match('(');
                Match("header");

                for(;;)
                {
                    if (curScript.PeekChar() == ')')
                        break;

                    s = GetNext();

                    if (s == ":signature")
                    {
                        headerString = GetString();

                        outScript.Write(headerString.ToCharArray(), 0, headerString.Length);
                        headerSize += headerString.Length;
                    }
                    else if (s == ":local-variable-counts")
                    {
                        Match('(');

                        int local_integers_1 = 0; int local_floats = 0; int local_strings_1 = 0;
                        int local_integers_2 = 0; int unknown_ints = 0; int local_strings_2 = 0;

                        while (curScript.PeekChar() != ')')
                        {
                            s = GetNext();

                            if (s == ":local-integers-1")
                                local_integers_1 = GetNumber();
                            else if (s == ":local-strings-1")
                                local_strings_1 = GetNumber();
                            else if (s == ":local-floats")
                                local_floats = GetNumber();
                            else if (s == ":local-integers-2")
                                local_integers_2 = GetNumber();
                            else if (s == ":local-strings-2")
                                local_strings_2 = GetNumber();
                            else if (s == ":unknown-ints")
                                unknown_ints = GetNumber();

                            //Console.WriteLine(s);

                            //Console.WriteLine(GetType(s));

                            //int num = GetNumber();
                            //Console.WriteLine(s);

                            //outScript.Write(num);
                            headerSize += 4;
                        }

                        outScript.Write(local_integers_1);
                        outScript.Write(local_floats);
                        outScript.Write(local_strings_1);
                        outScript.Write(local_integers_2);
                        outScript.Write(unknown_ints);
                        outScript.Write(local_strings_2);

                        Match(')');
                    }
                    else if (s == ":static-data/code-offsets")
                    {
                        outScript.Write(0x1c); // size of section
                        headerSize += 4;

                        for (int i = 0; i < 3; i++) // create tntries for 3 index sets total: u0041a7b0, u00417d60, and call
                        {
                            outScript.Write(0); // size of index set
                            headerSize += 4;
                            outScript.Write(0); // offset to index set
                            headerSize += 4;
                        }

                        s = GetNext();

                        if (s != "nil")
                        {
                            Console.WriteLine("Unexpected value for static-data/code-offsets: " + s);
                            Environment.Exit(1);
                        }
                    }
                    else if (s == ":static-integers")
                    {
                        SkipWhitespace();

                        if (curScript.PeekChar() == '(')
                        {
                            Match('(');

                            while (curScript.PeekChar() != ')')
                            {
                                int n = GetNumber();
                                staticIntList.Add(n);
                            }

                            Match(')');
                        }
                        else
                        {
                            s = GetNext();

                            if (s != "nil")
                            {
                                Console.WriteLine("Unknown value for static-integers: " + s);
                                Environment.Exit(1);
                            }
                        }

                    }
                    else if (s == ":external-string-file")
                    {
                        s = GetNext();

                        if (s != "nil" && s[0] != '"')
                        {
                            Console.WriteLine("Unexpected value for static-data/code-offsets: " + s);
                            Environment.Exit(1);
                        }
                        else
                        {
                            textFile = s;
                            textFile = textFile.Remove(0, 1);
                            textFile = textFile.Remove(textFile.Length - 1, 1);
                        }
                    }
                    else
                    {
                        Console.WriteLine("Unknown section: " + s);
                        Environment.Exit(1);
                    }
                }

                Match(')');
            } // end if
        }

        static int GetType(string inputType)
        {
            int output = 0;

            if (inputType[0] == ':')
                inputType = inputType.TrimStart(':');
            
            switch (inputType)
            {
                case "global-integer-1":
                    output = 3;
                    break;
                case "global-integer-2":
                    output = 6;
                    break;

                case "local-integer-1":
                    output = 9;
                    break;
                case "local-integer-2":
                    output = 12;
                    break;

                case "global-float":
                    output = 4;
                    break;
                case "local-float":
                    output = 10;
                    break;

                case "global-string-1":
                    output = 5;
                    break;
                case "global-string-2":
                    output = 8;
                    break;
                case "local-string-1":
                    output = 11;
                    break;
                case "local-string-2":
                    output = 14;
                    break;

                case "external-string":
                    output = 2;
                    break;
            }

            return output;
        }

        static void SkipWhitespace()
        {
            char c = (char)curScript.PeekChar();

            if (!String.IsNullOrWhiteSpace(c.ToString()))
                return;

            c = curScript.ReadChar();

            while (!curScript.IsEOF() &&
                String.IsNullOrWhiteSpace(c.ToString())) // skip all whitespace
            {
                c = curScript.ReadChar();
            }

            curScript.Seek(curScript.Position - 1);
        }

        /*
         * Skip any whitespace then read all non-whitespace characters until whitespace is hit.
         * Returns the non-whitespace characters read.
         */
        static string GetNext()
        {
            StringBuilder output = new StringBuilder();

            SkipWhitespace();

            char c = curScript.ReadChar();
            while (!curScript.IsEOF() &&
                !String.IsNullOrWhiteSpace(c.ToString()) &&
                c != '(' && c != ')') // skip all whitespace
            {
                output.Append(Convert.ToString(c));
                c = curScript.ReadChar();
            }

            curScript.Seek(curScript.Position - 1);
            

            return output.ToString();
        }

        // Get a string, must start and end with "
        static string GetString()
        {
            StringBuilder output = new StringBuilder();

            Match('"');

            for(;;)
            {
                char c = curScript.ReadChar();

                if (c == '"')
                {
                    break;
                }
                
                output.Append(Convert.ToString(c));
            }

            return output.ToString();
        }

        // Convert hex string to int
        static int GetNumber()
        {
            int output = 0;

            Match('#');
            Match('x');

            bool negative = false;
            if (curScript.PeekChar() == '-')
            {
                Match('-');
                negative = true;
            }

            bool readNumber = true;
            while (readNumber)
            {
                char c = curScript.ReadChar();

                if (c >= '0' && c <= '9')
                {
                    output <<= 4;
                    output += c - 0x30;
                }
                else if (c >= 'A' && c <= 'F')
                {
                    output <<= 4;
                    output += c - 0x37;
                }
                else if (c >= 'a' && c <= 'f')
                {
                    output <<= 4;
                    output += c - 0x57;
                }
                else
                {
                    readNumber = false;
                }
            }

            if (negative)
                output = 0 - output;

            curScript.Seek(curScript.Position - 1);
            

            return output;
        }
        
        static bool Match(char input)
        {
            SkipWhitespace();

            char readData = curScript.ReadChar();
            bool matched = readData == input;

            if (!matched)
            {
                Console.WriteLine("Expected " + input + " at byte " + curScript.Position + ", got: " + readData);
                Environment.Exit(1);
            }

            return matched;
        }

        static bool Match(string input)
        {
            SkipWhitespace();

            string readData = GetNext();
            bool matched = readData == input;

            if (!matched)
            {
                Console.WriteLine("Expected " + input + " at byte " + curScript.Position + ", got: " + readData);
                Environment.Exit(1);
            }

            return matched;
        }

        static void ParseParens()
        {
            Match('(');

            string type = GetNext();
            int num = GetNumber();

            //Console.WriteLine(type + " " + num);
            if (type == "external-string")
            {
                // store the offset we need to patch
                Label l = new Label();

                l.id = num;
                l.offset = outScript.BaseStream.Position + 4; // +4 to skip to the actual address we need to write
                strings.Add(l);

                if(newlineList.ContainsKey(l.id))
                {
                    newLines = newlineList[l.id].lines;
                    newLinesID = newlineList[l.id].id;
                }
            }
            else if (type == "static-integer-reference")
            {
                Label l = new Label();

                l.id = num;
                l.offset = outScript.BaseStream.Position + 4;
                fixStaticInts.Add(l);
            }

            outScript.Write(GetType(type));
            outScript.Write(num);

            Match(')');
        }

        static int GetCommand()
        {
            int opcode = -1;

            if (commandList.Count <= 0)
            {
                Console.WriteLine("Failed to initialize command list.");
                Environment.Exit(1);

                return opcode;
            }

            string commandName = GetNext().ToLower();

            if (commandList.ContainsKey(commandName))
            {
                if (commandName == "u0041a7b0")
                    u0041a7b0List.Add(outScript.BaseStream.Position);
                else if (commandName == "u00417d60")
                    u00417d60List.Add(outScript.BaseStream.Position);
                else if (commandName == "call")
                    callList.Add(outScript.BaseStream.Position);

                opcode = commandList[commandName];
                outScript.Write(opcode);
            }
            else
            {
                Console.WriteLine("Could not find in table: '" + commandName + "'");
                Environment.Exit(1);
            }

            return opcode;
        }

        struct Overflow
        {
            public int id;
            public int lines;
        };

        static Dictionary<int, byte[]> stringList = new Dictionary<int, byte[]>();
        static Dictionary<int, Overflow> newlineList = new Dictionary<int, Overflow>();
        static int overflowLine = 0x10000;
        static void ReadStrings(string inputStrings)
        {
            StreamReader stringFile = new StreamReader(inputStrings);

            while(stringFile.Peek() != -1)
            {
                string s = stringFile.ReadLine();
                s = s.Trim().Trim('{').Trim('}');


                if (s.StartsWith("ENG|")) // english line, save it
                {
                    s = s.Remove(0, 4); // remove everything up until the id

                    int pipe = s.IndexOf('|');

                    if (pipe == -1)
                        continue;

                    string id = s.Substring(0,pipe);
                    string text = s.Substring(pipe + 1);

                    //text = text.Replace("\\n", "\n");

                    List<string> strings = new List<string>();
                    int spi = 0;

                    if(text.Contains("\\n"))
                    {
                        while (text.Contains("\\n")) // split the lines up in case there's a \n
                        {
                            strings.Add(text.Substring(0, text.IndexOf("\\n")));
                            text = text.Remove(0, text.IndexOf("\\n") + 2);
                        }
                    }
                    strings.Add(text);

                    int idnum = Convert.ToInt32(id);

                    Overflow o = new Overflow();
                    o.lines = strings.Count - 1;
                    o.id = overflowLine;
                    newlineList[idnum] = o;

                    for (int i = 0; i < strings.Count; i++)
                    {                        
                        byte[] sb = System.Text.Encoding.GetEncoding(932).GetBytes(strings[i]);

                        if (i > 0)
                        {
                            idnum = overflowLine;
                            overflowLine++;
                        }

                        stringList[idnum] = sb;
                    }

                    //Environment.Exit(1);
                }
            }

            stringFile.Close();
        }

        static void WriteStrings()
        {
            for (int i = 0; i < strings.Count; i++) // start from the beginning of the array
            {
                Label l = strings[i];

                if (!stringList.ContainsKey(l.id))
                {
                    Console.WriteLine("Could not find ID " + l.id + " in the string table");
                    Environment.Exit(1);
                }

                outScript.BaseStream.Seek(0, SeekOrigin.End);
                long offset = outScript.BaseStream.Position - headerSize;
                //outScript.Write(s, 0, s.Length);

                //outScript.Write("\0".ToCharArray(), 0, 1);
                // write the padding. must be aligned to the 4th byte
                //outScript.Write("\0\0\0\0".ToCharArray(), 0, (int)(4 - (outScript.BaseStream.Length % 4)));

                byte[] s = stringList[l.id];
                for (int x = 0; x < s.Length; x++)
                {
                    byte b = (byte)((~(int)s[x])&0xff);
                    outScript.Write(b);
                }

                uint fill = 0xffffffff;
                outScript.Write((byte)(fill & 0xff));

                for (int x = 0, len = (int)(4 - (outScript.BaseStream.Length % 4)); x < len; x++)
                {
                    outScript.Write((byte)(fill & 0xff));                  
                }

                outScript.BaseStream.Seek(l.offset, SeekOrigin.Begin);
                outScript.Write((int)(offset / 4));
            }

            outScript.Seek(0, SeekOrigin.End);
        }

        static Dictionary<string, int> commandList = new Dictionary<string, int>();
        static void InitCommands()
        {
            commandList.Add("u004149c0", 0x00000001);
            commandList.Add("exit", 0x00000002);
            commandList.Add("u00417d60", 0x00000003);
            commandList.Add("u00417e30", 0x00000004);
            commandList.Add("ret", 0x00000005);
            commandList.Add("u00417e80", 0x00000006);
            commandList.Add("u00417f90", 0x00000007);
            commandList.Add("u00417fc0", 0x00000008);
            commandList.Add("exit-script", 0x00000009);
            commandList.Add("u00424170", 0x0000000A);
            commandList.Add("u00418090", 0x0000000B);
            commandList.Add("u004149e0", 0x0000000C);
            commandList.Add("u004181a0", 0x0000000D);
            commandList.Add("u00418200", 0x0000000E);
            commandList.Add("u00418300", 0x0000000F);
            commandList.Add("u00414a00", 0x00000010);
            commandList.Add("u00418330", 0x00000011);
            commandList.Add("u004183f0", 0x00000012);
            commandList.Add("u00418420", 0x00000013);
            commandList.Add("u00414a20", 0x00000014);
            commandList.Add("u00418490", 0x00000015);
            commandList.Add("u00418520", 0x00000016);
            commandList.Add("u00418560", 0x00000017);
            commandList.Add("u004185b0", 0x0000001E);
            commandList.Add("u00418690", 0x0000001F);
            commandList.Add("u004187c0", 0x00000020);
            commandList.Add("u00418860", 0x00000021);
            commandList.Add("u00418920", 0x00000022);
            commandList.Add("u004189d0", 0x00000023);
            commandList.Add("u00418a90", 0x00000024);
            commandList.Add("u00418b40", 0x00000025);
            commandList.Add("u00418c00", 0x00000026);
            commandList.Add("u00418cc0", 0x00000027);
            commandList.Add("u00418d90", 0x00000028);
            commandList.Add("u00418e60", 0x0000002A);
            commandList.Add("u00418f30", 0x0000002B);
            commandList.Add("u00419010", 0x0000002C);
            commandList.Add("u004190a0", 0x0000002D);
            commandList.Add("u004194b0", 0x0000002E);
            commandList.Add("u004195a0", 0x0000002F);
            commandList.Add("u00419670", 0x00000030);
            commandList.Add("u00419750", 0x00000031);
            commandList.Add("u004197c0", 0x00000032);
            commandList.Add("u00419900", 0x00000033);
            commandList.Add("u004199c0", 0x00000034);
            commandList.Add("u00419af0", 0x00000035);
            commandList.Add("u00419c00", 0x00000036);
            commandList.Add("u00419c90", 0x00000037);
            commandList.Add("u00419da0", 0x00000038);
            commandList.Add("u00419e80", 0x00000050);
            commandList.Add("u00419ec0", 0x00000051);
            commandList.Add("u00419f00", 0x00000052);
            commandList.Add("u00419f40", 0x00000053);
            commandList.Add("u00419f80", 0x00000054);
            commandList.Add("u00419fc0", 0x00000055);
            commandList.Add("u00419ff0", 0x00000056);
            commandList.Add("u0041a030", 0x00000057);
            commandList.Add("u0041a070", 0x00000058);
            commandList.Add("u0041a0b0", 0x00000059);
            commandList.Add("u0041a0f0", 0x0000005A);
            commandList.Add("u0041a130", 0x0000005B);
            commandList.Add("u0041a170", 0x0000005C);
            commandList.Add("u0041a1b0", 0x0000005D);
            commandList.Add("u0041a1f0", 0x0000005E);
            commandList.Add("u0041a230", 0x0000005F);
            commandList.Add("u0041a270", 0x00000060);
            commandList.Add("u0041a320", 0x00000061);
            commandList.Add("u0041a360", 0x00000062);
            commandList.Add("u00414a60", 0x00000063);
            commandList.Add("copy-local-array", 0x00000064);
            commandList.Add("u00414aa0", 0x00000065);
            commandList.Add("u00414ae0", 0x00000066);
            commandList.Add("u00414b20", 0x00000067);
            commandList.Add("u00414b60", 0x00000068);
            commandList.Add("u00414ba0", 0x00000069);
            commandList.Add("u00414be0", 0x0000006A);
            commandList.Add("u00414c20", 0x0000006B);
            commandList.Add("u0041a450", 0x0000006C);
            commandList.Add("u00416960", 0x0000006D);
            commandList.Add("show-text", 0x0000006E);
            commandList.Add("end-text-line", 0x0000006F);
            commandList.Add("u0041a750", 0x00000070);
            commandList.Add("u0041a7b0", 0x00000071);
            commandList.Add("wait-for-input", 0x00000072);
            commandList.Add("u0041ab30", 0x00000073);
            commandList.Add("u0041ac00", 0x00000074);
            commandList.Add("u0041ac30", 0x00000075);
            commandList.Add("u0041ac60", 0x00000076);
            commandList.Add("u0041acb0", 0x00000077);
            commandList.Add("u0041ad00", 0x00000078);
            commandList.Add("u0041ad30", 0x00000079);
            commandList.Add("u0041ad70", 0x0000007A);
            commandList.Add("u0041adb0", 0x0000007B);
            commandList.Add("u00416a90", 0x0000007C);
            commandList.Add("u0041ae00", 0x0000007D);
            commandList.Add("u0041aea0", 0x0000007E);
            commandList.Add("u00414c60", 0x0000007F);
            commandList.Add("u0041af00", 0x00000080);
            commandList.Add("u0041af30", 0x00000081);
            commandList.Add("u0041af80", 0x00000082);
            commandList.Add("u00414c90", 0x00000083);
            commandList.Add("u00414cf0", 0x00000085);
            commandList.Add("u0041b210", 0x00000086);
            commandList.Add("u00414d10", 0x00000087);
            commandList.Add("u0041b290", 0x00000088);
            commandList.Add("u0041b2e0", 0x00000089);
            commandList.Add("u0041b330", 0x0000008A);
            commandList.Add("u0041b3d0", 0x0000008B);
            commandList.Add("jmp", 0x0000008C);
            commandList.Add("u0041bce0", 0x0000008D);
            commandList.Add("u0041bd60", 0x0000008E);
            commandList.Add("call", 0x0000008F);
            commandList.Add("u0041beb0", 0x00000090);
            commandList.Add("u0041bfb0", 0x00000091);
            commandList.Add("u0041c030", 0x00000092);
            commandList.Add("u00415040", 0x00000093);
            commandList.Add("u00415090", 0x00000094);
            commandList.Add("u0041c0c0", 0x00000095);
            commandList.Add("u004150c0", 0x00000096);
            commandList.Add("u0041c150", 0x00000097);
            commandList.Add("jcc", 0x000000A0);
            commandList.Add("u00427c00", 0x000000A1);
            commandList.Add("u00427fd0", 0x000000A2);
            commandList.Add("u004244d0", 0x000000A3);
            commandList.Add("u0041c270", 0x000000AA);
            commandList.Add("u0041c330", 0x000000AB);
            commandList.Add("u0041c3e0", 0x000000AC);
            commandList.Add("u00415110", 0x000000AD);
            commandList.Add("u00415130", 0x000000AE);
            //commandList.Add("u00415490", 0x000000AF);
            commandList.Add("u0041c530", 0x000000B0);
            commandList.Add("u0041c560", 0x000000B1);
            commandList.Add("u0041c590", 0x000000B2);
            commandList.Add("u004154b0", 0x000000B3);
            commandList.Add("u0041d010", 0x000000B4);
            commandList.Add("u0041d050", 0x000000B5);
            commandList.Add("u0041d080", 0x000000B6);
            commandList.Add("u0041d0e0", 0x000000B7);
            commandList.Add("u00415520", 0x000000B8);
            commandList.Add("u0041d140", 0x000000B9);
            commandList.Add("u0041d0b0", 0x000000BA);
            commandList.Add("u0041d250", 0x000000BB);
            commandList.Add("u0041d280", 0x000000BC);
            commandList.Add("u00415570", 0x000000BD);
            commandList.Add("u004155e0", 0x000000BE);
            commandList.Add("u0041d1a0", 0x000000BF);
            commandList.Add("u00415620", 0x000000C0);
            commandList.Add("u00415650", 0x000000C1);
            commandList.Add("u0041d2b0", 0x000000C2);
            commandList.Add("u0041d390", 0x000000C3);
            commandList.Add("u0041d3e0", 0x000000C4);
            commandList.Add("u0041d4a0", 0x000000C5);
            commandList.Add("u0041d5d0", 0x000000C6);
            commandList.Add("u0041d760", 0x000000C7);
            commandList.Add("u0041dfa0", 0x000000C8);
            commandList.Add("u00415770", 0x000000C9);
            commandList.Add("u004157a0", 0x000000CA);
            commandList.Add("u00415800", 0x000000CB);
            commandList.Add("u0041e050", 0x000000CC);
            commandList.Add("u00416be0", 0x000000CD);
            commandList.Add("u0041e0b0", 0x000000CE);
            commandList.Add("u00416d40", 0x000000CF);
            commandList.Add("u00415830", 0x000000D0);
            commandList.Add("u00415860", 0x000000D1);
            commandList.Add("u0041e110", 0x000000D2);
            commandList.Add("u00425960", 0x000000D3);
            commandList.Add("u004266f0", 0x000000D4);
            commandList.Add("u004262c0", 0x000000D5);
            commandList.Add("u004267d0", 0x000000D6);
            commandList.Add("u0041e1a0", 0x000000D7);
            commandList.Add("u0041e150", 0x000000D8);
            commandList.Add("u00415880", 0x000000D9);
            commandList.Add("u004158b0", 0x000000DA);
            commandList.Add("u00415940", 0x000000FA);
            commandList.Add("u0041e240", 0x000000FB);
            commandList.Add("u004159f0", 0x000000FC);
            commandList.Add("u0041e2d0", 0x000000FD);
            commandList.Add("u0041e360", 0x000000FE);
            commandList.Add("u00415a10", 0x000000FF);
            commandList.Add("u00415a60", 0x00000100);
            commandList.Add("u00415bf0", 0x00000101);
            commandList.Add("u0041e3c0", 0x00000102);
            commandList.Add("u0041e4a0", 0x00000103);
            commandList.Add("u00415c50", 0x00000104);
            commandList.Add("u0041e4d0", 0x00000105);
            commandList.Add("u00415e40", 0x00000106);
            commandList.Add("u0041e500", 0x00000107);
            commandList.Add("u00415e70", 0x00000108);
            commandList.Add("u00415ec0", 0x00000109);
            commandList.Add("u0041e540", 0x0000010A);
            commandList.Add("u0041e5a0", 0x0000010B);
            commandList.Add("u0041e5e0", 0x0000010C);
            commandList.Add("u00415f10", 0x0000010D);
            commandList.Add("u0041e650", 0x0000010E);
            commandList.Add("u0041e690", 0x0000010F);
            commandList.Add("u0041e6c0", 0x0000012C);
            commandList.Add("u0041e720", 0x0000012D);
            commandList.Add("u0041e940", 0x0000012E);
            commandList.Add("u0041ecb0", 0x0000012F);
            commandList.Add("u00415f40", 0x00000130);
            commandList.Add("u00415f70", 0x00000131);
            commandList.Add("u0041ef00", 0x00000132);
            commandList.Add("u0041eff0", 0x00000133);
            commandList.Add("u0041f050", 0x00000134);
            commandList.Add("u0041f0e0", 0x00000135);
            commandList.Add("u0041f150", 0x00000136);
            commandList.Add("u0041f1c0", 0x00000137);
            commandList.Add("u0041f2b0", 0x00000138);
            commandList.Add("u0041f310", 0x00000139);
            commandList.Add("u0041f3a0", 0x0000013A);
            commandList.Add("u0041f440", 0x0000013B);
            commandList.Add("u0041f7e0", 0x0000013C);
            commandList.Add("u0041f840", 0x0000013D);
            commandList.Add("u0041f8d0", 0x0000013E);
            commandList.Add("u0041f950", 0x0000013F);
            commandList.Add("u0041f9c0", 0x00000140);
            commandList.Add("u0041faa0", 0x00000141);
            commandList.Add("u0041fb10", 0x00000142);
            commandList.Add("u00415fb0", 0x00000143);
            commandList.Add("u004259d0", 0x00000144);
            commandList.Add("u00416040", 0x00000145);
            commandList.Add("u0041fb40", 0x00000146);
            commandList.Add("u0041fb80", 0x00000147);
            commandList.Add("u004160a0", 0x00000148);
            commandList.Add("u0041fce0", 0x00000149);
            commandList.Add("u0041fd10", 0x0000014A);
            commandList.Add("u0041ff50", 0x0000014B);
            commandList.Add("u00420030", 0x0000014C);
            commandList.Add("u00420130", 0x0000014D);
            commandList.Add("u0041c5e0", 0x00000190);
            commandList.Add("u0041a4a0", 0x00000191);
            commandList.Add("u004252d0", 0x00000192);
            commandList.Add("u00425370", 0x00000193);
            commandList.Add("u00425480", 0x00000194);
            commandList.Add("u00425580", 0x00000195);
            commandList.Add("u0041b400", 0x00000196);
            commandList.Add("u0041b510", 0x00000197);
            commandList.Add("u0041b540", 0x00000198);
            commandList.Add("u00414d50", 0x00000199);
            commandList.Add("u00414e50", 0x0000019A);
            commandList.Add("u00414e80", 0x0000019B);
            commandList.Add("u00414ec0", 0x0000019C);
            commandList.Add("u0041c680", 0x0000019D);
            commandList.Add("u0041c6e0", 0x0000019E);
            commandList.Add("u0041c860", 0x0000019F);
            commandList.Add("u0041c9b0", 0x000001A0);
            commandList.Add("u0041cb40", 0x000001A1);
            commandList.Add("u00428010", 0x000001A2);
            commandList.Add("u00424580", 0x000001A3);
            commandList.Add("u0041b580", 0x000001A4);
            commandList.Add("u00427550", 0x000001A5);
            commandList.Add("u0041a4d0", 0x000001A6);
            commandList.Add("comment", 0x000001A7);
            commandList.Add("u00415490", 0x000001A8);
            commandList.Add("u00428090", 0x000001A9);
            commandList.Add("u00425920", 0x000001AA);
            commandList.Add("u0041cca0", 0x000001AB);
            commandList.Add("u0041cd80", 0x000001AC);
            commandList.Add("u004154f0", 0x000001AD);
            commandList.Add("u0041ced0", 0x000001AE);
            commandList.Add("u004245c0", 0x000001AF);
            commandList.Add("u0041a510", 0x000001B0);
            commandList.Add("u0041b5c0", 0x000001B1);
            commandList.Add("u00425790", 0x000001B2);
            commandList.Add("u004257d0", 0x000001B3);
            commandList.Add("u004237c0", 0x000001B4);
            commandList.Add("u0041b5f0", 0x000001B5);
            commandList.Add("u00414f60", 0x000001B6);
            commandList.Add("u0041b640", 0x000001B7);
            commandList.Add("u0041b670", 0x000001B8);
            commandList.Add("u0041b710", 0x000001B9);
            commandList.Add("u0041d850", 0x000001BA);
            commandList.Add("u0041b7b0", 0x000001BB);
            commandList.Add("u00415670", 0x000001BC);
            commandList.Add("u0041d910", 0x000001BD);
            commandList.Add("u0041d9d0", 0x000001BE);
            commandList.Add("u004156c0", 0x000001BF);
            commandList.Add("u0041db70", 0x000001C0);
            commandList.Add("u0041b820", 0x000001C1);
            commandList.Add("u0041b860", 0x000001C2);
            commandList.Add("u0041b8a0", 0x000001C3);
            commandList.Add("u00415720", 0x000001C4);
            commandList.Add("u00425800", 0x000001C5);
            commandList.Add("u0041dd80", 0x000001C6);
            commandList.Add("u00414f90", 0x000001C7);
            commandList.Add("u00425680", 0x000001C8);
            commandList.Add("u0041b8e0", 0x000001C9);
            commandList.Add("u0041b9b0", 0x000001CA);
            commandList.Add("u00414fd0", 0x000001CB);
            commandList.Add("u00415010", 0x000001CC);
            commandList.Add("u0041a560", 0x000001CD);
            commandList.Add("u0041b9f0", 0x000001CE);
            commandList.Add("u0041da10", 0x000001CF);
            commandList.Add("u0041ba80", 0x000001D0);
            commandList.Add("u0041bae0", 0x000001D1);
            commandList.Add("u0041bb40", 0x000001D2);
            commandList.Add("u0041bb90", 0x000001D3);
            commandList.Add("u0041bc00", 0x000001D4);
            commandList.Add("u00415700", 0x000001D5);
            commandList.Add("u0041da40", 0x000001D6);
            commandList.Add("u0041da80", 0x000001D7);
            commandList.Add("u0041dad0", 0x000001D8);
            commandList.Add("u0041db20", 0x000001D9);
            commandList.Add("u004160d0", 0x000001F4);
            commandList.Add("u00416120", 0x000001F5);
            commandList.Add("u00416170", 0x000001F6);
            commandList.Add("u00420270", 0x000001F7);
            commandList.Add("u004202c0", 0x000001F8);
            commandList.Add("u00420350", 0x000001F9);
            commandList.Add("u00420480", 0x000001FA);
            commandList.Add("u004204f0", 0x000001FB);
            commandList.Add("u004205f0", 0x000001FC);
            commandList.Add("u00420620", 0x000001FD);
            commandList.Add("u004206c0", 0x000001FE);
            commandList.Add("u00420770", 0x000001FF);
            commandList.Add("u00420800", 0x00000200);
            commandList.Add("u00416190", 0x00000201);
            commandList.Add("u00420880", 0x00000202);
            commandList.Add("u00420950", 0x00000203);
            commandList.Add("u00420a10", 0x00000204);
            commandList.Add("u00420a60", 0x00000205);
            commandList.Add("u004161c0", 0x00000206);
            commandList.Add("u00420b00", 0x00000207);
            commandList.Add("u00420bf0", 0x00000208);
            commandList.Add("u00420c50", 0x00000209);
            commandList.Add("u00420ce0", 0x0000020A);
            commandList.Add("u00420d50", 0x0000020B);
            commandList.Add("u00416200", 0x0000020C);
            commandList.Add("u00420e10", 0x0000020D);
            commandList.Add("u00416250", 0x0000020E);
            commandList.Add("u00420e40", 0x0000020F);
            commandList.Add("u00420ff0", 0x00000210);
            commandList.Add("u00421060", 0x00000211);
            commandList.Add("u00421090", 0x00000212);
            commandList.Add("u004210d0", 0x00000213);
            commandList.Add("u00421120", 0x00000214);
            commandList.Add("u00421160", 0x00000215);
            commandList.Add("u004211a0", 0x00000216);
            commandList.Add("u004211e0", 0x00000217);
            commandList.Add("u00421270", 0x00000218);
            commandList.Add("u004212e0", 0x00000219);
            commandList.Add("u00421370", 0x0000021A);
            commandList.Add("u004213e0", 0x0000021B);
            commandList.Add("u00416270", 0x0000021C);
            commandList.Add("u00421410", 0x0000021D);
            commandList.Add("u00421450", 0x0000021E);
            commandList.Add("u00421510", 0x0000021F);
            commandList.Add("u004215d0", 0x00000220);
            commandList.Add("u00421670", 0x00000221);
            commandList.Add("u004216c0", 0x00000222);
            commandList.Add("u00421700", 0x00000223);
            commandList.Add("u00416290", 0x00000224);
            commandList.Add("u00421780", 0x00000225);
            commandList.Add("u004217d0", 0x00000226);
            commandList.Add("u00421880", 0x00000227);
            commandList.Add("u00421940", 0x00000228);
            commandList.Add("u004219e0", 0x00000229);
            commandList.Add("u00421a90", 0x0000022A);
            commandList.Add("u00421b30", 0x0000022B);
            commandList.Add("u00421bd0", 0x0000022C);
            commandList.Add("u00421c60", 0x0000022D);
            commandList.Add("u00421d10", 0x0000022E);
            commandList.Add("u00421dd0", 0x0000022F);
            commandList.Add("u00421e70", 0x00000230);
            commandList.Add("u00421ea0", 0x00000231);
            commandList.Add("u00421ef0", 0x00000232);
            commandList.Add("u00421fb0", 0x00000233);
            commandList.Add("u00422060", 0x00000234);
            commandList.Add("u00422100", 0x00000235);
            commandList.Add("u004221a0", 0x00000236);
            commandList.Add("u00422350", 0x00000237);
            commandList.Add("u00422390", 0x00000238);
            commandList.Add("u004223c0", 0x00000239);
            commandList.Add("u00422420", 0x0000023A);
            commandList.Add("u00422460", 0x0000023B);
            commandList.Add("u004162b0", 0x0000023C);
            commandList.Add("u004162f0", 0x0000023D);
            commandList.Add("u004228c0", 0x0000023E);
            commandList.Add("u00422930", 0x0000023F);
            commandList.Add("u004229a0", 0x00000240);
            commandList.Add("u00422b80", 0x00000241);
            commandList.Add("u00422d60", 0x00000242);
            commandList.Add("u00417070", 0x00000243);
            commandList.Add("u00416360", 0x00000244);
            commandList.Add("u00422da0", 0x00000245);
            commandList.Add("u00422e10", 0x00000246);
            commandList.Add("u00416390", 0x00000247);
            commandList.Add("u00422e80", 0x00000248);
            commandList.Add("u00422eb0", 0x00000249);
            commandList.Add("u004163c0", 0x0000024A);
            commandList.Add("u00422ea0", 0x0000024E);
            commandList.Add("u00422ed0", 0x0000024F);
            commandList.Add("u00422f60", 0x00000250);
            commandList.Add("u00422ff0", 0x00000251);
            commandList.Add("u00422fe0", 0x00000258);
            commandList.Add("u00416410", 0x00000259);
            commandList.Add("u00423020", 0x000002BC);
            commandList.Add("u00423100", 0x000002BD);
            commandList.Add("u00423140", 0x000002BE);
            commandList.Add("u00423180", 0x000002BF);
            commandList.Add("u004231c0", 0x000002C0);
            commandList.Add("u00425bc0", 0x000002C1);
            commandList.Add("u00425cd0", 0x000002C2);
            commandList.Add("u00423200", 0x000002C3);
            commandList.Add("u00416450", 0x000002C4);
        }
    }
}
