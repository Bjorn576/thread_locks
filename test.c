#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#define MIN_DELAY 10000ULL
#define MAX_DELAY 10000000LL



int main()
{
  srand(time(NULL));
  
  struct timespec test1;
  struct timespec test2;
  unsigned long currdelay = MIN_DELAY;
  
  int i;
  for(i=0;i<100;i++)
  {
    test1.tv_nsec = currdelay * rand() / RAND_MAX;
    printf("%llu\n", test1.tv_nsec);
    
    printf("Nanosleep returns: %d\n", nanosleep(&test1, &test2));
  }
  
}