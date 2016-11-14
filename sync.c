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
#include <time.h>
#include <unistd.h>
#define MIN_DELAY 10ULL
#define MAX_DELAY 1000ULL

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
  return 0;
}

int my_spinlock_unlock(my_spinlock_t *lock)
{
  if(lock != NULL)
  {
    if(pthread_equal(pthread_self(), lock->owner))
    {      
      lock->owner = 0;
      lock->val = 0;
      return 0;
    }
    printf("Not the owner\n");
    return 1;
  }
  else
    return -1;
  
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
  pthread_t tid = pthread_self();
  if(pthread_equal(tid, lock->owner))
    return 1;
  
  while(tas(&(lock->val)));
  
  
  lock->owner = tid;
  //Should ^ be implemented like below?
  //cas(&(lock->owner), 0, tid);
  return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
  pthread_t tid = pthread_self();
  //If thread is trying to get a lock it already has
  if(pthread_equal(tid, lock->owner))
    return 1;
  
  while(1)
  {
      
    while(lock->val);
    if(!tas(&(lock->val)))
    {
      lock->owner = tid;
      //cas(&(lock->owner), 0, tid);
      return 0;  
    }
      
  }
  return 1;
}

//Does not spin but returns 1 if it acquires spin-lock on first try, 0 if it doesn't, and -1 if the lock was NULL
int my_spinlock_trylock(my_spinlock_t *lock)
{
  if(lock != NULL)
  {
    if(!tas(&(lock->val)))
      return 1;
    else
      return 0;
  }
  else
    return -1;
  
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
  //Nothing needs to be cleaned up
  return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{
  if(lock != NULL)
  {
    if(pthread_equal(lock->owner, pthread_self()))
    {
      
      lock->owner = 0;
      lock->val = 0;
      //printf("Unlocked!\n");
      return 0;
    }
    printf("Not equal to owner\n");
  }
}

int my_mutex_lock(my_mutex_t *lock)
{
  if(lock == NULL)
    return -1;
    
  
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
      //printf("LOCKED\n");
      return 0;  
    }
        
    //Random time between 0 and currdelay nanoseconds
    delay.tv_nsec = currdelay * rand() / RAND_MAX;
    
    //Sleep for that time
    printf("Sleep returned : %llu\n", nanosleep(&delay, &test));
    if(currdelay < MAX_DELAY)
      currdelay *= 2;
  }
}

int my_mutex_trylock(my_mutex_t *lock)
{
  if(lock != NULL)
  {
    if(!tas(&(lock->val)))
      return 1;
    else
      return 0;
  }
  else
    return -1;
}

/*
 * Queue Lock
 */


int my_queuelock_init(my_queuelock_t *lock)
{
  if(lock == NULL)
    return -1;

  lock->tqueue = 0;
  lock->tdequeue = 0;
  
}

int my_queuelock_destroy(my_queuelock_t *lock)
{
  return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
  if(lock==NULL)
    return -1;
  
  fna(&(lock->tqueue), 1);
}

int my_queuelock_lock(my_queuelock_t *lock)
{
  if(lock == NULL)
    return -1;

  //fna is an atomic operation (fetch and add)
  unsigned long my_ticket = fna(&(lock->tqueue), 1);
  printf("my_ticket after assignment: %d\n", my_ticket);
  

}

int my_queuelock_trylock(my_queuelock_t *lock)
{
}

