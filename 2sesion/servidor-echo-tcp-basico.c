#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[]){

    if(argc !=2) {
        printf("Error en el numero de argumentos %d de 2",argc);
         exit(1);}
         
    int socket_pasivo, socket_datos;
    struct sockaddr_in d_local;
    char buffer[2000];
    int recibidos;
    socket_pasivo= socket(PF_INET, SOCK_STREAM,0);
    if(socket_pasivo == -1){
        perror("Error en socket");
        exit(1);
    }
    d_local.sin_family =AF_INET;
    d_local.sin_addr.s_addr = htonl(INADDR_ANY);
    d_local.sin_port = htons(atoi(argv[1])); //Puerto pasado por parámetro

    if((bind(socket_pasivo,(struct sockaddr *)&d_local,sizeof(d_local))) == -1){
        perror("Error en bind");
        exit(1);
    }
    listen(socket_pasivo, SOMAXCONN);

    while(1){
        if((socket_datos=accept(socket_pasivo,0,0))<0){
            perror("Error en accept");
        }else{
            while((recibidos=read(socket_datos,buffer,sizeof(buffer))) > 0){
                //write(socket_datos,buffer,recibidos);
                send(socket_datos,buffer,recibidos,0);

            }
            close(socket_datos);
        }
        
    }
}
// recibidos = read(socket_datos, buffer,2000);
//          printf("Mensaje: %s\n",buffer);
//          send(socket_datos,buffer,recibidos,0);

