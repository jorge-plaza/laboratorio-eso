#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUMEROARG 10
void RemoveSpaces(char* source);

int main(){
    size_t longitudEntrada = 50;
    char * input = (char *) malloc(sizeof(char)*longitudEntrada);
    char * salida = "exit";
    char * cd = "cd";
    char * salto = "\n";
    char *cadenaSep[NUMEROARG];
    do
    {
        printf("UVash> ");
        getline(&input,&longitudEntrada,stdin);//TODO mirar strcspn para quitar el final de linea
        input[strcspn(input, "\n")]='\0';
        int i;
        //Tienes que crear un vector auxiliar ya que strsep() te modifica el vector que le pasas por agumentos
        char * aux = input;
        for (i = 0; i < NUMEROARG && (cadenaSep[i] = strsep(&aux, ">")) != NULL; i++);
        if (cadenaSep[1]==NULL)
        {
            //Si no se ha introducido la redireccion hay que separar los argumetos
            char * argumentos[NUMEROARG];
            char * aux2 = cadenaSep[0];
            for (i = 0; i < NUMEROARG && (argumentos[i] = strsep(&aux2, " ")) != NULL; i++);
            if (strcmp(argumentos[0],salto))
            {
                char * orden = argumentos[0];
                if(strcmp(orden,salida)==0) exit(0);
                if(strcmp(orden,cd)==0){
                    char * destino = argumentos[1];
                    if(chdir(destino)==0) continue;
                    else printf("ha ocurrido un error al cambiar de directorio\n");//TODO gestionar errores
                }
                else if (fork()==0)
                {
                    execvp(orden,argumentos);//solo devuelve algo si es un error, -1 y se guarda TODO MIRAR manual
                    kill(getpid(),SIGTERM);
                }
                else
                {
                    int status;
                    wait(&status);
                }
            }   
        }
        else
        {
            //Se ha encontrado la redireccion hay que guardar el fichero
            RemoveSpaces(cadenaSep[1]);
            char * pathFichero = cadenaSep[1];
            char * argumentos[NUMEROARG];
            char * aux2 = cadenaSep[0];
            for (i = 0; i < NUMEROARG && (argumentos[i] = strsep(&aux2, " ")) != NULL; i++);
            char * orden = argumentos[0];
            argumentos[strlen(*argumentos)-1]='\0';
            int fdout = open(pathFichero,O_WRONLY | O_CREAT | O_TRUNC, 0600);
            switch (fork()) {
            case -1:
                    //error;
            case 0:
                dup2(fdout, STDOUT_FILENO);
                dup2(fdout, STDERR_FILENO);
                close(fdout);
                execvp(orden,argumentos);
                kill(getpid(),SIGTERM);
                //TODO FALTARIA EL WAIT?
            }
        }        
    } while (strcmp(salida,input));
    exit(0);
}

void RemoveSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}