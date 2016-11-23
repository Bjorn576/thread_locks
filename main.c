#include "main.h"
#include "sync.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {
        c++;
    }
}

//Initialize lock variables
my_spinlock_t spinlock;
my_mutex_t mlock;
my_queuelock_t tlock;
pthread_spinlock_t pspin;
pthread_mutex_t count_mutex;

//Initialize test variables
int numThreads, numItterations, OperationsOutsideCS, OperationsInsideCS, testID;

//Pthread_spinlock test for each thread to call
void *pthreadSpinTest()
{

  int i;
	int j;
	int k;

	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {

		  for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		  {
		  	localCount++;
		  }

		  pthread_spin_lock(&pspin);
  		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		  {
  			c++;
		  }
  		pthread_spin_unlock(&pspin);

    }


}

//TTAS test for each thread to call
void *ttastest()
{
  int i;
	int j;
	int k;

	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {

		  for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		  {
		  	localCount++;
		  }

		  my_spinlock_lockTTAS(&spinlock);
  		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		  {
  			c++;
		  }
  		my_spinlock_unlock(&spinlock);

    }
}

//pthread mutex test for each thread to call
void *pthreadMutexTest()
{

  int i;
	int j;
	int k;

	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {

		  for(j=0;j<OperationsOutsideCS;j++)/*How much work is done outside the CS*/
		  {
		  	localCount++;
		  }


      pthread_mutex_lock(&count_mutex);
  		for(k=0;k<OperationsInsideCS;k++)/*How much work is done inside the CS*/
		  {
  			c++;
		  }
      pthread_mutex_unlock(&count_mutex);

    }


}



//TAS spinlock test for each thread to call
void *spinlocktest()
{

  int j;
  int i;
  int localc = 0;
  for(i=0;i<numItterations;i++)
  {
    //Operations without lock
    for (j=0;j<OperationsOutsideCS;j++)
    {
      localc++;
    }

    my_spinlock_lockTAS(&spinlock);
    //Test for two locks called by the same thread
    //my_spinlock_lockTAS(&spinlock);

    for(j=0;j<OperationsInsideCS;j++)
    {
      c++;
    }
    my_spinlock_unlock(&spinlock);
  }

}

//Ticketlock test for each thread to call
void *tlocktest()
{
  int i;
  int j;
  int localc = 0;
  for(i=0;i<numItterations;i++)
  {
    for(j=0;j<OperationsOutsideCS;j++)
    {
      localc++;
    }

    my_queuelock_lock(&tlock);

    for(j=0;j<OperationsInsideCS;j++)
    {
      c++;
    }
    my_queuelock_unlock(&tlock);
  }
}

//Mutex lock test for each thread to call
void *mlocktest()
{

  int j;
  int i;
  int localc = 0;
  srand(time(NULL));

  for(i=0;i<numItterations;i++)
  {
    for(j=0;j<OperationsOutsideCS;j++)
    {
      localc++;
    }

    my_mutex_lock(&mlock);

    for(j=0;j<OperationsInsideCS;j++)
    {
      c++;
    }
    my_mutex_unlock(&mlock);
  }
}

int runTest(int testID)
{

/*You must create all data structures and threads for you experiments*/

if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
{
  int p;
  unsigned long long average = 0;
  //Open file to write test data
  FILE* f = fopen("pmutex.dat", "w");
  for(p=0;p<10;p++)
  {
    c=0;
  	struct timespec start;
  	struct timespec stop;
  	unsigned long long result; //64 bit integer

  	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
  	int i;
  	int rt;

  	clock_gettime(CLOCK_MONOTONIC, &start);
  	for(i=0;i<numThreads;i++)
  	{

  	   if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) )
  	   {
  		    printf("Thread creation failed: %d\n", rt);
  		    return -1;
  	   }

  	}

  	for(i=0;i<numThreads;i++) //Wait for all threads to finish
  	{
  		 pthread_join(threads[i], NULL);
  	}
  	clock_gettime(CLOCK_MONOTONIC, &stop);

  	//printf("Threaded Run Pthread (Mutex) Total Count: %d\n", c);
  	result=timespecDiff(&stop,&start)/numItterations;
    average += result;

    fprintf(f, "%d %llu\n", p, result);
  	//printf("Pthread Mutex time(ns): %llu\n",result/numItterations);
    free(threads);

  }
  fclose(f);
  printf("Average of all runs on pthread_mutex was %llu\n", average/10);

}

if(testID == 0 || testID == 2) /*Pthread Spinlock*/
{
  int p;
  unsigned long long average = 0;
  //Open file to write test data
  FILE* f = fopen("pspin.dat", "w");
  for(p=0;p<10;p++)
  {
    struct timespec start;
    struct timespec stop;
    unsigned long long result;

    c=0;
    pthread_spin_init(&pspin, 0);
    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int rt;
    int i;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &pthreadSpinTest, NULL)) )
      {
         printf("Thread creation failed: %d\n", rt);
         return -1;
      }
    }

    for(i=0;i<numThreads;i++)
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    result = timespecDiff(&stop, &start)/numItterations;

    fprintf(f, "%d %llu\n", p, result);

    average += result;
    free(threads);
  }
  fclose(f);
  printf("Average of all runs on pthread_spinlock was %llu\n", average/10);
}

