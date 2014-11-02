#include <zlib.h>
#include "functions.h"
#include "functions-graphic.h"

int GetImageFromOffset(int);
int GetImageFromBuffer(int);


FILE *datafile = NULL;

int parsedImage = 0; // were the images parsed?
int bufferAddressCG = 0; // current buffer address for images
int bufferAddressUI = 0; // current buffer address for transparent images
int wroteImage = 0; // was the current image written already?
int srcBuffer = 0, destBuffer = 0; // source and destination image buffers
short destWidth = 0, destHeight = 0; // destination buffer's dimensions

void ParseImage()
{
	if(parsed==0)
		ParseArchive();
	
	imgentry = (ImageHeader*)calloc(amicount + 1, sizeof(*imgentry));
	parsedImage = 1;
}

int HookImage(int null, int res, int buffaddr)
{
	volatile int addr = -1;
	int rsize = 0;
	short w = 0, h = 0;

	w = res&0xffff;
	h = (res&0xffff0000)>>16;
	rsize = w*h*4;
	
	if(parsedImage == 0)
		ParseImage();
	
	if(parsedImage == 1 && amicount != 0)
	{		
		#if DEBUGIMG == 3
		FILE *imglog = fopen("img.txt", "a+");
		fprintf(imglog, "%08x\n", positionAddress);
		fclose(imglog);
		#endif
		
		addr = GetImageFromOffset(positionAddress);
		
		#if DEBUGIMG == 3
		{
			FILE *imglog = fopen("img.txt", "a+");
			fprintf(imglog, "found 1 %08x\n", addr);
			fclose(imglog);
		}
		#endif

		if(buffaddr != bufferAddressCG)
			wroteImage = 0;

		if(addr == -1)
			return false;

		if(wroteImage != 1 && amientry[addr].size > 0)
		{
			int buffersize = 0;
	
			if(!datafile)
			{
				datafile = fopen("data.ami","rb");

				if(!datafile)
					return false;
			}

			if(imgentry[addr].trans == 0)
			{
				fseek(datafile, amientry[addr].offset, SEEK_SET);
				imgentry[addr].bg = NULL;
				
				#if DEBUGIMG == 3
				{
					FILE *imglog = fopen("img.txt", "a+");
					fprintf(imglog, "(%08x) seeked to %08x (%08x)\n", amientry[addr].file, amientry[addr].offset, ftell(datafile));
					fclose(imglog);
				}
				#endif
			}
			else
			{
				/*
				{
					FILE *imglog = fopen("img.txt", "a+");
					fprintf(imglog, "(%08x) seeked to %08x (%08x) %08x\n", amientry[addr].file, amientry[addr].offset, ftell(datafile), destBuffer);
					fclose(imglog);
				}
				*/

				//destBuffer = buffaddr;
				//WriteTransparency(addr, 1);

				return false;
			}
			
			if(amientry[addr].compsize != 0)
			{
				unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize, sizeof(unsigned char));
				unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size, sizeof(unsigned char));
				uLongf size = amientry[addr].size;
				int retval = 0;
				
				fread(zbuffer, 1, amientry[addr].compsize, datafile);
				retval = uncompress(buffer, &size, zbuffer, amientry[addr].compsize);
				free(zbuffer);

				memcpy(&imgentry[addr], buffer, sizeof(*imgentry)-12);
				buffaddr -= rsize;
				buffersize = *(int*)(buffaddr-12);
	
				if(*(int*)(buffaddr-16)>buffersize)
					buffersize = *(int*)(buffaddr-16);

				if(buffersize!=0 && buffersize>size-12)
					memcpy((char*)buffaddr, buffer+sizeof(*imgentry)-12, size-12);
				else
					memcpy((char*)buffaddr, buffer+sizeof(*imgentry)-12, ((buffersize/4)/WIDTH)*WIDTH*4);
				
				free(buffer);
			}
			else
			{
				fread(&imgentry[addr], 1, sizeof(*imgentry)-12, datafile);

				buffaddr = buffaddr-((imgentry[addr].w-1)*imgentry[addr].h*4);
				buffersize = *(int*)(buffaddr-12);
	
				if(*(int*)(buffaddr-16)>buffersize)
					buffersize = *(int*)(buffaddr-16);

				if(buffersize!=0 && buffersize>amientry[addr].size)
					fread((char*)buffaddr, 1, amientry[addr].size, datafile);
				else
					fread((char*)buffaddr, 1, ((buffersize/4)/WIDTH)*WIDTH*4, datafile);
			}
			
			wroteImage = 1;
			return true;
		}
	}

	return false;
}

