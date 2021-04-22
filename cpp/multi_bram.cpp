// g++ -fopenmp multi_bram.cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <math.h>
#include <omp.h>

using namespace std;
using namespace std::chrono;

// addresses from vivado block design
unsigned int bram_size = 2048 * 4;

void sequential_copy(int num, unsigned long int *axi_bram_ctrl, unsigned long int *data)
{
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        off_t addr;
        if (num == 0)
            addr = 0x40000000;
        if (num == 1)
            addr = 0x80000000;
        if (num == 2)
            addr = 0x42000000;
        if (num == 3)
            addr = 0x82000000;
        axi_bram_ctrl = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
        data = (unsigned long int *)malloc(bram_size);

        // copy data to bram
        memcpy(axi_bram_ctrl, data, bram_size);

        close(fd);
    }
}
int main()
{
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    // sequential single channel
    start = high_resolution_clock::now();
    for (int i = 0; i < 128; i++)
    {
        unsigned long int *axi_bram_ctrl_0;
        unsigned long int *data_0;
        sequential_copy(0, axi_bram_ctrl_0, data_0);
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Seq 1 time: " << duration.count() << " us\n";

    // sequential dual channel
    start = high_resolution_clock::now();
    for (int i = 0; i < 128; i++)
    {
        unsigned long int *axi_bram_ctrl_0;
        unsigned long int *data_0;
        sequential_copy(i % 2, axi_bram_ctrl_0, data_0);
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Seq 2 time: " << duration.count() << " us\n";

    // parallel single channel
    start = high_resolution_clock::now();
#pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        for (int i = thread_num; i < 128; i += 2)
        {
            unsigned long int *axi_bram_ctrl_1;
            unsigned long int *data_1;
            sequential_copy(0, axi_bram_ctrl_1, data_1);
        }
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Par 1 time: " << duration.count() << " us\n";

    // parallel dual channel
    start = high_resolution_clock::now();
#pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        for (int i = thread_num; i < 128; i += 2)
        {
            unsigned long int *axi_bram_ctrl_1;
            unsigned long int *data_1;
            sequential_copy(thread_num, axi_bram_ctrl_1, data_1);
        }
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Par 2 time: " << duration.count() << " us\n";

    // parallel 4 channel
    start = high_resolution_clock::now();
#pragma omp parallel num_threads(4)
    {
        int thread_num = omp_get_thread_num();
        for (int i = thread_num; i < 128; i += 4)
        {
            unsigned long int *axi_bram_ctrl_1;
            unsigned long int *data_1;
            sequential_copy(thread_num, axi_bram_ctrl_1, data_1);
        }
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Par 3 time: " << duration.count() << " us\n";

    return 0;
}