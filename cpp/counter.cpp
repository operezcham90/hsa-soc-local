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
long int *lim;
long int *ctrl;
long int *cnt;
long int *acc;
long int *ver;
long int *avg;
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
        img1_bram[i] = i % 0x100;
    }
}
int open_mem()
{
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        exit(1);
    }
    lim = map_mem(gpio_bytes, 0x41200000);
    ctrl = map_mem(gpio_bytes, 0x41210000);
    cnt = map_mem(gpio_bytes, 0x41220000);
    acc = map_mem(gpio_bytes, 0x41230000);
    ver = map_mem(gpio_bytes, 0x41240000);
    avg = map_mem(gpio_bytes, 0x41250000);
    img1_bram = (unsigned char *)map_mem(bram_bytes, 0x40000000);
}
int close_mem()
{
    close(fd);
}
int print_version()
{
    cout << "ver " << ver[0] << "\n";
}
int set_limit(long int limit)
{
    lim[0] = limit;
    cout << "lim " << lim[0] << "\n";
}
int set_average(long int average)
{
    avg[0] = average;
    cout << "avg " << avg[0] << "\n";
}
int wait_clear()
{
    ctrl[0] = 0b10;
    while (cnt[0] | acc[0] > 0)
    {
    }
    ctrl[0] = 0b0;
}
int start_work(int square)
{
    if (square)
    {
        ctrl[0] = 0b101;
    }
    else
    {
        ctrl[0] = 0b1;
    }
}
int wait_work()
{
    while (cnt[0] < lim[0])
    {
    }
    ctrl[0] = 0b0;
}
int print_result()
{
    cout << "cnt " << cnt[0] << "\n";
    cout << "acc " << acc[0] << "\n";
}
int main()
{
    // begin
    open_mem();
    print_version();
    send_img();
    auto start = high_resolution_clock::now();
    // send work data
    set_limit(5);
    // work
    wait_clear();
    start_work(0);
    wait_work();
    print_result();
    // send work data
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