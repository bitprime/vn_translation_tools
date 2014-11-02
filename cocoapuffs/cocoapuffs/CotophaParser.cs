using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace cocoapuffs
{
    class CotophaParser
    {
        private bool showDebug = true;

        CotophaFile file;
        List<CotophaConst> consts = new List<CotophaConst>();
        List<CotophaAST.CotophaVariable> globals = new List<CotophaAST.CotophaVariable>();
        public static string[] variableTypes = new string[] { "Object", "Reference", "Array", "Hash", "Integer", "Real", "String", "Integer64", "Max" };
        private bool endOfFile = false;

        List<CotophaAST.CotophaFunction> functions = new List<CotophaAST.CotophaFunction>();

        public CotophaParser(string inputFilename)
        {
            file = new CotophaFile(inputFilename);

            Parse();
        }

        public void Parse()
        {
            ParseGlobal();
            ParseData();
            ParseConstStr();
            ParseFunction();
            ParseImage();

            if (endOfFile)
            {
                Console.WriteLine("; Global Variables");
                
                foreach (var global in globals)
                {
                    global.Print();
                }

                Console.WriteLine();
                Console.WriteLine("; Disassembled code");

                foreach (var function in functions)
                {
                    function.Print();
                    Console.WriteLine();
                }

                //Console.WriteLine("Successfully disassembled file");
            }
            else
            {
                foreach (var function in functions)
                {
                    function.Print();
                    Console.WriteLine();
                }

                Console.WriteLine("Failed to properly disassemble file");
            }
        }

        void ParseGlobal()
        {
            if (file.SeekToSection("global") == -1)
            {
                Console.WriteLine("Could not find 'global' section");
            }
            else
            {
                int entries = file.ReadInt32();

                for (int i = 0; i < entries; i++)
                {
                    string globalName = file.ReadString();
                    int type = file.ReadInt32();
                    string objectName = "";
                    
                    globals.Add(new CotophaAST.CotophaVariable());
                    globals[i].type = type;
                    globals[i].name = globalName;

                    if (type == 0) // object
                    {
                        objectName = file.ReadString();
                        globals[i].objectName = objectName;
                    }
                    else if (type == 1) // reference
                    {
                        Console.WriteLine("Unknown type");
                        Environment.Exit(1);
                    }
                    else if (type == 2) // array
                    {
                        globals[i].hasValue = true;
                        globals[i].rightObject = new CotophaAST.CotophaArray();
                        globals[i].rightObject.intValue = file.ReadInt32();

                        if (globals[i].rightObject.intValue == 0)
                            globals[i].hasValue = false;
                    }
                    else if (type == 3) // hash
                    {
                    }
                    else if (type == 4) // integer
                    {
                        globals[i].hasValue = true;

                        globals[i].rightObject = new CotophaAST.CotophaInteger();
                        globals[i].rightObject.intValue = file.ReadInt32();

                        if (globals[i].rightObject.intValue == 0)
                            globals[i].hasValue = false;
                    }
                    else if (type == 5) // real
                    {
                        globals[i].hasValue = true;
                        globals[i].rightObject = new CotophaAST.CotophaReal();
                        globals[i].rightObject.realValue = file.ReadDouble();

                        if (globals[i].rightObject.realValue == 0)
                            globals[i].hasValue = false;
                    }
                    else if (type == 6) // string
                    {
                        globals[i].hasValue = true;
                        globals[i].rightObject = new CotophaAST.CotophaString();
                        globals[i].rightObject.stringValue = file.ReadString();

                        if (String.IsNullOrWhiteSpace(globals[i].rightObject.stringValue))
                            globals[i].hasValue = false;
                    }
                    else if (type == 7) // integer64
                    {
                        Console.WriteLine("Unknown type");
                        Environment.Exit(1);
                    }
                    else if (type == 8) // max
                    {
                        Console.WriteLine("Unknown type");
                        Environment.Exit(1);
                    }
                }

                /*
                foreach (var var in globals)
                    var.Print();
                 */
            }
        }

        void ParseData()
        {
            if (file.SeekToSection("data") == -1)
            {
                Console.WriteLine("Could not find 'data' section");
            }
            else
            {
            }
        }

        void ParseConstStr()
        {
            if (file.SeekToSection("conststr") == -1)
            {
                Console.WriteLine("Could not find 'conststr' section");
            }
            else
            {
                int entries = file.ReadInt32();

                for (int i = 0; i < entries; i++)
                {
                    consts.Add(new CotophaConst());

                    consts[i].name = file.ReadString();
                    consts[i].referenceCount = file.ReadInt32();

                    for (int x = 0; x < consts[i].referenceCount; x++)
                    {
                        consts[i].references.Add(file.ReadInt32());
                    }
                }
            }
        }

        void ParseFunction()
        {
            if (file.SeekToSection("function") == -1)
            {
                Console.WriteLine("Could not find 'function' section");
            }
            else
            {

            }
        }

        string FindConstString(Int64 offset, Int64 imageOffset)
        {
            for (int i = 0; i < consts.Count; i++)
            {
                for (int x = 0; x < consts[i].referenceCount; x++)
                {
                    //Console.WriteLine("{0:x8} ?= {1:x8}", consts[i].references[x], offset);

                    if (consts[i].references[x] == offset - imageOffset)
                    {
                        return consts[i].name;
                    }
                }
            }

            return "";
        }

        private string GetConstString()
        {
            UInt32 objectNameLen = file.ReadUInt32();
            Int64 curOffset = file.GetPosition();
            int unk3 = file.ReadInt32();

            string output = FindConstString(curOffset, imageOffset);
            return output;
        }

        private CotophaAST.CotophaGeneric GetGlobalVariable()
        {
            int idx = file.ReadInt32();

            if (idx > globals.Count)
            {
                Console.WriteLine("Invalid global index: {0} (only have {1} total)", idx, globals.Count);
                Environment.Exit(1);
            }

            /*
            CotophaAST.CotophaGeneric global = new CotophaAST.CotophaGeneric();
            global.showType = false;
            global.type = 4;
            global.name = globals[idx].name;
            global.objectName = globals[idx].objectName;
            return global;
             */

            return globals[idx];
        }

        private Int64 imageOffset = 0;
        private void ParseImage()
        {
            Int64 sectionSize = file.SeekToSection("image");
            imageOffset = file.GetPosition();
            Int64 sectionEnd = imageOffset + sectionSize;
            var lastAddress = file.GetPosition();

            if (sectionSize == -1)
            {
                Console.WriteLine("Could not find 'image' section");
            }
            else
            {
                while (file.GetPosition() < sectionEnd)
                {
                    if (file.PeekByte() == 0x04)
                    {
                        byte op = file.ReadByte();
                        string name = file.ReadString();
                        int argCount = file.ReadInt32();

                        CotophaAST.CotophaFunction function = new CotophaAST.CotophaFunction();
                        function.functionName = name;

                        if (argCount == -1)
                        {
                            file.ReadByte();
                            file.ReadUInt32();
                        }

                        for (int i = 0; i < argCount; i++)
                        {
                            byte argType = file.ReadByte();
                            string argTypeName = "";

                            if (argType == 0)
                                argTypeName = file.ReadString();

                            string argName = file.ReadString();

                            CotophaAST.CotophaFunctionArgument argument = new CotophaAST.CotophaFunctionArgument();
                            argument.argumentType = argType;
                            argument.argumentTypeName = argTypeName;
                            argument.argumentName = argName;
                            function.arguments.Add(argument);
                        }

                        functions.Add(function);

                        ParseExpression();
                    }

                    //functions.Last().Print();
                    ParseExpression();

                    if (lastAddress == file.GetPosition())
                    {
                        Console.WriteLine("Loop detected, exiting... address: {0:x8} opcode: {1:x2}", file.GetPosition(), file.PeekByte());
                        functions.Last().Print();

                        Environment.Exit(1);
                    }

                    lastAddress = file.GetPosition();
                }
            }
        }

        private void ParseExpression()
        {
            ParseVariableDeclaration();

            if (showDebug)
            {
                Console.WriteLine("pos[{0:x8}] op[{1:x2}]", file.GetPosition(), file.PeekByte());
            }

            if (file.Match(new byte[] { 0x02, 0x02, 0x01 })) // push "this"
            {
                CotophaAST.CotophaVariable obj = new CotophaAST.CotophaVariable();
                obj.name = "this";
                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] { 0x02, 0x03, 0x04 })) // push global variable
            {
                functions.Last().stack.Add(GetGlobalVariable());
            }
            else if (file.Match(new byte[] {0x02, 0x02, 0x06}))
            {
                // variable initialization
                string variableName = file.ReadString();
                bool hasParent = false;
                bool hasThis = false;

                if (variableName == "parent")
                {
                    hasParent = true;
                }

                foreach (var cmd in functions.Last().arguments)
                {
                    if (cmd.argumentName == "this")
                    {
                        hasThis = true;
                        break;
                    }
                }

                CotophaAST.CotophaVariable obj = new CotophaAST.CotophaVariable();
                byte typeId = 0;
                bool hasType = true;
                obj.hasValue = true;

                if (hasParent)
                    obj.objectName = "parent";
                else
                    obj.objectName = "this";

                obj.name = variableName;
                obj.type = typeId;

                if (obj.objectName == "parent")
                {
                    obj.type = 0;
                    obj.showType = false;
                }

                if (file.Match(new byte[] {0x02, 0x00}))
                {
                    typeId = file.ReadByte();

                    //functions.Last().stack.Last().Print();
                    //obj.Print();

                    if (typeId == 0) // object
                    {
                        obj.stringValue = GetConstString();
                    }
                    else if (typeId == 4) // integer
                    {
                        obj.intValue = file.ReadInt32();
                    }
                    else if (typeId == 5) // real
                    {
                        obj.realValue = file.ReadDouble();
                    }
                    else if (typeId == 6) // string
                    {
                        obj.stringValue = file.ReadString();
                    }
                    else
                    {
                        functions.Last().stack.Last().Print();
                        Console.WriteLine("Unhandled variable assignment: stringValue type id {0} at 0x{1:x8}",
                                          typeId,
                                          (int) file.GetPosition());
                        Environment.Exit(1);
                    }
                }
                else
                {
                    obj.hasValue = false;
                    obj.showType = false;
                }

                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] {0x0b})) // subscript index
            {
                if (functions.Last().stack.Last().objType == typeof (CotophaAST.CotophaVariable))
                {
                    var obj = (CotophaAST.CotophaVariable) functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    var idxObj = new CotophaAST.CotophaInteger();
                    idxObj.intValue = obj.intValue;
                    obj.SetIndex(idxObj);

                    obj.hasValue = false;
                    //obj.showType = false;
                    functions.Last().stack.Add(obj);
                }
                else if (functions.Last().stack.Last().objType == typeof(CotophaAST.CotophaInteger))
                {
                    var obj1 = (CotophaAST.CotophaInteger)functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    var obj2 = (CotophaAST.CotophaVariable)functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    var obj3 = new CotophaAST.CotophaVariable();

                    var idxObj = new CotophaAST.CotophaInteger();
                    idxObj.intValue = obj1.intValue;
                    obj3.SetIndex(idxObj);

                    obj3.name = obj2.name;
                    obj3.type = 4;
                    obj3.showType = false;
                    obj3.hasValue = false;

                    functions.Last().stack.Add(obj3);
                }
                else if (functions.Last().stack.Last().objType == typeof(CotophaAST.CotophaString))
                {
                    var obj3 = new CotophaAST.CotophaVariable();
                    
                    var obj1 = (CotophaAST.CotophaString)functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    obj3.type = 4;
                    obj3.showType = false;
                    obj3.hasValue = false;
                    obj3.SetIndex(obj1);

                    if (functions.Last().stack.Last().objType == typeof(CotophaAST.CotophaString))
                    {
                        var obj2 = (CotophaAST.CotophaString)functions.Last().stack.Last();
                        functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);
                        obj3.name = obj2.stringValue;
                    }
                    else if (functions.Last().stack.Last().objType == typeof(CotophaAST.CotophaVariable))
                    {
                        var obj2 = (CotophaAST.CotophaVariable)functions.Last().stack.Last();
                        functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);
                        obj3.name = obj2.name;
                    }

                    functions.Last().stack.Add(obj3);
                }
                else
                {
                    Console.WriteLine("Don't know how to handle type {0}", functions.Last().stack.Last().objType);
                    Environment.Exit(1);
                }
            }
            else if (file.Match(new byte[] {0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00})) // EndFunc
            {
                functions.Last().isStruct = false;
                //functions.Last().Print();
            }
            else if (file.Match(new byte[] {0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x09, 0x01})) // EndStruct
            {
                functions.Last().isStruct = true;
                //functions.Last().Print();
            }
            else if (file.Match(new byte[] { 0x09, 0x00 })) // return
            {
                CotophaAST.CotophaReturn obj = new CotophaAST.CotophaReturn();
                var lastObject = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                obj.returnObject = lastObject;
                //obj.returnObject.Print();

                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] { 0x09, 0x01 })) // end of file
            {
                endOfFile = true;
            }
            else if (file.Match(new byte[] {0x02, 0x00, 0x00})) // object
            {
                string value = GetConstString();
                ;

                CotophaAST.CotophaObject obj = new CotophaAST.CotophaObject();
                obj.stringValue = value;
                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] {0x02, 0x00, 0x04})) // integer
            {
                int value = file.ReadInt32();

                CotophaAST.CotophaInteger obj = new CotophaAST.CotophaInteger();
                obj.intValue = value;
                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] {0x02, 0x00, 0x05})) // real
            {
                double value = file.ReadDouble();

                CotophaAST.CotophaReal obj = new CotophaAST.CotophaReal();
                obj.realValue = value;
                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] {0x02, 0x00, 0x06})) // string
            {
                string value = file.ReadString();

                CotophaAST.CotophaString obj = new CotophaAST.CotophaString();
                obj.stringValue = value;
                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] {0x02, 0x01, 0x04})) // variable reference
            {
                int idx = file.ReadInt32();
                int variableCount = 0;

                CotophaAST.CotophaVariable variableName = new CotophaAST.CotophaVariable();

                bool found = false;
                foreach (var cmd in functions.Last().variables)
                {
                    if (idx == variableCount)
                    {
                        variableName.name = cmd.name;
                        found = true;
                        break;
                    }

                    variableCount++;
                }

                if (!found)
                {
                    functions.Last().Print();

                    Console.WriteLine("Could not find variable");
                    Environment.Exit(1);
                }

                functions.Last().stack.Add(variableName);
            }
            else if (file.Match(new byte[] { 0x0a, 0x06 })) // member variable
            {
                if (functions.Last().stack.Last().objType == typeof (CotophaAST.CotophaString))
                {
                    var obj2 = (CotophaAST.CotophaString) functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    string name = GetConstString();

                    //functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    obj2.stringValue += "." + name;
                    obj2.Print();

                    functions.Last().stack.Add(obj2);
                }
                else if (functions.Last().stack.Last().objType == typeof(CotophaAST.CotophaVariable))
                {
                    var obj2 = (CotophaAST.CotophaVariable)functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    string name = GetConstString();

                    //functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    var nameObject = new CotophaAST.CotophaString();
                    nameObject.stringValue =  name;

                    obj2.leftObject = nameObject;

                    functions.Last().stack.Add(obj2);
                }
                else
                {
                    Console.WriteLine("Unexpected left name type: {0}", functions.Last().stack.Last().objType);
                    Environment.Exit(1);
                }
            }
            else if (file.Match(new byte[] {0x03, 0xff}))
            {
                //Console.WriteLine("Reached 0x03 0xff {0:x2}", file.PeekByte());

                bool showType = !file.Match(new byte[] {0x01});

                var obj = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                if (functions.Last().stack.Last().objType == typeof (CotophaAST.CotophaVariable))
                {
                    var obj2 = (CotophaAST.CotophaVariable)functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    obj2.hasValue = true;
                    obj2.showType = showType;
                    obj2.rightObject = obj;

                    functions.Last().stack.Add(obj2);
                    //obj2.Print();
                }
                else if (functions.Last().stack.Last().objType == typeof(CotophaAST.CotophaString))
                {
                    var obj2 = (CotophaAST.CotophaString)functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    var obj3 = new CotophaAST.CotophaVariable();

                    obj3.name = obj2.stringValue;
                    obj3.hasValue = true;
                    obj3.showType = showType;
                    obj3.rightObject = obj;

                    functions.Last().stack.Add(obj3);
                    //obj3.Print();
                }
                else if (functions.Last().stack.Last().objType == typeof(CotophaAST.CotophaReal))
                {
                    var obj2 = (CotophaAST.CotophaReal)functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    var obj3 = new CotophaAST.CotophaCall();

                    obj3.left = "Real";
                    obj3.args.Add(obj);

                    functions.Last().stack.Add(obj3);
                }
                else
                {
                    Console.WriteLine("Unexpected left hand type: {0}", functions.Last().stack.Last().objType);
                    Environment.Exit(1);
                }

                if (file.Match(new byte[] { 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 })) // repeat
                {
                    functions.Last().stack.Add(new CotophaAST.CotophaRepeatStatement());
                    ParseVariableDeclaration();
                }
            }
            else if (file.Match(new byte[] {0x00, 0x01})) // local variable
            {
                if (file.IsMatch(new byte[] { 0x00 }) ||
                    file.IsMatch(new byte[] { 0x01 }) ||
                    file.IsMatch(new byte[] { 0x02 }) ||
                    file.IsMatch(new byte[] { 0x03 }) ||
                    file.IsMatch(new byte[] { 0x04 }) ||
                    file.IsMatch(new byte[] { 0x05 }) ||
                    file.IsMatch(new byte[] { 0x06 }) ||
                    file.IsMatch(new byte[] { 0x07 }) ||
                    file.IsMatch(new byte[] { 0x08 }))
                {
                    CotophaAST.CotophaVariable obj = new CotophaAST.CotophaVariable();

                    byte typeId = file.ReadByte();

                    obj.context = 0;
                    obj.type = typeId;

                    if (typeId == 0)
                    {
                        obj.objectName = GetConstString();
                    }

                    obj.name = file.ReadString();
                    functions.Last().stack.Add(obj);
                }
                else
                {
                    Console.WriteLine("Handle non-0x02 0x00 case in 0x00 0x01 @ 0x{0:x8}", (int)file.GetPosition() - 1);
                    Environment.Exit(1);
                }
            }
            else if (file.Match(new byte[] {0x03})) // assignment operation
            {
                CotophaAST.CotophaAssignment assignment = new CotophaAST.CotophaAssignment();
                int op = file.ReadByte();

                var obj1 = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                var obj2 = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                //Console.Write("0x03 obj1: ");
                //obj1.Print();

                //Console.Write("0x03 obj2: ");
                //obj2.Print();

                assignment.op = op;
                assignment.leftObject = obj2;
                assignment.rightObject = obj1;

                functions.Last().stack.Add(assignment);

                //assignment.Print();
            }
            else if (file.Match(new byte[] {0x0c})) // math operation
            {
                CotophaAST.CotophaMathOp mathop = new CotophaAST.CotophaMathOp();
                int op = file.ReadByte();

                var obj1 = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                var obj2 = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                //Console.Write("0x0c obj1: ");
                //obj1.Print();

                //Console.Write("0x0c obj2: ");
                //obj2.Print();

                mathop.op = op;
                mathop.leftObject = obj2;
                mathop.rightObject = obj1;

                functions.Last().stack.Add(mathop);

                //mathop.Print();
            }
            else if (file.Match(new byte[] {0x0e})) // comparison operation
            {
                CotophaAST.CotophaComparison comparison = new CotophaAST.CotophaComparison();
                int op = file.ReadByte();

                var obj1 = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                var obj2 = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                comparison.op = op;
                comparison.leftObject = obj2;
                comparison.rightObject = obj1;

                functions.Last().stack.Add(comparison);
            }
            else if (file.Match(new byte[] {0x07, 0x00}))
            {
                int blockSize = file.ReadInt32();
                long currentPosition = file.GetPosition();

                var comparisonObj = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                if (functions.Last().stack.Count > 0 &&
                    functions.Last().stack.Last().objType == typeof (CotophaAST.CotophaElseIfStatement))
                {
                    CotophaAST.CotophaElseIfStatement obj =
                        (CotophaAST.CotophaElseIfStatement) functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    obj.leftObject = comparisonObj;
                    functions.Last().stack.Add(obj);
                }
                else
                {
                    //Console.WriteLine("If statement {0:x8} ({0} bytes) in length", blockSize);

                    CotophaAST.CotophaIfStatement obj = new CotophaAST.CotophaIfStatement();
                    obj.leftObject = comparisonObj;
                    functions.Last().stack.Add(obj);
                }

                if (blockSize < 0)
                {
                    return;
                }

                do
                {
                    ParseExpression();
                } while (file.GetPosition() - currentPosition < blockSize);

                if (file.GetPosition() - currentPosition == blockSize)
                {
                    functions.Last().stack.Add(new CotophaAST.CotophaEndIfStatement());
                }
                else
                {
                    Console.WriteLine("If block wrong size: {2:x8} {0:x8} {1:x8}", file.GetPosition() - currentPosition, blockSize, file.GetPosition());
                    Environment.Exit(1);
                }
            }
            else if (file.Match(new byte[] {0x06}))
            {
                int blockSize = file.ReadInt32();
                long currentPosition = file.GetPosition();

                if (functions.Last().stack.Last().objType == typeof (CotophaAST.CotophaEndIfStatement))
                {
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);
                    functions.Last().stack.Add(new CotophaAST.CotophaElseIfStatement());
                }

                //Console.WriteLine("ElseIf statement {0:x8} ({0} bytes) in length", blockSize);

                return;

                /*
                Console.WriteLine("{0:x8} {1:x2}", file.GetPosition(), file.PeekByte());
                functions.Last().Print();

                Environment.Exit(1);
                 */
            }
            else if (file.Match(new byte[] {0x08, 0x02})) // member function
            {
                int arguments = file.ReadInt32();
                string name = GetConstString();

                CotophaAST.CotophaCall obj = new CotophaAST.CotophaCall();
                obj.left = name;

                for (int i = 0; i < arguments; i++)
                {
                    var obj2 = functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    if (i + 1 < arguments)
                        obj.args.Add(obj2);
                    else
                        obj.leftObject = obj2;
                }

                obj.args.Reverse();

                functions.Last().stack.Add(obj);

                //file.Match(new byte[] {0x03, 0xff, 0x01});
            }
            else if (file.Match(new byte[] {0x08}))
            {
                byte type = file.ReadByte();
                int arguments = file.ReadInt32();
                string name = GetConstString();

                CotophaAST.CotophaCall obj = new CotophaAST.CotophaCall();

                obj.left = name;
                obj.right = "";

                CotophaAST.CotophaAssignment assignment = new CotophaAST.CotophaAssignment();
                assignment.op = 8;
                
                /*
                if (functions.Last().stack.Count != 1)
                {
                    assignment.leftObject = functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);
                }
                 */

                for (int i = 0; i < arguments; i++)
                {
                    var obj2 = functions.Last().stack.Last();
                    functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                    if (assignment.leftObject != null)
                    {
                        if (i + 1 < arguments)
                            obj.args.Add(obj2);
                        else
                            obj.leftObject = obj2;
                    }
                    else
                    {
                        obj.args.Add(obj2);
                    }
                }

                if (assignment.leftObject != null)
                {
                    assignment.rightObject = obj;
                    functions.Last().stack.Add(assignment);

                    //assignment.Print();
                }
                else
                {
                    functions.Last().stack.Add(obj);

                    //obj.Print();
                }
                
                //file.Match(new byte[] {0x03, 0xff, 0x01});
            }
            else if (file.Match(new byte[] { 0x05 })) // until
            {
                var obj = new CotophaAST.CotophaUntilStatement();

                do
                {
                    ParseExpression();
                } while (!file.IsMatch(new byte[] { 0x07, 0x00 }));

                ParseExpression(); // if statement

                if (functions.Last().stack.Last().objType != typeof (CotophaAST.CotophaIfStatement))
                {
                    Console.WriteLine("Unexpected type on right of Until. Found {0}", functions.Last().stack.Last().objType);
                    Environment.Exit(1);
                }

                var obj2 = functions.Last().stack.Last();
                functions.Last().stack.RemoveAt(functions.Last().stack.Count - 1);

                obj.rightObject = obj2.leftObject;

                functions.Last().stack.Add(obj);
            }
            else if (file.Match(new byte[] {0x01})) // ?
            {
                //Console.WriteLine("0x01, ignoring...");
            }
            else
            {
                /*
                Console.WriteLine("{0:x8} {1:x2}", file.GetPosition(), file.PeekByte());
                Console.WriteLine("Don't know where to go from here");
                Environment.Exit(1);
                 */
                return;
            }

            //ParseExpression();

            if (showDebug)
            {
                if (functions.Last().stack.Count > 0)
                {
                    Console.Write("{0} -> ", functions.Last().stack.Last().objType);
                    functions.Last().stack.Last().Print();
                }

                functions.Last().Print();   
            }
        }

        private void ParseVariableDeclaration()
        {
            while (file.Match(new byte[] {0x00, 0x02}))
            {
                if (file.IsMatch(new byte[] { 0x00 }) ||
                    file.IsMatch(new byte[] { 0x01 }) ||
                    file.IsMatch(new byte[] { 0x02 }) ||
                    file.IsMatch(new byte[] { 0x03 }) ||
                    file.IsMatch(new byte[] { 0x04 }) ||
                    file.IsMatch(new byte[] { 0x05 }) ||
                    file.IsMatch(new byte[] { 0x06 }) ||
                    file.IsMatch(new byte[] { 0x07 }) ||
                    file.IsMatch(new byte[] { 0x08 }))
                {
                    CotophaAST.CotophaVariable obj = new CotophaAST.CotophaVariable();

                    byte typeId = file.ReadByte();

                    obj.context = 0;
                    obj.type = typeId;

                    if (typeId == 0)
                    {
                        obj.objectName = GetConstString();

                        //Console.WriteLine("PUSHCONST {0} {1:x2} {2} {3:x8}", variableTypes[typeId], objectNameLen, FindConstString(curOffset, imageOffset), unk3);
                    }

                    obj.name = file.ReadString();
                    functions.Last().stack.Add(obj);
                }
                else
                {
                    Console.WriteLine("Inspect code located at 0x{0:x8}", (int)file.GetPosition() - 1);
                    Environment.Exit(1);
                }

                ParseExpression();
            }
        }
    }
}
