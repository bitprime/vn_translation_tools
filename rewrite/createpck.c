/*
	.pck archiver for Siglus Engine/Rewrite
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include "compression.h"
#include "structs.h"

FILE *infile = NULL, *outfile = NULL;

FileEntry *fileinfo = NULL;
wchar_t *filenamebuff = NULL;
int filecount = 0, buffsize = 0;

void ParseDirList(char *path);
void ParseDirFolder(char *path);
void ParseDir(char *path);

unsigned char *GetBuffer(char *filename, char *folderpath, int *outLen, int *outCount)
{
	char *filenameFull = (char*)calloc(MAX_PATH,sizeof(char));
	unsigned char *buffer = NULL;
	int len= 0, count = 0;
	FILE *outfile = NULL;
	
	strcpy(filenameFull,folderpath);
	strcat(filenameFull,"\\");
	strcat(filenameFull,filename);
	
	outfile = fopen(filenameFull,"rb");
	free(filenameFull);
	
	if(!outfile)
	{
		printf("Could not open %s\n",filename);
		exit(-1);
		return NULL;
	}
	
	fseek(outfile,0,SEEK_END);
	len = ftell(outfile)-4;
	rewind(outfile);
	fread(&count,1,4,outfile);
	
	buffer = (unsigned char*)calloc(len,sizeof(unsigned char));
	fread(buffer,1,len,outfile);
	fclose(outfile);
	
	*outLen = len;
	*outCount = count;
	
	return buffer;
}

void ParseDir(char *path)
{
	char *temp = (char*)calloc(strlen(path)+3,sizeof(char));
	FILE *list = NULL;
	
	strcpy(temp,path);
	strcat(temp,"\\list.dat");

	list = fopen(temp, "rb");
	
	if(!list)
	{
		ParseDirFolder(path);
	}
	else
	{
		fclose(list);
		ParseDirList(temp);
	}
	
	free(temp);
}

void ParseDirList(char *path)
{
	FILE *list = fopen(path,"rb");
	int entries = 0, i = 0, ignore = 0;
	
	fread(&entries, 1, 4, list);
	
	for(i = 0; i < entries; i++)
	{
		wchar_t *tempname = NULL;
		char *mbsfilename = NULL;
		int size = 0, len = 0, mbslen = 0;
		
		fread(&size, 1, 4, list);
		tempname = (wchar_t*)calloc(size, sizeof(wchar_t));
		fread(tempname, 1, size, list);
			
		if(fileinfo==NULL)
			fileinfo = (FileEntry*)calloc(filecount+1,sizeof(FileEntry));
		else
			fileinfo = (FileEntry*)realloc(fileinfo,(filecount+1)*sizeof(FileEntry));
				
		len = wcslen(tempname);
		
		if(filenamebuff==NULL)
			filenamebuff = (wchar_t*)calloc(len+1,sizeof(wchar_t));
		else
			filenamebuff = (wchar_t*)realloc(filenamebuff,(wcslen(filenamebuff)+len+1)*sizeof(wchar_t));
	
		wprintf(L"%s\n",tempname);
		len -= 3;
		wcsncat(filenamebuff,tempname,len);
			
		mbslen = wcstombs(NULL, tempname, 0);
		mbsfilename = (char*)calloc(mbslen + 1, sizeof(char));
		wcstombs(mbsfilename, tempname, mbslen);
			
		if(filecount>0)
			fileinfo[filecount].nameoffset = fileinfo[filecount-1].nameoffset+fileinfo[filecount-1].namesize;
		fileinfo[filecount].namesize = mbslen - 3;

		fileinfo[filecount].name = (char*)calloc(mbslen + 1,sizeof(char));
		strncpy(fileinfo[filecount++].name,mbsfilename, mbslen);
		
		free(tempname);
		free(mbsfilename);
	}
	
	fclose(list);
}

void ParseDirFolder(char *path)
{
    WIN32_FIND_DATA fd;
    HANDLE handle;
	char *temp = (char*)calloc(strlen(path)+3,sizeof(char));
	
	strcpy(temp,path);
	strcat(temp,"\\*\0");
        
    handle = FindFirstFile(temp,&fd);
    do 
    {
        if(strcmp(fd.cFileName,".") != 0 && strcmp(fd.cFileName,"..") != 0 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName,"vars.dat")!=0 && strcmp(fd.cFileName,"vars1.dat")!=0 && strcmp(fd.cFileName,"vars2.dat")!=0 && strcmp(fd.cFileName,"vars3.dat")!=0)
        {
			wchar_t *tempname = NULL;
			int len = mbstowcs(NULL,fd.cFileName,0);
			
			if(fileinfo==NULL)
				fileinfo = (FileEntry*)calloc(filecount+1,sizeof(FileEntry));
			else
				fileinfo = (FileEntry*)realloc(fileinfo,(filecount+1)*sizeof(FileEntry));
				
			if(filenamebuff==NULL)
				filenamebuff = (wchar_t*)calloc(len+1,sizeof(wchar_t));
			else
				filenamebuff = (wchar_t*)realloc(filenamebuff,(wcslen(filenamebuff)+len+1)*sizeof(wchar_t));
				
			tempname = (wchar_t*)calloc(len+1,sizeof(wchar_t));
			mbstowcs(tempname,fd.cFileName,len);
			
			wprintf(L"%s\n",tempname);
			len -= 3;
			wcsncat(filenamebuff,tempname,len);
			
			if(filecount>0)
				fileinfo[filecount].nameoffset = fileinfo[filecount-1].nameoffset+fileinfo[filecount-1].namesize;
			fileinfo[filecount].namesize = len;
			
			fileinfo[filecount].name = (char*)calloc(strlen(fd.cFileName)+1,sizeof(char));
			strcpy(fileinfo[filecount++].name,fd.cFileName);
			
			free(tempname);
        }
    } while(FindNextFile(handle,&fd) != 0);
}


void ParseFiles(FILE *outfile, char *folder, int compression)
{
	int i=0, x=0;
	
	for(i=0; i<filecount; i++)
	{
		FILE *infile = NULL;
		unsigned char *buffer = NULL, *output = NULL;
		char *path = (char*)calloc(MAX_PATH+1,sizeof(char));
		int complen = 0, size = 0;
		
		memset(path,'\0',MAX_PATH+1);
		
		strcpy(path,folder);
		strcat(path,"\\");
		strcat(path,fileinfo[i].name);
		
		infile = fopen(path,"rb");
		fseek(infile,0,SEEK_END);
		size = ftell(infile);
		rewind(infile);
		
		buffer = (unsigned char*)calloc(size, sizeof(unsigned char));
		fread(buffer,1,size,infile);
		
		output = CompressData(buffer,size,&fileinfo[i].size, compression);
		
		if(i>0)
			fileinfo[i].offset = fileinfo[i-1].offset+fileinfo[i-1].size;
		
		printf("%-52s %08x %08x %08x\n",path,fileinfo[i].offset,fileinfo[i].size,size);
		
		for(x=0; x<fileinfo[i].size; x++)
			output[x] ^= scrkey[x&0xff];
			
		if(pckhead.extraencrypt == 1)
		{
			for(x=0; x<fileinfo[i].size; x++)
				output[x] ^= key2[x&0x0f];
		}
		
		//memcpy(outbuffer+buffsize,output,fileinfo[i].size);
		fwrite(output, 1, fileinfo[i].size, outfile);
		
		free(path);
		
		if(buffer == output)
		{
			free(buffer);
			output = NULL;
		}
		else
		{
			
			if(buffer != NULL)
				free(buffer);
		
			if(output != NULL)
				free(output);
		}
			
		fclose(infile);
	}
}

int main(int argc, char **argv)
{
	FILE *outfile = NULL;
	unsigned char *vars = NULL, *vars1 = NULL, *vars2 = NULL, *vars3 = NULL;
	int varsLen = 0, vars1Len = 0, vars2Len = 0, vars3Len = 0;
	int varsCount = 0, vars1Count = 0, vars2Count = 0, vars3Count = 0;
	int i = 0, x = 0, compression = 0;
	
	setlocale(LC_ALL,"Japanese");
	
	if(argc<3)
	{
		printf("usage: %s infolder outpck [-c/-e]\n\nFlags:\n-c = Compression mode. A little slow but all files will be compressed.\n-e = Extra encryption (required for scripts)",argv[0]);
		return 0;
	}
	
	pckhead.extraencrypt = 0;
	for(i = 3; i < argc; i++)
	{
		if(strcmp(argv[i],"-c") == 0)
			compression = 1;
		else if(strcmp(argv[i],"-e") == 0)
			pckhead.extraencrypt = 1;
	}
	
	ParseDir(argv[1]);
	vars = GetBuffer("vars.dat", argv[1], &varsLen, &varsCount);
	vars1 = GetBuffer("vars1.dat", argv[1], &vars1Len, &vars1Count);
	vars2 = GetBuffer("vars2.dat", argv[1], &vars2Len, &vars2Count);
	vars3 = GetBuffer("vars3.dat", argv[1], &vars3Len, &vars3Count);
	
	pckhead.headersize = 0x5c;
	pckhead.table1.size = pckhead.globalvartable.size = pckhead.globalvartablestr.size = varsCount;
	(int)pckhead.table1.buffer = 0x5c;
	(int)pckhead.globalvartable.buffer = (int)pckhead.table1.buffer+varsCount*8;
	(int)pckhead.globalvartablestr.buffer = (int)pckhead.globalvartable.buffer+varsCount*8;
	(int)pckhead.filenametable1.buffer = varsLen+pckhead.headersize;
	(int)pckhead.filenametable1.size = vars1Count;
	(int)pckhead.filenametable2.buffer = pckhead.filenametable1.buffer+vars1Len;
	(int)pckhead.filenametable2.size = vars2Count;
	(int)pckhead.filenametable3.buffer = pckhead.filenametable2.buffer+vars2Len;
	(int)pckhead.filenametable3.size = vars3Count;
	(int)pckhead.filenametable4.buffer = pckhead.filenametable3.buffer+vars3Len;
	pckhead.filenametable4.size = pckhead.filenamestr.size = pckhead.filetable.size = pckhead.filestart.size = filecount;
	(int)pckhead.filenamestr.buffer = (int)pckhead.filenametable4.buffer+pckhead.filenametable4.size*8;
	(int)pckhead.filetable.buffer = (int)pckhead.filenamestr.buffer+(wcslen(filenamebuff)*2);
	(int)pckhead.filestart.buffer = (int)pckhead.filetable.buffer+pckhead.filetable.size*8;
	pckhead.unk2 = 0x366;
	
	outfile = fopen(argv[2],"wb+");
	
	// quick hackish way to do this without having to duplicate code or do a bunch of stuff
	// on the first pass, write all of the file information so the space will be there and
	// then write the file data (compressed or otherwise) and count the totals
	// on the second pass only rewrite the file information which will this time be correct
	
	for(x = 0; x < 2; x++)
	{
		fwrite(&pckhead,1,sizeof(pckhead),outfile);
		fwrite(vars,1,varsLen,outfile);
		fwrite(vars1,1,vars1Len,outfile);
		fwrite(vars2,1,vars2Len,outfile);
		fwrite(vars3,1,vars3Len,outfile);
		//fwrite(&fileinfo,sizeof(FileEntry),filecount,outfile);
	
		for(i=0; i<filecount; i++)
		{
			fwrite(&fileinfo[i].nameoffset,1,4,outfile);
			fwrite(&fileinfo[i].namesize,1,4,outfile);
		}
		
		fwrite(filenamebuff,2,wcslen(filenamebuff),outfile);
		
		for(i=0; i<filecount; i++)
		{
			fwrite(&fileinfo[i].offset,1,4,outfile);
			fwrite(&fileinfo[i].size,1,4,outfile);
		}
	
		if(x == 0)
		{
			ParseFiles(outfile,argv[1],compression);
			
			fclose(outfile);
			outfile = fopen(argv[2],"rb+");
		}
	}
	
	fclose(outfile);
	
	return 0;
}
