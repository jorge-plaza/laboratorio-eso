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
void separarCadena(char * input,char ** cadenaSep,char * delimitador);
void sinRedireccion(char * input,char ** cadenaSep);
void strip(char *s);
int numChar(char *arr[]);

int main(int argc, char **argv){
    size_t longitudEntrada = 50;
    char * input = (char *) malloc(sizeof(char)*longitudEntrada);
    char * salida = "exit";
    char * cd = "cd";
    char *cadenaSep[NUMEROARG];
    char * salto = "\n";
    char error_message[30] = "An error has occurred\n";
    if(argc>1){
        //MODO BATCH
        if(argc!=2) fprintf(stderr, "%s", error_message);
        FILE *fr = fopen(argv[1],"r");
        if(fr==NULL){
            printf("UVazip: cannot open file\n");
            exit(1);
        }
        int fallo = getline(&input,&longitudEntrada,fr);
        if (fallo==-1)
        {
            exit(1);
        }
        do
        {
            input[strcspn(input, "\n")]='\0';
            separarCadena(input,cadenaSep,">");
            if (cadenaSep[1]==NULL)
            {
                sinRedireccion(input,cadenaSep);
            }
            //HAY REDIRECCION
            else{
                if (cadenaSep[1][0]==10 || cadenaSep[1][0]==0)
                {
                    fprintf(stderr, "%s", error_message);
                    continue;
                }
                char * trasR[NUMEROARG];
                separarCadena(cadenaSep[1],trasR," ");
                char * pathFichero;
                char ** punteroSig = &trasR[2];
                char ** siguienteComando = (char**)malloc(sizeof(char)*NUMEROARG);
                if (trasR[0][0]==0)//HAY UN ESPACIO DELANTE
                {
                    pathFichero = trasR[1];
                    if(trasR[2]!=NULL){
                        int iterador =0;
                        while (*punteroSig!=NULL)
                        {
                            *siguienteComando=*punteroSig;
                            punteroSig++;
                            siguienteComando++;
                            iterador++;
                        }
                        for (size_t i = 0; i < iterador; i++)
                        {
                            //DECREMENTO PUNTEROS PARA QUE APUNTEN AL INICIO
                            punteroSig--;
                            siguienteComando--;
                        }
                    }
                }else
                {
                    pathFichero = trasR[0];
                    if(trasR[1]!=NULL){
                        int iterador =0;
                        while (*punteroSig!=NULL)
                        {
                            *siguienteComando=*punteroSig;
                            punteroSig++;
                            siguienteComando++;
                            iterador++;
                        }
                        for (size_t i = 0; i < iterador; i++)
                        {
                            //DECREMENTO PUNTEROS PARA QUE APUNTEN AL INICIO
                            punteroSig--;
                            siguienteComando--;
                        }
                    }
                }
                //RemoveSpaces(pathFichero);
                strip(pathFichero);
                input = cadenaSep[0];
                separarCadena(input,cadenaSep," ");
                char * orden;
                if((int)*cadenaSep[0]!=0)orden = cadenaSep[0];
                else{fprintf(stderr, "%s", error_message); continue;}
                //printf("num (%ld)\n",strlen(*cadenaSep));//TODO QUITAR EL ESPACIO SI METE UN ARGUMENTO
                //printf("cadenaSep (%d)\n",(int)*cadenaSep[0]);
                //cadenaSep[strlen(*cadenaSep)]=NULL;
                
                int numElementos = numChar(cadenaSep);
                cadenaSep[numElementos-1] = NULL;
                pid_t pid;
                if((pid = fork())<0){
                    fprintf(stderr, "%s", error_message);
                    exit(1);
                }
                else if(pid==0){
                    int fdout = open(pathFichero,O_WRONLY | O_CREAT | O_TRUNC, 0777);
                    if(fdout==-1) exit(1);
                    int test = 0;
                    int test2 = 0;
                    test = dup2(fdout, STDOUT_FILENO);
                    test2 = dup2(fdout, STDERR_FILENO);
                    //test = fcntl(1, F_DUPFD, fdout);
                    if (test==-1 || test2==-1){
                        exit(1);
                    }
                    close(fdout);
                    execvp(orden,cadenaSep);
                    kill(getpid(),SIGTERM);
                }else{
                    int status;
                    while (wait(&status)!=pid){}
                }
                if (*siguienteComando!=NULL)
                {
                    //TODO HACER BIEN EJECUTAR EL SIGUIENTE COMANDO
                    orden=*siguienteComando;
                    if (fork()==0)
                    {
                        execvp(orden,siguienteComando);
                        fprintf(stderr, "%s", error_message);//solo devuelve algo si es un error, -1 y se guarda TODO MIRAR manual
                        kill(getpid(),SIGTERM);
                    }
                    else
                    {
                        int status;
                        wait(&status);
                    }
                }
            }
        } while (getline(&input,&longitudEntrada,fr)!=-1);
        exit(0);
    }
    //MODO INTERACTIVO
    do
    {
        printf("UVash> ");
        getline(&input,&longitudEntrada,stdin);//TODO mirar strcspn para quitar el final de linea
        input[strcspn(input, "\n")]='\0';
        separarCadena(input,cadenaSep,"&");
        if(cadenaSep[1]==NULL){
            //Tienes que crear un vector auxiliar ya que strsep() te modifica el vector que le pasas por agumentos
            input=cadenaSep[0];
            separarCadena(input,cadenaSep,">");
            if (cadenaSep[1]==NULL)
            {
                //Si no se ha introducido la redireccion hay que separar los argumetos
                input = cadenaSep[0];
                separarCadena(input,cadenaSep," ");
                if (strcmp(cadenaSep[0],salto)){
                    char * orden = cadenaSep[0];
                    cadenaSep[strlen(*cadenaSep)]='\0';
                    if(strcmp(orden,salida)==0) exit(0);
                    if(strcmp(orden,cd)==0){
                        char * destino = cadenaSep[1];
                        if(chdir(destino)==0) continue;
                        else fprintf(stderr, "%s", error_message);
                    }
                    else if (fork()==0)
                    {
                        execvp(orden,cadenaSep);//solo devuelve algo si es un error, -1 y se guarda TODO MIRAR manual
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
                char * pathFichero = cadenaSep[1];
                RemoveSpaces(pathFichero);
                input = cadenaSep[0];
                separarCadena(input,cadenaSep," ");
                char * orden = cadenaSep[0];
                cadenaSep[strlen(*cadenaSep)]='\0';
                //int fdout = open(pathFichero,O_WRONLY | O_CREAT | O_TRUNC, 0600);
                FILE * fichero = fopen(pathFichero,"w");
                int fdout = fileno(fichero);
                switch (fork()) {
                case -1:
                        //error;
                case 0:
                    dup2(fdout, STDOUT_FILENO);
                    dup2(fdout, STDERR_FILENO);
                    fclose(fichero);
                    execvp(orden,cadenaSep);
                    kill(getpid(),SIGTERM);
                    //TODO FALTARIA EL WAIT?
                }
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
void separarCadena(char * input,char ** cadenaSep, char * delimitador){
    char * aux = input;
    for (int i = 0; i < NUMEROARG && (cadenaSep[i] = strsep(&aux, delimitador)) != NULL; i++);
}
void sinRedireccion(char * input,char ** cadenaSep){
    char * salto = "\n";
    char * cd = "cd";
    char * salida = "exit";
    char error_message[30] = "An error has occurred\n";
    input[strcspn(input, "\n")]='\0';
    separarCadena(input,cadenaSep," ");
    if (strcmp(cadenaSep[0],salto)){
        char * orden = cadenaSep[0];
        int numElementos = numChar(cadenaSep);
        //printf("elementos cadena: %d\n",numChar(cadenaSep));
        cadenaSep[numElementos]='\0';
        if(strcmp(orden,salida)==0){
            if(cadenaSep[1]!=NULL)fprintf(stderr, "%s", error_message);
            exit(0);
        } 
        if(strcmp(orden,cd)==0){
            char * destino = cadenaSep[1];
            if(chdir(destino)!=0)fprintf(stderr, "%s", error_message);
        }
        else if (fork()==0)
        {
            execvp(orden,cadenaSep);
            fprintf(stderr, "%s", error_message);//solo devuelve algo si es un error, -1 y se guarda TODO MIRAR manual
            kill(getpid(),SIGTERM);
        }
        else
        {
            int status;
            wait(&status);
        }
    }
}
void strip(char *s) {
    char *p2 = s;
    while(*s != '\0') {
        if(*s != '\t' && *s != '\n') {
            *p2++ = *s++;
        } else {
            ++s;
        }
    }
    *p2 = '\0';
}
int numChar(char *arr[]){
    int counter=0;
    for (int i=0; arr[i] != NULL; i++) {
        counter++;
    }
    return counter;
}