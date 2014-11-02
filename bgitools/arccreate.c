#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

FILE *infile = NULL;
FILE *outfile = NULL;

#pragma pack(1)
struct
{
    char sig[0x0c];
    int fileCount;
} Head;

struct 
{
    char filename[0x10];
    int offset;
    int fileSize;
    long long null;
} *Entry;

void parsedir(char *path, char *path2, int mode)
{
    WIN32_FIND_DATA fd;
    HANDLE handle;
        
    handle = FindFirstFile(path,&fd);
    do 
    {
        if(strcmp(fd.cFileName,".") != 0 && strcmp(fd.cFileName,"..") != 0 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) /* Skip cur dir, prev dir, and any hidden files */
        {
            if(mode==0)
            {
                Head.fileCount++;
            }
            else if(mode==1)
            {
                if(strlen(fd.cFileName)>0x10)
                    printf("The filename \"%s\" is over the limit, shortening name...\n");
                    
                strncpy(Entry[Head.fileCount].filename,fd.cFileName,0x0f);
                
                chdir(path2);
                infile = fopen(fd.cFileName,"rb");
                
                if(!infile)
                {
                    printf("Something went terribly wrong\n");
                    exit(1);
                }
                
                fseek(infile,0,SEEK_END);
                Entry[Head.fileCount].fileSize = ftell(infile);
                fclose(infile);
                chdir("..");
                    
                if(Head.fileCount==0)
                    Entry[Head.fileCount].offset = 0;
                else
                    Entry[Head.fileCount].offset = Entry[Head.fileCount-1].offset+Entry[Head.fileCount-1].fileSize;
                    
                Head.fileCount++;
            }
        }
    } while(FindNextFile(handle,&fd) != 0);
}

void writearc(char *path)
{
    int i=0;
    
    strncpy(Head.sig,"PackFile    ",0x0c);
    fwrite(&Head,1,sizeof(Head),outfile);
    
    for(i=0; i<Head.fileCount; i++)
        fwrite(&Entry[i],1,sizeof(*Entry),outfile);
    
    for(i=0; i<Head.fileCount; i++)
    {
        char *buffer = (char*)calloc(Entry[i].fileSize,sizeof(char));
        
        printf("[+] %-16s offset[%08x] size[%08x]\n",Entry[i].filename,Entry[i].offset,Entry[i].fileSize);
        
        chdir(path);
        infile = fopen(Entry[i].filename,"rb");
        
        fread(buffer,1,Entry[i].fileSize,infile);
        fwrite(buffer,1,Entry[i].fileSize,outfile);
        
        fclose(infile);
        chdir("..");
    }
}

int main(int argc, char **argv)
{
    char *temp = NULL;

    if(argc!=3)
    {
        printf("usage: %s inputFolder outputArc\n",argv[0]);
        return 0;
    }
    
    temp = (char*)calloc(strlen(argv[1])+3,sizeof(char));
    strcpy(temp,argv[1]);
    strcat(temp,"\\*");
    
    parsedir(temp,argv[1],0);
    Entry = calloc(Head.fileCount+1,sizeof(*Entry));
    Head.fileCount = 0;
    parsedir(temp,argv[1],1);
    
    outfile = fopen(argv[2],"wb");
    writearc(argv[1]);
    fclose(outfile);

    return 0;
}