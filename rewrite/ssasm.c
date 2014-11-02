/*
	.ss script assembler for Rewrite/Siglus Engine
*/

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "compression.h"
#include "bytecode.h"
#include "structs.h"

FILE *infile = NULL, *outfile = NULL;

Entry *strindex = NULL, *unk5index = NULL, *unk8index = NULL, *unk10index = NULL, *unk3 = NULL, *unk4 = NULL;
Stack bytestack;
StackInt labelstack;
int markerstack[100];
int *unk7stack = NULL, *unk12stack = NULL, *unk13stack = NULL;
wchar_t *strtable = NULL, *unk6 = NULL, *unk9 = NULL, *unk11 = NULL;

int GetType(char *string)
{	
	if(strlen(string)>3 && string[0]=='[' && string[3]==']')
		return 1;
	else if(strncmp(string,"SetLabel",8)==0)
		return 3;
	else if(strncmp(string,"SetMarker",9)==0)
		return 4;
	else if(strncmp(string,"SetFunction",9)==0)
		return 9;
	else if(strncmp(string,"#include",8)==0)
		return 5;
	else if(string[0]=='@')
		return 6;
	else if(string[0]=='#')
		return 7;
	else if(string[0]=='%')
		return 8;
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
	else if(strncmp(string,"SetFunction",11)==0)
		i = 12;
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
		instack->stack = (int*)realloc(instack->stack,(idx+1)*sizeof(int)+1); // the +1 to the sizeof is a hack. maybe look into this later
	
	instack->stack[idx] = input;
	
	if(instack->size<idx)
		instack->size = idx+1;
}

void PushArgs(Stack *stack, char *string, int op, int start)
{
	int i=start;
	
	if(opcode[op].type==0)
	{
		printf("Found nop\n");
		return;
	}
	
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

wchar_t *ParseText(FILE *infile, char marker, Entry *index, Entry *strtable, void *outindexptr)
{
	Entry *outindex = NULL;
	wchar_t *output = NULL;
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
		
		if(temp[0]==marker)
		{
			int len = 0, k=0;
			
			while(temp[wcslen(temp)-1]=='\r' || temp[wcslen(temp)-1]=='\n')
				temp[wcslen(temp)-1] = '\0';
				
			for(k=0; k<wcslen(temp)-1; k++)
			{
				if(temp[k]=='\\' && temp[k+1]=='n')
				{
					int l=0;
					
					temp[k++] = '\x0a', temp[k] = '\0';
					
					for(l=k; j<strlen(temp)-k-2; l++)
						temp[l] = temp[l+1];
				}
			}
			
			len = wcslen(temp+7);
			i++;
						
			if(strtable->size>0)
				output = (wchar_t*)realloc(output,(x+len+1)*2);
			else
				output = (wchar_t*)calloc(len+1,sizeof(wchar_t));
				
			//memcpy(output+x,temp+7,wcslen(temp+7));
			wcsncat(output,temp+7,wcslen(temp+7));
			
			if(index->size==0)
				outindex = (Entry*)calloc(j+1,sizeof(Entry));
			else
				outindex = (Entry*)realloc(outindex,(j+1)*sizeof(Entry));
				
			outindex[j].offset = oldlen;
			outindex[j].size = len;
			
			x += len, j++, oldlen += len;
			index->size += 1;
			strtable->size = x*2;
		}
	}
	
	*(int*)outindexptr = (int)outindex;
	
	rewind(infile);
	return output;
}

void ParseScript()
{
	while(!feof(infile))
	{	
		char *str = (char*)calloc(512,sizeof(char));
		
		//memset(str,'\0',512);
		fgets(str,512,infile);
		
		switch(GetType(str))
		//switch(-1)
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
				
				if(opcode==0x03)
				{
					printf("%s\n",str);
				}
				
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
								
				strtable = ParseText(textfile,'<',&scrhead.strindex,&scrhead.strtable,&strindex);
				unk6 = ParseText(textfile,'{',&scrhead.unk5,&scrhead.unk6,&unk5index);
				unk9 = ParseText(textfile,'[',&scrhead.unk8,&scrhead.unk9,&unk8index);
				unk11 = ParseText(textfile,'(',&scrhead.unk10,&scrhead.unk11,&unk10index);
				fclose(textfile);
			}
			break;
			
			case 6:
			{
				if(scrhead.unk12.size==0)
					unk12stack = (int*)calloc(1,sizeof(int));
				else
					unk12stack = (int*)realloc(unk12stack,(scrhead.unk12.size+1)*sizeof(int));
				
				sscanf(str+1,"%x",&unk12stack[scrhead.unk12.size++]);
			}
			break;
			
			case 7:
			{
				if(scrhead.unk13.size==0)
					unk13stack = (int*)calloc(1,sizeof(int));
				else
					unk13stack = (int*)realloc(unk13stack,(scrhead.unk13.size+1)*sizeof(int));
					
				sscanf(str+1,"%x",&unk13stack[scrhead.unk13.size++]);
			}
			break;
			
			case 8:
			{
				if(scrhead.unk4.size==0)
					unk4 = (Entry*)calloc(1,sizeof(Entry));
				else
					unk4 = (Entry*)realloc(unk4,(scrhead.unk4.size+1)*sizeof(Entry));
					
				sscanf(str+1,"%x %x",&unk4[scrhead.unk4.size].offset,&unk4[scrhead.unk4.size].size);
				scrhead.unk4.size++;
			}
			
			case 9:
			{
				int id = GetID(str);
				
				if(scrhead.unk3.size==0)
				{
					unk3 = (Entry*)calloc(id+1,sizeof(Entry));
				}
				else if(id>scrhead.unk3.size)
				{
					unk3 = (Entry*)realloc(unk3,(id+1)*sizeof(Entry));
					scrhead.unk3.size = id;
				}
				
				unk3[id].offset = id;
				unk3[id].size = bytestack.size;
			}
		
			default:
				break;
		}
		
		free(str);
	}
}

