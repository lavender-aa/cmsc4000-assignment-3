// ------------------- metrics -------------------

/*
    time(real) vs # threads
            1      2      3
   16:  44.00  44.34  44.68
    8:  46.21  46.31  46.42
    4:  84.67  84.88  84.44
    3: 111.90 111.64 111.80
    2: 163.39 163.41 163.32
    1: 326.70 326.45 326.66
*/


#include "util.h"


// extra globals for conditions and num buffer
pthread_mutex_t prod_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t num_avail = PTHREAD_COND_INITIALIZER;
pthread_cond_t num_taken = PTHREAD_COND_INITIALIZER;
u64 num = 0;
bool end_produced = false;
bool end_taken = false;


// ------------------- thread workers -------------------

// produces all integers START-END
void* producer_work(void* data) {
    u64 i;
    for(i=START; i<=END; i++) {
        pthread_mutex_lock(&prod_mutex);

        // wait for number to be taken by a consumer
        while(num != 0)
            pthread_cond_wait(&num_taken, &prod_mutex);

        // produce the next number (to the buffer)
        num = i;

        // allow a consumer to take the number
        pthread_cond_signal(&num_avail);
        pthread_mutex_unlock(&prod_mutex);
    }
    end_produced = true;
}

// take number-by-number to check if prime
void* consumer_work(void* data) {
    int thread_num = *((int*)data);
    u64 n;
    while(!end_taken) {

        // get the next number to check
        pthread_mutex_lock(&prod_mutex);

        // wait for next number to be available to take
        while(num == 0)
            pthread_cond_wait(&num_avail, &prod_mutex);

        // take it, reset buffer to 0
        n = num;
        num = 0;

        // if this was the last number produced, allow other consumer threads to exit
        if(end_produced) end_taken = true;

        // allow producer to produce another number
        pthread_cond_signal(&num_taken);
        pthread_mutex_unlock(&prod_mutex);

        // if the number taken is prime, add it to the list
        // (and increment total number of primes)
        if(is_prime(n)) {
            primes[thread_num][thread_num_found[thread_num]++] = n;
            printf("thread %d found prime: %lld\n", thread_num, n);

            pthread_mutex_lock(&data_mutex);
            num_primes++;
            pthread_mutex_unlock(&data_mutex);
        }
    }
    printf("thread %d finished\n", thread_num);
}


// ------------------- main -------------------

int main() {
    // init output file
    output = fopen("output.txt", "w");

    pthread_t producer;

    // start clock (benchmarking)
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // create all threads (1 producer, n consumers)
    pthread_create(&producer, NULL, producer_work, NULL);
    for(int i=0; i<NUM_THREADS; i++) {
        int* data = (int*) malloc(sizeof(int));
        *data = i;
        pthread_create(&(threads[i]), NULL, consumer_work, data);
    }

    // join all threads (1 producer, n consumers)
    pthread_join(producer, NULL);
    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // stop clock (benchmarking)
    gettimeofday(&end, NULL);
    real_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)*1e-6;

    // write data to output file
    print_results();
    fclose(output);
}