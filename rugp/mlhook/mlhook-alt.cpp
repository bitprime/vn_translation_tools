#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <zlib.h>
#include "mlhook-alt.h"

extern "C" _declspec(dllexport) void *HookMain()
{
	int addr=0;
	int idaddr=0;
	int textaddr=0;
	
	asm
	(
		"PUSH %eax;"
		"PUSH %ecx;"
		"PUSH %edx;"
		"PUSH %edx;"
	);
	
	asm
	(
		"MOV %0,%%ebx;" : "=m"(addr)
	);
	
	idaddr=addr;
	idaddr-=12;
	
	asm
	(
		"POP %edx;"
		"MOV %eax,[%edx-0x0c];"
		"AND %eax,0x00ffffff;"
	);
	
		
	textaddr = HookText((int*)addr, idaddr);
	
	asm
	(
		"MOV %ebx,%eax;"
		"POP %edx;"
		"POP %eax;"
		"POP %edx;"
		"POP %ecx;"
		"POP %eax;"
	);
}

extern "C" _declspec(dllexport) void DumpText(int *buffer, int *offset, int len)
{
    #if DEBUG >= 1
	if(dumpmem==1)
	{	
		unsigned char *chr = (unsigned char*)buffer;
		int testlen = 0;
		int length = len;
		
		if(dump==NULL) // don't reopen it if it's still looping
		{
			char *outname = (char*)calloc(MAX_PATH,sizeof(char*));
			
			sprintf(outname,"%08x.dmp",positionAddress);
			scrnum++;
			
            CreateDirectory("output",NULL);
            SetCurrentDirectory("output");
			dump = fopen(outname,"wb");
            scrlog = fopen("log.txt","a+");
            SetCurrentDirectory("..");
			free(outname);
            fprintf(scrlog,"%X %08x.dmp\n",fileType,positionAddress);
            fwrite(&fileType,1,4,dump);
		}
		
		if(length>0xFF)
		{
			testlen = chr[-1]<<8;
			testlen += chr[-2];
		}
		else
		{
			testlen = chr[-1];
		}
        
		if(testlen==length)
		{
			fwrite((char*)offset-0x0c,1,4,dump);
			fwrite(&len,1,4,dump);
			fwrite((char*)buffer,1,len,dump);
			fwrite("\0\0\0\0",1,4,dump);
		}
		else
		{
			unsigned char *ptr = (unsigned char*)buffer;
			
			asm
			(
				"MOV %1,[%0];"
				: "=r"(buffer), "=r"(ptr)
			);
			
			if(length>0xFF)
			{
				testlen = ptr[0]<<8;
				testlen += ptr[-1];
			}
			else
			{
				testlen = ptr[0];
			}
            
			if(testlen==length)
			{
				fwrite((char*)offset-0x0c,1,4,dump);
				fwrite(&len,1,4,dump);
				fwrite(ptr+1,1,len,dump);
				fwrite("\0\0\0\0",1,4,dump);
			}
		}
	}	
    #endif
    
    if(curFile!=positionAddress)
    {   
        curFile = positionAddress;
        ParseScr(positionAddress);
    }
}

void ParseScr(int file)
{
    int i=0;
    
	if(parsed==0)
		ParseArchive();
    
    //if(screntry!=NULL)
    //    free(screntry);
    
    //if(entry!=NULL)
    //    free(entry);
    
    if(amientry==NULL)
        return;
    
    for(i=0; i<amicount; i++)
    {
        if(amientry[i].file==file)
        {
            FILE *infile = fopen("data.ami","rb");
            ScrEntry *screntry = NULL;
            int x=0, addOffset=0;
            
            fseek(infile,amientry[i].offset+4,SEEK_SET);
            fread(&scrType,1,4,infile);
            
            if(file!=0xffffffff && fileType!=scrType)
            {
                fclose(infile);
                break;
            }
            
            fread(&scrcount,1,4,infile);
            
            screntry = (ScrEntry*)calloc(scrcount+1,sizeof(ScrEntry));
            //entry = (Entry*)calloc(scrcount+1,sizeof(Entry));
            entry = (Entry*)realloc(entry,(ec+scrcount+1)*sizeof(Entry));
            
            fread(screntry,1,sizeof(ScrEntry)*scrcount,infile);
            
            addOffset = amientry[i].offset;
            for(x=0; x<scrcount; x++,ec++)
            {   
                char msg[255];
                
                fseek(infile,addOffset+screntry[x].offset,SEEK_SET);
                
                entry[ec].str = (char*)calloc(screntry[x].len+1,sizeof(char));
                entry[ec].id = screntry[x].id;
                entry[ec].len = screntry[x].len;
                entry[ec].context = file;
                entry[ec].buffer = 0;
                
                fread(entry[ec].str,1,entry[ec].len,infile);
            }
            
            #if DEBUG > 1
            if(screntry==NULL || entry==NULL)
            {
                char msg[255];
                
                sprintf(msg,"ParseScr(): Address: %08x",file);
                MessageBox(NULL,msg,"Debug",MB_OK);
            }
            #endif
    
            free(screntry);
            fclose(infile);
            
            break;
        }
    }
}

