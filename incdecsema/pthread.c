// Based on code by Sam Siewert
// Modified by Phil Orlando
#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <semaphore.h>

#define COUNT  1000
#define SCHED_POLICY SCHED_FIFO

typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[2];
threadParams_t threadParams[2];

// added for sema 
sem_t sem;



// Unsafe global
int gsum=0;


void *incThread(void *threadp) //function for couting up with thread 0
{
    int i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for(i=0; i<COUNT; i++)
    {
        gsum=gsum+i;
        printf("Increment thread idx=%d, gsum=%d\n", threadParams->threadIdx, gsum);
    }
}


void *decThread(void *threadp)// function for counting down for thread 1
{
    int i;
    threadParams_t *threadParams = (threadParams_t *)threadp;
    
    sem_wait(&sem);

    for(i=0; i<COUNT; i++)
    {
        gsum=gsum-i;
        printf("Decrement thread idx=%d, gsum=%d\n", threadParams->threadIdx, gsum);
    }
}




int main (int argc, char *argv[])
{
   int rc, max_prio;
   int i=0; 
   
   sem_init(&sem, 0,1); //once dec made it will wait
   
   threadParams[i].threadIdx=i;
   pthread_create(&threads[i],   // pointer to thread descriptor
                  (void *)0,     // use default attributes
                  incThread, // thread function entry point add up incrementally 1000 times
                  (void *)&(threadParams[i]) // parameters to pass in
                 );
   i++; // increase i to 1 to switch threads 
   
   
   threadParams[i].threadIdx=i;
   pthread_create(&threads[i], (void *)0, decThread, (void *)&(threadParams[i]));// subract incrementally 1000 times. 

   pthread_join(threads[0], NULL);// wait for inc to be done
   sem_post(&sem);
   
   pthread_join(threads[1], NULL);// join when dec done
   
   sem_destroy(&sem);

   printf("TEST COMPLETE\n");
}
