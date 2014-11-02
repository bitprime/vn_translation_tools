/*
	.ss script assembler for Rewrite/Siglus Engine
*/

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
//#include "compression.h"
#include "bytecode.h"
#include "structs.h"

FILE *infile = NULL, *outfile = NULL;

Entry *strindex = NULL;
Stack bytestack;
StackInt labelstack;
int markerstack[100];
int *unk12stack = NULL, *unk13stack = NULL;
wchar_t *strtable = NULL;

int GetType(char *string)
{	
	if(string[0]=='[' && string[3]==']')
		return 1;
	else if(strncmp(string,"SetLabel",8)==0)
		return 3;
	else if(strncmp(string,"SetMarker",9)==0)
		return 4;
	else if(strncmp(string,"#include",8)==0)
		return 5;
	else if(string[0]=='@')
		return 6;
	else if(string[0]=='#')
		return 7;
	else if(string[0]!='\0' && string[0]!='\n' && strncmp(string,"//",2)!=0)
		return 2;
		
	return 0;
}

int GetID(char *string)
{
	char *temp = (char*)calloc(strlen(string),sizeof(char));
	int i=0,x=0,out=-1;
	
	if(strncmp(string,"SetLabel",8)==0)
		i = 9;
	else if(strncmp(string,"SetMarker",9)==0)
		i = 10;
	else if(string[0]=='[')
		i = 1;
	
	for(; i<strlen(string) && (string[i]>='0' && string[i]<='9'); i++, x++)
		temp[x] = string[i];
	
	if(strlen(temp)>0)
	{
		if(string[i]==']') // [id]s are in hex
			sscanf(temp,"%x",&out);
		else
			sscanf(temp,"%d",&out);
	}
			
	return out;
}

char *GetFuncName(char *string)
{
	char *output = (char*)calloc(strlen(string),sizeof(char));
	int i=0;
	
	while(string[i]!='(' && i<strlen(string))
		output[i] = string[i++];

	return output;
}

int GetOpcode(char *string)
{
	int i=0;
	
	for(i=0; i<sizeof(opcode)/sizeof(*opcode); i++)
	{		
		if(strcmp(opcode[i].name,string)==0)
			return (int)opcode[i].opcode;
	}
	
	return -1;
}

void Push(Stack *instack, int input, int len)
{
	if(instack->size>0)
		instack->stack = (unsigned char*)realloc(instack->stack,instack->size+len);
	else
		instack->stack = (unsigned char*)calloc(len,sizeof(char));
		
	memcpy(instack->stack+instack->size,&input,len);		
	instack->size += len;
}

void PushInt(StackInt *instack, int input, int idx)
{
	if(instack->size==0)
		instack->stack = (int*)calloc(idx+1,sizeof(int));
	else if(instack->size<idx)
		instack->stack = (int*)realloc(instack->stack,idx+1);
	
	instack->stack[idx] = input;
	
	if(instack->size<=idx)
		instack->size = idx+1;
}

void PushArgs(Stack *stack, char *string, int opcode, int start)
{
	int i=start;
	
	if(string[i]=='(')
	{
		i++;
		
		while(string[i]!=')')
		{
			char arg[24];
			int x=0,argn=0,textid=0;
			
			memset(arg,'\0',24);
			
			if(string[i]=='#' && string[i+1]=='<')
			{
				textid = 1;
				i+=2;
			}
				
			while((string[i]>='0' && string[i]<='9') || (string[i]>='a' && string[i]<='f'))
				arg[x++] = string[i++];
			
			if(textid)
			{
				sscanf(arg,"%d",&argn);
				x = 8;
			}
			else
			{
				sscanf(arg,"%x",&argn);
			}
				
			Push(stack,argn,x/2);
			
			if(textid)
				i++;
			
			if(string[i]==',')
			{
				//while((string[i+1]<'0' && string[i+1]>'9') || (string[i+1]<'a' && string[i+1]>'f') || string[i]!='#' || string[i]!=')')
				while(string[i+1]!=')' && string[i]!='#' && (string[i]<'0' || string[i]>'9') && (string[i]<'a' || string[i]>'f'))
					i++;
			}
		}
	}
}

