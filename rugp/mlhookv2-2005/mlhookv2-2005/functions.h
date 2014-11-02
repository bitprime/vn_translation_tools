#include "global.h"

#ifndef __PATCHES_H__
#define __PATCHES_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "functions-memory.h"
#include "functions-graphic.h"
#include "functions-text.h"

#pragma pack(1)
struct AmiEntry
{
	int file;
	int offset;
	int size;
	int compsize;
};

struct ScrEntry
{
	int offset;
	int len;
	int id;
};

struct Entry
{
	char *buffer;
	int buffer2;
	int context; // in what context is this line? the file offset should be stored here
	int len;
	int id;
	char *str;
};

struct ImageHeader
{
	char magic[4];
	short x, y;
	short w, h;
	int buffer; // should always be blank in the file
	unsigned char *bg;
	int trans;
};
#pragma pack()

extern FILE *dump; // output file when dumping scripts
extern FILE *scrlog; // log the scripts as they get dumped

extern AmiEntry *amientry;
extern Entry *entry;
extern ImageHeader *imgentry;

extern int parsed;
extern int entrycount;
extern int readFile;
extern int errorDisp;
extern int amicount;
extern int baseOffset;
extern int filetype;
extern int curFile;
extern int positionAddress;
extern int bufferAddressCG;
extern int bufferAddressUI;
extern int wroteImage;

#endif
