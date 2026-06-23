// Written by Phil Orlando for Exercise 3 of RTES
// compiled using ADD LINUX COMMAND HERE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef struct {
  double latitude, longitude, altitude;
  struct timespec logtime;
} Position;

Position position;
pthread_mutex_t mutexsem = PTHREAD_MUTEX_INITIALIZER;

void *writepos(void *arg)
{
  Position *pos = (Position *)arg;
  struct timespec startwrite, writeagain;
  double lat, lon, alt;

  clock_gettime(CLOCK_MONOTONIC, &startwrite);
  writeagain = startwrite;

  while(1) 
  {
    struct timespec curr_time, timeleft;
    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    timeleft.tv_sec = curr_time.tv_sec - startwrite.tv_sec;
    if(timeleft.tv_sec>= 180) break;
    
    lat = 1.5*timeleft.tv_sec + 15;
    lon = 2*timeleft.tv_sec +4;
    alt = 100*timeleft.tv_sec +5000;
    
    pthread_mutex_lock(&mutexsem);
    pos->latitude = lat;
    pos->longitude = lon;
    pos->altitude = alt;
    clock_gettime(CLOCK_MONOTONIC, &pos->logtime);
    pthread_mutex_unlock(&mutexsem);

    writeagain.tv_sec += 1;
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &writeagain, NULL);
  }
  return NULL;

}

void *readpos(void *arg)
{
  Position *pos = (Position *)arg;
  struct timespec startread, readagain;

  clock_gettime(CLOCK_MONOTONIC, &startwrite);
  readagain = startread;

  while(1) 
  {
     
    struct timespec curr_time;
    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    if((curr_time.tv_sec - startread.tv_sec) >= 180) break;

    pthread_mutex_lock(&mutexsem);
    Position localcopy = *pos;
    pthread_mutex_unlock(&mutexsem);
    printf("Positon at time %f is Lat: %f, Long: %f, Altitude: %f." , localcopy.logtime, localcopy.latitude, localcopy.longitude, localcopy.altitude);

    readagain.tv_sec += 10;

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &readagain, NULL);
  }

  return NULL;
}


int main( int argc, char** argv )
{
  pthread_t writethread, readthread;

  pthread_create(&writethread, NULL, writepos, &position);
  pthread_create(&readthread, NULL, readpos, &position);

  pthread_join(writethread, NULL);
  pthread_join(readthread, NULL);
  return 0;
}