void ParseText(FILE *infile)
{
	int i=0, x=0, j=0, oldlen = 0;
	
	fread(&i,1,2,infile);
	
	if(i!=0xfeff) // skip unicode BOM
		rewind(infile);		
	
	while(!feof(infile))
	{
		wchar_t temp[2048];
		
		memset(temp,'\0',2048);
		fgetws(temp,2048,infile);
		
		if(temp[0]=='\0')
			break;
		
		if(temp[0]=='<')
		{
			int len = 0;
			
			while(temp[wcslen(temp)-1]=='\r' || temp[wcslen(temp)-1]=='\n')
				temp[wcslen(temp)-1] = '\0';
			
			len = wcslen(temp+7);
			i++;
						
			if(scrhead.strtable.size>0)
				strtable = (wchar_t*)realloc(strtable,(x+len+1)*2);
			else
				strtable = (wchar_t*)calloc(len+1,sizeof(wchar_t));
			
			//wprintf(L"%s\n",temp+7);
			//memcpy(strtable+x,temp+7,wcslen(temp+7));
			wcsncat(strtable,temp+7,wcslen(temp+7));
			
			if(scrhead.strindex.size==0)
				strindex = (Entry*)calloc(j+1,sizeof(Entry));
			else
				strindex = (Entry*)realloc(strindex,(j+1)*sizeof(Entry));
				
			strindex[j].offset = oldlen;
			strindex[j].size = len;
			
			x += len, j++, oldlen += len;
			scrhead.strindex.size += 1;
			scrhead.strtable.size = x*2;
		}
	}
}

void ParseScript()
{
	char str[512];
	
	while(!feof(infile))
	{	
		memset(str,'\0',512);
		fgets(str,512,infile);
		
		switch(GetType(str))
		{
			case 1:
			{
				int opcode = GetID(str);
				
				if(opcode!=-1)
				{
					Push(&bytestack,opcode,1);
					PushArgs(&bytestack,str,opcode,4);
				}
			}
			break;
			
			case 2:
			{
				char *name = GetFuncName(str);
				int opcode = GetOpcode(name);
				
				Push(&bytestack,opcode,1);
				PushArgs(&bytestack,str,opcode,strlen(name));
				
				free(name);
			}
			break;
			
			case 3:
			{
				int id = GetID(str);
				
				if(id!=-1)
				{
					PushInt(&labelstack,bytestack.size,id);
					scrhead.labels.size++;
				}
				else
				{
					printf("Problem parsing label: %s",str);
					exit(1);
				}
			}
			break;
				
			case 4:
			{
				int id = GetID(str);
				
				if(id!=-1)
				{
					//Push(&markerstack,id,4);
					markerstack[id] = bytestack.size;
					scrhead.markers.size = 100;
				}
				else
				{
					printf("Problem parsing marker: %s",str);
					exit(1);
				}
			}
			break;
				
			case 5:
			{
				FILE *textfile = NULL;
				
				while(str[strlen(str)-1]=='\r' || str[strlen(str)-1]=='\n')
					str[strlen(str)-1] = '\0';
					
				textfile = fopen(str+9,"rb");
				
				if(!textfile)
				{
					printf("Could not open %s\n",str+9);
					exit(1);
				}
				
				ParseText(textfile);
				fclose(textfile);
			}
			break;
			
			case 6:
			{
				int t = 0;
				sscanf(str+1,"%x",&t);
				
				if(scrhead.unk12.size==0)
					unk12stack = (int*)calloc(1,sizeof(int));
				else
					unk12stack = (int*)realloc(unk12stack,(scrhead.unk12.size+1)*sizeof(int));
				
				unk12stack[scrhead.unk12.size++] = t;
			}
			break;
			
			case 7:
			{
				int t = 0;
				sscanf(str+1,"%x",&t);
				
				if(scrhead.unk13.size==0)
					unk13stack = (int*)calloc(1,sizeof(int));
				else
					unk13stack = (int*)realloc(unk13stack,(scrhead.unk13.size+1)*sizeof(int));
					
				unk13stack[scrhead.unk13.size++] = t;
			}
			break;
		
			default:
				break;
		}
	}
}

