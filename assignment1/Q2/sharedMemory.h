#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H


#include <stdbool.h>
#include <sys/ipc.h>

#define BLOCK_SIZE 4096
#define DEFAULT_KEY 12345

int get_id(char *filename, int size);
int get_id_with_key(key_t key, int size);
void * attach_memory(int shmid);
bool detach_memory(void *block);
bool destroy_memory(int shmid);


#endif

