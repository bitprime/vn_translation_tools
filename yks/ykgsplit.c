#include <stdio.h>
#include <stdlib.h>

#pragma pack(1)
struct
{
    char magic[8];
    int dataStart;
    char unk[0x1c];
    int file1Offset;
    int file1Size;
    int file2Offset;
    int file2Size;
    int file3Offset;
    int file3Size;
} Header;

int main(int argc, char **argv)
{
    FILE *infile = NULL;
    FILE *outfile = NULL;
    
    if(argc != 2)
    {
        printf("usage: %s infile\n",argv[0]);
        exit(1);
    }
    
    infile = fopen(argv[1],"rb");
    
    fread(&Header,1,sizeof(Header),infile);
    
    if(Header.file1Size!=0)
    {
        char *buffer = (char*)calloc(Header.file1Size,sizeof(char));
        char *outName = (char*)calloc(strlen(argv[1])+2,sizeof(char));
        
        strncpy(outName,argv[1],strlen(argv[1])-4);
        strcat(outName,"_1");
        
        fseek(infile,Header.file1Offset,SEEK_SET);
        fread(buffer,1,Header.file1Size,infile);
        
        if(buffer[1] == 'G' &&  buffer[2] == 'N' && buffer[3] == 'P')
        {
            buffer[1] = 'P';
            buffer[3] = 'G';
        }
        
        if(strncmp(buffer,"\x89PNG",4)==0)
            strcat(outName,".png\0");
        else if(strncmp(buffer,"BM",2)==0)
            strcat(outName,".bmp\0");
        else
            strcat(outName,".dat\0");
        
        outfile = fopen(outName,"wb");
        fwrite(buffer,1,Header.file1Size,outfile);
        
        free(buffer);
        fclose(outfile);
    }
    
    if(Header.file2Size!=0)
    {
        char *buffer = (char*)calloc(Header.file2Size,sizeof(char));
        char *outName = (char*)calloc(strlen(argv[1])+2,sizeof(char));
        
        strncpy(outName,argv[1],strlen(argv[1])-4);
        strcat(outName,"_2");
        
        fseek(infile,Header.file2Offset,SEEK_SET);
        fread(buffer,1,Header.file2Size,infile);
        
        if(buffer[1] == 'G' &&  buffer[2] == 'N' && buffer[3] == 'P')
        {
            buffer[1] = 'P';
            buffer[3] = 'G';
        }
        
        if(strncmp(buffer,"\x89PNG",4)==0)
            strcat(outName,".png\0");
        else if(strncmp(buffer,"BM",2)==0)
            strcat(outName,".bmp\0");
        else
            strcat(outName,".dat\0");
        
        outfile = fopen(outName,"wb");
        fwrite(buffer,1,Header.file2Size,outfile);
        
        free(buffer);
        fclose(outfile);
    }
    
    if(Header.file3Size!=0)
    {
        char *buffer = (char*)calloc(Header.file3Size,sizeof(char));
        char *outName = (char*)calloc(strlen(argv[1])+2,sizeof(char));
        
        strncpy(outName,argv[1],strlen(argv[1])-4);
        strcat(outName,"_3");
        
        fseek(infile,Header.file3Offset,SEEK_SET);
        fread(buffer,1,Header.file3Size,infile);
        
        if(buffer[1] == 'G' &&  buffer[2] == 'N' && buffer[3] == 'P')
        {
            buffer[1] = 'P';
            buffer[3] = 'G';
        }
        
        if(strncmp(buffer,"\x89PNG",4)==0)
            strcat(outName,".png\0");
        else if(strncmp(buffer,"BM",2)==0)
            strcat(outName,".bmp\0");
        else
            strcat(outName,".dat\0");
        
        outfile = fopen(outName,"wb");
        fwrite(buffer,1,Header.file3Size,outfile);
        
        free(buffer);
        fclose(outfile);
    }

    return 0;
}
