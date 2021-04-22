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
int barrier = 0;

void sequential_copy(int num, unsigned long int *axi_bram_ctrl, unsigned long int *data)
{
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        off_t addr = 0x40000000 * (num + 1);
        axi_bram_ctrl = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
        data = (unsigned long int *)malloc(bram_size);

        // copy data to bram
        int p_size = 1024;
        for (int i = 0; i < bram_size / p_size; i++)
        {
            memcpy(axi_bram_ctrl + ((p_size / 4) * i), data + ((p_size / 4) * i), p_size);
        }

        close(fd);
    }
}
int main()
{
    auto start;
    auto stop;
    auto duration;
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

    return 0;
}