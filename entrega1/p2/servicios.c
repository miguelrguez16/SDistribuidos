#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "hospital.h"
#include "util.h"


// VARIABLES GLOBALES
// ====================================================================

int numero_quirofanos;  // numero de quirofanos disponible (se inicializan desde init)
int numero_equipos;     // numero de equipos de enfermeria (se inicializan desde init)

// Estado de cada quirofano del hospital 0 indica libre, 1 ocupado
int *estado_quirofanos; //array de estado de los quirofanos (se inicializa desde init)

// Estado de cada quirofano del hospital 0 indica libre, 1 ocupado
int *estado_equipos; //array de estado de los equipos de enfermeria (se inicializa desde init)


//No necesitamos mutex porque las peticiones se ejecutan de forma serializada
//y no concurrente

// Funciones auxiliares para inicializar arrays (no son accesibles por RPC)
// Marca como libres todas las posiciones de un cierto array
void inicializar_array_estado(int *p,int posiciones)
{
  register int i;

  for (i=0;i<posiciones;i++)
    p[i]=LIBRE;
}

// Dimensiona los arrays y marca sus elementos como libres
// Solo lo hace la primera vez que es llamada
void init() {
  // Booleano para saber si ya se han inicializado las variables anteriores (para no volver a hacerlo)
  static int inicializado = 0;
  
	if (inicializado) return;
	estado_quirofanos=(int *) malloc(sizeof(int)*numero_quirofanos);
	if (estado_quirofanos==NULL)
	{
		fprintf(stderr,"No se pudo reservar memoria para representar el estado de los quirofanos");
		exit(4);
	}        
	estado_equipos=(int *) malloc(sizeof(int)*numero_equipos);
	if (estado_equipos==NULL)
	{
		fprintf(stderr,"No se pudo reservar memoria para representar el estado de los equipos de enfermeria");
		exit(4);
	}  	
	inicializar_array_estado(estado_quirofanos,numero_quirofanos);
	mostrar_estado_recursos("QUIROFANOS",estado_quirofanos,numero_quirofanos);
	inicializar_array_estado(estado_equipos,numero_equipos);
	mostrar_estado_recursos("EQUIPO",estado_equipos,numero_equipos);
	inicializado = 1;
}


// ***************************************************************************************
// Implementación de los procedimientos remotos

int * inicializar_hospital_1_svc(init_data *data, struct svc_req * peticion){
  static int r=0;  // Variable no usada, pero necesaria para poder retornar int*

  // ******** A RELLENAR ************
  // Usa init() para inicializar recursos
  //obtener datos enviados por el cliente para inicializar
  numero_equipos=data->num_equipo;
  numero_quirofanos=data->num_quirofano;
  init();
  return &r;
}

int * reservar_quirofano_1_svc(int *id_medico, struct svc_req * peticion)
{
  static int res;
  // ******** A RELLENAR ************
  // Busca en el array de quirófanos uno libre, lo marca como ocupado
  // y retorna su índice. Si no encuentra ninguno retorna -1
  res=SIN_RECURSO;
  int i;
//mientras no haya encontrado nada y me quede por recorrer
  for (i = 0; i < numero_quirofanos && res==SIN_RECURSO; i++){
      if(estado_quirofanos[i]==LIBRE){
        res=i;
        estado_quirofanos[i]=OCUPADO;
      }
  }
  /*if(res!=SIN_RECURSO){
    printf("quirofano: %d al medico: %d\n",res,*id_medico);
  }*/
  
  // Mostramos por pantalla el estado del array, para depuración
  mostrar_estado_recursos("QUIROFANOS",estado_quirofanos,numero_quirofanos);
  return(&res);
}

int * reservar_equipo_enfermeria_1_svc(int *id_medico, struct svc_req *peticion)
{
  static int res;
  
  // ******** A RELLENAR ************
  // Busca en el array de equipos uno libre, lo marca como ocupado
  // y retorna su índice. Si no encuentra ninguno retorna -1
  res=SIN_RECURSO;
  int i;
  //mientras no haya encontrado nada y me quede por recorrer
  for (i = 0; i < numero_equipos && res==SIN_RECURSO; i++){
      if(estado_equipos[i]==LIBRE){
        res=i;
        estado_equipos[i]=OCUPADO;
      }
  }
  /*
  if(res!=SIN_RECURSO){
    printf("equipo: %d al medico: %d\n",res,*id_medico);
  }*/
  // Mostramos por pantalla el estado del array, para depuración
  mostrar_estado_recursos("EQUIPO",estado_equipos,numero_equipos);
  return(&res);

}

int * liberar_recursos_1_svc(recursos *datos, struct svc_req *peticion)
{
  static int res=0;   // Variable no usada, pero necesaria para retornar int*

  // ******** A RELLENAR ************
  // Marca como libres los recursos indicados  
  estado_equipos[datos->key_equipos]=LIBRE;
  estado_quirofanos[datos->key_quirofanos]=LIBRE;
  //printf("libera : %d, %d \n",datos->key_quirofanos,datos->key_equipos);
  // Mostramos por pantalla el estado de los arrays, para depuración
  mostrar_estado_recursos("QUIROFANOS",estado_quirofanos,numero_quirofanos);
  mostrar_estado_recursos("EQUIPO",estado_equipos,numero_equipos);
  return(&res);
}

