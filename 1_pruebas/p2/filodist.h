#define MAX_BOCADOS 10

typedef enum {
     no_sentado=0, queriendo_comer=1,
     comiendo=2, dejando_comer=3,
     pensando=4,
     // APARTADO 1
     esperando_terminar=5, terminar=6

}estado_filosofo;