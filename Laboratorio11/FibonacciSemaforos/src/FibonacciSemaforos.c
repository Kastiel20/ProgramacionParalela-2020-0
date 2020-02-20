/*
 ============================================================================
 Name        : FibonacciSemaforos.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello OpenMP World in C
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>  /* Semaphores are not part of Pthreads */

const int MAX_THREADS = 1024;
const int MSG_MAX = 100;

/* Global variables:  accessible to all threads */
int thread_count;
char** messages;
sem_t* semaphores;
float next,first=0, second=1;
double sumg=0;
long long n;
void Usage(char* prog_name);
void *Send_msg(void* rank); /* Thread function */
void* Thread_sum(void* rank);
void Get_args(int argc, char* argv[]);
/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	Get_args(argc, argv);
	if (thread_count <= 0 || thread_count > MAX_THREADS)
		Usage(argv[0]);

	thread_handles = malloc(thread_count * sizeof(pthread_t));
	messages = malloc(thread_count * sizeof(char*));
	semaphores = malloc(thread_count * sizeof(sem_t));
	for (thread = 0; thread < thread_count; thread++) {
		messages[thread] = NULL;
		/* Initialize all semaphores to 0 -- i.e., locked */
		sem_init(&semaphores[thread], 0, 0); //Vector de semaforos
	}
	sem_post(&semaphores[0]);
	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], (pthread_attr_t*) NULL,
				Thread_sum, (void*) thread);

	for (thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	for (thread = 0; thread < thread_count; thread++) {
		free(messages[thread]);
		sem_destroy(&semaphores[thread]);
	}

	printf("\nWith n = %lld terms,\n", n);
	printf("   Multi-threaded estimate of fibonacci  = %.15f\n", sumg);

	free(messages);
	free(semaphores);
	free(thread_handles);

	return 0;
} /* main */

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

	fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
	exit(0);
} /* Usage */

/*-------------------------------------------------------------------
 * Function:       Send_msg
 * Purpose:        Create a message and ``send'' it by copying it
 *                 into the global messages array.  Receive a message
 *                 and print it.
 * In arg:         rank
 * Global in:      thread_count
 * Global in/out:  messages, semaphores
 * Return val:     Ignored
 * Note:           The my_msg buffer is freed in main
 */
void *Send_msg(void* rank) {
	long my_rank = (long) rank;
	long dest = (my_rank + 1) % thread_count;
	char* my_msg = (char*) malloc(MSG_MAX * sizeof(char));

	sprintf(my_msg, "Hello to %ld from %ld", dest, my_rank);
	messages[dest] = my_msg;
	sem_post(&semaphores[dest]); /* "Unlock" the semaphore of dest */

	sem_wait(&semaphores[my_rank]); /* Wait for our semaphore to be unlocked */
	printf("Thread %ld > %s\n", my_rank, messages[my_rank]);

	return NULL;
} /* Send_msg */


void* Thread_sum(void* rank) {
   long my_rank = (long) rank;
   long long my_n = n/thread_count;
   long dest = (my_rank + 1) % thread_count;


   sem_wait(&semaphores[my_rank]); /* Wait for our semaphore to be unlocked */
   for(int i=0 ; i<my_n;i++){
	   if(i<=1 && my_rank==0){
		   next=i;
	   }else{
		   next=first+second;
		   first=second;
		   second=next;
	  	}
	   	sumg=sumg+next;
	    printf("\nHilo %ld debug -> %.15f",my_rank,next);
	}
   sem_post(&semaphores[dest]); /* "Unlock" the semaphore of dest */
   return NULL;
}
void Get_args(int argc, char* argv[]) {
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0) Usage(argv[0]);
} /* Get_args */

