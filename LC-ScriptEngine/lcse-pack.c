#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

FILE *outdat = NULL;
FILE *outlst = NULL;
FILE *inlst = NULL;
int offset = 0, filecount = 0;

void ReadDirectory(char*);
void ReadList(char*);

int main(int argc, char **argv)
{
	char *datname = NULL;
	char *lstname = NULL;
	char *inlstname = NULL;
	char *temp = NULL;
	
	if(argc<2)
	{
		printf("usage: %s inputfolder (optional: inputlist)\n",argv[0]);
		return -1;
	}
	
	datname = (char*)calloc(strlen(argv[1])+5,sizeof(char));
	strcpy(datname,argv[1]);
	strcat(datname,".dat");
	
	lstname = (char*)calloc(strlen(argv[1])+5,sizeof(char));
	strcpy(lstname,argv[1]);
	strcat(lstname,".lst");
	
	if(argc==3)
	{
		inlst = fopen(argv[2],"r");
	}
	
	temp = (char*)calloc(strlen(argv[1])+3,sizeof(char));
	strcpy(temp,argv[1]);
	strcat(temp,"\\*");
	
	outdat = fopen(datname,"wb");
	outlst = fopen(lstname,"wb");
	
	fwrite(&filecount,1,4,outlst);
	
	if(argc==3)
		ReadList(temp);
	else
		ReadDirectory(temp);
		
	fclose(outlst);
	outlst = fopen(lstname,"rb+");
	
	filecount ^= 0x01010101;
	fwrite(&filecount,1,4,outlst);
	
	free(temp);
	free(datname);
	free(lstname);
	
	if(argc==3)
	{
		free(inlstname);
		fclose(inlst);
	}
	
	fclose(outdat);
	fclose(outlst);
}

void AddFile(char *path, char *filename)
{
	FILE *data = NULL;
	unsigned char *buffer = NULL;
	char *temp = (char*)calloc(strlen(filename)+1,sizeof(char));
	char *datpath = (char*)calloc(strlen(path)+strlen(filename)+1,sizeof(char));
	unsigned char *outname = (unsigned char*)calloc(0x40,sizeof(unsigned char));
	int i=0, filelen = 0;
	int type = 0;
	int tempoffset = offset;
	
	strcat(temp,filename);
	
	for(i=strlen(temp)-1; i>0; i--)
	{
		if(temp[i]=='.')
		{
			if(strncmp((char*)(temp+i),".snx",4)==0)
				type = 1;
			else if(strncmp((char*)(temp+i),".bmp",4)==0)
				type = 2;
			else if(strncmp((char*)(temp+i),".png",4)==0)
				type = 3;
			else if(strncmp((char*)(temp+i),".wav",4)==0)
				type = 4;
			else if(strncmp((char*)(temp+i),".ogg",4)==0)
				type = 5;
			
			temp[i] = '\0';
			break;
		}
	}
	
	strncpy(datpath,path,strlen(path)-1);
	strcat(datpath,filename);
	
	data = fopen(datpath,"rb");
	
	if(!data)
	{
		printf("Could not open %s\n",data);
		exit(-1);
	}
	
	fseek(data,0,SEEK_END);
	filelen = ftell(data);
	rewind(data);
	
	buffer = (unsigned char*)calloc(filelen,sizeof(unsigned char));
	fread(buffer,1,filelen,data);
	
	if(type==1) // .snx
	{
		for(i=0; i<filelen; i++)
			buffer[i] ^= 0x02;
	}
	
	fwrite(buffer,1,filelen,outdat);
	printf("[+] %-30s offset[%08x] size[%08x]\n",temp,tempoffset,filelen);
	
	offset += filelen;

	tempoffset ^= 0x01010101;
	filelen ^= 0x01010101;
	
	fwrite(&tempoffset,1,4,outlst);
	fwrite(&filelen,1,4,outlst);
	
	strncpy(outname,temp,0x40);
	for(i=0; i<strlen(outname); i++)
		outname[i] ^= 0x01;
		
	fwrite(outname,1,0x40,outlst);
	fwrite(&type,1,4,outlst);
	
	free(temp);
	fclose(data);
	free(buffer);
	
	filecount++;
}

void ReadList(char *path)
{
	char line[1024];
	
	while(fgets(line,1024,inlst)!=NULL)
	{
		while(line[strlen(line)-1]=='\n')
			line[strlen(line)-1]='\0';
			
		AddFile(path,line);
	}
}

void ReadDirectory(char *path)
{
    WIN32_FIND_DATA fd;
    HANDLE handle;
    
    handle = FindFirstFile(path,&fd);
    do 
    {
        if(strcmp(fd.cFileName,".") != 0 && strcmp(fd.cFileName,"..") != 0 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
        {       
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
				AddFile(path,fd.cFileName);
			}
        }
    } while(FindNextFile(handle,&fd) != 0);
}
