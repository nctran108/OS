CC = gcc 
BIN =  ravg avg_svc
GEN = avg_clnt.c avg_svc.c avg_xdr.c avg.h
RPCCOM = rpcgen
CFLAGS= -I/usr/include/tirpc

all: $(BIN)
	
ravg: ravg.o avg_clnt.o avg_xdr.o
	$(CC) ${CFLAGS} -o $@ ravg.o avg_clnt.o avg_xdr.o -lnsl -ltirpc -lm

ravg.o: ravg.c avg.h
	$(CC) ${CFLAGS} -g ravg.c -c -lm

avg_svc: avg_proc.o avg_svc.o avg_xdr.o
	$(CC) ${CFLAGS} -o $@ avg_proc.o avg_svc.o avg_xdr.o -lnsl -ltirpc -lm

avg_proc.o: avg_proc.c avg.h
	$(CC) ${CFLAGS} -g avg_proc.c -c -lm

$(GEN): avg.x
	$(RPCCOM) avg.x

clean cleanup:
	rm -f $(GEN) *.o $(BIN)
