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
unsigned int bram_bytes = 2048 * 4;
long int *limit;
long int *control;
long int *counter;
long int *accumulated;
long int *version;
long int *average;
unsigned char *img1_bram;
int fd;
// open memory
long int *map_mem(unsigned int bytes, off_t addr)
{
    return (long int *)mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
}
int send_img()
{
    for (int i = 0; i < bram_bytes; i++)
    {
        if (i % 2 == 0)
        {
            img1_bram[i] = 1;
        }
        else
        {
            img1_bram[i] = 3;
        }
    }
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
    average = map_mem(gpio_bytes, 0x41250000);
    img1_bram = (unsigned char *)map_mem(bram_bytes, 0x40000000);
}
int close_mem()
{
    close(fd);
}
int print_version()
{
    cout << "PL VER " << version[0] << "\n";
}
int set_limit(long int lim)
{
    limit[0] = lim;
    cout << "limit " << limit[0] << "\n";
}
int set_average(long int avg)
{
    average[0] = avg;
    cout << "average " << average[0] << "\n";
}
int wait_clear()
{
    control[0] = 0b10;
    while (counter[0] + accumulated[0] > 0)
    {
    }
    control[0] = 0b0;
}
int start_work(int square)
{
    if (square)
    {
        control[0] = 0b101;
    }
    else
    {
        control[0] = 0b1;
    }
}
int wait_work()
{
    while (counter[0] < limit[0])
    {
    }
    control[0] = 0b0;
}
int print_result()
{
    cout << "count " << counter[0] << "\n";
    cout << "acc " << accumulated[0] << "\n";
}
int main()
{
    // begin
    open_mem();
    print_version();
    auto start = high_resolution_clock::now();
    // send work data
    send_img();
    set_limit(1000);
    set_average(0);
    // work
    wait_clear();
    start_work(0);
    wait_work();
    print_result();
    // send work data
    set_limit(1000);
    set_average(2);
    // work
    wait_clear();
    start_work(1);
    wait_work();
    print_result();
    // end
    close_mem();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "time: " << duration.count() << " us\n";
    return 0;
}