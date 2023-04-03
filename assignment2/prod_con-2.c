#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "sharedMemory.h"
 
#define SIZE 20
#define NUMB_THREADS 3
#define NUMB_PROS 3
#define PRODUCER_LOOPS 1
#define PID (int) getpid()
 
typedef struct buffer_t{
    int *buff;
    int nextin;
    int nextout;
    int buffer_index;
};

buffer_t buffer;

typedef struct producer
{
    int userId;
    int jobSize;
};

sem_t *buffer_mutex; 
sem_t *full_sem;  /* when 0, buffer is full */
sem_t *empty_sem; /* when 0, buffer is empty. Kind of
                like an index for the buffer */
/* initially buffer will be empty.  full_sem
will be initialized to buffer SIZE, which means
SIZE number of producer threads can write to it.
And empty_sem will be initialized to 0, so no
consumer can read from buffer until a producer
thread posts to empty_sem */


/* 
void insertbuffer(int *buffer, int *buffer_index, buffer_t value) {
    if (*buffer_index < SIZE) {
        buffer[*buffer_index++] = value;
    } else {
        printf("Buffer overflow\n");
    }
}
 
buffer_t dequeuebuffer(int *buffer, int buffer_index) {
    if (buffer_index > 0) {
        return buffer[--buffer_index]; // buffer_index-- would be error!
    } else {
        printf("Buffer underflow\n");
    }
    return 0;
}
*/ 

void producer_process() {
    int value;
    int i=0;
   // while (i++ < PRODUCER_LOOPS) {
        sleep(rand() % 10);
        value = rand() % 100;
        sem_wait(&buffer->full_sem); // sem=0: wait. sem>0: go and decrement it
        /* possible race condition here. After this thread wakes up,
           another thread could aqcuire mutex before this one, and add to list.
           Then the list would be full again
           and when this thread tried to insert to buffer there would be
           a buffer overflow error */
        //sem_wait(&lock);
        sem_wait(&buffer->buffer_mutex); // protecting critical section
        printf("enqueue buffer\n");
        //insertbuffer(shm_buffer,shm_index,value);
        sem_post(&buffer->buffer_mutex);
        //sem_post(&lock);
        sem_post(&buffer->empty_sem); // post (increment) emptybuffer semaphore
        printf("Producer %d from [parent] %d added %d to buffer\n", PID, getppid(), value);
   // }
}
 
void *consumer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    
    int value = 0;
    int i=0;
    //while (i++ < PRODUCER_LOOPS) {
        sem_wait(&buffer->empty_sem);
        //sem_wait(&lock);
        // there could be race condition here, that could cause
        //   buffer underflow error 
        sem_wait(&buffer->buffer_mutex);
        //value = dequeuebuffer(shm_buffer,*shm_index);
        printf("dequeue value\n");
        sem_post(&buffer->buffer_mutex);
        //sem_post(&lock);
        sem_post(&buffer->full_sem); // post (increment) fullbuffer semaphore
        printf("Consumer %d in %d dequeue %d from buffer\n", thread_numb, PID, value);
    //}
    pthread_exit(0);
}

int main(int argc, int **argv) {
    //buffer_index = 0;
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int shmid = get_id_with_key(DEFAULT_KEY, sizeof(buffer_t));

    if (shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    buffer = attach_memory(shmid);
    if (!buffer){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    full_sem = sem_open("full", // sem_t *sem
             O_CREAT,
             0666,
             SIZE); // unsigned int value. Initial value
    empty_sem = sem_open("empty",
             O_CREAT,
             0666,
             0);

    buffer_mutex = sem_init("mutex",
             O_CREAT,
             0666,
             1);

    buffer.buff = malloc(sizeof(int) * SIZE);

    buffer.nextin = buffer->nextout = 0;
    buffer.buffer_index = 0;



    /* full_sem is initialized to buffer size because SIZE number of
       producers can add one element to buffer each. They will wait
       semaphore each time, which will decrement semaphore value.
       empty_sem is initialized to 0, because buffer starts empty and
       consumer cannot take any element from it. They will have to wait
       until producer posts to that semaphore (increments semaphore
       value) */
    
    pid_t = pid;
    for (int i = 0; i < NUMB_PROS; i++) {
        pid = fork();
        if (pid == 0){
            producer_process();
            exit(0);
        }
    }

    pthread_t consumerThread[NUMB_THREADS];
    int thread_numb[NUMB_THREADS];

    for(int i = 0; i < NUMB_THREADS; i++){
        // playing a bit with thread and thread_numb pointers...
        thread_numb[i] = i;
        pthread_create(&consumerThread[i], // pthread_t *t
                    NULL, // const pthread_attr_t *attr
                    consumer, // void *(*start_routine) (void *)
                    &thread_numb[i]);  // void *arg
    }

    for (int i = 0; i < NUMB_PROS; i++) {
        wait(NULL);
    }
        
    for (int i = 0; i < NUMB_THREADS; i++)
        pthread_join(consumerThread[i], NULL);
    
    printf("here\n");
    sem_destroy(&buffer->full_sem);
    sem_destroy(&buffer->empty_sem);
    sem_destroy(&buffer->buffer_mutex);

    free(buffer->buff);

    if (!detach_memory(buffer)) printf("cannot detach shared memory %d.\n", shmid);

    if (!destroy_memory(shmid)) printf("cannot destroy shared memory %d.\n", shmid);
    return 0;
}