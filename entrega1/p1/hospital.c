#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include "cola.h"
#include "util.h"

#define MAX_QUIROFANOS    4
#define MAX_EQUIPOS       6
#define MAX_MEDICOS      10
#define MAX_PACIENTES     4

#define LIBRE             0
#define OCUPADO           1

// VARIABLES GLOBALES
// ====================================================================

// Estado de cada quirofano del hospital 0 indica libre, 1 ocupado
int estado_quirofanos[MAX_QUIROFANOS];

// Mutex para evitar el acceso de dos hilos simultáneos al array anterior
pthread_mutex_t manipular_quirofanos;

// Estado de cada quirofano del hospital 0 indica libre, 1 ocupado
int estado_equipos[MAX_EQUIPOS];

// Mutex para evitar el acceso de dos hilos simultáneos al array anterior
pthread_mutex_t manipular_equipos;


// Colas para comunicar medicos y coordinadores
Cola cola_peticion_quirofano, cola_concesion_quirofano, cola_liberacion_quirofano;
Cola cola_peticion_equipo, cola_concesion_equipo; 

// Semáforo para bloquear al coordinador de reserva de quirofanos si no 
// quirofanos libres hasta que se libere uno
sem_t quirofanos_libres; 


// Semáforo para bloquear al coordinador de equipos de enfermeria si no 
// hay equipos libres hasta que se libere uno
sem_t equipos_libres; 

// ====================================================================
// FUNCIONES de manejo del array de quirofanos y de equipos de quirofano
// ====================================================================
void  LiberarQuirofano(int quirofano_liberado){
    // Se pone un 0 en la posición del quirofano liberada
    // Pero el acceso al array de estados de quirofano
    // debe estar protegido por un cerrojo   
    // ************** A RELLENAR ****************** 
    int ret = 0;
  
    ret = pthread_mutex_lock(&manipular_quirofanos);
    if (ret != 0){
      perror("Error LiberarQuirofano --> coger mutex manipular_quirofanos.\n");
      exit(1);
    }
    estado_quirofanos[quirofano_liberado] = LIBRE;
    //mostrar_estado_quirofanos("QUIROFANOS",estado_quirofanos,MAX_QUIROFANOS);
    ret = pthread_mutex_unlock(&manipular_quirofanos);
    if (ret != 0){
      perror("Error LiberarQuirofano --> soltar mutex manipular_quirofanos.\n");
      exit(1);
   }

    // Y se señala que hay un quirofano libre más, mediante el semáforo
    // ************** A RELLENAR ****************** 
    ret = sem_post(&quirofanos_libres);
    if (ret != 0){
      perror("Error LiberarQuirofano - semaforo post num_huecos.\n");
      exit(1);
  }

}

void  LiberarEquipo(int equipo_liberado) {
    // Se pone un 0 en la posición del equipo liberado
    // Pero el acceso al array de estados de equipos
    // debe estar protegido por un cerrojo   
    // ************** A RELLENAR ****************** 
    int ret = 0;
  
    ret = pthread_mutex_lock(&manipular_equipos);
    if (ret != 0){
      perror("Error LiberarQuirofano --> coger mutex manipular_equipos.\n");
      exit(1);
    }
    estado_equipos[equipo_liberado] = LIBRE;
    //mostrar_estado_equipos("EQUIPOS despues liberar",estado_quirofanos,MAX_EQUIPOS);

    ret = pthread_mutex_unlock(&manipular_equipos);
    if (ret != 0){
      perror("Error LiberarQuirofano --> soltar mutex manipular_equipos.\n");
      exit(1);
    }

    // Y se señala que hay un equipo libre más, mediante el semáforo
    // ************** A RELLENAR ****************** 
    ret = sem_post(&equipos_libres);//equiposlibres++
    if (ret != 0){
      perror("Error LiberarQuirofano --> incrementar semaforo num_huecos.\n");
      exit(1);
  }

}

