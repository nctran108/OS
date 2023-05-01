#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include "dynamic_mem.h"

#define NUMPOINTERS 10
int *ptr[2*NUMPOINTERS];
void *heap_start;
void *heap_end;

void memoryLeak(struct block_meta *block){
    int totalSize = 0;
    while (block) {
        if (!block->free) {
            totalSize += block->size;
        }
        block = block->next;
    }
    printf("heap size: %ld\n", heap_end - heap_start);
    printf("allocated size: %d\n", totalSize);
    printf("memory leaks: %ld\n", heap_end - heap_start - totalSize);
}

void mallocPointers(size_t size){
    printf("Call malloc\n");
    int rand_size;
    for (int i = 0; i < 2*NUMPOINTERS; i++){
        rand_size = rand() % 21;
        ptr[i] = malloc(rand_size*size);
    }
}

void printMemorySize(struct block_meta *block) {
    while (block) {
        if (!block->free) {
            printf("block size: %ld\n", block->size);
        }
        block = block->next;
    }
}

void reallocPointers(size_t size){
    printf("Call Realloc\n");
    ptr[3] = realloc(ptr[3], size*2);
    ptr[5] = realloc(ptr[5], size*5);
    ptr[6] = realloc(ptr[6], size*6);
    ptr[9] = realloc(ptr[9], size);
    ptr[10] = realloc(ptr[10], size*5);
    ptr[11] = realloc(ptr[11], size*7);
    ptr[13] = realloc(ptr[13], size*12);
    ptr[15] = realloc(ptr[15], size*13);
    ptr[17] = realloc(ptr[17], size*15);
    ptr[18] = realloc(ptr[18], size*40);
}

void callocPointers(size_t elsize){
    printf("Call calloc\n");
    ptr[3] = calloc(1, elsize);
    ptr[6] = calloc(2, elsize);
    ptr[9] = calloc(4, elsize);
    ptr[10] = calloc(6, elsize);
    ptr[11] = calloc(8, elsize);
    ptr[12] = calloc(10, elsize);
    ptr[14] = calloc(12, elsize);
    ptr[15] = calloc(14, elsize);
    ptr[18] = calloc(16, elsize);
    ptr[19] = calloc(18, elsize);
}

void freePointers(){
    printf("Free Memories\n");
    free(ptr[3]);
    free(ptr[6]);
    free(ptr[9]);
    free(ptr[10]);
    free(ptr[11]);
    free(ptr[12]);
    free(ptr[14]);
    free(ptr[15]);
    free(ptr[18]);
    free(ptr[19]);
}

int main() {
    heap_start = sbrk(0);
    printf("start heap address %p\n", heap_start);

    mallocPointers(sizeof(int)); 
    struct block_meta *block = get_block_ptr(ptr[0]); 
    //printMemorySize(block);
    heap_end = sbrk(0);
    printf("end heap address after malloc %p\n", heap_end);
    memoryLeak(block);

    freePointers();
    //printMemorySize(block);
    heap_end = sbrk(0);
    printf("end heap address after free %p\n", heap_end);
    memoryLeak(block);

    callocPointers(sizeof(int));
    //printMemorySize(block);
    heap_end = sbrk(0);
    printf("end heap address after calloc %p\n", heap_end);
    memoryLeak(block);

    reallocPointers(sizeof(int));
    //printMemorySize(block);
    heap_end = sbrk(0);
    printf("end heap address after realloc %p\n", heap_end);
    memoryLeak(block);

    return 0;
}