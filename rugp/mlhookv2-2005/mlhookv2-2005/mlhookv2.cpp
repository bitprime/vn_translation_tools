#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "patches.h"

#define MODFILENAME "mlhookv2.rpo"
	#define MODVER "1.00.00"
#if GAME == MUVLUVEXUL
	#define MODDESC "Built for Muv-Luv DVD-ROM Ver. (5.60.10)"
#elif GAME == MUVLUVALT
	#define MODDESC "Built for Muv-Luv Alternative (5.70.18)"
#elif GAME == KIMINOZOLE
	#define MODDESC "Built for Kimi ga Nozomu Eien Latest Edition (5.81.03)"
#endif

#if LOGCHOICES != 0 || DUMPMEM != 0 || DEBUG != 0 || DEBUGIMG != 0 || SKIPTEXT != 0 || DEBUGIDS != 0 || SELFFIX != 0
	#define MODDEBUGSTR "\r\n\t!!DEBUG MODE!! DO NOT RELEASE"
#else
	#define MODDEBUGSTR ""
#endif

#define MODCOPY ""
#define VERSTRING MODFILENAME" Version "MODVER"\r\n\t"MODDESC MODDEBUGSTR"\r\n\t"MODCOPY"\r\n\r\n"

extern "C" _declspec(dllexport) void PluginThisLibrary(void)
{	
	PatchDLL();
}

extern "C" _declspec(dllexport) char *GetPluginString(void)
{	
	return VERSTRING;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	return true;
}
