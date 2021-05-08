#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <iostream>
#include <chrono>

#define CDMA_B 40
#define RDWRSYNC 0x101002
#define RDWR 0x3
#define SH 0x1
#define GPIO_0 0x41200000
#define GPIO_1 0x41210000
#define CDMA_0 0x7E200000
#define CDMA_1 0x7E210000
#define CDMA_2 0x7E220000
#define CDMA_3 0x7E230000
#define DDR_0 0x0A000000
#define DDR_1 0x0B000000
#define DDR_2 0x0C000000
#define DDR_3 0x0D000000
#define BUSY 0x2
#define RESET 0x4
#define STDBY 0x0
#define SLAVE_BRAM 0xC0000000
#define B_8K 0x2000

using namespace std;
using namespace std::chrono;

unsigned long *cdma_0;
unsigned long *cdma_1;
unsigned long *cdma_2;
unsigned long *cdma_3;
unsigned long *data_0;
unsigned long *data_1;
unsigned long *data_2;
unsigned long *data_3;
unsigned long *gpio_0;
unsigned long *gpio_1;

int main()
{
    // time metrics
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // open cdma
    start = high_resolution_clock::now();
    int dh = open("/dev/mem", RDWRSYNC);
    cdma_0 = (unsigned long *)mmap(NULL, CDMA_B, RDWR, SH, dh, CDMA_0);
    cdma_1 = (unsigned long *)mmap(NULL, CDMA_B, RDWR, SH, dh, CDMA_1);
    cdma_2 = (unsigned long *)mmap(NULL, CDMA_B, RDWR, SH, dh, CDMA_2);
    cdma_3 = (unsigned long *)mmap(NULL, CDMA_B, RDWR, SH, dh, CDMA_3);
    data_0 = (unsigned long *)mmap(NULL, B_8K, RDWR, SH, dh, DDR_0);
    data_1 = (unsigned long *)mmap(NULL, B_8K, RDWR, SH, dh, DDR_1);
    data_2 = (unsigned long *)mmap(NULL, B_8K, RDWR, SH, dh, DDR_2);
    data_3 = (unsigned long *)mmap(NULL, B_8K, RDWR, SH, dh, DDR_3);
    gpio_0 = (unsigned long *)mmap(NULL, 4, RDWR, SH, dh, GPIO_0);
    gpio_1 = (unsigned long *)mmap(NULL, 4, RDWR, SH, dh, GPIO_1);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Open: " << duration.count() << " us\n";
    // write data
    start = high_resolution_clock::now();
    for (int i = 0; i < 10; i++)
    {
        data_0[i] = 4;
        data_1[i] = 3;
        data_2[i] = 2;
        data_3[i] = 1;
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Write: " << duration.count() << " us\n";
    // reset cdma
    start = high_resolution_clock::now();
    cout << hex << cdma_0[1] << "\n";
    while (!(cdma_0[1] & BUSY) && !(cdma_1[1] & BUSY) && !(cdma_2[1] & BUSY) && !(cdma_3[1] & BUSY))
    {
        cdma_0[0] = RESET;
        cdma_1[0] = RESET;
        cdma_2[0] = RESET;
        cdma_3[0] = RESET;
        cout << hex << cdma_0[1] << "\n";
    }
    // cdma config
    cdma_0[0] = STDBY;
    cdma_1[0] = STDBY;
    cdma_2[0] = STDBY;
    cdma_3[0] = STDBY;
    // destiny address
    cdma_0[8] = SLAVE_BRAM;
    cdma_1[8] = SLAVE_BRAM;
    cdma_2[8] = SLAVE_BRAM;
    cdma_3[8] = SLAVE_BRAM;
    // source address
    cdma_0[6] = DDR_0;
    cdma_1[6] = DDR_1;
    cdma_2[6] = DDR_2;
    cdma_3[6] = DDR_3;
    // start work
    cdma_0[10] = B_8K;
    cdma_1[10] = B_8K;
    cdma_2[10] = B_8K;
    cdma_3[10] = B_8K;
    // wait work
    cout << hex << cdma_0[1] << "\n";
    while (!(cdma_0[1] & BUSY) && !(cdma_1[1] & BUSY) && !(cdma_2[1] & BUSY) && !(cdma_3[1] & BUSY))
    {
        cout << hex << cdma_0[1] << "\n";
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Send: " << duration.count() << " us\n";
    // read answer
    start = high_resolution_clock::now();
    gpio_0[0] = 0x0;
    long res = gpio_1[0];
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Read: " << duration.count() << " us\n";
    cout << dec << "4 + 3 + 2 + 1 = " << res << "\n";
}