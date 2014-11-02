#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = NULL, *outfile = NULL;
	char *buffer = NULL;
	int filesize = 0, i = 0, offset = 0, size = 0;
	
	if(argc != 2)
	{
		printf("usage: %s infile.yks\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"rb");
	
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return 0;
	}
	
	fseek(infile, 0x20, SEEK_SET);
	fread(&offset, 1, 4, infile);
	fread(&size, 1, 4, infile);
	
	fseek(infile,0,SEEK_END);
	filesize = ftell(infile);
	rewind(infile);

	buffer = (char*)calloc(filesize,sizeof(char));
	fread(buffer, 1, filesize, infile);
	
	for(i = offset; i - offset < size; i++)
	{
		buffer[i] ^= 0xaa;
	}
	
	fclose(infile);
	
	outfile = fopen(argv[1],"wb");
	fwrite(buffer,1,filesize,outfile);
	fclose(outfile);
	
	free(buffer);

	return 0;
}
