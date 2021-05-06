COMPIL=gcc
CFLAGS=-Wall -lm -pthread

all: cleanall hospital
		@ echo "Compilado"

cleanall : clean
	@rm -f hospital

clean :
	@rm -f *.o core *~

hospital : hospital.c cola.c util.c
	$(COMPIL) -o hospital hospital.c cola.c util.c $(CFLAGS)