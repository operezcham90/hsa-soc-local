#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
off_t axi_bram_ctrl_0_addr = 0x40000000;
off_t axi_bram_ctrl_1_addr = 0x42000000;
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
long int *axi_bram_ctrl_0;
long int *axi_bram_ctrl_1;
long int *axi_gpio_0;
long int *axi_gpio_1;
int main()
{
    unsigned int bram_size = 0x8000;
    unsigned int gpio_size = 0x4;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_bram_ctrl_0 = (long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_bram_ctrl_0_addr);
        axi_bram_ctrl_1 = (long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_bram_ctrl_1_addr);
        axi_gpio_0 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_0_addr);
        axi_gpio_1 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_1_addr);

        axi_bram_ctrl_0[0] = 1234;
        printf("Read common\n");
        printf("%ld\n", axi_bram_ctrl_1[0]);

        printf("Read low level\n");
        axi_gpio_0[0] = 0;
        printf("0: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 1;
        printf("1: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 2;
        printf("2: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 3;
        printf("3: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 4;
        printf("4: %ld\n", axi_gpio_1[0]);

        axi_bram_ctrl_0[0] = 4231;
        printf("Read common\n");
        printf("%ld\n", axi_bram_ctrl_1[0]);

        printf("Read low level\n");
        axi_gpio_0[0] = 0;
        printf("0: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 1;
        printf("1: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 2;
        printf("2: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 3;
        printf("3: %ld\n", axi_gpio_1[0]);
        axi_gpio_0[0] = 4;
        printf("4: %ld\n", axi_gpio_1[0]);

        close(fd);
    }
}