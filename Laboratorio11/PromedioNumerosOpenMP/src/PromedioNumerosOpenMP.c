/*
 ============================================================================
 Name        : PromedioNumerosOpenMP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello OpenMP World in C
 ============================================================================
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char *argv[]) {
  int numThreads, tid;
  int n=80;
  int A[n];
  srand(time(NULL));

  for(int i=0;i<n;i++){
	  A[i]=rand()%100;
  }
  int sum=0,prom;
  omp_set_num_threads(10);
  /* This creates a team of threads; each thread has own copy of variables  */
#pragma omp parallel private(numThreads, tid)
 {
   tid = omp_get_thread_num();
   numThreads = omp_get_num_threads();

   for(int i=tid*(n/numThreads);i<(n/numThreads)*(tid+1);i++){
	 sum=sum+A[i];
	 printf(" thread number %d : %d \n", tid,A[i]);
   }


   /* The following is executed by the master thread only (tid=0) */
   if (tid == 0)
     {
       prom=sum/numThreads;
       printf("Number of terms is %d\n", n);
       printf("Number of threads is %d\n", numThreads);
       printf("Average of numbers is %d\n", prom);
     }

 }
 return 0;
}

