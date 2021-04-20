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
int brams = 16;
int barrier = 0;

void sequential_copy(int num, unsigned long int *axi_bram_ctrl, unsigned long int *data)
{
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        off_t addr = 0x40000000 + 0x2000000 * num;
        axi_bram_ctrl = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
        data = (unsigned long int *)malloc(bram_size);

        // copy data to bram
        memcpy(axi_bram_ctrl, data, bram_size);

        close(fd);
    }
}
int main()
{
    // sequential
    unsigned long int *axi_bram_ctrl_0;
    unsigned long int *data_0;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < 16; i++)
    {
        sequential_copy(i, axi_bram_ctrl_0, data_0);
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Seq time: " << duration.count() << " us\n";

    // parallel
    start = high_resolution_clock::now();
#pragma omp parallel
    {
        unsigned long int *axi_bram_ctrl_1;
        unsigned long int *data_1;
        int thread_num = omp_get_thread_num();
        for (int i = thread_num; i < 16; i += 2)
        {
            sequential_copy(i, axi_bram_ctrl_1, data_1);
        }
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Par time: " << duration.count() << " us\n";

    return 0;
}