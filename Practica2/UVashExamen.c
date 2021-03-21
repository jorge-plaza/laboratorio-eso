/*
    GRUPO X4-8
    JORGE PLAZA LAZO
    MANUEL MENDEZ CALVO
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <fcntl.h>

#define NUMEROARG 10
void RemoveSpaces(char* source);
void separarCadena(char * input,char ** cadenaSep,char * delimitador);
void sinRedireccion(char * input,char ** cadenaSep);
void conRedireccion(char * input,char ** cadenaSep,bool paralelo);
void strip(char *s);
char * stripInicio(char *s);
char * stripFin(char *s);
char ** stripMedio(char *cadenaSep[]);
int numChar(char *arr[]);

int main(int argc, char **argv){
    size_t longitudEntrada = 50;
    char * input = (char *) malloc(sizeof(char)*longitudEntrada);
    char * salida = "exit";
    char *cadenaSep[NUMEROARG];
    char * salto = "\n";
    char error_message[30] = "An error has occurred\n";
    if(argc>1){
        //MODO BATCH
        if(argc!=2) {
            fprintf(stderr, "%s", error_message);
            exit(1);
        }
        FILE *fr = fopen(argv[1],"r");
        if(fr==NULL){
            fprintf(stderr, "%s", error_message);
            exit(1);
        }
        int fallo = getline(&input,&longitudEntrada,fr);        
        if (fallo==-1){
            fprintf(stderr, "%s", error_message);
            exit(1);
        }
        do
        {
            if(strcmp(input,salto)==0) continue;
            input[strcspn(input, "\n")]='\0';
            //BUSCAMOS COMANDOS EN PARALELO
            separarCadena(input, cadenaSep, "&");
            if ((int)*cadenaSep[0]==0 && (int)*cadenaSep[1]==0)
            {
                fprintf(stderr, "%s", error_message);
                continue;
            }
            //NO HAY COMANDO EN PARALELO
            if(cadenaSep[1]==NULL){
                separarCadena(input,cadenaSep,">");
                if (cadenaSep[1]==NULL){
                    sinRedireccion(input,cadenaSep);
                }
                //HAY REDIRECCION
                else{
                    conRedireccion(input,cadenaSep,false);
                }
            }
            //HAY COMANDO PARALELO
            else
            {
                int numComandos = numChar(cadenaSep);
                int i;
                char *comando=(char*)malloc(sizeof(char)*255);
                char * cadenaSepParalel[NUMEROARG];
                for(i=0;i<numComandos;i++){
                    strcpy(comando,cadenaSep[i]);
                    strcpy(input,cadenaSep[i]);
                    //BUSCAMOS REDIRECCION
                    separarCadena(input,cadenaSepParalel,">");
                    if (cadenaSepParalel[1]==NULL){
                        sinRedireccion(comando,cadenaSepParalel);
                    }
                    else{
                        conRedireccion(comando,cadenaSepParalel,true);
                    }
                }
                //ESPERAMOS A QUE TODOS ACABEN PARA PASAR AL SIGUIENTE COMANDO DEL FICHERO
                for (i = 0; i < numComandos; i++)
                {
                    wait(NULL);
                }  
            }
        } while (getline(&input,&longitudEntrada,fr)!=-1);
        exit(0);
    }
    //MODO INTERACTIVO
    do
    {
        printf("UVash> ");
        getline(&input,&longitudEntrada,stdin);
        if(strcmp(input,salto)==0) continue;
        input[strcspn(input, "\n")]='\0';
        separarCadena(input, cadenaSep, "&");
        if ((int)*cadenaSep[0]==0 && (int)*cadenaSep[1]==0)
        {
            fprintf(stderr, "%s", error_message);
            continue;
        }
        if(cadenaSep[1]==NULL){
            separarCadena(input,cadenaSep,">");
            if (cadenaSep[1]==NULL){
                sinRedireccion(input,cadenaSep);
            }
            //HAY REDIRECCION
            else{
                conRedireccion(input,cadenaSep,false);
            }
        }
        //HAY COMANDO PARALELO
        else
        {
            int numComandos = numChar(cadenaSep);
            int i;
            char *comando=(char*)malloc(sizeof(char)*255);
            char * cadenaSepParalel[NUMEROARG];
            for(i=0;i<numComandos;i++){
                strcpy(comando,cadenaSep[i]);
                strcpy(input,cadenaSep[i]);
                separarCadena(input,cadenaSepParalel,">");
                if (cadenaSepParalel[1]==NULL){
                    sinRedireccion(comando,cadenaSepParalel);
                }
                else{
                    conRedireccion(comando,cadenaSepParalel,true);
                }
            }
            for (i = 0; i < numComandos; i++)
            {
                wait(NULL);
            }  
        }
    } while (strcmp(salida,input));
    exit(0);
}
void separarCadena(char * input,char ** cadenaSep, char * delimitador){
    char * aux = input;
    for (int i = 0; i < NUMEROARG && (cadenaSep[i] = strsep(&aux, delimitador)) != NULL; i++);
}
void sinRedireccion(char * input,char ** cadenaSep){
    char * salto = "\n";
    char * cd = "cd";
    char * salida = "exit";
    char * michmod = "MiChmod";
    char error_message[30] = "An error has occurred\n";
    input[strcspn(input, "\n")]='\0';
    input[strcspn(input, "\t")]='\0';
    input = stripInicio(input);
    input = stripFin(input);
    separarCadena(input,cadenaSep," ");
    if (strcmp(cadenaSep[0],salto)){
        RemoveSpaces(cadenaSep[0]);
        if((int)*cadenaSep[0]!=0){
            char * orden = cadenaSep[0];
            int numElementos = numChar(cadenaSep);
            cadenaSep=stripMedio(cadenaSep);
            cadenaSep[numElementos]='\0';
            if(strcmp(orden,salida)==0){
                if(cadenaSep[1]!=NULL)fprintf(stderr, "%s", error_message);
                exit(0);
            } 
            if(strcmp(orden,cd)==0){
                char * destino = cadenaSep[1];
                if(chdir(destino)!=0)fprintf(stderr, "%s", error_message);
            }
            if(strcmp(orden, michmod)==0){
                if(chmod("UVash.c",0777)!=0){
                    fprintf(stderr, "%s", error_message);
                }
            }
            else if (fork()==0)
            {
                execvp(orden,cadenaSep);
                fprintf(stderr, "%s", error_message);
                kill(getpid(),SIGTERM);
            }
            else
            {
                int status;
                wait(&status);
            }
        }
    }
}
void conRedireccion(char * input, char ** cadenaSep, bool paralelo){
    char error_message[30] = "An error has occurred\n";
    if (cadenaSep[1][0]==10 || cadenaSep[1][0]==0 || cadenaSep[2]!=NULL){
        fprintf(stderr, "%s", error_message);
    }
    else{
        char *auxFichero = cadenaSep[1];
        char **auxPath=(char**)malloc(sizeof(char)*100);
        for (size_t i = 0; i < NUMEROARG; i++)
        {
            auxPath[i]=0;
        }
        
        separarCadena(auxFichero,auxPath," ");
        int j=0;
        while ((int)*auxPath[j]==0)
        {
            j++;
        }
        if (auxPath[j+1]!=NULL)
        {
            if ((int)*auxPath[j+1]!=0)
            {
                fprintf(stderr, "%s", error_message);
                return;
            }
        }
        char * pathFichero;
        pathFichero=auxPath[j];        
        strip(pathFichero);
        input = cadenaSep[0];
        separarCadena(input,cadenaSep," ");
        if(numChar(cadenaSep)<2){
            fprintf(stderr, "%s", error_message);
            return;
        }
        char * orden;
        if((int)*cadenaSep[0]!=0 || (int)*cadenaSep[1]!=0){
            if((int)*cadenaSep[0]!=0){
                orden = cadenaSep[0];
            }
            else{
                orden = cadenaSep[1];
            }
            int numElementos = numChar(cadenaSep);
            cadenaSep[numElementos-1] = NULL;
            /* printf("cadenaSep 0 %s\n",cadenaSep[0]);
            printf("cadenaSep 1 %s\n",cadenaSep[1]);
            printf("cadenaSep 2 %s\n",cadenaSep[2]);
            printf("pathFichero %s\n",pathFichero); */
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
                if (test==-1 || test2==-1){
                    exit(1);
                }
                close(fdout);
                execvp(orden,cadenaSep);
            }
            else
            {
                if (!paralelo)
                {
                    wait(NULL);
                }
                
            }
            
        }
        else{
            fprintf(stderr, "%s", error_message);
        }
    }
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
char * stripInicio(char *s){
    while(*s == ' ') {
        if(*s != '\t' && *s != ' ') {
        } else {
            ++s;
        }
    }
    return s;
}
char * stripFin(char *s){
    char * ps = strchr(s,'\0');
    int contador=0;
    while (*ps == ' ' || *ps == '\0')
    {
        --ps;
        contador++;
    }
    ps+=1;
    *ps='\0';
    return s;
}
char ** stripMedio(char *cadenaSep[]){
    int i,j;
    bool limpio = false;
    int numElementos = numChar(cadenaSep);
    int contadorVacio;
    while (!limpio)
    {
        contadorVacio=0;
        for (i = 0; i < numElementos; i++){
            if((int)*cadenaSep[i]==0 || (int)*cadenaSep[i]==1){
                contadorVacio++;
                for (j = i; j < numElementos-1; j++)
                {
                    cadenaSep[j]=cadenaSep[j+1];
                }
                cadenaSep[j]='\0';
                numElementos--;
            }
        }
        if(contadorVacio==0)limpio=true;
    }
    return cadenaSep;
}
int numChar(char *arr[]){
    int counter=0;
    for (int i=0; arr[i] != NULL; i++) {
        counter++;
    }
    return counter;
}