#include "patches-memory.h"

#if GAME == MUVLUVEXUL
NAKED void MemPatch_StoreBufferStruct()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6b643;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x83A90;
		
		pushad;
		call StoreBufferStruct;
		popad;

		mov ecx, edi;
		push edx;
label2:
		call dword ptr ds:[0];
		mov ecx, edi;
		
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StorePositionAddress1()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x40630;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x140C8;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		mov eax, 1;
		call StorePositionAddress;
		popad;

		push edi;

		push callexit;
label1:
		push 0x10000000;
		retn;

callexit:
		mov ecx, [esi+0x24];
		mov edx, [ecx+0x10];
label2:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StorePositionAddress2()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x141f4;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		mov eax, 2;
		mov edi, ebx;
		call StorePositionAddress;
		popad;

		push 0;
		push ebx;
		mov edx, [ecx];
		call [edx+0x30];
		mov ebx, [edi+0x40];

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_HookMalloc()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x28111;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x28107;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		call HookMalloc;
		popad;

		mov [esi+4], eax;
		test eax, eax;
		jne skipjump;

label1:
		push 0x10000000;
		retn;

skipjump:
		mov [esi+8], edi;

label2:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StoreBufferAddress()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x32fe4;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		call StoreBufferAddress;
		popad;

		mov [esi+0x14], eax;
		pop edi;
		mov eax, 1;
		pop esi;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}
#elif GAME == MUVLUVALT
NAKED void MemPatch_StoreBufferStruct()
{
	__asm
	{	
		//LEA EDI,DWORD PTR DS:[EDX+EDX+2]
		lea edi, [edx+edx+2];

		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		//add dword ptr [ebx+1], 0x5e953;
		add dword ptr [ebx+1], 0x6527D;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x759B8;
		pop ebx;
		pop eax;
		
		pushad;
		call StoreBufferStruct;
		popad;

//		mov ecx, edi;
//		push edx;
		push edi;
label2:
		call dword ptr ds:[0];
//		mov ecx, edi;
		mov ecx, ebp;
		
//		pop ebx; // moved up to the top of the function
//		pop eax;
		mov [esp], eax;
		pop eax;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StorePositionAddress1()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3D452;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xF85A;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		mov edi, ebx;
		mov eax, 1;
		call StorePositionAddress;
		popad;

		push ebx;

		push callexit;
label1:
		push 0x10000000;
		retn;

callexit:
		mov ecx, [esi+0x34];
		cdq;
label2:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StorePositionAddress2()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xF9A6;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		mov eax, 2;
		call StorePositionAddress;
		popad;

		push 0;
		push eax;
		push ebx;
		mov edx, [ecx];
		call [edx+0x70];
		mov ecx, [edi+0x14];

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StorePositionAddress3()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xF366;
		mov ebx, label2; // fix the call
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3D452;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		mov edi, eax;
		mov eax, 3;
		call StorePositionAddress;
		popad;

		push eax;
		push exitcall;

label2:
		push 0x10000000;
		retn;

exitcall:
		mov edi, eax;
		mov eax, [esi+0x10];

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_HookMalloc()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x24A11;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x24A07;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		call HookMalloc;
		popad;

		mov [esi+4], eax;
		test eax, eax;
		jne skipjump;

label1:
		push 0x10000000;
		retn;

skipjump:
		mov [esi+8], edi;

label2:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}
NAKED void MemPatch_StoreBufferAddress()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x2FEB4;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		call StoreBufferAddress;
		popad;

		mov [esi+0x38], eax;
		pop edi;
		mov eax, 1;
		pop esi;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}
#elif GAME == KIMINOZOLE
NAKED void MemPatch_StoreBufferStruct()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x69873;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x81a58;
		
		pushad;
		call StoreBufferStruct;
		popad;

		mov ecx, edi;
		push edx;
label2:
		call dword ptr ds:[0];
		mov ecx, edi;
		
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StorePositionAddress1()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x4197c;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x103ba;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		mov edi, ebx;
		mov eax, 1;
		call StorePositionAddress;
		popad;

		push ebx;

		push callexit;
label1:
		push 0x10000000;
		retn;

callexit:
		mov ecx, [esi+0x34];
		cdq;
label2:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StorePositionAddress2()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x10506;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		mov eax, 2;
		call StorePositionAddress;
		popad;

		push 0;
		push eax;
		push ebx;
		mov edx, [ecx];
		call [edx+0x70];
		mov ecx, [edi+0x14];

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_HookMalloc()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x27e31;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x27e27;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		call HookMalloc;
		popad;

		mov [esi+4], eax;
		test eax, eax;
		jne skipjump;

label1:
		push 0x10000000;
		retn;

skipjump:
		mov [esi+8], edi;

label2:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void MemPatch_StoreBufferAddress()
{	
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x338d4;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		call StoreBufferAddress;
		popad;

		mov [esi+0x38], eax;
		pop edi;
		mov eax, 1;
		pop esi;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}
#endif
