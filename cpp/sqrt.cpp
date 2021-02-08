#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <chrono>
int main()
{
    auto start = high_resolution_clock::now();
    for (long int i = 0; i < 1000; i++)
    {
        long int r = i + i;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Add: " << duration.count() << " us\n";
}