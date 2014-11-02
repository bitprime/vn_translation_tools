#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile,*outfile;
	char *buffer1,*buffer2;
	int fsize=0,i=0,w=0,h=0,x=0,z=0,type=0,bpp=0;
	
	if(argc!=4)
	{
		printf("usage: %s.exe inputType inputBMP outputBMP\n\nTypes:\n1 = Nscripter\n2 = PS2",argv[0]);
		return 0;
	}
    
    type = atoi(argv[1]);
    
    if(type!=1 && type!=2)
    {
		printf("usage: %s.exe type inputBMP outputBMP\n\nTypes:\n1 = Nscripter\n2 = PS2",argv[0]);
		return 0;
    }
	
	infile = fopen(argv[2],"rb");
	
    if(!infile)
    {
        printf("Could not open %s\n",argv[2]);
        return -1;
    }
    
	mkdir("output");
	chdir("output");
	
	outfile = fopen(argv[3],"wb");
	
    fseek(infile,2,SEEK_SET);
    fread(&fsize,1,4,infile);
    fseek(infile,0x12,SEEK_SET);
    fread(&w,1,4,infile);
    fread(&h,1,4,infile);
    fseek(infile,2,SEEK_CUR);
    fread(&bpp,1,4,infile);
    
    if(type==1)
    {	
    	fwrite("BM",1,2,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x7a\x00\x00\x00",1,4,outfile);
    	fwrite("\x6c\x00\x00\x00",1,4,outfile);
    	i=w/2;
    	fwrite(&i,1,4,outfile);
    	fwrite(&h,1,4,outfile);
    	fwrite("\x01\x00\x20\x00",1,4,outfile);
    	fwrite("\x03\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\xFF\x00",1,4,outfile);
	    fwrite("\x00\xFF\x00\x00",1,4,outfile);
	    fwrite("\xFF\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\xFF",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	
	    fseek(infile,0x36,SEEK_SET);
	    for(i=0; i<h; i++)
	    {
		    buffer1 = (char*)calloc(w*3,sizeof(char*));
		    buffer2 = (char*)calloc(w*3,sizeof(char*));
    		
		    fread(buffer1,1,w*3/2,infile);
		    fread(buffer2,1,w*3/2,infile);
    		
		    /*for(x=0; x<w*3/2; x++)
		    {
    			buffer2[x] = ~buffer2[x];
		    }*/
    		
		    for(x=0; x<w*3/2; x++)
		    {
			    if(x!=0 && x%3==0)
				    fwrite(&buffer2[x-1],1,1,outfile);
    				
    			fwrite(&buffer1[x],1,1,outfile);
		    }
    		
		    // Get that last byte in
		    if(x!=0 && x%3==0)
			    fwrite(&buffer2[x-1],1,1,outfile);
    		
		    for(x=0; x<w/2%4-3; x++)
			    fwrite("\x00",1,1,outfile);
    			
		    fseek(infile,w%4,SEEK_CUR);
    		
		    free(buffer1);
		    free(buffer2);
    	}
    	
	    fsize = ftell(outfile);
	    fseek(outfile,0x2,SEEK_SET);
	    fwrite(&fsize,1,4,outfile);
	    fclose(outfile);
    }
    else if(type==2)
    {
        int hdrSize=0;
        
        if(bpp!=32)
        {
            printf("Must be a 32bit BMP, this is a %dbit BMP\n",bpp);
            return -1;
        }
        
        fseek(infile,0x0a,SEEK_SET);
        fread(&hdrSize,1,4,infile);
        
        rewind(infile);
        
        fsize+=0x44;
    	fwrite("BM",1,2,outfile);
        fwrite(&fsize,1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x7a\x00\x00\x00",1,4,outfile);
    	fwrite("\x6c\x00\x00\x00",1,4,outfile);
    	fwrite(&w,1,4,outfile);
    	fwrite(&h,1,4,outfile);
    	fwrite("\x01\x00\x20\x00",1,4,outfile);
    	fwrite("\x03\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\x00\x00",1,4,outfile);
	    fwrite("\x00\x00\xFF\x00",1,4,outfile);
	    fwrite("\x00\xFF\x00\x00",1,4,outfile);
	    fwrite("\xFF\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\xFF",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x01\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
    	fwrite("\x00\x00\x00\x00",1,4,outfile);
        
        fseek(infile,hdrSize,SEEK_SET);
        for(i=0; i<w*h; i++)
        {
            unsigned int colorBuff = 0;
            
            fread(&colorBuff,1,4,infile);
            
            if((colorBuff&0xff000000)>0)
                colorBuff = ((((((colorBuff&0xff000000)>>24)-1)*2)+1)<<24)+(colorBuff&0x00ffffff);
            
            colorBuff = ((~((colorBuff&0xff000000)>>24))<<24)+(colorBuff&0x00ffffff);
            
            fwrite(&colorBuff,1,4,outfile);
        }
    	
	    fsize = ftell(outfile);
	    fseek(outfile,0x2,SEEK_SET);
	    fwrite(&fsize,1,4,outfile);
	    fclose(outfile);
    }
    
	fclose(infile);
	
	chdir("..");
	
	return 0;
}
