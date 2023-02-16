main: regular_forks.o nested_forks.o sharedMemory.o
	gcc regular_forks.o sharedMemory.o -o regular
	gcc nested_forks.o sharedMemory.o -o nested
forks.o: regular_forks.c nested_forks.c sharedMemory.c
	gcc -c regular_forks.c nested_forks.c sharedMemory.c
sharedMemory.o: sharedMemory.c sharedMemory.h
	gcc -c sharedMemory.c

clean:
	rm *.o regular nested