int HookText(int *string, int id)
{		
	char *ptr = (char*)string;
    
	if(parsed==0)
		ParseArchive();
	
	/*if(str!=NULL)
		free(str);
	
	str = (char*)calloc(2048,sizeof(char*));
    */
        
    if(str==NULL)
        str = (char*)calloc(2064,sizeof(char*));
    else
        memset(str,'\0',2064);
	
	if(logchoices==1 && ptr[0]=='\x12')
	{
		int len=strlen((char*)string);
        unsigned int hash = GenHash((char*)string,HASHKEY,len);
	
        filelog = fopen("log.dat","ab");
        
        fseek(filelog,0,SEEK_END);
        fwrite(&hash,1,4,filelog);
		//fwrite(&id,1,4,filelog);
		fwrite(&len,1,4,filelog);
		fwrite((char*)string,1,len,filelog);
        
        fclose(filelog);
	}
	
	if(dumpmem==1 && dump!=NULL)
	{
		fclose(dump);
		fclose(scrlog);
		dump=NULL;
	}
	
	if(ec==0)
	{
		strcpy(str+16,(char*)string);
	}
	else
	{
		int i=0;
		int found=0;
        char msg[512];
        int len=strlen((char*)string);
	
		for(i=0; i<ec; i++)
		{   
            if(entry[i].id==(GenHash((char*)string,HASHKEY,len)) || ((id&0x0000ffff)==entry[i].id && (entry[i].buffer==0 || entry[i].buffer==(int)string)))
            {   
                entry[i].buffer = (int)string;
                strncpy(str+16,entry[i].str,entry[i].len);
                
                #if DEBUGMSG == 1
                sprintf(msg,"Orig (%04x): %s\nNew (%04x): %s\nBuffer: %08x\nCur Buffer: %08x\n%x\0",id&0x0000ffff,(char*)string,entry[i].id,entry[i].str,entry[i].buffer,(int)string);
                MessageBox(NULL,msg,"Message Found",MB_OK);
                #endif
                
                found = 1;
                break;
            }
		}
		
		if(found==0)
		{
			strcpy(str+16,(char*)string);
			//MessageBox(NULL,str,"Message Not Found",MB_OK);
		}
		
		found = 0;
	}
	
	return((int)(str+16));
}

void ParseArchive()
{
    FILE *infile = fopen("data.ami","rb");
    char magic[4];
    
    parsed = 1;
    
    fread(magic,1,4,infile);
    fread(&amicount,1,4,infile);
    fread(&baseOffset,1,4,infile);
    fseek(infile,4,SEEK_CUR);
    
    if(strncmp(magic,"AMI",3)!=0)
    {
        amt = -1;
    }
    else
    {   
        char msg[255];
        
        amientry = (AmiEntry*)calloc(amicount+1,sizeof(AmiEntry));
        fread(amientry,1,sizeof(AmiEntry)*amicount,infile);
        ParseScr(0xffffffff);
    }
}

void ParseImage()
{
    if(parsed==0)
    {
        ParseArchive();
    }
    
    imgentry = (ImageHeader*)calloc(amicount+1,sizeof(*imgentry));
    parsedImage = 1;
}

extern "C" _declspec(dllexport) void StorePositionAddress()
{   
    __asm
    (
        "mov %1, %%eax; mov %0, %%ebx": "=m"(positionAddress), "=m"(fileType)
    );
    
    wroteImage = 0;
}

extern "C" _declspec(dllexport) void StoreBufferAddress()
{
    __asm
    (
        "mov %0, %%eax": "=m"(bufferAddressCG)
    );
}

int GetImageFromOffset(int addr)
{
    int i=0;
    char msg[255];        
    
    for(i=0; i<amicount; i++)
    {
        if(amientry[i].file==addr)
        {
            return i;
        }
    }
    
    return -1;
}

