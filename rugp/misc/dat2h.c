#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *infile = NULL, *outfile = NULL;
	int len = 0, i = 0;
	
	if(argc!=4)
	{
		printf("usage: %s infile outfile buffname\n",argv[0]);
		return -1;
	}
	
	infile = fopen(argv[1],"rb");
	
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return -2;
	}
	
	outfile = fopen(argv[2],"w");
	
	fseek(infile,0,SEEK_END);
	len = ftell(infile);
	rewind(infile);
	
	fprintf(outfile,"#ifndef __%s_H__\n#define __%s_H__\n#include \"global.h\"\n\n#if GAME == MUVLUVEXUL\nstatic int %s_length = %d;\nstatic unsigned char %s_data[] =\n{",toupper(argv[3]),toupper(argv[3]),argv[3],len,argv[3]);
	
	for(i=0; i<len; i++)
	{
		unsigned char temp = 0;
		
		if((i%16)==0)
			fprintf(outfile,"\n\t");
		
		fread(&temp,1,1,infile);
		
		fprintf(outfile,"0x%02x",temp);
		
		if(i+1<len)
			fprintf(outfile,", ");
		else
			fprintf(outfile,"\n};");
		
	}
	
	fprintf(outfile,"\n#endif\n#endif\n");
	
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
