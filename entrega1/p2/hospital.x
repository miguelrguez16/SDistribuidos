/* Declaración de los tipos de datos necesarios para las funciones */

struct init_data {
  int num_quirofano;
  int num_equipo;
};

struct recursos {
  int key_quirofanos;
  int key_equipos;
};

program HOSPITAL{
   version PRIMERA{
     int inicializar_hospital(init_data)=1;
     int reservar_quirofano(int)=2;
     int reservar_equipo_enfermeria(int)=3;
     int liberar_recursos(recursos)=4;
   }=1;  
}=400000073; /* Pon aqui el numero de programa que quieras, respetando las limitaciones de ONCRPC  rango hasta 500000F*/
