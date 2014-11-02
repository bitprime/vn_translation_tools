#include "global.h"

#ifndef __PATCHES_H__
#define __PATCHES_H__
#include "functions.h"
#include "patches-graphic.h"
#include "patches-memory.h"
#include "patches-text.h"
#include "patches-resources.h"

#define ENDFUNC int 0xff;
#define NAKED __declspec(naked)

typedef struct
{
	char *dll;
	int offset;
	char *func;
	int len;
	unsigned char *buffer;
	int type;
} Patch;

enum { PATCH = 0, JMP, SKIP, DATA };

int GetPatchLength(char *);
void PatchDLL();
#endif