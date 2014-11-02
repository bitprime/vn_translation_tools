/*
	.ss script disassembler for Rewrite/Siglus Engine
*/

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "bytecode.h"
#include "structs.h"

FILE *infile = NULL, *outfile = NULL;
Entry *strings = NULL, *strings2 = NULL, *strings3 = NULL, *strings4 = NULL, *unk3 = NULL, *unk4 = NULL;
int *labels = NULL, *markers = NULL, *unk7 = NULL, *unk12 = NULL, *unk13 = NULL;
int *stack = NULL, *cmdstack = NULL, stacksize = 0, cmdstacksize = 0, stacksizecopy = 0, cmdstacksizecopy = 0;

#define Push(stack, stacksize, stacksizecopy, input) \
	if(stack!=NULL && stacksizecopy > stacksize) \
		stack = (int*)realloc(stack,(stacksize+1)*sizeof(int)); \
	else\
		stack = (int*)calloc(stacksize+1,sizeof(int)); \
		 \
	stack[stacksize] = input; \
	stacksize++, stacksizecopy = stacksize;

#define FreeStack(stack, stacksize, stacksizecopy)\
	free(stack); \
	stack = NULL, stacksize = 0, stacksizecopy = 0;

void DumpStrings(Entry *input, Entry strtable, Entry index, char *start, char *end)
{
	int i=0;
	
	fseek(infile,index.offset,SEEK_SET);
	fread(input,1,index.size*8,infile);
		
	for(i=0; i<index.size; i++)
	{
		wchar_t *string = (wchar_t*)calloc((input[i].size+1)*2,sizeof(wchar_t)), *outstr = NULL;
		int j=0, x=0;
		
		fseek(infile,strtable.offset+(input[i].offset*2),SEEK_SET);
		
		fwprintf(outfile,L"%s%04d%s ",start,i,end);
		fread(string,2,input[i].size,infile);
		
		outstr = (wchar_t*)calloc(wcslen(string)*2,sizeof(wchar_t));
		for(j=0; j<wcslen(string); j++,x++)
		{
			if(string[j]=='\n')
				outstr[x++] = '\\', outstr[x] = 'n';
			else
				outstr[x] = string[j];
		}
		
		fwprintf(outfile,L"%s\r\n",outstr);
		
		free(string);
		free(outstr);
	}
	
	printf("Dumped %d lines\n",strtable.size);
}

void ParseTable(int *output, Entry input)
{
	fseek(infile,input.offset,SEEK_SET);
	fread(output,4,input.size,infile);
}

void ParseTableEntry(Entry *output, Entry input)
{
	fseek(infile,input.offset,SEEK_SET);
	fread(output,sizeof(Entry),input.size,infile);
}

int IsLabel(int *input, int id, int size)
{
	int i=0;
	
	
	for(i=0; i<size; i++)
	{
		if(input[i]!=0 && id==input[i]+scrhead.bytecode.offset)
			return i;
	}
	
	return -1;
}

int ReadArgs(char o)
{
	int count = 0, argcount = 0, var = 0;	
	
	fread(&count,1,4,infile);
	fprintf(outfile,"%08x",count); // amount of arguments passed to the function
	argcount = count;
	
	while(--count>=0) // used to compare the last x amount of variables on the stack
	{
		fread(&var,1,4,infile);
		fprintf(outfile,", %08x",var);
		
		if(var==0xffffffff)
		{
			//printf("%08x\n",count);
			fprintf(outfile,", ");
			ReadArgs(o);
		}
	}
	
	return argcount;
}

