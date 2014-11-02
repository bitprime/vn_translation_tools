#ifndef __FUNCTIONS_TEXT_H__
#define __FUNCTIONS_TEXT_H__
#include "functions.h"

void HookMain();
void DumpText(char *, 
#if GAME == MUVLUVALT
			  char *,
#endif
			  int);
void ParseArchive();
char *CheckVoiceName(char *);

#endif
