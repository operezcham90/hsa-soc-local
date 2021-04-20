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

/*void *task1(void *arg)
{
    memcpy(axi_bram_ctrl_0_bytes, data_bytes, half_bram_size);
    memcpy(axi_bram_ctrl_2_bytes, data3_bytes, half_bram_size);
    memcpy(axi_bram_ctrl_1_bytes, data2_bytes, half_bram_size);
    memcpy(axi_bram_ctrl_3_bytes, data4_bytes, half_bram_size);
    return NULL;
}
void *task2(void *arg)
{
    memcpy(axi_bram_ctrl_1_bytes + half_bram_size, data2_bytes + half_bram_size, half_bram_size);
    memcpy(axi_bram_ctrl_3_bytes + half_bram_size, data4_bytes + half_bram_size, half_bram_size);
    memcpy(axi_bram_ctrl_0_bytes + half_bram_size, data_bytes + half_bram_size, half_bram_size);
    memcpy(axi_bram_ctrl_2_bytes + half_bram_size, data3_bytes + half_bram_size, half_bram_size);
    return NULL;
}
void parallel_copy(int num)
{
    pthread_t a1;

    pthread_t b1;
    pthread_t b2;

    pthread_t c1;
    pthread_t c2;
    pthread_t c3;

    pthread_t d1;
    pthread_t d2;
    pthread_t d3;
    pthread_t d4;

    pthread_attr_t attr0;
    pthread_attr_t attr1;
    cpu_set_t cpus0;
    cpu_set_t cpus1;
    sched_param params;
    params.sched_priority = 5;
    pthread_attr_init(&attr0);
    pthread_attr_init(&attr1);
    CPU_ZERO(&cpus0);
    CPU_ZERO(&cpus1);
    CPU_SET(0, &cpus0);
    CPU_SET(1, &cpus1);
    pthread_attr_setaffinity_np(&attr0, sizeof(cpu_set_t), &cpus0);
    pthread_attr_setaffinity_np(&attr1, sizeof(cpu_set_t), &cpus1);
    pthread_attr_setschedparam(&attr0, &params);
    pthread_attr_setschedparam(&attr1, &params);

    start = high_resolution_clock::now();
    pthread_create(&a1, &attr0, task1, NULL);
    pthread_join(a1, NULL);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Write 1 BRAM: " << duration.count() << " us\n";

    start = high_resolution_clock::now();
    pthread_create(&b1, &attr0, task1, NULL);
    pthread_create(&b2, &attr1, task2, NULL);
    pthread_join(b1, NULL);
    pthread_join(b2, NULL);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Write 2 BRAM: " << duration.count() << " us\n";
}*/
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
            cout << "BRAM at ";
            cout << hex << addr;
            cout << "ready\n";
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

    return 0;
}