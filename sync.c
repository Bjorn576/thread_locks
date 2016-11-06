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

/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
  if(lock != NULL)
  {
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
    lock->val = 0;
    printf("UNLOCKED\n");
    return 0;
  }
  return -1;
  
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
  while(tas(&(lock->val)));
  printf("Locked!\n");
  return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
  while(1)
  {
    while(lock->val);
    if(!tas(&(lock->val)))
    {
      printf("LOCKED\n");
      return 1;  
    }
      
  }
  return 0;
}

int my_spinlock_trylock(my_spinlock_t *lock)
{
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
}

int my_mutex_destroy(my_mutex_t *lock)
{
}

int my_mutex_unlock(my_mutex_t *lock)
{
}

int my_mutex_lock(my_mutex_t *lock)
{
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

