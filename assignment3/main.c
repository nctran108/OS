#include <stdio.h>
#include "dynamic_mem.h"

int main() {
    int *arr = malloc(sizeof(int) * 10);

    for(int i = 0; i < 10; i++){
        arr[i] = i;
    }

    for(int i = 0; i < 10; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    return 0;
}