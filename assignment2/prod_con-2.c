#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "sharedMemory.h"
 
#define SIZE 5
#define NUMB_THREADS 3
#define NUMB_PROS 3
#define PRODUCER_LOOPS 1
 
typedef int buffer_t;
//buffer_t buffer[SIZE];
//int buffer_index;
 
pthread_mutex_t buffer_mutex;
/* initially buffer will be empty.  full_sem
   will be initialized to buffer SIZE, which means
   SIZE number of producer threads can write to it.
   And empty_sem will be initialized to 0, so no
   consumer can read from buffer until a producer
   thread posts to empty_sem */
sem_t full_sem;  /* when 0, buffer is full */
sem_t empty_sem; /* when 0, buffer is empty. Kind of
                    like an index for the buffer */
sem_t lock;
 
 
 
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
 
 
void producer(int *shm_buffer, int *shm_index, void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    while (i++ < PRODUCER_LOOPS) {
        sleep(rand() % 10);
        value = rand() % 100;
        sem_wait(&full_sem); // sem=0: wait. sem>0: go and decrement it
        /* possible race condition here. After this thread wakes up,
           another thread could aqcuire mutex before this one, and add to list.
           Then the list would be full again
           and when this thread tried to insert to buffer there would be
           a buffer overflow error */
        sem_wait(&lock);
        //pthread_mutex_lock(&buffer_mutex); /* protecting critical section */
        insertbuffer(shm_buffer,shm_index,value);
        //pthread_mutex_unlock(&buffer_mutex);
        sem_post(&lock);
        sem_post(&empty_sem); // post (increment) emptybuffer semaphore
        printf("Producer %d and %d from [parent] %d added %d to buffer\n", thread_numb, getpid(), getppid(), value);
    }
    pthread_exit(0);
}
 
void *consumer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    int shmid_index = get_id_with_key(DEFAULT_KEY, sizeof(int));
    int shmid_buffer = get_id_with_key(DEFAULT_KEY + 1, sizeof(int) * SIZE);

    int *shm_index = attach_memory(shmid_index);
    int *shm_buffer = attach_memory(shmid_buffer);
    if (!shm_index || !shm_buffer){
        perror("shmat");
        exit(1);
    }
    buffer_t value;
    int i=0;
    while (i++ < PRODUCER_LOOPS) {
        sem_wait(&empty_sem);
        sem_wait(&lock);
        /* there could be race condition here, that could cause
           buffer underflow error */
        //pthread_mutex_lock(&buffer_mutex);
        value = dequeuebuffer(shm_buffer,*shm_index);
        //pthread_mutex_unlock(&buffer_mutex);
        sem_post(&lock);
        sem_post(&full_sem); // post (increment) fullbuffer semaphore
        printf("Consumer %d in %d dequeue %d from buffer\n", thread_numb, getpid(), value);
    }
    if (!detach_memory(shm_index)) printf("cannot detach shared memory %d.\n", shmid_index);
    if (!detach_memory(shm_buffer)) printf("cannot detach shared memory %d.\n", shmid_buffer);
    pthread_exit(0);
}
 
int main(int argc, int **argv) {
    //buffer_index = 0;
 
    pthread_mutex_init(&buffer_mutex, NULL);
    sem_init(&full_sem, // sem_t *sem
             0, // int pshared. 0 = shared between threads of process,  1 = shared between processes
             SIZE); // unsigned int value. Initial value
    sem_init(&empty_sem,
             0,
             0);
    sem_init(&lock,0,1);

    int shmid_index;
    int shmid_buffer;
    int *shm_index;
    buffer_t *shm_buffer;
    /* full_sem is initialized to buffer size because SIZE number of
       producers can add one element to buffer each. They will wait
       semaphore each time, which will decrement semaphore value.
       empty_sem is initialized to 0, because buffer starts empty and
       consumer cannot take any element from it. They will have to wait
       until producer posts to that semaphore (increments semaphore
       value) */
    pthread_t thread[NUMB_THREADS];
    int thread_numb[NUMB_THREADS];
    int thread_pros[NUMB_PROS];
    
    for (int i = 0; i < NUMB_PROS; i++) {
        shmid_index = get_id_with_key(DEFAULT_KEY, sizeof(int));
        shmid_buffer = get_id_with_key(DEFAULT_KEY + 1, sizeof(int) * SIZE);

        shm_index = attach_memory(shmid_index);
        shm_buffer = attach_memory(shmid_buffer);
        if (!shm_index || !shm_buffer){
            perror("shmat");
            exit(1);
        }
        //PID = fork();
        thread_pros[i] = i;
        *shm_index = 0;
        if (fork() == 0){
            //pthread_create(thread + i,
            //            NULL,
            //            producer,
            //            ,shm_buffer,shm_index,thread_pros + i);
            producer(shm_buffer,shm_index,thread_pros + i);
            if (!detach_memory(shm_index)) printf("cannot detach shared memory %d.\n", shmid_index);
            if (!detach_memory(shm_buffer)) printf("cannot detach shared memory %d.\n", shmid_buffer);
        }
    }
    for(int j = 0; j < NUMB_THREADS; j++){

        thread_numb[j] = j;
        // playing a bit with thread and thread_numb pointers...
        pthread_create(&thread[j], // pthread_t *t
                    NULL, // const pthread_attr_t *attr
                    consumer, // void *(*start_routine) (void *)
                    &thread_numb[j]);  // void *arg
    }

    for (int j = 0; j < NUMB_THREADS; j++)
        pthread_join(thread[j], NULL);
 
    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
    if (!destroy_memory(shmid_index)) printf("cannot destroy shared memory %d.\n", shmid_index);
    if (!destroy_memory(shmid_buffer)) printf("cannot destroy shared memory %d.\n", shmid_buffer);
 
    return 0;
}