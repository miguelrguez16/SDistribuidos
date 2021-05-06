#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
    if(argc != 2){
        perro("Number of arguments not valid");
        exit(1);
    }

    int socket_datos, recibidos;
    struct sockaddr_in d_serv; //Direccion servidor
    socklen_t ldir = sizeof(d_serv);
    char *fin="FIN\n";
    char intro[500];

    socket_datos = socket(PF_INET,SOCK_DGRAM,0);

    d_serv.sin_family = AF_INET;
    d_serv.sin_addr.s_addr=inet_addr("127.0.0.1");
    d_serv.sin_port = htons(atoi(argv[1]));
    
    do{
       printf("-> ");
       fgets(intro,499,stdin);
       sendto(socket_datos,intro,strlen(intro),0, (struct sockaddr *)&d_serv,ldir );
       recibidos = recvfrom(socket_datos ,intro,500,0,
        (struct sockaddr *)&d_serv,&ldir);
       intro[recibidos]=0;
       printf("%s\n",intro);

    }while(strcmp(intro,fin)!=0);
    exit(0);



}