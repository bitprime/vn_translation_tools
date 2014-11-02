#define _CRT_SECURE_NO_WARNINGS

#define MUVLUVEXUL 1
#define KIMINOZOLE 2
#define MUVLUVALT 3

//#define GAME MUVLUVEXUL // valid = (1) MUVLUVEXUL, (2) KIMINOZOLE, (3) MUVLUVALT
#define GAME MUVLUVALT // valid = (1) MUVLUVEXUL, (2) KIMINOZOLE, (3) MUVLUVALT
//#define GAME KIMINOZOLE // valid = (1) MUVLUVEXUL, (2) KIMINOZOLE, (3) MUVLUVALT
#define EXACTGAME GAME

#define HASHKEY 0x57357357 // the key used to generate the hash

#define LOGCHOICES 0 // log choices as they are displayed in game?
#define DUMPMEM 0 // dump scripts. set to 1 to dump them. must have DEBUG set to 1
#define DEBUG 0 // debug warnings/errors level
#define DEBUGIMG 0 // set to 3 to dump image list
#define SKIPTEXT 0 // enable glitchy text which forces the engine to keep continuing
#define DEBUGIDS 0
#define SELFFIX 0
#define MLALT_FIXBOAT 1 // force the check for the save checking stuff that wasn't in mlalt 18+ normally

#if GAME == MUVLUVEXUL
#define REGSTR "Software\\relic UGP Applications\\age\\マブラヴ11\\Installation\0"
#elif GAME == MUVLUVALT
#define REGSTR "Software\\relic UGP Applications\\age\\マブラヴオルタネイティヴ\\Installation\0"
#elif GAME == KIMINOZOLE
#define REGSTR "Software\\relic UGP Applications\\age\\君が望む永遠LE\\Installation\0"
#else
#define REGSTR ""
#endif