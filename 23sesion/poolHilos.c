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

#define PUERTO 488448
#define MAX_LINEA 80

struct ColaTrabajos {
	int *datos;
	int tam_cola;
	int head;
	int tail;
	sem_t posiciones_libres;
	sem_t posiciones_ocupadas;
	pthread_mutex_t mutex;
}; typedef struct ColaTrabajos ColaTrabajos;

void inicializar_cola(ColaTrabajos *q, int tam){
	int tmp;
	q->datos=(int *) malloc (sizeof(int)*tam);
	if(q->datos==NULL){
		perror("Error al reservar memoria\n");
		exit(1);
	}
	q->tam_cola=tam;
	q->head=0;
	q->tail=0;

	//al principio la cola esta vacia
	tmp = sem_init(&q->posiciones_libres, 0, tam);
	if (tmp !=0 ){
		perror("Error al inicializar el semaforo posiciones_libres\n");
		exit(1);
	}
	//al principio la cola esta vacia
	tmp = sem_init(&q->posiciones_ocupadas, 0, 0);
	if (tmp !=0 ){
		perror("Error al inicializar el semaforo posiciones_ocupadas\n");
		exit(1);
	}

	tmp = pthread_mutex_init(&q->mutex,NULL);
	if(tmp !=0 ){
		perror("Error al inicializar el mutex\n");
		exit(1);
	}

	printf("Cola inicializada con %d de longitud\n",q->tam_cola);

}
void destruir_cola(ColaTrabajos *q){
	int tmp = 0;
	free(q->datos);
	//destruir los semaforos y el mutex
	//dar de naja al semaforo en la libreria de hilos, se va a dejar de utilizar
	tmp = sem_destroy(&q->posiciones_libres);
	if(tmp !=0 ){
        perror("Error al liberar el semaforo posiciones_libres\n"); 
        exit(1);
    }

	tmp = sem_destroy(&q->posiciones_ocupadas);
	if(tmp !=0 ){
        perror("Error al liberar el semaforo posiciones_ocupadas\n");
        exit(1);
    }

	tmp=pthread_mutex_destroy(&q->mutex);
	if(tmp != 0) {
		perror("Error al liberar el mutex\n");
	}
}
void insertar_dato_cola(ColaTrabajos *q, int socket){ 
	//no seria necesario solo hay ub hilo jefe
	//intentar hacer operacion de decremento sobre el semaforo
	//man sem_wait
	int tmp;

	//Decrementar el semaforo
	tmp = sem_wait(&q->posiciones_libres);
	if(tmp !=0 ){
		perror("Error insertar dato - decrementar semaforo\n");
		exit(1);
	}

	//coger el Mutex
	tmp = pthread_mutex_lock(&q->mutex);
	if(tmp !=0 ){
		perror("Error insertar dato - coger el mutex\n"); 
		exit(1);
	}
	printf("Valor de insertar socket: %d\n", socket);
	q->datos[q->head] = socket;
	q->head = (q->head + 1) % q->tam_cola;

	tmp = pthread_mutex_unlock(&q->mutex);
	if(tmp != 0){ 
		perror("Error insertar dato - soltar el mutex\n"); 
		exit(1);
	}

	//decrementar el semaforo
	tmp = sem_post(&q->posiciones_ocupadas);
	if(tmp ==-1){ 
		perror("Error insertar dato - decrementar semaforo\n"); 
		exit(1);
	}
}
int obtener_dato_cola(ColaTrabajos *q){
	int descriptor;
	int tmp;

	tmp = sem_wait(&q->posiciones_ocupadas);
	if(tmp ==-1){
		perror("Error obtener dato - decrementar semaforo\n");
		exit(1);
	}

	tmp = pthread_mutex_lock(&q->mutex);
	if(tmp != 0) {
		perror("Error obtener dato - coger mutex\n");
		exit(1);
	}
	descriptor = q->datos[q->tail];
	q->tail = (q->tail + 1) % q->tam_cola;


	tmp = pthread_mutex_unlock(&q->mutex);
	if(tmp != 0) {
		perror("Error obtener dato - soltar mutex\n");
		exit(1);
	}

	tmp = sem_post(&q->posiciones_libres);
	if(tmp ==-1){ 
		perror("Error obtener dato - decrementar semaforo\n"); 
		exit(1);
	}
	return descriptor;

}
/*************************************/
//Establecimiento de la conexion
int sockEscucha, sockDatos;
ColaTrabajos q;

