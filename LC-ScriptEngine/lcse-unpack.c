#include <stdio.h>
#include <stdlib.h>

FILE *lst = NULL;
FILE *dat = NULL;
FILE *outlst = NULL;

typedef struct
{
	int offset;
	int size;
	char name[0x40];
	int type;
} ENTRY;

ENTRY *Entry;

int fcount = 0;

void DumpFile(char *name, int offset, int size, int type)
{
	FILE *outfile = NULL;
	unsigned char *buffer = (unsigned char*)calloc(size,sizeof(unsigned char));
	char *outname = (char*)calloc(strlen(name)+5,sizeof(char));
	int i=0;

	fseek(dat,offset,SEEK_SET);
	fread(buffer,1,size,dat);

	strcpy(outname,name);

	// check if we need to decrypt this file (anything besides WAV and PNG is assumed)
	switch(type)
	{
		case 1:
			strcat(outname,".snx");
		
			for(i=0; i<size; i++) // decrypt it
				buffer[i] ^= 0x02;
			break;
		case 2:
			strcat(outname,".bmp");
			break;
		case 3:
			strcat(outname,".png");
			break;
		case 4:
			strcat(outname,".wav");
			break;
		case 5:
			strcat(outname,".ogg");
			break;
		default:
			printf("unknown type: %d\n",type);
			strcat(outname,".dat");
			break;
	}
	
	fprintf(outlst,"%s\n",outname);
	outfile = fopen(outname,"wb");
	
	if(!outfile)
	{
		printf("Could not open %s for writing\n",name);
		exit(-2);
	}
	fwrite(buffer,1,size,outfile);

	free(buffer);
	fclose(outfile);
}

int main(int argc, char **argv)
{
	char *lstname = NULL;
	char *outlstname = NULL;
	int i=0;

	if(argc<2)
	{
		printf("usage: %s infile (optional: outfolder)\n",argv[0]);
		return 0;
	}

	dat = fopen(argv[1],"rb");

	if(!dat)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	lstname = (char*)calloc(strlen(argv[1])+5,sizeof(char));
	strcpy(lstname,argv[1]);
	strcat(lstname,".lst");

	lst = fopen(lstname,"rb");
	if(!lst)
	{
		printf("Could not open %s\n",lstname);
		return -1;
	}
	
	outlstname = (char*)calloc(strlen(argv[1])+5,sizeof(char));
	strcpy(lstname,argv[1]);
	strcat(lstname,".txt");
	
	outlst = fopen(lstname,"w");
	if(!outlst)
	{
		printf("Could not open %s\n",outlstname);
		return -1;
	}

	fread(&fcount,1,4,lst);
	fcount ^= 0x01010101;

	if(argc==3)
	{
		mkdir(argv[2]);
		chdir(argv[2]);
	}

	Entry = (ENTRY*)calloc(fcount+1,sizeof(ENTRY));
	for(i=0; i<fcount; i++)
	{
		int x=0;
		fread(&Entry[i],1,sizeof(ENTRY),lst);

		Entry[i].size ^= 0x01010101;
		Entry[i].offset ^= 0x01010101;

		while(Entry[i].name[x]!='\0')
			Entry[i].name[x++] ^= 0x01;

		printf("[+] %-30s offset[%08x] size[%08x] type[%d]\n",Entry[i].name,Entry[i].offset,Entry[i].size,Entry[i].type);

		DumpFile(Entry[i].name,Entry[i].offset,Entry[i].size,Entry[i].type);
	}

	return 0;
}
