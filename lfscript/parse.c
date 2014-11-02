#include <stdio.h>
#include <stdlib.h>

#define MIN_LEN 10

unsigned short punc[] = { 0x8163, 0x8142, 0x8141, 0x8164, 0x8169, 0x816F, 0x8179, 0x8177, 0x8175, 0x8183, 0x8167, 0x816A, 0x8170, 0x817A, 0x8178, 0x8176, 0x8184, 0x8140, 0x8144, 0x8149, 0x8168, 0x815B, 0x815C };

int main(int argc, char **argv)
{
	FILE *infile = fopen(argv[1],"rb");
	FILE *outfile = fopen("output.txt","ab+");
	FILE *outfile2 = fopen("filtered.txt","ab+");
	FILE *outfile3 = fopen("counts.txt","ab+");
	FILE *outfile4 = fopen("bytes.bin","ab+");
	unsigned char *buffer = NULL;
	int scriptSize = 0;
	int curLine = 0, realLines = 0, realBytes = 0, curPos = 0, curPosBak = 0, curLen = 0;
	
	fseek(infile,0,SEEK_END);
	scriptSize = ftell(infile);
	rewind(infile);
	
	buffer = (unsigned char*)calloc(scriptSize, sizeof(unsigned char));	
	fread(buffer,1,scriptSize,infile);
	
	fclose(infile);	
	
	while(curPos < scriptSize)
	{
		unsigned char c = buffer[curPos];
		int filtered = 0;
		curPosBak = curPos;
		curPos++;
		curLen = 0;
		
		if(c == ',')
		{
			curLine++;
			filtered = 2;
		}
		else
		{
			if(c >= 0x80 || c == '<' || (c == '\\' && buffer[curPos] == 'n')) // japanese or in-command scripting text
			{
				int hasPunc = 0;
				
				while(c != ',')
				{
					curLen++;
					
					if(c >= 0x80)
					{
						int i = 0;
						
						for(i = 0; i < sizeof(punc) / sizeof(unsigned short); i++)
						{
							if(((c << 8) | buffer[curPos]) == punc[i])
							{
								hasPunc = 1;
								break;
							}
						}
					
						curPos++;
					}
					else
					{
						if(c == '.' && (buffer[curPos] == 'a' || buffer[curPos] == 't'))
						{
							filtered = 1;
						}
					}
					
					c = buffer[curPos++];
				}
				
				if(c == ',')
					curPos--;
					
				if(curLen <= MIN_LEN && !hasPunc)
					filtered = 1;
					
				if(filtered) // double check
				{
					int i = 0;
					
					for(i = 0; i < sizeof(punc) / sizeof(unsigned short); i++)
					{
						if(((buffer[curPosBak] << 8) | buffer[curPosBak+1]) == punc[i])
						{
							filtered = 0;
							break;
						}
					}
				}
			}
			else // ascii
			{
				while(c != ',')
				{
					if(c >= 0x80)
						curPos++;
					
					c = buffer[curPos++];
				}
				
				if(c == ',')
					curPos--;
					
				filtered = 1;
			}
		}
		
		if(filtered == 1)
		{
			int i = 0;
			
			for(i = curPosBak; i < curPos; i++)
				fwrite(&buffer[i], 1, 1, outfile2);
				
			i = 0x0d;
			fwrite(&i,1,1,outfile2);
			i = 0x0a;
			fwrite(&i,1,1,outfile2);
		}
		else if(filtered == 0)
		{
			int i = 0;
			
			for(i = curPosBak; i < curPos; i++)
			{
				fwrite(&buffer[i], 1, 1, outfile);
				realBytes++;
			}
			
			i = 0x0d;
			fwrite(&i,1,1,outfile);
			i = 0x0a;
			fwrite(&i,1,1,outfile);
			
			realLines++;
		}
	}
	
	fprintf(outfile3,"%-20s\t%4d lines\t%2.2lf kb\r\n", argv[2], realLines, (double)(realBytes)/1024);
	fwrite(&realBytes,1,4,outfile4);
	fwrite(&realLines,1,4,outfile4);

	free(buffer);
	fclose(outfile);
	fclose(outfile2);
	fclose(outfile3);
	fclose(outfile4);
}
