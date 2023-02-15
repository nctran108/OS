#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "sharedMemory.h"

#define AID 10
#define PROCESS "I am Process A"

int main()
{
    char c;
    int shmid[2];
    int size[2] = {sizeof(char), sizeof(int)};
    char *shm_char;
    int *shm_int;

    key_t key = DEFAULT_KEY;

    for(int i = 0; i < 2; i++){
        shmid[i] = get_id_with_key(key+i, size[i]);
    }

    shm_char = attach_memory(shmid[0]);
    shm_int = attach_memory(shmid[1]);
    if(!shm_char || !shm_int)
    {
        perror("shmat");
        exit(1);
    }
    strncpy(shm_char,PROCESS, sizeof(PROCESS));
    printf("%s\n", shm_char);

    *shm_int = AID;

    while( *shm_int != 20 ) sleep(1);
    printf("%s\n", shm_char);

    while( *shm_int != 30 ) sleep(1);
    printf("%s\n", shm_char);
        

    if (!detach_memory(shm_char)) printf("cannot detatch share memory %d.\n", shmid[0]);
    if (!detach_memory(shm_int)) printf("cannot detatch share memory %d.\n", shmid[1]);

    for(int i = 0; i < 2; i++){
        if (!destroy_memory(shmid[i])) printf("cannot destroy share memory %d.\n", i);
    }

    printf("GoodBye\n");
    return 0;
}