if(testID == 0 || testID == 3) /*MySpinlockTAS*/
{
  int p;
  unsigned long long average = 0;
  //Open file to write test data
  FILE* f = fopen("myspin.dat", "w");
  for(p=0;p<10;p++)
  {
    struct timespec start;
    struct timespec stop;
    unsigned long long result;

    c = 0;
    my_spinlock_init(&spinlock);
    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int rt;
    int i;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &spinlocktest, NULL)) )
      {
         printf("Thread creation failed: %d\n", rt);
         return -1;
      }
    }

    for(i=0;i<numThreads;i++)
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    result = timespecDiff(&stop, &start)/numItterations;
    fprintf(f, "%d %llu\n", p, result);
    average += result;
    free(threads);
  }
  fclose(f);
  printf("Average of all runs on my_spinlock was %llu\n", average/10);
}

if(testID == 0 || testID == 4)
{
  int p;
  unsigned long long average = 0;
  //Open file to write test data
  FILE* f = fopen("ttasspin.dat", "w");
  for(p=0;p<10;p++)
  {
    struct timespec start;
    struct timespec stop;
    unsigned long long result;
    c = 0;
    my_spinlock_init(&spinlock);

    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int rt;
    int i;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &ttastest, NULL)) )
      {
         printf("Thread creation failed: %d\n", rt);
         return -1;
      }
    }

    for(i=0;i<numThreads;i++)
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    result = timespecDiff(&stop, &start)/numItterations;
    average += result;
    fprintf(f, "%d %llu\n", p, result);
    free(threads);
  }
  fclose(f);
  printf("Average of all runs on TTAS was %llu\n", average /10);
}

//My_Mutex lock testing
if(testID == 0 || testID == 5)
{
  int p;
  unsigned long long average = 0;
  FILE* f = fopen("mymutex.dat", "w");
  for(p=0;p<10;p++)
  {
    struct timespec start;
    struct timespec stop;
    unsigned long long result;
   
    c = 0;
    my_mutex_init(&mlock);
    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int rt;
    int i;
    //printf("OKAY2\n");
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &mlocktest, NULL)) )
      {
         printf("Thread creation failed: %d\n", rt);
         return -1;
      }
    }

    //printf("OKAY3\n");

    for(i=0;i<numThreads;i++)
    {
      //printf("Thread joined\n");
      pthread_join(threads[i], NULL);
    }
    //printf("OKAY4\n");
    clock_gettime(CLOCK_MONOTONIC, &stop);
    result = timespecDiff(&stop, &start)/numItterations;
    fprintf(f, "%d %llu\n", p, result);
    average += result;
    free(threads);
  }
  fclose(f);
  printf("Average of all runs on my_mutex was %lluns\n", average/10);
}

//ticket_lock testing
if(testID == 0 || testID == 6)
{
  int p;
  unsigned long long average = 0;
  FILE* f = fopen("mymutex.dat", "w");
  for(p=0;p<10;p++)
  {
    struct timespec start;
    struct timespec stop;
    unsigned long long result;

    c=0;
    my_queuelock_init(&tlock);

    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int rt;
    int i;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &tlocktest, NULL)) )
      {
         printf("Thread creation failed: %d\n", rt);
         return -1;
      }
    }

    for(i=0;i<numThreads;i++)
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    result = timespecDiff(&stop, &start)/numItterations;
    fprintf(f, "%d %llu\n", p, result);
    average += result;
    free(threads);
  }
  printf("Average of all runs on ticketlock was %lluns\n", average/10);
}


	return 0;
}

int testAndSetExample()
{
volatile long test = 0; //Test is set to 0
printf("Test before atomic OP:%d\n",test);
tas(&test);
printf("Test after atomic OP:%d\n",test);
}

int processInput(int argc, char *argv[])
{

/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/
  int i;
  int tflag, iflag, oflag, cflag, dflag;

  tflag = iflag = oflag = cflag = dflag = 0;
  printf("%d %d %d %d %d\n", tflag, iflag, oflag, cflag, dflag);

  //Custom input values
  for (i=1;i<argc;i++)
  {
    if(!strcmp("-t", argv[i]) && i+1 < argc)
    {
      numThreads = atoi(argv[i+1]);
      tflag = 1;
    }
    else if(!strcmp("-i", argv[i]) && i+1 < argc)
    {
      numItterations = atoi(argv[i+1]);
      iflag = 1;
    }
    else if(!strcmp("-o", argv[i]) && i+1 < argc)
    {
      OperationsOutsideCS = atoi(argv[i+1]);
      oflag = 1;
    }
    else if(!strcmp("-c", argv[i]) && i+1 < argc)
    {
      OperationsInsideCS = atoi(argv[i+1]);
      cflag = 1;
    }
    else if(!strcmp("-d", argv[i]) && i+1 < argc)
    {
      testID = atoi(argv[i+1]);
      dflag = 1;
    }
  }
  printf("\n%d %d %d %d %d\n", tflag, iflag, oflag, cflag, dflag);

  //Set default values if necessary
  if(!tflag)
    numThreads = 4;
  if(!iflag)
    numItterations = 100000;
  if(!dflag)
    testID = 2;
  if(!oflag)
    OperationsOutsideCS = 0;
  if(!cflag)
    OperationsInsideCS = 1;



	return 0;
}


int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");

	//testAndSetExample(); //Uncomment this line to see how to use TAS

	processInput(argc,argv);
  printf("NUMBER OF THREADS: %d\n", numThreads);
  printf("NUMBER OF ITERATIONS: %d\n", numItterations);
  printf("NUMBER OF OperationsOutsideCS: %d\n", OperationsOutsideCS);
  printf("NUMBER OF OperationsInsideCS: %d\n\n", OperationsInsideCS);
  printf("TESTID: %d\n", testID);
	runTest(testID);
	return 0;

}