int main(int argc, char **argv)
{
	unsigned char *buffer = NULL, *output = NULL;
	int strtablesize = 0, pos = 0, unk6size = 0, unk9size = 0, unk11size = 0;
	
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
	
	strtablesize = scrhead.strtable.size, unk6size = scrhead.unk6.size, unk9size = scrhead.unk9.size, unk11size = scrhead.unk11.size;
	scrhead.headersize = sizeof(scrhead);
	scrhead.strtable.offset = scrhead.headersize+scrhead.strindex.size*sizeof(Entry);
	scrhead.bytecode.offset = scrhead.strtable.offset+scrhead.strtable.size;
	scrhead.bytecode.size = bytestack.size;
	scrhead.strindex.offset = scrhead.headersize;
	scrhead.strtable.size = scrhead.strindex.size;
	scrhead.labels.offset = scrhead.bytecode.offset+scrhead.bytecode.size;
	scrhead.markers.offset = scrhead.labels.offset+scrhead.labels.size*4;
	scrhead.unk3.offset =  scrhead.markers.offset+scrhead.markers.size*4;
	scrhead.unk4.offset = scrhead.unk3.offset+scrhead.unk3.size*4;
	scrhead.unk5.offset = scrhead.unk4.offset+scrhead.unk4.size*4;
	scrhead.unk6.offset = scrhead.unk5.offset+scrhead.unk5.size*4;
	scrhead.unk7.offset = scrhead.unk6.offset+scrhead.unk6.size*4;
	scrhead.unk8.offset = scrhead.unk7.offset+scrhead.unk7.size*4;
	scrhead.unk9.offset = scrhead.unk8.offset+scrhead.unk8.size*4;
	scrhead.unk10.offset = scrhead.unk9.offset+scrhead.unk9.size*4;
	scrhead.unk11.offset = scrhead.markers.offset+scrhead.markers.size*4;
	scrhead.unk12.offset = scrhead.markers.offset+scrhead.markers.size*4;
	scrhead.unk13.offset = scrhead.unk12.offset+scrhead.unk12.size*4;
	
	fwrite(&scrhead,1,sizeof(scrhead),outfile);;
	fwrite(strindex,sizeof(Entry),scrhead.strindex.size,outfile);
	fwrite(strtable,1,strtablesize,outfile);
	fwrite(bytestack.stack,1,scrhead.bytecode.size,outfile);
	fwrite(labelstack.stack,4,scrhead.labels.size,outfile);
	fwrite(&markerstack,4,scrhead.markers.size,outfile);
	fwrite(&unk3,4,scrhead.unk3.size,outfile);
	fwrite(&unk4,sizeof(Entry),scrhead.unk4.size,outfile);
	fwrite(&unk5index,sizeof(Entry),scrhead.unk5.size,outfile);
	fwrite(unk6,sizeof(wchar_t),unk6size,outfile);
	fwrite(&unk8index,sizeof(Entry),scrhead.unk8.size,outfile);
	fwrite(unk7stack,4,scrhead.unk7.size,outfile);
	fwrite(unk9,sizeof(wchar_t),unk9size,outfile);
	fwrite(&unk10index,sizeof(Entry),scrhead.unk10.size,outfile);
	fwrite(unk11,sizeof(wchar_t),unk11size,outfile);
	fwrite(unk12stack,4,scrhead.unk12.size,outfile);
	fwrite(unk13stack,4,scrhead.unk13.size,outfile);
	
	/*
	buffer = (unsigned char*)calloc(scrhead.unk13.offset+scrhead.unk13.size*4,sizeof(unsigned char));
	memcpy(buffer+pos,&scrhead,sizeof(scrhead));	pos += sizeof(scrhead);
	memcpy(buffer+pos,strindex,sizeof(Entry)*scrhead.strindex.size);	pos += sizeof(Entry)*scrhead.strindex.size;
	memcpy(buffer+pos,strtable,strtablesize);	pos += strtablesize;
	memcpy(buffer+pos,bytestack.stack,scrhead.bytecode.size);	pos += scrhead.bytecode.size;
	memcpy(buffer+pos,labelstack.stack,4*scrhead.labels.size);	pos += 4*scrhead.labels.size;
	memcpy(buffer+pos,&markerstack,4*scrhead.markers.size); pos += 4*scrhead.markers.size;
	memcpy(buffer+pos,unk12stack,4*scrhead.unk12.size);	pos += 4*scrhead.unk12.size;
	memcpy(buffer+pos,unk13stack,4*scrhead.unk13.size);	pos += 4*scrhead.unk13.size;
	
	//output = CompressData(buffer,pos,&pos);
	fwrite(buffer,1,pos,outfile);
	*/
	
	free(buffer);
	free(output);
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
