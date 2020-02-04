/*
 * Emilio Francesquini <e.francesquini@ufabc.edu.br>
 * 2018-10-01
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include <x86intrin.h>

#define MICRO ((unsigned long) 1000000)
#define NANOS ((unsigned long)1000000000)
#define ITERS 300000000
long int temp = 0;

void foo () {
  for (int i = 0; i< ITERS; i++)
    temp+=temp;
}

int main() {

  printf("---------------------------------------\n");
  printf("gettimeofday()\n");
  struct timeval before, after;
  time_t         elapsedUs;
  gettimeofday(&before, NULL);
  foo();
  gettimeofday(&after, NULL);
  elapsedUs = after.tv_usec - before.tv_usec + (after.tv_sec - before.tv_sec) * MICRO;
  printf("Tempo total: %lu us - %lf s - ResoluÃ§Ã£o: %lu ns\n",  elapsedUs, (double)elapsedUs/MICRO, NANOS/MICRO);


  /*===============================================
    ===============================================
    ===============================================*/

  printf("---------------------------------------\n");
  printf("clock()\n");
  clock_t start = clock();// Overflow em 72 minutos em mÃ¡quinas 32 bits!
  foo();
  double elapsedTime = (double)(clock() - start) / CLOCKS_PER_SEC;
  printf ("Tempo total: %lf ResoluÃ§Ã£o: %ld ns\n", elapsedTime, NANOS/CLOCKS_PER_SEC);

  /*===============================================
    ===============================================
    ===============================================*/

  printf("---------------------------------------\n");
  printf("clock_get_time()\n");
  uint64_t diff;
  struct timespec tick, tock;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tick);
  foo();
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tock);
  diff = NANOS * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
  printf("Tempo total: %" PRIu64 " ns - %lf s\n", diff, (double)diff/NANOS);

  /* Tipos de relÃ³gios. Veja: man clock_gettime

     CLOCK_REALTIME
     System-wide clock that  measures  real  (i.e.,  wall-clock)  time.

     CLOCK_REALTIME_COARSE (since Linux 2.6.32; Linux-specific)
     A  faster  but  less precise version of CLOCK_REALTIME.

     CLOCK_MONOTONIC
     Clock  that  cannot  be set and represents monotonic time since some unspecified starting
     point.

     CLOCK_MONOTONIC_COARSE (since Linux 2.6.32; Linux-specific)
     A faster but less precise version of CLOCK_MONOTONIC.  Use when you need very  fast,  but
     not fine-grained timestamps.

     CLOCK_MONOTONIC_RAW (since Linux 2.6.28; Linux-specific)
     Similar to CLOCK_MONOTONIC, but provides access to a raw hardware-based time that is  not
     subject to NTP adjustments or the incremental adjustments performed by adjtime(3).

     CLOCK_BOOTTIME (since Linux 2.6.39; Linux-specific)
     Identical  to  CLOCK_MONOTONIC,  except it also includes any time that the system is susâ€
     pended.

     CLOCK_PROCESS_CPUTIME_ID (since Linux 2.6.12)
     Per-process CPU-time clock (measures CPU time consumed by all threads in the process).

     CLOCK_THREAD_CPUTIME_ID (since Linux 2.6.12)
     Thread-specific CPU-time clock.
  */

  struct timespec res;
  clock_getres(CLOCK_REALTIME, &res);
  printf("ResoluÃ§Ã£o CLOCK_REALTIME: %" PRIu64 " ns\n",
         (uint64_t) NANOS * res.tv_sec + res.tv_nsec);
  clock_getres(CLOCK_REALTIME_COARSE, &res);
  printf("ResoluÃ§Ã£o CLOCK_REALTIME_COARSE: %" PRIu64 " ns\n",
         (uint64_t) NANOS * res.tv_sec + res.tv_nsec);
  clock_getres(CLOCK_MONOTONIC, &res);
  printf("ResoluÃ§Ã£o CLOCK_MONOTONIC: %" PRIu64 " ns\n",
         (uint64_t) NANOS * res.tv_sec + res.tv_nsec);
  clock_getres(CLOCK_MONOTONIC_COARSE, &res);
  printf("ResoluÃ§Ã£o CLOCK_MONOTONIC_COARSE: %" PRIu64 " ns\n",
         (uint64_t) NANOS * res.tv_sec + res.tv_nsec);
  clock_getres(CLOCK_PROCESS_CPUTIME_ID, &res);
  printf("ResoluÃ§Ã£o CLOCK_PROCESS_CPUTIME_ID: %" PRIu64 " ns\n",
         (uint64_t) NANOS * res.tv_sec + res.tv_nsec);
  clock_getres(CLOCK_THREAD_CPUTIME_ID, &res);
  printf("ResoluÃ§Ã£o CLOCK_THREAD_CPUTIME_ID: %" PRIu64 " ns\n",
         (uint64_t) NANOS * res.tv_sec + res.tv_nsec);
  clock_getres(CLOCK_BOOTTIME, &res);
  printf("ResoluÃ§Ã£o CLOCK_BOOTTIME: %" PRIu64 " ns\n",
         (uint64_t) NANOS * res.tv_sec + res.tv_nsec);


   /*===============================================
    ===============================================
    ===============================================*/

  /*
    Para info apenas. Complicado utilizar pois existem diversos
    problemas como:
    - NÃ£o tem suporte em todas as arquiteturas
    - NÃ£o tem suporte em todos os compiladores
    - Reordenamento de instruÃ§Ãµes do processador
    - VariaÃ§Ãµes de frequÃªncia
    - NÃ£o mede em tempo, mas em ciclos de clock (desconhecido e
      variÃ¡vel)
      - A menos que o processador suporte "invariant TSC"
   */
  printf("---------------------------------------\n");
  printf("__rdtsc()\n");
  uint64_t tsc = __rdtsc();
  foo();
  uint64_t tsc2 = __rdtsc();
  uint64_t tscDiff = tsc2 - tsc;
  printf ("Tempo total: %" PRIu64 " ciclos - Supondo 2.9GHz: %lf s\n", tscDiff, (double)tscDiff/2900000000.0);

  printf("---------------------------------------\n");
}