extern "C" _declspec(dllexport) void HookImage()
{
    int addr = 0;
    
    if(parsedImage==0)
    {
        ParseImage();
    }
    
    if(parsedImage==1 && amicount!=0)
    {
        char msg[256];
        
        #if DEBUG > 0
        FILE *imglog = fopen("img.txt","a+");
        fprintf(imglog,"%08x\n",positionAddress);
        fclose(imglog);
        #endif
        
        addr = GetImageFromOffset(positionAddress);
        
        if(addr!=-1 && wroteImage!=1 && amientry[addr].size>0)
        {
            FILE *imgfile = fopen("data.ami","rb");
            
            if(imgentry[addr].read==0)
            {
                fseek(imgfile,amientry[addr].offset,SEEK_SET);
                imgentry[addr].bg = NULL;
                imgentry[addr].read = 1;
            }
            
            if(amientry[addr].compsize!=0)
            {
                unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize,sizeof(unsigned char));
                unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size,sizeof(unsigned char));
                uLongf size = amientry[addr].size;
                
                fread(zbuffer,1,amientry[addr].compsize,imgfile);
                uncompress(buffer,&size,zbuffer,amientry[addr].compsize);
                free(zbuffer);
                
                //memcpy((char*)(bufferAddressCG+0x20),(char*)(buffer+sizeof(*imgentry)-12),size-12);
                memcpy((char*)bufferAddressCG,(char*)(buffer+sizeof(*imgentry)-12),size-12);
                memcpy(&imgentry[addr],buffer,sizeof(*imgentry)-12);
                
                free(buffer);
            }
            else
            {
                fread(&imgentry[addr],1,sizeof(*imgentry)-12,imgfile);
                //fread((char*)bufferAddressCG+0x20,1,amientry[addr].size,imgfile);
                fread((char*)bufferAddressCG,1,amientry[addr].size,imgfile);
                //sprintf(msg,"Buffer Address: %08x\0",bufferAddressCG);
                //MessageBox(NULL,msg,"Debug",MB_OK);
            }
            
            fclose(imgfile);
            
            wroteImage = 1;
        }
    }
}

int GetImageFromBuffer(int srcBuffer)
{
    int i=0;
    
    for(i=0; i<amicount; i++)
    {
        if(imgentry[i].buffer==srcBuffer)
        {
            return i;
        }
    }
    
    return -1;
}

extern "C" _declspec(dllexport) void StoreBufferStruct()
{
    followBuffer = 1;
}

extern "C" _declspec(dllexport) void HookMalloc()
{
    int i=0;
    char msg[255];
    
    if(parsedImage==0)
    {
        ParseImage();
    }
    
    if(followBuffer == 1)
    {
        asm("mov %0, %%eax": "=m"(bufferAddressUI));
        
        for(i=0; i<amicount; i++)
        {
            if(amientry[i].file==positionAddress)
            {
                #if DEBUGMSG == 1
                sprintf(msg,"Address: %08x\nPosition address: %08x",bufferAddressUI,positionAddress);
                MessageBox(NULL,msg,"Debug",MB_OK);
                #endif
                
                imgentry[i].buffer = bufferAddressUI;
            }
            else if(imgentry[i].buffer == bufferAddressUI)
            {
                imgentry[i].buffer = 0;
            }
        }
        
        followBuffer = 0;
    }
}

