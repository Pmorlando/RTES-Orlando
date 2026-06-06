// https://www.i-programmer.info/programming/cc/13002-applying-c-deadline-scheduling.html?start=1
// Code taken from Sam Siewert and modified by Phil Orlando

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <stdint.h>

#include <unistd.h>
#include <time.h>
#include <sched.h>

#define USE_DEADLINE //comment out to do the FIFO sked


struct sched_attr 
{
    uint32_t size;
    uint32_t sched_policy;
    uint64_t sched_flags;
    int32_t sched_nice;
    uint32_t sched_priority;
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) 
{
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

void *threadA(void *p) 
{
    struct timespec current;
    
#ifdef USE_DEADLINE // for using deadline will trigger this
    
    struct sched_attr attr = 
    {
        .size = sizeof (attr),
        .sched_policy = SCHED_DEADLINE,
        .sched_runtime = 10 * 1000 * 1000,
        .sched_period = 2 * 1000 * 1000 * 1000,
        .sched_deadline = 11 * 1000 * 1000
    };

    sched_setattr(0, &attr, 0);
#else 
    // FIFO 
    struct sched_param fifo_param;
    fifo_param.sched_priority=sched_get_priority_max(SCHED_FIFO);
    
    
    for (;;) 
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &current);//clock wont be affected by outside changes
        printf("Time is %ld sec and %ld nsec\n",current.sec, current.nsec);
        
        fflush(0);
        sched_yield();
    };
}


int main(int argc, char** argv) 
{
    pthread_t pthreadA;
    pthread_create(&pthreadA, NULL, threadA, NULL);
    pthread_exit(0);
    return (EXIT_SUCCESS);
}
