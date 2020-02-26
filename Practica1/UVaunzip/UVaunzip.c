#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char const *argv[]){
    if(argc==1){
        printf("UVaunzip: file1 [file2 ...]\n");
        exit(1);
    }
    else
    {
        int * bufferBytes = (int *)malloc(sizeof(int)*500);
        char * caracter = (char *)malloc(1);
        for (size_t i = 1; i < argc; i++)
        {
            FILE *fr = fopen(argv[i],"r");
            if(fr==NULL){
                printf("UVaunzip: cannot open file\n");
                exit(1);
            }
            while (fread(bufferBytes,sizeof(int),1,fr))
            {   
                int iteraciones = *bufferBytes;
                fread(caracter,sizeof(char),1,fr);
                //printf("%d%s\n",*bufferBytes,caracter);
                for (size_t i = 0; i < iteraciones; i++)
                {
                    printf("%s",caracter);
                }                
            }
        }
        
    }
    
    exit(0);
}