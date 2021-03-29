#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
// addresses from vivado block design
unsigned long int *axi_bram_ctrl_0;
unsigned long int *axi_bram_ctrl_1;
unsigned long int *axi_bram_ctrl_2;
unsigned long int *axi_gpio_0;
unsigned long int *axi_gpio_1;
unsigned long int *axi_gpio_2;
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
        axi_bram_ctrl_1 = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x42000000);
        axi_bram_ctrl_2 = (unsigned long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x44000000);
        axi_gpio_0 = (unsigned long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41200000);
        axi_gpio_1 = (unsigned long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41210000);
        axi_gpio_2 = (unsigned long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41220000);
        unsigned char *axi_bram_ctrl_0_bytes = (unsigned char *)axi_bram_ctrl_0;
        unsigned char *axi_bram_ctrl_1_bytes = (unsigned char *)axi_bram_ctrl_1;

        for (unsigned long int i = 0; i < 10; i++)
        {
            printf("BRAM %lu : %ld\n", i, axi_bram_ctrl_2[i]);
        }

        axi_bram_ctrl_0[0] = 0;
        printf("Not started\n");
        printf("Debug0: %lu Debug1: %lu Debug2: %ld\n", axi_gpio_0[0], axi_gpio_1[0], axi_gpio_2[0]);
        printf("Debug0: %lu Debug1: %lu Debug2: %ld\n", axi_gpio_0[0], axi_gpio_1[0], axi_gpio_2[0]);
        printf("Debug0: %lu Debug1: %lu Debug2: %ld\n", axi_gpio_0[0], axi_gpio_1[0], axi_gpio_2[0]);

        axi_bram_ctrl_0_bytes[4] = 1;
        axi_bram_ctrl_0_bytes[5] = 2;
        axi_bram_ctrl_0_bytes[6] = 3;
        axi_bram_ctrl_0_bytes[7] = 4;
        axi_bram_ctrl_0_bytes[8] = 1;
        axi_bram_ctrl_0_bytes[9] = 2;
        axi_bram_ctrl_0_bytes[10] = 3;
        axi_bram_ctrl_0_bytes[11] = 4;

        axi_bram_ctrl_1_bytes[4] = 4;
        axi_bram_ctrl_1_bytes[5] = 3;
        axi_bram_ctrl_1_bytes[6] = 2;
        axi_bram_ctrl_1_bytes[7] = 1;
        axi_bram_ctrl_1_bytes[8] = 1;
        axi_bram_ctrl_1_bytes[9] = 2;
        axi_bram_ctrl_1_bytes[10] = 3;
        axi_bram_ctrl_1_bytes[11] = 4;

        axi_bram_ctrl_0[0] = 4;
        printf("Started\n");
        printf("Debug0: %lu Debug1: %lu Debug2: %ld\n", axi_gpio_0[0], axi_gpio_1[0], axi_gpio_2[0]);
        printf("Debug0: %lu Debug1: %lu Debug2: %ld\n", axi_gpio_0[0], axi_gpio_1[0], axi_gpio_2[0]);
        printf("Debug0: %lu Debug1: %lu Debug2: %ld\n", axi_gpio_0[0], axi_gpio_1[0], axi_gpio_2[0]);

        axi_bram_ctrl_0[0] = 0;
        printf("Done?\n");
        printf("LEN : %ld\n", axi_bram_ctrl_0[0]);

        for (unsigned long int i = 0; i < 10; i++)
        {
            printf("BRAM %lu : %ld\n", i, axi_bram_ctrl_2[i]);
        }

        close(fd);
    }
}