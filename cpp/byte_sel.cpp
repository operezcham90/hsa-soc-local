#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
// addresses from vivado block design
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
    // size of bram default block
    unsigned int bram_size = 2048 * 4;
    // 32 bits
    unsigned int gpio_size = 4;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_bram_ctrl_0 = (long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_bram_ctrl_0_addr);
        axi_bram_ctrl_1 = (long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_bram_ctrl_1_addr);
        axi_gpio_0 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_0_addr);
        axi_gpio_1 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_1_addr);
        // init BRAM
        axi_bram_ctrl_0[0] = 0x01020304;
        printf("Data written\n");
        // set address
        axi_gpio_0[0] = 0;
        // read byte
        printf("Byte 0: %ld\n", axi_gpio_1[0]);
        // set address
        axi_gpio_0[0] = 1;
        // read byte
        printf("Byte 1: %ld\n", axi_gpio_1[0]);
        // set address
        axi_gpio_0[0] = 2;
        // read byte
        printf("Byte 2: %ld\n", axi_gpio_1[0]);
        // set address
        axi_gpio_0[0] = 3;
        // read byte
        printf("Byte 3: %ld\n", axi_gpio_1[0]);
        // init BRAM
        unsigned char *axi_bram_ctrl_0_bytes = (unsigned char *)axi_bram_ctrl_0;
        axi_bram_ctrl_0_bytes[0] = 8;
        axi_bram_ctrl_0_bytes[1] = 7;
        axi_bram_ctrl_0_bytes[2] = 6;
        axi_bram_ctrl_0_bytes[3] = 5;
        printf("Data written\n");
        // set address
        axi_gpio_0[0] = 0;
        // read byte
        printf("Byte 0: %ld\n", axi_gpio_1[0]);
        // set address
        axi_gpio_0[0] = 1;
        // read byte
        printf("Byte 1: %ld\n", axi_gpio_1[0]);
        // set address
        axi_gpio_0[0] = 2;
        // read byte
        printf("Byte 2: %ld\n", axi_gpio_1[0]);
        // set address
        axi_gpio_0[0] = 3;
        // read byte
        printf("Byte 3: %ld\n", axi_gpio_1[0]);
        // end
        close(fd);
    }
}