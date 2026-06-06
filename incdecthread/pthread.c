// Based on code by Sam Siewert
// Modified by Phil Orlando
#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

#define COUNT  1000
#define SCHED_POLICY

typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[2];
threadParams_t threadParams[2];

// added for scheduling 
struct sched_param fifo_param;
pthread_attr_t incattr;
pthread_attr_t decattr;


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
   
   pthread_attr_init(&incattr);
   pthread_attr_init(&decattr);
   
   pthread_attr_setinheritsched(&incattr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setinheritsched(&decattr, PTHREAD_EXPLICIT_SCHED);
   
   pthread_attr_setschedpolicy(&incattr, SCHED_POLICY);
   pthread_attr_setschedpolicy(&decattr, SCHED_POLICY);
   
   max_prio=sched_get_priority_max(SCHED_POLICY);// gets max priority available 
   fifo_param.sched_priority=max_prio;
   pthread_attr_setschedparam(&incattr, &fifo_param);
   
   
   threadParams[i].threadIdx=i;
   pthread_create(&threads[i],   // pointer to thread descriptor
                  &incattr,     // use default attributes
                  incThread, // thread function entry point add up incrementally 1000 times
                  (void *)&(threadParams[i]) // parameters to pass in
                 );
   i++; // increase i to 1 to switch threads 
   
   fifo_param.sched_priority=max_prio-1;
   pthread_attr_setschedparam(&decattr, &fifo_param);
   
   threadParams[i].threadIdx=i;
   pthread_create(&threads[i], &decattr, decThread, (void *)&(threadParams[i]));// subract incrementally 1000 times. 

   for(i=0; i<2; i++)
     pthread_join(threads[i], NULL);

   printf("TEST COMPLETE\n");
}
