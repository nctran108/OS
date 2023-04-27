#include <stdio.h>
#include <unistd.h>
#include "dynamic_mem.h"

#define NUMPOINTERS 10
int *ptr[NUMPOINTERS];
int *ptr2[NUMPOINTERS];

void printHeapAddr(void *heap_start){
    void *heap_end;
    printf("heap start address: %p\n", heap_start);
    heap_end = sbrk(0);
    printf("heap end address %p\n", heap_end);
}

void mallocPointers(size_t size){
    void *heap_start;
    for (int i = 0; i < NUMPOINTERS; i++){
        heap_start = sbrk(0);
        ptr[i] = malloc(size);
        printHeapAddr(ptr[i]);
    }
}

void reallocPointers(size_t size){
    printf("Call Realloc\n");
    printf("heap start %p\n", ptr[0]);
    for (int i = 0; i < NUMPOINTERS; i++){
        ptr[i] = realloc(ptr[i],size);
    }
    void *heap_end;
    heap_end = sbrk(0);
    printf("heap end address %p\n", heap_end);
}

void callocPointers(size_t nelem, size_t elsize){
    printf("call calloc\n");
    for (int i = 0; i < NUMPOINTERS; i++){
        ptr2[i] = calloc(nelem,elsize);
        printHeapAddr(ptr2[i]);
    }
}

void freePointers(){
    for (int i = 0; i < NUMPOINTERS; i++){
        free(ptr[i]);
    }
}

int main() {
    printf("%p\n",ptr[0]);
    mallocPointers(sizeof(int) * 10);
    reallocPointers(sizeof(int) * 20);
    struct block_meta *block = get_block_ptr(ptr[1]);
    printf("block %p\n", block);
    printf("size %ld\n", block->size);
    callocPointers(10,sizeof(int));
    freePointers();
    return 0;
}