main: processA.o processB.o processC.o sharedMemory.o
	gcc processA.o sharedMemory.o -o progA
	gcc processB.o sharedMemory.o -o progB
	gcc processC.o sharedMemory.o -o progC
process.o: processA.c processB.c processC.c sharedMemory.c
	gcc -c processA.c processB.c processC.c sharedMemory.c
sharedMemory.o: sharedMemory.c sharedMemory.h
	gcc -c sharedMemory.c

clean:
	rm *.o prog*