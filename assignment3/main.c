#include <stdio.h>
#include <unistd.h>
#include "dynamic_mem.h"

int main() {   
    void *heap_start;
    void *heap_end;
    int *arr = malloc(sizeof(int) * 10);

    struct block_meta *ptr = get_block_ptr(arr);   
    heap_end =sbrk(0);
    brk(heap_start);
    printf("heap start address: %p\n", heap_start);
    printf("heap end address %p\n", heap_end);

    printf("size = %ld\n", ptr->size);
    if (ptr->free == 0)
        printf("not free\n");

    for(int i = 0; i < 10; i++){
        arr[i] = i;
    }

    for(int i = 0; i < 10; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    if (ptr->free == 1)
        printf("freed\n");
    return 0;
}