int HookImageUI()
{
	int mode = 0;

	//MessageBox(NULL, "Test", "Debug", MB_OK);

	__asm
	{
		mov mode, eax;
	};
	
	if(mode == 0)
	{
		char msg[256];

		__asm
		{
			pop edi;
			mov destHeight, di;
			pop esi;
			mov destWidth, si;
			push edi;
			push esi;
#if GAME == MUVLUVEXALT
			mov srcBuffer, ecx;
			mov destBuffer, edx;
#elif GAME == MUVLUVALT
			push eax;
			mov eax, [esp+0x30];
			mov srcBuffer, eax;
			pop eax;
			mov destBuffer, ebx;
#endif

		};

		if(srcBuffer != 0 && imgentry[GetImageFromBuffer(srcBuffer)].buffer == srcBuffer)
			return 1;
	}
	else if(mode == 1)
	{
		if(parsedImage == 0)
			ParseImage();
		
		#if DEBUGIMG == 3
		FILE *imglog = fopen("img.txt", "a+");
		fprintf(imglog, "%08x\n", positionAddress);
		fclose(imglog);
		#endif
		
		#if DEBUGIMG == 1
		sprintf(msg, "%08x", positionAddress);
		MessageBox(NULL, msg, "Debug", MB_OK);
		#endif

		if(parsedImage == 1 && amicount != 0)
		{
			int addr = GetImageFromBuffer(srcBuffer);
			int bufferSize = 0;
		
			#if DEBUGIMG == 3
			{
				FILE *imglog = fopen("img.txt", "a+");
				fprintf(imglog, "found 2 %08x\n", addr);
				fclose(imglog);
			}
			#endif
				
			imgentry[addr].trans = 1;
			WriteTransparency(addr, 0);
		}
	}
	
	return 0;
}

void HookImageRGB(int resolution, int bytes, int buffer)
{
	int addr = 0;

	if(parsedImage == 0)
		ParseImage();
	
	if(parsedImage == 1 && amicount != 0)
	{
		char msg[256];
		int width = (resolution & 0xffff0000) >> 16, height = resolution & 0x0000ffff;
		
		#if DEBUGIMG == 3
		FILE *imglog = fopen("img.txt", "a+");
		fprintf(imglog, "%08x\n", positionAddress);
		fclose(imglog);
		#endif
		
		addr = GetImageFromOffset(positionAddress);
		
		#if DEBUGIMG == 3
		{
			FILE *imglog = fopen("img.txt", "a+");
			fprintf(imglog, "found 3 %08x\n", addr);
			fclose(imglog);
		}
		#endif

		if(addr != -1 && wroteImage != 1 && amientry[addr].size > 0)
		{
			int bufferAddress = buffer - (width * (height - 1) * 4);
	
			if(!datafile)
			{
				datafile = fopen("data.ami","rb");

				if(!datafile)
					return;
			}
				
			fseek(datafile, amientry[addr].offset, SEEK_SET);
			imgentry[addr].bg = NULL;
			
			if(amientry[addr].compsize != 0)
			{
				unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize, sizeof(unsigned char));
				unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size, sizeof(unsigned char));
				uLongf size = amientry[addr].size;
				
				fread(zbuffer, 1, amientry[addr].compsize, datafile);
				uncompress(buffer, &size, zbuffer, amientry[addr].compsize);
				free(zbuffer);

				memcpy((char*)bufferAddress, (char*)(buffer + sizeof(*imgentry) - 12), size - 12);
				memcpy(&imgentry[addr], buffer, sizeof(*imgentry) - 12);
				
				free(buffer);
			}
			else
			{
				fread(&imgentry[addr], 1, sizeof(*imgentry) - 12, datafile);
				fread((char*)bufferAddress, 1, amientry[addr].size, datafile);
			}
			
			wroteImage = 1;
		}
	}
}

int GetImageFromOffset(int addr)
{
	int i = 0;	  
	
	for(i = 0; i < amicount; i++)
		if(amientry[i].file == addr)
			return i;
	
	return -1;
}

int GetImageFromBuffer(int srcBuffer)
{
	int i = 0;
	
	for(i = 0; i < amicount; i++)
	{
		if((int)imgentry[i].buffer == srcBuffer)
			return i;
	}
	
	return -1;
}


