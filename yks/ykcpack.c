/*
    YKC archive packer - for YukaScript engine
    
    format:
    
    [header]
        char *magic "YKC001\0\0"
        int headerSize
        int unk1 ; always null?
        int tableOffset
        int tableSize
        
    [table]
        int ptrToFilename
        int filenameLength (includes null space)
        int ptrToFile
        int fileSize
        int unk ; null?
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAGIC "YKC001\0\0"

FILE *outfile = NULL;
FILE *updateFile = NULL;

#pragma pack(1)
struct
{
    int headerSize;
    int unk1;
    int tableOffset;
    int tableSize;
} YKCHeader;

struct
{
    char *filename;
    int ptrToFilename;
    int filenameLength;
    int ptrToFile;
    int fileSize;
    int unk;
    char *buffer;
} *YKCEntry;

int entries = 0;
char *basePath = NULL;
char *updateArc = NULL;

// dir parse func from nipa
void addEntry(char *path, char *filename)
{
    FILE *tempFile = NULL;
    char *tempPath = (char*)calloc(MAX_PATH,sizeof(char));
    int i=0,x=0;
    YKCEntry = realloc(YKCEntry,(entries+1)*sizeof(*YKCEntry));
    
    YKCEntry[entries].filename = (char*)calloc(MAX_PATH,sizeof(char));
    strncpy(YKCEntry[entries].filename,path,strlen(path)-1);
    strncat(YKCEntry[entries].filename,filename,strlen(filename));
    
    strcat(tempPath,YKCEntry[entries].filename);
    
    for(i=strlen(basePath)-1; i<strlen(YKCEntry[entries].filename); i++,x++)
    {
        YKCEntry[entries].filename[x] = YKCEntry[entries].filename[i];
    }
    YKCEntry[entries].filename[x] = '\0';
    
    tempFile = fopen(tempPath,"rb");
    
    if(!tempFile)
    {
        printf("Could not open %s\n",tempPath);
        exit(1);
    }
    
    fprintf(updateFile,"I   %s  %s\n",updateArc,YKCEntry[entries].filename);
    fseek(tempFile,0,SEEK_END);
    
    YKCEntry[entries].filenameLength = strlen(YKCEntry[entries].filename)+1;
    YKCEntry[entries].fileSize = (int)ftell(tempFile);
    YKCEntry[entries].buffer = (char*)calloc(ftell(tempFile),sizeof(char));
    
    if(entries>0)
        YKCEntry[entries].ptrToFile = YKCEntry[entries-1].ptrToFile+YKCEntry[entries-1].fileSize;
    else
        YKCEntry[entries].ptrToFile = YKCHeader.headerSize;
    
    fseek(tempFile,0,SEEK_SET);
    fread(YKCEntry[entries].buffer,1,YKCEntry[entries].fileSize,tempFile);
    YKCHeader.tableOffset += strlen(YKCEntry[entries].filename)+1;
        
    entries++;
    free(tempPath);
    fclose(tempFile);
}
 
void parseDir(char *path)
{
	WIN32_FIND_DATA fd;
	HANDLE handle;
		
	handle = FindFirstFile(path,&fd);
	do 
	{
		if(strcmp(fd.cFileName,".") != 0 && strcmp(fd.cFileName,"..") != 0 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
		{				
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char *name = (char*)calloc(MAX_PATH,sizeof(char));
				 
                strncpy(name,path,strlen(path)-1);
                strcat(name,fd.cFileName);
				strcat(name,"\\*");
				
				parseDir(name);
                
				free(name);
			}
            else
            { 
                addEntry(path,fd.cFileName);
            }
		}
	} while(FindNextFile(handle,&fd) != 0);
}

void createArchive()
{
    fwrite(MAGIC,1,8,outfile);
    YKCHeader.tableSize = entries*0x14;
    YKCHeader.tableOffset += YKCEntry[entries-1].ptrToFile+YKCEntry[entries-1].fileSize;
    
    fwrite(&YKCHeader,1,sizeof(YKCHeader),outfile);
}

void writeArchive()
{
    int i = 0;
    
    for(i=0; i<entries; i++)
    {
        fwrite(YKCEntry[i].buffer,1,YKCEntry[i].fileSize,outfile);
    }
    
    for(i=0; i<entries; i++)
    {
        YKCEntry[i].ptrToFilename = ftell(outfile);
        fwrite(YKCEntry[i].filename,1,strlen(YKCEntry[i].filename)+1,outfile);
    }
    
    for(i=0; i<entries; i++)
    {
        int strLen = (int)strlen(YKCEntry[i].filename)+1;
        
        printf("[+] %-40s offset[%08x] size[%08x]\n",YKCEntry[i].filename,YKCEntry[i].ptrToFile,YKCEntry[i].fileSize);
        
        fwrite(&YKCEntry[i].ptrToFilename,1,4,outfile);
        fwrite(&strLen,1,4,outfile);
        fwrite(&YKCEntry[i].ptrToFile,1,4,outfile);
        fwrite(&YKCEntry[i].fileSize,1,4,outfile);
        fwrite("\0\0\0\0",1,4,outfile);
    }
}

int main(int argc, char **argv)
{
    char *tempPath = NULL;

    if(argc != 4)
    {
        printf("usage: %s outfile infolder fileToUpdate\n",argv[0]);
        return 1;
    }
    
    updateFile = fopen("Update.lst","w");
    basePath = (char*)calloc(strlen(argv[2])+4,sizeof(char));
    tempPath = (char*)calloc(strlen(argv[2])+4,sizeof(char));
    updateArc = (char*)calloc(strlen(argv[3])+1,sizeof(char));
    strcpy(updateArc,argv[3]);
    strcpy(tempPath,argv[2]);
    strcat(tempPath,"\\*\0");
    strcpy(basePath,tempPath);
    
    fprintf(updateFile,"P   %s\n",argv[1]);

    outfile = fopen(argv[1],"wb");
    YKCHeader.headerSize = 0x18;
    
    parseDir(tempPath);
    createArchive();
    writeArchive();
    
    return 0;
}
