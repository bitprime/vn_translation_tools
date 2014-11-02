#ifndef __PATCHES_MEMORY_H__
#define __PATCHES_MEMORY_H__
#include "patches.h"
#include "functions-memory.h"

void MemPatch_StoreBufferStruct();

#if GAME == MUVLUVEXUL
void MemPatch_StorePositionAddress1();
void MemPatch_StorePositionAddress2();
void MemPatch_HookMalloc();
void MemPatch_StoreBufferAddress();
#elif GAME == MUVLUVALT
void MemPatch_StorePositionAddress1();
void MemPatch_StorePositionAddress2();
void MemPatch_StorePositionAddress3();
void MemPatch_HookMalloc();
void MemPatch_StoreBufferAddress();
#elif GAME == KIMINOZOLE
void MemPatch_StorePositionAddress1();
void MemPatch_StorePositionAddress2();
void MemPatch_HookMalloc();
void MemPatch_StoreBufferAddress();
#endif

#endif
