#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

unsigned long slave_ddr = 0x00000000;
unsigned long bytes_8k = 0x2000;
unsigned long *cdma_0;
unsigned long *cdma_1;
unsigned long *cdma_2;
unsigned long *cdma_3;

int main()
{
    // time metrics
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // open cdma
    start = high_resolution_clock::now();
    int dh = open("/dev/mem", 0x101002);
    cdma_0 = (unsigned long *)mmap(NULL, 40, 0x3, 0x1, dh, 0x7E200000);
    cdma_1 = (unsigned long *)mmap(NULL, 40, 0x3, 0x1, dh, 0x7E210000);
    cdma_2 = (unsigned long *)mmap(NULL, 40, 0x3, 0x1, dh, 0x7E220000);
    cdma_3 = (unsigned long *)mmap(NULL, 40, 0x3, 0x1, dh, 0x7E230000);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Open: " << duration.count() << " us\n";
    // reset cdma
    start = high_resolution_clock::now();
    while (!(cdma_0[1] & 0x2) && !(cdma_1[1] & 0x2) && !(cdma_2[1] & 0x2) && !(cdma_3[1] & 0x2))
    {
        cdma_0[0] = 0x4;
        cdma_1[0] = 0x4;
        cdma_2[0] = 0x4;
        cdma_3[0] = 0x4;
    }
    cdma_0[0] = 0x0;
    cdma_1[0] = 0x0;
    cdma_2[0] = 0x0;
    cdma_3[0] = 0x0;
    cdma_0[8] = 0xC0000000;
    cdma_1[8] = 0xC0000000;
    cdma_2[8] = 0xC0000000;
    cdma_3[8] = 0xC0000000;
    cdma_0[6] = 0x00000000;
    cdma_1[6] = 0x00002000;
    cdma_2[6] = 0x00004000;
    cdma_3[6] = 0x00006000;
    cdma_0[10] = 0x2000;
    cdma_1[10] = 0x2000;
    cdma_2[10] = 0x2000;
    cdma_3[10] = 0x2000;
    while (!(cdma_0[1] & 0x2) && !(cdma_1[1] & 0x2) && !(cdma_2[1] & 0x2) && !(cdma_3[1] & 0x2))
    {
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Send: " << duration.count() << " us\n";
}