extern "C" _declspec(dllexport) int HookUIImage()
{
    int mode = 0;
    char msg[256];
    
    __asm("mov %0, %%eax" : "=r"(mode));
    
    mode &= 0x000000ff;
    
    if(mode==0)
    {
        //asm("int 3");
        asm("mov %0, %%ecx;" : "=m"(srcBuffer));
        asm("mov %0, %%edx;" : "=m"(destBuffer));
        //asm("mov %0, [%%esp+0x138]" : "=r"(w));
        
        /*
        asm("mov %0, word ptr ds:[%%esp+0x13c]" : "=r"(xpos));
        asm("mov %0, word ptr ds:[%%esp+0x13e]" : "=r"(ypos));
        */
        
        if(imgentry[GetImageFromBuffer(srcBuffer)].buffer == srcBuffer)
        {       
            //asm("mov dword ptr ds:[%esp+0x150], 1");
            return 1;
        }
        
        /*
        sprintf(msg,"Mode 0\nDestination: %08x\nSource: %08x\0",destBuffer,srcBuffer);
        MessageBox(NULL,msg,"Debug",MB_OK);
        */
    }
    else if(mode==1)
    {     
        if(parsedImage==0)
        {
            ParseImage();
        }
        
        if(parsedImage==1 && amicount!=0)
        {
            int addr = GetImageFromBuffer(srcBuffer);
                
            if(addr!=-1 && amientry[addr].size>0)
            {
                FILE *imgfile = fopen("data.ami","rb");
                unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size,sizeof(unsigned char));
                int j=0,bufferOffset=0;
                int off=0,z=0,i=0;
                int copyBuffer = 0, o = 0;
            
                //sprintf(msg,"Mode 1\nDestination: %08x\nSource: %08x\nSize: %08x\0",destBuffer,srcBuffer,amientry[addr].size);
                //MessageBox(NULL,msg,"Debug",MB_OK);
                
                fseek(imgfile,amientry[addr].offset,SEEK_SET);
                
                if(amientry[addr].compsize!=0)
                {
                    unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize,sizeof(unsigned char));
                    uLongf compsize = amientry[addr].size;
                    
                    fread(zbuffer,1,amientry[addr].compsize,imgfile);
                    uncompress(buffer,&compsize,zbuffer,amientry[addr].compsize);
                    free(zbuffer);
                    memcpy(&imgentry[addr],buffer,sizeof(*imgentry)-12);
                }
                else
                {
                    fread(&imgentry[addr],1,sizeof(*imgentry)-12,imgfile);
                    fread(buffer,1,amientry[addr].size,imgfile);
                }
                
                w = imgentry[addr].w;
                h = imgentry[addr].h;
                xpos = imgentry[addr].x;
                ypos = imgentry[addr].y;
                
                bufferOffset = destBuffer+((xpos-((w/4)*ypos))*4);
                bufferOffset &= 0xffff0000;
                
                if((int*)bufferOffset==NULL)
                {
                    free(buffer);
                    return 0;
                }
                
                while(*(int*)(bufferOffset+0x10)!=0x001d5000)
                    bufferOffset-=0x10000;
                bufferOffset += 0x20;
                
                if(imgentry[addr].bg==NULL)
                {
                    imgentry[addr].bg = (unsigned char*)calloc(0x001d5000,sizeof(unsigned char));
                    copyBuffer = 1;
                }
                
                /*
                    Alpha blending method: 
                    
                    N[A] = S[A];
                    N[R] = S[R] + (D[R]-S[R]) * ((float)D[A]/255);
                    N[G] = S[G] + (D[G]-S[G]) * ((float)D[A]/255);
                    N[B] = S[B] + (D[B]-S[B]) * ((float)D[A]/255);
                */  
                
                for(z=0; z<h; z++)
                {
                    int pos = xpos*4+((resw/4)*(resh/4)*4-(ypos+h)*resw);
                    
                    for(i=0; i<w*4; i+=4)
                    {
                        {
                            unsigned char r, g, b, a;
                            unsigned char fr, fg, fb, fa;
                            
                            if(copyBuffer == 1)
                            {
                                imgentry[addr].bg[o] = *(char*)(bufferOffset+off+pos+i);
                                imgentry[addr].bg[o+1] = *(char*)(bufferOffset+off+pos+i+1);
                                imgentry[addr].bg[o+2] = *(char*)(bufferOffset+off+pos+i+2);
                                imgentry[addr].bg[o+3] = *(char*)(bufferOffset+off+pos+i+3);
                            }
                            else
                            {
                                *(char*)(bufferOffset+off+pos+i) = imgentry[addr].bg[o];
                                *(char*)(bufferOffset+off+pos+i+1) = imgentry[addr].bg[o+1];
                                *(char*)(bufferOffset+off+pos+i+2) = imgentry[addr].bg[o+2];
                                *(char*)(bufferOffset+off+pos+i+3) = imgentry[addr].bg[o+3];
                            }
                            o += 4;
                            
                            b =  *(char*)(bufferOffset+off+pos+i);
                            g = *(char*)(bufferOffset+off+pos+i+1);
                            r = *(char*)(bufferOffset+off+pos+i+2);
                            a = *(char*)(bufferOffset+off+pos+i+3);
                            
                            
                            fb = buffer[i+j];
                            fg = buffer[i+j+1];
                            fr = buffer[i+j+2];
                            fa = buffer[i+j+3];
                            
                            if(fa==0xff) // splitting these two section up since the calculations don't need to be done, thus saving a tiny bit of time
                            {
                                *(char*)(bufferOffset+pos+off+i) = fb;
                                *(char*)(bufferOffset+pos+off+i+1) = fg;
                                *(char*)(bufferOffset+pos+off+i+2) = fr;
                                *(char*)(bufferOffset+pos+off+i+3) = fa;
                            }
                            else if(a!=0xff && fa!=0xff)
                            {
                                *(char*)(bufferOffset+pos+off+i) = b + (fb-b) * ((float)fa/255);
                                *(char*)(bufferOffset+pos+off+i+1) = g + (fg-g) * ((float)fa/255);
                                *(char*)(bufferOffset+pos+off+i+2) = r + (fr-r) * ((float)fa/255);
                            }
                        }
                    }
                 
                    j += w*4;
                    off += resw;
                }
            
                free(buffer);
                fclose(imgfile);
            }
        }
    }
    
    return 0;
}

unsigned int GenHash(char *string, unsigned int key, int len)
{
	unsigned int hash = 0, i=0;

	for(i=0; i<len; i++)
        hash = (((key^hash)<<3)+hash)^string[i];

	return hash;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	return true;
}
