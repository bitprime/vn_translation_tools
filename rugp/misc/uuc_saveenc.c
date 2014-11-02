/*
	rugp save decryption
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = NULL,  *outfile = NULL;
	unsigned char *buffer = NULL;
	unsigned int size1=0, size2=0, size=0, outsize=0;
	int decAmt=0x20, i=0, x=0;
	unsigned int key=0x0732CE9A;

	if(argc!=3)
	{
		printf("usage: %s infile outfile\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"rb");
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	outfile = fopen(argv[2],"wb");
	
	fseek(infile,0,SEEK_END);
	size = size1 = size2 = ftell(infile);
	rewind(infile);

	size1 = (~size1)^0xC92E568B;
	size2 = (size2<<3)^0xC92E568F;

	buffer = (unsigned char*)calloc(size*2,sizeof(unsigned char));

	for(i=size; i>0;)
	{	
		unsigned short hash = 0;
		decAmt = 0x20;
		
		if(i-decAmt<0)
			decAmt = i;
		
		i -= decAmt;

		fread(buffer+x,1,decAmt,infile);

		outsize += decAmt+2;
		for(; decAmt>0; decAmt--, x++)
		{
			hash += buffer[x]*decAmt;
			buffer[x] ^= (key&0xff);
			key = ~(key*2+((key>>0x0f)&1)+0xA3B376C9); 
		}
		
		*(unsigned short*)(buffer+x) = hash;
		hash=0, x+=2;
	}
	
	fwrite(&size1,1,4,outfile);
	fwrite(&size2,1,4,outfile);
	fwrite(buffer,1,outsize,outfile);
	free(buffer);
	
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
