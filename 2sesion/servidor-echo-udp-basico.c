#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[]){

    if(argc !=2) {
        printf("Error en el numero de argumentos %d de 2",argc);
        exit(1);
    }
    
    int socket_datos,recibidos;
    struct sockaddr_in d_local,d_cliente;
    socklen_t ldir = sizeof(d_cliente);
    char buffer[2000];

    socket_datos= socket(PF_INET, SOCK_DGRAM,0); //parametro UDP
    if(socket_datos == -1){
        perror("Error en socket");
        exit(1);
    }
    d_local.sin_family =AF_INET;
    d_local.sin_addr.s_addr = htonl(INADDR_ANY); //para que el cliente se conecte debe ser localhost
    d_local.sin_port = htons(atoi(argv[1])); //Puerto pasado por parámetro

    if((bind(socket_datos,(struct sockaddr *)&d_local,sizeof(d_local))) == -1){
        perror("Error al vincular el socket");
        exit(1);
    }
    while(1){ //atencion a clientes
        if((recibidos=recvfrom(socket_datos,buffer,sizeof(buffer),0, (struct sockaddr *) &d_cliente, &ldir))<0){
            perror("Error en el recv");
        }else{
            //printf("Cliente desde %s (%d)\n", inet_ntoa(d_cliente.sin_addr), ntohs(d_cliente.sin_port));
            if(sendto(socket_datos,buffer, recibidos, 0 , (struct sockaddr *) &d_cliente, ldir)<0){
                perror("Error en el sendto");
            }
        }
    }
}