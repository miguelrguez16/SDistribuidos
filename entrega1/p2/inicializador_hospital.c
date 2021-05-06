#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include "hospital.h"

#define SERVIDOR "127.0.0.1"

int  main(int argc,char *argv[])
{
  // *************** A RELLENAR *********************
  // Completar el resto de este programa, que debe invocar por RPC la función
  // inicializar_hospital() en el servidor, pasándole los parámetros apropiados
  // que se extraen de la línea de comandos
  //
  // argv[1] será el valor de max_quirofanos
  // argv[2] será el valor de max_equipos
  //
  // El programa debe comprobar que ambos parámetros se han especificado en línea de
  // comandos y que ambos son enteros positivos, emitiendo un mensaje si no es así
  int n_quirofanos, n_equipos;
  CLIENT *clnt;

  if(argc != 3){
    fprintf(stderr,"Forma de uso: %s <max_quirofanos> <max_equipos>\n",argv[0]);
    exit(1);
  }
  if (atoi((char *) argv[1])<=0) {
     fprintf(stderr,"El parametro  <max_quirofanos> debe ser entero positivo\n");
     exit(1);
  }
  if (atoi((char *) argv[2])<=0) {
     fprintf(stderr,"El parametro  <max_equipos> debe ser entero positivo\n");
     exit(1);
  }
  n_quirofanos=atoi(argv[1]);
  n_equipos=atoi(argv[2]);

  //printf("Valor incial  %d equipos: %d",n_quirofanos,n_equipos);

  init_data *datosIniciales;
  datosIniciales = (init_data *) malloc(sizeof(init_data));
  if (datosIniciales == NULL){
     fprintf(stderr,"Error incializador reserva memoria\n");
     exit(1);
  }
  datosIniciales->num_quirofano=n_quirofanos;
  datosIniciales->num_equipo=n_equipos;

  clnt = clnt_create(SERVIDOR,HOSPITAL,PRIMERA, "tcp");
  if(clnt == NULL){
    clnt_pcreateerror("Error inicializador_hospital inicializar cliente\n");
    exit(1);
  }

  inicializar_hospital_1(datosIniciales,clnt);
}