int ReservarQuirofano(void){
    int quirofano_encontrado = -1;
    register int i;

    // Esperar semáforo a que haya quirofanos libres
    // ************** A RELLENAR ****************** 
    int ret = 0;
    ret = sem_wait(&quirofanos_libres);
    if(ret !=0){
      perror("Error ReservaQuirofano -->semaforo wait quirofanos_libres \n");
      exit(1);
    }

    // Buscar el número de quirofano libre, protegiendo el acceso al
    // array de quirofanos con el cerrojo
    ret = pthread_mutex_lock(&manipular_quirofanos);
    if (ret != 0){
      perror("Error ReservaQuirofano --> mutex lock \n");
      exit(1);

    }

    // ************** A RELLENAR ******************
    // busqueda del quirófano libre
    for (i = 0; i < MAX_QUIROFANOS && quirofano_encontrado==-1; i++) {
        if(estado_quirofanos[i] == LIBRE){
          quirofano_encontrado=i;
        }
    }
    // Poner un 1 en la posición encontrada
    estado_quirofanos[quirofano_encontrado] = OCUPADO;
    //mostrar_estado_quirofanos("QUIROFANOS",estado_quirofanos,MAX_QUIROFANOS);

    ret = pthread_mutex_unlock(&manipular_quirofanos);
    if (ret != 0){
      perror("Error ReservaQuirofano --> mutex unlock \n");
      exit(1);
    }
    // Retornar el quirofano encontrado
    return quirofano_encontrado;
}

int ReservarEquipo(void){
    int equipo_encontrado = -1;
    register int i;
    //mostrar_estado_equipos("Equipos antes de reservar", estado_equipos,MAX_EQUIPOS);
    // Esperar semáforo a que haya equipos libres
    // ************** A RELLENAR ****************** 
    int ret = 0;
    ret = sem_wait(&equipos_libres);
    if(ret !=0){
      perror("Error ReservarEquipo --> semaforo wait equipos_libres \n");
      exit(1);
    }

    // Buscar el número de equipo libre, protegiendo el acceso al
    // array de estado de equiposs con el cerrojo
    ret = pthread_mutex_lock(&manipular_equipos);
    if(ret !=0){
      perror("Error ReservarEquipo --> mutex lock \n");
      exit(1);
    }
    // ************** A RELLENAR ******************
    // busqueda del equipo libre

    for (i = 0; i < MAX_EQUIPOS && equipo_encontrado==-1; i++) { 
        if(estado_equipos[i] == LIBRE){
          equipo_encontrado=i;
        }
    }

    // Poner un 1 en la posición encontrada
    estado_equipos[equipo_encontrado] = OCUPADO;
    //mostrar_estado_equipos("EQUIPOS",estado_equipos,MAX_EQUIPOS);

    ret = pthread_mutex_unlock(&manipular_equipos);
    if(ret !=0){
      perror("Error ReservarEquipo --> mutex unlock \n");
      exit(1);
    }

    //mostrar_estado_equipos("Equipos despues de reservar", estado_equipos,MAX_EQUIPOS);

    // Retornar el quirofano encontrado
    return equipo_encontrado;
}


// ====================================================================
// Implementación de los hilos
// ====================================================================

void *coordinador_reserva_quirofanos(void * nada) {  // No recibe parámetro
    int concedido;
    dato_cola * p;
    char msg[100];

    //int ret;

  //Codigo del coordinador de reserva de quirofanos
  while (1)  {
    // Esperar a que un Medico solicite quirofano
    // ************** A RELLENAR ****************** 
    /*ret = sem_wait(&quirofanos_libres);
    if (ret == -1){
      perror("Error coordinador_reserva_quirofanos --> semaforo decrementar quirofanos_libres\n");
      exit(1);
    }*/
    p = (dato_cola *) malloc (sizeof(dato_cola));
    if (p == NULL){
      perror("Error reservar memoria\n");
      exit(1);
    }
    p=obtener_dato_cola(&cola_peticion_quirofano);

    // Información de depuración
    sprintf(msg, "Coordinador de Reserva de Quirofanos busca quirofano para medico %d\n", p->id1);
    log_debug(msg);

    // Buscar quirofano libre llamando a ReservarQuirofano()
    concedido = ReservarQuirofano();

    // Información de depuración
    sprintf(msg, "Coordinador de Reserva de Quirofanos concede quirofano %d al medico %d\n", concedido, p->id1);
    log_debug(msg);

    // Notificar al Medico qué quirofano se le ha concedido
    // ************** A RELLENAR ****************** 
      // -- Liberamos previamente memoria del mensaje procesado
      // -- reservamos memoria para el mensaje de respuesta
      // -- asignamos el campo apropiado del mensaje de respuesta
      // -- y lo insertamos en la cola apropiada
    free(p);

    p=(dato_cola *) malloc (sizeof(dato_cola));
    if( p==NULL){
      perror("Error coordinador_rerseva_quirofanos --> Reservar memoria.\n");
      exit(1);
    }
    p->id1=concedido;

    insertar_dato_cola(&cola_concesion_quirofano,p);

  }
  return NULL;
}


