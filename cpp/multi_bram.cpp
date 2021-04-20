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
    int longs = bram_size / 4;
    for (unsigned long int i = 0; i < longs; i++)
    {
        data[0][i] = i;
    }

    // make copies to other data sources
    for (int i = 2; i < brams; i += 2)
    {
        memcpy(data[i], data[0], bram_size);
    }

    // copy data to bram
    for (int i = 0; i < brams; i += 2)
    {
        unsigned char *dest = (unsigned char *)axi_bram_ctrl[i];
        memcpy(dest, data[i], bram_size);
    }
    return NULL;
}
void *task1(void *arg)
{
    // set first data source
    int longs = bram_size / 4;
    for (unsigned long int i = 0; i < longs; i++)
    {
        data[1][i] = i;
    }

    // make copies to other data sources
    for (int i = 3; i < brams; i += 2)
    {
        memcpy(data[i], data[1], bram_size);
    }

    // copy data to bram
    for (int i = 1; i < brams; i += 2)
    {
        unsigned char *dest = (unsigned char *)axi_bram_ctrl[i];
        memcpy(dest, data[i], bram_size);
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
        pthread_attr_t attr0;
        pthread_attr_t attr1;
        cpu_set_t cpu0;
        cpu_set_t cpu1;
        sched_param params;
        params.sched_priority = 1000;
        pthread_attr_init(&attr0);
        pthread_attr_init(&attr1);
        CPU_ZERO(&cpu0);
        CPU_SET(0, &cpu0);
        CPU_ZERO(&cpu1);
        CPU_SET(1, &cpu1);
        pthread_attr_setaffinity_np(&attr0, sizeof(cpu_set_t), &cpu0);
        pthread_attr_setaffinity_np(&attr1, sizeof(cpu_set_t), &cpu1);
        pthread_attr_setschedparam(&attr0, &params);
        pthread_attr_setschedparam(&attr1, &params);
        pthread_create(&t0, &attr0, task0, NULL);
        pthread_create(&t1, &attr1, task1, NULL);
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

    for (int i = 2; i <= 16; i += 2)
    {
        auto start = high_resolution_clock::now();
        parallel_copy(i);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << dec << "Par time " << i << ": " << duration.count() << " us\n";
    }

    return 0;
}