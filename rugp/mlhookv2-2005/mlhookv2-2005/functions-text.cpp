#include "functions.h"

char *HookText(char *, int);
unsigned int GenHash(char *, unsigned int, int);
void ParseArchive();
void ParseScr(int);

FILE *dump = NULL; // output file when dumping scripts
FILE *scrlog = NULL; // log the scripts as they get dumped

AmiEntry *amientry = NULL;
Entry *entry = NULL;
ImageHeader *imgentry = NULL;

int parsed = 0; // .ami archived parsed status
int entrycount = 0; // amount of string entries in the .ami archive
int readFile = 0; // latest script file opened using ParseScr()
int errorDisp = 0; // if the .ami archive is missing, display an error but only once
int amicount = 0; // amount of entries inside of the ami archive
int baseOffset = 0; // base offset in the .ami archive
int filetype = 0; // current file type of selected file in .ami archive
int curFile = 0; // current open file in rUGP
int positionAddress = 0; // file offset in .rio

BYTE amipatht[MAX_PATH] = { 0 }; // path from strAbsolutePath

#define OpenTextFile(_context,cmpid,txtfile)\
	if(entry[i].context == _context && (id&0xffffff) == cmpid)\
	_OpenTextFile(txtfile);

inline void _OpenTextFile(char *txtfile)
{
	STARTUPINFO         siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	char *notepaddir = (char*)calloc(MAX_PATH+256,sizeof(char));
	char *tfilepath = (char*)calloc(MAX_PATH+256,sizeof(char));
	char *filepath = (char*)calloc(MAX_PATH+256,sizeof(char));

	if(amipatht[0]!='\0')
		sprintf(tfilepath,"%s%s\0",amipatht,txtfile); // have absolute path
	else
		sprintf(tfilepath,"%s\0",txtfile); // don't have absolute path

	FILE *test = fopen(tfilepath,"rb");

	if(!test)
	{
		char msg[MAX_PATH+512];
		
		sprintf(msg,"Could not open requested file: %s\n",tfilepath);
		MessageBox(NULL,msg,"Error",MB_OK);

		return;
	}

	fclose(test);

	sprintf(filepath," %s\0",tfilepath);

	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	GetWindowsDirectory(notepaddir, MAX_PATH);
	strcat(notepaddir, "\\notepad.exe\0");

	CreateProcess(notepaddir, filepath, 0, 0, FALSE, CREATE_DEFAULT_ERROR_MODE, 0, 0, &siStartupInfo, &piProcessInfo);

	free(filepath);
	free(tfilepath);
	free(notepaddir);
}

void HookMain()
{
	char *addr;
	int textaddr, id;

	__asm
	{
		push eax;
		push ecx;
		push edx;
#if GAME == MUVLUVEXUL || GAME == MUVLUVALT
		mov addr, ebx;
		mov eax, [ebx-0x0c];
#elif GAME == KIMINOZOLE
		mov addr, edx;
		mov eax, [edx-0x0c];
#endif
		and eax, 0xffffff;
		mov id, eax;
	};
	
	textaddr = (int)HookText(addr, id);
	
	__asm
	{
#if GAME == MUVLUVEXUL || GAME == MUVLUVALT
		mov ebx, eax;
		mov [esp+0x0c], eax; // little hack since msvc thinks it's so important to preserve ebx 
#elif GAME == KIMINOZOLE
		mov [esp+0x14], eax;
#endif
		pop edx;
		pop ecx;
		pop eax;
	};
}

