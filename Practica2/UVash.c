#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    size_t longitudEntrada = 50;
    char * input = (char *) malloc(sizeof(char)*longitudEntrada);
    char * salida = "exit\n";
    do
    {
        printf("UVash> ");
        getline(&input,&longitudEntrada,stdin);
    } while (strcmp(salida,input));

    exit(0);
}