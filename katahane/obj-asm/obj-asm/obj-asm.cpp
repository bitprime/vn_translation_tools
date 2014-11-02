#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>

using namespace std;

#include "obj-asm.h"
#include "tohex.h"
#include "crc32.h"

int head_entry = 0;
int head_lines = 0;

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		cout << "usage: " << argv[0] << " inasm inscript outmjo" << endl;
		return 1;
	}

	ReadASMData(argv[1]);
	ReadScriptData(argv[2]);
	ParseCode();
	FixJumps();
	WriteHeader(argv[3]);

	return 0;
	
}

void ReadASMData(char *filename)
{
	ifstream input(filename, ifstream::in);
	int i = 0;

	while(input.good())
	{
		char line[0x1000];

		input.getline(line, 0x1000);

		if(strlen(line) != 0)
			infile.push_back(line);
	}

	input.close();
}

void ReadScriptData(char *filename)
{
	ifstream input(filename, ifstream::in);
	int i = 0;

	while(input.good())
	{
		char _line[0x1000];

		input.getline(_line, 0x1000);

		string line = boost::lexical_cast<string>(_line);
		if(line.length() != 0 && line[0] == '<') // only read in lines that are <>s
		{
			string id = line.substr(1, 8);
			string text = line.substr(11, line.length() - 11);
			
			script[id] = text;
		}
	}

	input.close();
}

string GetString(string id)
{
	if(script.empty() ||
		script.find(id) == script.end())
		return "";

	return script[id];
}

