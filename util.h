// header file to contain all the shared portions of each program;
// essentially everything except main() and the thread worker function(s)

#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

// control parameters
#define NUM_THREADS 16
#define PRIMES_PER_LINE 5


// shorthands
#define u64 uint64_t


// constants
#define START 1777770000
#define END   1777771999
#define BLOCK_SIZE (int) (END - START + 1)/NUM_THREADS


// globals: list of threads, list of primes, number of primes
pthread_t threads[NUM_THREADS];
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
u64 primes[NUM_THREADS][BLOCK_SIZE/2] = {0};
int thread_num_found[NUM_THREADS] = {0};
int num_primes = 0;
double real_time;


// output file (number of primes found by each thread)
FILE* output;


// helpers

// this had to be changed; the original returned
// that `1` is prime (it is not). 
bool isPrime(u64 n) {
    u64 k, limit;

    // guard clauses
    if(n <= 1) return false;
    if(n == 2) return true;

    // check factors until n/2
    limit = n/2;
    for(k = 2; k <= limit; k++) {
        if(n % k == 0) return false;
    }

    return true;
}


void print_results() {
    fprintf(output, "Total number of primes found: %d\n", num_primes);
    fprintf(output, "Number of threads used: %d\n", NUM_THREADS);
    fprintf(output, "Total time taken: %.2fs\n\n", real_time);

    // thread info (in order)
    for(int i=0; i<NUM_THREADS; i++) {
        u64 start = START + i * BLOCK_SIZE;
        u64 end = start + BLOCK_SIZE - 1;
        fprintf(output, "Thread %d: %d primes found in range [%llu, %llu] (%llu checked)\n\t",
                    i, thread_num_found[i], start, end, end-start);
            
        // print primes found by that thread
        for(int j=0; j<BLOCK_SIZE/2; j++) {
            u64 prime = primes[i][j];
            if(prime == 0) break;
            fprintf(output, "%lld ", prime);
            if(j+1 % PRIMES_PER_LINE == 0) fprintf(output, "\n\t");
        }

        fprintf(output, "\n\n");
    }
}

#endif