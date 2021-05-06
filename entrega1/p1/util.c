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


// Función de utilidad para mostrar el array estado_quirofanos
// precedido de un mensaje que recibe como parámetro.
//
// Ejemplo de uso:
//
//   mostrar_estado_quirofanos("Antes de reservar quirofano", estado_quirofanos, MAX_QUIROFANOS)
//   ...
//   mostrar_estado_quirofanos("Despues de reservar quirofano", estado_quirofanos, MAX_QUIROFANOS)
//
void mostrar_estado_quirofanos(char * msg, int *estado_quirofanos, int num_quirofanos) {
    char buff[200];
    sprintf(buff, "%s -> ESTADO QUIROFANOS: [ ", msg);
    flockfile(stdout);
    log_debug(buff);
    for (int i=0; i<num_quirofanos;i++) {
        printf("%d ", estado_quirofanos[i]);
    }
    printf("]\n");
    funlockfile(stdout);
}

// Función de utilidad para mostrar el array estado_equipos
// precedido de un mensaje que recibe como parámetro.
//
// Ejemplo de uso:
//
//   mostrar_estado_equipos("Antes de solicitar equipo quirofano", estado_equipos, MAX_EQUIPOS)
//   ...
//   mostrar_estado_equipos("Despues de solicitar equipo quirofano", estado_equipos, MAX_EQUIPOS)
//
void mostrar_estado_equipos(char * msg, int *estado_equipos, int num_equipos) {
    char buff[200];
    sprintf(buff, "%s -> ESTADO EQUIPOS: [ ", msg);
    flockfile(stdout);
    log_debug(buff);
    for (int i=0; i<num_equipos;i++) {
//      if (estado_equipos[i])
//        printf("Equipo %d: ASIGNADO",i);
//      else
//        printf("Equipo %d: SIN ASIGNAR",i);
        printf("%d ", estado_equipos[i]);
    }
    printf("]\n");
    funlockfile(stdout);
}
