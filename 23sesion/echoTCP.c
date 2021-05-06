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


#define PUERTO 48666 // Puerto por defecto (el estándar 7 está protegido)
#define MAX_LINEA 80

/**** Definición de tipos de datos ****/
// Datos de la conexión (que se le pasan al hilo)
struct conex{
  char *origen;       //IP origen
  int puerto_origen;  //Puerto de origen
  int sock;           //Socket de conexión con el cliente
  unsigned int numhilo;  //Ordinal del hilo que atiende la petición
};
typedef struct conex datos_conexion;

//Socket de escucha y de datos
int sockEscucha, sockDatos;

/* Un manejador para cuando se pulse ctrl-c enb el servidor*/
void SalirBien(int s){
    printf("Interrupcion desde teclado.\nTerminando servidor");
    close(sockEscucha);
    exit(0);
}


int CrearSocketServidorTCP(int puerto){
    int ret;
    int sock;
    struct sockaddr_in dir;

    //creacion del socket --> TCP
    ret=socket(PF_INET, SOCK_STREAM,0);
    if (ret==-1){ 
        perror("Error al crear el socket"); 
        exit(1);
    }

    sock=ret;
    dir.sin_family=AF_INET;
    dir.sin_port=htons(puerto);
    dir.sin_addr.s_addr=htonl(INADDR_ANY);

    ret=bind(sock, (struct sockaddr *) &dir, sizeof(dir));
    if (ret==-1){ 
        perror("Error al asignar ip al socket"); 
        close(sock); 
        exit(1);
    }

    ret=listen(sock, SOMAXCONN);
    if (ret==-1){ 
        perror("Error al poner en modo escucha"); 
        close(sock); 
        exit(1);
    }
    printf("completado creacion socket de escucha\n");
    return sock;
}

int AceptarConexion(int sock, char **ip, int *puerto){
    int ret;
    struct sockaddr_in dir;
    static socklen_t longitud= sizeof (dir);

    ret=accept(sock, (struct sockaddr *) &dir, &longitud);
    if (ret==-1){ 
        perror("Error al aceptar"); 
        exit(1);
    }

    *ip=inet_ntoa(dir.sin_addr); //guardar ip en 2
    *puerto=ntohs(dir.sin_port); //guardar puerto en 3

    printf("Aceptada conexion por el servidor\n");
    return ret;
}

int Enviar(int sock, char *buff, int longitud){
    int ret;
    ret = send(sock, buff, longitud,0);
    if(ret==-1) { 
        perror("Error al enviar"); 
        exit(1);
    }
    printf("Correcto al enviar -- servidor");

    return ret;
}

int Recibir(int sock, char *buff, int longitud){
    int ret;
    ret = recv(sock, buff, longitud,0);
    if(ret==-1) {
        perror("Error al recibir");
        exit(1);
    }
    printf("Correcto al recibir -- servidor");
    return ret;
}

void CerrarSocket(int sock){
    if(close(sock)==-1){
        perror("ERROR: al cerrar el socket");
    }
}

//Funcion que ejecuta el hilo que aitende cada peticion
void * servidorEcho(datos_conexion *s){
    int recibidos;
    char buffer [MAX_LINEA];
    printf("Hilo %d: -- Recibida conexión desde %s(%d)\n",s->numhilo, s->origen, s->puerto_origen);

    do{
        recibidos=Recibir(s->sock, buffer,MAX_LINEA);
        printf("Hilo %d: -- Recibido un mensaje de long: %d\n",s->numhilo, recibidos);
        buffer[recibidos]=0; //terminador
        printf("Hilo %d: -- Contenido: %s\n", s->numhilo, buffer);
        Enviar(s->sock,buffer,strlen(buffer));
    }while(recibidos!=0);
    CerrarSocket(s->sock);
    free(s->origen);
    free(s);
    return NULL;

}


int main(int argc, char *argv[]){
    int puerto;
    char *desde;
    int puerto_desde;
    int n_hilos = 0;
    pthread_t tid;
    datos_conexion *con;

    if(argc<2){
        printf("Uso: %s [puerto]\n", argv[0]);
        printf("    puerto por defecto = %d\n", PUERTO);
    }

    if(argc >1) puerto=atoi(argv[1]);
    else puerto=PUERTO;


    //tratar el ctrl-c
    signal(SIGINT,SalirBien);

    sockEscucha = CrearSocketServidorTCP(puerto);
    while(1){
        //esperamos la llegada de nuevas conexiones
        sockDatos=AceptarConexion(sockEscucha, &desde, &puerto_desde);
        //Reservamos espacio para almacenar los datos de la nueva conexión
        //se pasan todos los datos a la estructura para pasarselos al hilo
        con=(datos_conexion *) malloc(sizeof(datos_conexion));
        con->origen=(char *) malloc(strlen(desde)+1);
        strcpy(con->origen,desde);
        con->puerto_origen=puerto_desde;
        con->sock=sockDatos;
        con->numhilo=n_hilos;

        //Creamos un nuevo hilo que atenderá la nueva conexión
        pthread_create(&tid,NULL,(void *) servidorEcho, (void *) con);
        //hacemos que el hilo sea independiente
        pthread_detach(tid);
        printf("Servidor prin: Lanzado el hijo: %d.\n", n_hilos);

        //incremento del número de hilos lanzados
        n_hilos++;
    }
        return 0;
}
