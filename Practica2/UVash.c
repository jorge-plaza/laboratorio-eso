#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUMEROARG=10

int main(){
    size_t longitudEntrada = 50;
    char * input = (char *) malloc(sizeof(char)*longitudEntrada);
    char * salida = "exit\n";
    char *argumentos[NUMEROARG];
    do
    {
        printf("UVash> ");
        getline(&input,&longitudEntrada,stdin);
        int i;
        //Tienes que crear un vector auxiliar ya que strsep() te modifica el vector que le pasas por agumentos
        char * aux = input;
        for (i = 0; i < NUMEROARG && (argumentos[i] = strsep(&aux, " ")) != NULL; i++);
        //for (int c = 0; c < i; c++) printf(" arg %d : [%s]\n", c, argumentos[c]);
    } while (strcmp(salida,input));

    exit(0);
}