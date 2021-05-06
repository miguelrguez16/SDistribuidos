#include "cola.h"
#include "stdlib.h"
#include <errno.h>
#include <stdio.h>


/* Este fichero está vacío
   Debes escribirlo tú, implementando las funciones que están declaradas en cola.h
*/

void inicializar_cola(Cola *cola, int tam_cola){
   int ret = 0;

   cola->head=0;
   cola->tail=0;
   cola->tam_cola=tam_cola;

   cola->datos=(dato_cola **) malloc (sizeof(dato_cola)*tam_cola);
   if(cola->datos==NULL){
		perror("Error Cola al reservar memoria\n");
		exit(1);
	}

   ret = pthread_mutex_init(&cola->mutex,NULL);
	if(ret !=0 ){
		perror("Error Cola al inicializar  mutex\n");
		exit(1);
	}

   ret = sem_init(&cola->num_huecos, 0, tam_cola);
	if (ret != 0 ){
		perror("Error Cola al inicializar semaforo num_huecos\n");
		exit(1);
	}

   ret = sem_init(&cola->num_ocupados,0,0);
   if (ret != 0){
      perror("Error Cola al inicializar semaforo num_ocupados\n");
      exit(1);
   }
}
void destruir_cola(Cola *cola){
   int ret=0;

   free(cola->datos);

   ret = sem_destroy(&cola->num_huecos);
   if (ret != 0){
      perror("Error Cola  al liberar semaforo num_huecos\n");
      exit(1);
   }
   ret = sem_destroy(&cola->num_ocupados);
   if (ret != 0){
      perror("Error Cola  al liberar semaforo num_ocupados\n");
      exit(1);
   }

   ret=pthread_mutex_destroy(&cola->mutex);
	if(ret != 0) {
		perror("Error Cola  al liberar el mutex\n");
      exit(1);
	}

}
void insertar_dato_cola(Cola *cola, dato_cola * dato){
   int ret = 0;
   ret = sem_wait(&cola->num_huecos);
   if (ret != 0){
      perror("Error Cola insertar dato - decrementar semaforo num_huecos.\n");
      exit(1);
   }

   ret = pthread_mutex_lock(&cola->mutex);
   if (ret != 0){
      perror("Error Cola insertar dato - coger mutex.\n");
      exit(1);
   }
   
   cola->datos[cola->head] = dato;
	cola->head = (cola->head + 1) % cola->tam_cola;

   ret = pthread_mutex_unlock(&cola->mutex);
   if (ret != 0){
      perror("Error Cola insertar dato - solar mutex.\n");
      exit(1);
   }

   ret = sem_post(&cola->num_ocupados);
   if (ret != 0){
      perror("Error Cola insertar dato - incrementar semaforo num_ocupados.\n");
      exit(1);
   }
}

dato_cola * obtener_dato_cola(Cola *cola){
   int ret = 0;
   dato_cola *obtener;

   ret = sem_wait(&cola->num_ocupados);
   if (ret != 0){
      perror("Error Cola obtener datos - decrementar semaforo num_ocupados");
      exit(1);
   }

   ret = pthread_mutex_lock(&cola->mutex);
   if (ret != 0){
      perror("Error Cola obtener datos - coger mutex.\n");
      exit(1);
   }

   obtener = cola->datos[cola->tail];
	cola->tail = (cola->tail + 1) % cola->tam_cola;

   ret = pthread_mutex_unlock(&cola->mutex);
   if (ret != 0){
      perror("Error Cola obtener datos - solar mutex.\n");
      exit(1);
   }

   ret = sem_post(&cola->num_huecos);
   if (ret != 0){
      perror("Error Cola insertar dato - incrementar semaforo num_huecos.\n");
      exit(1);
   }

   return obtener;
}