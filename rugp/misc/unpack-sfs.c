#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = fopen(argv[1], "rb");
	FILE *outfile = NULL;
	int curFile = 0;

	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return 1;
	}

	mkdir(argv[2]);
	chdir(argv[2]);

	while(!feof(infile))
	{
		char outname[0x20];
		unsigned char *buffer = NULL;
		int offset = 0, curOffset = 0;
		int len = 0;
		int i = 0;

		fread(&offset,1,4,infile);
		curOffset = ftell(infile);
		fread(&len,1,4,infile);

		offset *= 0x800;
		len *= 0x800;

		if(len == 0)
			break;

		len -= offset;

		fseek(infile,offset,SEEK_SET);
		buffer = (unsigned char*)calloc(len,sizeof(unsigned char));

		fread(buffer,1,len,infile);

		sprintf(outname, "%08d.bin\0", curFile);
		if(memcmp(buffer,"VAG",3)==0) // playstation VAG audio format
		{
			memcpy(outname,buffer+0x20,0x10);
			
			for(i=0; i<strlen(outname); i++)
			{
				if(outname[i]=='.')
				{
					break;
				}
			}

			memcpy(outname+i,".vag\0",5);
		}

		outfile = fopen(outname,"wb");

		fwrite(buffer,1,len,outfile);
		printf("[+] %-32s\toffset[%08x]\tsize[%08x]\n",outname,offset,len);

		fclose(outfile);
		free(buffer);

		fseek(infile,curOffset,SEEK_SET);
		curFile++;
	}

	return 0;
}