void *coordinador_equipos_enfermeria(void * nada){  // No recibe parámetro

    int concedido;
    dato_cola * p;
    char msg[100];

    //int ret;

  //Codigo del coordinador de reserva de equipos de enfermeria de quirofano
  while (1)  {
    // Esperar a que un Medico solicite equipo de enfermeria
    // ************** A RELLENAR ****************** 
    /*ret = sem_wait(&equipos_libres);
    if (ret == -1){
      perror("Error coordinador_equipos_enfermeria --> semaforo decrementar equipos_libres\n");
      exit(1);
    }*/
    p = (dato_cola *) malloc (sizeof(dato_cola));
    if (p == NULL){
      perror("Error reservar memoria\n");
      exit(1);
    }
    p=obtener_dato_cola(&cola_peticion_equipo);
    // Información de depuración
    sprintf(msg, "Coordinador de Equipos de Enfermeria busca equipo para medico %d\n", p->id1);
    log_debug(msg);

    // Buscar equipo libre llamando a ReservarEquipo()
    concedido = ReservarEquipo();

    // Información de depuración
    sprintf(msg, "Coordinador de Equipos de Enfermeria concede equipo %d al medico %d\n", 
            concedido, p->id1);
    log_debug(msg);

    // Notificar al Medico qué equipo  se le ha concedido
    // ************** A RELLENAR ****************** 
      // -- Liberamos previamente memoria del mensaje procesado
      // -- reservamos memoria para el mensaje de respuesta
      // -- asignamos el campo apropiado del mensaje de respuesta
      // -- y lo insertamos en la cola apropiada
    free(p);
    p=(dato_cola *) malloc (sizeof(dato_cola));
    if( p==NULL){
      perror("Error coordinador_equipos_enfermeria --> Reservar memoria.\n");
      exit(1);
    }
    p->id1=concedido;

    insertar_dato_cola(&cola_concesion_equipo,p);

  }
  return NULL;
}

void *coordinador_liberacion_quirofanos(void *nada) { // No recibe parámetro

  //int quirofano_liberado;
  //int equipo_liberado;
  dato_cola *p;
  char msg[100];

  //int ret;
  // Código del coordinador de liberacion de quirofanos
  while (1) {
    // Esperar a que un Medico nos indique que ha terminado de operar en un quirofano
    // ************** A RELLENAR ****************** 
    /*ret = sem_post(&quirofanos_libres);
    if (ret == -1){
      perror("Error coor_lib_quirofanos --> semaforo wait\n");
      exit(1);
    }*/
    p=(dato_cola *) malloc (sizeof(dato_cola));
    if( p==NULL){
      perror("Error coordinador_equipos_enfermeria --> Reservar memoria.\n");
      exit(1);
    }
    p=obtener_dato_cola(&cola_liberacion_quirofano);
    

    // Marcar el quirofano como libre llamando a LiberarQuirofano()
    LiberarQuirofano(p->id1);

    // Indicar que el equipo de enfermeria esta libre llamando a LiberarEquipo()
    LiberarEquipo(p->id2);

    // Información de depuración
    sprintf(msg, "Coordinador de Liberacion de Quirofanos libera el quirofano  %d y el equipo %d\n", p->id1,p->id2);
    log_debug(msg);
    free(p);  // Destruimos memoria mensaje procesado

    
  }
  return NULL;
}


// Hilo que simula al Medico. Recibe un número que es el ID del médico
// Efectúa un bucle en el que va solicitando quirofano para realizae
// intervenciones. Antes de cada una de esas operaciones solicita 
// un quirófano y espera que le sea concedio. Para poder operar tiene
// que tener asignado un equipo de enfermeria de quirofano
//
// Luego simula que opera al paciente.
//
// Tras ello libera el quirófano y el equipo de enfermeria
//

