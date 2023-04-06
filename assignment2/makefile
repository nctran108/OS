main: lifo.o fifo.o sharedMemory.o
	gcc lifo.o sharedMemory.o -o progLIFO
	gcc fifo.o sharedMemory.o -o progFIFO
lifo.o: lifo.c sharedMemory.c
	gcc -c lifo.c sharedMemory.c
fifo.o: fifo.c sharedMemory.c
	gcc -c fifo.c sharedMemory.c
sharedMemory.o: sharedMemory.c sharedMemory.h
	gcc -c sharedMemory.c

clean:
	rm *.o prog*