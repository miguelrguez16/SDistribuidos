#include <pthread.h>
#include <semaphore.h>

typedef struct {
  int id1; 
  int id2;
} dato_cola;


struct Cola {
    dato_cola **datos;   
    int head;
    int tail;
    int tam_cola;
    pthread_mutex_t mutex;
    sem_t num_huecos;
    sem_t num_ocupados;
};
typedef struct Cola Cola;

void inicializar_cola(Cola *cola, int tam_cola);
void destruir_cola(Cola *cola);
void insertar_dato_cola(Cola *cola, dato_cola * dato);
dato_cola * obtener_dato_cola(Cola *cola);
