#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define MAX_LINEA 80

/*Variables que quizas sean necesarias para el programa*/
int cabeza, cola, TAM_COLA, socket_Escucha, socket_Datos, puerto;
sem_t num_ocupados;
sem_t num_huecos;
struct sockaddr_in dir;
int *cola_de_trabajos;

/* Un manejador para cuando se pulse Ctrl-C en el servidor*/
void SalirBien(int s)
{
  printf("Interrupcion desde teclado. Terminando servidor. \n");
  close(socket_Escucha);
  exit(0);
}

/* Funcion para crear el socket pasivo de escucha*/
int CrearSocketServidorTCP(void * s)
{
  int valor_retornado;
  int socket_Interno;

  valor_retornado=socket(PF_INET, SOCK_STREAM, 0);
  if (valor_retornado==-1) {
    perror("Al crear socket");
    exit(-1);
  }
  socket_Interno = valor_retornado;
  dir.sin_family=AF_INET;
  dir.sin_port=htons(puerto);
  dir.sin_addr.s_addr=htonl(INADDR_ANY);
  valor_retornado=bind(socket_Interno,(struct sockaddr *)  &dir, sizeof(dir));
  if (valor_retornado==-1) {
    perror("Al asignar direccion");
    close(socket_Interno);
    exit(-1);
  }
  valor_retornado=listen(socket_Interno, SOMAXCONN);
  if (valor_retornado==-1) {
    perror("Al poner en modo escucha");
    close(socket_Interno);
    exit(-1);
  }
  return socket_Interno;
}

/* Funcion Echo -> para el paso de mensajes */
void *  servicioEcho(int socket_Interno){
	int bytes_recibidos, bytes_enviados;
	char buffer[MAX_LINEA];
	do{
		bytes_recibidos=recv(socket_Interno, buffer, MAX_LINEA, 0);
		if(bytes_recibidos==-1){
			perror("Error en la funcion recv() - al recibir los datos");
			exit(-1);
		}
		printf("Recibido %d bytes - Mensaje: %s", bytes_recibidos, buffer);
		buffer[bytes_recibidos]=0; 

		bytes_enviados=send(socket_Interno, buffer, strlen(buffer), 0);
		if(bytes_enviados == -1){
			perror("Error en la funcion send() - al enviar los datos");
			exit(-1);
		}
	}while(bytes_recibidos != 0);
	if(close(socket_Interno)==-1){
		perror("Al cerrar el socket");
	}
	return NULL;
}

/* Hilo jefe que se encarga de escuchar nuevas conexiones y meterlas en la cola de 
	trabajos*/
void hilo_jefe(void * s){
	int socket_Interno;
	while(1){
		printf("Hilo jefe a la espera de clientes...\n");
		socket_Interno = accept(socket_Escucha, 0, 0);
		if(socket_Interno == -1){
			perror("Error en la funcion accept() - al aceptar un trabajo en el jefe");
			exit(-1);
		}
		printf("Se ha aceptado una nueva conexion ->\n");
		sem_wait(&num_huecos);
		cola_de_trabajos[cabeza]=socket_Interno;
		cabeza = (cabeza + 1) % TAM_COLA;
		sem_post(&num_ocupados);
	}
}

/* Metodo de los hilos trabajadores que son quienes procesan los trabajos
	de la cola de trabajos */
int hilo_trabajador(void * s){
	int socket_Interno;
	while(1){
		sem_wait(&num_ocupados);
		pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
		int pthread_mutex_lock(pthread_mutex_t *mutex);
		socket_Interno=cola_de_trabajos[cola];
		cola = (cola + 1) % TAM_COLA;
		int phtread_mutex_unlock(pthread_mutex_t *mutex);
		int pthread_mutex_destroy(pthread_mutex_t *mutex);
		servicioEcho(socket_Interno);
		sem_post(&num_huecos);
		}
}


int main(int argc, char * argv[]){

	/* Inicializamos algunas estructuras de datos que puedan ser necesarios + datos */
	int numThreads;
	cabeza = 0;
	cola = 0;
	pthread_t tid_jefe;
	pthread_t tid_trabajador;

	if (argc<2) {
		printf("Uso: %s <puerto>\n", argv[0]);
		exit(1);
	}

	puerto = atoi(argv[1]);
	TAM_COLA= 5;
	numThreads = 3;
	cola_de_trabajos = (int *) malloc (TAM_COLA * sizeof(int));


	// Tratar el Ctrl-C para cerrar bien el socket de escucha
	signal(SIGINT, SalirBien);

	//PASO 2: crear el soket pasivo
	socket_Escucha=CrearSocketServidorTCP(NULL);

	/* Creamos los semaforos y los inicializamos */

	sem_init(&num_huecos, 0, TAM_COLA);
	sem_init(&num_ocupados, 0, 0);

	/* Creamos el hilo jefe que será el que trate la cola de trabajos */
	pthread_create(&tid_jefe, NULL, (void*) hilo_jefe, NULL);
	printf("El hilo jefe está operativo y esperando trabajos...\n");

	/* Creamos los hilos trabajadores indicados segun la variable */
	int iterador;
	for(iterador = 0; iterador < numThreads; iterador++){
		pthread_create(&tid_trabajador, NULL, (void *) hilo_trabajador, NULL);
		printf("Hilo trabajador %d lanzado esperando trabajo...\n", iterador);
	}
	pthread_join(tid_jefe, NULL);

	return 0;
}
