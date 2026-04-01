// ------------------- metrics -------------------

/*
    time(real) vs # threads
            1      2      3      4      5      6      7      8      9     10
   16:  52.90  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx
    8:  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx
    4:  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx  xx.xx
    3: xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx
    2: xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx
    1: xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx xxx.xx
*/

/*
     number of primes found by each thread (in order; 0-n)
    16: 6 8 2 9
        8 7 7 9
        6 8 2 3
        10 9 6 6

    8: x x x x
       x x x x

    4: x x x x

    3: x x x

    2: x x

    1: 106
*/


#include "util.h"


// ------------------- thread worker -------------------

void* thread_work(void* data) {
    int thread_num = *((int*)data);
    free(data);

    thread_num_found[thread_num] = 0;

    // narrow search range by thread number
    u64 start = START + thread_num * BLOCK_SIZE;
    u64 end = start + BLOCK_SIZE - 1;

    // if last thread, make sure it checks until the last value
    if(thread_num + 1 == NUM_THREADS) end = END;

    // iterate over numbers in range
    for(u64 i = start; i <= end; i++) {

        if(isPrime(i)) {
            // data mutex for shared resource (num_primes)
            pthread_mutex_lock(&data_mutex);
            num_primes++;
            pthread_mutex_unlock(&data_mutex);

            // modify lists (no mutex needed; all lists separate)
            // (increments num_found at same time)
            primes[thread_num][thread_num_found[thread_num]++] = i;
        }
    }

    // print number of primes found by this thread
    printf("Thread %d done\n", thread_num);

    // not necessary, but can uncomment:
    pthread_exit(NULL);
}


// ------------------- main -------------------

int main() {

    // open output file for writing
    output = fopen("output.txt", "w");

    // start clock (benchmarking)
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // initialize threads
    for(int i=0; i<NUM_THREADS; i++) {
        int* data = (int*) malloc(sizeof(int));
        *data = i;
        pthread_create(&(threads[i]), NULL, thread_work, data);
    }

    // join threads
    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // stop clock (benchmarking)
    gettimeofday(&end, NULL);
    real_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)*1e-6;

    // write results to output file then close
    print_results();
    fclose(output);

    return 0;
}