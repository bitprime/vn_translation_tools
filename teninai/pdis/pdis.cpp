#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>

using namespace std;

const bool resolveStringIdx = true;

class Operator
{
public:
	int offset;
	string name;
	int start;
	int end;
	bool checkOffset;

	Operator(int _offset, string _name, int _start = 0, int _end = 0)
	{
		offset = _offset;
		name = _name;
		start = _start;
		end = _end;
	}
};

vector<Operator*> ops;
vector<string> vars;

vector<string> stringTable;

int main(int argc, char **argv)
{
	ifstream infile;
	ofstream outfile;

	if(argc != 2)
	{
		cout << "usage: " << argv[0] << " infile outfile" << endl;
		return 0;
	}

	infile.open(argv[1], ifstream::binary);
	
	if(!infile.is_open())
	{
		cout << "Could not open file: " << argv[1] << endl;
		return -1;
	}
	
	int bytecodeSize = 0;
	infile.read((char*)&bytecodeSize, 2);
	bytecodeSize = bytecodeSize * 2 + 2;

	infile.seekg(bytecodeSize, ios_base::beg);

	// read in string sizes
	int stringCount = 0;
	vector<int> stringLens;

	infile.read((char*)&stringCount, 2);
	stringLens.push_back(0);
	int stringStart = bytecodeSize + (stringCount * 2);
	
	int maxLen = 0;
	infile.seekg(0, ios_base::end);
	maxLen = infile.tellg();
	maxLen -= stringStart;
	infile.seekg(stringStart, ios_base::beg);

	// read in strings
	for(int i = 0; i < stringCount; i++)
	{
		char *str = new char[maxLen + 1];
		memset(str, '\0', maxLen + 1);

		for(int x = 0; x < maxLen; x++)
		{
			infile.read(&str[x], 1);

			if(str[x] == '\0')
				break;
		}

		string str2 = string(str);
		stringTable.push_back(str2);
		delete str;
	}

	if(stringTable.size() != stringCount)
	{
		cout << "Could not read in entire string table" << endl;
		exit(-1);
	}
	
	infile.clear();
	infile.seekg(0x02, ios_base::beg); // start of bytecode
	
	while(!infile.eof())
	{
		int offset = infile.tellg();

		if(offset >= bytecodeSize)
			break;

		char cmdStr[3];
		cmdStr[2] = '\0';

		infile.read(cmdStr, 2);

		int cmd = *(unsigned short*)&cmdStr;

		switch(cmd)
		{
			case 0x203a: // ;
				{
					ops.push_back(new Operator(offset, ";"));
				}
				break;

			case 0x6E46: // Fn
				{
					int endPos = 0;
					infile.read((char*)&endPos, 2);

					stringstream funcName;
					funcName << "function Func_" << hex << setw(8) << setfill('0') << offset;

					ops.push_back(new Operator(offset, funcName.str(), 0, endPos));
				}
				break;
	
			case 0x7B: // {
				{
					int pos = 0;
					infile.read((char*)&pos,2);
					ops.push_back(new Operator(offset, "{", 0, pos));
					ops[ops.size() - 1]->checkOffset = true;
				}
				break;
	
			case 0x7D: // }
				{
					int pos = 0;
					infile.read((char*)&pos,2);
					//ops.push_back(new Operator(offset, "}", pos, 0));

					for(int i = 0; i < ops.size(); i++)
					{
						if(ops[i]->checkOffset == true && ops[i]->offset == pos)
							ops[i]->checkOffset = false;
					}
				}
				break;

			case 0x6143: // Ca
				{
					int fileId = 0, funcOffset = 0;

					infile.read((char*)&fileId,2);
					infile.read((char*)&funcOffset,2);

					stringstream funcName;
					funcName << "CallScript[" << fileId << ", 0x" << hex << setw(4) << setfill('0') << funcOffset * 2 + 2 << "]";
					ops.push_back(new Operator(offset, funcName.str()));
				}
				break;

			case 0x6643: // Cf
				{
					int fileId = 0, funcOffset = 0;
						
					infile.read((char*)&fileId,2);
					infile.read((char*)&funcOffset,2);

					stringstream funcName;
					funcName << "CallFunction[" << fileId << ", 0x" << hex << setw(4) << setfill('0') << funcOffset * 2 + 2 << "]";
					ops.push_back(new Operator(offset, funcName.str()));
				}
				break;

			case 0x6B42: // Bk
				{
					int pos = 0;
					infile.read((char*)&pos,2);
					ops.push_back(new Operator(offset, "break", pos, 0));
				}
				break;

			case 0x7452: // Rt
				{
					ops.push_back(new Operator(offset, "return"));
				}
				break;

			case 0x7845: // Ex
				{
					ops.push_back(new Operator(offset, "exit"));
				}
				break;

			case 0x6f46: // Fo
				{
					int pos = 0, pos2 = 0;
					infile.read((char*)&pos,2);
					infile.read((char*)&pos2,2);
					ops.push_back(new Operator(offset, "for", pos, 0));
				}
				break;

			case 0x6857: // Wh
				{
					int pos = 0;
					infile.read((char*)&pos,2);
					ops.push_back(new Operator(offset, "while", pos, 0));
				}
				break;

			case 0x7045: // Ep
				{
					int pos = 0;
					infile.read((char*)&pos,2);
					ops.push_back(new Operator(offset, "else if")); // ???
				}
				break;

			case 0x6c45: // El
				{
					ops.push_back(new Operator(offset, "else"));
				}
				break;

			case 0x7953: // Sy
				{
					ops.push_back(new Operator(offset, "Sy"));
				}
				break;

			case 0x6649: // If
				{
					ops.push_back(new Operator(offset, "if")); 
				}
				break;

			case 0x7254: // Tr
				{
					int startPos = 0;

					infile.read((char*)&startPos,4);
					ops.push_back(new Operator(offset, "Tr", startPos, 0)); 
				}
				break;

			case 0x7753: // Sw
				{
					int endPos = 0;

					infile.read((char*)&endPos,2);
					ops.push_back(new Operator(offset, "switch", 0, endPos)); 
					//ops[ops.size() - 1]->checkOffset = true;
				}
				break;

			case 0x7342: // Bs
				{
					int startPos = 0;

					infile.read((char*)&startPos,2);
					ops.push_back(new Operator(offset, "Bs", startPos, 0));
				}
				break;

			case 0x7345: // Es
				{
					int statements = 0;

					infile.read((char*)&statements,2);

					stringstream statement = stringstream();
					statement << "Es " << statements;

					for(int i = 0; i < statements; i++)
					{
						int soffset = 0, id = 0;

						infile.read((char*)&soffset,2);
						infile.read((char*)&id,2);
					}

					ops.push_back(new Operator(offset, statement.str()));
				}
				break;

			case 0x7652: // Rv
				{
					ops.push_back(new Operator(offset, "Rv"));
				}
				break;
				
			default: // unknown
				{
					if(cmdStr[1] == 'l' && (cmdStr[0] == '#' || cmdStr[0] == '$' || cmdStr[0] == '~' || cmdStr[0] == '@'))
					{
						unsigned int num = 0;
						infile.read((char*)&num,4);

						stringstream numStr;
						numStr << cmdStr[0] << "l[0x" << setw(8) << setfill('0') << num << "]";
						ops.push_back(new Operator(offset, numStr.str()));
					}
					else if(cmdStr[1] == 's' && (cmdStr[0] == '#' || cmdStr[0] == '$' || cmdStr[0] == '~' || cmdStr[0] == '@'))
					{
						unsigned int num = 0;
						infile.read((char*)&num,2);

						stringstream numStr;

						if(cmdStr[0] == '$' && num == 0x003c) // character's name
						{
							numStr << cmdStr[0] << "s[$playerName]";
						}
						else if(cmdStr[0] == '$' && num == 0x003b) // character's name 2
						{
							numStr << cmdStr[0] << "s[$playerName2]";
						}
						else
						{
							if(resolveStringIdx && num < stringTable.size())
								numStr << cmdStr[0] << "s[" << stringTable[num] << "]";
							else
								numStr << cmdStr[0] << "s[0x" << setw(4) << setfill('0') << hex << num << "]";
						}

						ops.push_back(new Operator(offset, numStr.str()));
					}
					else if(cmdStr[1] == 'i' && (cmdStr[0] == '#' || cmdStr[0] == '$' || cmdStr[0] == '~' || cmdStr[0] == '@'))
					{
						unsigned int num = 0;
						infile.read((char*)&num,2);

						stringstream numStr;
						numStr << cmdStr[0] << "i[0x" << setw(4) << setfill('0') << hex << num << "]";
						ops.push_back(new Operator(offset, numStr.str()));
					}
					else if(cmdStr[0] == ',' || cmdStr[0] == '(' || cmdStr[0] == ')' ||
						    cmdStr[0] == '=' || cmdStr[0] == '+' || cmdStr[0] == '-' ||
							cmdStr[0] == '/' || cmdStr[0] == '#' || cmdStr[0] == '&' ||
							cmdStr[0] == '~' || cmdStr[0] == '*' || cmdStr[0] == '%' ||
							cmdStr[0] == '|' || cmdStr[0] == '^' || cmdStr[0] == '>' ||
							cmdStr[0] == '<' ||
							((cmdStr[0] == '<' || cmdStr[0] == '>') && cmdStr[1] == '=') ||
							(((cmdStr[0] == '!' || cmdStr[0] == '=') && cmdStr[1] == '=')))
					{
						ops.push_back(new Operator(offset, string(cmdStr)));
					}

					// not the proper disassembly
					else if(!(isalpha((cmd >> 8)) && isalpha((cmd & 0xff))))
					{
						stringstream funcName;
						funcName << "Call[0x" << hex << setw(2) << setfill('0') << cmd << "]";
						ops.push_back(new Operator(offset, funcName.str()));
					}

					else
					{
						cout << "Unknown command: " << hex << cmd << " at " << hex << offset << endl;
						exit(-1);
					}
				}
				break;
		}

		for(int i = 0; i < ops.size(); i++)
		{
			if(ops[i]->checkOffset == true)
			{
				if(ops[i]->end * 2 + 2 == offset)
				{
					ops.push_back(new Operator(-1, "}", 0, 0));
					ops[i]->checkOffset = false;
				}
			}
		}
	}
	
	for(int i = 0; i < ops.size(); i++)
	{
		if(ops[i]->checkOffset == true)
		{
			if(ops[i]->end == 0xffff)
			{
				ops.push_back(new Operator(-1, "}", 0, 0));
				ops[i]->checkOffset = false;
			}
		}
	}

	int tabDepth = 0;
	bool updateTabs = true;
	for(int x = 0; x < ops.size(); x++)
	{
		if(ops[x]->name == "}")
		{
			tabDepth--;
			cout << endl;
			updateTabs = true;
		}
		if(ops[x]->name == "{")
		{
			cout << endl;
			updateTabs = true;
		}

		if(updateTabs)
		{
			for(int i = 0; i < tabDepth; i++)
				cout << "    ";
			updateTabs = false;
		}

		cout << ops[x]->name;

		if(x+1 < ops.size() && ops[x+1]->name != "," && ops[x+1]->name != ";" && ops[x+1]->name != ";" && ops[x]->name != "(" && ops[x+1]->name != ")")
			cout << " ";

		if(ops[x]->name == ";")
		{
			cout << endl;
			updateTabs = true;
		}

		if(ops[x]->name == "{")
		{
			cout << endl;
			tabDepth++;
			updateTabs = true;
		}
		else if(ops[x]->name == "}")
		{
			cout << endl;
			updateTabs = true;
		}
	}
	
	infile.close();
	
	return 0;
}
