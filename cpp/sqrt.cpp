#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
using namespace std;
using namespace std::chrono;
int main()
{
    auto start = high_resolution_clock::now();
    for (long int i = 0; i < 100000; i++)
    {
        long int r = i + i;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Add: " << duration.count() << " us\n";

    start = high_resolution_clock::now();
    for (long int i = 0; i < 100000; i++)
    {
        long int r = i - i;
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Sub: " << duration.count() << " us\n";

    start = high_resolution_clock::now();
    for (long int i = 0; i < 100000; i++)
    {
        long int r = i * i;
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Mul: " << duration.count() << " us\n";

    start = high_resolution_clock::now();
    for (long int i = 0; i < 100000; i++)
    {
        long int r = i / i;
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Div: " << duration.count() << " us\n";

    start = high_resolution_clock::now();
    for (long int i = 0; i < 100000; i++)
    {
        long int r = sqrt((double)i);
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Sqr: " << duration.count() << " us\n";
}