#include <stdio.h>
#include <stdlib.h>

#pragma pack(1)
struct
{
    char magic[8];
    int headerSize;
    int unk1;
    int tableOffset;
    int tableSize;
} YKCHeader;

struct
{
    int ptrToFilename;
    int filenameLength;
    int ptrToFile;
    int fileSize;
    int unk;
    char *filename;
} *YKCEntry;

void createDirectories(char *string)
{
    char *temp = (char*)calloc(512,sizeof(char));
    int i=0,x=0,depth=0;
    
    for(i=0,x=0; i<strlen(string); i++)
    {
        temp[x++] = string[i];
        temp[x+1] = '\0';
        
        if(string[i] == '\\')
        {
            mkdir(temp);
            chdir(temp);
            x=0;
            depth++;
        }
    }
    
    for(i=0; i<depth; i++)
        chdir("..");
    
    free(temp);
}

int main(int argc, char **argv)
{
    FILE *infile = NULL;
    int i=0;
    
    if(argc!=2)
    {
        printf("usage; %s infile\n",argv[0]);
        exit(1);
    }
    
    infile = fopen(argv[1],"rb");
    
    if(!infile)
    {
        printf("Could not open %s\n",argv[1]);
        exit(1);
    }
    
    fread(&YKCHeader,1,sizeof(YKCHeader),infile);
    
    fseek(infile,YKCHeader.tableOffset,SEEK_SET);
    YKCEntry = calloc(YKCHeader.tableSize/5,sizeof(YKCEntry[0]));
    for(i=0; i<YKCHeader.tableSize/20; i++)
    {
        long curPos = 0;
        FILE *outfile = NULL;
        char *buffer = NULL;
        
        fread(&YKCEntry[i],1,20,infile);
        
        YKCEntry[i].filename = (char*)calloc(YKCEntry[i].filenameLength+1,sizeof(char));
        
        curPos = ftell(infile);
        fseek(infile,YKCEntry[i].ptrToFilename,SEEK_SET);
        fread(YKCEntry[i].filename,1,YKCEntry[i].filenameLength,infile);
        
        createDirectories(YKCEntry[i].filename);
        outfile = fopen(YKCEntry[i].filename,"wb");
        
        if(!outfile)
        {
            printf("Could not open %s for writing\n",YKCEntry[i].filename);
            exit(1);
        }
        
        buffer = (char*)calloc(YKCEntry[i].fileSize,sizeof(char));
        fseek(infile,YKCEntry[i].ptrToFile,SEEK_SET);
        fread(buffer,1,YKCEntry[i].fileSize,infile);
        fwrite(buffer,1,YKCEntry[i].fileSize,outfile);
        
        fseek(infile,curPos,SEEK_SET);
        
        printf("[+] %-40s offset[%08x] size[%08x]\n",YKCEntry[i].filename,YKCEntry[i].ptrToFile,YKCEntry[i].fileSize);
        
        free(buffer);
        fclose(outfile);
    }

    return 0;
}