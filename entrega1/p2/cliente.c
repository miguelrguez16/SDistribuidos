/* 
Cliente de RPC que simula las operaciones de varios medicos
*/
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include <pthread.h>
#include "util.h"
#include "hospital.h"


#define SERVIDOR "127.0.0.1"

//Variables globales

int num_medicos;
int max_pacientes;

// Función que simula un médico, y que será ejecutada en su propio hilo
// Recibe como parámetro el id del médico
void *Medico(int *id)
{
  CLIENT *cl;
  int id_medico;
  int id_equipo;
  int quirofano;
  int n_operaciones;       // Contador de operaciones realizadas
  char msg[200];           // Buffer auxiliar para crear mensajes de log
  int *res;                // para recibir resultados de las RPCs
  int continuar;
  double t;                //variable para almacenar tiempos

  id_medico = *id;   // Capturar el id del medico en una variable local
  free(id);          // Ya no necesitamos el parámetro recibido, lo liberamos

  // Bucle de simulación de cada operacion
  for (n_operaciones=0; n_operaciones<max_pacientes; n_operaciones++)  {

    // ********** A RELLENAR *******************
    // Inicialización de la variable cl
    cl = clnt_create(SERVIDOR,HOSPITAL,PRIMERA,"tcp");
    if(cl == NULL){
      clnt_pcreateerror("Error: Medico inicializar cliente \n");
      exit(1);
    }

    // Mostrar información de depuración
    sprintf(msg, "Medico %d solicita quirofano para operacion (paciente %d)\n",
            id_medico, n_operaciones);
    log_debug(msg);
    // ************** A RELLENAR ******************
    // Primera Invocacion remota para solicitar quirofano
    //   NOTA: esta solicitud debe realizarse en bucle mientras se reciba -1 como respuesta
    //         y el bucle debe contener un sleep() de alrededor de 0.3s (puede ser aleatorio)
    //         para evitar saturar la CPU

    continuar=SIN_RECURSO;//-1
    //mientras no haya asignacion de quirofano, pido
    while(continuar==SIN_RECURSO){
      res = reservar_quirofano_1(&id_medico,cl);
      if(res == NULL){
        fprintf(stderr,"Error Medico reservar_quirofano_1\n");
        exit(1);
      }
      continuar=(int) *res;
      sleep(randRange(0, 1));//t = randRange(0, 1);
      //printf("Valor reservado: %d",continuar);
    }
    quirofano=0;
    quirofano=continuar;
    //printf("Medico %d -> quirofano %d.\n",id_medico,quirofano);
    // Mostrar información de depuración   
    sprintf(msg, "Medico %d solicita equipo para operacion (paciente %d)\n", id_medico, n_operaciones);
    log_debug(msg);    
    // ************** A RELLENAR ******************
    // Segunda Invocacion remota para solicitar equipo de enfermeria
    //   NOTA: esta solicitud debe realizarse en bucle mientras se reciba -1 como respuesta
    //         y el bucle debe contener un sleep() de alrededor de 0.3s (puede ser aleatorio)
    //         para evitar saturar la CPU
    continuar=SIN_RECURSO;
    //mientras no haya asignacion de equipo, pido
    while(continuar==SIN_RECURSO){
      res = reservar_equipo_enfermeria_1(&id_medico,cl);
      if(res == NULL){
        fprintf(stderr,"Error Medico reservar_equipo_enfermeria_1\n");
        exit(1);
      }
      continuar=(int) *res;
      sleep(randRange(0, 1));//t = randRange(0, 1);
    }
  
    
    id_equipo=0;
    id_equipo=continuar;
    //printf("Medico %d -> equipo %d.\n",id_medico,id_equipo);

    // Simular que se usa el quirofano durante un tiempo aleatorio
    t = randRange(2, 3);
    sprintf(msg, "Medico %d usa el quirofano %d y equipo %d durante %fs (paciente %d)\n", id_medico, quirofano, id_equipo, t, n_operaciones);
    log_debug(msg);
    sleep(t);
    sprintf(msg, "Medico %d libera el quirofano %d tras operacion (paciente %d)\n",  id_medico, quirofano, n_operaciones);
    log_debug(msg);
    // ************** A RELLENAR ******************
    // Tercera invocacion remota para notificar al coordinador de liberacion de quirofanos que está libre
    recursos  *datosLiberar;
    datosLiberar = (recursos *) malloc (sizeof(recursos));
    if (datosLiberar == NULL){
      fprintf(stderr,"Error Medico liberacion médico\n");
      exit(1);
    }
    datosLiberar->key_quirofanos=quirofano;
    datosLiberar->key_equipos=id_equipo;
    res = liberar_recursos_1(datosLiberar,cl);
    if (res == NULL){
       fprintf(stderr,"Error Medico liberar_recursos_1\n");
        exit(1);
    }
    clnt_destroy(cl);
  }

  return NULL;
}

int  main(int argc,char *argv[])
{
  register int i;   // Indice para bucles
  int *id_medico;   // Para ir creando dinámicamente los id de medico
  pthread_t *datos_hilo;

  if (argc!=3)  {
     fprintf(stderr,"Forma de uso: %s <numero_medicos> <max_pacientes>\n",argv[0]);
     exit(1);
  } 
  if (atoi((char *) argv[1])<=0)  {
     fprintf(stderr,"El parametro  <numero_medicos> debe ser un entero positivo\n");
     exit(3);
  }
  if (atoi((char *) argv[2])<=0)  {
     fprintf(stderr,"El parametro  <max_pacientes> debe ser un entero positivo\n");
     exit(3);
  }

  num_medicos=atoi(argv[1]);
  max_pacientes=atoi(argv[2]);

  //Reservamos memoria para los objetos de datos de hilo
  datos_hilo=(pthread_t *) malloc(sizeof(pthread_t)*num_medicos);
  if (datos_hilo==NULL)
  {

     fprintf(stderr,"No hay memoria suficiente para los objetos de datos de hilo");
     exit(4);
  }
 
  // Creación de un hilo para cada Medico. Estos sí reciben como parámetro
  // un puntero a entero que será su id_medico. Se crea dinámicamente uno
  // para cada hilo y se le asigna el contador del bucle
  int ret = 0;
  for (i=0;i<num_medicos;i++){
    id_medico = (int *) malloc (sizeof(int));
    *id_medico = i;
    // ************** A RELLENAR ****************** (creación del hilo médico)
    ret = pthread_create(&datos_hilo[i],NULL, (void *) Medico, (int *) id_medico);
    if (ret != 0){
      fprintf(stderr,"Error cliente main creacion hilos\n");
      exit(1);
    }
  }

  // main espera a que terminen todos los medicos
  for (i=0;i<num_medicos;i++) {
    pthread_join(datos_hilo[i],NULL);
  }
  free(datos_hilo);
}
