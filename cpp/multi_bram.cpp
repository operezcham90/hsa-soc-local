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
unsigned long int *axi_bram_ctrl_0;
unsigned long int *axi_bram_ctrl_1;
unsigned long int *axi_bram_ctrl_2;
unsigned long int *axi_bram_ctrl_3;
unsigned long int *data;
unsigned long int *data2;
unsigned long int *data3;
unsigned long int *data4;
unsigned int bram_size = 2048 * 4;
unsigned int half_bram_size = 2048 * 2;

void *task1(void *arg)
{
    memcpy(axi_bram_ctrl_0, data, half_bram_size);
    memcpy(axi_bram_ctrl_2, data3, half_bram_size);
    memcpy(axi_bram_ctrl_1, data2, half_bram_size);
    memcpy(axi_bram_ctrl_3, data4, half_bram_size);
    return NULL;
}
void *task2(void *arg)
{
    memcpy(axi_bram_ctrl_1 + half_bram_size, data2 + half_bram_size, half_bram_size);
    memcpy(axi_bram_ctrl_3 + half_bram_size, data4 + half_bram_size, half_bram_size);
    memcpy(axi_bram_ctrl_0 + half_bram_size, data + half_bram_size, half_bram_size);
    memcpy(axi_bram_ctrl_2 + half_bram_size, data3 + half_bram_size, half_bram_size);
    return NULL;
}
void *task3(void *arg)
{

    return NULL;
}
void *task4(void *arg)
{

    return NULL;
}
int main()
{
    // size of bram default block
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_bram_ctrl_0 = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x40000000);
        axi_bram_ctrl_1 = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x42000000);
        axi_bram_ctrl_2 = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x44000000);
        axi_bram_ctrl_3 = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x46000000);

        data = (unsigned long int *)malloc(bram_size);
        data2 = (unsigned long int *)malloc(bram_size);
        data3 = (unsigned long int *)malloc(bram_size);
        data4 = (unsigned long int *)malloc(bram_size);
        for (unsigned long int i = 0; i < bram_size / 4; i++)
        {
            data[i] = i;
        }

        auto start = high_resolution_clock::now();
        memcpy(data2, data, bram_size);
        memcpy(data3, data, bram_size);
        memcpy(data4, data, bram_size);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Preparation: " << duration.count() << " us\n";

        unsigned char *axi_bram_ctrl_0_bytes = (unsigned char *)axi_bram_ctrl_0;
        unsigned char *data_bytes = (unsigned char *)data;
        unsigned char *axi_bram_ctrl_1_bytes = (unsigned char *)axi_bram_ctrl_1;
        unsigned char *data2_bytes = (unsigned char *)data2;
        unsigned char *axi_bram_ctrl_2_bytes = (unsigned char *)axi_bram_ctrl_2;
        unsigned char *data3_bytes = (unsigned char *)data3;
        unsigned char *axi_bram_ctrl_3_bytes = (unsigned char *)axi_bram_ctrl_3;
        unsigned char *data4_bytes = (unsigned char *)data4;

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

        /*start = high_resolution_clock::now();
        int pieces = 128;
        int step = bram_size / pieces;
        int pos = 0;
        for (int i = 0; i < pieces; i++)
        {
            memcpy(axi_bram_ctrl_0_bytes + pos, data_bytes + pos, step);
            pos += step;
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 1 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        pos = 0;
        for (int i = 0; i < pieces; i++)
        {
            memcpy(axi_bram_ctrl_0_bytes + pos, data_bytes + pos, step);
            memcpy(axi_bram_ctrl_1_bytes + pos, data2_bytes + pos, step);
            pos += step;
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 2 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        pos = 0;
        for (int i = 0; i < pieces; i++)
        {
            memcpy(axi_bram_ctrl_0_bytes + pos, data_bytes + pos, step);
            memcpy(axi_bram_ctrl_1_bytes + pos, data2_bytes + pos, step);
            memcpy(axi_bram_ctrl_2_bytes + pos, data3_bytes + pos, step);
            pos += step;
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 3 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        pos = 0;
        for (int i = 0; i < pieces; i++)
        {
            memcpy(axi_bram_ctrl_0_bytes + pos, data_bytes + pos, step);
            memcpy(axi_bram_ctrl_1_bytes + pos, data2_bytes + pos, step);
            memcpy(axi_bram_ctrl_2_bytes + pos, data3_bytes + pos, step);
            memcpy(axi_bram_ctrl_3_bytes + pos, data4_bytes + pos, step);
            pos += step;
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 4 BRAM: " << duration.count() << " us\n";*/

        close(fd);
    }
    return 0;
}