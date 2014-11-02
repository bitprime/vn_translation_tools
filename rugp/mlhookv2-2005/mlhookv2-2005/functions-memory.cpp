#include "functions.h"

int followBuffer = 0; // should the buffer be followed into HookMalloc?

void StoreBufferStruct()
{
	followBuffer = 1;
}

void StorePositionAddress()
{
	__asm
	{
		mov positionAddress, edi;
		mov filetype, eax; 
	};

	wroteImage = 0;
}

void HookMalloc()
{	
	if(parsedImage == 0)
		ParseImage();
	
	if(followBuffer == 1)
	{   
		int i = 0;

		__asm
		{
			mov bufferAddressUI, eax;
		}
		
		for(i=0; i<amicount; i++)
		{
			if(amientry[i].file == positionAddress)
			{
				imgentry[i].buffer = bufferAddressUI;
				break;
			}
			else if(imgentry[i].buffer == bufferAddressUI)
			{
				imgentry[i].buffer = 0;
			}
		}
		
		followBuffer = 0;
	}
}

void StoreBufferAddress()
{
	__asm
	{
		mov bufferAddressCG, eax;
	};
}
