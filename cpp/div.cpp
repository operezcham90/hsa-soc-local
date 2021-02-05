#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
int main()
{
    // 32 bits
    unsigned int gpio_size = 0x4;
    int fd;
    long int *axi_gpio_0;
    long int *axi_gpio_1;
    long int *axi_gpio_2;
    long int *axi_gpio_3;
    long int *axi_gpio_4;
    long int *axi_gpio_5;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_gpio_0 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41200000);
        axi_gpio_1 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41210000);
        axi_gpio_2 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41220000);
        axi_gpio_3 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41230000);
        axi_gpio_4 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41240000);
        axi_gpio_5 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41250000);

        axi_gpio_0[0] = 5;
        axi_gpio_1[0] = 2;
        printf("Res: %ld\n", axi_gpio_2[0]);

        axi_gpio_3[0] = 5;
        axi_gpio_4[0] = 2;
        printf("Res: %ld\n", axi_gpio_5[0]);

        close(fd);
    }
}