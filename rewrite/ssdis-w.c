/*
	.ss script disassembler for Rewrite/Siglus Engine
*/

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "bytecode.h"
#include "structs.h"

Entry *strings = NULL;
FILE *infile = NULL, *outfile = NULL;
int *labels = NULL, *markers = NULL, *unk12 = NULL, *unk13 = NULL;

void DumpStrings(char *filename)
{
	int i=0;
	
	outfile = fopen(filename,"wb");
	fwrite("\xff\xfe",1,2,outfile);
	
	for(i=0; i<scrhead.strindex.size; i++)
	{
		wchar_t *string = (wchar_t*)calloc((strings[i].size+1)*2,sizeof(wchar_t)), *outstr = NULL;
		int j=0, x=0;
		
		fseek(infile,scrhead.strtable.offset+(strings[i].offset*2),SEEK_SET);
		
		fwprintf(outfile,L"<%04d> ",i);
		fread(string,1,strings[i].size*2,infile);
		
		outstr = (wchar_t*)calloc(wcslen(string)*2,sizeof(wchar_t));
		for(j=0; j<wcslen(string); j++,x++)
		{
			if(string[j]=='\n')
			{
				outstr[x++] = '\\';
				outstr[x] = 'n';
			}
			else
			{
				outstr[x] = string[j];
			}
		}
		
		fwprintf(outfile,L"%s\r\n",outstr);
		
		free(string);
		free(outstr);
	}
	
	printf("Dumped %d lines to %s\n",scrhead.strindex.size,filename);
	fclose(outfile);
}

void ParseTable(int *output, int offset, int size)
{
	fseek(infile,offset,SEEK_SET);
	fread(output,4,size,infile);
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

void DumpBytecode(char *filename, char *origname, char *stringsname)
{
	int pushed = 0, i = 0;
	
	outfile = fopen(filename,"w");
	
	fprintf(outfile,"// Disassembly of %s\n\n",origname);
	fprintf(outfile,"#include %s\n\n",stringsname);
	
	// parser stuff
	fseek(infile,scrhead.bytecode.offset,SEEK_SET);
	
	while(ftell(infile)<scrhead.bytecode.offset+scrhead.bytecode.size)
	{
		int o = 0, id = 0, id2 = 0;
		
		fprintf(outfile,"%08x ",ftell(infile));
		
		id = IsLabel(labels,ftell(infile),scrhead.labels.size);
		if(id!=-1)
			fprintf(outfile,"\nSetLabel %d:\n",id);
		
		id2 = IsLabel(markers,ftell(infile),scrhead.markers.size);
		if(id2!=-1)
		{
			if(id==-1)
				fprintf(outfile,"\n");
				
			fprintf(outfile,"SetMarker %d:\n",id2);
		}
		
		fread(&o,1,1,infile);
		
		if(o<0 || o>0x32)
		{
			printf("Invalid opcode %02x found at %08x\n",opcode,ftell(infile)-1);
			exit(1);
		}
		else
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
							case 0x0a: // imm
								fread(&var,1,4,infile);
								fprintf(outfile,"%08x",var);
								pushed = var;
								break;
							
							case 0x14: // string
								fread(&var,1,4,infile);
								fprintf(outfile,"#<%04d>",var);
								break;
						}
					}
					break;
					
					case 0x15:
					case 0x30:
					{
						int count = 0;
						
						//printf("%08x\n",ftell(infile)-1);
						
						if(o==0x30)
						{
							fread(&var,1,4,infile);
							fprintf(outfile,"%08x, ",var);
						}
						
						fread(&count,1,4,infile);
						fprintf(outfile,"%08x",count);
						
						while(--count>=0)
						{
							fread(&var,1,4,infile);
							fprintf(outfile,", %08x",var);
						}
						
						if(o==0x30)
						{
							fread(&count,1,4,infile);
							fprintf(outfile,", %08x",count);
							
							if(count>0)
							{
								//printf("%08x\n",count);
								exit(1);
							}
							
							if(count-->0)
							{
								fread(&var,1,4,infile);
								fprintf(outfile,", %08x",var);
							}
							
							fread(&var,1,4,infile);
							fprintf(outfile,", %08x",var);
							
							// pushed==0x04 || pushed==0x05
							if(pushed==0x0c || pushed==0x4c || pushed==0x4d)
							{
								fread(&var,1,4,infile);
								fprintf(outfile,", %08x",var);
							}
						}
					}
					break;
				}
			}
		}
		
		fprintf(outfile,")\n");		
	}
	
	for(i=0; i<scrhead.unk12.size; i++)
		fprintf(outfile,"\n@%08x",unk12[i]);
	for(i=0; i<scrhead.unk13.size; i++)
		fprintf(outfile,"\n#%08x",unk13[i]);
	
	printf("Dumped bytecode to %s\n",filename);
	fclose(outfile);
}

int main(int argc, char **argv)
{
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
	labels = (int*)calloc(scrhead.labels.size+1,sizeof(int));
	markers = (int*)calloc(scrhead.markers.size+1,sizeof(int));
	unk12 = (int*)calloc(scrhead.unk12.size+1,sizeof(int));
	unk13 = (int*)calloc(scrhead.unk13.size+1,sizeof(int));
	
	fseek(infile,scrhead.strindex.offset,SEEK_SET);
	fread(strings,1,scrhead.strindex.size*8,infile);
	
	setlocale(LC_ALL, "Japanese");

	ParseTable(labels,scrhead.labels.offset,scrhead.labels.size);
	ParseTable(markers,scrhead.markers.offset,scrhead.markers.size);
	ParseTable(unk12,scrhead.unk12.offset,scrhead.unk12.size);
	ParseTable(unk13,scrhead.unk13.offset,scrhead.unk13.size);
	
	DumpStrings(argv[2]);
	DumpBytecode(argv[3],argv[1],argv[2]);
	
	fclose(infile);

	return 0;
}