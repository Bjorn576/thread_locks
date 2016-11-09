#include "main.h"
#include "sync.h"
#include <string.h>
#include <stdio.h>

void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   
}


pthread_mutex_t count_mutex;
int numThreads, numItterations, OperationsOutsideCS, OperationsInsideCS, testID;

void *pthreadMutexTest()
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
		
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_mutex_unlock(&count_mutex);    
	
    }   


}

my_spinlock_t spinlock;
my_mutex_t mlock;

void *mlocktest()
{
  int j;
  int i;
  for(i=0;i<numItterations;i++)
  {
    
    my_mutex_lock(&mlock);    
    
    
    for(j=0;j<OperationsInsideCS;j++)
    {
      c++;
    }
    my_mutex_unlock(&mlock);
  }
}
void *spinlocktest()
{
  
  int j;
  int i;
  for(i=0;i<numItterations;i++)
  {
    
    my_spinlock_lockTAS(&spinlock);    
    //Test for two locks called by the same thread
    my_spinlock_lockTAS(&spinlock); 
    
    for(j=0;j<OperationsInsideCS;j++)
    {
      c++;
    }
    my_spinlock_unlock(&spinlock);
  }
  
}

int runTest(int testID)
{

/*You must create all data structures and threads for you experiments*/

if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
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

	printf("Threaded Run Pthread (Mutex) Total Count: %d\n", c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);

}

if(testID == 0 || testID == 2) /*Pthread Spinlock*/
{
  
}

if(testID == 0 || testID == 3) /*MySpinlockTAS*/
{
  c=0;
  my_spinlock_init(&spinlock);
  pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
  int rt;
  int i;

  for(i=0;i<numThreads;i++)
  {
    printf("Create thread\n");
    if( rt=(pthread_create( threads+i, NULL, &spinlocktest, NULL)) )
    {
       printf("Thread creation failed: %d\n", rt);
       return -1;	
    }
  }
  
  for(i=0;i<numThreads;i++)
  {
    pthread_join(threads[i], NULL);
    printf("Thread joined\n");
  }
  printf("DONE TEST ON MY_SPINLOCK\n");
}

if(testID == 0 || testID == 4)
{
  //my_mutex_lock testing
  c=0;
  my_mutex_init(&mlock);
  pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
  int rt;
  int i;

  for(i=0;i<numThreads;i++)
  {
    printf("Create thread\n");
    if( rt=(pthread_create( threads+i, NULL, &mlocktest, NULL)) )
    {
       printf("Thread creation failed: %d\n", rt);
       return -1;	
    }
  }
  
  for(i=0;i<numThreads;i++)
  {
    pthread_join(threads[i], NULL);
    printf("Thread joined\n");
  }
  printf("DONE TEST ON MY_MUTEX\n");
  
}

/*....you must implement the other tests....*/

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
  printf("All these should be 0 %d %d %d %d %d\n", tflag, iflag, oflag, cflag, dflag);
  
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
  printf("All these should be 0 %d %d %d %d %d\n", tflag, iflag, oflag, cflag, dflag);
  
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
  printf("NUMBER OF OperationsInsideCS: %d\n", OperationsInsideCS);
  printf("TESTID: %d\n", testID);
	runTest(testID);
	return 0;

}
