#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>


void main(int argc, char *argv[]){

    char buffer[100]; //leera del archivo en bytes
    pid_t pid;          //pid procesos
    size_t cuantos;

    int fds[2]; //array de enteros de dos posiciones
    pipe(fds);  //Crea un pipe

    int fd; //descriptor del fichero

    if(argc!=2){
        fprintf(stderr, "Uso: ejecutable archivo\nNumber of arguments:%d\n", argc);
        exit(1);
    }

    if((fd=open(argv[1],O_RDONLY))<0){
    //if((fd=open("readme.txt",O_RDONLY))<0){
        fprintf(stderr, "Error apertura de fichero\n");
       // fprintf(stderr, "Fichero: %s\n",argv[1]);
        exit(2); 
    }

    if ((pid =fork()) < 0){
        fprintf(stderr, "Error primer fork");
        exit(1);
    }
    else if(pid ==0) { //Estoy en el hijo
        //envia el archivo al otro hijo
        //cierra el descriptor de salida (lectura) de la pipe (no lo utiliza)
        close(fds[0]);
        while ((cuantos=read(fd,buffer,sizeof(buffer)))>0){ //lee hasta que detecte el fin fichero
            write(fds[1],buffer,cuantos); //-> Escribe en el extremo de escritura de la pipe
        }
        close(fd);      //cierro el fichero (mio)
        close(fds[1]);
    }else{
        if ((pid =fork())<0){
        fprintf(stderr, "Error segundo fork");
        exit(1);
        } else if (pid == 0){
            //cierra el descriptor de entrada (escritura) de la pipe (no lo utiliza)
            close(fds[1]);
            //debe leer de la pipe
            while((cuantos = read(fds[0],buffer, sizeof(buffer))) >0){
                write(STDOUT_FILENO, buffer,cuantos); //escribe por la salida estándar
            }
            close(fds[0]);

        } else{
            //El padre cierra ambos extremos de la pipe y sincroniza con los hijos
            close(fd);
            close(fds[0]); close(fds[1]);
            wait(0); wait(0);
        }
    }
}
