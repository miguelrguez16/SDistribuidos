#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"


// Función de utilidad, para generar los tiempos usados por los medicos al operar
// Devuelve un número aleatorio comprendido entre min y max
double randRange(double min, double max)
{
  return min + (rand() / (double) RAND_MAX * (max - min + 1));
}


// Función de utilidad para depuración. Emite por pantalla el mensaje
// que se le pasa como parámetro, pero pone delante del mensaje un
// timestamp, para poder ordenar la salida por si saliera desordenada
//
// Ejemplo de uso:
//
//  log_debug("Medico 0 operando")
//
// Más ejemplos en el programa principal.
void log_debug(char *msg){
  struct timespec t;
  clock_gettime(_POSIX_MONOTONIC_CLOCK, &t);
  printf("[%ld.%09ld] %s", t.tv_sec, t.tv_nsec, msg);
}

// Funcion para mostrar el estado de los recursos

void mostrar_estado_recursos(char * msg, int *estado, int num)
{
    char buff[200];
    sprintf(buff, "ESTADO %s: [ ", msg);
    flockfile(stdout);
    log_debug(buff);
    for (int i=0; i<num;i++) {
      /*
      if (estado[i]!=LIBRE)
        printf("Recurso %d: ASIGNADO\n",i);
      else
        printf("Recurso %d: SIN ASIGNAR\n",i);
      */
      printf("%d ", estado[i]);
    }
    printf("]\n");
    funlockfile(stdout);
}
