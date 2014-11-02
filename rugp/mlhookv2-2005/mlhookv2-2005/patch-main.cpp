#include <windows.h>
#include <stdio.h>
#include "patches.h"

/*
	The struct that determines what gets patched and how it gets patched.
	If a patch is not present in this struct then it will not get applied.
*/
Patch pdata[] = {
	{ "mlhookv2.rpo", 0x1000, NULL, 0, NULL, SKIP }, // unprotect the DLL so we can do some self-editing from the hook code

	#if GAME == MUVLUVEXUL
	{ "rvmm.dll", 0x3f248, (char*)GfxPatch_GetImageSize, GetPatchLength((char*)GfxPatch_GetImageSize), NULL, JMP },
	{ "rvmm.dll", 0x6b638, (char*)MemPatch_StoreBufferStruct, GetPatchLength((char*)MemPatch_StoreBufferStruct), NULL, JMP },
	{ "rvmm.dll", 0x6dad4, (char*)GfxPatch_HookImage, GetPatchLength((char*)GfxPatch_HookImage), NULL, JMP },
	{ "rvmm.dll", 0x6dbf9, (char*)GfxPatch_HookImageUI, GetPatchLength((char*)GfxPatch_HookImageUI), NULL, JMP },
	
	{ "UnivUI.dll", 0x140bc, (char*)MemPatch_StorePositionAddress1, GetPatchLength((char*)MemPatch_StorePositionAddress1), NULL, JMP },
	{ "UnivUI.dll", 0x141e9, (char*)MemPatch_StorePositionAddress2, GetPatchLength((char*)MemPatch_StorePositionAddress2), NULL, JMP },
	{ "UnivUI.dll", 0x280fd, (char*)MemPatch_HookMalloc, GetPatchLength((char*)MemPatch_HookMalloc), NULL, JMP },
	{ "UnivUI.dll", 0x32fda, (char*)MemPatch_StoreBufferAddress, GetPatchLength((char*)MemPatch_StoreBufferAddress), NULL, JMP },
	{ "UnivUI.dll", 0x3b72b, (char*)GfxPatch_HookImageRGB, GetPatchLength((char*)GfxPatch_HookImageRGB), NULL, JMP },
	
	{ "Vm60.dll", 0x9f09, (char*)TextPatch_nTextSpeed1, GetPatchLength((char*)TextPatch_nTextSpeed1), NULL, PATCH },
	{ "Vm60.dll", 0xa1ac, (char*)TextPatch_arraySerifSetting, GetPatchLength((char*)TextPatch_arraySerifSetting), NULL, JMP },
	{ "Vm60.dll", 0xfbf8, (char*)TextPatch_HookMain, GetPatchLength((char*)TextPatch_HookMain), NULL, JMP },
	{ "Vm60.dll", 0x1905a, (char*)TextPatch_nTextSpeed2, GetPatchLength((char*)TextPatch_nTextSpeed2), NULL, PATCH },
	{ "Vm60.dll", 0x60044, (char*)TextPatch_DumpText, GetPatchLength((char*)TextPatch_DumpText), NULL, JMP },

	// resources
	{ "GMfc.dll", 0x23000, (char*)gmfc_data, gmfc_length, NULL, DATA },

	{ "rvmm.dll", 0xe6000, (char*)rvmm_data, rvmm_length, NULL, DATA },
	{ "rvmm.dll", 0xcd190, (char*)rvmm2_data, rvmm2_length, NULL, DATA },

	{ "Vm60.dll", 0xae000, (char*)vm60_data, vm60_length, NULL, DATA },
	{ "Vm60.dll", 0xab9b0, (char*)vm603_data, vm603_length, NULL, DATA },
	{ "Vm60.dll", 0xa1338, (char*)vm602_data, vm602_length, NULL, DATA },
	{ "Vm60.dll", 0xa1470, (char*)vm604_data, vm604_length, NULL, DATA },
	{ "Vm60.dll", 0xa1b20, (char*)vm605_data, vm605_length, NULL, DATA },
	{ "Vm60.dll", 0xa1e18, (char*)vm608_data, vm608_length, NULL, DATA },
	{ "Vm60.dll", 0xa1ef0, (char*)vm606_data, vm606_length, NULL, DATA },
	{ "Vm60.dll", 0xa2330, (char*)vm607_data, vm607_length, NULL, DATA },
	{ "Vm60.dll", 0xa44b0, (char*)vm609_data, vm609_length, NULL, DATA },
	{ "Vm60.dll", 0x3582, (char*)vm60_rsrcpatch1, GetPatchLength((char*)vm60_rsrcpatch1), NULL, JMP },
	{ "Vm60.dll", 0xc096, (char*)vm60_rsrcpatch2, GetPatchLength((char*)vm60_rsrcpatch2), NULL, JMP },
	{ "Vm60.dll", 0xc0fd, (char*)vm60_rsrcpatch3, GetPatchLength((char*)vm60_rsrcpatch3), NULL, JMP },
	{ "Vm60.dll", 0xc10f, (char*)vm60_rsrcpatch4, GetPatchLength((char*)vm60_rsrcpatch4), NULL, JMP },
	{ "Vm60.dll", 0xc14d, (char*)vm60_rsrcpatch5, GetPatchLength((char*)vm60_rsrcpatch5), NULL, JMP },
	#elif GAME == MUVLUVALT
	{ "rvmm.dll", 0x27ab8, (char*)GfxPatch_GetImageSize, GetPatchLength((char*)GfxPatch_GetImageSize), NULL, JMP },
	{ "rvmm.dll", 0x2e92a, (char*)GfxPatch_GetImageSize2, GetPatchLength((char*)GfxPatch_GetImageSize2), NULL, JMP },
	//{ "rvmm.dll", 0x65479, (char*)GfxPatch_HookImage, GetPatchLength((char*)GfxPatch_HookImage), NULL, JMP },
	{ "rvmm.dll", 0x65837, (char*)GfxPatch_HookImageUI, GetPatchLength((char*)GfxPatch_HookImageUI), NULL, JMP },
	{ "rvmm.dll", 0x6549D, (char*)GfxPatch_HookImage2, GetPatchLength((char*)GfxPatch_HookImage2), NULL, JMP },
	{ "rvmm.dll", 0x5EE93, (char*)GfxPatch_HookImage3, GetPatchLength((char*)GfxPatch_HookImage3), NULL, JMP },
	//{ "rvmm.dll", 0x5E948, (char*)MemPatch_StoreBufferStruct, GetPatchLength((char*)MemPatch_StoreBufferStruct), NULL, JMP },
	{ "rvmm.dll", 0x65270, (char*)MemPatch_StoreBufferStruct, GetPatchLength((char*)MemPatch_StoreBufferStruct), NULL, JMP },
	
	{ "UnivUI.dll", 0xF850, (char*)MemPatch_StorePositionAddress1, GetPatchLength((char*)MemPatch_StorePositionAddress1), NULL, JMP },
	{ "UnivUI.dll", 0xF99A, (char*)MemPatch_StorePositionAddress2, GetPatchLength((char*)MemPatch_StorePositionAddress2), NULL, JMP },
	{ "UnivUI.dll", 0xF35B, (char*)MemPatch_StorePositionAddress3, GetPatchLength((char*)MemPatch_StorePositionAddress3), NULL, JMP },
	{ "UnivUI.dll", 0x249FD, (char*)MemPatch_HookMalloc, GetPatchLength((char*)MemPatch_HookMalloc), NULL, JMP },
	{ "UnivUI.dll", 0x2FEAA, (char*)MemPatch_StoreBufferAddress, GetPatchLength((char*)MemPatch_StoreBufferAddress), NULL, JMP },
	{ "UnivUI.dll", 0x37FDB, (char*)GfxPatch_HookImageRGB, GetPatchLength((char*)GfxPatch_HookImageRGB), NULL, JMP },
	
	{ "Vm60.dll", 0x12438, (char*)TextPatch_HookMain, GetPatchLength((char*)TextPatch_HookMain), NULL, JMP },
	{ "Vm60.dll", 0xa51c, (char*)TextPatch_arraySerifSetting, GetPatchLength((char*)TextPatch_arraySerifSetting), NULL, JMP },
	{ "Vm60.dll", 0x5711F, (char*)TextPatch_DumpText, GetPatchLength((char*)TextPatch_DumpText), NULL, JMP },
	{ "Vm60.dll", 0xB54D, (char*)TextPatch_FixVoiceNames, GetPatchLength((char*)TextPatch_FixVoiceNames), NULL, JMP },
	{ "Vm60.dll", 0xB98E, (char*)TextPatch_FixVoiceNames2, GetPatchLength((char*)TextPatch_FixVoiceNames2), NULL, JMP },

	{ "GMfc.dll", 0x23000, (char*)gmfc_alt_data, gmfc_alt_length, NULL, DATA },

	{ "rvmm.dll", 0xb5000, (char*)rvmm_alt_data, rvmm_alt_length, NULL, DATA },
	{ "rvmm.dll", 0x85058, (char*)rvmm2_alt_data, rvmm2_alt_length, NULL, DATA },

	{ "Vm60.dll", 0x8B000, (char*)vm60_alt_data, vm60_alt_length, NULL, DATA },
	{ "Vm60.dll", 0x87EC8, (char*)vm603_data, vm603_length, NULL, DATA },
	{ "Vm60.dll", 0x80478, (char*)vm602_data, vm602_length, NULL, DATA },
	{ "Vm60.dll", 0x805B4, (char*)vm604_data, vm604_length, NULL, DATA },
	{ "Vm60.dll", 0x80C68, (char*)vm605_data, vm605_length, NULL, DATA },
	{ "Vm60.dll", 0x80FC0, (char*)vm608_data, vm608_length, NULL, DATA },
	{ "Vm60.dll", 0x810B0, (char*)vm606_data, vm606_length, NULL, DATA },
	{ "Vm60.dll", 0x81640, (char*)vm607_alt_data, vm607_alt_length, NULL, DATA },
	{ "Vm60.dll", 0x83BE0, (char*)vm609_data, vm609_length, NULL, DATA },
	
	{ "Vm60.dll", 0x1294b, (char*)vm60_nametagfix, 1, NULL, DATA },
	{ "Vm60.dll", 0x12960, (char*)vm60_nametagfix, 1, NULL, DATA },

	{ "Vm60.dll", 0xdE77, (char*)vm60_rsrcpatch1, GetPatchLength((char*)vm60_rsrcpatch1), NULL, JMP },
	{ "Vm60.dll", 0xdE34, (char*)vm60_rsrcpatch2, GetPatchLength((char*)vm60_rsrcpatch2), NULL, JMP },
	#elif GAME == KIMINOZOLE
	{ "rvmm.dll", 0x2c9a9, (char*)GfxPatch_GetImageSize, GetPatchLength((char*)GfxPatch_GetImageSize), NULL, JMP },
	{ "rvmm.dll", 0x69868, (char*)MemPatch_StoreBufferStruct, GetPatchLength((char*)MemPatch_StoreBufferStruct), NULL, JMP },
	{ "rvmm.dll", 0x6bfe3, (char*)GfxPatch_HookImage, GetPatchLength((char*)GfxPatch_HookImage), NULL, JMP },
	{ "rvmm.dll", 0x6c10e, (char*)GfxPatch_HookImageUI, GetPatchLength((char*)GfxPatch_HookImageUI), NULL, JMP },
	
	{ "UnivUI.dll", 0x103b0, (char*)MemPatch_StorePositionAddress1, GetPatchLength((char*)MemPatch_StorePositionAddress1), NULL, JMP },
	{ "UnivUI.dll", 0x104fa, (char*)MemPatch_StorePositionAddress2, GetPatchLength((char*)MemPatch_StorePositionAddress2), NULL, JMP },
	{ "UnivUI.dll", 0x27e1d, (char*)MemPatch_HookMalloc, GetPatchLength((char*)MemPatch_HookMalloc), NULL, JMP },
	{ "UnivUI.dll", 0x338ca, (char*)MemPatch_StoreBufferAddress, GetPatchLength((char*)MemPatch_StoreBufferAddress), NULL, JMP },
	{ "UnivUI.dll", 0x3bd3b, (char*)GfxPatch_HookImageRGB, GetPatchLength((char*)GfxPatch_HookImageRGB), NULL, JMP },
	
	//{ "Vm60.dll", 0xb57f, (char*)TextPatch_nTextSpeed1, GetPatchLength((char*)TextPatch_nTextSpeed1), NULL, PATCH },
	{ "Vm60.dll", 0xb84c, (char*)TextPatch_arraySerifSetting, GetPatchLength((char*)TextPatch_arraySerifSetting), NULL, JMP },
	{ "Vm60.dll", 0x14388, (char*)TextPatch_HookMain, GetPatchLength((char*)TextPatch_HookMain), NULL, JMP },
	//{ "Vm60.dll", 0x232ad, (char*)TextPatch_nTextSpeed2, GetPatchLength((char*)TextPatch_nTextSpeed2), NULL, PATCH },
	//{ "Vm60.dll", 0x232c8, (char*)TextPatch_nTextSpeed2, GetPatchLength((char*)TextPatch_nTextSpeed2), NULL, PATCH },
	{ "Vm60.dll", 0x6e474, (char*)TextPatch_DumpText, GetPatchLength((char*)TextPatch_DumpText), NULL, JMP },
	#endif

	NULL
};

