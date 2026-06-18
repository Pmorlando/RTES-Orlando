// Based on code by Sam Siewert
// Modified by Phil Orlando for quiz 2

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

#define NUM_THREADS 3

typedef struct
{
    int threadIdx;
    int threadSum; // added struct for the sum for each thread
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];


void *counterThread(void *threadp)
{
    int sum=0, i, j;
    threadParams_t *threadParams = (threadParams_t *)threadp;
    //int startsum = threadParams->threadIdx * 100 + 1;// each start point for 1,100,200
    // not correct so switching to if loop because starts were 1 101 and 201
    int startsum = 0;
    int endsum = 0;
    if(threadParams->threadIdx==0)
    {
      startsum = 1;
      endsum =99;
    }
    else
    {
      startsum = threadParams->threadIdx * 100;
      endsum = startsum + 99; // needed to switch to this so the end of sum was correct
    }
    
    //int endsum = startsum + 99;// each end so 99, 199, 299
    
    for(i=startsum; i<endsum+1;i++)
    {
        sum = sum+i;// each sum
        printf("i is %d\n",i);
    }   
    threadParams->threadSum = sum;//assign each sum value to the thread struct
    
    printf("Thread idx=%d, sum=%d\n", //adjusted printout to remove sum range
           threadParams->threadIdx, sum);
}


int main (int argc, char *argv[])
{
   int rc;
   int i;

   for(i=0; i < NUM_THREADS; i++)
   {
       threadParams[i].threadIdx=i;

       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );

   }

   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);
        
   int totalsum = 0;// struggling with naming the variables for each sum to be able to sum 
   for(i=0; i< NUM_THREADS;i++)
   {
     totalsum = totalsum + threadParams[i].threadSum;
   }
   
   printf("Total sum =%d\n",// total sum output instead of test complete 
          totalsum);
}
