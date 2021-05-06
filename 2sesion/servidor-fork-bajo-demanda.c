#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


void atrapa(int s){//atrapar la señal 
    wait(0);
    return;
}

int main(int argc, char *argv[]){
    if(argc != 2){ 
        printf("Error número de argumentos\n");
        exit(1);
    } 

    int sock_datos, sock_pasivo;
    struct sockaddr_in dir_local; //Direccion local pasivo 
    int leidos;
    char buffer[100];
    pid_t pid;

    signal(SIGCHLD,atrapa);

    sock_pasivo=socket(PF_INET,SOCK_STREAM,0);//Estableces sock pasivo
    dir_local.sin_family =AF_INET;
    dir_local.sin_addr.s_addr=htonl(INADDR_ANY); //localhost
    dir_local.sin_port=htons(atoi(argv[1]));

    if(bind(sock_pasivo, (struct sockaddr *) &dir_local,sizeof(dir_local))<0){
        perror("Error al vincular el socket");
        exit(1);
    }

    listen(sock_pasivo,SOMAXCONN);

    while(1){
        printf("Padre antes del accept pid: (%d)\n",getpid());
          if((sock_datos=accept(sock_pasivo,0,0))<0){
            perror("Error en accept");
        }else{
            printf("Padre antes del fork pid: (%d)\n",getpid());
            if((pid=fork())<0){
                perror("Error en el fork()\n");
                close(sock_datos);
            } else if(pid==0){ //Estoy en el hijo
                printf("Hijo (%d) antes de lectura\n",getpid());

                while( (leidos=read(sock_datos, buffer,sizeof(buffer)))>0){
                     write(sock_datos,buffer,leidos);
                }
                printf("Hijo (%d) despues de lectura\n",getpid());

                close(sock_datos);
                exit(0);
            }else{
                printf("Padre (%d)despues del fork\n", getpid());
                close(sock_datos);
            }
        }
    }
}