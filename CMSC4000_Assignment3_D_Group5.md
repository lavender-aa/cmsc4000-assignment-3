# Part A
## Instructions
Write a multithreaded C program to find and print all prime numbers between 1,777,770,000 and 1,777,771,999. 

The range must be divided equally among threads, where each thread will work to find all prime numbers in its assigned part. 

Use the function below to determine whether a number is prime or not:
```C
int is_prime(unsigned long long n) {
    unsigned long long k, limit;
    
    if(n == 2) return 1;
    if(n%2 == 0) return 0;
    
    limit = n/2;
    for(k=3; k<=limit; k++) {
        if(n%k == 0) return 0;
    }
    return 1;
}
```

Measure how long your program will take to find all prime numbers using 1, 2, 3, 4, 8, and 16 threads. 

Additionally, count how many prime numbers are found by each thread and their count in its assigned range part. 

Name your program CMSC4000_Assignment3_A_Group\#.c where \# is your group number.
## Modifications
First, this function for determining prime-ness is incorrect; `is_prime(1)` returns `1` (true), despite 1 not being a prime number. The following code, equivalent in runtime complexity, was used:
```c
#include <stdbool.h>
#include <stdint.h>
#define u64 uint64_t

bool is_prime(u64 n) {
    u64 k, limit;
    
    if(n == 1) return false;
    if(n == 2) return true;
    
    limit = n/2;
    for(k=2, k<=limit; k++) {
        if(n%k == 0) return false;
    }
    return true;
}
```
The following has been modified:
- Integer types representing booleans have been converted into the `bool` type (for clarity)
- Variables of type `unsigned long long` have been converted to an alias of the `uint64_t` type (to fix their size across platforms)
- The guard clauses have been reworked to correct the mistake of returning `true` when `n=1`
- The limit check begins with `k=2` instead of `k=3` (result of fixed guard clauses)
## Results and Analysis
Below are two graphs:
- Figure A.1: number of threads vs average runtime (over 10 runs)
- Figure A.2: number of threads vs thread number number of primes found (constant across runs).
  
\[figures A.1, A.2]

Given that the latter graph is 3d, its individual slices can be viewed in [[#Appendix A]]. Additionally, all graphs used in this report are available [online(TODO: link)].

# Part B
## Instructions
Change the code in Figure 2-33 in the textbook such that the producer produces all the integer numbers between 1,777,770,000 and 1,777,771,999 while the consumer will work in a loop to take number by number to cehck them an only prints the prime numbers.

Generate the same metrics as in part A (using the same range of threads). Compare the execution times with those from part A.

## Results

## Comparison with Part A

# Part C
## Instructions
Change the program from part B to have a 2-item buffer instead of a 1-item buffer. Take the same measurements as in part B, but exclude the 1-thread case, and compare the execution times with those from part B. 

## Results

## Comparison with Part B

***
# Appendix A
All graphs shown are available in [thisTODO:link] desmos folder.
## Figure A.2 Slices
\[slices here; 6 total]