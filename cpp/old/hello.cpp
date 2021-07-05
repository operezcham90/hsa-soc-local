// g++ hello.cpp -fopenmp -o hello
#include <iostream>
#include <sched.h>
#include <omp.h>

int barrier0 = 0;
int barrier1 = 0;
int barrier2 = 0;

int main()
{
    
#pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int cpu_num = sched_getcpu();
    }
    return 0;
}