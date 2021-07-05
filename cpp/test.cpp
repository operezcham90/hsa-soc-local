// g++ test.cpp -o test `pkg-config --cflags --libs opencv`
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <math.h>
#define GPIO_BYTES 4
#define CDMA_BYTES 40
#define BRAM_BYTES 8192
using namespace std;
int fd;
unsigned long int *axi_gpio_0;
unsigned long int *axi_gpio_1;
unsigned long int *axi_gpio_2;
unsigned long int *axi_gpio_3;
unsigned long int *axi_cdma_0;
unsigned long int *axi_cdma_1;
unsigned long int *axi_cdma_2;
unsigned long int *axi_cdma_3;
unsigned long int *ddr_0;
unsigned long int *ddr_1;
unsigned long int *ddr_2;
unsigned long int *ddr_3;
unsigned long int *ddr_4;
unsigned long int *ddr_5;
unsigned long int *ddr_6;
unsigned long int *ddr_7;
unsigned long int *ddr_8;
unsigned long int idx = 4;
unsigned long int num_elem = 4;
unsigned long int *map_mem(unsigned int bytes, off_t addr)
{
    return (unsigned long int *)mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
}
int open_mem()
{
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        exit(1);
    }
    axi_gpio_0 = map_mem(GPIO_BYTES, 0x41200000);
    axi_gpio_1 = map_mem(GPIO_BYTES, 0x41210000);
    axi_gpio_2 = map_mem(GPIO_BYTES, 0x41220000);
    axi_gpio_3 = map_mem(GPIO_BYTES, 0x41230000);
    axi_cdma_0 = map_mem(CDMA_BYTES, 0x7E200000);
    axi_cdma_1 = map_mem(CDMA_BYTES, 0x7E210000);
    axi_cdma_2 = map_mem(CDMA_BYTES, 0x7E220000);
    axi_cdma_3 = map_mem(CDMA_BYTES, 0x7E230000);
    ddr_0 = map_mem(BRAM_BYTES, 0x0E000000);
    ddr_1 = map_mem(BRAM_BYTES, 0x0F000000);
    ddr_2 = map_mem(BRAM_BYTES, 0x10000000);
    ddr_3 = map_mem(BRAM_BYTES, 0x11000000);
    ddr_4 = map_mem(BRAM_BYTES, 0x12000000);
    ddr_5 = map_mem(BRAM_BYTES, 0x13000000);
    ddr_6 = map_mem(BRAM_BYTES, 0x14000000);
    ddr_7 = map_mem(BRAM_BYTES, 0x15000000);
    ddr_8 = map_mem(BRAM_BYTES, 0x16000000);
}
int close_mem()
{
    close(fd);
}
void print_version()
{
    cout << "version:" << axi_gpio_0[0] << "\n";
}
void clear_signal()
{
    axi_gpio_1[0] = 0;
}
void write_data()
{
    unsigned char *data_t = (unsigned char *)ddr_0;
    unsigned char *data_i_0 = (unsigned char *)ddr_1;
    unsigned char *data_i_1 = (unsigned char *)ddr_3;
    unsigned char *data_i_2 = (unsigned char *)ddr_5;
    unsigned char *data_i_3 = (unsigned char *)ddr_7;
    for (int i = 0; i < num_elem; i++)
    {
        data_t[i] = 1 + i;
        data_i_0[i] = 2 + i;
        data_i_1[i] = 10 - i;
        data_i_2[i] = 1 + (i % 2);
        data_i_3[i] = 10 - (i % 2);
    }
    // write t
    axi_cdma_0[0] = 0x4;
    axi_cdma_0[0] = 0x0;
    axi_cdma_0[6] = 0x0E000000;
    axi_cdma_0[8] = 0xC0000000;
    axi_cdma_0[10] = num_elem;
    while ((axi_cdma_0[1] & 0x00000002) == 0)
    {
    }
    // write i
    axi_cdma_0[0] = 0x4;
    axi_cdma_0[0] = 0x0;
    axi_cdma_0[6] = 0x0F000000;
    axi_cdma_0[8] = 0xC2000000;

    axi_cdma_1[0] = 0x4;
    axi_cdma_1[0] = 0x0;
    axi_cdma_1[6] = 0x11000000;
    axi_cdma_1[8] = 0xC0000000;

    axi_cdma_2[0] = 0x4;
    axi_cdma_2[0] = 0x0;
    axi_cdma_2[6] = 0x13000000;
    axi_cdma_2[8] = 0xC0000000;

    axi_cdma_3[0] = 0x4;
    axi_cdma_3[0] = 0x0;
    axi_cdma_3[6] = 0x15000000;
    axi_cdma_3[8] = 0xC0000000;

    axi_cdma_0[10] = num_elem;
    axi_cdma_1[10] = num_elem;
    axi_cdma_2[10] = num_elem;
    axi_cdma_3[10] = num_elem;
    while (
        (axi_cdma_0[1] & 0x00000002) == 0 &&
        (axi_cdma_1[1] & 0x00000002) == 0 &&
        (axi_cdma_2[1] & 0x00000002) == 0 &&
        (axi_cdma_3[1] & 0x00000002) == 0)
    {
    }
}
void start_signal()
{
    axi_gpio_2[0] = idx;
    axi_gpio_1[0] = num_elem;
}
void wait_done()
{
    while (axi_gpio_3[0] != 0x7)
    {
    }
}
void read_data()
{
    axi_cdma_0[0] = 0x4;
    axi_cdma_0[0] = 0x0;
    axi_cdma_0[6] = 0xC4000000;
    axi_cdma_0[8] = 0x10000000;

    axi_cdma_1[0] = 0x4;
    axi_cdma_1[0] = 0x0;
    axi_cdma_1[6] = 0xC2000000;
    axi_cdma_1[8] = 0x12000000;

    axi_cdma_2[0] = 0x4;
    axi_cdma_2[0] = 0x0;
    axi_cdma_2[6] = 0xC2000000;
    axi_cdma_2[8] = 0x14000000;

    axi_cdma_3[0] = 0x4;
    axi_cdma_3[0] = 0x0;
    axi_cdma_3[6] = 0xC2000000;
    axi_cdma_3[8] = 0x16000000;

    axi_cdma_0[10] = 256;
    axi_cdma_1[10] = 256;
    axi_cdma_2[10] = 256;
    axi_cdma_3[10] = 256;
    while (
        (axi_cdma_0[1] & 0x00000002) == 0 &&
        (axi_cdma_1[1] & 0x00000002) == 0 &&
        (axi_cdma_2[1] & 0x00000002) == 0 &&
        (axi_cdma_3[1] & 0x00000002) == 0)
    {
    }
}
void print_results()
{
    cout << "bram:" << ddr_2[idx / 4] << "\n";
    cout << "bram:" << ddr_4[idx / 4] << "\n";
    cout << "bram:" << ddr_6[idx / 4] << "\n";
    cout << "bram:" << ddr_8[idx / 4] << "\n";
    cout << "state:" << axi_gpio_3[0] << "\n";
}
int main()
{
    open_mem();
    print_version();
    clear_signal();
    write_data();
    start_signal();
    wait_done();
    read_data();
    print_results();
    close_mem();
    return 0;
}