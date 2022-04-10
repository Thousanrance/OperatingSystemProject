/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task task_queue[QUEUE_SIZE];
task worktodo;

// the thread pool
pthread_t threadpool[NUMBER_OF_THREADS];

// the mutex lock for task enqueue and dequeue
pthread_mutex_t mutex;

//semaphore for working one task in one thread
sem_t semaphore;

// the current length of task queue
int curLEN;

//if threads working
int thread_working[NUMBER_OF_THREADS];


// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
	if(curLEN >= QUEUE_SIZE)
	{
		printf("ERROR: Full task queue!\n");
		return 1;
	}
	
	pthread_mutex_lock(&mutex);
	//critical section
	
	task_queue[curLEN] = t;
	curLEN++;
	
	//critical section
	pthread_mutex_unlock(&mutex);
	
    	return 0;
}

// remove a task from the queue
task dequeue() 
{
	if(curLEN <= 0)
	{
		worktodo.data = NULL;
		worktodo.function = NULL;
		printf("ERROR: Empty task queue!\n");
		return worktodo;
	}
	
	pthread_mutex_lock(&mutex);
	//critical section
	
	worktodo = task_queue[0];
	curLEN--;
	//move forward(use array as list)
	for(int i = 0; i < curLEN; i++)
	{
		task_queue[i] = task_queue[i + 1];
	}
	task_queue[curLEN].data = NULL;
	task_queue[curLEN].function = NULL;
	
	//critical section
	pthread_mutex_unlock(&mutex);
	
    	return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    int index = *((int*)param);
    worktodo = dequeue();
    printf("Executing in thread %d.\n", index);
    execute(worktodo.function, worktodo.data);
    
    thread_working[index] = 0;
    sem_post(&semaphore); //release the semaphore

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    worktodo.function = somefunction;
    worktodo.data = p;
    
    int FULL = enqueue(worktodo);   
    if(FULL)
    {
    	printf("ERROR: Pool submit failed!\n");
    	return 1;
    }
    
    sem_wait(&semaphore); //acquire the semaphore
    int index = 0;
    while(1) //find an available thread
    {
    	if(thread_working[index] == 0)
    	{
    		thread_working[index] = 1;
    		break;
    	}
    	index = (index + 1)%(NUMBER_OF_THREADS);
    }
    pthread_create(&threadpool[index], NULL, &worker, &index);

    return 0;
}

// initialize the thread pool
void pool_init(void)
{
	curLEN = 0;
	for(int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		thread_working[i] = 0; // no thread is working
	}
	pthread_mutex_init(&mutex, NULL);
	sem_init(&semaphore, 0, NUMBER_OF_THREADS);
}

// shutdown the thread pool
void pool_shutdown(void)
{
	for(int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		pthread_join(threadpool[i], NULL);
	}
	pthread_mutex_destroy(&mutex);
	sem_destroy(&semaphore);
}


