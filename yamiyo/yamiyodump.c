/*
format:
key: 91a5f7c5

header:
dword - pkg filesize
dword - file count

entries:
0x80 bytes - filename
dword - filesize
dword - offset
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = NULL;
	
	static unsigned char keyb[4] = { 0xc5, 0xf7, 0xa5, 0x91 };
	//static unsigned int key = 0xc5f7a591;
	static unsigned int key = 0x91a5f7c5;
	int pkgSize = 0;
	int fileCount = 0;
	int i = 0;
	int curFile = 0;
	
	if(argc != 3)
	{
		printf("usage: %s inpkg outfolder\n",argv[0]);
		return 1;
	}
	
	infile = fopen(argv[1],"rb");
	
/*
	while(!feof(infile))
	{
		unsigned int c = 0;
		int len = 0;
		
		if((len = fread(&c,1,4,infile)) != 4)
		{
			if(len == 0)
				break;
		
			// not a full dword
			printf("Decrypting %d bytes (%08x)\n", len, c);
			c ^= key&(~(0xff<<(len*8)));
			fwrite(&c,1,len,outfile);
			
			break;
		}
		
		c ^= key;
		fwrite(&c,1,4,outfile);
	}
*/

	mkdir(argv[2]);
	chdir(argv[2]);

	fread(&pkgSize,1,4,infile);
	fread(&fileCount,1,4,infile);
	
	pkgSize ^= key;
	fileCount ^= key;
	
	for(i = 0; i < fileCount; i++)
	{
		FILE *outfile = NULL;
		unsigned char *filename = (unsigned char*)calloc(0x80,sizeof(unsigned char));
		unsigned char *data = NULL;
		int dataLen = 0;
		int dataOffset = 0;
		int x = 0;
		int curOffset = 0;
		
		fread(filename,1,0x80,infile);
		fread(&dataLen,1,4,infile);
		fread(&dataOffset,1,4,infile);
		
		for(x = 0; x < 0x80; x++)
			filename[x] ^= keyb[x % 4];
		
		dataLen ^= key;
		dataOffset ^= key;
		
		printf("[%04x] %-32s size[%08x] offset[%08x]\n",curFile,filename,dataLen,dataOffset);
		
		outfile = fopen(filename,"wb");
		data = (unsigned char*)calloc(dataLen,sizeof(unsigned char));
		
		curOffset = ftell(infile);
		fseek(infile,dataOffset,SEEK_SET);
		fread(data,1,dataLen,infile);
		
		for(x = 0; x < dataLen; x++)
			data[x] ^= keyb[x % 4];

		fwrite(data,1,dataLen,outfile);
		fclose(outfile);
		free(data);
		free(filename);
		
		fseek(infile,curOffset,SEEK_SET);
		curFile++;
	}
	
	chdir("..");
	
	fclose(infile);
	
	return 0;	
}