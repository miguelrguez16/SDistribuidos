#include <stdio.h>    // Para FILE*
#include <errno.h>    // para perror()
#include <stdlib.h>   // para exit()
#include <rpc/types.h>
#include <rpc/rpc.h>
#include "tipos.h"
int main(){
    //Declaramos las variables indicadas en el enunciado además de 
    //el fichero y la operacion XDR
    FILE *fichero;
    XDR operacion;
    Lista lista;
    lista.siguiente = NULL;

    //Inicializamos el fichero
    fichero = fopen("Lista.dat", "r");
    if(fichero == NULL){
        //Si entra en el if es que ha fallado al abrir el fichero
        perror("Erro en la apertura de Lista.dat\n");
        exit(1);
    }


    xdrstdio_create(&operacion, fichero,XDR_DECODE);
    xdr_Lista(&operacion, &lista);
    xdr_destroy(&operacion);

    while(lista.siguiente->siguiente != NULL){
        printf("(Lista) -> %d\n", lista.dato);
        lista.siguiente=lista.siguiente->siguiente;
    }
        
    return 0;


}