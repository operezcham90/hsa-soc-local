// g++ -lpthread multi_bram.cpp
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
#include <pthread.h>

using namespace std;
using namespace std::chrono;

// addresses from vivado block design
unsigned long int **axi_bram_ctrl;
unsigned long int **data;
unsigned int bram_size = 2048 * 4;
unsigned int half_bram_size = 2048 * 2;
int brams = 0;

void *task0(void *arg)
{
    // set first data source
    int longs = half_bram_size / 4;
    for (unsigned long int i = 0; i < longs; i++)
    {
        data[0][i] = i;
    }

    // make copies to other data sources
    for (int i = 1; i < brams; i++)
    {
        memcpy(data[i], data[0], half_bram_size);
    }

    // copy data to bram
    for (int i = 0; i < brams; i++)
    {
        unsigned char *dest = (unsigned char *)axi_bram_ctrl[i];
        memcpy(dest, data[i], half_bram_size);
    }
    return NULL;
}
void *task1(void *arg)
{
    // set first data source
    int longs = half_bram_size / 4;
    for (unsigned long int i = 0; i < longs; i++)
    {
        data[0][i + longs] = i + longs;
    }

    // make copies to other data sources
    for (int i = 1; i < brams; i++)
    {
        unsigned char *dest = (unsigned char *)data[i];
        unsigned char *src = (unsigned char *)data[0];
        memcpy(dest + half_bram_size, src + half_bram_size, half_bram_size);
    }

    // copy data to bram
    for (int i = 0; i < brams; i++)
    {
        unsigned char *dest = (unsigned char *)axi_bram_ctrl[i];
        unsigned char *src = (unsigned char *)data[i];
        memcpy(dest + half_bram_size, src + half_bram_size, half_bram_size);
    }
    return NULL;
}
void parallel_copy(int num)
{
    brams = num;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        // alloc pointers to brams
        axi_bram_ctrl = (unsigned long int **)malloc(brams * sizeof(unsigned long int *));

        // alloc pointers to data sources
        data = (unsigned long int **)malloc(brams * sizeof(unsigned long int *));

        // map brams
        for (int i = 0; i < brams; i++)
        {
            off_t addr = 0x40000000 + 0x2000000 * i;
            axi_bram_ctrl[i] = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
            /*cout << "BRAM at ";
            cout << hex << addr;
            cout << " ready\n";*/
            data[i] = (unsigned long int *)malloc(bram_size);
        }

        pthread_t t0;
        pthread_t t1;
        pthread_attr_t attr;
        cpu_set_t cpus;
        sched_param params;
        params.sched_priority = 1000;
        pthread_attr_init(&attr);
        CPU_ZERO(&cpus);
        CPU_SET(0, &cpus);
        CPU_SET(1, &cpus);
        pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
        pthread_attr_setschedparam(&attr, &params);
        pthread_create(&t0, &attr, task0, NULL);
        pthread_create(&t1, &attr, task1, NULL);
        pthread_join(t0, NULL);
        pthread_join(t1, NULL);

        close(fd);
    }
}
void sequential_copy(int num)
{
    brams = num;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        // alloc pointers to brams
        axi_bram_ctrl = (unsigned long int **)malloc(brams * sizeof(unsigned long int *));

        // alloc pointers to data sources
        data = (unsigned long int **)malloc(brams * sizeof(unsigned long int *));

        // map brams
        for (int i = 0; i < brams; i++)
        {
            off_t addr = 0x40000000 + 0x2000000 * i;
            axi_bram_ctrl[i] = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
            /*cout << "BRAM at ";
            cout << hex << addr;
            cout << " ready\n";*/
            data[i] = (unsigned long int *)malloc(bram_size);
        }

        // set first data source
        for (unsigned long int i = 0; i < bram_size / 4; i++)
        {
            data[0][i] = i;
        }

        // make copies to other data sources
        for (int i = 1; i < brams; i++)
        {
            memcpy(data[i], data[0], bram_size);
        }

        // copy data to bram
        for (int i = 0; i < brams; i++)
        {
            memcpy(axi_bram_ctrl[i], data[i], bram_size);
        }

        close(fd);
    }
}
int main()
{
    for (int i = 1; i <= 16; i++)
    {
        auto start = high_resolution_clock::now();
        sequential_copy(i);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << dec << "Seq time " << i << ": " << duration.count() << " us\n";
    }

    for (int i = 1; i <= 16; i++)
    {
        auto start = high_resolution_clock::now();
        parallel_copy(i);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << dec << "Par time " << i << ": " << duration.count() << " us\n";
    }

    return 0;
}