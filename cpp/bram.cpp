#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
off_t axi_bram_ctrl_0_addr = 0x40000000;
off_t axi_bram_ctrl_1_addr = 0x42000000;
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
long long int *axi_bram_ctrl_0;
long long int *axi_bram_ctrl_1;
long long int *axi_gpio_0;
long long int *axi_gpio_1;
int main()
{
    unsigned int bram_size = 0x8000;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        bram_write = (long long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, bram_write_addr);
        bram_read_0 = (long long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, bram_read_0_addr);
        bram_read_1 = (long long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, bram_read_1_addr);
        bram_write[0] = 0;
        printf("%lld\n", bram_write[0]);
        printf("%lld\n", bram_read_0[0]);
        printf("%lld\n", bram_read_1[0]);
        bram_write[0] = 1;
        printf("%lld\n", bram_write[0]);
        printf("%lld\n", bram_read_0[0]);
        printf("%lld\n", bram_read_1[0]);
        close(fd);
    }
}