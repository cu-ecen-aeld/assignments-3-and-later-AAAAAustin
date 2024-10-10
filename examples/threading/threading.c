#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Optional: use these functions to add debug or error prints to your application
//#define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	
	int return_code;
	struct thread_data *thread_func_args = (struct thread_data *) thread_param;

	struct timespec wait_to_obtain_ms;
	wait_to_obtain_ms.tv_sec = 0;
	wait_to_obtain_ms.tv_nsec = thread_func_args->wait_to_obtain_ms * 1000000;
	nanosleep(&wait_to_obtain_ms, NULL);

	return_code = pthread_mutex_lock( thread_func_args->mutex );
	if ( return_code != 0 ) {
		ERROR_LOG("FAILURE PTHREAD MUTEX LOCK");
		return thread_param;
	}
	
	struct timespec wait_to_release_ms;
    wait_to_release_ms.tv_sec = 0;
    wait_to_release_ms.tv_nsec = thread_func_args->wait_to_release_ms * 1000000;
    nanosleep(&wait_to_release_ms, NULL);

	return_code = pthread_mutex_unlock( thread_func_args->mutex );
	if ( return_code != 0 ) {
        ERROR_LOG("FAILURE PTHREAD MUTEX LOCK"); 
		return thread_param;
    }

	thread_func_args->thread_complete_success = true;

    return thread_param;

}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

	int return_code;
	struct thread_data *thread_stuff = malloc(sizeof(struct thread_data));

	if ( thread_stuff == NULL ) {
		ERROR_LOG("FAILURE ALLOCATING MEMORY");
		return EXIT_FAILURE;
	} 
	
	thread_stuff->thread_complete_success = false;
	thread_stuff->wait_to_obtain_ms = wait_to_obtain_ms;
	thread_stuff->wait_to_release_ms = wait_to_release_ms; 
	thread_stuff->mutex = mutex;

	return_code = pthread_create(thread, NULL, threadfunc, (void *)thread_stuff); 
	if ( return_code != 0 ) {
		ERROR_LOG("FAILURE PTHREAD_CREATE");
		free(thread_stuff);
		return EXIT_FAILURE;
	}
	else {
		return true;
	}

    return false;

}

