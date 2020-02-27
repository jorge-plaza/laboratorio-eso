#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define NUMEROARG 10

int main(){
    size_t longitudEntrada = 50;
    char * input = (char *) malloc(sizeof(char)*longitudEntrada);
    char * salida = "exit";
    char * salto = "\n";
    char *argumentos[NUMEROARG];
    do
    {
        printf("UVash> ");
        getline(&input,&longitudEntrada,stdin);//TODO mirar strcspn para quitar el final de linea
        input[strcspn(input, "\n")]='\0';
        int i;
        //Tienes que crear un vector auxiliar ya que strsep() te modifica el vector que le pasas por agumentos
        char * aux = input;
        for (i = 0; i < NUMEROARG && (argumentos[i] = strsep(&aux, " ")) != NULL; i++);
        //for (int c = 0; c < i; c++) printf(" arg %d : [%s]\n", c, argumentos[c]);
        if (strcmp(argumentos[0],salto))
        {
            char * orden = argumentos[0];
            if(strcmp(orden,salida)==0) exit(0);
            if (fork()==0)
            {
                execvp(orden,argumentos);
                kill(getpid(),SIGTERM);
            }
            else
            {
                int status;
                wait(&status);
            }
            
        }
        
    } while (strcmp(salida,input));

    exit(0);
}