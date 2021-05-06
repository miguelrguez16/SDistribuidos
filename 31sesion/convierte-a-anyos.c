#include <stdio.h>
int main(){
    unsigned long int numero = 0xD4CCF49E;  // Pon el tuyo
    double segundos_por_anyo = 365.25 * 24 * 60 * 60;
    double anyos = numero / segundos_por_anyo;
    printf("%f\n", anyos);
    return 0;
}