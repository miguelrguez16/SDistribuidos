#include <time.h>
#include <stdio.h>       // printf()
#include <stdlib.h>      // exit()
#include <sys/socket.h>  // socket(), bind(), listen(), recv(), send(), etc
#include <arpa/inet.h>   // sockaddr_in
#include <errno.h>       // perror()
#include <sys/select.h>  // select() y fd_set
#include <unistd.h>      // close()

int main(int argc, char * argv[]){
	int socket_datos;
	int bytes_recibidos;
	struct sockaddr_in dir_servidor;
	socklen_t ldir = sizeof(dir_servidor);
	unsigned long int buffer;
	unsigned long int num_dec;

	socket_datos = socket(PF_INET, SOCK_DGRAM, 0);
	dir_servidor.sin_family = AF_INET;
	dir_servidor.sin_addr.s_addr = inet_addr("129.6.15.28");
	dir_servidor.sin_port = htons(37);

	//Envio el datagrama
	sendto(socket_datos, NULL, 0, 0, (struct sockaddr *) &dir_servidor, ldir);

	//Recibo el datagram
	bytes_recibidos = recvfrom(socket_datos, &buffer, sizeof(buffer), 0,(struct sockaddr *) &dir_servidor, &ldir);

	printf("Hemos recibido: %d bytes\n", bytes_recibidos);
	buffer = ntohl(buffer);
	printf("%lu\n", buffer);
	//buffer = buffer - 2208988800;
	//printf("%s\n", ctime(&buffer));
	double anyos = buffer / (365.25 * 24 * 3600);
	printf("Años: %f\n", anyos);
}