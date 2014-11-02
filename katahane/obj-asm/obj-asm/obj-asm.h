#ifndef __OBJ_ASM_H__
#define __OBJ_ASM_H__
typedef boost::unordered_map< string, string > Map;
typedef boost::unordered_map< string, long > Labels;
typedef vector < int * > Functions;
typedef vector < int * > Jumps;

void ReadASMData(char *);
void ReadScriptData(char *);
void ParseCode();
void FixJumps();
void WriteHeader(char *);
string GetString(int);
void EncryptBuffer(unsigned char *, int);

short GetOpcode(string);


vector < string > infile;
Map script;
Labels labels;
Jumps jumps;
Functions funcs;

#endif