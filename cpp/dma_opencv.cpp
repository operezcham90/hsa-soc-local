#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

unsigned long axi_cdma_0 = 0x7E200000;
unsigned long axi_cdma_1 = 0x7E210000;
unsigned long axi_cdma_2 = 0x7E220000;
unsigned long axi_cdma_3 = 0x7E230000;
unsigned long axi_gpio_0 = 0x41200000;
unsigned long axi_gpio_1 = 0x41210000;
unsigned long slave_axi_bram_ctrl = 0xC0000000;
unsigned long slave_ddr = 0x00000000;
unsigned long 8k_bytes = 0x2000;
unsigned long *cdma_0;
unsigned long *cdma_1;
unsigned long *cdma_2;
unsigned long *cdma_3;

unsigned long *open_cdma(unsigned long addr)
{
    int dh = open("/dev/mem", O_RDWR | O_SYNC);
    return (unsigned long *)mmap(NULL, 8k_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, dh, addr);
}

/*void reset_cdma()
{
    while ((cdma[1] & 0x00000002) == 0 && (cdma1[1] & 0x00000002) == 0)
    {
        cdma[0] = 0x00000004;
        cdma1[0] = 0x00000004;
    }
}*/

int main()
{
    // time metrics
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    start = high_resolution_clock::now();
    cdma_0 = open_cdma(axi_cdma_0);
    cdma_1 = open_cdma(axi_cdma_1);
    cdma_2 = open_cdma(axi_cdma_2);
    cdma_3 = open_cdma(axi_cdma_3);
    stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Open: " << duration.count() << " us\n";
}