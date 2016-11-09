#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "sync.h"

int main()
{
  srand(time(NULL));
  
  struct timespec test1;
  struct timespec test2;
  
  test1.tv_nsec = 1000000ULL + rand() / (RAND_MAX / (1000000000ULL - 1000000ULL + 1) + 1);
  printf("%llu\n", test1.tv_nsec);
  
  nanosleep(&test1, &test2);
}