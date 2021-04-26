#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main()
{
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    int dh = open("/dev/mem", O_RDWR | O_SYNC);
    if (dh == -1)
    {
        cout << "devmem fail\n";
        return 1;
    }
    unsigned long map_size = 0x2000; // 8K
    unsigned long src_addr = 0x0e000000;
    unsigned long dest_addr = 0x40000000;
    unsigned long *cdma = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x7E200000);
    unsigned long *ddr_src = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, src_addr);
    unsigned long *ddr_dest = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, dest_addr);
    // write source
    for (int i = 0; i < 0x800; i++)
    {
        ddr_src[i] = i + 1;
    }
    cout << "SRC data:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_src[i] << " ";
    }
    // write destiny
    for (int i = 0; i < 0x800; i++)
    {
        ddr_dest[i] = 0;
    }
    cout << "\nDEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
    }
    // reset CDMA
    start = high_resolution_clock::now();
    cout << "\nCDMA reset\n";
    while ((cdma[1] & 0x00000002) == 0)
    {
        cdma[0] = 0x00000004;
    }
    cout << "CDMA ready\n";
    cdma[0] = 0x00000000;
    // set CDMA
    cdma[6] = src_addr;
    cdma[8] = dest_addr;
    cdma[10] = map_size;
    // wait transfer
    while ((cdma[1] & 0x00000002) == 0)
    {
    }
    cout << "CDMA done\n";
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Time: " << duration.count() << " us\n";
    cout << "DEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
    }
    // confirmation
    int i = 0;
    for (int i = 0; i < 0x800; i++)
    {
        i += ddr_src[i] - ddr_dest[i];
    }
    if (i == 0)
    {
        cout << "\nComparison successful\n";
    }
    else
    {
        cout << "\nComparison failed\n";
    }
}