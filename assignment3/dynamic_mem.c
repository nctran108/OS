#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "dynamic_mem.h"

void *global_base = NULL;

struct block_meta *find_free_block(struct block_meta **last, size_t size)
{
    struct block_meta *current = global_base;
    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

struct block_meta *request_space(struct block_meta* last, size_t size){
    struct block_meta* block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    assert((void*)block == request);
    if (request == (void*) -1) {
        return NULL;
    }

    if (last) {
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = 0x12345678;
    return block;
}

struct block_meta *get_block_ptr(void *ptr) {
    return (struct block_meta *)ptr -1;
}

void *malloc(size_t size){
    struct block_meta *block;

    if (size <= 0) {
        return NULL;
    }

    if (!global_base) {
        block = request_space(NULL, size);
        if (!block) {
            return NULL;
        }
        global_base = block;
    } else {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if (!block) { // fail to find free block
            block = request_space(last, size);
            if (!block) {
                return NULL;
            }
        } else { // find free block
            block->free = 0;
            block->magic = 0x77777777;
        }
    }
    return block + 1;
}

void *realloc(void *ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }

    struct block_meta * block_ptr = get_block_ptr(ptr);
    if (block_ptr->size >= size) {
        return ptr;
    }
    
    void *new_ptr;
    new_ptr = malloc(size);
    if (!new_ptr) {
        return NULL;
    }
    memcpy(new_ptr, ptr, block_ptr->size);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t nelem, size_t elsize) {
    size_t size = nelem * elsize;
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void free(void *ptr){
    if (!ptr) {
        return;
    }

    struct  block_meta* block_ptr = get_block_ptr(ptr);
    assert(block_ptr->free == 0);
    assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->free = 1;
    block_ptr->magic = 0x55555555;
}