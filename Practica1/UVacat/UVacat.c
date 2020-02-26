#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
	int i;
	for(i=1;i<argc;i++){
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL){
			printf("UVacat: no puedo abrir fichero\n");
			exit(1);
		}
		char buffer[255];
		while(fgets(buffer, 255, fp)){
			printf("%s", buffer);
		}
		fclose(fp);
	}
	exit(0);
}
