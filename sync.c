/*
 * NAME, etc.
 *
 * sync.c
 *
 */

#define _REENTRANT

#include "sync.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define MIN_DELAY 10000
#define MAX_DELAY 1000000000

/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
  if(lock != NULL)
  {
    lock->owner = 0;
    lock->val = 0;
    return 0;
  }
  return -1;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
}

int my_spinlock_unlock(my_spinlock_t *lock)
{
  if(lock != NULL)
  {
    if(pthread_self() == lock->owner)
    {
      lock->val = 0;
      printf("UNLOCKED\n");
      lock->owner = 0;
      return 0;
    }
  }
  return -1;
  
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
  int tid = pthread_self();
  if(lock->owner == tid)
  {
    return 1;
  }
  
  while(tas(&(lock->val)));
  printf("Locked!\n");

  lock->owner = tid;
  //Should ^ be implemented like below?
  //cas(&(lock->owner), 0, tid);

  
  return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
  int tid = pthread_self();
  //If thread is trying to get a lock it already has
  if(lock->owner == tid)
  {
    return 1;
  }
  
  while(1)
  {
    while(lock->val);
    if(!tas(&(lock->val)))
    {
      printf("LOCKED\n");
      if(lock->owner == tid)
      {
        return 1;
      }
      lock->owner = tid;
      //cas(&(lock->owner), 0, tid);
      return 0;  
    }
      
  }
  return 1;
}

int my_spinlock_trylock(my_spinlock_t *lock)
{
  if(lock->val == 1)
    return 0;
  else
    my_spinlock_lockTTAS(lock);
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
  if(lock != NULL)
  {
    lock->val = 0;
    lock->owner = 0;
  }
}

int my_mutex_destroy(my_mutex_t *lock)
{
}

int my_mutex_unlock(my_mutex_t *lock)
{
  if(lock != NULL)
  {
    if(pthread_self() == lock->owner)
    {
      lock->val = 0;
      lock->owner = 0;
      printf("MUTEX UNLOCKED\n");
      return 0;
    }
  }
}

int my_mutex_lock(my_mutex_t *lock)
{
  srand(time(NULL));
  struct timespec delay;
  struct timespec test;
  unsigned long long currdelay = MIN_DELAY;
  
  while(1)
  {
    //Wait until lock 'looks' available
    while(lock->val);
    if(!tas(&(lock->val)))
    {
      lock->owner = pthread_self();
      printf("LOCKED\n");
      return 0;  
    }
        
      //Random time between 0 and currdelay nanoseconds
      delay.tv_nsec = currdelay * rand() / RAND_MAX;
      
      //Sleep for that time
      nanosleep(&delay, &test);
      if(currdelay < MAX_DELAY)
        currdelay *= 2;
    }
}

int my_mutex_trylock(my_mutex_t *lock)
{
}

/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *lock)
{
}

int my_queuelock_destroy(my_queuelock_t *lock)
{
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
}

int my_queuelock_lock(my_queuelock_t *lock)
{
}

int my_queuelock_trylock(my_queuelock_t *lock)
{
}

