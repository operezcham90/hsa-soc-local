#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sched.h>
#include <time.h>

#define MY_SIZE_BYTE 64

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

unsigned long int CDMA = 0x7E200000;
unsigned long int RAM = 0x00000000;
unsigned long int BRAM = 0x00001000;

unsigned long int FILESIZE = 1;

int main(int argc, char *argv[])
{
    //	if (argc > 3)
    //		sleepTime=(unsigned int)atoi(argv[3]);
    if (argc > 1)
        FILESIZE = (unsigned int)atoi(argv[1]);

    volatile unsigned int i;
    volatile int value;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    void *cdma = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, CDMA);
    void *bram = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BRAM);
    void *ram = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, RAM);

    printf("BRAM contents: ");
    for (i = 0; i < 20; i++)
    {
        printf("%d, ", ((volatile unsigned int *)bram)[i]);
    }
    printf("\n");

    printf("RAM contents: ");
    for (i = 0; i < 20; i++)
    {
        printf("%d, ", ((volatile unsigned int *)ram)[i]);
    }
    printf("\n");

    printf("This shit too: ");
    for (i = 0; i < 80; i++)
    {
        printf("%d, ", *(volatile unsigned int *)(ram + i));
    }
    printf("\n");

    printf("Setting DMA to write to RAM.\n");

    *(volatile unsigned int *)(cdma + 0x00) = ((unsigned int)0x00000000);
    value = *(volatile unsigned int *)(cdma + 0x00);
    printf("Control Register = 0x%08x\n", value);

    *(volatile unsigned int *)(cdma + 0x18) = (volatile unsigned int)(BRAM);
    value = *(volatile unsigned int *)(cdma + 0x18);
    printf("Source Pointer = 0x%08x\n", value);

    *(volatile unsigned int *)(cdma + 0x20) = (volatile unsigned int)(RAM);
    value = *(volatile unsigned int *)(cdma + 0x20);
    printf("Destination Pointer = 0x%08x\n", value);

    *(volatile unsigned int *)(cdma + 0x28) = ((volatile unsigned int)MAP_SIZE);
    //*(volatile unsigned int *)(cdma + 0x28) = ((volatile unsigned int)4096 * FILESIZE);
    value = *(volatile unsigned int *)(cdma + 0x28);
    printf("Bytes to Transfer (BTT) = 0x%08x\n", value);

    //Wait for the "idle" bit to go high.
    while (*(volatile unsigned int *)(cdma + 0x4) & (0x1) == 0)
    {
    }

    for (i = 0; i < 5; i++)
    {
        //Read status register
        value = ((volatile unsigned int *)cdma)[0x4];
        printf("status = 0x%08x\n", value);
    }

    //	printf("BRAM contents: ");
    //	for (i=0; i<20; i++) {
    //		printf("%d, ",((volatile unsigned int *)bram)[i]);
    //	}
    //	printf("\n");

    printf("RAM contents: ");
    for (i = 0; i < 20; i++)
    {
        printf("%d, ", ((volatile unsigned int *)ram)[i]);
    }
    printf("\n");

    return 0;
}