void DumpBytecode(char *filename, char *origname, char *stringsname)
{
	FILE *outfile = fopen(filename,"w");
	int pushed = 0, i = 0;
	
	fprintf(outfile,"// Disassembly of %s\n\n",origname);
	fprintf(outfile,"#include %s\n\n",stringsname);
	
	// parser stuff
	fseek(infile,scrhead.bytecode.offset,SEEK_SET);
	
	while(ftell(infile)<scrhead.bytecode.offset+scrhead.bytecode.size)
	{
		int o = 0, id = 0, id2 = 0, id3 = 0;
		
		id = IsLabel(labels,ftell(infile),scrhead.labels.size);
		id2 = IsLabel(markers,ftell(infile),scrhead.markers.size);
		id3 = IsLabel(unk7,ftell(infile),scrhead.unk7.size);
		
		if(id!=-1)
			fprintf(outfile,"\nSetLabel %d:\n",id);
		if(id2!=-1)
		{
			if(id==-1) fprintf(outfile,"\n");
			fprintf(outfile,"SetMarker %d:\n",id2);
		}
		if(id3!=-1)
		{
			if(id2==-1) fprintf(outfile,"\n");
			fprintf(outfile,"SetFunction %d:\n",id3);
		}
		
		
		fprintf(outfile,"<%08x> ",ftell(infile));
		fread(&o,1,1,infile);
		fprintf(outfile,"%02x ",o);
		
		if(o<0 || o>0x32)
		{
			printf("Invalid opcode %02x found at %08x\n",opcode,ftell(infile)-1);
			exit(1);
		}
		else if(o!=0)
		{
			int var = 0;
			char var2 = 0;
			
			if(strcmp(opcode[o].name,"")==0 || opcode[o].type==0)
				fprintf(outfile,"[%02x]",o);
			else
				fprintf(outfile,"%s", opcode[o].name);
			fprintf(outfile,"(");
				
			if(opcode[o].type != 7)
			{
				switch(opcode[o].type)
				{
					case 0:
						fprintf(outfile,"%s",opcode[o].name);
						break;
						
					case 1:
						break;
						
					case 2:
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x",var);
						break;
						
					case 3:
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x",var);
						break;
						
					case 4:
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x",var);
						break;
						
					case 5:
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						fread(&var2,1,1,infile);
						fprintf(outfile,"%02x",var2);
						break;
						
					case 6:
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						fread(&var2,1,1,infile);
						fprintf(outfile,"%02x",var2);
						break;
				}
			}
			else
			{
				switch(o)
				{
					case 0x02:
					{
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						
						switch(var)
						{						
							case 0x14: // string
								fread(&var,1,4,infile);
								fprintf(outfile,"#<%04d>",var);
								break;
								
							default: // not a string
								fread(&var,1,4,infile);
								fprintf(outfile,"%08x",var);
								break;
						}
						
						Push(stack,stacksize,stacksizecopy,var);
					}
					break;
					
					case 0x08:
					{
						if(cmdstack==NULL)
							cmdstack = (int*)calloc(10000,sizeof(int));

						cmdstack[cmdstacksize] = stacksize;
						cmdstacksize++;
					}
					break;
					
					case 0x15:
					{
						ReadArgs(o);
					}
					break;
					
					case 0x30:
					{	
						int argcount = 0, m = 0;
						
						fread(&var,1,4,infile);
						fprintf(outfile,"%08x, ",var);
						
						argcount = ReadArgs(o);
						
						fread(&var,1,4,infile);
						fprintf(outfile,", %08x",var); // unknown
						argcount = var;
						
						if(argcount>0)
						{
							for(m=0; m<argcount; m++)
							{
								fread(&var,1,4,infile);
								fprintf(outfile,", %08x",var);
							}
						}
						
						fread(&var,1,4,infile);
						fprintf(outfile,", %08x",var); // target opcode variable type
						
						while(--cmdstacksize>0)
						{
							switch(cmdstack[cmdstacksize])
							{								
								case 0x0c:
								case 0x16: // goes back and starts reading more input
									//printf("%08x %08x %08x\n",cmd,stack[cmd],stack[cmd+1]);
									//exit(1);
									fread(&var,1,4,infile);
									fprintf(outfile,", %08x",var);
									fread(&var,1,4,infile);
									fprintf(outfile,", %08x",var);
									fread(&var,1,1,infile);
									fprintf(outfile,", %02x",var);
									break;
							
								case 0x26:
									break;
							
								case 0x4c:
								case 0x4d:
									fread(&var,1,4,infile);
									fprintf(outfile,", %08x",var);
									break;
							}
						}
						
						cmdstacksize = 0;
					}
					
					break;
				}
			}
			
			if(opcode[o].free==1)
			{
				FreeStack(cmdstack,cmdstacksize,cmdstacksizecopy);
			}
		}
		
		if(o!=0)
			fprintf(outfile,")\n");
	}
	
	for(i=0; i<scrhead.unk4.size; i++)
		fprintf(outfile,"\n%%%08x %08x",unk4[i].offset,unk4[i].size);
//	for(i=0; i<scrhead.unk7.size; i++)
//		fprintf(outfile,"\n%%08x",unk7[i]);
	for(i=0; i<scrhead.unk12.size; i++)
		fprintf(outfile,"\n@%08x",unk12[i]);
	for(i=0; i<scrhead.unk13.size; i++)
		fprintf(outfile,"\n#%08x",unk13[i]);
	
	printf("Dumped bytecode to %s\n",filename);
	fclose(outfile);
}

