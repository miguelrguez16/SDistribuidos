#include <stdio.h>    // Para FILE*
#include <errno.h>    // para perror()
#include <stdlib.h>   // para exit()
#include <rpc/types.h>
#include <rpc/rpc.h> 
#include "tipos.h"

int main(){
		/* Declaramos las variables necesarias*/
		Texto t;
		Persona p;
		Resultado r;
		FILE* ficherito;
		XDR op;

		/* Inicializamos a NULL aquellas que sean de tipo Texto */
		t = NULL; p.nombre = NULL; r.Resultado_u.error = NULL;
		
		//Apartado 1
		ficherito = fopen("Texto.dat", "r");
		if(ficherito == NULL){
			perror("Al abrir el ficherito del apartado 1");
			exit(1);
		}
		xdrstdio_create(&op, ficherito, XDR_DECODE);
		xdr_Texto(&op, &t);
		xdr_destroy(&op);
		fclose(ficherito);
		printf("(Texto) -> %s\n", t);

		
		
		//Apartado 2
		ficherito = fopen("Persona.dat", "r");
		if(ficherito==NULL){
			perror("Al abrir el ficherito del apartado 2");
			exit(1);
		}
		xdrstdio_create(&op, ficherito, XDR_DECODE);
		xdr_Persona(&op, &p);
		xdr_destroy(&op);
		fclose(ficherito);
		printf("(Persona) -> Nombre: %s - Edad: %d\n", p.nombre, p.edad);

		

		//Apartado 3
		ficherito = fopen("Resultado.dat", "r");
		if(ficherito == NULL){
			perror("Al abrir el ficherito del apartado 3");
			exit(1);
		}
		xdrstdio_create(&op, ficherito, XDR_DECODE);
		xdr_Resultado(&op, &r);
		xdr_destroy(&op);
		fclose(ficherito);
		
		switch(r.caso){
			case 1: printf("(Resultado) -> %d", r.Resultado_u.n);
			case 2: printf("(Resultado) -> %f", r.Resultado_u.x);
			case 3: printf("(Resultado) -> %s", r.Resultado_u.error);
		}
	return 0;
}
