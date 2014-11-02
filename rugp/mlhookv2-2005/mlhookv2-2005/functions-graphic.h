#ifndef __FUNCTIONS_GRAPHIC_H__
#define __FUNCTIONS_GRAPHIC_H__
#include "functions.h"

int HookImage(int, int, int);
int HookImageUI();
void HookImageRGB(int, int, int);
void ParseImage();
int WriteTransparency(int, int);

extern int parsedImage;

#define WIDTH 0x258
#endif
