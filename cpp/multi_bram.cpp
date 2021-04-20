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
unsigned int bram_size = 2048 * 4;

void *task1(void *arg)
{
    memcpy(axi_bram_ctrl_0, data, bram_size);
    return NULL;
}
void *task2(void *arg)
{
    memcpy(axi_bram_ctrl_1, data, bram_size);
    return NULL;
}
void *task3(void *arg)
{
    memcpy(axi_bram_ctrl_2, data, bram_size);
    return NULL;
}
void *task4(void *arg)
{
    memcpy(axi_bram_ctrl_3, data, bram_size);
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
        for (unsigned long int i = 0; i < bram_size / 4; i++)
        {
            data[i] = i;
        }

        auto start = high_resolution_clock::now();
        pthread_t a1;
        pthread_create(&a1, NULL, task1, NULL);
        pthread_join(a1, NULL);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Write 1 BRAM: " << duration.count() << " us\n";

        close(fd);
    }
    return 0;
}