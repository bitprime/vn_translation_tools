#include <stdio.h>
#include <windows.h>

typedef struct
{
	char filename[32];
	int offset;
	int flag;
	int size;
	int compsize; // one of these is the compressed size, not sure which. not needed for my purpose
}  PackEntry;

int main(int argc, char **argv)
{
	FILE *infile = NULL, *list = NULL;
	PackEntry *entries = NULL;
	int fileCount=0,i=0;
	
	if(argc!=3)
	{
		printf("usage: %s infolder outpack\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"rb");
	fseek(infile,8,SEEK_SET);
	fread(&fileCount,1,4,infile);
	fseek(infile,0x10,SEEK_SET);
	printf("%d\n",fileCount);
	
	entries = (PackEntry*)calloc(fileCount+1,sizeof(PackEntry));
	fread(entries,sizeof(PackEntry),fileCount,infile);
	
	list = fopen("list.txt","w");
	
	mkdir("output");
	chdir("output");
	for(i=0; i<fileCount; i++)
	{
		FILE *outfile = fopen(entries[i].filename,"wb");
		char *buffer = (char*)calloc(entries[i].size,sizeof(char));
		
		printf("[+] %03d %s [%08x][%08x]\n",i,entries[i].filename,entries[i].offset,entries[i].size);
		fprintf(list,"%s\n",entries[i].filename);
		
		fseek(infile,entries[i].offset,SEEK_SET);
		fread(buffer,1,entries[i].size,infile);
		fwrite(buffer,1,entries[i].size,outfile);
		
		fclose(outfile);
		free(buffer);
	}
	chdir("..");
	
	fclose(list);
	fclose(infile);
	
	return 0;
}
