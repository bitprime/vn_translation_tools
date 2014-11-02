#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#pragma pack(1)
typedef struct HEADER // @00h
{
	char head[16]; // @00h
	int files; // @10h
	int namestart; // @14h
	int datastart; // @18h
} ARCHeader;

typedef struct ENTRY // @1ch
{
	int crc32; // @00h
	int offset; // @04h
	int size; // @08h
} ARCEntry;
#pragma pack()

void CreateDir(char *);
void ParseFile(char *, char *);
char **GetFilenames(ARCHeader, FILE *);

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("usage: %s infile outfilder\n",argv[0]);
		return 1;
	}
	
	ParseFile(argv[1], argv[2]);
}


// create output directory and change directory to it
void CreateDir(char *foldername)
{
	mkdir(foldername);
	chdir(foldername);
}

// open the arc file and dump its contents to the current folder
void ParseFile(char *arcname, char *foldername)
{
	FILE *infile = fopen(arcname, "rb");
	char **filenames = NULL;
	ARCHeader header;
	ARCEntry *entries;
	int i = 0;
	
	CreateDir(foldername);
	
	if(!infile)
	{
		printf("Could not open file: %s\n",arcname);
		printf("Error code: %d\n", GetLastError());
		exit(-1);
	}
	
	memset(&header, '\0', sizeof(ARCHeader));
	fread(&header, 1, sizeof(ARCHeader), infile);
	
	entries = (ARCEntry*)calloc(header.files + 1,sizeof(ARCEntry));
	fread(entries, header.files + 1, sizeof(ARCEntry), infile);
	
	filenames = GetFilenames(header, infile);
	
	for(i=0; i<header.files; i++)
	{
		FILE *outfile = fopen(filenames[i], "wb");
		unsigned char *buffer = NULL;
		int len = entries[i].size;
		
		printf("[%04d] %-24s\toffset[%08d] size[%08x] crc32[%08x]\n", i, filenames[i], entries[i].offset, len, entries[i].crc32);
		
		buffer = (unsigned char*)calloc(len, sizeof(unsigned char));
		
		fseek(infile, entries[i].offset, SEEK_SET);
		fread(buffer, 1, len, infile);
		fwrite(buffer, 1, len, outfile);
		
		free(buffer);
		fclose(outfile);
	}	
	
	fclose(infile);
	free(entries);
}

// read the entries in the filename table and return a list
char **GetFilenames(ARCHeader header, FILE *infile)
{
	size_t curoffset = ftell(infile);
	char **outlist = (char**)calloc(header.files + 1, sizeof(char*));
	int i = 0;
	
	fseek(infile, header.namestart, SEEK_SET);
	
	for(i=0; i < header.files; i++)
	{
		int x = 0;
		
		outlist[i] = (char*)calloc(255, sizeof(char));
		
		while(x < 255)
		{
			fread(&outlist[i][x], 1, 1, infile);
			
			if(outlist[i][x] == 0)
				break;
				
			x++;
		}
	}
	
	
	fseek(infile, curoffset, SEEK_SET);
	
	return outlist;
}
