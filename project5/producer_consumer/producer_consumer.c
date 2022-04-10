#include "buffer.h"
#include <stdlib.h> /* required for rand() */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define RAND_MAX_SLEEP 3
#define RAND_MAX_ITEM 100

/* the buffer */
buffer_item buffer[BUFFER_SIZE];
int curSize;

int insert_item(buffer_item item)
{
	/* insert item into buffer
	return 0 if successful, otherwise
	return -1 indicating an error condition */
	if(curSize >= BUFFER_SIZE)
	{
		return -1;
	}
	buffer[curSize] = item;
	curSize++;
	return 0;
}

int remove_item(buffer_item *item)
{
	/* remove an object from buffer
	placing it in item
	return 0 if successful, otherwise
	return -1 indicating an error condition */
	if(curSize <= 0)
	{
		return -1;
	}
	*item = buffer[0];
	curSize--;
	for(int i = 0; i < curSize; i++) //use array as list
	{
		buffer[i] = buffer[i + 1];
	}
	buffer[curSize] = -1;
	return 0;
}


/* the producer and consumer thread */
pthread_mutex_t mutex;
sem_t full;
sem_t empty;

void *producer(void *param)
{
	buffer_item item;
	while(1)
	{
		/* sleep for a random period of time */
		int sleep_t = rand()%RAND_MAX_SLEEP;
		sleep(sleep_t);
		
		/* produce an item in next produced */
		/* generate a random number */
		item = rand()%RAND_MAX_ITEM;
		
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		/* add next produced to the buffer */
		if(insert_item(item))
		{
			printf("ERROR: Insert failed!\n");
		}
		else
		{
			printf("Producer produced %d.\n",item);
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
}

void *consumer(void *param)
{
	buffer_item item;
	while(1)
	{
		/* sleep for a random period of time */
		int sleep_t = rand()%RAND_MAX_SLEEP;
		sleep(sleep_t);
		
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		/* remove an item from buffer to next consumed */
		if(remove_item(&item))
		{
			printf("ERROR: Remove failed!\n");
		}
		else
		{
			printf("Consumer consumed %d.\n",item);
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
		/* consume the item in next comsumed */
	}
}

/* main */
int main(int argc, char *argv[])
{
	/* 1. Get command line arguments argv[1],argv[2],argv[3]
		argv[1]: How long to sleep before terminating
		argv[2]: The number of producer threads
		argv[3]: The number of consumer threads
	*/
	if(argc != 4)
	{
		printf("ERROR: Wrong input!\n");
		return 1;
	}
	const int sleep_time = atoi(argv[1]);
	const int num_of_producer = atoi(argv[2]);
	const int num_of_consumer = atoi(argv[3]);
	
	/* 2. Initialize buffer */
	curSize = 0;
	pthread_mutex_init(&mutex, NULL);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);
	
	/* 3. Create producer thread(s) */
	pthread_t producer_thread[num_of_producer];
	for(int i = 0; i < num_of_producer; i++)
	{
		pthread_create(&producer_thread[i], NULL, producer, NULL);
	}
	
	/* 4. Create consumer thread(s) */
	pthread_t consumer_thread[num_of_consumer];
	for(int j = 0; j < num_of_consumer; j++)
	{
		pthread_create(&consumer_thread[j], NULL, consumer, NULL);
	}
	
	/* 5. Sleep */
	printf("Sleep begin.\n");
	sleep(sleep_time);
	printf("Terminate.\n");
	
	/* 6. Exit */
	for(int i = 0; i < num_of_producer; i++)
	{
		pthread_cancel(producer_thread[i]);
	}
	for(int j = 0; j < num_of_consumer; j++)
	{
		pthread_cancel(consumer_thread[j]);
	}
	
	pthread_mutex_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);
	
	return 0;
}