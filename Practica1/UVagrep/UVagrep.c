#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]){
	if (argc<2){
		printf("UVagrep: searchterm [file ...]\n");
		exit(1);
	}
	else
	{
		int i;
		char *palabra = argv[1];
		char *buffer;
		size_t bufferSize = 255;
		ssize_t numeroChars;
		buffer = (char *)malloc(bufferSize * sizeof(char));
		if( buffer == NULL)
		{
			perror("Unable to allocate buffer");
			exit(1);
		}
		if (argc==2)
		{
			while (fgets(buffer,250,stdin))
			{
				if(strstr(buffer,palabra)!=NULL){
					printf("%s",buffer);
				}
			}
		}
		else{
			for(i=2;i<argc;i++){
				FILE *fp = fopen(argv[i], "r");
				if(fp==NULL){
					printf("UVagrep: cannot open file\n");
					exit(1);
				}
				else{	
					while ((numeroChars=getline(&buffer,&bufferSize, fp))!=-1){
						if (strstr(buffer,palabra) != NULL)
						{
							printf("%s", buffer);
						}
					}
					fclose(fp);
				}
			}
		}
		free(buffer);
	}
	
	exit(0);
}