int main(int argc, char **argv)
{
	char *outstrings = NULL, *outbyte = NULL;
	int i = 0;
	
	if(argc!=4)
	{
		printf("usage; %s infile outstring outcode\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"rb");
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	fread(&scrhead,1,sizeof(scrhead),infile);
	
	strings = (Entry*)calloc(scrhead.strindex.size,sizeof(Entry));
	strings2 = (Entry*)calloc(scrhead.unk6.size,sizeof(Entry));
	strings3 = (Entry*)calloc(scrhead.unk9.size,sizeof(Entry));
	strings4 = (Entry*)calloc(scrhead.unk11.size,sizeof(Entry));
	unk4 = (Entry*)calloc(scrhead.unk11.size,sizeof(Entry));
	labels = (int*)calloc(scrhead.labels.size+1,sizeof(int));
	markers = (int*)calloc(scrhead.markers.size+1,sizeof(int));
	//unk3 = (Entry*)calloc(scrhead.unk11.size,sizeof(Entry));
	unk7 = (int*)calloc(scrhead.unk7.size+1,sizeof(int));
	unk12 = (int*)calloc(scrhead.unk12.size+1,sizeof(int));
	unk13 = (int*)calloc(scrhead.unk13.size+1,sizeof(int));
		
	setlocale(LC_ALL, "Japanese");

	ParseTable(labels,scrhead.labels);
	ParseTable(markers,scrhead.markers);
	//ParseTable(unk3,scrhead.unk3);
	ParseTable(unk7,scrhead.unk7);
	ParseTable(unk12,scrhead.unk12);
	ParseTable(unk13,scrhead.unk13);
	
	outstrings = (char*)calloc(strlen(argv[1])+10,sizeof(char));
	outbyte = (char*)calloc(strlen(argv[1])+10,sizeof(char));

	for(i=strlen(argv[1]); i>0 && argv[1][i]!='.'; i--);
	strncpy(outstrings,argv[1],i);
	strcat(outstrings,"-text.txt");
	
	strncpy(outbyte,argv[1],i);
	strcat(outbyte,"-code.txt");
	
	printf("%s %s\n",outstrings,outbyte);
	
	outfile = fopen(outstrings,"wb");
	fwrite("\xff\xfe",1,2,outfile);
	DumpStrings(strings, scrhead.strtable, scrhead.strindex, "<", ">");
	DumpStrings(strings2, scrhead.unk6, scrhead.unk5, "[", "]");
	DumpStrings(strings3, scrhead.unk9, scrhead.unk8, "{", "}");
	DumpStrings(strings4, scrhead.unk11, scrhead.unk10, "(", ")");
	ParseTableEntry(unk4,scrhead.unk4);
	fclose(outfile);
	
	DumpBytecode(outbyte,argv[1],outstrings);
	
	fclose(infile);

	return 0;
}