char *HookText(char *input, int id)
{		
	if(parsed == 0)
		ParseArchive();
	
#if LOGCHOICES == 1
	if(input[0] == '\x12')
	{
		int len = strlen(input);
		unsigned int hash = GenHash(input, HASHKEY, len);
		FILE *filelog = NULL;

		CreateDirectory("output", NULL);
		filelog = fopen("output\\log.dat", "ab");
		
		fseek(filelog, 0, SEEK_END);
		fwrite(&hash,1, 4, filelog);
		fwrite(&len,1, 4, filelog);
		fwrite(input,1, len, filelog);
		
		fclose(filelog);
	}
#endif
	
#if DUMPMEM == 1
	if(dump != NULL)
	{
		fclose(dump);
		fclose(scrlog);
		dump = NULL;
	}
#endif

#if SKIPTEXT == 1
	if(input[0] != '\x12')
	{
		return "\x05\x00";
	}
#endif
	
	if(entrycount == 0)
	{
		return input;
	}
	else
	{
		int i = 0, found = 0, len = strlen(input);
	
		for(i = 0; i < entrycount; i++)
		{   
			/*
			{
				FILE *txtlog = fopen("txtdbg2.txt","a+");
				fprintf(txtlog,"file[%08x] id[%08x] buffer[%08x] buffer2[%08x] %s\n",entry[i].context,entry[i].id,entry[i].buffer,entry[i].buffer2,entry[i].str);
				fclose(txtlog);
			}
			*/

			if(((id & 0x00ffffff) == entry[i].id &&
				((entry[i].buffer == 0 &&
				  entry[i].context == readFile) ||
				 (entry[i].buffer != 0 &&
				  entry[i].buffer == input))) ||
				(entry[i].id == GenHash(input, HASHKEY, len)))
			{
				// proof of concept code for loading a notepad at a certain part of a script
				// replace the context with the file and the id with the id where you want the txt to load at
#if GAME == MUVLUVALT
				OpenTextFile(0x55c43c48, 0x000023b4, "Coup d'etat Arc Ending Speech.txt");
				OpenTextFile(0x5f4cf75e, 0x00002008, "Early Sadogashima Arc Congratulatory Letter.txt");
				OpenTextFile(0x5fd145f8, 0x0000062d, "Yuuko's Voice-only Line Before Operation Cherry Blossom.txt");
#endif

				entry[i].buffer = input;
				found = 1;

				break;
			}
		}
		
		if(found == 0)
		{
#if DEBUGIDS
			{
				FILE *txtlog = fopen("txtdbg.txt","a+");
				fprintf(txtlog,"file[%08x] id[%08x] buffer[%08x] %s\n",readFile,id&0x00ffffff,(int)input,input);
				fclose(txtlog);
			}
#endif

			return input;
		}
		else
		{
			return entry[i].str;
		}
		
		found = 0;
	}

	return input;
}

void DumpText(char *input,
#if GAME == MUVLUVALT
			  char *input2,
#endif
			  int len)
{ 
	int i = 0, id = 0, id2 = 0;
	char msg[256];

	
	if(curFile != positionAddress)
	{   
		curFile = positionAddress;
		ParseScr(positionAddress);

#if GAME == MUVLUVALT && MLALT_FIXBOAT == 1
		if(curFile==0x214b3fa2)
		{
			if(*(char*)(input+0xe10)=='0')
				*(char*)(input+0xe10) = '1';
		}
		else if(curFile==0x22a5d446)
		{
			if(*(char*)(input+0x4319)=='0')
				*(char*)(input+0x4319) = '1';
		}
#endif
		
#if DEBUG >= 1 && DUMPMEM == 1
		if(dump != NULL)
		{
			fclose(dump);
			fclose(scrlog);
			dump = NULL;
		}
#endif
	}

	id = *(int*)(input - 0x0c) & 0xffffff;
#if GAME == MUVLUVALT
	id2 = *(int*)(input2 - 0x0c) & 0xffffff;
#endif

	for(i = 0; i < entrycount; i++)
	{
		if(
#if GAME == MUVLUVALT
			entry[i].context == positionAddress && entry[i].id == id2
#else
			entry[i].context == positionAddress && entry[i].id == id
#endif
			)
		{
#if GAME == MUVLUVALT
#if SELFFIX == 1
			if(entry[i].context == positionAddress && entry[i].id == id)
				entry[i].id = id2;
#endif
			entry[i].buffer = input2;
#else
			entry[i].buffer = input;
#endif
			break;
		}
	}

#if DEBUG >= 1 && DUMPMEM == 1
	if(dump == NULL) // don't reopen it if it's still looping
	{
		char *outname = (char*)calloc(MAX_PATH, sizeof(char));
		char msg[256];
			
		sprintf(outname, "%08x.dmp", curFile);
		sprintf(msg, "%s\n", outname);
		//MessageBox(NULL, msg, "Debug", MB_OK);
		
		CreateDirectory("output", NULL);
		SetCurrentDirectory("output");

		dump = fopen(outname,"rb");
		if(dump!=NULL)
		{
			sprintf(msg, "%s appears to be a duplicate. Possibly missed a position hooking command. Look into this.\n", outname);
			//MessageBox(NULL, msg, "Debug", MB_OK);
			fclose(dump);
		}
			
		dump = fopen(outname, "wb");
		scrlog = fopen("log.txt", "a+");
		SetCurrentDirectory("..");
		free(outname);
		fprintf(scrlog, "%X %08x.dmp\n", filetype, curFile);
		fwrite(&filetype, 1, 4, dump);
	}
		
	fwrite(input-0x0c, 1, 4, dump);
	fwrite(&len, 1, 4, dump);
	fwrite(input, 1, len, dump);
	fwrite("\0\0\0\0", 1, 4, dump);
#endif
}

