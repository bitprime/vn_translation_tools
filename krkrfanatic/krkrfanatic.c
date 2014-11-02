#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = NULL;
	FILE *outfile = NULL;
	unsigned char *buffer = NULL;
	int buffLen = 0, buffOffset = 0, i = 0;
	
	if(argc != 4 || (argc == 4 && (strcmp(argv[1],"-e")!=0 && strcmp(argv[1],"-d")!=0)))
	{
		printf("usage: %s -d/-e infile outfile\n\t-d = decrypt mode\n\t-e = encrypt mode",argv[0]);
		return 1;
	}
	
	// read data
	infile = fopen(argv[2],"rb");
	
	if(!infile)
	{
		printf("Could not open %s\n",argv[2]);
		return -1;
	}
	
	fseek(infile,0,SEEK_END);
	buffLen = ftell(infile);
	rewind(infile);
	
	buffer = (unsigned char*)calloc(buffLen,sizeof(unsigned char));
	fread(buffer,1,buffLen,infile);	
	fclose(infile);
	
	// decrypt/encrypt data
	if(strcmp(argv[1],"-d")==0)
	{
		buffOffset = 5;
	}
	else if(strcmp(argv[1],"-e")==0)
	{
		buffOffset = 2;
	}
	
	for(i = buffOffset; i < buffLen; i += 2)
	{
		unsigned int d = *(unsigned short*)(buffer+i);
		unsigned short d2 = d;
		
		d2 &= 0x5555;
		d &= 0xaaaaaaaa;
		d >>= 1;
		d2 += d2;
		d |= d2;
		
		*(unsigned short*)(buffer+i) = (unsigned short)d;
	}
	
	// write data
	outfile = fopen(argv[3],"wb");
	
	if(!outfile)
	{
		printf("Could not open %s\n",argv[3]);
		return -2;
	}
	
	if(strcmp(argv[1],"-e")==0)
	{
		int d = 0x01fefe;
		fwrite(&d, 1, 3, outfile);
		
		buffOffset = 0;
	}
	else if(strcmp(argv[1],"-d")==0)
	{
		buffOffset = 3;
		buffLen -= 3;
	}
	
	fwrite(buffer+buffOffset,1,buffLen,outfile);	
	fclose(outfile);
	
	free(buffer);

	return 0;
}
