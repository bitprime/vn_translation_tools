#include <stdio.h>
#include <stdlib.h>

FILE *infile = NULL;
int n=0;

void ReadByte(int count)
{
    int i=0;
    char x=0;
    
    for(i=0; i<count; i++,n++)
    {
        fread(&x,1,1,infile);
        printf("%d: %02x\n",n,x);
    }
}

void ReadShort(int count)
{
    int i=0;
    short x=0;
    
    for(i=0; i<count; i++,n++)
    {
        fread(&x,1,2,infile);
        printf("%d: %04x\n",n,x);
    }
}

void ReadInt(int count)
{
    int i=0, x=0;
    
    for(i=0; i<count; i++,n++)
    {
        fread(&x,1,4,infile);
        printf("%d: %08x\n",n,x);
    }
}

void ReadString(int count)
{
    int i=0,len=0;
    
    for(i=0; i<count; i++,n++)
    {
        char *str = NULL;
        
        fread(&len,1,1,infile);
        
        if(len>0)
        {
            str = (char*)calloc(len+1,sizeof(char));
            fread(str,1,len,infile);
        }
        
        printf("%d: %s\n",n,str);
            
        if(str)
            free(str);
    }
}

void DeserializeICI(char *filename)
{
    int s=0,entries=0,i=0;
    
    infile = fopen(filename,"rb");
    
    fseek(infile,0x1d,SEEK_SET); // skip data which brings you to the CObjectArcMan class
    
    // part 1, header
    fread(&s,1,4,infile);
    printf("%d: %08x\n",n++,s);
    
    ReadInt(1);
    ReadByte(2);
    ReadInt(3);
    
    if(s>=6)
        ReadInt(3);
    
    if(s>=8)
        ReadInt(1);
    
    ReadString(1);
    ReadInt(1);
    ReadString(1);
    ReadInt(1);
    ReadString(3);
    ReadInt(1);
    fseek(infile,2,SEEK_CUR);
    ReadString(1);
    ReadInt(1);
	exit(1);
    
    if(s>=9)
        ReadString(1);
        
    if(s>=7)
        ReadString(1);
    
    if(s>=5)
        ReadInt(1);
        
    // part 2, per-game
    fread(&entries,1,2,infile);
    printf("Game entries: %d\n",entries);
    n++;
    
    for(i=0; i<entries; i++)
    {
        int buffer=0,bshift=0;
    
        ReadByte(1);
        fread(&s,1,2,infile);
        ReadString(5);
        
        ReadInt(2);
        ReadInt(2);
        
        if(s<6)
            ReadInt(1);
            
        ReadInt(1);
        ReadString(1);
        
        ReadInt(2);
        fread(&buffer,1,4,infile);
        fread(&bshift,1,4,infile);
        
        ReadInt(1);
        ReadString(1);
        ReadInt(5);
        ReadString(1);
    
    
        buffer = (((buffer+0xffff)>>bshift>>0x10)+7)>>3; // size of the amount of check bytes
        fread(&bshift,1,2,infile);
        bshift *= 4; // some stuff that gets read into an array but i'm not sure what it's used for
        fseek(infile,buffer+bshift,SEEK_CUR); // skip to next entry
    }
    
    fclose(infile);
}

void main(int argc, char **argv)
{
    DeserializeICI(argv[1]);
}
