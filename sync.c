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
#define MIN_DELAY 1
#define MAX_DELAY 10

/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
  if(lock != NULL)
  {
    lock->owner = 0;
    lock->val = 0;
    lock->lcount = 0;
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
      lock->lcount--;
      //if lcount is 0 then we have undone all the locks called by a particular thread
      if(lock->lcount == 0)
      {
	       lock->owner = 0;
         lock->val = 0;
      }
      return 0;
    }
    return 1;
  }
  else
    return -1;

}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
  pthread_t tid = pthread_self();
  if(pthread_equal(tid, lock->owner))
  {
    lock->lcount++;
    return 1;
  }

  while(tas(&(lock->val)));

  lock->lcount++;
  lock->owner = tid;
  return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
  pthread_t tid = pthread_self();
  //If thread is trying to get a lock it already has
  if(pthread_equal(tid, lock->owner))
  {
    lock->lcount++;
    return 1;
  }

  while(1)
  {
    while(lock->val);
    if(!tas(&(lock->val)))
    {
      lock->lcount++;
      lock->owner = tid;
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
    {
      lock->lcount++;
      lock->owner = pthread_self();
      return 1;
    }
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
    lock->lcount = 0;
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
      lock->lcount--;
      if(lock->lcount == 0)
      {
	       lock->owner = 0;
         lock->val = 0;
      }
      return 0;
    }
  }
}

int my_mutex_lock(my_mutex_t *lock)
{
  if(lock == NULL)
    return -1;

  pthread_t tid = pthread_self();
  if(pthread_equal(tid, lock->owner))
  {
    lock->lcount++;
    return 0;
  }
  struct timespec delay;
  //struct timespec test;
  long sleep;
  unsigned long long currdelay = MIN_DELAY;

  while(1)
  {
    //Wait until lock 'looks' available
    while(lock->val);
    if(!tas(&(lock->val)))
    {
      lock->lcount++;
      lock->owner = pthread_self();
      return 0;
    }

    //Random time between 0 and currdelay microseconds
    sleep = currdelay * rand() / RAND_MAX;

    //Sleep for that time
    usleep(sleep);

    if(currdelay < MAX_DELAY)
      currdelay *= 2;
  }
}

int my_mutex_trylock(my_mutex_t *lock)
{
  if(lock != NULL)
  {
    if(!tas(&(lock->val)))
    {
     //Should a thread be able to trylock it's own lock?
      lock->lcount++;
      return 1;
    }
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
  lock->owner = 0;
}

int my_queuelock_destroy(my_queuelock_t *lock)
{
  return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
  if(lock==NULL)
    return -1;

  if(pthread_equal(pthread_self(), lock->owner))
  {
    lock->lcount--;
    if(lock->lcount == 0)
    {
      //increment the 'nowserving' ticket, set owner to nothing, and decrement number of locks acquired by particular thread.
      lock->owner = 0;
      lock->val = 0;
      fna(&(lock->tdequeue), 1);
      return 0;
    }
    return 1;
  }
}

int my_queuelock_lock(my_queuelock_t *lock)
{
  if(lock == NULL)
    return -1;

  pthread_t tid = pthread_self();
  if(pthread_equal(tid, lock->owner))
  {
    lock->lcount++;
    return 1;
  }


  //fna is an atomic operation (fetch and add)
  unsigned long my_ticket = fna(&(lock->tqueue), 1);

  //busy wait if my_ticket is not 'being served'
  //if this check fails, the thread gives up the CPU to make sure all threads are able to check this condition in an acceptable amount of time
  while(my_ticket != lock->tdequeue || lock->val == 1)
    pthread_yield();
  //Only one thread can leave this loop at a given moment because of the queue so don't need condition
  lock->owner = tid;
  lock->lcount++;
  lock->val = 1;
}

int my_queuelock_trylock(my_queuelock_t *lock)
{
  if(lock == NULL)
    return -1;

  //Val for this lock is just a flag for trylock
  if(!tas(&(lock->val)))
  {
    lock->lcount++;
    return 1;
  }
  else
  {
    return 0;
  }
}
