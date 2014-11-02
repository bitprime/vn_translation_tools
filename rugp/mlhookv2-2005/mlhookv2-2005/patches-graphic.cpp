#include "patches-graphic.h"

short width = 0, height = 0;
int tempa = 0, tempb = 0;
short tempc = 0, tempd = 0;

#if GAME == MUVLUVEXUL
NAKED void GfxPatch_GetImageSize()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JNZ command
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3f26e;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x830B4;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3F25B;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov dx, [ecx+4];
		mov bx, [ecx+6];
		mov width, dx;
		mov height, bx;
		popad;

		push ecx;
		push edi;
		
label2:
		call dword ptr ds:[0];
		add esp, 0x0c;
		test eax, eax;

		jz label4;
label1:
		push 0x10000000;
		retn;
label4:
		mov ecx, dword ptr ss:[esp+0x44];

label3:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_HookImage()
{
	__asm
	{	
		push eax;
		add esp, 4;
		pop eax;
		sub esp, 4;
		pop eax;
		
		pushad;
		push ebp;
		mov ax, [esp+0x62];
		dec ax;
		push ax;
		mov ax, [esp+0x62];
		push ax;
		push esp;
		call HookImage;
		pop esp;
		pop eax;
		pop ebp;
		popad;

		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_HookImageUI()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6dc40;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6dc05;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov si, width;
		mov di, height;
		mov ebx, [esp+20];
		mov al, 0;
		call HookImageUI;
		cmp eax, 1;
		popad;

		je cont;
		
		push edx;
		push eax;
		push edi;
		push ecx;
		
		push exitfunc; // set the returning address

label1:
		push 0x10000000;
		retn;

cont:
		add ebx, 0x10;
		sub esp, 0x10;
		xor eax, eax;
		pushad;
		mov al, 1;
		call HookImageUI;
		popad;
		
exitfunc:
		add esp, 0x28;
label3:
		push 0x10000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void GfxPatch_HookImageRGB()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3b820;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3b736;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov eax, [esp+0x20];
		mov tempa, eax;
		mov eax, [esp+0x24];
		mov tempb, eax;
		mov ax, [esp+0x2c];
		mov tempc, ax;
		mov ax, [esp+0x2e];
		mov tempd, ax;
		popad;

		push label2;
label1:
		push 0x10000000;
		retn;

label2:
		push tempa;
		push tempb;
		push tempc;
		push tempd;
		call HookImageRGB;
		pop eax;
		pop eax;
		pop eax;

		pop edi;
		pop esi;
		pop ebx;
		add esp, 0x10;

label3:
		push 0x10000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}
#elif GAME == MUVLUVALT
NAKED void GfxPatch_GetImageSize()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JNZ command
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x27ADE;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x75084;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x27ACB;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov dx, [ecx+4];
		mov bx, [ecx+6];
		mov width, dx;
		mov height, bx;
		popad;

		push ecx;
		push edi;
		
label2:
		call dword ptr ds:[0];
		add esp, 0x0c;
		test eax, eax;

		jz label4;
label1:
		push 0x10000000;
		retn;
label4:
		mov ecx, dword ptr ss:[esp+0x44];

label3:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_GetImageSize2()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x75084;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x2E936;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov dx, [ebx+4];
		mov bx, [ebx+6];
		mov width, dx;
		mov height, bx;
		popad;
		
		call esi;
label2:
		mov ebx, dword ptr ds:[0];
		lea edx, [esp+0x30];

label3:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_HookImage()
{
	__asm
	{	
		push eax;
		add esp, 4;
		pop eax;
		sub esp, 4;
		pop eax;

		pop edi;
		pop esi;
		mov esp, ebp;
		pop ebp;
		
		pushad;
		push ebp;
		mov ax, [esp+0x62];
		dec ax;
		push ax;
		mov ax, [esp+0x62];
		push ax;
		push esp;
		call HookImage;
		pop esp;
		pop eax;
		pop ebp;
		popad;

		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_HookImageUI()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x640F0;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x65842;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;

		mov si, width;
		mov di, height;
		mov ebx, ebp;
		mov eax, 0;
		call HookImageUI;
		cmp eax, 1;
		popad;

		je cont;
		
		add edi, 0x40;
		push ebp;
		push edi;
		push eax;
		
		push exitfunc; // set the returning address

label1:
		push 0x10000000;
		retn;

cont:
//		add ebx, 0x10;
		sub esp, 0x0c;
		xor eax, eax;
		pushad;
		mov eax, 1;
		call HookImageUI;
		popad;
		xor eax, eax;
		xor ecx, ecx;
		xor edx, edx;
		
exitfunc:
		//add esp, 0x28;
label3:
		push 0x10000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

unsigned char temp = 0, jumpdata = 0;
NAKED void GfxPatch_HookImage2()
{

	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x654AD;
		mov ebx, label4; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x654B4;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		mov temp, dl;
		mov edx, [esp+0x14];

		pushad;
		sub edx, 0x00010000;
		push eax;
		push ebx;
		push edx;
		push esp;
		call HookImage;
		pop esp;
		pop edx;
		pop ebx;
		mov jumpdata, al;
		pop eax;
		popad;

		push edx;
		mov edx, [esp+0x14];
		push edx;
		push eax;
		push ebx;
		push ecx;
		push ebp;

		cmp jumpdata, 1;
		je skipdata;

		push edx;
		mov dl, temp;
		test dl, dl;
		pop edx;

label3:
		push 0x10000000;
		retn;

skipdata:
		mov edx, 0xffffffff;

		push edx;
		mov dl, temp;
		test dl, dl;
		pop edx;
label4:
		push 0x10000000;
		retn;

		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void GfxPatch_HookImage3()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x758A4;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x5EE9E;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		pushad;
		push ebp;
		mov ax, [esp+0x66];
		dec ax;
		push ax;
		mov ax, [esp+0x66];
		push ax;
		push esp;
		call HookImage;
		pop esp;
		pop eax;
		pop ebp;
		popad;

		add esp, 0x1c;
		mov ecx, ebx;

label1:
		call dword ptr ds:[0x10000000];

label2:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_HookImageRGB()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x380D0;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x37FE6;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov eax, [esp+0x20];
		mov tempa, eax;
		mov eax, [esp+0x24];
		mov tempb, eax;
		mov ax, [esp+0x2c];
		mov tempc, ax;
		mov ax, [esp+0x2e];
		mov tempd, ax;
		popad;

		push label2;
label1:
		push 0x10000000;
		retn;

label2:
		push tempa;
		push tempb;
		push tempc;
		push tempd;
		call HookImageRGB;
		pop eax;
		pop eax;
		pop eax;

		pop edi;
		pop esi;
		pop ebx;
		add esp, 0x10;

label3:
		push 0x10000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}
#elif GAME == KIMINOZOLE
NAKED void GfxPatch_GetImageSize()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JNZ command
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x2c9de;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+3], eax;
		add dword ptr [ebx+3], 0x810b8;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x2c9bc;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov dx, [eax+4];
		mov bx, [eax+6];
		mov width, dx;
		mov height, bx;
		popad;

		push eax;
		push edi;
		
label2:
		call dword ptr ds:[0];
		add esp, 0x0c;
		test eax, eax;

		jz label4;
label1:
		push 0x10000000;
		retn;
label4:
		mov ecx, dword ptr ss:[esp+0x4c];

label3:
		push 0x10000000;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_HookImage()
{
	__asm
	{	
		push eax;
		add esp, 4;
		pop eax;
		sub esp, 4;
		pop eax;
		
		pushad;
		push ebp;
		call HookImage;
		pop ebp;
		popad;

		add esp, 4;
		pop edi;
		pop esi;
		mov esp, ebp;
		pop ebp;
		retn;
		ENDFUNC
		ENDFUNC
	};
}

NAKED void GfxPatch_HookImageUI()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6c150;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6c11a;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov si, width;
		mov di, height;
		mov ebx, [esp+20];
		mov al, 0;
		call HookImageUI;
		cmp eax, 1;
		popad;

		je cont;
		
		push edx;
		push eax;
		push edi;
		push ecx;
		
		push exitfunc; // set the returning address

label1:
		push 0x10000000;
		retn;

cont:
		add ebx, 0x10;
		sub esp, 0x10;
		xor eax, eax;
		pushad;
		mov al, 1;
		call HookImageUI;
		popad;
		
exitfunc:
		add esp, 0x28;
label3:
		push 0x10000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void GfxPatch_HookImageRGB()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3be30;
		mov ebx, label3; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x3bd46;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		mov eax, [esp+0x20];
		mov tempa, eax;
		mov eax, [esp+0x24];
		mov tempb, eax;
		mov ax, [esp+0x2c];
		mov tempc, ax;
		mov ax, [esp+0x2e];
		mov tempd, ax;
		popad;

		push label2;
label1:
		push 0x10000000;
		retn;

label2:
		push tempa;
		push tempb;
		push tempc;
		push tempd;
		call HookImageRGB;
		pop eax;
		pop eax;
		pop eax;

		pop edi;
		pop esi;
		pop ebx;
		add esp, 0x10;

label3:
		push 0x10000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}
#endif
