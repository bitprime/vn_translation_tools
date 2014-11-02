#ifndef __RESOURCES_H__
#define __RESOURCES_H__
#include "patches.h"
#include "rvmm.h" // resource dialog
#include "rvmm2.h" // menu text/dialogs
#include "rvmm-alt.h" // resource dialog
#include "rvmm2-alt.h" // menu text/dialogs
#include "vm60.h" // resource dialog
#include "vm60-alt.h" // resource dialog
#include "vm602.h" // menu text/dialogs
#include "vm603.h" // menu text/dialogs
#include "vm604.h" // menu text/dialogs
#include "vm605.h" // menu text/dialogs
#include "vm606.h" // menu text/dialogs
#include "vm607.h" // menu text/dialogs
#include "vm607-alt.h" // menu text/dialogs
#include "vm608.h" // menu text/dialogs
#include "vm609.h" // menu text/dialogs
#include "gmfc.h" // resource dialog
#include "gmfc-alt.h" // resource dialog

#if GAME == MUVLUVEXUL
void vm60_rsrcpatch1();
void vm60_rsrcpatch2();
void vm60_rsrcpatch3();
void vm60_rsrcpatch4();
void vm60_rsrcpatch5();
#elif GAME == MUVLUVALT
void vm60_rsrcpatch1();
void vm60_rsrcpatch2();
#endif
#endif
