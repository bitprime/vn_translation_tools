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

PackEntry *entries = NULL;
unsigned char pad[4] = { 0 };
int fileCount=0;

int AddFile(char *name)
{
	FILE *infile = fopen(name,"rb");
	long fsize=0;
	
	if(!infile)
	{
		printf("Could not open %s\n",name);
		return -1;
	}
	
	fseek(infile,0,SEEK_END);
	fsize = ftell(infile);
	fclose(infile);
	
	entries = (PackEntry*)realloc(entries,(fileCount+1)*sizeof(PackEntry));
	
	memset(entries[fileCount].filename,'\0',32);
	strcpy(entries[fileCount].filename,name);
	entries[fileCount].filename[strlen(name)] = '\0';
	entries[fileCount].offset = 0; // will be fixed later before writing archive
	entries[fileCount].size = fsize;
	entries[fileCount].compsize = fsize; // compression is unsupported
	entries[fileCount].flag = 0x20000000; // some kind of flag, unknown usage and no time to research it right now
	fileCount++;
}

void WriteFiles(FILE *outfile)
{
	int i=0,size=0;
	
	for(i=0; i<fileCount; i++)
	{
		FILE *infile = fopen(entries[i].filename,"rb");
		unsigned char *buffer = (unsigned char*)calloc(entries[i].size,sizeof(unsigned char));
		
		if(!infile)
		{
			printf("Could not open %s\n",entries[i].filename);
			return;
		}
		
		if(ftell(outfile)!=entries[i].offset)
		{
			int x=0,z=0,j=0;
			
			for(x=entries[i].offset-ftell(outfile),j=0; x>0; x--,z++,j++)
			{
				fwrite(&pad[j],1,1,outfile);
				
				if(j>3)
					j = 0;
			}
		}
		
		fread(buffer,1,entries[i].size,infile);
		
		printf("[+] %03d %s [%08x][%08x]\n",i,entries[i].filename,ftell(outfile),entries[i].size);
		
		if(entries[i].filename[strlen(entries[i].filename)-2]=='.' && entries[i].filename[strlen(entries[i].filename)-1]=='s')
		{
			int x=0;
			
			for(x=0; x<entries[i].size; x++)
				buffer[x] ^= 0xff;
		}
		memcpy(pad,buffer,4);
		
		fwrite(buffer,1,entries[i].size,outfile);
		
		free(buffer);
		fclose(infile);
	}
}

void parsedir(char *path)
{
    WIN32_FIND_DATA fd;
    HANDLE handle;
	FILE *outfile = fopen("temp.dat","wb");
        
    handle = FindFirstFile(path,&fd);
    do 
    {
        if(strcmp(fd.cFileName,".") != 0 && strcmp(fd.cFileName,"..") != 0 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
        {                
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {	
				FILE *infile = NULL;
				char *temppath = (char*)calloc(strlen(path)+1,sizeof(char));
				char *buffer = NULL;
				int len=0;
				
				strcpy(temppath,path);
				temppath[strlen(temppath)-1] = '\0';
				
				chdir(temppath);
				AddFile(fd.cFileName);
				chdir("..");
				
				free(temppath);
            }
        }
    } while(FindNextFile(handle,&fd)!=0);
	
	fclose(outfile);
}

/*void parsedir(char *path)
{
	FILE *list = fopen("list.txt","r");
	
	while(!feof(list))
	{
		char file[256];
		
		fscanf(list,"%s\n",file);
		
		chdir("output");
		AddFile(file);
		chdir("..");
	}
}*/

void FixHeader(void)
{
	int i=0, base=fileCount*sizeof(PackEntry)+16;
	
	for(i=0; i<fileCount; i++)
	{
		entries[i].offset = base;
		base += entries[i].size;
		
		while((entries[i].offset%4)!=0)
		{
			base++;
			entries[i].offset++;
		}
	}
}

void WriteHeader(FILE *outfile)
{
	int i=0;
	
	fwrite("PACKDAT.",1,8,outfile);
	fwrite(&fileCount,1,4,outfile);
	fwrite(&fileCount,1,4,outfile); // again? why?
	
	for(i=0; i<fileCount; i++)
		fwrite(&entries[i],1,sizeof(PackEntry),outfile);
}

int main(int argc, char **argv)
{
	FILE *outfile = NULL;
	char *tempname = NULL;
	int j=0;
	
	if(argc!=3)
	{
		printf("usage: %s infolder outpack\n",argv[0]);
		return 0;
	}
	
	outfile = fopen(argv[2],"wb");
	tempname = (char*)calloc(strlen(argv[1])+4,sizeof(char));
	sprintf(tempname,"%s\\*",argv[1]);
	
	parsedir(tempname);
	FixHeader();
	WriteHeader(outfile);
	
	chdir(argv[1]);
	WriteFiles(outfile);
	chdir("..");
	
	free(tempname);
	
	while((ftell(outfile)%4)!=0)
	{
		fwrite(&pad[j],1,1,outfile);
		j = 1-j;
	}
	
	fclose(outfile);
	
	return 0;
}
