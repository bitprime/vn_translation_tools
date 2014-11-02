#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *scr = fopen(argv[1],"rb");
	FILE *infile = fopen(argv[2],"rb");
	FILE *outfile = fopen(argv[3],"wb");
	char *buffer = NULL;
	int size = 0, pos = atoi(argv[4]);
	
	buffer = (char*)calloc(pos,sizeof(char));
	
	fread(buffer,1,pos,scr);
	fwrite(buffer,1,pos,outfile);
	
	free(buffer);
	fclose(scr);
	
	fseek(infile,0,SEEK_END);
	size = ftell(infile);
	rewind(infile);
	
	buffer = (char*)calloc(size,sizeof(char));
	fread(buffer,1,size,infile);
	fwrite(buffer,1,size,outfile);
	
	fclose(infile);
	fclose(outfile);
	free(buffer);
}
