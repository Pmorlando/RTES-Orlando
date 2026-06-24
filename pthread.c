// Based on code by Sam Siewert
// Modified by Phil Orlando for quiz 2 and again modified for the midterm
// modeled after 


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

#define NUM_THREADS 3
#define SCHED_POLICY SCHED_FIFO;


typedef struct
{
    int threadIdx;
    int threadSum; // added struct for the sum for each thread
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];
pthread_attr_t fifo_sched_attr;
struct sched_param max_param, mid_param, low_param; 



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
   // set scheduler i think
   pthread_attr_init(&fifo_sched_attr);
   pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_FIFO);



   // thread 0 1-99
   max_param.sched_priority = 99;
   pthread_attr_setschedparam(&fifo_sched_attr, &max_param);



   pthread_create(&threads[0],   // pointer to thread descriptor
                      &fifo_sched_attr,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[0]) // parameters to pass in
                     );


   // thread 1 100-199
   mid_param.sched_priority = 50;
   pthread_attr_setschedparam(&fifo_sched_attr, &mid_param);


   pthread_create(&threads[1],   // pointer to thread descriptor
                      &fifo_sched_attr,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[1]) // parameters to pass in
                     );

   // thread 2 200-199
   low_param.sched_priority = 1;
   pthread_attr_setschedparam(&fifo_sched_attr, &low_param);


   pthread_create(&threads[2],   // pointer to thread descriptor
                      &fifo_sched_attr,    // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[2]) // parameters to pass in
                     );




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
