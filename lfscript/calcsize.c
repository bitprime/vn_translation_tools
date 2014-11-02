#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *infile = fopen("bytes.bin","rb");
	int totalBytes = 0, totalLines = 0, scripts = 0;;
	
	while(!feof(infile))
	{
		int bytes = 0, lines = 0;
		fread(&bytes,1,4,infile);
		fread(&lines,1,4,infile);
		
		totalBytes += bytes;
		totalLines += lines;
		scripts++;
	}
	
	printf("total scripts: %d\n",scripts);
	printf("total lines: %d\n",totalLines);
	printf("total kb: %4.2f kb\n",(double)(totalBytes)/1024);

	return 0;
}
