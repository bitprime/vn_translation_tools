#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <zlib.h>
#include "mlhook.h"

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
//		"PUSH %edx;" // for muvluv TE
	);
	
	asm
	(
		"MOV %0,%%ebx;" : "=r"(addr)
	);
	
	idaddr=addr;
	idaddr-=12;
	
	asm
	(
//		"POP %edx;" // for muvluv TE
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

extern "C" _declspec(dllexport) int HookNamePrefetch(unsigned char *buffer)
{
	return (int)buffer;
	
    if(ec<=0)
        return (int)buffer;
    
    if((int)buffer==(int)origbuffer && readName==1)
    {
        readName = 0;
        
        return (int)prevbuffer;
    }
    else
    {   
        prevbuffer = buffer;
        readName = 1;
    
        return origbuffer;
    }
    
    /*
    int i=0;
    
    if(buffer==namebuff)
        return (int)prevbuffer;
    
    prevbuffer = buffer;
    
    if(namebuff==NULL)
        namebuff = (unsigned char*)calloc(128,sizeof(unsigned char));
        
    memset(namebuff,'\0',128);
    
    for(i=0; i<strlen((char*)buffer); i++)
    {
        if(buffer[i-2]==0x81 && buffer[i-1]==0x7a)
            break;
            
        namebuff[i] = buffer[i];
    }
    
    for(i=0; i<namecnt; i++)
    {
        if(stricmp((char*)namebuff,names[i])==0)
        {
            memset(namebuff,'\0',128);
            strncpy((char*)namebuff,names[i+1],strlen(names[i+1]));
            break;
        }
    }
    
    if(i>=namecnt)
    {
        memset(namebuff,'\0',128);
        strcpy((char*)namebuff,(char*)buffer);
    }
    
    return (int)namebuff;
    */
}

extern "C" _declspec(dllexport) void DumpText(int *buffer, int len, int *realbuffer)
{
    unsigned char *chr = (unsigned char*)buffer;
    int i=0,id=0;
	
    if(curFile!=positionAddress)
    {   
		curFile = positionAddress;
        ParseScr(positionAddress);
		
		#if DEBUGLOG == 1
		if(debuglogoff==0)
		{
			FILE *debuglog = fopen("debug.txt","a");
			fprintf(debuglog,"Parsed new script (%08x), now dumping entries...\n",curFile);
			fclose(debuglog);
			debuglogoff = 1;
		}
		#endif
		
		#if DEBUG>=1
		if(fileOpen==1)
		{
			fileOpen = 0;
			fclose(dump);
			dump = NULL;
		}
		#endif
    }

    #if DEBUG >= 1
	if(dumpmem==1)
	{	
		int testlen = 0;
		int length = len;
		
		if(dump==NULL) // don't reopen it if it's still looping
		{
			char *outname = (char*)calloc(MAX_PATH,sizeof(char*));
			
			char msg[255];
			
			sprintf(outname,"%08x.dmp",curFile);
			sprintf(msg,"%08x %08x %s\n",curFile,positionAddress,outname);
			MessageBox(NULL,msg,"Debug",MB_OK);
			scrnum++;
			
            CreateDirectory("output",NULL);
            SetCurrentDirectory("output");
			dump = fopen(outname,"wb");
            scrlog = fopen("log.txt","a+");
            SetCurrentDirectory("..");
			free(outname);
            fprintf(scrlog,"%X %08x.dmp\n",fileType,curFile);
            fwrite(&fileType,1,4,dump);
			
			fileOpen = 1;
		}
		
		fwrite((char*)buffer-0x0c,1,4,dump);
		fwrite(&len,1,4,dump);
		fwrite((char*)buffer,1,len,dump);
		fwrite("\0\0\0\0",1,4,dump);
		
		/*
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
			fwrite((char*)buffer-0x0c,1,4,dump);
			fwrite(&len,1,4,dump);
			fwrite((char*)buffer,1,len,dump);
			fwrite("\0\0\0\0",1,4,dump);
		}
		else
		{
			unsigned char *ptr = chr-1;
			
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
				fwrite((char*)buffer-0x0c,1,4,dump);
				fwrite(&len,1,4,dump);
				fwrite(ptr+1,1,len,dump);
				fwrite("\0\0\0\0",1,4,dump);
			}
		}
		*/
	}	
    #endif
	
    for(i=0; i<ec; i++)
    {   
        if(curFile==entry[i].context && entry[i].buffer == 0 && (*(buffer-3)&0xffffff)==entry[i].id)
        {
			
            entry[i].buffer = (int)buffer;
            entry[i].buffer2 = (int)realbuffer;
            break;
        }
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

            if(!infile)
                break;
            
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
                readFile = file;
				
				
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
        
    /*
    if(str==NULL)
        str = (char*)calloc(2046,sizeof(char*));
    else
        memset(str,'\0',2046);
     */
	
	#if DEBUGLOG == 1
	if(debuglogoff==1)
	{
		FILE *debuglog = fopen("debug.txt","a");
		int num=0;
		
		for(num=0; num<ec; num++)
			fprintf(debuglog,"elm: %d\tid: %08x\tlen: %d\tcontext: %08x\tbuffer: %08x\tbuffer 2: %08x\tstr buffer: %08x\tstring: %s\n",num,entry[num].id,entry[num].len,entry[num].context,entry[num].buffer,entry[num].buffer2,(int)entry[num].str,entry[num].str);
		
		fprintf(debuglog,"Done parsing script\n\n");
		fclose(debuglog);
		debuglogoff = 0;
	}
	#endif
	 
	 #if DEBUGLOG == 1
	 {
		FILE *debuglog = fopen("debug.txt","a");
		SYSTEMTIME systemTime;
		
		GetLocalTime(&systemTime);
		fprintf(debuglog,"(%04d-%02d-%02d %02d:%02d:%0d) Hooking %08x (id: %08x)... ",systemTime.wYear,systemTime.wMonth,systemTime.wDay,systemTime.wHour,systemTime.wMinute,systemTime.wSecond,(int)string,id);
		fclose(debuglog);
	 }
	 #endif
	
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
		fileOpen = 0;
		fclose(dump);
		fclose(scrlog);
		dump=NULL;
	}
	
	if(ec==0)
	{
		//strcpy(str+16,(char*)string);
        return (int)string;
	}
	else
	{
		int i=0;
		int found=0;
        char msg[512];
        int len=strlen((char*)string);
	
		for(i=0; i<ec; i++)
		{   
            if(((id&0x00ffffff)==entry[i].id && ((entry[i].buffer==0 && entry[i].context==readFile) || entry[i].buffer==(int)string)) || (entry[i].id==GenHash((char*)string,HASHKEY,len)))
            {
                #if DEBUGMSG == 1
                sprintf(msg,"Orig (%04x): %s\nNew (%04x): %s\nBuffer: %08x\nCur Buffer: %08x\n%x\0",id&0x00ffffff,(char*)string,entry[i].id,entry[i].str,entry[i].buffer,(int)string);
                MessageBox(NULL,msg,"Message Found",MB_OK);
                #endif
				
                entry[i].buffer = (int)string;
                origbuffer = (int)string + 2;
                //strncpy(str+16,entry[i].str,entry[i].len);
                
                found = 1;
                break;
            }
		}
		
		if(found==0)
		{
			#if DEBUGLOG == 1
			{
				FILE *debuglog = fopen("debug.txt","a");
				fprintf(debuglog,"equivalent line not found\nOrig line: %s\n\n",(char*)string);
				fclose(debuglog);
			}
			#endif
			
            return (int)string;
			//strcpy(str+16,(char*)string);
			//MessageBox(NULL,str,"Message Not Found",MB_OK);
		}
        else
        {	
			#if DEBUGLOG == 1
			{
				FILE *debuglog = fopen("debug.txt","a");
				fprintf(debuglog,"line found: %08x\nOrig line: %s\nNew line: %s\n\n",(int)entry[i].str,(char*)string,entry[i].str);
				fclose(debuglog);
			}
			#endif
			
            return (int)entry[i].str;
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
    
    if(!infile)
    {
        if(errorDisp!=1)
        {
            errorDisp = 1;
            MessageBox(NULL,"Could not open data.ami","Error",MB_OK);
        }
        
        return;
    }
    
    fread(magic,1,4,infile);
    fread(&amicount,1,4,infile);
    fread(&baseOffset,1,4,infile);
    fseek(infile,4,SEEK_CUR);
    
    if(strncmp(magic,"AMI",3)!=0)
    {
        amt = -1;
        
        if(errorDisp!=1)
        {
            errorDisp = 1;
            MessageBox(NULL,"Invalid data.ami","Error",MB_OK);
        }
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
        "mov %1, %%eax; mov %0, %%edi": "=r"(positionAddress), "=m"(fileType)
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
        
        #if DEBUGIMG == 3
        FILE *imglog = fopen("img.txt","a+");
        fprintf(imglog,"%08x\n",positionAddress);
        fclose(imglog);
		#endif
        #if DEBUGIMG == 1
		sprintf(msg,"%08x",positionAddress);
		MessageBox(NULL,msg,"Debug",MB_OK);
        #endif
        
        addr = GetImageFromOffset(positionAddress);
        
        if(addr!=-1 && wroteImage!=1 && amientry[addr].size>0)
        {
            FILE *imgfile = fopen("data.ami","rb");
    
            if(!imgfile)
                return;
                
            if(imgentry[addr].read==0)
            {
                fseek(imgfile,amientry[addr].offset,SEEK_SET);
                imgentry[addr].bg = NULL;
                //imgentry[addr].read = 1;
            }
            
            if(amientry[addr].compsize!=0)
            {
                unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize,sizeof(unsigned char));
                unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size,sizeof(unsigned char));
                uLongf size = amientry[addr].size;
                
                fread(zbuffer,1,amientry[addr].compsize,imgfile);
                uncompress(buffer,&size,zbuffer,amientry[addr].compsize);
                free(zbuffer);
                
				#if DEBUGMSG == -1
				{
					int ii=0,xx=0;
					char hexbuff[512] = { 0 };
					int eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0;
					
					__asm("mov %0, %%eax": "=m"(eax));
					__asm("mov %0, %%ebx": "=m"(ebx));
					__asm("mov %0, %%ecx": "=m"(ecx));
					__asm("mov %0, %%edx": "=m"(edx));
					__asm("mov %0, %%esi": "=m"(esi));
					__asm("mov %0, %%edi": "=m"(edi));
					
					sprintf(msg,"Input buffer address: %08x\nOutput buffer address: %08x\n\nDump:\n",(buffer+sizeof(*imgentry)-12),(bufferAddressCG+0x20));
					sprintf(msg,"%s\nEAX: %08x\tEBX: %08x\nECX: %08x\tEDX: %08x\nESI: %08x\tEDI: %08x\n\n",msg,eax,ebx,ecx,edx,esi,edi);
					
					for(ii=0; ii<10; ii++)
					{
						sprintf(hexbuff,"%s%08x: ",hexbuff,(bufferAddressCG+(xx*ii)));
					
						for(xx=0; xx<16; xx++)
							sprintf(hexbuff,"%s %02x",hexbuff,*(unsigned char*)(bufferAddressCG+(xx*ii)));
						
						sprintf(msg,"%s%s\n",msg,hexbuff);
						memset(hexbuff,0,512);
					}
					
				}
				
				MessageBox(NULL,msg,"Debug",MB_OK);
				#endif
				
                //memcpy((char*)(bufferAddressCG+0x20),(char*)(buffer+sizeof(*imgentry)-12),size-12);
                memcpy((char*)bufferAddressCG,(char*)(buffer+sizeof(*imgentry)-12),size-12);
                
				#if DEBUGMSG == -1
				sprintf(msg,"Copied image\n");
				MessageBox(NULL,msg,"Debug",MB_OK);
				#endif
				
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

extern "C" _declspec(dllexport) void HookImageRGB(int resolution, int bytes, int buffer)
{
    int addr = 0;
    
    if(parsedImage==0)
        ParseImage();
    
    if(parsedImage==1 && amicount!=0)
    {
        char msg[256];
		int height = (resolution&0xffff0000)>>16, width = resolution&0x0000ffff;
		
        #if DEBUGIMG == 3
        FILE *imglog = fopen("img.txt","a+");
        fprintf(imglog,"%08x\n",positionAddress);
        fclose(imglog);
		#endif
		
        addr = GetImageFromOffset(positionAddress);
        
        if(addr!=-1 && wroteImage!=1 && amientry[addr].size>0)
        {
            FILE *imgfile = fopen("data.ami","rb");
			int bufferAddress = buffer-(width*(height-1)*4);
    
            if(!imgfile)
                return;
                
            if(imgentry[addr].read==0)
            {
                fseek(imgfile,amientry[addr].offset,SEEK_SET);
                imgentry[addr].bg = NULL;
            }
            
            if(amientry[addr].compsize!=0)
            {
                unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize,sizeof(unsigned char));
                unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size,sizeof(unsigned char));
                uLongf size = amientry[addr].size;
                
                fread(zbuffer,1,amientry[addr].compsize,imgfile);
                uncompress(buffer,&size,zbuffer,amientry[addr].compsize);
                free(zbuffer);
				
                memcpy((char*)bufferAddress,(char*)(buffer+sizeof(*imgentry)-12),size-12);
                memcpy(&imgentry[addr],buffer,sizeof(*imgentry)-12);
                
                free(buffer);
            }
            else
            {
                fread(&imgentry[addr],1,sizeof(*imgentry)-12,imgfile);
                fread((char*)bufferAddress,1,amientry[addr].size,imgfile);
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
    
    if(parsedImage==0)
    {
        ParseImage();
    }
    
    if(followBuffer == 1)
    {
        char msg[256];
        
        asm("mov %0, %%eax": "=m"(bufferAddressUI));
        
        for(i=0; i<amicount; i++)
        {
            if(amientry[i].file==positionAddress)
            {
                //sprintf(msg,"Address: %08x\nPosition address: %08x",bufferAddressUI,positionAddress);
                //MessageBox(NULL,msg,"Debug",MB_OK);
                
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
        asm("mov %0, %%si;" : "=m"(destWidth));
        asm("mov %0, %%di;" : "=m"(destHeight));
        asm("mov %0, %%ebx;" : "=m"(destWidth2));
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
		
        #if DEBUGIMG == 3
        FILE *imglog = fopen("img.txt","a+");
        fprintf(imglog,"%08x\n",positionAddress);
        fclose(imglog);
		#endif
		
		#if DEBUGIMG == 1
		sprintf(msg,"%08x",positionAddress);
		MessageBox(NULL,msg,"Debug",MB_OK);
        #endif
        
        if(parsedImage==1 && amicount!=0)
        {
            int addr = GetImageFromBuffer(srcBuffer);
			int bufferSize = 0;
                
            if(addr!=-1 && amientry[addr].size>0)
            {
                FILE *imgfile = fopen("data.ami","rb");
                unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size,sizeof(unsigned char));
                int j=0,bufferOffset=0;
                int off=0,z=0,i=0;
                int copyBuffer = 0, o = 0;
                
                if(!imgfile)
                    return 0;
            
				#if DEBUGIMG == 2
                sprintf(msg,"Mode 1\nDestination: %08x\nSource: %08x\nSize: %08x\0",destBuffer,srcBuffer,amientry[addr].size);
                MessageBox(NULL,msg,"Debug",MB_OK);
				#endif
                
                fseek(imgfile,amientry[addr].offset,SEEK_SET);
                
                if(amientry[addr].compsize!=0)
                {
                    unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize,sizeof(unsigned char));
                    uLongf compsize = amientry[addr].size;
                    
                    fread(zbuffer,1,amientry[addr].compsize,imgfile);
                    uncompress(buffer,&compsize,zbuffer,amientry[addr].compsize);
                    free(zbuffer);
                    memcpy(&imgentry[addr],buffer,sizeof(*imgentry)-12);
					buffer += 12;
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
                
                /*
                bufferOffset = destBuffer+((xpos-((w/4)*ypos))*4);
                bufferOffset &= 0xffff0000;
				
				bufferSize = ((resh+resw)*4)&0xfffff000;
				bufferSize += 0x1000;
				
				while(*(int*)(bufferOffset)!=bufferSize)
                    bufferOffset-=0x10;
                bufferOffset += 0x20;
				
				for(;;)
				{
					if(*(int*)(bufferOffset+0x10)!=0 && *(int*)(bufferOffset+0x10)==*(int*)(bufferOffset+0x14) && *(int*)(bufferOffset+0x1c)==0x04000000)
					{
						bufferOffset += 0x20;
						break;
					}
					else if(*(short*)(bufferOffset+0x06)==0x0101 && *(int*)(bufferOffset+0x08)==0xffeeffee && *(int*)(bufferOffset+0x0C)==0x00000000)
					{
						sprintf(msg,"%dx%d (%xx%x) [%08x]",destWidth,destHeight,destWidth,destHeight,destWidth*destHeight*4);
						MessageBox(NULL,msg,"Debug",MB_OK);
						bufferOffset += 0x40;
						break;
					}
					else
					{
						bufferOffset-=0x10000;
					}
				}
				*/
				
				bufferOffset = destBuffer-(destWidth*(destHeight-1)*4);
				resh = destHeight*4;
				resw = destWidth*4;
				
                if((int*)bufferOffset==NULL)
                {
                    free(buffer);
                    return 0;
                }
                
                if(imgentry[addr].bg==NULL)
                {
                    imgentry[addr].bg = (unsigned char*)calloc(destWidth*destHeight*4,sizeof(unsigned char));
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
                            unsigned char r=0, g=0, b=0, a=0;
                            unsigned char fr=0, fg=0, fb=0, fa=0;
                            
                            if(copyBuffer == 1)
                            {
                                imgentry[addr].bg[o] = *(unsigned char*)(bufferOffset+off+pos+i);
                                imgentry[addr].bg[o+1] = *(unsigned char*)(bufferOffset+off+pos+i+1);
                                imgentry[addr].bg[o+2] = *(unsigned char*)(bufferOffset+off+pos+i+2);
                                imgentry[addr].bg[o+3] = *(unsigned char*)(bufferOffset+off+pos+i+3);
                            }
                            else
                            {
                                *(unsigned char*)(bufferOffset+off+pos+i) = imgentry[addr].bg[o];
                                *(unsigned char*)(bufferOffset+off+pos+i+1) = imgentry[addr].bg[o+1];
                                *(unsigned char*)(bufferOffset+off+pos+i+2) = imgentry[addr].bg[o+2];
                                *(unsigned char*)(bufferOffset+off+pos+i+3) = imgentry[addr].bg[o+3];
                            }
                            o += 4;
                            
                            b = *(unsigned char*)(bufferOffset+off+pos+i);
                            g = *(unsigned char*)(bufferOffset+off+pos+i+1);
                            r = *(unsigned char*)(bufferOffset+off+pos+i+2);
                            a = *(unsigned char*)(bufferOffset+off+pos+i+3);
                            
                            
                            fb = buffer[i+j];
                            fg = buffer[i+j+1];
                            fr = buffer[i+j+2];
                            fa = buffer[i+j+3];
                            	
                            *(unsigned char*)(bufferOffset+pos+off+i) = (unsigned char)((float)b + (float)(fb-b) * (float)((float)fa/(float)255.0f));
                            *(unsigned char*)(bufferOffset+pos+off+i+1) = (unsigned char)((float)g + (float)(fg-g) * (float)((float)fa/(float)255.0f));
                            *(unsigned char*)(bufferOffset+pos+off+i+2) = (unsigned char)((float)r + (float)(fr-r) * (float)((float)fa/(float)255.0f));
							*(unsigned char*)(bufferOffset+pos+off+i+3) = (fa/2)&0x7f;
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
