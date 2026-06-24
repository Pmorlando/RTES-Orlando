// Written by Phil Orlando for Exercise 3 of RTES
// compiled using gcc MUTEXprob5.c -o MUTEXprob5 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>


typedef struct {
  double latitude, longitude, altitude;
  struct timespec logtime; // defined timespec struct 
} Position;

Position position;
pthread_mutex_t mutexsem = PTHREAD_MUTEX_INITIALIZER;

void *writepos(void *arg)
{
  Position *pos = (Position *)arg;
  struct timespec startwrite, writeagain;
  double lat, lon, alt;

  clock_gettime(CLOCK_MONOTONIC, &startwrite); //initial time start for writing thread
  writeagain = startwrite;

  while(1) 
  {
    struct timespec curr_time, timeleft;
    clock_gettime(CLOCK_MONOTONIC, &curr_time); //getting current time of loop
    timeleft.tv_sec = curr_time.tv_sec - startwrite.tv_sec;
    if(timeleft.tv_sec>= 180) break; //180 sec run time 
    
    lat = 1.5*timeleft.tv_sec + 15;//linear operations to the data
    lon = 2*timeleft.tv_sec +4;
    alt = 3*timeleft.tv_sec+5000;
    
    pthread_mutex_lock(&mutexsem);//lock mutex semaphore
    pos->latitude = lat;//reassign position
    pos->longitude = lon;
    pos->altitude = alt;
    clock_gettime(CLOCK_MONOTONIC, &pos->logtime);//assign time to the data logged 
    printf("Positon logged at time %ld sec, %ld nsec is Lat: %.2f, Long: %.2f, Altitude: %.2f.\n", 
            pos->logtime.tv_sec, 
            pos->logtime.tv_nsec,
            pos->latitude, 
            pos->longitude, 
            pos->altitude); // print out what is written to global data 
    
    

    writeagain.tv_sec += 15; // pause with mutex locked to trigger read timeout
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &writeagain, NULL);
    pthread_mutex_unlock(&mutexsem); // unlock mutex
    
    writeagain.tv_sec += 5; // pause to let read see changes
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &writeagain, NULL);
  }
  return NULL;

}

void *readpos(void *arg)
{
  Position *pos = (Position *)arg;
  struct timespec startread, readagain;

  clock_gettime(CLOCK_MONOTONIC, &startread);// read thread start time
  readagain = startread;

  while(1) 
  {
     
    struct timespec curr_time;
    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    if((curr_time.tv_sec - startread.tv_sec) >= 180) break; //180 sec run time
    
    struct timespec timerwait;
    timerwait.tv_sec = curr_time.tv_sec + 10;

    int noget = pthread_mutex_timedlock(&mutexsem,&timerwait);//try to access mutex for 10 sec
    
    if(noget == 0)
    {
      Position localcopy = *pos;//make local copy of position struct to read from
      // would ideally unlock but putting timestamp and print inbetween mutex lock
    
      clock_gettime(CLOCK_MONOTONIC, &curr_time);
      printf("Position read at time %ld sec, %ld nsec is Lat: %.2f, Long: %.2f, Altitude: %.2f logged at %ld sec, %ld nsec.\n", 
              curr_time.tv_sec,
              curr_time.tv_nsec, 
              localcopy.latitude,
              localcopy.longitude, 
              localcopy.altitude,
              localcopy.logtime.tv_sec,
              localcopy.logtime.tv_nsec);
      pthread_mutex_unlock(&mutexsem); // unlock the mutex
    
      readagain.tv_sec += 1;// sleep for 1 sec to make more constant checking
      clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &readagain, NULL);
    }
    else if(noget == ETIMEDOUT)
    {
      struct timespec timeout;
      clock_gettime(CLOCK_MONOTONIC, &timeout); 
      printf("No new data available %ld sec %ld nsec\n",
              timeout.tv_sec,
              timeout.tv_nsec);
    }
    
  }

  return NULL;
}


int main( int argc, char** argv )
{
  pthread_t writethread, readthread;// make thread names 

  pthread_create(&writethread, NULL, writepos, &position);//create each thread and tell it what function to run and the position struct
  pthread_create(&readthread, NULL, readpos, &position);

  pthread_join(writethread, NULL);// join threads once complete
  pthread_join(readthread, NULL);
  return 0;
}
