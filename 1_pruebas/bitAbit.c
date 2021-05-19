#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

void main(){
    unsigned char tok = 45;
    unsigned char tokenaux;
    tokenaux=~tok;

    printf("char %u\n",tok);
    printf("char %u\n",tokenaux);

    tok+=32;
    printf("char %u\n",tok);

    tokenaux = 1;
    tokenaux = tokenaux << 5;
    tok&=tokenaux;
    printf("char %hhn\n",tok);


}