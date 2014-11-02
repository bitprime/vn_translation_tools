#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "structs.h"

int main(int argc, char **argv)
{
	FILE *infile = NULL, *outfile = NULL;
	Entry *strings = NULL;
	int i = 0, prevline = 0;
	
	if(argc!=3)
	{
		printf("usage: %s inscript outtext\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"rb");
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	outfile = fopen("script-unicode.txt","ab+");
	fwrite("\xff\xfe",1,2,outfile);
	
	setlocale(LC_ALL, "Japanese");
	
	fread(&scrhead,sizeof(scrhead),1,infile);
	
	strings = (Entry*)calloc(scrhead.strindex.size,sizeof(Entry));
	fseek(infile,scrhead.strindex.offset,SEEK_SET);
	fread(strings,sizeof(Entry),scrhead.strindex.size,infile);
	
	for(i=0; i<scrhead.strindex.size; i++)
	{
		wchar_t *line = (wchar_t*)calloc(strings[i].size+1,sizeof(wchar_t));
		wchar_t *lineparsed = (wchar_t*)calloc(strings[i].size*2,sizeof(wchar_t));
		int x = 0, j = 0, l = 0;
		int key = 0;
		
		fseek(infile,scrhead.strtable.offset+strings[i].offset*sizeof(wchar_t),SEEK_SET);
		fread(line,sizeof(wchar_t),strings[i].size,infile);
		
		// for v2.00 of rewrite trial (and probably newer siglusengine stuff)
		key = i * 0x7087;
		for(l=0; l<strings[i].size; l++)
		{
			line[l] ^= key; 
		}
		
		if(line[0] == L'u' ||
			line[0] == L'w' ||
			line[0] == L'i')
		{
			unsigned char *tbuff = (unsigned char*)calloc(prevline, sizeof(unsigned char));
			int end = 0, size = 0;
			size = prevline;
			
			fclose(outfile);
			outfile = fopen("script-unicode.txt","rb");
			fread(tbuff,1,size,outfile);

			fclose(outfile);
			outfile = fopen("script-unicode.txt","wb");
			fwrite(tbuff,1,size,outfile);
			
			fclose(outfile);
			outfile = fopen("script-unicode.txt","ab+");
			
			free(tbuff);
		}
		else
		{
			prevline = ftell(outfile);
		}
		
		if(line[0] > 0x100)
		{
			for(x=0,j=0; x<strings[i].size; j++, x++)
			{
				if(line[x]=='\n')
					fwprintf(outfile,L"\\n");
				else
					fwprintf(outfile,L"%c",line[x]);
					
				scriptSize++;
			}
		
			fwprintf(outfile,L"\r\n");		
		}
		
		free(line);
	}
	
	return 0;
}
