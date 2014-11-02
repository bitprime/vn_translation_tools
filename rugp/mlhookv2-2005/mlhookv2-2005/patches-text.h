#ifndef __PATCHES_TEXT_H__
#define __PATCHES_TEXT_H__
#include "patches.h"
#include "functions-text.h"

void TextPatch_nTextSpeed1();
void TextPatch_nTextSpeed2();

#if GAME == MUVLUVEXUL
void TextPatch_arraySerifSetting();
void TextPatch_HookMain();
void TextPatch_DumpText();
#elif GAME == MUVLUVALT
static char vm60_nametagfix[1] = { 0x40 };
void TextPatch_arraySerifSetting();
void TextPatch_HookMain();
void TextPatch_DumpText();
void TextPatch_FixVoiceNames();
void TextPatch_FixVoiceNames2();
#elif GAME == KIMINOZOLE
void TextPatch_arraySerifSetting();
void TextPatch_HookMain();
void TextPatch_DumpText();
#endif

#endif
