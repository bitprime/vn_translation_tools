#include "patches-text.h"

#if EXACTGAME == MUVLUVEXUL
char ArraySerifText[] = "_‚»‚Ì‘¼/16752740/0//Shirogane:Takeru/16777215/0//Kagami:Sumika/16761008/0//Mitsurugi:Meiya/11591935/0//Tamase:Miki/16744688/0//Sakaki:Chizuru/8450192/0//Ayamine:Kei/12628192/0//Yoroi:Mikoto/8454120/0//Jinguuji:Marimo/16775296/0//Kouzuki:Yuuko/16765160/0//Tsukuyomi:Mana/15269856/0//Kamiyo:Tatsumi/15771840/0//Tomoe:Yukino/16765096/0//Ebisu:Minagi/16773312/0//Suzumiya:Akane/16762976/0//Ichimonji:Takahashi /14737632/0//Mikoto/8454120/0//Yashiro:Kasumi/16575743/0//";
#elif EXACTGAME == MUVLUVALT
char ArraySerifText[] = "_‚»‚Ì‘¼/16752740/0//Shirogane:Takeru/13684964/0//Kagami:Sumika/16761008/0//Mitsurugi:Meiya/11591935/0//Tamase:Miki/16744688/0//Sakaki:Chizuru/8450192/0//Ayamine:Kei/12628192/0//Yoroi:Mikoto:Mikoto/8454120/0//Yashiro:Kasumi/15651071/0//Jinguuji:Marimo/16775296/0//Kouzuki:Yuuko/16765160/0//1st Lt. Tsukuyomi:Tsukuyomi:Mana/15269856/0//Kamiyo:Tatsumi/15771840/0//Tomoe:Yukino/16765096/0//Ebisu:Minagi/16773312/0//2nd Lt. Suzumiya:Suzumiya:Akane:/16762976/0//Suzumiya:1st Lt. Suzumiya/16769200/0//Hayase:1st Lt. Hayase/7973631/0//Munakata:1st Lt. Munakata/16023195/0//Kazama:2nd Lt. Kazama/10005656/0//Kashiwagi:2nd Lt. Kashiwagi/7386329/0//Isumi:Michiru:Capt. Isumi/16743022/0//";
#elif EXACTGAME == KIMINOZOLE
char ArraySerifText[] = "_‚»‚Ì‘¼/16777215/0//Haruka:Suzumiya/16761008/0//Mitsuki:Hayase/11591935/0//Akane:Suzumiya/16762976/0//Shinji:Taira/15269856/0//Mayu:Tamano/8454120/0//Ayu:Daikuuji/16775296/0//Manami:Homura/8450192/0//Fumio:Hoshino/16765160/0//Hotaru:Amakawa/16744688/0//Motoko:Kouzuki/15794048/0//Kenzou:Sakiyama/14737632/0//";
#endif

int ArraySerifTextAddr = (int)ArraySerifText;

NAKED void TextPatch_nTextSpeed1()
{
	__asm
	{
		mov eax, 0x20;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_nTextSpeed2()
{
	__asm
	{
		push 0x20;
		ENDFUNC;
		ENDFUNC;
	};
}

#if GAME == MUVLUVEXUL
NAKED void TextPatch_arraySerifSetting()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xc730;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xa1b7;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		lea esi, [esp+0x20];
		mov eax, ArraySerifTextAddr;
		mov [esi], eax;
		popad;

		push exitfunc;
label1:
		push 0x10000000;
		retn;

exitfunc:
		mov edx, [esi+0x1f4];

label2:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_HookMain()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xfc02;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		push ebx;
		mov ebx, [esp+0x24];
		call HookMain;
		push ebp;
		push esi;
		push edi;
		mov edi, ecx;

label1:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_DumpText()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JE
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x60051;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6bd88;
		mov ebx, label3; // fix the return back to where it should go when the func is done
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x60051;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		call DumpText;
		popad;

		cmp eax, esi;
		jne skipjump;

label1:
		push 0x10000000;
		retn;

skipjump:
		push 0;
		push 3;

label3:
		push 0x10000000;
label2:
		push 0x10000000;
		retn;

		ENDFUNC;
		ENDFUNC;
	};
}
#elif GAME == MUVLUVALT
NAKED void TextPatch_HookMain()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x12442;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		push ebx;
		mov ebx, [esp+0x34];
		push ebp;
		call HookMain;
		xor ebp, ebp;
		push esi;
		push edi;

label1:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_DumpText()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JE
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x57131;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x65A8C;
		mov ebx, label3; // fix the return back to where it should go when the func is done
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x5712C;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		push eax;
#if GAME == MUVLUVALT
		push edi;
#endif
		push edx;
		call DumpText;
		pop edx;
#if GAME == MUVLUVALT
		pop edi;
#endif
		pop eax;
		popad;
		
		push exitcall;
label2:
		push 0x10000000;
		retn;

exitcall:
		cmp eax, esi;
		jne skipjump;

label1:
		push 0x10000000;
		retn;

skipjump:
		push 0;
		push 3;

label3:
		push 0x10000000;
		retn;

		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_arraySerifSetting()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xEAE0;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xA527;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		lea esi, [esp+0x20];
		mov eax, ArraySerifTextAddr;
		mov [esi], eax;
		popad;

		push exitfunc;
label1:
		push 0x10000000;
		retn;

exitfunc:
		mov edx, [esi+0x1f4];

label2:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_FixVoiceNames()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xB55B;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		push eax;
		call CheckVoiceName;
		mov [esp+0x20], eax;
		pop eax;
		popad;

		push eax;
		mov eax, [ebp+0xc0];
		push 0;
		push 0x180;

exitfunc:
		mov edx, [esi+0x1f4];

label2:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_FixVoiceNames2()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xB99B;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		push ecx;
		call CheckVoiceName;
		mov [esp+0x1c], eax;
		pop eax;
		popad;

		mov edx, [esi+0xc0];
		push ecx;
		push ebp;
		push 0x181;

label2:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}
#elif GAME == KIMINOZOLE
NAKED void TextPatch_arraySerifSetting()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x104d0;
		mov ebx, label2; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0xb857;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		lea esi, [esp+0x20];
		mov eax, ArraySerifTextAddr;
		mov [esi], eax;
		popad;

		push exitfunc;
label1:
		push 0x10000000;
		retn;

exitfunc:
		mov edx, [esi+0x1f4];

label2:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_HookMain()
{
	__asm
	{	
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JMP back to the destination
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x14392;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;
		
		push ebx;
		push ebp;
		mov ebp, [esp+0x40];
		call HookMain;
		push esi;
		push edi;
		xor edi, edi;

label1:
		push 0x1000000;
		retn;
		ENDFUNC;
		ENDFUNC;
	};
}

NAKED void TextPatch_DumpText()
{
	__asm
	{
		push eax;
		mov eax, [esp+4];
		push ebx;
		mov ebx, label1; // fix the JE
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6e481;
		mov ebx, label2; // fix the CALL
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x80a8a;
		mov ebx, label3; // fix the return back to where it should go when the func is done
		mov dword ptr [ebx+1], eax;
		add dword ptr [ebx+1], 0x6e481;
		pop ebx;
		pop eax;
		mov [esp], eax;
		pop eax;

		pushad;
		call DumpText;
		popad;

		cmp eax, esi;
		jne skipjump;

label1:
		push 0x10000000;
		retn;

skipjump:
		push 0;
		push 3;

label3:
		push 0x10000000;
label2:
		push 0x10000000;
		retn;

		ENDFUNC;
		ENDFUNC;
	};
}
#endif
