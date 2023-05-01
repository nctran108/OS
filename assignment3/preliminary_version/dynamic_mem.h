#ifndef DYNAMIC_MEM_H
#define DYNAMIC_MEM_H

#include <sys/types.h>

struct block_meta {
    size_t size;
    struct block_meta *next;
    int free;
    int magic; //for debug
};

#define META_SIZE sizeof(struct block_meta)

struct block_meta *find_free_block(struct block_meta **last, size_t size);
struct block_meta *request_space(struct block_meta* last, size_t size);
struct block_meta *get_block_ptr(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nelem, size_t elsize);
void free(void *ptr);

#endif