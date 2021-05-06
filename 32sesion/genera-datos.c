#include <stdio.h>    // Para FILE*
#include <errno.h>    // para perror()
#include <stdlib.h>   // para exit()
#include <rpc/types.h>
#include <rpc/rpc.h> 
#include "tipos.h"


int main(){
    Texto t;
    Persona p;
    Resultado r;
    char buffer[100];
    FILE *ficherito;
    XDR op;

    //Apartado 1
		ficherito=fopen("Texto.dat", "w");  // Abrir para "w"rite
		if (ficherito==NULL) {              // Comprobar errores
			perror("Al abrir el fichero para apartado 1"); 
			exit(1); 
		}
		xdrstdio_create(&op, ficherito, XDR_ENCODE);
		t = "Probando";
		xdr_Texto(&op, &t);
		xdr_destroy(&op);
		fclose(ficherito);

		
		
		//Apartado 2
		p.nombre = "Maria";
		p.edad = 22;
		ficherito = fopen("Persona.dat", "w");
		if(ficherito == NULL){
			perror("Al abrir el fichero para apartado 2");
			exit(1);
		}
		xdrstdio_create(&op, ficherito, XDR_ENCODE);
		xdr_Persona(&op,&p);
		xdr_destroy(&op);
		fclose(ficherito);

		
		
		//Apartado 3
		ficherito = fopen("Resultado.dat", "w");
		if(ficherito == NULL){
			perror("Al abrir el fichero para el apartado 3");
			exit(1);
		}

		xdrstdio_create(&op, ficherito, XDR_ENCODE);
		
		printf("Escoge una opcion: \n"); 
			printf("1. Entero\n");
			printf("2. Real\n");
			printf("3. Texto\n");
		printf("Tu eleccion: ");
		scanf("%d",&r.caso);
		
		if(r.caso == 1){
			printf("Introduce un entero: ");
			scanf("%d", &r.Resultado_u.n);
			printf("(int) -> %d\n", r.Resultado_u.n);
		}
		else if(r.caso == 2){
			printf("Introduce un float: ");
			scanf("%f", &r.Resultado_u.x);
			printf("(float) -> %f\n", r.Resultado_u.x);
		}
		else if(r.caso == 3){
			printf("Introduce un texto: ");
			getchar();
			fgets(buffer, 100, stdin);
			r.Resultado_u.error = buffer;
			printf("(Texto) -> %s\n", r.Resultado_u.error);
		} else {
			printf("Has introducido una opcion incorrecta\n");
			exit(1);
		}
		xdr_Resultado(&op, &r);
		xdr_destroy(&op);
		fclose(ficherito);

		return 0;


}