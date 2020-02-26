#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
int contarFicherosLectura(char const *argv[], int argc);
void tratarCadena(char **vectorConStrings, int numeroFicheroLectura);

int main(int argc, char const *argv[]){
    size_t bufferSize = 255;
    int numeroFLectura = contarFicherosLectura(argv,argc);
    if(numeroFLectura==0){
        printf("UVazip: file1 [file2 ...]\n");
        exit(1);
    }
    char * buffer = (char *)malloc(sizeof(char) * bufferSize);
    char caracterAnterior;
    int contador = 0;
    bool escritura;
    for (size_t j = 0; j < numeroFLectura; j++)
    {
        escritura=false;
        int iterador=1; //empezamos en 1 porque la lista de argumentos el primer fichero esta en la posicion 1
        FILE *fr = fopen(argv[iterador],"r");
        if(fr==NULL){
            printf("UVazip: cannot open file\n");
            exit(1);
        }
        getline(&buffer,&bufferSize,fr);
        caracterAnterior=buffer[0];
        do
        {
            size_t longitud=strlen(buffer);
            for (size_t i = 0; i < longitud; i++)
            {
                if(buffer[i]==caracterAnterior){
                    contador++;
                }
                if(longitud-1==i || buffer[i]!=caracterAnterior)
                {
                    if(buffer[i]!=caracterAnterior){
                        fwrite(&contador,sizeof(int),1,stdout);
                        fputc(caracterAnterior,stdout);
                        escritura=true;
                        contador=1;
                    }
                    caracterAnterior=buffer[i];
                }
            }
            
        } while (getline(&buffer,&bufferSize,fr)!=-1);
    }
    char * salto = "\n";
    if(!escritura || strcmp(salto,&caracterAnterior)){
        fwrite(&contador,sizeof(int),1,stdout);
        fputc(caracterAnterior,stdout);
    }
    exit(0);
}
int contarFicherosLectura(char const *argv[], int argc){
    int numero = 0;
    int iterador = 1;
    while (iterador<argc && strcmp(argv[iterador],">")!=0){
        numero++;
        iterador++;
    } 
    return numero;
}