void *Medico(int *id){
  int id_medico;      
  int n_operaciones;       // Contador de operaciones realizadas
  char msg[200];     // Buffer auxiliar para crear mensajes de log
  dato_cola *p;
  int quirofano,equipo;

  id_medico = *id;   // Capturar el id del medico en una variable local
  free(id);          // Ya no necesitamos el parámetro recibido, lo liberamos

  // Bucle de simulación de cada operacion
  for (n_operaciones=0; n_operaciones<MAX_PACIENTES; n_operaciones++)  {
    double t;   // Tiempo de uso del quirofano (aleatorio)

    // Mostrar información de depuración
    sprintf(msg, "Medico %d solicita quirofano para operacion (paciente %d)\n", 
            id_medico, n_operaciones);
    log_debug(msg);

    // Solicitar quirofano al coord. de reserva de quirofanos (enviándole id_medico)
    // ************** A RELLENAR ****************** 
      // -- reservamos memoria para el mensaje a insertar
      // -- asignamos el campo apropiado del mensaje
      // -- y lo insertamos en la cola apropiada
    p = (dato_cola *) malloc (sizeof(dato_cola));
    if (p == NULL){
      perror("Error Medico --> Reservar memoria - uno.\n");
      exit(1);
    }
    p->id1=id_medico;
    insertar_dato_cola(&cola_peticion_quirofano,p);

    // Esperar concesión y obtener el quirofano concedido
    p=obtener_dato_cola(&cola_concesion_quirofano);
    quirofano=p->id1;
    free(p);  //liberamos la memoria del mensaje recibido

    // Construimos mensaje para el coordinador de equipos de enfermeria
    // ************** A RELLENAR ****************** 
      // -- reservamos memoria para el mensaje a insertar
      // -- asignamos el campo apropiado del mensaje
      // -- y lo insertamos en la cola apropiada
    p = (dato_cola *) malloc (sizeof(dato_cola));
    if (p == NULL){
      perror("Error Medico --> Reservar memoria - dos.\n");
      exit(1);
    }
    p->id1=id_medico;
    insertar_dato_cola(&cola_peticion_equipo,p);

    // Esperar asignacion de equipo de quirofano
    p=obtener_dato_cola(&cola_concesion_equipo);
    equipo=p->id1;
    free(p);  //liberamos la memoria del mensaje recibido

    // Simular que se usa el quirofano durante un tiempo aleatorio
    t = randRange(1, 3);
    sprintf(msg, "Medico %d usa el quirofano %d durante %fs (paciente %d)\n", 
            id_medico, quirofano, t, n_operaciones); 
    log_debug(msg);
    sleep(t);
    sprintf(msg, "Medico %d libera el quirofano %d tras operacion (paciente %d)\n", 
            id_medico, quirofano, n_operaciones);
    log_debug(msg);

    // Notificar al coordinador de liberacion de quirofanos que está libre
    // ************** A RELLENAR ****************** 
      // -- reservamos memoria para el mensaje a insertar
      // -- asignamos los campos apropiados del mensaje
      // -- y lo insertamos en la cola apropiada
    p = (dato_cola *) malloc (sizeof(dato_cola));
    if (p == NULL){
      perror("Error Medico --> Reservar memoria - dos.\n");
      exit(1);
    }
    //printf("Medico %d libera el equipo %d tras operacion (paciente %d)\n",id_medico,equipo,n_operaciones);
    p->id1=quirofano;
    p->id2=equipo;
    insertar_dato_cola(&cola_liberacion_quirofano,p);
  }
  return NULL;
}

// ====================================================================
// PROGRAMA PRINCIPAL
// ====================================================================

// Su misión es crear e inicializar todos los recursos de sincronización, 
// lanzar todos los hilos y esperar a que los hilos-avión finalicen

