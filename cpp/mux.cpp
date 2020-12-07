#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
// addresses from vivado block design
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
long int *axi_gpio_0;
long int *axi_gpio_1;
int main()
{
    // 32 bits
    unsigned int gpio_size = 0x4;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_gpio_0 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_0_addr);
        axi_gpio_1 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_1_addr);
        //A
        long int A = 0x0001;
        //B
        long int B = 0x0200;
        // selA
        long int selA = 0;
        long int selB = 0b10000000000000000;

        // write
        axi_gpio_0[0] = A | B | selA;
        // read
        printf("mux: %ld\n", axi_gpio_1[0]);

        // write
        axi_gpio_0[0] = A | B | selB;
        // read
        printf("mux: %ld\n", axi_gpio_1[0]);

        close(fd);
    }
}