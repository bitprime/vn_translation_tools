#define DEBUGMSG 0
#define DEBUG 2
#define resw 800*4
#define resh 600*4
#define HASHKEY 0x57357357
 
extern "C" _declspec(dllexport) int HookText(int *, int);
extern "C" _declspec(dllexport) void DumpText(int*, int*, int);
extern "C" _declspec(dllexport) void StoreBufferAddress();
extern "C" _declspec(dllexport) void StorePositionAddress();
extern "C" _declspec(dllexport) void HookImage();

void DumpScript(int*);
void ParseArchive();
void ParseScr(int);
unsigned int GenHash(char *, unsigned int, int);

#pragma pack(1)
typedef struct AmiEntry
{
    int file;
    int offset;
    int size;
    int compsize;
};

typedef struct ScrEntry
{
    int offset;
    int len;
    int id;
};

typedef struct Entry
{
    int buffer;
    int context; // in what context is this line? the file offset should be stored here
    int len;
    int id;
    char *str;
};

typedef struct ImageHeader
{
    char magic[4];
    short x, y;
    short w, h;
    int buffer; // should always be blank in the file
    unsigned char *bg;
    int read;
};
#pragma pack()

AmiEntry *amientry = NULL;
Entry *entry = NULL;
ImageHeader *imgentry = NULL;
FILE *filelog = NULL;
FILE *dump = NULL;
FILE *scrlog = NULL;
char *str = NULL;

// for text hooking-related things
#if DEBUG == 1 || DEBUG == 2
int dumpmem=1;
#else
int dumpmem=0;
#endif

#if DEBUG == 2
int logchoices=1;
#else
int logchoices=0;
#endif

int amt=0;
int scrnum=0;
int parsed=0;
int curFile=0;
int amicount = 0;
int scrcount = 0;
int baseOffset = 0;
int ec = 0; // entry count

// for image hooking-related things
int parsedImage = 0;
int bufferAddressCG = 0;
int bufferAddressUI = 0;
int positionAddress = 0;
int fileType = 0;
int scrType = 0;
int wroteImage = 0;
int srcBuffer = 0, destBuffer = 0;
int followBuffer = 0;
short xpos, ypos;
int w, h;
