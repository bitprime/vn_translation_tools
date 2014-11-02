#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE *outfile = NULL;
    FILE *infile = NULL;
    int i=0;
    int offset = 0x40;
    
    if(argc!=3 && argc!=4 && argc!=5)
    {
        printf("usage: %s output.ykg input1 input2(optional) input3(optional)\n",argv[0]);
        exit(1);
    }
    
    outfile = fopen(argv[1],"wb");
    
    fwrite("YKG000\0\0",1,8,outfile);
    fwrite("\x40\0\0\0",1,4,outfile);
    
    for(i=0; i<0x1c/4; i++)
        fwrite("\0\0\0\0",1,4,outfile);
    
    for(i=0; i<argc-2; i++)
    {
        long fileLen = 0;

        infile = fopen(argv[i+2],"rb");
        
        if(!infile)
        {
            printf("Could not open %s\n",argv[i+2]);
            exit(1);
        }
        
        fseek(infile,0,SEEK_END);
        fileLen = ftell(infile);
        fclose(infile);
        
        fwrite(&offset,1,4,outfile);
        fwrite(&fileLen,1,4,outfile);
        
        offset += fileLen;
    }
    
    for(i=0; i<((argc-2)-3)*2; i++)
        fwrite("\0\0\0\0",1,4,outfile);
    
    for(i=0; i<argc-2; i++)
    {
        char *buffer = NULL;
        long fileLen = 0;
        
        infile = fopen(argv[i+2],"rb");
        fseek(infile,0,SEEK_END);
        fileLen = ftell(infile);
        rewind(infile);
        
        buffer = (char*)calloc(fileLen,sizeof(char));
        fread(buffer,1,fileLen,infile);
        fwrite(buffer,1,fileLen,outfile);
        
        offset += fileLen;
        
        free(buffer);
        fclose(infile);
    }
    
    return 0;
}
