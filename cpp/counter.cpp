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
long int *axi_gpio_ctrl;
long int *axi_gpio_cnt;
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
    axi_gpio_ctrl = map_mem(gpio_bytes, 0x41200000);
    axi_gpio_cnt = map_mem(gpio_bytes, 0x41210000);
}
int close_mem()
{
    close(fd);
}
int wait_clear()
{
    axi_gpio_ctrl[0] = 0b10;
    while (axi_gpio_cnt[0] > 0)
    {
    }
    axi_gpio_ctrl[0] = 0b0;
}
int start_work()
{
    axi_gpio_ctrl[0] = 0b1;
}
int main()
{
    // begin
    open_mem();
    auto start = high_resolution_clock::now();
    wait_clear();
    start_work();
    while (axi_gpio_cnt[0] > 1000000)
    {
        cout << "cnt: " << axi_gpio_cnt[0] << "\n";
    }
    wait_clear();
    start_work();
    while (axi_gpio_cnt[0] > 1000)
    {
        cout << "cnt: " << axi_gpio_cnt[0] << "\n";
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "time: " << duration.count() << " us\n";
    return 0;
}