#include <stdio.h>

void main(int argc, char *argv[]){
    int i= 0;
    printf("Numero de argumentos: %d\n", argc);
    for(i=0;i<argc;i++){
        printf("Argumento %d valor: %s\n",i,argv[i]);
    }
}
