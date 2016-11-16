#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef uint8_t BYTE;


#define BLOCKSIZE 512

int main (void)
{
    int jpgnum = 0;
    
	FILE* fp = fopen("card.raw", "r");


    if (fp == NULL)
    {
        fprintf(stderr,"Could not open file.\n");
        return 1;
    }
    
   
    FILE *outfile;
    outfile = NULL;
    do
    {
        BYTE block[BLOCKSIZE] = {};
        
      
        int i=0;
        while(i<BLOCKSIZE)
        {
           
            if (feof(fp))
            {
                fclose(fp);
                
               
                if (outfile != NULL)
                    fclose(outfile);
                    
                return 0;
            }
            
           
            fread(&block[i], sizeof (BYTE), 1, fp);
            
            i++;
        }
        
       
        if ((block[0] == 0xff) && (block[1] == 0xd8) && (block[2] == 0xff) && (block[3] == 0xe0 || block[3] == 0xe1))
        {
           
            if (outfile != NULL)
            {
                fclose(outfile);
            }
            
         
            char filename[4];
            
           
            sprintf(filename, "%03d.jpg", jpgnum);
            jpgnum++; 
            
         
            if ((outfile = fopen(filename, "w")) == NULL)
            {
               fprintf(stderr,"Could not write image.\n");
                return 1;
            }
 
           
            fwrite(&block[0], BLOCKSIZE * sizeof (BYTE), 1, outfile);
        }
       
        else if (outfile != NULL) 
        {
            fwrite(&block[0], BLOCKSIZE * sizeof (BYTE), 1, outfile);
        }
     }while(1);
   
    fclose(fp);
 
   
    return 0;
}