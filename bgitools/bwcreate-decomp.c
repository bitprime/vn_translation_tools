/*
  -- INARE r41/a @ 2009-12-27 10:37:42 --
   bwcreate.exe;ia32;PE;win32;msvcrt;
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bw = 0x20207762;

FILE *fp_in;
char *pbuf2;

struct bwhead {
 int len;
 int bw;
 int ncount;
 int d1;
 int d2;
 int d3;
 int d4;
 int d5;
 int d6;
 int d7;
 int d8;
 int d9;
 int d10;
 int d11;
 int d12;
 int d13;
} h;

void read_input() {
 char a[8];
 int i;
 fread(a,1,5,fp_in);
 if(memcmp(a,"OggS",4)) {
  printf("Not a valid Ogg file\n");
  fclose(fp_in);
  exit(1);
 }
 fread(&h.d3,1,4,fp_in);
 fseek(fp_in,40,SEEK_SET);
 fread(&h.d2,1,4,fp_in);
 fseek(fp_in,76,SEEK_SET);
 fread(&h.d4,1,4,fp_in);
 fseek(fp_in,0,SEEK_END);
 h.ncount = ftell(fp_in);
 rewind(fp_in);
 pbuf2 = calloc(h.ncount,1);
 fread(pbuf2,1,h.ncount,fp_in);
 fclose(fp_in);
 i = h.ncount-7;
 if(i < h.ncount) {
  char *j = &pbuf2[i+4];
  do {
   if(i<=0)
    break;
   if(*(unsigned int *)(j-4)==0x5367674f && j[0]==0 && j[1]==5) {
    h.d1 = pbuf2[i+6];
    i += 6;
    break;
   }
   i--;
   j--;
  } while(i>=h.ncount);
 }
 if(!i) {
  printf("Could not find required information to convert this Ogg into a BGI Ogg\n");
  free(pbuf2);
  fclose(fp_in);
  exit(1);
 }
}

void write_and_close(char *fname) {
 FILE *fp_out = fopen(fname,"wb");
 fwrite(&h,1,64,fp_out);
 fwrite(pbuf2,1,h.ncount,fp_out);
 free(pbuf2);
 fclose(fp_out);
}

int main(int argc, char **argv) {
 if(argc!=3) {
  printf("usage: %s inputOgg outputBGIOgg\n",argv[0]);
  return 0;
 }
 if(!(fp_in=fopen(argv[1],"rb"))) {
  printf("Could not open %s\n",argv[1]);
  return -1;
 }
 h.len = 64;
 h.bw = bw;
 h.ncount = 0;
 h.d12 = 0;
 h.d13 = 0;
 h.d11 = 0;
 h.d8 = 0;
 h.d9 = 0;
 h.d6 = 0;
 h.d7 = 0;
 h.d5 = 0;
 h.d10 = 3;
 read_input();
 write_and_close(argv[2]);
 fclose(fp_in);
 return 0;
}