void SalirBien(int s){
	perror("Interrupcion de servidor.\n");
	close(sockEscucha);
	destruir_cola(&q);
	exit(0);
}
int CrearSocketServidorTCP(int puerto) {
	int ret;
	int sock;
	struct sockaddr_in dir;

	ret=socket(PF_INET, SOCK_STREAM, 0);
	if (ret==-1) {
		perror("Error al crear socket\n");
    	exit(1);
  	}
  	sock=ret;
  	dir.sin_family=AF_INET;
  	dir.sin_port=htons(puerto);
 	dir.sin_addr.s_addr=htonl(INADDR_ANY);


  	ret=bind(sock,(struct sockaddr *)  &dir, sizeof(dir));
 	if (ret==-1) {
    	perror("Error al asignar direccion\n");
    	close(sock);
    	exit(1);
  	}
  	ret=listen(sock, SOMAXCONN);
  	if (ret==-1) {
    	perror("Error al poner en modo escucha\n");
    	close(sock);
    	exit(1);
  	}

  	printf("Socket Escucha creado\n");
  	return sock;
}
int Enviar(int sock, char *buff, int longitud){
    int ret;
    ret = send(sock, buff, longitud,0);
    if(ret==-1) { 
        perror("Error al enviar\n"); 
        exit(1);
    }
    //printf("Correcto al enviar -- servidor");

    return ret;
}

int Recibir(int sock, char *buff, int longitud){
    int ret;
    ret = recv(sock, buff, longitud,0);
    if(ret==-1) {
        perror("Error al recibir");
        exit(1);
    }
    //printf("Correcto al recibir -- servidor\n");
    return ret;
}
int AceptarConexion(int sock) {
	int ret;
	struct sockaddr_in dir;
	static socklen_t longitud=sizeof(dir);

	ret=accept(sock, (struct sockaddr *) &dir, &longitud);
	if (ret==-1) {
    	perror("Error al aceptar\n");
    	exit(1);
 	}
  	printf("Conexion Aceptada\n");
  	return ret;
}

void CerrarSocket(int sock){
	if(close(sock)==-1){
		perror("Error al cerrar el socket\n");
	}
}

/* Funcion Echo -> para el paso de mensajes */
void * servicioEcho(int socket_Interno){
	int bytes_recibidos, bytes_enviados;
	char buffer[MAX_LINEA];
	do{
		bytes_recibidos=Recibir(socket_Interno,buffer,MAX_LINEA);
		printf("Recibido: %d bytes con: %s\n", bytes_recibidos, buffer);
		buffer[bytes_recibidos]=0; 
		Enviar(socket_Interno,buffer,strlen(buffer));
	}while(bytes_recibidos != 0);
	if(close(socket_Interno)==-1){
		perror("Al cerrar el socket\n");
	}
	return NULL;
}

/* Hilo jefe que se encarga de escuchar nuevas conexiones y meterlas en la cola de  trabajos*/
void hilo_jefe(void * s){
	int socket_para_trabajar;
	while(1){
		printf("Hilo jefe a la espera de clientes...\n");
		socket_para_trabajar = AceptarConexion(sockEscucha);
		printf("Aceptada nueva conexion ->%d\n",socket_para_trabajar);
		insertar_dato_cola(&q,socket_para_trabajar);
	}
}

/* Metodo de los hilos trabajadores que son quienes procesan los trabajos
	de la cola de trabajos */
int hilo_trabajador(void * s){
	int socket_trabajar;
	while(1){
		socket_trabajar=obtener_dato_cola(&q);
		servicioEcho(socket_trabajar);
	}
}

int main (int argc, char *argv[]){
	int numThreads;
	pthread_t tid_jefe;
	pthread_t tid_trabajador;
	int puerto;
	int TAM_COLA = 5;
	

	if (argc<2) {
   		printf("\nUso: %s [puerto]\n", argv[0]);
    	printf("     puerto por defecto = %d\n", PUERTO);
  	}

	if (argc>1) puerto=atoi(argv[1]);
  	else puerto=PUERTO;

	signal(SIGINT, SalirBien);


	numThreads=5;
	//numThreads = sysconf(_SC_NPROCESSORS_ONLN)*1,2;
	//printf("número de hilos: %d\n", numThreads);
	/* INICIALIZAR LA COLA DE TRABAJOS*/
	inicializar_cola(&q,TAM_COLA);
	/* ESTABLECER SOCKET DE ESCUCHA*/
	sockEscucha=CrearSocketServidorTCP(puerto);


	/*CREACION DEL HILO JEFE*/
	pthread_create(&tid_jefe, NULL, (void*) hilo_jefe, NULL);
	printf("El hilo jefe está operativo y esperando trabajos...\n");


	/*CREACION DE LOS HILOS TRABAJADORES*/
	int i;
	for (i = 0; i < numThreads; i++){
		pthread_create(&tid_trabajador, NULL, (void *) hilo_trabajador, NULL);
		printf("Hilo trabajador: %d lanzado\n", i);
	}
	pthread_join(tid_jefe, NULL);

	return 0;
}
