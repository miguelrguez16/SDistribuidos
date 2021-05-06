COMPIL=gcc
CFLAGS=-Wall -lm -pthread

all: cleanall genera servidor cliente inicializador_hospital
		@ echo "Compilado"

cleanall : clean
	@rm -f servidor cliente inicializador_hospital

clean :
	@rm -f *.o core *~

genera: hospital.x
	rpcgen hospital.x

servidor: servicios.c util.c hospital_svc.c hospital_xdr.c
	$(COMPIL) -o servidor servicios.c util.c hospital_svc.c hospital_xdr.c

cliente: cliente.c util.c hospital_clnt.c hospital_xdr.c
	$(COMPIL) -o cliente cliente.c util.c hospital_clnt.c hospital_xdr.c $(CFLAGS)

inicializador_hospital: inicializador_hospital.c hospital_clnt.c hospital_xdr.c
	$(COMPIL) -o inicializador_hospital inicializador_hospital.c hospital_clnt.c hospital_xdr.c