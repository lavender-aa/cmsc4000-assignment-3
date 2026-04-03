// ------------------- metrics -------------------

/*
    time(real) vs # threads
            1      2      3
   16:  47.83  49.43  49.07
    8:  59.30  61.24  59.63
    4:  95.60  95.44  95.63
    3: 110.55 110.53 110.42
    2: 170.71 170.63 170.68
    1: 326.27 327.12 326.11
*/

/*
     number of primes found by each thread (in order; 0-n)
    16: 6 8 2 9
        8 7 7 9
        6 8 2 3
        10 9 6 6

    8: 14 11 15 16
       14  5 19 12

    4: 25 31 19 31

    3: 35 36 35

    2: 56 50

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

        if(is_prime(i)) {
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