#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
// addresses from vivado block design
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
off_t axi_gpio_2_addr = 0x41220000;
long int *axi_gpio_0;
long int *axi_gpio_1;
long int *axi_gpio_2;
int main()
{
    // 32 bits
    unsigned int gpio_size = 0x4;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_gpio_0 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_0_addr);
        axi_gpio_1 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_1_addr);
        axi_gpio_2 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_2_addr);

        unsigned char *bytes = (unsigned char *)axi_gpio_0;
        bytes[0] = 1;
        bytes[1] = 2;
        bytes[2] = 0;
        bytes[3] = 4;

        axi_gpio_1[0] = 0;
        printf("B0: %ld\n", axi_gpio_2[0]);

        axi_gpio_1[0] = 1;
        printf("B1: %ld\n", axi_gpio_2[0]);

        axi_gpio_1[0] = 2;
        printf("B2: %ld\n", axi_gpio_2[0]);

        axi_gpio_1[0] = 3;
        printf("B3: %ld\n", axi_gpio_2[0]);

        bytes[0] = 255;
        bytes[1] = 0;
        bytes[2] = 0;
        bytes[3] = 255;

        axi_gpio_1[0] = 0;
        printf("B0: %ld\n", axi_gpio_2[0]);

        axi_gpio_1[0] = 1;
        printf("B1: %ld\n", axi_gpio_2[0]);

        axi_gpio_1[0] = 2;
        printf("B2: %ld\n", axi_gpio_2[0]);

        axi_gpio_1[0] = 3;
        printf("B3: %ld\n", axi_gpio_2[0]);

        close(fd);
    }
}