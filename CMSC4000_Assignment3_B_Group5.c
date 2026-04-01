// ------------------- metrics -------------------

/*
    time(real) vs # threads
            1      2      3      4      5      6      7      8      9     10
   16:  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx
    8:  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx
    4:  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx
    3: xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx
    2: xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx
    1: xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx
*/

/*
     number of primes found by each thread (in order; 0-n)
     will be different each time.. TODO: what to do
    16: x x x x
        x x x x
        x x x x
        x x x x

    8: x x x x
       x x x x

    4: x x x x

    3: x x x

    2: x x

    1: 106
*/


// part B -- book figure 2.33
#include "util.h"


// extra globals for conditions and num buffer
pthread_cond_t num_avail = PTHREAD_COND_INITIALIZER;
pthread_cond_t num_taken = PTHREAD_COND_INITIALIZER;
u64 num = 0;


// ------------------- thread workers -------------------

/* how to use:

thread1:
    pthread_mutex_lock(&mutex);
    while (!condition)
        pthread_cond_wait(&cond, &mutex);
    // do something that requires holding the mutex and condition is true
    pthread_mutex_unlock(&mutex);

thread2:
    pthread_mutex_lock(&mutex);
    // do something that might make condition true
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

*/

// produces all integers START-END
void* producer_work(void* ptr) {
    u64 i;
    for(i=START; i<=END; i++) {
        pthread_mutex_lock(&data_mutex);
        while(num != 0)
            pthread_cond_wait(&num_taken, &data_mutex);
        num = i;
        pthread_cond_signal(&num_avail);
        pthread_mutex_unlock(&data_mutex);
    }

    pthread_exit(NULL);
}

// take number-by-number to check if prime
void* consumer_work(void* ptr) {
    // while last not taken:
        // if can take num, take
        // release data mutex
        // calculate if prime, act accordingly

    pthread_exit(NULL);
}


// ------------------- main -------------------

int main() {
    pthread_t producer;
    pthread_t consumers[NUM_THREADS];
    
    // create all threads (1 producer, n consumers)
    pthread_create(&producer, NULL, producer_work, NULL);
    for(int i=0; i<NUM_THREADS; i++) {
        int* data = (int*) malloc(sizeof(int));
        *data = i;
        pthread_create(&(consumers[i]), NULL, consumer_work, data);
    }

    // join all threads (1 producer, n consumers)
    pthread_join(producer, NULL);
    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}