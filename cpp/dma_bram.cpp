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
    unsigned long src_addr1 = 0x0f000000;
    unsigned long dest_addr1 = 0x42000000;
    unsigned long src_addr2 = 0x10000000;
    unsigned long dest_addr2 = 0x44000000;
    unsigned long src_addr3 = 0x11000000;
    unsigned long dest_addr3 = 0x46000000;

    unsigned long *cdma = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x7E200000);
    unsigned long *ddr_src = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, src_addr);
    unsigned long *ddr_dest = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, dest_addr);
    unsigned long *cdma1 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x7E210000);
    unsigned long *ddr_src1 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, src_addr1);
    unsigned long *ddr_dest1 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, dest_addr1);
    unsigned long *cdma2 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x7E220000);
    unsigned long *ddr_src2 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, src_addr2);
    unsigned long *ddr_dest2 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, dest_addr2);
    unsigned long *cdma3 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x7E230000);
    unsigned long *ddr_src3 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, src_addr3);
    unsigned long *ddr_dest3 = (unsigned long *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, dest_addr3);

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
    //cout << "\nCDMA reset\n";
    while ((cdma[1] & 0x00000002) == 0)
    {
        cdma[0] = 0x00000004;
    }
    //cout << "CDMA ready\n";
    cdma[0] = 0x00000000;
    // set CDMA
    cdma[6] = src_addr;
    cdma[8] = dest_addr;
    cdma[10] = map_size;
    // wait transfer
    while ((cdma[1] & 0x00000002) == 0)
    {
    }
    //cout << "CDMA done\n";
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Time: " << duration.count() << " us\n";
    cout << "DEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
    }
    // confirmation
    int diff = 0;
    for (int i = 0; i < 0x800; i++)
    {
        diff += ddr_src[i] - ddr_dest[i];
    }
    if (diff == 0)
    {
        cout << "\nComparison successful\n";
    }
    else
    {
        cout << "\nComparison failed\n";
    }

    // write source
    for (int i = 0; i < 0x800; i++)
    {
        ddr_src[i] = i + 1;
        ddr_src1[i] = i + 2;
    }
    cout << "SRC data:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_src[i] << " ";
        cout << hex << ddr_src1[i] << " ";
    }
    // write destiny
    for (int i = 0; i < 0x800; i++)
    {
        ddr_dest[i] = 0;
        ddr_dest1[i] = 0;
    }
    cout << "\nDEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
        cout << hex << ddr_dest1[i] << " ";
    }
    // reset CDMA
    start = high_resolution_clock::now();
    //cout << "\nCDMA reset\n";
    while ((cdma[1] & 0x00000002) == 0 && (cdma1[1] & 0x00000002) == 0)
    {
        cdma[0] = 0x00000004;
        cdma1[0] = 0x00000004;
    }
    //cout << "CDMA ready\n";
    cdma[0] = 0x00000000;
    cdma1[0] = 0x00000000;
    // set CDMA
    cdma[6] = src_addr;
    cdma[8] = dest_addr;
    cdma1[6] = src_addr1;
    cdma1[8] = dest_addr1;
    cdma[10] = map_size;
    cdma1[10] = map_size;
    // wait transfer
    while ((cdma[1] & 0x00000002) == 0 && (cdma1[1] & 0x00000002) == 0)
    {
    }
    //cout << "CDMA done\n";
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Time: " << duration.count() << " us\n";
    cout << "DEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
        cout << hex << ddr_dest1[i] << " ";
    }
    // confirmation
    diff = 0;
    for (int i = 0; i < 0x800; i++)
    {
        diff += ddr_src[i] - ddr_dest[i];
        diff += ddr_src1[i] - ddr_dest1[i];
    }
    if (diff == 0)
    {
        cout << "\nComparison successful\n";
    }
    else
    {
        cout << "\nComparison failed\n";
    }

    // write source
    for (int i = 0; i < 0x800; i++)
    {
        ddr_src[i] = i + 1;
        ddr_src1[i] = i + 2;
        ddr_src2[i] = i + 3;
        ddr_src3[i] = i + 4;
    }
    cout << "SRC data:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_src[i] << " ";
        cout << hex << ddr_src1[i] << " ";
        cout << hex << ddr_src2[i] << " ";
        cout << hex << ddr_src3[i] << " ";
    }
    // write destiny
    for (int i = 0; i < 0x800; i++)
    {
        ddr_dest[i] = 0;
        ddr_dest1[i] = 0;
        ddr_dest2[i] = 0;
        ddr_dest3[i] = 0;
    }
    cout << "\nDEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
        cout << hex << ddr_dest1[i] << " ";
        cout << hex << ddr_dest2[i] << " ";
        cout << hex << ddr_dest3[i] << " ";
    }
    // reset CDMA
    start = high_resolution_clock::now();
    //cout << "\nCDMA reset\n";
    while ((cdma[1] & 0x00000002) == 0 &&
           (cdma1[1] & 0x00000002) == 0 &&
           (cdma2[1] & 0x00000002) == 0 &&
           (cdma3[1] & 0x00000002) == 0)
    {
        cdma[0] = 0x00000004;
        cdma1[0] = 0x00000004;
        cdma2[0] = 0x00000004;
        cdma3[0] = 0x00000004;
    }
    //cout << "CDMA ready\n";
    cdma[0] = 0x00000000;
    cdma1[0] = 0x00000000;
    cdma2[0] = 0x00000000;
    cdma3[0] = 0x00000000;
    // set CDMA
    cdma[6] = src_addr;
    cdma[8] = dest_addr;
    cdma1[6] = src_addr1;
    cdma1[8] = dest_addr1;
    cdma2[6] = src_addr2;
    cdma2[8] = dest_addr2;
    cdma3[6] = src_addr3;
    cdma3[8] = dest_addr3;

    cdma[10] = map_size;
    cdma1[10] = map_size;
    cdma2[10] = map_size;
    cdma3[10] = map_size;
    // wait transfer
    while ((cdma[1] & 0x00000002) == 0 &&
           (cdma1[1] & 0x00000002) == 0 &&
           (cdma2[1] & 0x00000002) == 0 &&
           (cdma3[1] & 0x00000002) == 0)
    {
    }
    //cout << "CDMA done\n";
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << dec << "Time: " << duration.count() << " us\n";
    cout << "DEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
        cout << hex << ddr_dest1[i] << " ";
        cout << hex << ddr_dest2[i] << " ";
        cout << hex << ddr_dest3[i] << " ";
    }
    // confirmation
    diff = 0;
    for (int i = 0; i < 0x800; i++)
    {
        diff += ddr_src[i] - ddr_dest[i];
        diff += ddr_src1[i] - ddr_dest1[i];
        diff += ddr_src2[i] - ddr_dest2[i];
        diff += ddr_src3[i] - ddr_dest3[i];
    }
    if (diff == 0)
    {
        cout << "\nComparison successful\n";
    }
    else
    {
        cout << "\nComparison failed\n";
    }
}