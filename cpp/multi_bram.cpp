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
#include <thread>

using namespace std;
using namespace std::chrono;

// addresses from vivado block design
unsigned long int *axi_bram_ctrl_0;
unsigned long int *axi_bram_ctrl_1;
unsigned long int *axi_bram_ctrl_2;
unsigned long int *axi_bram_ctrl_3;
unsigned long int *data;
unsigned int bram_size = 2048 * 4;

void task1()
{
    memcpy(axi_bram_ctrl_0, data, bram_size);
}
void task2()
{
    memcpy(axi_bram_ctrl_1, data, bram_size);
}
void task3()
{
    memcpy(axi_bram_ctrl_2, data, bram_size);
}
void task4()
{
    memcpy(axi_bram_ctrl_3, data, bram_size);
}
int main()
{
    unsigned int c = thread::hardware_concurrency();
    cout << "number of cores: " << c << "\n";

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
        thread t1(task1);
        t1.join();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Write 1 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        thread t11(task1);
        thread t21(task2);
        t11.join();
        t21.join();
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 2 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        thread t12(task1);
        thread t22(task2);
        thread t32(task3);
        t12.join();
        t22.join();
        t32.join();
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 3 BRAM: " << duration.count() << " us\n";

        start = high_resolution_clock::now();
        thread t13(task1);
        thread t23(task2);
        thread t33(task3);
        thread t43(task4);
        t13.join();
        t23.join();
        t33.join();
        t43.join();
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Write 4 BRAM: " << duration.count() << " us\n";

        close(fd);
    }
}