#ifndef __PATCHES_GRAPHIC_H__
#define __PATCHES_GRAPHIC_H__
#include "patches.h"
#include "functions-graphic.h"

static int followBuffer = 0;

#if GAME == MUVLUVEXUL
void GfxPatch_GetImageSize();
void GfxPatch_HookImage();
void GfxPatch_HookImageUI();
void GfxPatch_HookImageRGB();
#elif GAME == MUVLUVALT
void GfxPatch_GetImageSize();
void GfxPatch_GetImageSize2();
void GfxPatch_HookImage();
void GfxPatch_HookImage2();
void GfxPatch_HookImage3();
void GfxPatch_HookImageUI();
void GfxPatch_HookImageRGB();
#elif GAME == KIMINOZOLE
void GfxPatch_GetImageSize();
void GfxPatch_HookImage();
void GfxPatch_HookImageUI();
void GfxPatch_HookImageRGB();
#endif

#endif
