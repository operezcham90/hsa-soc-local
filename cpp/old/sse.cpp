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
#define GPIO_BYTES 4
using namespace std;
int fd;
unsigned long int *axi_gpio_0;
unsigned long int *axi_gpio_1;
unsigned long int *axi_gpio_2;
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
}
int close_mem()
{
    close(fd);
}
int main()
{
    open_mem();

    // send value
    axi_gpio_0[0] = 1;
    cout << "in:" << 1 << "\n";
    // send conf
    axi_gpio_1[0] = 0x0;
    // read value
    cout << "out:" << axi_gpio_2[0] << "\n";

    // send value
    axi_gpio_0[0] = 2;
    cout << "in:" << 2 << "\n";
    // send conf
    axi_gpio_1[0] = 0x0;
    // read value
    cout << "out:" << axi_gpio_2[0] << "\n";

    // send value
    axi_gpio_0[0] = 4;
    cout << "in:" << 4 << "\n";
    // send conf
    axi_gpio_1[0] = 0x0;
    // read value
    cout << "out:" << axi_gpio_2[0] << "\n";

    // send value
    axi_gpio_0[0] = 3;
    cout << "in:" << 3 << "\n";
    // send conf
    axi_gpio_1[0] = 0x1;
    // read value
    cout << "out:" << axi_gpio_2[0] << "\n";

    close_mem();
    return 0;
}