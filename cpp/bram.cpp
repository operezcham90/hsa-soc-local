#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
// addresses from vivado block design
unsigned long int *axi_bram_ctrl_0;
unsigned long int *axi_gpio_0;
unsigned long int *axi_gpio_1;
unsigned long int *axi_gpio_2;
unsigned long int *axi_gpio_3;
int main()
{
    // size of bram default block
    unsigned int bram_size = 2048 * 4;
    // 32 bits
    unsigned int gpio_size = 4;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_bram_ctrl_0 = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x40000000);
        axi_gpio_0 = (unsigned long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41200000);
        axi_gpio_1 = (unsigned long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41210000);
        axi_gpio_2 = (unsigned long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41220000);
        axi_gpio_3 = (unsigned long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41230000);
        unsigned char *axi_bram_ctrl_0_bytes = (unsigned char *)axi_bram_ctrl_0;

        for (long int i = 0; i < 1000; i++)
        {
            axi_bram_ctrl_0_bytes[i + 4] = i + 1;
        }

        axi_bram_ctrl_0[0] = 0;
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        axi_bram_ctrl_0[0] = 10;
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        axi_bram_ctrl_0[0] = 0;
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);
        printf("Count: %lu Avg: %lu Err: %lu\n", axi_gpio_1[0], axi_gpio_2[0], axi_gpio_3[0]);

        close(fd);
    }
}