/*
	The actual patching function. The general idea is to copy
	naked functions as patches from mlhook into the game's DLLs.
*/
void PatchDLL()
{
	for(int i=0; pdata[i].dll!=NULL; i++)
	{
		HMODULE Module = NULL;
		MEMORY_BASIC_INFORMATION MemoryInfo;

		Module = (HMODULE)((int)(GetModuleHandle(pdata[i].dll))+(int)(pdata[i].offset)); // skip to the first section
			
		if(VirtualQuery(Module, &MemoryInfo, sizeof(MEMORY_BASIC_INFORMATION)))
		{
			if(VirtualProtect(MemoryInfo.BaseAddress, MemoryInfo.RegionSize, PAGE_EXECUTE_READWRITE, &MemoryInfo.Protect))
			{	
				if(pdata[i].type == PATCH)
				{
					pdata[i].buffer = (unsigned char*)calloc(pdata[i].len,sizeof(unsigned char));
					pdata[i].len = pdata[i].len; // store the result of GetPatchLength into the field instead of calling it repeatedly
	
					memcpy(pdata[i].buffer, Module, pdata[i].len);
					memcpy(Module, pdata[i].func, pdata[i].len);
				}
				else if(pdata[i].type == JMP)
				{
					int offset = (int)Module;

					pdata[i].buffer = (unsigned char*)calloc(13,sizeof(unsigned char));
					pdata[i].len = 10;

					memcpy(pdata[i].buffer, Module, pdata[i].len);
					memset((char*)offset, '\x68', 1); // PUSH ...
					offset++;
					*(unsigned int*)offset = (int)GetModuleHandle(pdata[i].dll);
					offset += 4;
					memset((char*)offset, '\xe9', 1); // JMP ...
					offset++;
					*(unsigned int*)offset = (int)pdata[i].func-offset-4;
					offset += 4;
				}
				else if(pdata[i].type == DATA)
				{
					int offset = (int)Module;

					pdata[i].buffer = (unsigned char*)calloc(pdata[i].len,sizeof(unsigned char));
					
					memcpy(pdata[i].buffer, Module, pdata[i].len);
					memcpy(Module, pdata[i].func, pdata[i].len);
				}

				if(pdata[i].type != SKIP)
					VirtualProtect(MemoryInfo.BaseAddress, MemoryInfo.RegionSize, MemoryInfo.Protect, NULL);
			}
		}
	}
}

/*
	This function gets the real length of an input patch function.
	All patch functions should end in "int 0FFh int 0FFh", which is
	the ending used for searching.
*/
int GetPatchLength(char *PatchBuffer)
{
	int i=0;
	
	while(*(int*)(PatchBuffer+i) != 0xffcdffcd)
		i++;
		
	return i;
}
