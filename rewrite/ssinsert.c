#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "structs.h"

FILE *origscript = NULL, *text = NULL, *outfile = NULL;
Entry *stringtable = NULL;
wchar_t *string = NULL;

void ParseText()
{
	int i=0, x=0, j=0, oldlen = 0, b = scrhead.strindex.size;
	
	fread(&i,1,2,text);
	
	if(i!=0xfeff) // skip unicode BOM
		rewind(text);
		
	scrhead.strindex.size = scrhead.strtable.size = 0;
	
	while(!feof(text))
	{
		wchar_t temp[2048];
		
		memset(temp,'\0',2048);
		fgetws(temp,2048,text);
		
		if(temp[0]=='\0')
			break;
		
		if(temp[0]=='<')
		{
			int len = 0, k=0;
			
			if(temp[wcslen(temp)-1]=='\r' || temp[wcslen(temp)-1]=='\n')
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
				else if(temp[k]=='\\' && temp[k+1]=='\\')
					break;
			}
			temp[k] = '\0';
			
			len = wcslen(temp+7);
			i++;
						
			if(scrhead.strtable.size==0)
			{
				string = (wchar_t*)calloc(len+1,sizeof(wchar_t));
				stringtable = (Entry*)calloc(j+1,sizeof(Entry));
			}
			else
			{
				string = (wchar_t*)realloc(string,(x+len+1)*2);
				stringtable = (Entry*)realloc(stringtable,(j+1)*sizeof(Entry));
			}
			
			wcsncat(string,temp+7,wcslen(temp+7));
				
			stringtable[j].offset = oldlen;
			stringtable[j].size = len;
			
			x += len, j++, oldlen += len;
			scrhead.strindex.size += 1;
			scrhead.strtable.size += 1;
		}
	}
	
	if(scrhead.strindex.size!=b)
	{
		printf("Found %d lines, expected %d\n",scrhead.strindex.size,b);
		exit(1);
	}
}

void InsertScript()
{
	char *buffer = NULL;
	int size = 0;
	
	fseek(origscript,0,SEEK_END);
	size = ftell(origscript)-scrhead.headersize;
	fseek(origscript,scrhead.headersize,SEEK_SET);
	
	buffer = (char*)calloc(size,sizeof(char));
	fread(buffer,1,size,origscript);
	
	scrhead.strindex.offset = size+scrhead.headersize;
	scrhead.strtable.offset = scrhead.strindex.offset+scrhead.strindex.size*sizeof(Entry);
	
	fwrite(&scrhead,sizeof(scrhead),1,outfile);	
	fwrite(buffer,1,size,outfile);	
	fwrite(stringtable,sizeof(Entry),scrhead.strindex.size,outfile);
	fwrite(string,sizeof(wchar_t),wcslen(string),outfile);
	fwrite("\0\0",1,2,outfile);
}
	
int main(int argc, char **argv)
{	
	if(argc!=4)
	{
		printf("usage: %s inscript intext outscript\n",argv[0]);
		return 0;
	}
	
	origscript = fopen(argv[1],"rb");
	if(!origscript)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	text = fopen(argv[2],"rb");
	if(!text)
	{
		printf("Could not open %s\n",argv[2]);
		return -1;
	}
	
	outfile = fopen(argv[3],"wb");
	
	fread(&scrhead,sizeof(scrhead),1,origscript);
	
	setlocale(LC_ALL, "Japanese");
	ParseText();
	InsertScript();
	
	return 0;
}
