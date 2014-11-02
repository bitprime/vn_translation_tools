#include <stdio.h>
#include <stdlib.h>

#pragma pack(1)
struct
{
    char magic[6];
    short ver;
    int dataStart;
    int null;
    int byteStart;
    int byteSize;
    int tableStart;
    int tableSize;
    int resStart;
    int resSize;
    int unk1;
    int unk2;
} YKSHead;

struct
{
    int opcode;
    int argCount;
    int *args;
} *opcodeTable;

struct
{
    int type;
    int arg1;
    int arg2;
    int arg3;
} *offsetTable;

struct
{
    char *resource1;
    char *resource2;
    char *resource3;
} *resTable;

int main(int argc, char **argv)
{
    FILE *infile = NULL;
    FILE *outfile = NULL;
    int i=0,x=0;
    int curPos = 0;
    
    if(argc!=3)
    {
        printf("Usage: %s input.yks output.xml\n",argv[0]);
        exit(1);
    }
    
    infile = fopen(argv[1],"rb");
    if(!infile)
    {
        printf("Could not open %s\n",argv[1]);
        exit(1);
    }
    
    outfile = fopen(argv[2],"w");
    
    // checkScript()
    fread(&YKSHead,1,sizeof(YKSHead),infile);
    
    if(strncmp("YKS001",YKSHead.magic,6)!=0)
    {
        fprintf(outfile,"Invalid YKS script\n");
        return 1;
    }
    
    offsetTable = calloc(YKSHead.tableSize,sizeof(*offsetTable));
    fseek(infile,YKSHead.tableStart,SEEK_SET);
    for(i=0; i<YKSHead.tableSize; i++)
    {
        fread(&offsetTable[i],1,sizeof(*offsetTable),infile);
    }
    
    opcodeTable = calloc(YKSHead.byteSize,sizeof(*opcodeTable));
    fseek(infile,YKSHead.byteStart,SEEK_SET);
    for(i=0,curPos=0; curPos<YKSHead.byteSize; i++,curPos++)
    {
        fread(&opcodeTable[i].opcode,1,4,infile);
        
        if(offsetTable[opcodeTable[i].opcode].type==0)
        {
            fread(&opcodeTable[i].argCount,1,4,infile);
            opcodeTable[i].args = (int*)calloc(opcodeTable[i].argCount,sizeof(int));
            
            curPos++;
            
            for(x=0; x<opcodeTable[i].argCount; x++,curPos++)
            {
                fread(&opcodeTable[i].args[x],1,4,infile);
            }
        }
    }
    
    resTable = calloc(YKSHead.resSize,sizeof(*resTable));
    fseek(infile,YKSHead.resStart,SEEK_SET);
    
    fprintf(outfile,"<?xml version=\"1.0\" encoding=\"Shift-JIS\"?>\n");
    fprintf(outfile,"<code name=\"%s\">\n",argv[1]);
    for(x=0; x<i; x++)
    {
        printf("<%04x> Struct[%04x]:\n\t[1] %08x\n\t[2] %08x\n\t[3] %08x\n\t[4] %08x\n",curPos,opcodeTable[x].opcode,offsetTable[opcodeTable[x].opcode].type,offsetTable[opcodeTable[x].opcode].arg1,offsetTable[opcodeTable[x].opcode].arg2,offsetTable[opcodeTable[x].opcode].arg3);
        
        switch(offsetTable[opcodeTable[x].opcode].type)
        {
            case 0x00:
                {
                    char **tempStr = (char**)calloc(opcodeTable[x].argCount+1,sizeof(char*));
                    int tempVar = 0;
                    int r=0;
                    
                    for(r=0; r<opcodeTable[x].argCount+1; r++)
                        tempStr[r] = (char*)calloc(128,sizeof(char));
            
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg1,SEEK_SET);
                    fread(tempStr[0],1,128,infile);
                    
                    fprintf(outfile,"\t<function name=\"%s\" args=\"%d\" id=\"%d\">\n",tempStr[0],opcodeTable[x].argCount,offsetTable[opcodeTable[x].opcode].arg2);
                    
                    for(r=1; r<opcodeTable[x].argCount+1; r++)
                    {   
                        switch(offsetTable[opcodeTable[x].args[r-1]].type)
                        {
                            case 1:
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg1,SEEK_SET);
                                fread(&tempVar,1,2,infile);
                                fprintf(outfile,"\t\t<unk1>%u, ",tempVar);
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg2,SEEK_SET);
                                fread(&tempVar,1,2,infile);
                                fprintf(outfile,"%u</unk1>\n",tempVar);
                                break;
                            case 4:
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg2,SEEK_SET);
                                fread(&tempVar,1,4,infile);
                                fprintf(outfile,"\t\t<num>%u</num>\n",tempVar);
                                break;
                            case 5:
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg2,SEEK_SET);
                                fread(tempStr[r],1,128,infile);
                                fprintf(outfile,"\t\t<string>%s</string>\n",tempStr[r]);
                                break;
                            case 8:
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg1,SEEK_SET);
                                fread(tempStr[r],1,128,infile);
                                fprintf(outfile,"\t\t<var name=\"%s\">",tempStr[r]);
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg3,SEEK_SET);
                                fread(&tempVar,1,4,infile);
                                fprintf(outfile,"%u</var>\n",tempVar);
                                break;
                            case 9:
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg1,SEEK_SET);
                                fread(tempStr[r],1,128,infile);
                                fprintf(outfile,"\t\t<varstr name=\"%s\">",tempStr[r]);
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg3,SEEK_SET);
                                fread(&tempVar,1,4,infile);
                                fprintf(outfile,"%u</varstr>\n",tempVar);
                                break;
                            case 10:
                                fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].args[r-1]].arg2,SEEK_SET);
                                fprintf(outfile,"\t\t<unk2>%u</unk2>\n",offsetTable[opcodeTable[x].args[r-1]].arg2);
                                break;
                            default:
                                printf("New func opcode: %x\n",offsetTable[opcodeTable[x].args[r-1]].type); 
                                exit(1);
                                break;
                        }
                    }
                    
                    fprintf(outfile,"\t</function>\n\n");
                    free(tempStr);
                }
                break;
            case 0x01:
                {
                    // args 1 and 2, both strings
                    int tempVar = 0;
                    
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg1,SEEK_SET);
                    fread(&tempVar,1,2,infile);
                    fprintf(outfile,"\t<unk1>%u, ",tempVar);
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg2,SEEK_SET);
                    fread(&tempVar,1,2,infile);
                    fprintf(outfile,"%u</unk1>\n\n",tempVar);                
                }
                break;
            case 0x04:
                {
                    /*
                        arg2 = pointer to num
                    */
                    char *tempStr = (char*)calloc(128,sizeof(char));
                    int tempVar = 0;
                    
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg2,SEEK_SET);
                    fread(&tempVar,1,4,infile);
                    
                    printf("%x\n",ftell(infile));    
                    fprintf(outfile,"\t<num>%d</num>\n\n",tempStr,tempVar);
                
                    free(tempStr);
                }
                break;
            case 0x08:
                {
                    /*
                        arg1 = pointer to variable name
                        arg3 = pointer to num to store in variable
                    */
                    char *tempStr = (char*)calloc(128,sizeof(char));
                    int tempVar = 0;
                    
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg1,SEEK_SET);
                    fread(tempStr,1,128,infile);
                    
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg3,SEEK_SET);
                    fread(&tempVar,1,4,infile);
                        
                    fprintf(outfile,"\t<var name=\"%s\">%d</var>\n\n",tempStr,tempVar);
                
                    free(tempStr);
                }
                break;
            case 0x09:
                {
                    /*
                        arg1 = pointer to variable name
                        arg3 = pointer to num to store variable
                    */
                    char *tempStr = (char*)calloc(128,sizeof(char));
                    int tempVar = 0;
                    
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg1,SEEK_SET);
                    fread(tempStr,1,128,infile);
                    
                    fseek(infile,YKSHead.resStart+offsetTable[opcodeTable[x].opcode].arg3,SEEK_SET);
                    fread(&tempVar,1,4,infile);
                        
                    fprintf(outfile,"\t<varstr name=\"%s\">%d</varstr>\n\n",tempStr,tempVar);
                
                    free(tempStr);
                }
                break;
            case 0x0a:
                fprintf(outfile,"\t<unk2>%d</unk2>\n\n",offsetTable[opcodeTable[x].opcode].arg2);
                break;
            default:
                printf("New opcode: %x %x\n",offsetTable[opcodeTable[x].opcode].type,ftell(infile));
                exit(1);
                break;
        }
    }
    
    fprintf(outfile,"</code>");

    return 0;
}
