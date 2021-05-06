#ifndef __UTIL_H__

#define CIERTO            1
#define FALSO             0
#define LIBRE             0
#define OCUPADO           1
#define SIN_RECURSO      -1


double randRange(double min, double max);
void log_debug(char *msg);
void mostrar_estado_recursos(char * msg, int *estado, int num);

#define __UTIL_H__
#endif
