#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main(int argc,char *argv[]){
    if(argc != 2){ 
        printf("Error número de argumentos\n");
        exit(1);
    } 

    int sock_datos, sock_pasivo;
    struct sockaddr_in dir_local; //Direccion local pasivo 
    int leidos;
    char buffer[100];
    pid_t pid;

    sock_pasivo=socket(PF_INET,SOCK_STREAM,0);//Estableces sock pasivo
    dir_local.sin_family =AF_INET;
    dir_local.sin_addr.s_addr=htonl(INADDR_ANY); //localhost
    dir_local.sin_port=htons(atoi(argv[1]));

    if(bind(sock_pasivo, (struct sockaddr *) &dir_local,sizeof(dir_local))<0){
        perror("Error al vincular el socket\n");
        exit(1);
    }

    listen(sock_pasivo,SOMAXCONN);

    //POOL de procesos
    for (int i = 0; i < 3; i++){
        if((pid=fork())<0){
            perror("Error en el fork\n");
        }else if(pid==0){  break;  } //hijo
    }
    printf("creación de hijos\n");
    
    while(1){ //los 4 procesos entran aquí, accept es bloqueante
        printf("Proceso: (%d) antes accept()\n",getpid());
        if((sock_datos=accept(sock_pasivo,0,0))<0){
            perror("Error en accept");
        } else{
            printf("Proceso: (%d) antes de lectura\n",getpid());
            while( (leidos=read(sock_datos, buffer,sizeof(buffer)))>0){
                     write(sock_datos,buffer,leidos);
            }
            printf("Proceso: (%d) despues de lectura\n",getpid());
            printf("Proceso: (%d) cliente desconectado\n",getpid());
            close(sock_datos);
        }
    }
}