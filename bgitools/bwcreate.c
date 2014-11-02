/*
    Create audio for BGI engine from OGG files
    
    bw format:

    dword - bw size
    char[4] - "bw  "
    dword - original ogg size
    dword - song length (see details below)
    dword - unk1 (see details below)
    dword - unk2 (see details below)
    dword - unk3 (see details below)
    dword - unk (can be null?)
    long - null
    long - null
    long - 3
    long - null
    
    unk1 = 0x28 bytes in
    unk2 = 0x24 bytes in
    unk3 = 0x4c bytes in (?)
*/

#include <stdio.h>
#include <stdlib.h>

#pragma pack(1)
struct
{
    int bwSize;
    char sig[4];
    int oggSize;
    int songLength;
    int unk1;
    int unk2;
    int unk3;
    int unk4;
    long long null0;
    long long null01;
    long null3;
    long null02;
    long long null03;
} BWHead;

FILE *infile = NULL;
char *buffer = NULL;

void FillHeader()
{
    char head[5];
    int i=0;
    
    fread(head,1,5,infile);
    
    if(strncmp(head,"OggS",4)!=0)
    {
        printf("Not a valid Ogg file\n");
        
        fclose(infile);
        
        exit(1);
    }
    
    fread(&BWHead.unk2,1,4,infile);
    fseek(infile,0x28,SEEK_SET);
    fread(&BWHead.unk1,1,4,infile);
    fseek(infile,0x4c,SEEK_SET);
    fread(&BWHead.unk3,1,4,infile);
    
    fseek(infile,0,SEEK_END);
    BWHead.oggSize = ftell(infile);
    rewind(infile);
    
    buffer = (char*)calloc(BWHead.oggSize,sizeof(char));
    
    fread(buffer,1,BWHead.oggSize,infile);
    fclose(infile);
    
    // find OggS\x00\x05 for songLength
    for(i=BWHead.oggSize-7; i<BWHead.oggSize && i>0; i-=1)
    {
        if(memcmp(buffer+i,"OggS\x00\x05",6)==0)
        {
            i+=6;
            memcpy(&BWHead.songLength,buffer+i,4);
            break;
        }
    }
    
    if(i==0)
    {
        printf("Could not find required information to convert this Ogg into a BGI Ogg\n");
        
        free(buffer);
        fclose(infile);
        
        exit(1);
    }
}

void CreateFile(char *outputName)
{
    FILE *outfile = fopen(outputName,"wb");
    
    fwrite(&BWHead,1,sizeof(BWHead),outfile);
    fwrite(buffer,1,BWHead.oggSize,outfile);
    
    free(buffer);
    fclose(outfile);
}

int main(int argc, char **argv)
{
    if(argc!=3)
    {
        printf("usage: %s inputOgg outputBGIOgg\n",argv[0]);
        return 0;
    }

    infile = fopen(argv[1],"rb");
    
    if(!infile)
    {
        printf("Could not open %s\n",argv[1]);
        return -1;
    }
    
    // initialize struct
    BWHead.bwSize = 0x40;
    strcpy(BWHead.sig,"bw  ");
    BWHead.unk4 = BWHead.null0 = BWHead.null01 = BWHead.null02 = BWHead.null03 = 0;
    BWHead.null3 = 3;
    
    FillHeader();
    CreateFile(argv[2]);
    
    fclose(infile);
    
    return 0;
}
