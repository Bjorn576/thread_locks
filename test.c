#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "sync.h"
#define MIN_DELAY 1000000ULL
#define MAX_DELAY 1000000000ULL



int main()
{
  srand(time(NULL));
  
  struct timespec test1;
  struct timespec test2;
  
  test1.tv_nsec = MIN_DELAY + rand() / (RAND_MAX / (MAX_DELAY - MIN_DELAY + 1) + 1);
  printf("%llu\n", test1.tv_nsec);
  
  nanosleep(&test1, &test2);
}