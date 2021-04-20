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

void *task1(void *arg)
{
    memcpy(axi_bram_ctrl_0, data, bram_size);
    return NULL;
}
void *task2(void *arg)
{
    memcpy(axi_bram_ctrl_1, data2, bram_size);
    return NULL;
}
void *task3(void *arg)
{
    memcpy(axi_bram_ctrl_2, data3, bram_size);
    return NULL;
}
void *task4(void *arg)
{
    memcpy(axi_bram_ctrl_3, data4, bram_size);
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

        /*pthread_t a1;
        pthread_t b1;
        pthread_t b2;
        pthread_t c1;
        pthread_t c2;
        pthread_t c3;
        pthread_t d1;
        pthread_t d2;
        pthread_t d3;
        pthread_t d4;

        start = high_resolution_clock::now();
        pthread_create(&a1, NULL, task1, NULL);
        pthread_join(a1, NULL);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 1 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        pthread_create(&b1, NULL, task1, NULL);
        pthread_create(&b2, NULL, task2, NULL);
        pthread_join(b1, NULL);
        pthread_join(b2, NULL);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 2 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        pthread_create(&c1, NULL, task1, NULL);
        pthread_create(&c2, NULL, task2, NULL);
        pthread_create(&c3, NULL, task3, NULL);
        pthread_join(c1, NULL);
        pthread_join(c2, NULL);
        pthread_join(c3, NULL);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 3 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        pthread_create(&d1, NULL, task1, NULL);
        pthread_create(&d2, NULL, task2, NULL);
        pthread_create(&d3, NULL, task3, NULL);
        pthread_create(&d4, NULL, task4, NULL);
        pthread_join(d1, NULL);
        pthread_join(d2, NULL);
        pthread_join(d3, NULL);
        pthread_join(d4, NULL);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 4 BRAM: " << duration.count() << " us\n";*/

        start = high_resolution_clock::now();
        int step = bram_size / 128;
        for (int i = 0; i < 128; i++)
        {
            int pos = step * i;
            memcpy(axi_bram_ctrl_0 + pos, data + pos, step);
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 1 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        memcpy(axi_bram_ctrl_0, data, bram_size);
        memcpy(axi_bram_ctrl_1, data2, bram_size);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 2 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        memcpy(axi_bram_ctrl_0, data, bram_size);
        memcpy(axi_bram_ctrl_1, data2, bram_size);
        memcpy(axi_bram_ctrl_2, data3, bram_size);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 3 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        memcpy(axi_bram_ctrl_0, data, bram_size);
        memcpy(axi_bram_ctrl_1, data2, bram_size);
        memcpy(axi_bram_ctrl_2, data3, bram_size);
        memcpy(axi_bram_ctrl_3, data4, bram_size);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 4 BRAM: " << duration.count() << " us\n";

        close(fd);
    }
    return 0;
}