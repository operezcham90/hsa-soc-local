#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
using namespace std;
using namespace std::chrono;
// AXI pointers
unsigned int gpio_bytes = 4;
long int *limit;
long int *control;
long int *counter;
long int *accumulated;
long int *version;
int fd;
// open memory
long int *map_mem(unsigned int bytes, off_t addr)
{
    return (long int *)mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
}
int open_mem()
{
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        exit(1);
    }
    limit = map_mem(gpio_bytes, 0x41200000);
    control = map_mem(gpio_bytes, 0x41210000);
    counter = map_mem(gpio_bytes, 0x41220000);
    accumulated = map_mem(gpio_bytes, 0x41230000);
    version = map_mem(gpio_bytes, 0x41240000);
}
int close_mem()
{
    close(fd);
}
int main()
{
    open_mem();
    long int none = 0b0;
    long int work = 0b1;
    long int clear = 0b10;
    long int square = 0b100;

    cout << "HW VER " << version[0] << "\n";

    for (long int i = 1; i < 5; i++)
    {
        limit[0] = i;
        cout << "*Limit " << limit[0] << "\n";
        control[0] = clear;
        control[0] = none;
        control[0] = work;
        while (counter[0] < limit[0])
        {
        }
        control[0] = none;
        cout << "Counter " << counter[0] << "\n";
        cout << "Accumulated " << accumulated[0] << "\n";
    }

    close_mem();
    return 0;
}