#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <iostream>

using namespace std;

int main()
{
    int dh = open("/dev/mem", O_RDWR | O_SYNC);
    if (dh == -1)
    {
        cout << "devmem fail\n";
        return 1;
    }
    int map_size = 0x10000; // 64K
    unsigned int *cdma = (unsigned int *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x7E200000);
    unsigned int *ddr_src = (unsigned int *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x0e000000);
    unsigned int *ddr_dest = (unsigned int *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x0f000000);
    // reset CDMA
    cout << "CDMA reset\n";
    while ((cdma[1] & 0x00000002) == 0)
    {
        cdma[0] = 0x00000004;
    }
    cout << "CDMA in idle state\n";
    // write source
    for (int i = 0; i < 8; i++)
    {
        ddr_src[i] = i + 1;
    }
    cout << "SRC data:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_src[i] << " ";
    }
    // write destiny
    for (int i = 0; i < 8; i++)
    {
        ddr_dest[i] = 0;
    }
    cout << "\nDEST:\n";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << ddr_dest[i] << " ";
    }
}