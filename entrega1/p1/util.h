#ifndef __UTIL_H__

double randRange(double min, double max);
void log_debug(char *msg);
void mostrar_estado_quirofanos(char * msg, int *estado_quirofanos, int num_quirofanos);
void mostrar_estado_equipos(char * msg, int *estado_equipos, int num_equipos);

#define __UTIL_H__
#endif