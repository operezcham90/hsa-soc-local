#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <iostream>

using namespace std;

int main()
{
    int map_size = 65535;
    int dh = open("/dev/mem", O_RDWR | O_SYNC);
    if (dh == -1)
    {
        cout << "devmem fail\n";
    }
    unsigned int *cdma = (unsigned int *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x7E200000);
    unsigned int *ddr_src = (unsigned int *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x0e000000);
    unsigned int *ddr_dest = (unsigned int *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x0f000000);
    cout << "CDMA:\n";
    for (int i = 0; i < 2; i++)
    {
        cout << hex << cdma[i] << " ";
    }
    cout << "SRC:\n";
    for (int i = 0; i < 20; i++)
    {
        cout << hex << ddr_src[i] << " ";
    }
    cout << "DEST:\n";
    for (int i = 0; i < 20; i++)
    {
        cout << hex << ddr_dest[i] << " ";
    }
}