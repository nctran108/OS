#ifndef DYNAMIC_MEM_H
#define DYNAMIC_MEM_H

#include <string.h>
#include <sys/types.h>

struct block_meta {
    size_t size;
    struct block_meta *next;
    int free;
    int magic; //for debug
};

#define META_SIZE sizeof(struct  block_meta)

void *global_base = NULL;

struct block_meta *find_free_block(struct block_meta **last, size_t size);
struct block_meta *request_space(struct block_meta* last, size_t size);
void *malloc(size_t size);
void free(void *prt);

#endif