bool findNextText(int cur, bool follow)
{
	for(int i = cur; i < infile.size(); i++)
	{
		vector<string> args;
		split(args, infile[i], boost::is_any_of("\t"), boost::token_compress_on);
		
		if(args[0] == "entry:") // entry point, save addr in mjo
		{
		}
		else if(args[0].substr(0, 6) == "START:" ||
				args[0].substr(0, 2) == "//")
		{
		}
		else if(args[0].substr(0, 5) == "func_") // function label, save addr for mjo too
		{
		}
		else // has arguments
		{
			short opcode = GetOpcode(args[1]);
			
			for(int x=2; x < args.size(); x++)
			{
				int arg = 0;

				if(opcode == 0x0840)
				{
					if(args[x][0] == '<') // string	
					{
						char null = '\0';
						string line = args[x].substr(1, args[x].length() - 2);
						unsigned short len = (unsigned short)line.length();
						//cout << line << endl;

						if(line[0] == '@' && len == 9)
						{
							string id = line.substr(1, len - 1);
							line = GetString(id);

							if(line == "")
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

void ParseCode()
{
	ofstream outfile("temp.bin", ifstream::out | ifstream::binary);
	int idx = 0;

	if(!outfile.good())
	{
		cout << "Could not open temp.bin" << endl;
		exit(1);
	}

	//cout << infile.size() << endl;
	for(int i=0; i < infile.size(); i++)
	{
		/*
		if(infile[i].length() >= 2 &&
			infile[i].substr(0, 2) != "//" && 
			infile[i].find("call_0842	<n>") != infile[i].npos)
		{
			bool next = findNextText(i+1,true);

			if(next)
			{
				std::cout << infile[i] << std::endl;
				continue;
			}
		}
		*/

		vector<string> args;
		split(args, infile[i], boost::is_any_of("\t"), boost::token_compress_on);

		if(args.size() <= 0)
			continue;

		if(args[0] == "entry:") // entry point, save addr in mjo
		{
			head_entry = outfile.tellp();
		}
		else if(args[0].substr(0, 6) == "START:" ||
				args[0].substr(0, 2) == "//")
		{
			// ignore
		}
		else if(args[0].substr(0, 5) == "func_") // function label, save addr for mjo too
		{
			int crc32 = GetInt(args[0].substr(5, 8));
			int offset = outfile.tellp();
			int *t = new int[2];

			t[0] = crc32;
			t[1] = offset;

			funcs.push_back(t);
		}
		else // has arguments
		{
			short opcode = GetOpcode(args[1]);

			string label = args[0].substr(0, 8);
			labels[label] = outfile.tellp();
			
			outfile.write((char*)&opcode, 2);

			// jump opcodes
			if(opcode == 0x82c || opcode == 0x82d || opcode == 0x82e ||
				opcode == 0x830 || opcode == 0x831 || opcode == 0x832 ||
				opcode == 0x833 || opcode == 0x838 || opcode == 0x839)
			{
				//cout << infile[i] << " " << labels[args[2]] << endl;
				//jumps.push_back(outfile.tellp());

				if(idx >= jumps.size())
				{
					jumps.reserve(idx + 1);
				}

				int arg = GetInt(args[3]);
				int *t = new int[2];
				t[0] = boost::lexical_cast<int>(outfile.tellp());
				t[1] = GetInt(args[2]);
				jumps.push_back(t);

				outfile.write((char*)&arg, 4);
				
				//cout << hex << t[0] << " " << t[1] << endl;
			}
			else if(opcode == 0x83a)
			{
				unsigned short arg = GetShort(args[2]);
				head_lines = arg;

				outfile.write((char*)&arg, 2);
			}
			else
			{
				for(int x=2; x < args.size(); x++)
				{
					int arg = 0;
	
					if(args[x][0] == '<') // string	
					{
						char null = '\0';
						string line = args[x].substr(1, args[x].length() - 2);
						unsigned short len = (unsigned short)line.length();
						//cout << line << endl;

						bool skipWrite = false;
						if(line[0] == '@' && len == 9)
						{
							string id = line.substr(1, len - 1);
							line = GetString(id);

							size_t offset = outfile.tellp();

							unsigned short len = 0;
							outfile.write((char*)&len, 2);

							for(int i=0; i<line.length(); i++)
							{
								if(line[i] == '\\')
								{
									unsigned short opcode2 = 0x0842;
									unsigned int arg2 = 0;

									if(line[i+1] == 'n') // new line
									{
										arg2 = 0x006e0002;
									}
									else if(line[i+1] == 'p') // pause
									{
										arg2 = 0x00700002;
									}
									else if(line[i+1] == 'w') // wipe
									{
										arg2 = 0x00770002;
									}

									if(arg2 != 0)
									{
										if(i - 2 > 0 &&  // check if we can go back 2 characters
										   !(line[i-2] == '\\' && // check if the second to last character is a \\ and if it is, if it's a command
										     (line[i-1] == 'n' ||
										      line[i-1] == 'p' ||
										      line[i-1] == 'w')))
										{
											// if all of that checks out we know we are writing a text string instead of command again
											outfile.write(&null, 1);
											len++;

											size_t curoffset = outfile.tellp();
											//cout << hex << i << " " << line[i - 2] << " " << len << endl;

											outfile.seekp(offset);
											outfile.write((char*)&len, 2);
											outfile.seekp(curoffset);

											unsigned short arg3 = 0x0841;
											outfile.write((char*)&arg3, 2);
										}

										outfile.write((char*)&opcode2, 2);
										outfile.write((char*)&arg2, 4);

										len = 0;
										i++;

										if(i+2 < line.length() && // check if we can even go 2 more characters over
											!(line[i+1] == '\\' && // check if the next character is a \\ and if it is, if it's a command
											  (line[i+2] == 'n' ||
											   line[i+2] == 'p' ||
											   line[i+2] == 'w')))
										{
											if(i < line.length())
											{	
												outfile.write((char*)&opcode, 2);
	
												offset = outfile.tellp();
												outfile.write((char*)&len, 2);
											}
										}
									}
									else
									{
										outfile.write(&line[i], 1);
										len++;
									}
								} // '\\' check
								else
								{
									outfile.write(&line[i], 1);
									len++;
								}
							} // loop end

							if(len > 0)
							{
								outfile.write(&null, 1);
								len++; // account for null terminator

								size_t curoffset = outfile.tellp();
								outfile.seekp(offset);
								outfile.write((char*)&len, 2);
								outfile.seekp(curoffset);
							}

							line = "";
							skipWrite = true;
						}
	
						len = (unsigned short)line.length();

						if(!skipWrite)
						{
							len++; // account for null terminator
							outfile.write((char*)&len, 2);
		
							for(int i=0; i < len - 1; i++)
							{
								outfile.write(&line[i], 1);
							}
	
							outfile.write(&null, 1);
						}
					}
					else
					{
						if(args[x].length() == 8) // int 
						{
							arg = GetInt(args[x]);
							outfile.write((char*)&arg, 4);
						}
						else if(args[x].length() == 4) // short
						{
							arg = GetShort(args[x]);
							outfile.write((char*)&arg, 2);
						}
						else if(args[x].length() == 2) // char
						{
							arg = GetByte(args[x]);
							outfile.write((char*)&arg, 1);
						}
	
						//cout << hex << arg << endl;
					}
				}
			}
		}
	}

	outfile.close();
}

void FixJumps()
{
	ofstream outfile("temp.bin", ifstream::in | ifstream::out | ifstream::binary);

	//cout << jumps.size() << endl;

	for(int i = 0; i < jumps.size(); i++)
	{
		char _label[10];
		string label;

		sprintf(_label, "%08x\0", jumps[i][1]);
		label = boost::lexical_cast<string>(_label);

		outfile.seekp(jumps[i][0]);
		int offset = labels[label] - jumps[i][0] - 4;
		outfile.write((char*)&offset, 4);

		cout << hex << jumps[i][0] << ": " << jumps[i][1] << "->" << labels[label] << "(" << (labels[label] - jumps[i][0] - 4) << ")" << endl;
	}

	outfile.close();
}

void WriteHeader(char *filename)
{
	ofstream outfile(filename, ifstream::out | ifstream::binary);
	char mjo[] = "MajiroObjX1.000\0";
	
	outfile.write(mjo, strlen(mjo) + 1);
	outfile.write((char*)&head_entry, 4);
	outfile.write((char*)&head_lines, 4);

	int funcCount = funcs.size();
	outfile.write((char*)&funcCount, 4);

	for(int i=0; i<funcCount; i++)
	{
		outfile.write((char*)&funcs[i][0], 4);
		outfile.write((char*)&funcs[i][1], 4);
	}

	ifstream infile("temp.bin", ifstream::in | ifstream::binary);

	int len = 0;

	infile.seekg(0, ios::end);
	len = infile.tellg();
	infile.seekg(0, ios::beg);

	unsigned char *buffer = new unsigned char[len];
	infile.read((char*)buffer, len);

	EncryptBuffer(buffer, len);

	outfile.write((char*)&len, 4);
	outfile.write((char*)buffer, len);

	infile.close();
}

short GetOpcode(string call)
{
	short output = 0;
	string opstr = call.substr(5, 4);

	//cout << "orig: " << opstr << endl;
	output = GetShort(opstr);
	//cout << "converted: " << hex << output << endl << endl;

	return output;
}

void EncryptBuffer(unsigned char *data, int size)
{
	int *pdata = (int*)data;
	unsigned char *pcrc = (unsigned char*)&crctable;
	int i=0,x=0;
	int loop1 = (size >> 10) << 10;
	
	for(i=0; i * 4 < loop1; i++)
	{
		pdata[i] ^= crctable[x&0xff];
		x++;
	}
	
	i *= 4;
	x = 0;
	
	while(i < size)
	{
		data[i] ^= pcrc[x&0x7ff]; // cap it at 0x400
		i++, x++;
	}
}
