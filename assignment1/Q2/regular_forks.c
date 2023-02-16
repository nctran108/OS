#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "sharedMemory.h"

#define AID 10
#define BID 20
#define CID 30
#define PROCESSA "I am Process A"
#define PROCESSB "I am Process B"
#define PROCESSC "I am Process C"

int main(){
    int pid;
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
    strncpy(shm_char,PROCESSA, sizeof(PROCESSA));
    printf("%s\n", shm_char);

    *shm_int = AID;

    pid = fork();

    if(pid == 0){
        // child process
        while (*shm_int !=10) sleep(1);
        strncpy(shm_char,PROCESSB, sizeof(PROCESSB));
        usleep(1);
        *shm_int = BID;
    }
    else {
        // main process
        pid = fork();
        if (pid == 0) {
            // another child process
            while (*shm_int !=20) sleep(1);
            strncpy(shm_char,PROCESSC, sizeof(PROCESSC));
            usleep(1);
            *shm_int = CID;
        }
        else {
            // still in main process
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
        }
    }

    return 0;
}