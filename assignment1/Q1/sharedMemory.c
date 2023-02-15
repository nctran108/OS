#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sharedMemory.h"

#define ERROR (-1)

int get_id(char *filename, int size){
    key_t key;

    if(!filename){
        key = DEFAULT_KEY;
    }
    else{
        key = ftok(filename, 0);
    }

    return shmget(key, size, IPC_CREAT | 0666);
}

int get_id_with_key(key_t key, int size){
    return shmget(key, size, IPC_CREAT | 0666);
}

void * attach_memory(int shmid){
    void *result = shmat(shmid, NULL, 0);
    if((char *) result == (char *) ERROR) return NULL;
    return result;
}

bool detach_memory(void *block){
    return (shmdt(block) != ERROR);
}

bool destroy_memory(int shmid){
    return (shmctl(shmid, IPC_RMID, NULL) != ERROR);
}