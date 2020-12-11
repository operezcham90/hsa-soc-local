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
off_t axi_gpio_5_addr = 0x41250000;
off_t axi_gpio_6_addr = 0x41260000;
long int *axi_bram_ctrl_0;
long int *axi_bram_ctrl_1;
long int *axi_gpio_0;
long int *axi_gpio_1;
long int *axi_gpio_2;
long int *axi_gpio_3;
long int *axi_gpio_4;
long int *axi_gpio_5;
long int *axi_gpio_6;
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
        axi_gpio_5 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_5_addr);
        axi_gpio_6 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_6_addr);
        // init BRAM
        int length = 3;
        for (long int i = 0; i < length; i++)
        {
            axi_bram_ctrl_0[i] = i;
            axi_bram_ctrl_1[i] = i * 2;
        }
        printf("Data written\n");
        // Constants
        long int CLEAR = 0b10;
        long int SQUARED = 0b100;
        long int WORK = 0b1;
        long int WAIT = 0b0;
        // Clear previous data
        axi_gpio_3[0] = CLEAR | WAIT;
        axi_gpio_3[0] = CLEAR | WORK;
        axi_gpio_1[0] = 0;
        axi_gpio_2[0] = 0;
        // Acc average
        printf("Start acc i: %ld\n", axi_gpio_4[0]);
        printf("Start acc t: %ld\n", axi_gpio_5[0]);
        printf("Start acc cross: %ld\n", axi_gpio_6[0]);
        for (int i = 0; i < length; i++)
        {
            axi_gpio_0[0] = i;
            axi_gpio_3[0] = WAIT;
            axi_gpio_3[0] = WAIT;
            axi_gpio_3[0] = WAIT;
            axi_gpio_3[0] = WAIT;
            axi_gpio_3[0] = WORK;
            axi_gpio_3[0] = WORK;
            axi_gpio_3[0] = WORK;
            axi_gpio_3[0] = WORK;
        }
        printf("End acc i: %ld\n", axi_gpio_4[0]);
        printf("End acc t: %ld\n", axi_gpio_5[0]);
        printf("End acc cross: %ld\n", axi_gpio_6[0]);
        long int i_avg = axi_gpio_4[0] / length;
        long int t_avg = axi_gpio_5[0] / length;
        // Clear previous data
        axi_gpio_3[0] = CLEAR | WAIT | SQUARED;
        axi_gpio_3[0] = CLEAR | WORK | SQUARED;
        axi_gpio_1[0] = 0;
        axi_gpio_2[0] = 0;
        // Acc squared
        printf("Start acc i: %ld\n", axi_gpio_4[0]);
        printf("Start acc t: %ld\n", axi_gpio_5[0]);
        printf("Start acc cross: %ld\n", axi_gpio_6[0]);
        for (int i = 0; i < length; i++)
        {
            axi_gpio_0[0] = i;
            axi_gpio_3[0] = WAIT | SQUARED;
            axi_gpio_3[0] = WORK | SQUARED;
        }
        printf("End acc i: %ld\n", axi_gpio_4[0]);
        printf("End acc t: %ld\n", axi_gpio_5[0]);
        printf("End acc cross: %ld\n", axi_gpio_6[0]);
        // Clear previous data
        axi_gpio_3[0] = CLEAR | WAIT | SQUARED;
        axi_gpio_3[0] = CLEAR | WORK | SQUARED;
        axi_gpio_1[0] = i_avg;
        axi_gpio_2[0] = t_avg;
        // Acc squared errors
        printf("Start acc i: %ld\n", axi_gpio_4[0]);
        printf("Start acc t: %ld\n", axi_gpio_5[0]);
        printf("Start acc cross: %ld\n", axi_gpio_6[0]);
        for (int i = 0; i < length; i++)
        {
            axi_gpio_0[0] = i;
            axi_gpio_3[0] = WAIT | SQUARED;
            axi_gpio_3[0] = WORK | SQUARED;
        }
        printf("End acc i: %ld\n", axi_gpio_4[0]);
        printf("End acc t: %ld\n", axi_gpio_5[0]);
        printf("End acc cross: %ld\n", axi_gpio_6[0]);
        // end
        close(fd);
    }
}