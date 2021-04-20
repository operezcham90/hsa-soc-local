// g++ hello.cpp -fopenmp -o hello
#include <iostream>
#include <sched.h>
#include <omp.h>

int done = 0;

int main()
{
#pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int cpu_num = sched_getcpu();

        while (cpu_num != done)
        {
        }
        std::cout << thread_num << " running in " << cpu_num << std::endl;
        done++;
    }

    return 0;
}