#include <stdio.h>
#include <stdlib.h>

FILE *infile = NULL;

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

void extractData(char *filename, int offset, int size)
{
    char *buffer = (char*)calloc(size,sizeof(char));
    
    FILE *outfile = fopen(filename,"wb");
    
    printf("[-] %-16s offset[%08x] size[%08x]\n",filename,offset,size);
    
    fseek(infile,offset+sizeof(Head)+sizeof(*Entry)*Head.fileCount,SEEK_SET);
    fread(buffer,1,size,infile);
    fwrite(buffer,1,size,outfile);
    
    free(buffer);
    fclose(outfile);
}

void parseArc()
{
    int i=0;
    
    fread(&Head,1,sizeof(Head),infile);
    
    Entry = calloc(Head.fileCount+1,sizeof(*Entry));
    
    fread(Entry,1,sizeof(*Entry)*Head.fileCount,infile);
        
    for(i=0; i<Head.fileCount; i++)
        extractData(Entry[i].filename,Entry[i].offset,Entry[i].fileSize);
}

int main(int argc, char **argv)
{
    if(argc!=3)
    {
        printf("usage: %s.exe inputArc outputFolder\n",argv[0]);
        return 0;
    }
    
    infile = fopen(argv[1],"rb");
    if(!infile)
    {
        printf("Could not open %s\n",argv[1]);
        return -1;
    }
    
    mkdir(argv[2]);
    chdir(argv[2]);
    
    parseArc();
    
    chdir("..");
    
    fclose(infile);
    
    return 0;
}
