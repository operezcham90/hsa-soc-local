#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
// addresses from vivado block design
long int *axi_bram_ctrl_0;
long int *axi_gpio_0;
long int *axi_gpio_1;
int main()
{
    // size of bram default block
    unsigned int bram_size = 2048 * 4;
    // 32 bits
    unsigned int gpio_size = 4;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_bram_ctrl_0 = (long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x40000000);
        axi_gpio_0 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41200000);
        axi_gpio_1 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41210000);
        unsigned char *axi_bram_ctrl_0_bytes = (unsigned char *)axi_bram_ctrl_0;

        axi_bram_ctrl_0_bytes[0] = 4;
        axi_bram_ctrl_0_bytes[1] = 3;
        axi_bram_ctrl_0_bytes[2] = 2;
        axi_bram_ctrl_0_bytes[3] = 1;

        axi_gpio_0[0] = 0;
        printf("Count: %ld\n", axi_gpio_1[0]);

        axi_gpio_0[0] = 1;
        printf("Count: %ld\n", axi_gpio_1[0]);

        axi_gpio_0[0] = 2;
        printf("Count: %ld\n", axi_gpio_1[0]);

        axi_gpio_0[0] = 3;
        printf("Count: %ld\n", axi_gpio_1[0]);

        close(fd);
    }
}