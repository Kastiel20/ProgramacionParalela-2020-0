/* File:     pth_pi_mutex.c
 * Purpose:  Estimate pi using series
 *
 *              pi = 4*[1 - 1/3 + 1/5 - 1/7 + 1/9 - . . . ]
 *
 *           This version uses a mutex to protect the critical section
 *
 * Compile:  gcc -g -Wall -o pth_pi_mutex pth_pi_mutex.c -lm -lpthread
 *           timer.h needs to be available
 * Run:      ./pth_pi_mutex <number of threads> <n>
 *           n is the number of terms of the Maclaurin series to use
 *           n should be evenly divisible by the number of threads
 *
 * Input:    none
 * Output:   The estimate of pi using multiple threads, one thread, and the
 *           value computed by the math library arctan function
 *           Also elapsed times for the multithreaded and singlethreaded
 *           computations.
 *
 * Notes:
 *    1.  The radius of convergence for the series is only 1.  So the
 *        series converges quite slowly.
 *
 * IPP:   Section 4.6 (pp. 168 and ff.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

const int MAX_THREADS = 1024;

long thread_count;
long long n;
double sum;
pthread_mutex_t mutex;

double a=10, b=100, h, local_a, local_b;
double local_int, total_int;

void* Thread_sum(void* rank);
/* Calculate local integral  */
double Trap(double left_endpt, double right_endpt, int trap_count,
   double base_len);
/* Function we're integrating */
double f(double x);
/* Only executed by main thread */
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

int main(int argc, char* argv[]) {
   long       thread;  /* Use long in case of a 64-bit system */
   pthread_t* thread_handles;
   double start, finish, elapsed;

   /* Get number of threads from command line */
   Get_args(argc, argv);

   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));
   pthread_mutex_init(&mutex, NULL);
   sum = 0.0;

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
          Thread_sum, (void*)thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);
   GET_TIME(finish);
   elapsed = finish - start;


   printf("With n = %lld particiones,\n", n);
   printf(" El area estimada del trapecio es = %.15f\n", total_int);
   printf("The elapsed time is %e seconds\n", elapsed);


   pthread_mutex_destroy(&mutex);
   free(thread_handles);
   return 0;
}  /* main */

/*------------------------------------------------------------------*/
void* Thread_sum(void* rank) {
   long my_rank = (long) rank;
   long long local_n = n/thread_count;

   h = (b-a)/n;          /* h is the same for all processes */

   local_a = a + my_rank*local_n*h;
   local_b = local_a + local_n*h;
   local_int = Trap(local_a, local_b, local_n, h);

   pthread_mutex_lock(&mutex);
   total_int += local_int;
   pthread_mutex_unlock(&mutex);

   return NULL;
}  /* Thread_sum */



/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0) Usage(argv[0]);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
   fprintf(stderr, "   n is the number of terms and should be >= 1\n");
   fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
   exit(0);
} /* Usage */

double Trap(
      double left_endpt  /* in */,
      double right_endpt /* in */,
      int    trap_count  /* in */,
      double base_len    /* in */) {
   double estimate, x;
   int i;

   estimate = (f(left_endpt) + f(right_endpt))/2.0;
   for (i = 1; i <= trap_count-1; i++) {
      x = left_endpt + i*base_len;
      estimate += f(x);
   }
   estimate = estimate*base_len;

   return estimate;
} /*  Trap  */

double f(double x /* in */) {
   return x*x;
}