void ParseArchive()
{
	FILE *infile = NULL;
	char amipath[MAX_PATH] = { 0 };
	char magic[4] = { 0 };
	DWORD amipathtype = 0;
	HKEY outKey = NULL;
	
	parsed = 1;

	// HKEY_CURRENT_USER Software\\relic UGP Applications\\age\\ƒ}ƒuƒ‰ƒ”ƒIƒ‹ƒ^ƒlƒCƒeƒBƒ”\\Installation	
	RegOpenKey(HKEY_CURRENT_USER, REGSTR, &outKey);

	if(outKey != NULL)
	{
		char msg[256];
		int retcode = 0;
		DWORD amipathlen = sizeof(amipath);
		DWORD type = REG_SZ;

		retcode = RegQueryValueEx(outKey, "strAbsolutePath", 0, &type, amipatht, &amipathlen);

		sprintf(amipath,"%s\\data.ami\0",amipatht);
		RegCloseKey(outKey);
	}
	else
	{
		MessageBox(NULL,"Could not read correct path from registry","Debug",MB_OK);
	}

	// "data.ami"
	infile = fopen(amipath, "rb");
	
	if(!infile)
	{
		if(errorDisp != 1)
		{
			char msg[1024], dir[512];

			GetCurrentDirectory(512,dir);
			sprintf(msg,"Could not open %s, currently in %s",amipath,dir);

			errorDisp = 1;
			MessageBox(NULL, msg, "Error", MB_OK);
		}
		
		return;
	}
	
	fread(magic, 1, 4, infile);
	
	if(strncmp(magic, "AMI", 3) != 0)
	{
		if(errorDisp != 1)
		{
			errorDisp = 1;
			MessageBox(NULL, "Invalid data.ami", "Error", MB_OK);
		}
	}
	else
	{   
		fread(&amicount, 1, 4, infile);
		fread(&baseOffset, 1, 4, infile);
		fseek(infile, 4, SEEK_CUR);

		amientry = (AmiEntry*)calloc(amicount+1, sizeof(AmiEntry));

		fread(amientry, sizeof(AmiEntry), amicount, infile);
		ParseScr(0xffffffff);
		ParseScr(0xfffffffe);
	}
}

void ParseScr(int file)
{
	int i = 0;
	
	if(parsed == 0)
		ParseArchive();
	
	if(amientry == NULL)
		return;
	
	for(i = 0; i < amicount; i++)
	{
		if(amientry[i].file == file)
		{
			FILE *infile = fopen("data.ami", "rb");
			ScrEntry *screntry = NULL;
			int x = 0, scrtype = 0, scrcount = 0;

			if(!infile)
				break;
			
			fseek(infile, amientry[i].offset+4, SEEK_SET);
			fread(&scrtype, 1, 4, infile);
			
			if((file != 0xffffffff && file != 0xfffffffe) && filetype != scrtype)
			{
				fclose(infile);
				break;
			}
			
			fread(&scrcount, 1, 4, infile);
			
			screntry = (ScrEntry*)calloc(scrcount+1, sizeof(ScrEntry));
			entry = (Entry*)realloc(entry, (entrycount+scrcount+1)*sizeof(Entry));
			
			fread(screntry, sizeof(ScrEntry), scrcount, infile);
			
			for(x = 0; x < scrcount; x++, entrycount++) // build string entries
			{   
				fseek(infile, amientry[i].offset+screntry[x].offset, SEEK_SET);
				
				entry[entrycount].str = (char*)calloc(screntry[x].len+1, sizeof(char));
				entry[entrycount].id = screntry[x].id;
				entry[entrycount].len = screntry[x].len;
				entry[entrycount].context = file;
				entry[entrycount].buffer = 0;
				
				fread(entry[entrycount].str, 1, entry[entrycount].len, infile);
				readFile = file;
			}
	
			free(screntry);
			fclose(infile);
			
			break;
		}
	}

	if(imgentry != NULL)
	{
		for(i=0; i<amicount; i++) // free any unused memory
		{
			if(imgentry[i].bg != NULL)
			{
				free(imgentry[i].bg);
				imgentry[i].bg = NULL;
			}
		}
	}
}


