#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int numProcesosHijosFin;

void finalizarHijo(int s)   {
    numProcesosHijosFin--;
    return;
}

int main(int argc, char *argv[]){
	
	int sock_pasivo, sock_datos;
	struct sockaddr_in dir_local;		
	int resultado_bind, resultado_listen, puerto;
	int bytes_recibidos, bytes_enviados;
	char buffer[1000];
	numProcesosHijosFin = 0;
	
    
	
	signal(SIGCHLD, finalizarHijo);
	
	
	puerto = atoi(argv[1]);
	
	sock_pasivo = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_pasivo == -1) {
		perror("Al crear el socket"); 
		exit(1);
	}
	
	dir_local.sin_family = AF_INET;
	dir_local.sin_port = htons(puerto);
	dir_local.sin_addr.s_addr = htonl(INADDR_ANY);	

	resultado_bind = bind(sock_pasivo, (struct sockaddr *) &dir_local, sizeof(dir_local));
	if(resultado_bind == -1) {
		perror("Al usar el bind"); 
		exit(1);
	}
	
	resultado_listen = listen(sock_pasivo, SOMAXCONN);
	if(resultado_listen==-1) {
		perror("A la escucha"); 
		exit(1);
	}
	
	//Con esto, el primer fork crea un proceso, como ahora tenemos dos el siguiente
	//Creará dos procesos, total de 4, y una vez más será un proceso por cada uno
	//Luego habrá ocho procesos extra creados.
	int i;
	for(i = 0; i < 3; i++){
		if(fork() == 0) break;
	}
	
	int acepta_servicio;
	
	while(1){
		printf("Proceso padre con pid %d esperando clientes...\n", getppid());
		sock_datos = accept(sock_pasivo, NULL, NULL);
		if(sock_datos == -1) {
			perror("En la acepta de clientes"); 
			exit(1); 
		}
		printf("Se ha aceptado una conexion\n");
		if(fork() == 0) {
			//Un proceso hijo atiende al nuevo cliente 
			close(sock_pasivo);
			acepta_servicio = 1;
			while(acepta_servicio){
				printf("Hijo %d atendiendo conexion...\n", getpid());
				bytes_recibidos = recv(sock_datos, buffer, 100, 0);
				if(bytes_recibidos ==-1) { perror("En recv"); exit(1); }
				//Si no se acepta ningun byte, es que el cliente se ha desconectado 
				if(bytes_recibidos == 0){
					printf("El cliente que atiende el proceso %d se ha desconectado\n", getpid());
					close(sock_datos);
					acepta_servicio = 0;
				}
				else {
					send(sock_datos, buffer, bytes_recibidos, 0);
					buffer[bytes_recibidos] = 0;
					printf("Proceso %d : %s \n", getpid(), buffer);
				}
			}
			printf("El proceso %d finaliza...\n", getpid());
			exit(0);
		} else {
			close(sock_datos);
			numProcesosHijosFin++;
			printf("Hijos disponibles %d\n", numProcesosHijosFin);
			if(numProcesosHijosFin == 4) wait(0);
		}
	}	
	return 0;
}