int main(int argc, char **argv)
{
	unsigned char *buffer = NULL, *output = NULL;
	int strtablesize = 0, pos = 0;
	
	if(argc!=3)
	{
		printf("usage: %s inscript outfile\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"r");
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	setlocale(LC_ALL, "Japanese");
	
	ParseScript();
	
	outfile = fopen(argv[2],"wb");
	
	strtablesize = scrhead.strtable.size;
	scrhead.headersize = sizeof(scrhead);
	scrhead.strtable.offset = scrhead.headersize+scrhead.strindex.size*sizeof(Entry);
	scrhead.bytecode.offset = scrhead.strtable.offset+scrhead.strtable.size;
	scrhead.bytecode.size = bytestack.size;
	scrhead.strindex.offset = scrhead.headersize;
	scrhead.strtable.size = scrhead.strindex.size;
	scrhead.labels.offset = scrhead.bytecode.offset+scrhead.bytecode.size;
	scrhead.markers.offset = scrhead.labels.offset+scrhead.labels.size*4;
	scrhead.unk3.offset = scrhead.unk4.offset = scrhead.unk5.offset = scrhead.unk6.offset = scrhead.markers.offset+scrhead.markers.size*4;
	scrhead.unk7.offset = scrhead.unk8.offset = scrhead.unk9.offset = scrhead.unk10.offset = scrhead.markers.offset+scrhead.markers.size*4;
	scrhead.unk11.offset = scrhead.markers.offset+scrhead.markers.size*4;
	scrhead.unk12.offset = scrhead.markers.offset+scrhead.markers.size*4;
	scrhead.unk13.offset = scrhead.unk12.offset+scrhead.unk12.size*4;
	
	buffer = (unsigned char*)calloc(scrhead.unk13.offset+scrhead.unk13.size*4,sizeof(unsigned char));
	
	/*
	fwrite(&scrhead,1,sizeof(scrhead),outfile);
	fwrite(strindex,sizeof(Entry),scrhead.strindex.size,outfile);
	fwrite(strtable,1,strtablesize,outfile);
	fwrite(bytestack.stack,1,scrhead.bytecode.size,outfile);
	fwrite(labelstack.stack,4,scrhead.labels.size,outfile);
	fwrite(&markerstack,4,scrhead.markers.size,outfile);
	fwrite(unk12stack,4,scrhead.unk12.size,outfile);
	fwrite(unk13stack,4,scrhead.unk13.size,outfile);
	*/
	
	memcpy(buffer+pos,&scrhead,sizeof(scrhead));	pos += sizeof(scrhead);
	memcpy(buffer+pos,strindex,sizeof(Entry)*scrhead.strindex.size);	pos += sizeof(Entry)*scrhead.strindex.size;
	memcpy(buffer+pos,strtable,strtablesize);	pos += strtablesize;
	memcpy(buffer+pos,bytestack.stack,scrhead.bytecode.size);	pos += scrhead.bytecode.size;
	memcpy(buffer+pos,labelstack.stack,4*scrhead.labels.size);	pos += 4*scrhead.labels.size;
	memcpy(buffer+pos,&markerstack,4*scrhead.markers.size); pos += 4*scrhead.markers.size;
	memcpy(buffer+pos,unk12stack,4*scrhead.unk12.size);	pos += 4*scrhead.unk12.size;
	memcpy(buffer+pos,unk13stack,4*scrhead.unk13.size);	pos += 4*scrhead.unk13.size;
	
	//output = CompressData(buffer,pos,&pos);
	//fwrite(output,1,pos,outfile);
	fwrite(buffer,1,pos,outfile);
	
	free(buffer);
	free(output);
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
