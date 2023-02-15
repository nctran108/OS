#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sharedMemory.h"

#define CID 30
#define PROCESS "I am Process C"

int main()
{
    int shmid[2];
    int size[2] = {sizeof(char), sizeof(int)};
    char *shm;
    int *shm2;

    key_t key = DEFAULT_KEY;

    for(int i = 0; i < 2; i++){
        shmid[i] = get_id_with_key(key+i, size[i]);
        if(shmid[i] < 0){
            perror("shmget");
            exit(1);
        }
    }

    shm = attach_memory(shmid[0]);
    shm2 = attach_memory(shmid[1]);
    if(!shm || !shm2)
    {
        perror("shmat");
        exit(1);
    }

    while (*shm2 != 20) sleep(1);

    strncpy(shm,PROCESS, sizeof(PROCESS));
    usleep(1);
    *shm2 = CID;
    
    return 0;
}
