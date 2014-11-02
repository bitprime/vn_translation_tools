#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "structs.h"

int main(int argc, char **argv)
{
	FILE *infile = NULL, *outfile = NULL;
	Entry *strings = NULL;
	int i = 0;
	
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
	
	outfile = fopen(argv[2],"wb");
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
		
		fseek(infile,scrhead.strtable.offset+strings[i].offset*sizeof(wchar_t),SEEK_SET);
		fread(line,sizeof(wchar_t),strings[i].size,infile);
		
		fwprintf(outfile,L"//");
		for(l=0; l<2; l++)
		{
			fwprintf(outfile,L"<%04d> ",i);
		
			for(x=0,j=0; x<strings[i].size; j++, x++)
			{
				if(line[x]=='\n')
					fwprintf(outfile,L"\\n");
				else
					fwprintf(outfile,L"%c",line[x]);
			}
		
			fwprintf(outfile,L"\r\n");
		}
		fwprintf(outfile,L"\r\n");
		
		free(line);
	}
	
	return 0;
}
