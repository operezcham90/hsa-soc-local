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
long int *axi_gpio_ver;
long int *axi_gpio_lim;
long int *axi_bram_ctrl_0;
long int *axi_gpio_res;
long int *axi_gpio_avg;
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
    axi_gpio_ctrl = map_mem(gpio_bytes, 0x41210000);
    axi_gpio_cnt = map_mem(gpio_bytes, 0x41230000);
    axi_gpio_ver = map_mem(gpio_bytes, 0x41200000);
    axi_gpio_lim = map_mem(gpio_bytes, 0x41220000);
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
    cout << "clear done\n";
}
int set_limit(long int val)
{
    axi_gpio_lim[0] = val;
    cout << "lim set: " << axi_gpio_lim[0] << "\n";
}
int start_work()
{
    axi_gpio_ctrl[0] = 0b1;
}
int wait_work()
{
    while (axi_gpio_cnt[0] < axi_gpio_lim[0])
    {
    }
    cout << "work done\n";
}
int print_res()
{
    cout << "lim: " << axi_gpio_lim[0]
         << " cnt: " << axi_gpio_cnt[0]
         << "\n";
}
int print_ver()
{
    cout << "ver: " << axi_gpio_ver[0] << "\n";
}
/*int write_bram(unsigned char val)
{
    //memcpy(axi_bram_ctrl_0, i_data + (r * bram_length), limit);
    unsigned char *bram = (unsigned char *)axi_bram_ctrl_0;
    for (int i = 0; i < bram_bytes; i++)
    {
        bram[i] = val;
    }
}*/
int main()
{
    auto start = high_resolution_clock::now();
    open_mem();
    print_ver();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "time0: " << duration.count() << " us\n";

    start = high_resolution_clock::now();
    //write_bram(0x01);
    //set_avg(0);
    wait_clear();
    set_limit(bram_bytes);
    start_work();
    wait_work();
    print_res();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "time1: " << duration.count() << " us\n";

    start = high_resolution_clock::now();
    //write_bram(0xFF);
    //set_avg(0);
    wait_clear();
    set_limit(bram_bytes);
    start_work();
    wait_work();
    print_res();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "time2: " << duration.count() << " us\n";

    return 0;
}