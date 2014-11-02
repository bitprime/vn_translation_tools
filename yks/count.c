#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = fopen(argv[1],"r");
	int kbCount=0,lineCount=0;

	while(!feof(infile))
	{
		int a=0,b=0;

		fscanf(infile,"%d %d\n",&a,&b);
		
		lineCount += a;
		kbCount += b;
	}
	
	printf("%d %lf\n",lineCount,(double)kbCount/1024000);

	fclose(infile);

	return 0;
}
