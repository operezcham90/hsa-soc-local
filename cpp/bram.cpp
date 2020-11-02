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
off_t axi_gpio_2_addr = 0x41220000;
off_t axi_gpio_3_addr = 0x41230000;
off_t axi_gpio_4_addr = 0x41240000;
long int *axi_bram_ctrl_0;
long int *axi_bram_ctrl_1;
long int *axi_gpio_0;
long int *axi_gpio_1;
long int *axi_gpio_2;
long int *axi_gpio_3;
long int *axi_gpio_4;
int main()
{
    // size of bram default block
    unsigned int bram_size = 0x8000;
    // 32 bits
    unsigned int gpio_size = 0x4;
    int fd;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
    {
        axi_bram_ctrl_0 = (long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_bram_ctrl_0_addr);
        axi_bram_ctrl_1 = (long int *)mmap(NULL, bram_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_bram_ctrl_1_addr);
        axi_gpio_0 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_0_addr);
        axi_gpio_1 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_1_addr);
        axi_gpio_2 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_2_addr);
        axi_gpio_3 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_3_addr);
        axi_gpio_4 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_4_addr);

        // init BRAM
        for (long int i = 0; i < 1000; i++)
        {
            axi_bram_ctrl_0[i] = i;
        }
        printf("Data written\n");
        // verfy BRAM
        int confirmed = 1;
        for (long int i = 0; i < 10; i++)
        {
            if (axi_bram_ctrl_1[i] != i)
            {
                confirmed = 0;
            }
        }
        printf("Data read\n");
        printf("Confirmed: %d\n", confirmed);

        // verfy BRAM low level
        confirmed = 1;
        for (long int i = 0; i < 10; i++)
        {
            axi_gpio_0[0] = i << 2;
            if (axi_gpio_3[0] != i)
            {
                confirmed = 0;
            }
        }
        printf("Data read\n");
        printf("Confirmed: %d\n", confirmed);

        // Accumulator
        long int WAIT = 0;
        long int CLEAR = 2;
        long int WORK = 1;
        printf("Value: %ld\n", axi_gpio_1[0]);
        axi_gpio_4[0] = 2;
        axi_gpio_2[0] = CLEAR | WAIT;
        axi_gpio_2[0] = CLEAR | WAIT;
        axi_gpio_2[0] = CLEAR |WORK;
        axi_gpio_2[0] = CLEAR |WORK;
        axi_gpio_2[0] = WAIT;
        axi_gpio_2[0] = WAIT;
        axi_gpio_2[0] = WORK;
        axi_gpio_2[0] = WORK;
        axi_gpio_2[0] = WAIT;
        axi_gpio_2[0] = WAIT;
        axi_gpio_2[0] = WORK;
        axi_gpio_2[0] = WORK;
        printf("Value: %ld\n", axi_gpio_1[0]);

        close(fd);
    }
}