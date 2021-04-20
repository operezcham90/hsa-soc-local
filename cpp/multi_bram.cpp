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
unsigned long int *axi_bram_ctrl;
unsigned long int *data;
unsigned int bram_size = 2048 * 4;
int brams = 16;
int barrier = 0;

void sequential_copy(int num)
{
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        off_t addr = 0x40000000 + 0x2000000 * num;
        axi_bram_ctrl = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
        data = (unsigned long int *)malloc(bram_size);

        // set data source
        for (unsigned long int i = 0; i < bram_size / 4; i++)
        {
            data[i] = i;
        }

        // copy data to bram
        memcpy(axi_bram_ctrl, data, bram_size);

        close(fd);
    }
}
int main()
{
#pragma omp parallel
    {
        int thread_num = omp_get_thread_num();

        if (thread_num == 0)
        {
            for (int i = 1; i <= 16; i++)
            {
                auto start = high_resolution_clock::now();
                sequential_copy(i);
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(stop - start);
                cout << dec << "Seq time " << i << ": " << duration.count() << " us\n";
            }
            barrier++;
        }
        while (thread_num != barrier)
        {
        }
        barrier++;
    }

    /*for (int i = 2; i <= 16; i += 2)
    {
        auto start = high_resolution_clock::now();
        parallel_copy(i);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << dec << "Par time " << i << ": " << duration.count() << " us\n";
    }*/

    return 0;
}