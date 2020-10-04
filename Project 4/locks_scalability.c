/**
 * Compile flags:
 *
 * -DTAS_CAS : include TAS lock implementation with __sync_val_compare_and_swap()
 * -DTTAS_CAS: include TTAS lock implementation with __sync_val_compare_and_swap()
 * -DTAS_TS : include TAS lock implementation with __sync_lock_test_and_set()
 * -DTTAS_TS: include TTAS lock implementation with __sync_lock_test_and_set() 
 * -DMUTEX: include Pthread mutex implementation
 *
 * -DREAL: compile for real machine
 * -DSNIPER: compile for Sniper simulator
 *
 * -DDEBUG: include debugging information 
 */

#define _GNU_SOURCE

#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef REAL
#include <sys/time.h>
#elif defined SNIPER
#include "sim_api.h"
#endif

#if ( defined TAS_CAS || defined TTAS_CAS || defined TAS_TS || defined TTAS_TS ) 
#include "lock.h"
#endif

static void* malloc_safe(size_t);
static inline double dummy_work(double);

/* total iterations the critical section is executed by each thread*/
unsigned long iters;  

/* amount of work performed in the critical section */
unsigned long grain = 1;

/* synchronization barrier */
pthread_barrier_t bar;

#ifdef REAL
struct timeval begin, end;
#endif 

double val = 0.0;

/* lock/mutex definition */
#if ( defined TAS_CAS || defined TTAS_CAS || defined TAS_TS || defined TTAS_TS ) 
spinlock_t lock;
#elif defined MUTEX
pthread_mutex_t mutex;
#endif

/* thread arguments data structure definition */
typedef struct {
    int my_id; //thread id
} targs_t;

/* Function to be executed by each thread upon creation */
void* thread_fn(void *args)
{
    int i;
    targs_t *ta = (targs_t*)args;
    cpu_set_t mask;
    pthread_t me;
    int res;
    unsigned long it = 0;
    double in = 1.0, out;

    me = pthread_self();

    /* Each thread sets its own affinity. 
     * Specifically, thread i (i=0...nthreads-1), sets its 
     * affinity to cpu i. 
     */
    CPU_ZERO(&mask);
    CPU_SET(ta->my_id, &mask);
    res = pthread_setaffinity_np(me, sizeof(cpu_set_t), &mask);
    if (res) {
        fprintf(stderr, "Error in pthread_setaffinity_np.\n");
        exit(EXIT_FAILURE);
    }

#if DEBUG
    /* Make sure that affinity was set correctly */
    res = pthread_getaffinity_np(me, sizeof(cpu_set_t), &mask);
    if (res) {
        fprintf(stderr, "Error in pthread_getaffinity_np.\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&mutex);
    fprintf(stderr, "Thread %d started on cpus ", ta->my_id);
    for ( i = 0; i < CPU_SETSIZE; i++ )
        if (CPU_ISSET(i, &mask))
            fprintf(stderr, " %d", i);
    fprintf(stderr, "\n");
    pthread_mutex_unlock(&mutex);
#endif

    /* Begin of ROI: Thread "0" will start measurements at this point */
    pthread_barrier_wait(&bar);
    if (ta->my_id == 0)
#ifdef SNIPER
        SimRoiStart();
#elif defined REAL
        gettimeofday(&begin, NULL);
#endif
    pthread_barrier_wait(&bar);


    /* Execute critical section "iters" times */
    while (it++ < iters) {
        /* Critical section begin: acquire the lock */
#ifdef MUTEX
        pthread_mutex_lock(&mutex);
#elif defined TAS_CAS
        spin_lock_tas_cas(&lock);
#elif defined TTAS_CAS
        spin_lock_ttas_cas(&lock);
#elif defined TAS_TS
        spin_lock_tas_ts(&lock);
#elif defined TTAS_TS
        spin_lock_ttas_ts(&lock);
#endif

        /* do some dummy work just to spend time */
        out = dummy_work(in);
        val += out;

        /* Critical section end: release the lock */
#ifdef MUTEX
        pthread_mutex_unlock(&mutex);
#elif ( defined TAS_CAS || defined TTAS_CAS || defined TAS_TS || defined TTAS_TS ) 
        spin_unlock(&lock);
#endif
    }

    /* End of ROI: Thread "0" will stop measurements at this point */
    pthread_barrier_wait(&bar);
    if ( ta->my_id == 0 ) {
#ifdef SNIPER
        SimRoiEnd();
#elif defined(REAL)
        gettimeofday(&end, NULL);
#endif
    }
    pthread_barrier_wait(&bar);


#if DEBUG
    pthread_mutex_lock(&mutex);
    fprintf(stderr, "Thread %d stopped with dummy result: %lf\n",ta->my_id, out);
    pthread_mutex_unlock(&mutex);
#endif
    
    pthread_exit(NULL);
} 

int main(int argc, char **argv)
{
    targs_t *targs;
    pthread_t *tids;
    int i, res, nthreads;
    double elapsed;

    if ( argc < 4 ) {
       printf("Usage: ./prog <nthreads> <iterations> <grain>\n");
       exit(EXIT_FAILURE);
    }
  
    nthreads = atoi(argv[1]);
    iters = atol(argv[2]);
    grain = atol(argv[3]);

    /* allocate thread structures */
    tids = (pthread_t *)malloc_safe(nthreads * sizeof(pthread_t));
    targs = (targs_t *)malloc_safe(nthreads * sizeof(targs_t)); 

    /* initialize barrier */
    pthread_barrier_init(&bar, NULL, nthreads);

    /* initialize lock or mutex */
#if ( defined TAS_CAS || defined TTAS_CAS || defined TAS_TS || defined TTAS_TS ) 
    spin_lock_init(&lock);
#elif defined MUTEX
    pthread_mutex_init(&mutex, NULL);
#endif

    /* create and spawn threads */
    for ( i = 0; i < nthreads; i++ ) {
        targs[i].my_id = i;
        res = pthread_create(&tids[i], NULL, thread_fn, (void*)&targs[i]);
        if ( res ) {
            fprintf(stderr, "Error in pthread_create.\n");
            exit(EXIT_FAILURE);
        }
    }

    /* join threads */
    for ( i = 0; i < nthreads; i++ ) {
        pthread_join(tids[i], NULL);
    }

#ifdef REAL
    elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec)/1000000.0; 
    fprintf(stdout, "Execution time:%lf seconds\n", elapsed);
#endif
#ifdef DEBUG
    fprintf(stdout, "Val:%lf\n", val);
#endif

    pthread_barrier_destroy(&bar);

    free(tids);
    free(targs);
    
    return 0;
}


static void* malloc_safe(size_t size)
{
    void *p;
    
    if ( !(p = malloc(size)) ) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    return p;
}


static inline double dummy_work(double x)
{
    int i; 

    for ( i=0; i<grain; i++ ) {
        x = x * 2.0;
        x = x / 2.0;
    }

    return x;
}