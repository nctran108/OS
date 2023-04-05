#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include "sharedMemory.h"

#define SIZE 20
#define PID (int) getpid()

typedef struct{
    int userId;
    int jobSize;
} print_job; 

typedef struct{
    print_job jobs[SIZE];
    int nextin;
    int nextout;
    int buffer_index;
} buffer_t;

buffer_t *queue;

sem_t *buffer_mutex; 
sem_t *full_sem; 
sem_t *empty_sem;

int shmid, shmid2;
double *average_wating;

void insertqueue(print_job job) {
    if (queue->buffer_index < SIZE) {
        queue->jobs[queue->buffer_index++] = job;
    } else {
        printf("Buffer overflow\n");
    }
}
 
print_job dequeuebuffer() {
    print_job empty_job = {0,0};
    if (queue->buffer_index > 0) {
        return queue->jobs[--queue->buffer_index];
    } else {
        printf("Buffer underflow\n");
    }
    return empty_job;
}

void producer_process(){
    time_t begin, end;
    int num_jobs = rand() % SIZE + 1; 
    int job_size;
    int i=0;
    double temp;
    time(&begin);
    while (i++ < num_jobs) {
        random_delay();
        job_size = rand() % 901 + 100; // range between 100 and 1000 bytes

        print_job job = {PID, job_size};

        sem_wait(full_sem); // sem=0: wait. sem>0: go and decrement it
        /* possible race condition here. After this thread wakes up,
           another thread could aqcuire mutex before this one, and add to list.
           Then the list would be full again
           and when this thread tried to insert to buffer there would be
           a buffer overflow error */
        //sem_wait(&lock);
        sem_wait(buffer_mutex); // protecting critical section
        //printf("enqueue buffer\n");
        insertqueue(job);
        sem_post(buffer_mutex);
        //sem_post(&lock);
        sem_post(empty_sem); // post (increment) emptybuffer semaphore
        printf("Producer %d from [parent] %d added %d to buffer\n", PID, getppid(), job_size);
    }
    time(&end);
    
    *average_wating += (double) (end - begin) / num_jobs;
}
 
void *consumer(void *thread_n){
    int thread_numb = *(int *)thread_n;
    print_job job;
    while (1) {
        sem_wait(empty_sem);
        //sem_wait(&lock);
        // there could be race condition here, that could cause
        //   buffer underflow error 
        sem_wait(buffer_mutex);
        job = dequeuebuffer();
        //printf("dequeue value\n");
        sem_post(buffer_mutex);
        //sem_post(&lock);
        sem_post(full_sem); // post (increment) fullbuffer semaphore
        printf("Consumer %d in %d dequeue <%d, %d> from buffer\n", gettid(), PID, job.userId, job.jobSize);
    }
    pthread_exit(0);
}

void semaphore_init(){
    full_sem = sem_open("nctran_full",O_CREAT,0666,SIZE); 
    empty_sem = sem_open("nctran_empty", O_CREAT,0666,0);
    buffer_mutex = sem_open("nctran_mutex",O_CREAT,0666,1);
}

void semaphore_free(){
    sem_close(empty_sem);
    sem_close(full_sem);
    sem_close(buffer_mutex);

    sem_unlink("nctran_full");
    sem_unlink("nctran_empty");
    sem_unlink("nctran_mutex");
}

void random_delay(){
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = (rand() % 901 + 100) * 1000000;
    nanosleep(&ts, NULL);
}

void my_handler(int dummy){
    printf("Detected CTR-C signal no. %d \n", dummy);

    if (!detach_memory(queue)) printf("cannot detach queue.\n");
    if (!detach_memory(average_wating)) printf("cannot detach average time.\n");
    destroy_memory(shmid);
    destroy_memory(shmid2);
    semaphore_free();
    exit(0);
}

int main(int argc, char **argv) {
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);

    time_t begin, end;
    time(&begin);
    
    semaphore_init();
    shmid = get_id("nctran_queue", sizeof(buffer_t));
    shmid2 = get_id_with_key(DEFAULT_KEY, sizeof(double));

    queue = attach_memory(shmid);
    average_wating = attach_memory(shmid2);

    if (!queue || !average_wating){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    queue->nextin = 0;
    queue->nextout = 0;
    queue->buffer_index = 0;

    signal(SIGINT, my_handler);

    for (int i = 0; i < num_producers; i++) {
        pid_t pid = fork();
        if (pid == 0){
            srand(PID + i);
            producer_process();
            exit(0);
        }
    }

    pthread_t consumerThread[num_consumers];
    int thread_numb[num_consumers];

    for(int i = 0; i < num_consumers; i++){
        // playing a bit with thread and thread_numb pointers...
        
        thread_numb[i] = i;
        pthread_create(&consumerThread[i], // pthread_t *t
                    NULL, // const pthread_attr_t *attr
                    consumer, // void *(*start_routine) (void *)
                    &thread_numb[i]);  // void *arg
        
    } 

    for (int i = 0; i < num_producers; i++) {
        wait(NULL);
    }

    for (int i = 0; i < num_consumers; i++)
        pthread_cancel(consumerThread[i]);

    time(&end);

    time_t elapsed = end - begin;
    printf("Execution time: %ld seconds.\n", elapsed);    

    printf("Average waiting time: %lf seconds.\n", *average_wating);

    if (!detach_memory(queue)) printf("cannot detach queue.\n");
    if (!destroy_memory(shmid)) printf("cannot destroy shared memories %d.\n", shmid);
    if (!detach_memory(average_wating)) printf("cannot detach average time.\n");
    if (!destroy_memory(shmid2)) printf("cannot destroy shared memories %d.\n", shmid2);

    semaphore_free();
    return 0;
}