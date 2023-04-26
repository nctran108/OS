#include <stdio.h>
#include <unistd.h>
#include "dynamic_mem.h"

#define NUMPOINTERS 10
int *ptr[NUMPOINTERS];

void printHeapAddr(void *ptr){
    void *heap_end;
    printf("heap start address: %p\n", ptr);
    heap_end = sbrk(0);
    printf("heap end address %p\n", heap_end);
}

void mallocPointers(size_t size){
    for (int i = 0; i < NUMPOINTERS; i++){
        ptr[i] = malloc(size);
        printHeapAddr(ptr[i]);
    }
}

void reallocPointers(size_t size){
    printf("Call Realloc\n");
    for (int i = 0; i < NUMPOINTERS; i++){
        ptr[i] = realloc(ptr[i],size);
    }
    void *heap_end;
    heap_end = sbrk(0);
    printf("heap end address %p\n", heap_end);
}

void freePointers(){
    for (int i = 0; i < NUMPOINTERS; i++){
        free(ptr[i]);
    }
}

int main() {
    mallocPointers(sizeof(int) * 10);
    reallocPointers(20);
    freePointers();
    return 0;
}