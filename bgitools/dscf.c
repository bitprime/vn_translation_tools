#include <stdio.h>
#include <stdlib.h>

unsigned int GenKey(unsigned int key)
{
  unsigned int result; // eax@1

  result = ((20021 * key >> 16)
          + 20021 * key + 2)
          + 346 * key) & 0x7FFF;
		  
  return result;
}


int main(int argc, char **argv)
{
	FILE *infile = fopen(argv[1],"rb");
	FILE *outfile = fopen(argv[2],"wb");
	
	unsigned char *buffer = NULL;
	unsigned char *data = NULL;
	int len = 0;
	unsigned int key = 0;
	int i = 0;
	
	fseek(infile,0,SEEK_END);
	len = ftell(infile);
	rewind(infile);
	
	buffer = (unsigned char*)calloc(len,sizeof(unsigned char));
	
	fread(buffer,1,len,infile);
	
	key = *(unsigned int*)(buffer+0x10);
	data = buffer+0x20;
	
	for(i = 0; i < 512; i++)
	{
		key = GenKey(key);
		data[i] -= key;
	}
		
	fwrite(buffer,1,len,outfile);	
	
	fclose(infile);
	fclose(outfile);
	
	free(buffer);
}
