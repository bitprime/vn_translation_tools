#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = fopen(argv[1],"rb");
	FILE *outfile = fopen(argv[2],"wb");
	char *buffer = NULL;
	int i = 0, size = 0, pos = 0; //atoi(argv[3]);
	
	fseek(infile,0,SEEK_END);
	size = ftell(infile);
	//rewind(infile);
	fseek(infile,0x54, SEEK_SET);
	fread(&pos,1,4,infile);
	fseek(infile,0x30,SEEK_SET);
	
	buffer = (char*)calloc(size-pos,sizeof(char));
	fseek(infile,pos,SEEK_CUR);
	fread(buffer,1,size-pos,infile);
	
	for(i=0; i<size-pos; i++)
	{
		if(buffer[i]=='\0')
			buffer[i] = '\n';
	}
	
	fwrite(buffer,1,size-pos,outfile);
	fclose(infile);
	fclose(outfile);	
}
