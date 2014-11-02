// for "Lost Colors" which runs on the ANOS engine

#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

#pragma pack(1)
struct FileEntry
{
	char filename[0x20];
	int unk;
	int offset;
	int compsize;
	int decompsize;
} *Entry;
#pragma pack()

unsigned int sar(unsigned int input, unsigned int count)
{
	unsigned int output = 0;
	
	__asm
	{
		pushad;
		mov esi, input;
		mov ecx, count;
		sar esi, cl;
		mov output, esi;
		popad;
	};
	
	return output;
}

unsigned char GenerateKey(char *input)
{
	int i=0;
	unsigned char key = 0;
	
	for(i=0; i<strlen(input); i++)
	{
		unsigned char temp = input[i];
		
		if(temp >= 0x61 && temp <= 0x7a)
			temp += 0xe0;
		
		key ^= temp;
		temp = key >> 7;
		key <<= 1;
		key |= temp;
	}
	
	return key;
}

void DecryptChunk(unsigned char *input, int size, unsigned char key)
{
	int i=0, c=0;
	
	for(i=0; i<size; i++)
	{
		int t = 8;
		
		t -= c;
		input[i] = ((input[i]<<t)&0xff) | (input[i]>>(c&0xff));
		input[i] ^= key;
		
		c++, key++;
		c &= 7;
	}
}

unsigned char *DecompFile(unsigned char *input, int insize, int outsize)
{
	unsigned char *output = (unsigned char*)calloc(outsize, sizeof(unsigned char));
	unsigned char *dict = (unsigned char*)calloc(0x1000, sizeof(unsigned char));
	int i=0, c=0, out=0, o=0, l=0xfee;
	unsigned int test = 0;
	
	for(;i<insize;)
	{
		int t = 8;
		unsigned int temp = 0;
		
		#ifdef DEBUG
		printf("[%08x] ",i);
		#endif
		
		test >>= 1;
		
		#ifdef DEBUG
		printf("%d ",!((test&0xff00)>>8)&1);
		#endif
		if(!((test&0xff00)>>8)&1)
		{
			c = i&7;
			t -= c;
			temp = ((input[i]<<t)&0xff) | sar(input[i],(c&0xff));
			#ifdef DEBUG
			printf("<%02x %02x> ",input[i],c);
			#endif
			temp &= 0xff;
			temp |= 0xff00;
			
			i++, out++;			
			test = temp;
		}
		
		if(i>insize)
			break;
			
		#ifdef DEBUG
		printf("%04x %d ",test,!(test&1));
		#endif
		
		if(!(test&1))
		{		
			c = i&7;
			t = 8-c;
			temp = (input[i]<<t) | sar(input[i],(c&0xff));
			temp &= 0xff;
			#ifdef DEBUG
			printf("b[%08x %02x] ",i,input[i]);
			#endif
			i++;
			
			if(i<insize)
			{
				unsigned int temp3 = 0, len = 0;
				int x = 0;
				
				c = i&7;
				t = 8-c;
				temp3 = (input[i]<<t) | sar(input[i],(c&0xff));
				i++;
				
				temp3 &= 0xff;
				temp |= ((temp3&0xf0)<<4);
				#ifdef DEBUG
				printf("%02x %04x %04x\n",i,temp3,temp);
				#endif
				len = (temp3&0x0f)+2;
				
				out++;
				
				// supposed to check for the line here but whatever
				// 0040A176  |. /78 43         |JS SHORT anos3.0040A1BB
				
				#ifdef DEBUG
				printf("%08x %08x\n",temp,len);
				#endif

				for(x=0; x<=len; x++)
				{
					unsigned char b = dict[temp++];
					
					output[o++] = b;
					dict[l++] = b;
					
					l &= 0xfff;
					temp &= 0xfff;
				}
			}
		}
		else
		{		
			c = i&7;
			t = 8-c;
			temp = (input[i]<<t) | sar(input[i],(c&0xff));
			temp &= 0xff;
			
			#ifdef DEBUG
			printf("a[%02x %02x]\n",input[i],temp);
			#endif
			
			output[o++] = temp;
			dict[l++] = temp;
			
			l &= 0xfff;
			i++;
		}
	}
		
	return output;
}

void ParseFiles(unsigned char *buffer, int dataoffset, int filecount)
{
	FILE *outfile = NULL;
	int i=0;
	
	for(i=0; i<filecount; i++)
	{
		unsigned char *output = NULL;
		Entry = buffer + (i*0x30);
		
		printf("[+] %-24s offset[%08x] compsz[%08x] origsz[%08x]\n",Entry->filename,Entry->offset,Entry->compsize,Entry->decompsize);
		output = DecompFile(buffer+Entry->offset-16,Entry->compsize,Entry->decompsize);		
		
		outfile = fopen(Entry->filename,"wb");
		fwrite(output, 1, Entry->decompsize, outfile);
		fclose(outfile);
		free(output);
	}
}

int main(int argc, char **argv)
{
	FILE *inpak = NULL, *outpak = NULL;
	unsigned char key = 0;
	unsigned char *buffer = NULL;
	int filelen = 0, filecount = 0, offset = 0;

	if(argc!=3)
	{
		printf("uasge: %s in.pak outfolder\n");
		return 1;
	}
	
	key = GenerateKey(argv[1]);
	
	inpak = fopen(argv[1],"rb");
	fseek(inpak,0,SEEK_END);
	filelen = ftell(inpak);
	rewind(inpak);
	
	buffer = (unsigned char*)calloc(filelen,sizeof(unsigned char));
	fread(buffer, 1, filelen, inpak);
	DecryptChunk(buffer, 16, key);
	
	filecount = *(int*)(buffer+8);
	offset = filecount*0x30;
	DecryptChunk(buffer+16, offset, key);
	
	if(argv[2][0]!='.' && argv[2][0]!='\0')
	{
		mkdir(argv[2]);
		chdir(argv[2]);
	}
	
	ParseFiles(buffer+16,offset,filecount);
	
	fclose(inpak);
	free(buffer);
	
	if(argv[2][0]!='.' && argv[2][0]!='\0')
		chdir("..");

	return 0;
}
