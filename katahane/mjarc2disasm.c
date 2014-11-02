#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile = NULL;
	FILE *outfile = NULL;
	int curline = 0;
	
	if(argc != 3)
	{
		printf("usage: %s inscript inasm outscript outasm\n",argv[0]);
		return 1;
	}
	
	infile = fopen(argv[1], "r");
	outfile = fopen(argv[3], "w");
	
	while(!feof(infile))
	{
		char line[0x1000];
		int i = 0, startpos = 0;
		
		memset(line, '\0', 0x1000);
		fgets(line, 0x1000, infile);
		
		if(line[0] != '<')
			continue;
		
		while(line[strlen(line) - 1] == '\r' || 
			line[strlen(line) - 1] == '\n')
				line[strlen(line) - 1] = '\0';
				
		fprintf(outfile,"<%08d> ",curline);
		curline++;
		
		while(line[startpos] != '>' && line[startpos+1] != ' ')
			startpos++;
		startpos += 2;
		
		for(i=startpos; i<strlen(line); i++)
		{
			if(line[i] == '=')
			{
				i++;
				
				/*
				if(line[i] == '~' && (line[i+1] == 'p' || line[i+1] == 'c'))
				{
					line[i] = '\\';
				}
				else */
				
				if(line[i] == '\r' || line[i] == '\n' || line[i] == '\0')
				{
					line[i-1] = '\0';
					fprintf(outfile,"%s\n",line+startpos);
				}
				else
				{
					if(strcmp(line+i, "-") != 0)
						fprintf(outfile,"%s",line+i);
					fprintf(outfile,"\n");
				}
				
				break;
			}
		}
		
		//printf("%s\n",line);
	}
	
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
