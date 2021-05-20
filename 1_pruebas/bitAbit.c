#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

int contador=1;

void binario(unsigned char *tok, unsigned char *tokenBits);
void anadir(unsigned char *tok);


void anadir(unsigned char *tok){
    unsigned char tokenaux,tmp, bit=1;
    printf("Valor del contador %d\n",contador);
    if(contador==1){        //1
        tokenaux=~*tok;
        bit=bit<<5;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;
        printf("Nuevo Valor char %d\n",*tok);
        contador++;
        return ;
    }else if(contador==2){  //2
        tokenaux=~*tok;
        bit=bit<<5;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;//original sin 3 primeros
        bit=bit<<1;
        tokenaux=~*tok;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;
        printf("Nuevo Valor 2 char %d\n",*tok);
        contador++;
        return ;
    }else if(contador==3){  //3
        tokenaux=~*tok;
        bit=bit<<6;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;//original sin 3 primeros
        //montamos mascara
        bit=1;
        bit=bit<<5;
        tmp=1;
        tmp=tmp<<6;
        bit|=tmp;//mascara con 0 1 1 ...
        //Nuevo valor
        tokenaux=~*tok;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;
        printf("Nuevo Valor 3 char %d\n",*tok);
        contador++;
        return ;
    }else if(contador==4){ //4
        tokenaux=~*tok;
        bit=bit<<5;
        tmp=1;
        tmp=tmp<<6;
        bit|=tmp;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;//original sin 3 primeros
        //montamos la mascara
        bit=bit<<2;// mascara 1 0 0 ..
        tokenaux=~*tok;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;
        printf("Nuevo Valor4 char %d\n",*tok);
        contador++;
        return ;
    }else if(contador==5){ //5
        tokenaux=~*tok;
        bit=bit<<7;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;//original sin 3 primeros
        //montamos la mascara
        tmp=1;
        tmp=tmp<<5;
        bit|=tmp;
        tokenaux=~*tok;
        tokenaux=tokenaux^bit;
        *tok=~tokenaux;
        printf("Nuevo Valor 5 char %d\n",*tok);
        contador++;
        return ;
    }else{
        printf("\n WARNING");
        return ;
    }
}

void binario(unsigned char *tok, unsigned char *tokenBits){
    unsigned char aux,bit;
    //vamos bit a bit
  	for(int i = 0; i < 8;i++) {
        bit = 1;
        //aux=token>>1;
	    aux=*tok;
	    aux=aux>>i;
		  bit&=aux;
		  if(bit){
        tokenBits[7-i]='1';
      }else{ 
        tokenBits[7-i]='0';
      }
    }
}


void main(){
    unsigned char tok = 30;
    unsigned char bit = 1;

    unsigned char tokenaux;
    unsigned char tokenBits[8];

    printf("char %d\n",tok);
    anadir(&tok);
    printf("char %d\n",tok);
    binario(&tok,tokenBits);
    printf("char %d\n",tok);
    printf("char tokenBits %s\n",tokenBits);
printf("\n");
    printf("char %d\n",tok);
    anadir(&tok);
    printf("char %d\n",tok);
    binario(&tok,tokenBits);
    printf("char %d\n",tok);
    printf("char tokenBits %s\n",tokenBits);
printf("\n");
    printf("char %d\n",tok);
    anadir(&tok);
    printf("char %d\n",tok);
    binario(&tok,tokenBits);
    printf("char %d\n",tok);
    printf("char tokenBits %s\n",tokenBits);
printf("\n");
    printf("char %d\n",tok);
    anadir(&tok);
    printf("char %d\n",tok);
    binario(&tok,tokenBits);
    printf("char %d\n",tok);
    printf("char tokenBits %s\n",tokenBits);
printf("\n");
    printf("char %d\n",tok);
    anadir(&tok);
    printf("char %d\n",tok);
    binario(&tok,tokenBits);
    printf("char %d\n",tok);
    printf("char tokenBits %s\n",tokenBits);
}

