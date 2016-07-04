/*
 * Read - Write lock example.
 *
 * The lock allows concurrent read operations and exclusive write operations
 *
 * This example uses two-mutex implementation for read-write lock.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_READER 10
#define MAX_WRITER 2

int counter=0;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void *thread_function_reader(void *ptr);
void *thread_function_writer(void *ptr);

void lock()
{
	pthread_mutex_lock( &mutex1 );
}

void unlock()
{
	pthread_mutex_unlock( &mutex1 );
}


pthread_mutex_t mutex_read = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_exclusive = PTHREAD_MUTEX_INITIALIZER;
int reader_counter = 0;

void read_lock()
{
	pthread_mutex_lock( &mutex_read );
	reader_counter++;
	if (reader_counter == 1)
		pthread_mutex_lock( &mutex_exclusive );
	pthread_mutex_unlock( &mutex_read );
}

void read_unlock()
{
	pthread_mutex_lock( &mutex_read );
	reader_counter--;
	if (reader_counter == 0)
		pthread_mutex_unlock( &mutex_exclusive );
	pthread_mutex_unlock( &mutex_read );
}

void write_lock()
{
	pthread_mutex_lock( &mutex_exclusive );
}

void write_unlock()
{
	pthread_mutex_unlock( &mutex_exclusive );
}

int main(int argc, char **argv)
{
	pthread_t threads[MAX_READER + MAX_WRITER];
	int  ret, i;

	srand(time(NULL));

	for ( i = 0; i < MAX_READER + MAX_WRITER; ++i) {
		ret = pthread_create( &threads[i], NULL, i < MAX_READER ? thread_function_reader :
				thread_function_writer , (void*) i);
		if(ret)
		{
			fprintf(stderr,"Error - pthread_create() return code: %d\n",ret);
			exit(EXIT_FAILURE);
		}
	}

	for ( i = 0; i < MAX_READER + MAX_WRITER; ++i) {
		pthread_join( threads[i], NULL);
	}

	exit(EXIT_SUCCESS);
}

void *thread_function_reader( void *ptr )
{
	int id = (int) ptr;
	int r;
	char message[64] = {};

	sprintf(message, "Thread %d ", id);

	while(1) {
		read_lock();
		printf("%s %d \n", message, counter);
		read_unlock();
		r = rand() % 10;
		sleep(r);
	}
}

void *thread_function_writer( void *ptr )
{
	int id = (int) ptr;
	int r;
	char message[64] = {};

	sprintf(message, "Thread %d ", id);

	while(1) {
		write_lock();
		counter++;
		printf("%s %d \n", message, counter);
		write_unlock();
		r = rand() % 10;
		sleep(r);
	}
}