int WriteTransparency(int addr, int frompos)
{
	if(addr != -1 && amientry[addr].size > 0)
	{
		unsigned char *buffer = (unsigned char*)calloc(amientry[addr].size, sizeof(unsigned char));
		int i = 0, j = 0, z = 0;
		int o = 0, off = 0, bufferOffset = 0, copyBuffer = 0;
		int w = 0, resw = 0, h = 0, resh = 0;
		int xpos = 0, ypos = 0;
				
		if(!datafile)
		{
			datafile = fopen("data.ami","rb");
	
			if(!datafile)
				return 0;
		}
				
		fseek(datafile, amientry[addr].offset, SEEK_SET);
				
		if(amientry[addr].compsize != 0)
		{
			unsigned char *zbuffer = (unsigned char*)calloc(amientry[addr].compsize, sizeof(unsigned char));
			uLongf compsize = amientry[addr].size;
			
			fread(zbuffer, 1, amientry[addr].compsize, datafile);
			uncompress(buffer, &compsize, zbuffer, amientry[addr].compsize);
			free(zbuffer);

			memcpy(&imgentry[addr], buffer, sizeof(*imgentry) - 12);
			buffer += 12;
		}
		else
		{
			fread(&imgentry[addr], 1, sizeof(*imgentry) - 12, datafile);
			fread(buffer, 1, amientry[addr].size, datafile);
		}
		
		w = imgentry[addr].w;
		h = imgentry[addr].h;
		xpos = imgentry[addr].x;
		ypos = imgentry[addr].y;
		
		if(!frompos)
			bufferOffset = destBuffer - (destWidth * (destHeight - 1) * 4); // destWidth and destHeight not properly set in crash

		resh = destHeight * 4;
		resw = destWidth * 4;
		
		if((int*)bufferOffset == NULL)
		{
			free(buffer);
			return 0;
		}
		
		if(imgentry[addr].bg == NULL)
		{
			imgentry[addr].bg = (unsigned char*)calloc(destWidth * destHeight * 4, sizeof(unsigned char));
			copyBuffer = 1;
		}
				
		/*
			Alpha blending method: 
			
			N[A] = S[A];
			N[R] = S[R] + (D[R]-S[R]) * ((float)D[A]/255);
			N[G] = S[G] + (D[G]-S[G]) * ((float)D[A]/255);
			N[B] = S[B] + (D[B]-S[B]) * ((float)D[A]/255);
		*/

		for(z = 0; z < h; z++)
		{
			int pos = xpos * 4 + ((resw / 4) * (resh / 4) * 4 - (ypos + h) * resw);
			
			for(i = 0; i < w * 4; i += 4)
			{
				{
					unsigned char r = 0, g = 0, b = 0, a = 0;
					unsigned char fr = 0, fg = 0, fb = 0, fa = 0;
					
					if(imgentry[addr].bg!=NULL)
					{
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
						
					if(
#if GAME == MUVLUVALT
						1 // because the game does not clear the buffer for transparent images like EX/UL, we're just going to write the data directly onto the surface
#else
						*(int*)(bufferOffset+off+pos+i)==0 && fa!=0
#endif
						)
					{
						*(unsigned char*)(bufferOffset+pos+off+i) = fb;
						*(unsigned char*)(bufferOffset+pos+off+i+1) = fg;
						*(unsigned char*)(bufferOffset+pos+off+i+2) = fr;
						*(unsigned char*)(bufferOffset+pos+off+i+3) = (unsigned char)(fa/2)&0x7f;
					}
#if GAME == MUVLUVALT
					else if(fa==0) // the current pixel is supposed to be blank in both places. i don't want to possibly introduce problems in other versions so it's restricted to alt for the time being
					{
						*(unsigned char*)(bufferOffset+pos+off+i) = 0;
						*(unsigned char*)(bufferOffset+pos+off+i+1) = 0;
						*(unsigned char*)(bufferOffset+pos+off+i+2) = 0;
						*(unsigned char*)(bufferOffset+pos+off+i+3) = 0;
					}
#endif
					else
					{
						*(unsigned char*)(bufferOffset+pos+off+i) = (unsigned char)((float)b + (float)(fb-b) * (float)((float)fa/(float)255.0f));
						*(unsigned char*)(bufferOffset+pos+off+i+1) = (unsigned char)((float)g + (float)(fg-g) * (float)((float)fa/(float)255.0f));
						*(unsigned char*)(bufferOffset+pos+off+i+2) = (unsigned char)((float)r + (float)(fr-r) * (float)((float)fa/(float)255.0f));
						*(unsigned char*)(bufferOffset+pos+off+i+3) = (unsigned char)(fa/2)&0x7f;
					}
				}
			}
				 
			j += w * 4;
			off += resw;
		}
	
		free(buffer);
	}

	return 0;
}
