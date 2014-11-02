#include "patches-resources.h"

#if GAME == MUVLUVEXUL
NAKED void vm60_rsrcpatch1()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x358C;
		mov ebx, label2; // fix the offset
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xA15AF;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		push eax;
label2:
		push 0x10000000;
		mov [esp+0x1c], esi;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void vm60_rsrcpatch2()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xC0A0;
		mov ebx, label2; // fix the offset
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xA2406;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label2:
		push 0x10000000;
		push 0x0A001;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void vm60_rsrcpatch3()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xC107;
		mov ebx, label2; // fix the offset
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xA23BE;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label2:
		push 0x10000000;
		push 0x0A004;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void vm60_rsrcpatch4()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xC119;
		mov ebx, label2; // fix the offset
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xA23A2;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label2:
		push 0x10000000;
		push 0x0A004;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void vm60_rsrcpatch5()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xC157;
		mov ebx, label2; // fix the offset
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xA238E;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label2:
		push 0x10000000;
		push 0x0A003;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}
#elif GAME == MUVLUVALT
NAKED void vm60_rsrcpatch1()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xDE81;
		mov ebx, label2; // fix the offset
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x81742;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label2:
		push 0x10000000;
		push 0x0A003;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void vm60_rsrcpatch2()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xDE3E;
		mov ebx, label2; // fix the offset
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x81762;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

label2:
		push 0x10000000;
		push 0x0A001;

label1:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}
#endif

