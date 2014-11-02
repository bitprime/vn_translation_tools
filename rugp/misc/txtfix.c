#include <stdio.h>
#include <stdlib.h>

char *wordwrap(char *, int);
char *ParseCmd(char *);
void ParseScript(char *, int);
void CreateScr(char *, char *);

#pragma pack(1)
struct Header
{
    char magic[4];
    int type;
    int count;
} header;

struct Entry
{
    int offset;
    int len;
    int id;
} *entries;

struct StringEntry
{
    char *str;
} *strings;
#pragma pack()

char *outname = NULL;
int lineLen = 0, novel = 0, newline = 0, curPos = 0;
unsigned int fileaddr = 0;
FILE *scriptfile = NULL;
int curoff = 0;
FILE *outfile = NULL;
int found = 0, total = 0;

int FindID(int oldid)
{
	int newid = 0;
	
	while(fread(&newid,1,3,scriptfile)==3)
	{			
		if((newid&0x00ffffff) == (oldid&0x00ffffff))
		{
			if(fread(&newid,1,2,scriptfile)==2)
			{
				if(fread(&newid,1,3,scriptfile)==3)
				{
					unsigned int t = 0;
					
					if(fread(&t,1,4,scriptfile)!=4)
						break;
						
					t &= 0xffff0000;
					t >>= 16;
					
					//printf("%08x %08x\n",t,ftell(scriptfile));
					
					if(t==0) // the length can't be 0
						continue;
					
					fread(&t,1,1,scriptfile);
					
					if(t>0x20 && t<0x80) // most likely not a real line
						break;
					
					//printf("%08x",newid&0x00ffffff);
					fseek(scriptfile,-13,SEEK_CUR);
					found++;
					return newid&0x00ffffff;
					break;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		
		fseek(scriptfile,-2,SEEK_CUR);
	}
}

void ParseScript(char *filename, int wrapLen)
{
    FILE *infile = fopen(filename,"rb");
    FILE *str = NULL;
    char *input = NULL;
    int id=0, offset=sizeof(header), origLen = wrapLen;
    
    if(!infile)
    {
        printf("Could not open %s\n",filename);
        exit(-1);
    }
    
    //str = fopen("temp","wb");
    input = (char*)calloc(8192,sizeof(char));
    
    header.type = 1;
    
    while(!feof(infile))
    {   
        char *temp = (char*)calloc(12,sizeof(char));
        
        fgets(input,8192,infile);
        
        if(strncmp(input,"#FILENAME ",10)==0)
        {
            outname = (char*)calloc(strlen(input),sizeof(char));
            memcpy(outname,input+10,strlen(input)-10);
            
            while(outname[strlen(outname)-1] == '\x0d' || outname[strlen(outname)-1] == '\x0a')
                outname[strlen(outname)-1] = '\0';
           
            sscanf(outname,"%08x",&fileaddr);
        }
        else if(strncmp(input,"#TYPE ",6)==0)
        {
			char riofile[512];
			char newoutname[512];
			
            memcpy(temp,input+6,strlen(input)-6);
            
            while(temp[strlen(temp)-1] == '\x0d' || temp[strlen(temp)-1] == '\x0a')
                temp[strlen(temp)-1] = '\0';
            
            header.type = atoi(temp);
            memset(temp,'\0',12);
            
            if(header.type==0)
                header.type = 1;
			
			sprintf(riofile,"%03d",header.type);
			mkdir(riofile);
			chdir(riofile);
				
			if(header.type==1)
				sprintf(riofile,"..\\scrdump.exe \"F:\\age\\マブラヴオルタネイティヴ\\Alternative.rio\" %08x %08x.dat\0",fileaddr,fileaddr);
			else
				sprintf(riofile,"..\\scrdump.exe \"F:\\age\\マブラヴオルタネイティヴ\\Alternative.rio.%03d\" %08x %08x.dat\0",header.type,fileaddr,fileaddr);
			printf("%s\n",riofile);
				
			system(riofile);
			
			sprintf(newoutname,"%s\0",filename);
			outfile = fopen(newoutname,"w");
			if(!outfile)
				printf("Could not open %s\n",newoutname);
			
			sprintf(riofile,"%08x\0",fileaddr);
			scriptfile = fopen(riofile,"rb");
			if(!scriptfile)
				printf("Could not open %s\n",riofile);
			
			
			while(input[strlen(input)-1]=='\n' ||
					input[strlen(input)-1]=='\x0d' ||
					input[strlen(input)-1]=='\x0a')
				input[strlen(input)-1]='\0';
				
			fprintf(outfile,"#FILENAME %s\n",outname);
			fprintf(outfile,"%s\n",input);
        }
        else if(!isspace(input[0]) && strncmp(input,"//",2)!=0 && input[0]!='\0')
        {
            int i=0;
			
			while(input[strlen(input)-1]=='\n' ||
					input[strlen(input)-1]=='\x0d' ||
					input[strlen(input)-1]=='\x0a')
				input[strlen(input)-1]='\0';
            
            memcpy(temp,input+1,8);
            sscanf(temp,"%x",&id);
			
			//printf("%08x -> ",id);
			id = FindID(id);
			//printf("\n");
			
			total++;
			fprintf(outfile,"<%08x>%s\n",id,input+10);
        }
        else if(!isspace(input[0]) && strncmp(input,"//<",3)==0 && input[0]!='\0')
        {
            int i=0;
			
			while(input[strlen(input)-1]=='\n' ||
					input[strlen(input)-1]=='\x0d' ||
					input[strlen(input)-1]=='\x0a')
				input[strlen(input)-1]='\0';
            
            memcpy(temp,input+1,11);
            sscanf(temp+3,"%x",&id);
			
			//printf("//%08x -> ",id);
			id = FindID(id);
			//printf("\n");
			
			total++;
			fprintf(outfile,"//<%08x>%s\n",id,input+12);
        }
		else
		{
			while(input[strlen(input)-1]=='\n' ||
					input[strlen(input)-1]=='\x0d' ||
					input[strlen(input)-1]=='\x0a')
				input[strlen(input)-1]='\0';
				
			fprintf(outfile,"%s\n",input);
		}
        
        free(input);
        input = (char*)calloc(8192,sizeof(char));
    }
	
    fclose(infile);
	
	if(str)
		fclose(str);
		
	fclose(scriptfile);
	
	if(outfile)
		fclose(outfile);
}

int main(int argc, char **argv)
{
    if(argc<3)
    {
        printf("usage: %s infile outfolder (optional: wordwrapping length)\n",argv[0]);
        return 0;
    }
    
    if(argc==4)
        ParseScript(argv[1],atoi(argv[3]));
    else
        ParseScript(argv[1],0);
		
	if(found!=total)
	{
		FILE *log = fopen("..\log.txt","a");
		fprintf(log,"Found mismatch in %s\n",argv[1]);
		fclose(log);
	}
    
    return 0;
}