unsigned int GenHash(char *string, unsigned int key, int len)
{
	unsigned int hash = 0, i=0;

	for(i = 0; i < (unsigned int)len; i++)
		hash = (((key ^ hash) << 3) + hash) ^ string[i];

	return hash;
}

char *CheckVoiceName(char *input)
{
	const static char nameTable[][2][32] =
	{
		{ "ˆÉ‹÷‚Ý‚¿‚é\0", "Isumi Michiru\0" },
		{ "^”ü“â\0", "Ebisu Minagi\0" },
		{ "ŠÓƒ‰Ä\0", "Kagami Sumika\0" },
		{ "ŒŽ—[ŒÄ\0", "Kouzuki Yuuko\0" },
		{ "ŠZˆß¶‹ß\0", "Yoroi Sakon\0" },
		{ "‘¬£…ŒŽ\0", "Hayase Mitsuki\0" },
		{ "_‹{Ži‚Ü‚è‚à\0", "Jinguuji Marimo\0" },
		{ "_‘ã’F\0", "Kamiyo Tatsumi\0" },
		{ "ŒŽ‰r^“ß\0", "Tsukuyomi Mana\0" },
		{ "ƒAƒ‹ƒtƒŒƒbƒhEƒEƒH[ƒPƒ“\0", "Alfred Walken\0" },
		{ "”–Ø°Žq\0", "Kashiwagi Haruko\0" },
		{ "‚»‚Ì‘¼\0", "Other\0" },
		{ "¹–¶®Æ\0", "Sagiri Naoya\0" },
		{ "ŽÐ‰à\0", "Yashiro Kasumi\0" },
		{ "—Á‹{ˆ©\0", "Suzumiya Akane\0" },
		{ "Ê•ôŒd\0", "Ayamine Kei\0" },
		{ "ƒsƒAƒeƒBƒt’†ˆÑ\0", "1st Lt. Piatif\0" },
		{ "—Á‹{ê¡\0", "Suzumiya Haruka\0" },
		{ "åç’ß\0", "Sakaki Chizuru\0" },
		{ "—I—z\0", "Yuuhi\0" },
		{ "”bá”T\0", "Tomoe Yukino\0" },
		{ "ŒäŒ•–»–é\0", "Mitsurugi Meiya\0" },
		{ "Žì£Œ·åÖ\0", "Tamase Genjousai\0" },
		{ "•—ŠÔ“˜Žq\0", "Kazama Touko\0" },
		{ "”’‹â•\0", "Shirogane Takeru\0" },
		{ "@‘œ”üá\0", "Munakata Misae\0" },
		{ "Žì£p•P\0", "Tamase Miki\0" },
		{ "ŠZˆß‘¸lE”ü‹Õ\0", "Yoroi Mikoto/Mikoto\0" },
		{ "ƒCƒ‹ƒ}EƒeƒXƒŒƒt\0", "Irma Thesleff\0" },
		{ "ƒ‰ƒ_ƒrƒmƒbƒhŽi—ß\0", "Gen. Radhabinod" }
	};
	const static char nameTableLen = sizeof(nameTable)/64;

	int i = 0;

	// find tab
	for(i=0; i<strlen(input); i++)
	{
		if((unsigned char)input[i]>0x7f)
			i++;
		else if(input[i]=='\t' || input[i]=='\0')
			break;
	}

	if(input[i]=='\t')
	{
		int len = i;

		input[i]='\0';

		for(i=0; i<nameTableLen; i++)
		{
			if(memcmp(nameTable[i][0], input, len)==0) // found name in our translation table
			{
				static char outputName[132];
				int engNameLen = strlen(nameTable[i][1]);
				int x = 0;

				memset(outputName,'\0',132);
				memcpy(outputName,nameTable[i][1],engNameLen);

				outputName[engNameLen] = '\t';
				for(x=engNameLen+1, i=len+1; input[i]!='\0'; x++, i++) // append ON/OFF
					outputName[x] = input[i];				

				return (char*)outputName;
			}
		}
	}

	return input;
}
