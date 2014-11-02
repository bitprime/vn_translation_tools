#ifndef __LZSS_H__
#define __LZSS_H__

char *LZEncode(char*, size_t, size_t*);
void LZDecode(char*, size_t, char*, size_t*);

#endif