int main(void) {
  register int i;   // Indice para bucles
  int *id_medico;   // Para ir creando dinámicamente los id de medico

  // Variables para almacenar los identificadores de hilos
  pthread_t c_reservas,c_liberacion,c_equipos;
  pthread_t datos_hilo[MAX_MEDICOS];
  
  setbuf(stdout,NULL); //quitamos el buffer de la salida estandar

  // Inicializar semáforoS y mutex
  // ************** A RELLENAR ****************** 
  int ret = 0;

  ret = sem_init(&equipos_libres,0,MAX_EQUIPOS);
  if (ret == -1){
    perror("Error incializar semaforo equipos_libres");
    exit(1);
  }
  ret = sem_init(&quirofanos_libres,0,MAX_QUIROFANOS);
  if (ret == -1){
    perror("Error incializar semaforo quirofanos_libres");
    exit(1);
  }

  ret = pthread_mutex_init(&manipular_quirofanos,NULL);
	if(ret !=0 ){
		perror("Error al inicializar mutex manipular_quirofanos\n");
		exit(1);
	}

  ret = pthread_mutex_init(&manipular_equipos,NULL);
	if(ret !=0 ){
		perror("Error al inicializar mutex manipular_equipos\n");
		exit(1);
	}

  // Inicializar todas las colas utilizadas
  // ************** A RELLENAR ******************
  //medicos solicitan el quirofano
  inicializar_cola(&cola_peticion_quirofano,MAX_QUIROFANOS);
  //neducis recibiran el quirofano concedido
  inicializar_cola(&cola_concesion_quirofano,MAX_QUIROFANOS);
  //medicos liberan quirofano y equipo
  inicializar_cola(&cola_liberacion_quirofano, MAX_QUIROFANOS);
  //medicos solicitan el eqipo de enfermeria
  inicializar_cola(&cola_peticion_equipo, MAX_EQUIPOS);
  //medicos recibiran el equipo de enfermeria
  inicializar_cola(&cola_concesion_equipo, MAX_EQUIPOS);
 

  // Rellenar con ceros el estado de todos los quirofanos 
  for (i=0;i<MAX_QUIROFANOS;i++) estado_quirofanos[i] = LIBRE;

  // Rellenar con ceros el estado de todos los equipos 
  for (i=0;i<MAX_EQUIPOS;i++) estado_equipos[i] = LIBRE;

  //mostrar_estado_quirofanos("QUIROFANOS",estado_quirofanos,MAX_QUIROFANOS);
  //mostrar_estado_equipos("EQUIPOS",estado_equipos,MAX_EQUIPOS);

  // Creación de los hilos coordinadores
  // No reciben parámetros por lo que se pasa NULL
  pthread_create(&c_reservas,NULL,
                  (void *) coordinador_reserva_quirofanos,      
                  (void *) NULL);
  pthread_create(&c_liberacion,NULL,
                  (void *) coordinador_liberacion_quirofanos,
                  (void *) NULL);
  pthread_create(&c_equipos,NULL,
                  (void *) coordinador_equipos_enfermeria,
                  (void *) NULL);

  // Creación de un hilo para cada Medico. Estos sí reciben como parámetro
  // un puntero a entero que será su id_medico. Se crea dinámicamente uno
  // para cada hilo y se le asigna el contador del bucle
  for (i=0;i<MAX_MEDICOS;i++) {
    id_medico = (int *) malloc (sizeof(int));
    *id_medico = i;    
    // ************** A RELLENAR ****************** 
    // creación del hilo médico
    ret = pthread_create(&datos_hilo[i], NULL,(void *) Medico,(int *) id_medico);
    if (ret != 0){
      perror("Error en la creacion del hilo medico\n");
      exit(1);
    }

  }

  // main espera a que terminen todos los medicos
  for (i=0;i<MAX_MEDICOS;i++)
  {
    ret = pthread_join(datos_hilo[i],NULL);
    if(ret != 0){
      perror("Error en la espera de finalizacion\n");
      exit(1);
    }
  }

  // Finalización. Destruir el mutex, el semáforo y las colas
  // ************** A RELLENAR ****************** 

  destruir_cola(&cola_peticion_equipo);
  destruir_cola(&cola_concesion_equipo);
  destruir_cola(&cola_liberacion_quirofano);
  destruir_cola(&cola_concesion_quirofano);
  destruir_cola(&cola_peticion_quirofano);

  ret = sem_destroy(&equipos_libres);
  if (ret != 0){
      perror("Error al liberar semaforo equipos_libres\n");
      exit(1);
   }
   ret = sem_destroy(&quirofanos_libres);
   if (ret != 0){
      perror("Error al liberar semaforo quirofanos_libres\n");
   }

  ret=pthread_mutex_destroy(&manipular_equipos);
	if(ret != 0) {
		perror("Error al liberar el mutex manipular_equipos\n");
	}
  ret=pthread_mutex_destroy(&manipular_quirofanos);
	if(ret != 0) {
		perror("Error al liberar el mutex manipular_equipos\n");
	}
  //mostrar_estado_quirofanos("QUIROFANOS al finalizar",estado_quirofanos,MAX_QUIROFANOS);
  //mostrar_estado_equipos("EQUIPOS al finalizar",estado_equipos,MAX_EQUIPOS);

  return 0;
}
