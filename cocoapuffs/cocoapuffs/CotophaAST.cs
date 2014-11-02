using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace cocoapuffs
{
    class CotophaAST
    {
        public class CotophaGeneric
        {
            public Int32 relativeOffset;
            public Int32 absoluteOffset;
            public byte opcode;
            public string name;
            public System.Type objType;

            public byte context;
            public int type;
            public string objectName;

            public bool hasValue;
            public int intValue;
            public double realValue;
            public string stringValue;

            public bool stringLiteral;

            public bool hasIndex;
            public CotophaGeneric index;

            // math and function calls
            public int op;
            public string left;
            public CotophaGeneric leftObject;

            public string right;
            public CotophaGeneric rightObject;
            public byte rightType;
            public int rightInt;
            public string rightString;
            public double rightReal;

            public bool showType;

            public List<CotophaFunctionArgument> arguments = new List<CotophaFunctionArgument>(); // function declariations
            public List<CotophaGeneric> args = new List<CotophaGeneric>(); // function calls

            public CotophaGeneric()
            {
                objType = typeof(CotophaGeneric);
                showType = true;
            }

            public string Print(bool suppressOutput = false)
            {
                //return (string)objType.GetMethod("Print").Invoke(this, new object[] { suppressOutput });

                string str = _ToString(false);
                Console.WriteLine(str);
                return str;
            }

            public string _ToString(bool includeDetails = false)
            {
                return (string)objType.GetMethod("_ToString").Invoke(this, new object[] { includeDetails });
            }
        }

        public class CotophaObject : CotophaGeneric
        {
            public int unk0;
            public int unk1;
            public string value;

            public CotophaObject()
            {
                objType = typeof(CotophaObject);
                type = 0;
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }

            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{0:x8} {1:x8} {2} {3}", unk0, unk1, name, value);
                return output.ToString();
            }
        }

        public class CotophaString : CotophaGeneric
        {
            public CotophaString()
            {
                objType = typeof(CotophaString);
                type = 6;
                stringLiteral = true;
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }

            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                if(stringLiteral)
                    output.AppendFormat("\"{0}\"", stringValue);
                else
                    output.AppendFormat("{0}", stringValue);

                return output.ToString();
            }
        }

        public class CotophaInteger : CotophaGeneric
        {
            public CotophaInteger()
            {
                objType = typeof(CotophaInteger);
                type = 4;
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }

            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{1}", name, intValue);
                return output.ToString();
            }
        }

        public class CotophaReference : CotophaGeneric
        {
            public CotophaReference()
            {
                objType = typeof(CotophaReference);
                type = 1;
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{0}", name);
                return output.ToString();
            }
        }

        public class CotophaArray : CotophaGeneric
        {
            public CotophaArray()
            {
                objType = typeof(CotophaArray);
                type = 2;
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                if (intValue != 0)
                {
                    Console.WriteLine("CotophaArray has intValue: {0}", intValue);
                    Environment.Exit(1);
                }

                //output.AppendFormat("{0}", name);
                return output.ToString();
            }
        }

        public class CotophaReal : CotophaGeneric
        {
            public CotophaReal()
            {
                objType = typeof(CotophaReal);
                type = 5;
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{1:0.0}", name, realValue);
                return output.ToString();
            }
        }

        public class CotophaVariable : CotophaGeneric
        {
            public CotophaVariable()
            {
                objType = typeof (CotophaVariable);
                hasValue = false;
                hasIndex = false;
                leftObject = null;
                rightObject = null;
            }

            public void SetIndex(CotophaGeneric index)
            {
                this.hasIndex = true;
                this.index = index;
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }

            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                //if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                //output.AppendFormat("{0} {1} {2} {3}", context, type, stringValue._ToString(false), name);

                string[] typeStrings = new string[] { "Object", "Reference", "Array", "Hash", "Integer", "Real", "String", "Integer64", "Max" };
                string typeString = "";

                if (type == 0)
                {
                    typeString = objectName;
                }
                else if(type < typeStrings.Length)
                {
                    typeString = typeStrings[type];
                }
                else
                {
                    Console.WriteLine("Invalid type. Found {0}, greater than max {1}", type, typeStrings.Length);
                }

                string prettyName = name;

                if (hasIndex)
                {
                    prettyName += "[" + index._ToString() + "]";
                }

                if (leftObject != null)
                {
                    leftObject.stringLiteral = false;
                    prettyName += "." + leftObject._ToString();
                }

                if (!hasValue)
                {
                    if(showType)
                        output.AppendFormat("{0} {1}", typeString, prettyName);
                    else if (type < 2 && typeString != name)
                        output.AppendFormat("{0}.{1}", typeString, prettyName);
                    else
                        output.AppendFormat("{0}", prettyName);
                }
                else if (rightObject != null)
                {
                    if (showType)
                        output.AppendFormat("{0} {1} := {2}", typeString, prettyName, rightObject._ToString());
                    else
                        output.AppendFormat("{1} := {2:}", typeString, prettyName, rightObject._ToString());
                }
                else
                {
                    switch (type)
                    {
                        case 0x00:
                            if (typeString != name && !String.IsNullOrWhiteSpace(typeString))
                                output.AppendFormat("{0}.{1} := {2}", typeString, prettyName, stringValue);
                            else
                                output.AppendFormat("{0} := {1}", prettyName, stringValue);
                            break;
                        case 0x04:
                            if (intValue > 256)
                                if (showType)
                                    output.AppendFormat("{0} {1} := {2:X8}H", typeString, prettyName, intValue);
                                else
                                    output.AppendFormat("{1} := {2:X8}H", typeString, prettyName, intValue);
                            else
                                if (showType)
                                    output.AppendFormat("{0} {1} := {2}", typeString, prettyName, intValue);
                                else
                                    output.AppendFormat("{1} := {2}", typeString, prettyName, intValue);
                            break;
                        case 0x05:
                            if(showType)
                                output.AppendFormat("{0} {1} := {2:0.0}", typeString, prettyName, realValue);
                            else
                                output.AppendFormat("{1} := {2:0.0}", typeString, prettyName, realValue);
                            break;
                        case 0x06:
                            if (showType)
                                output.AppendFormat("{0} {1} := {2}", typeString, prettyName, stringValue);
                            else
                                output.AppendFormat("{1} := {2}", typeString, prettyName, stringValue);
                            break;
                    }
                }

                return output.ToString();
            }
        }

        public class CotophaAssignment : CotophaGeneric
        {
            public CotophaAssignment()
            {
                objType = typeof(CotophaAssignment);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                string[] operators = new string[] { "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", ":=" };

                if (op >= operators.Length)
                {
                    Console.WriteLine("Invalid operator value: {0:x2}", op);
                    Environment.Exit(1);
                }

                if (leftObject == null)
                    Console.WriteLine("No left object");
                if (rightObject == null) 
                    Console.WriteLine("No right object");

                output.AppendFormat("{0} {1} {2}", leftObject._ToString(), operators[op], rightObject._ToString());

                return output.ToString();
            }
        }

        public class CotophaEndFunction : CotophaGeneric
        {
            public CotophaEndFunction()
            {
                objType = typeof(CotophaEndFunction);
            }
        }

        public class CotophaJump : CotophaGeneric
        {
            public int offset;

            public CotophaJump()
            {
                objType = typeof(CotophaJump);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{0}", offset);
                return output.ToString();
            }
        }

        public class CotophaJumpZero : CotophaGeneric
        {
            public int offset;

            public CotophaJumpZero()
            {
                objType = typeof(CotophaJumpZero);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{0}", offset);
                return output.ToString();
            }
        }

        public class CotophaCall : CotophaGeneric
        {
            CotophaObject callObject;

            public CotophaCall()
            {
                objType = typeof(CotophaCall);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if(!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }

            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();

                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                if (!String.IsNullOrWhiteSpace(left) && leftObject == null)
                    output.AppendFormat("{0}(", left);
                else
                    output.AppendFormat("{0}.{1}(", leftObject._ToString(), left);

                for (int i = 0; i < args.Count; i++)
                {
                    var obj = args[i];

                    output.Append(" ");

                    output.AppendFormat("{0}", obj._ToString());

                    if (i + 1 < args.Count)
                        output.AppendFormat(", ");

                    output.Append(" ");
                }

                output.AppendFormat(")");

                return output.ToString();
            }
        }

        public class CotophaReturn : CotophaGeneric
        {
            public int endType;
            public CotophaGeneric returnObject;

            public CotophaReturn()
            {
                objType = typeof(CotophaReturn);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("Return {0}", returnObject._ToString());
                return output.ToString();
            }
        }

        public class CotophaMember : CotophaGeneric
        {
            CotophaObject memberObject;

            public CotophaMember()
            {
                objType = typeof(CotophaMember);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{0}", memberObject._ToString(false));
                return output.ToString();
            }
        }

        public class CotophaIndex : CotophaGeneric
        {
            public int index;

            public CotophaIndex()
            {
                objType = typeof (CotophaIndex);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("[{0}]", index);
                return output.ToString();
            }
        }

        public class CotophaMathOp : CotophaGeneric
        {
            public CotophaMathOp()
            {
                objType = typeof(CotophaMathOp);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                string[] operators = new string[] { "+", "-", "*", "/", "%", "&", "|", "^", "=", "&&", "||" };

                if (op > operators.Length)
                {
                    Console.WriteLine("Invalid operator value: {0:x2}", op);
                    Environment.Exit(1);
                }

                output.AppendFormat("{0} {1} {2}", leftObject._ToString(), operators[op], rightObject._ToString());

                return output.ToString();
            }
        }

        public class CotophaMiscOp : CotophaGeneric
        {
            public int op;

            public CotophaMiscOp()
            {
                objType = typeof(CotophaMiscOp);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if(includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);
                output.AppendFormat("{0}", op);
                return output.ToString();
            }
        }

        public class CotophaComparison : CotophaGeneric
        {
            public int op;

            public CotophaComparison()
            {
                objType = typeof(CotophaComparison);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                string[] operators = new string[] { "!=", "==", "<", "<=", ">", ">=" };

                if (op > operators.Length)
                {
                    Console.WriteLine("Invalid operator value: {0:x2}", op);
                    Environment.Exit(1);
                }

                output.AppendFormat("{0} {1} {2}", leftObject._ToString(), operators[op], rightObject._ToString());

                return output.ToString();
            }
        }

        public class CotophaIfStatement : CotophaGeneric
        {
            public CotophaIfStatement()
            {
                objType = typeof(CotophaIfStatement);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                output.AppendFormat("If {0}", leftObject._ToString());

                return output.ToString();
            }
        }

        public class CotophaElseIfStatement : CotophaGeneric
        {
            public CotophaElseIfStatement()
            {
                objType = typeof (CotophaElseIfStatement);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }

            public new string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails)
                    output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode,
                                        System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                if (leftObject == null)
                    output.AppendFormat("Else");
                else
                    output.AppendFormat("ElseIf {0}", leftObject._ToString());

                return output.ToString();
            }
        }

        public class CotophaElseStatement : CotophaGeneric
        {
            public CotophaElseStatement()
            {
                objType = typeof(CotophaElseStatement);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                output.AppendFormat("Else {0}", leftObject._ToString());

                return output.ToString();
            }
        }

        public class CotophaEndIfStatement : CotophaGeneric
        {
            public CotophaEndIfStatement()
            {
                objType = typeof(CotophaEndIfStatement);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                output.AppendFormat("EndIf");

                return output.ToString();
            }
        }

        public class CotophaRepeatStatement : CotophaGeneric
        {
            public CotophaRepeatStatement()
            {
                objType = typeof(CotophaRepeatStatement);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                output.AppendFormat("Repeat");

                return output.ToString();
            }
        }

        public class CotophaUntilStatement : CotophaGeneric
        {
            public CotophaUntilStatement()
            {
                objType = typeof(CotophaUntilStatement);
            }

            public new string Print(bool suppressOutput = false)
            {
                string str = _ToString(false);

                if (!suppressOutput)
                    Console.WriteLine(str);

                return str;
            }
            new public string _ToString(bool includeDetails = false)
            {
                StringBuilder output = new StringBuilder();
                if (includeDetails) output.AppendFormat("[rel:{0:x8} abs:{1:x8} op:{2:x2} {3}] ", relativeOffset, absoluteOffset, opcode, System.Reflection.MethodBase.GetCurrentMethod().ReflectedType.Name);

                output.AppendFormat("Until {0}", rightObject._ToString());

                return output.ToString();
            }
        }


        public struct CotophaFunctionArgument
        {
            public int argumentType;
            public string argumentTypeName;
            public string argumentName;
            public bool fromFunc;
        };

        public class CotophaFunction : CotophaGeneric
        {
            public bool isStruct = false;
            public string functionName;
            public List<CotophaGeneric> stack = new List<CotophaGeneric>();

            public List<CotophaVariable> variables
            {
                get
                {
                    var list = new List<CotophaVariable>();

                    foreach (var cmd in arguments)
                    {
                        var obj = new CotophaVariable();
                        obj.type = (byte)cmd.argumentType;
                        obj.objectName = cmd.argumentTypeName;
                        obj.name = cmd.argumentName;
                        list.Add(obj);
                    }

                    foreach (var cmd in stack)
                    {
                        if (cmd.objType == typeof (CotophaVariable) && cmd.showType == true)
                        {
                            list.Add((CotophaVariable)cmd);
                        }
                    }

                    return list;
                }
            }

            public CotophaFunction()
            {
                objType = typeof(CotophaFunction);
            }

            new public string Print(bool suppressOutput = false)
            {
                string className = "";
                string functionName = this.functionName;

                if (functionName.Contains("::"))
                {
                    className = functionName.Substring(0, functionName.IndexOf("::", System.StringComparison.Ordinal));
                    functionName = functionName.Remove(0, functionName.IndexOf("::", System.StringComparison.Ordinal) + 2);
                }

                string prettyName = className;

                if (className != "" && className != functionName)
                {
                    prettyName += "::" + functionName;
                }
                else
                {
                    prettyName = functionName;
                }

                if (isStruct)
                {
                    Console.WriteLine("Structure\t{0}", prettyName);
                }
                else
                {
                    Console.Write("Function\t{0}( ", prettyName);

                    for (int i = 0; i < arguments.Count; i++)
                    {
                        if (arguments[i].fromFunc)
                            continue;

                        if (arguments[i].argumentName == "this")
                            continue;

                        if (arguments[i].argumentTypeName != "")
                            Console.Write(arguments[i].argumentTypeName + " " + arguments[i].argumentName);
                        else
                            Console.Write(CotophaParser.variableTypes[arguments[i].argumentType] + " " + arguments[i].argumentName);

                        if (i + 1 < arguments.Count)
                            Console.Write(",");

                        Console.Write(" ");
                    }

                    Console.WriteLine(")");
                }

                int depth = 1;
                foreach (var command in stack)
                {
                    //Console.WriteLine(command.objType);

                    if (command.objType == typeof(CotophaElseIfStatement) ||
                        command.objType == typeof(CotophaElseStatement) ||
                        command.objType == typeof(CotophaEndIfStatement))
                    {
                        depth--;
                    }

                    for (int i = 0; i < depth; i++)
                        Console.Write("\t");


                    command.Print();

                    if (command.objType == typeof(CotophaIfStatement) ||
                        command.objType == typeof(CotophaElseIfStatement) ||
                        command.objType == typeof(CotophaElseStatement) ||
                        command.objType == typeof(CotophaRepeatStatement))
                    {
                        depth++;
                    }
                    else if (command.objType == typeof(CotophaUntilStatement))
                    {
                        depth--;
                    }
                }

                Console.WriteLine(Terminal());

                return "<TODO: Add proper Print() for CotophaFunction>";
            }

            public string Terminal()
            {
                return isStruct ? "EndStruct" : "EndFunc";
            }

            public CotophaFunctionArgument GetArgument(int argNum)
            {
                if (argNum > arguments.Count)
                {
                    return new CotophaFunctionArgument();
                    //throw new Exception("Invalid argument number");
                }

                return arguments[argNum];
            }
        };
    }
}
