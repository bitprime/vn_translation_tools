/*
	Parse Scene.pck from Siglus Engine
*/

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <windows.h>
#include <locale.h>
#include "compression.h"
#include "structs.h"

#define CreateBuffer(I,O,T) (int)I.buffer += (int)buffer; \
O = (T*)calloc(I.size+1,sizeof(EntryChar)); \
memcpy(O,I.buffer,I.size*8);

EntryStr *filenamestr = NULL;
EntryChar *filetable = NULL;
int main(int argc, char **argv)
{
	FILE *infile = NULL, *outfile = NULL, *list = NULL;
	unsigned char *buffer = NULL;
	int size = 0, i = 0;

	setlocale(LC_ALL, "");
	
	if(argc<2)
	{
		printf("usage: %s infile [outfolder]\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"rb");
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	fread(&pckhead,1,sizeof(pckhead),infile);
	fseek(infile,0,SEEK_END);
	size = ftell(infile);
	rewind(infile);
	
	buffer = (unsigned char*)calloc(size,sizeof(unsigned char));
	fread(buffer,1,size,infile);
	fclose(infile);
		
	(int)pckhead.filenamestr.buffer += (int)buffer;
	CreateBuffer(pckhead.filenametable4, filenamestr, EntryStr);
	CreateBuffer(pckhead.filetable, filetable, EntryChar);
	
	if(argc==3)
	{
		mkdir(argv[2]);
		chdir(argv[2]);
	}
	
	list = fopen("list.dat","wb");
	fwrite(&pckhead.filenametable4.size, 1, 4, list);
	
	for(i=0; i<pckhead.filenametable4.size; i++)
	{
		wchar_t *outname = NULL;
		HANDLE hFile;
		unsigned char *output = NULL;
		int x=0, complen = 0, decomplen = 0;
		int ignore;
		
		(int)filenamestr[i].buffer = (int)filenamestr[i].buffer*2 + (int)pckhead.filenamestr.buffer;
		
		wprintf(L"[%03d] ",i);
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), filenamestr[i].buffer, filenamestr[i].size, &ignore, NULL);
		wprintf(L" offset[%08x] len[%08x]\n",(int)filetable[i].buffer+(int)pckhead.filestart.buffer,filetable[i].size);
		
		outname = (wchar_t*)calloc(wcslen(filenamestr[i].buffer)+4,sizeof(wchar_t));
		wcsncpy(outname,filenamestr[i].buffer,filenamestr[i].size);
		wcscat(outname,L".ss\0");
		
		ignore = wcslen(outname) * 2;
		fwrite(&ignore, 1, 4, list);
		fwrite(outname, 1, ignore, list);
		ignore = 0;
		
		//outfile = _wfopen(outname,L"wb");
		hFile = CreateFileW(outname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
		(int)filetable[i].buffer += (int)buffer+(int)pckhead.filestart.buffer;

		if(pckhead.extraencrypt==1)
		{
			for(x=0; x<filetable[i].size; x++)
				filetable[i].buffer[x] ^= key2[x&0x0f];
		}	
		
		for(x=0; x<filetable[i].size; x++)
			filetable[i].buffer[x] ^= scrkey[x&0xff];
	
		complen = *(int*)filetable[i].buffer;
		decomplen = *(int*)(filetable[i].buffer+4);
		
		////fwrite(filetable[i].buffer,1,filetable[i].size,outfile);
		//WriteFile(hFile, filetable[i].buffer, filetable[i].size, NULL, NULL);
		
		output = (unsigned char*)calloc(decomplen, sizeof(unsigned char));
		DecompressData(filetable[i].buffer+8,output,decomplen);
		
		//fwrite(output,1,decomplen,outfile);
		WriteFile(hFile, output, decomplen, &x, NULL);
		CloseHandle(hFile);
		
		free(output);
		free(outname);
	}
	
	fclose(list);
	
	
	(int)pckhead.table1.buffer += (int)buffer;	
	outfile = fopen("vars.dat","wb");
	fwrite(&pckhead.globalvartable.size,1,4,outfile);
	fwrite(pckhead.table1.buffer,8,(int)pckhead.filenametable1.buffer-pckhead.headersize,outfile);
	fclose(outfile);
	
	(int)pckhead.filenametable1.buffer += (int)buffer;	
	(int)pckhead.filenametable2.buffer += (int)buffer;	
	(int)pckhead.filenametable3.buffer += (int)buffer;	
	
	outfile = fopen("vars1.dat","wb");
	i = (int)pckhead.filenametable2.buffer-(int)pckhead.filenametable1.buffer;
	fwrite(&pckhead.filenametable1.size,1,4,outfile);
	fwrite(pckhead.filenametable1.buffer,1,(int)pckhead.filenametable2.buffer-(int)pckhead.filenametable1.buffer,outfile);
	fclose(outfile);
	
	outfile = fopen("vars2.dat","wb");
	i = (int)pckhead.filenametable3.buffer-(int)pckhead.filenametable2.buffer;
	fwrite(&pckhead.filenametable2.size,1,4,outfile);
	fwrite(pckhead.filenametable2.buffer,1,(int)pckhead.filenametable3.buffer-(int)pckhead.filenametable2.buffer,outfile);
	fclose(outfile);
	
	outfile = fopen("vars3.dat","wb");
	i = (int)pckhead.filenametable4.buffer-(int)pckhead.filenametable3.buffer;
	fwrite(&pckhead.filenametable3.size,1,4,outfile);
	fwrite(pckhead.filenametable3.buffer,1,(int)pckhead.filenametable4.buffer-(int)pckhead.filenametable3.buffer,outfile);
	fclose(outfile);
		
	/*
	(int)pckhead.table1.buffer += (int)buffer;	
	outfile = fopen("var1.dat","wb");
	fwrite(pckhead.table1.buffer,1,pckhead.table1.size*4,outfile);
	fclose(outfile);
	
	(int)pckhead.globalvartable.buffer += (int)buffer;
	outfile = fopen("var2.dat","wb");
	fwrite(pckhead.globalvartable.buffer,1,pckhead.globalvartable.size*4,outfile);
	fclose(outfile);
	
	pckhead.globalvartablestr.size = (int)pckhead.filenametable1.buffer - (int)pckhead.globalvartablestr.buffer;
	(int)pckhead.globalvartablestr.buffer += (int)buffer;
	outfile = fopen("var3.dat","wb");
	fwrite(pckhead.globalvartablestr.buffer,1,pckhead.globalvartablestr.size,outfile);
	fclose(outfile);
	*/
	
	free(buffer);	
	fclose(infile);

	return 0;
}
