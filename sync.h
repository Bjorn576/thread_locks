/*
 * NAME, etc.
 *
 * sync.h
 */

#ifndef _m_SYNC_H_
#define _m_SYNC_H_

#include "atomic_ops.h"
#include <pthread.h>

struct my_mutex_struct {
  volatile unsigned long val;
  volatile pthread_t owner;
  volatile int lcount;
};




typedef struct my_mutex_struct my_mutex_t;


int my_mutex_init(my_mutex_t *lock);
int my_mutex_unlock(my_mutex_t *lock);
int my_mutex_destroy(my_mutex_t *lock);

int my_mutex_lock(my_mutex_t *lock);
int my_mutex_trylock(my_mutex_t *lock);



/*Spinlock Starts here*/

struct my_spinlock_struct {
  volatile unsigned long val;
  volatile pthread_t owner;
  //lcount tracks number of times locked
  volatile unsigned int lcount;
};

typedef struct my_spinlock_struct my_spinlock_t;

int my_spinlock_init(my_spinlock_t *lock);
int my_spinlock_destroy(my_spinlock_t *lock);
int my_spinlock_unlock(my_spinlock_t *lock);

int my_spinlock_lockTAS(my_spinlock_t *lock);
int my_spinlock_lockTTAS(my_spinlock_t *lock);
int my_spinlock_trylock(my_spinlock_t *lock);


/*Spinlock Starts here*/

//2 integers defined for the queue and deqeue values
struct my_queuelock_struct {
  volatile unsigned int tqueue;
  volatile unsigned int tdequeue;
  volatile unsigned long val;
  volatile unsigned int lcount;
  volatile pthread_t owner;
};

typedef struct my_queuelock_struct my_queuelock_t;

int my_queuelock_init(my_queuelock_t *lock);
int my_queuelock_destroy(my_queuelock_t *lock);
int my_queuelock_unlock(my_queuelock_t *lock);

int my_queuelock_lock(my_queuelock_t *lock);
int my_queuelock_trylock(my_queuelock_t *lock);


#endif
