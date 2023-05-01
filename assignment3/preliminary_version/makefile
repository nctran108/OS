main: main.o dynamic_mem.o
	gcc main.o dynamic_mem.o -o main
main.o: main.c dynamic_mem.c
	gcc -c main.c dynamic_mem.c
dynamicMem.o: dynamic_mem.c dynamic_mem.h
	gcc -c dynamic_mem.c

clean:
	rm *.o main