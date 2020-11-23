// g++ zncc_unit_opencv.cpp -o zncc_unit_opencv `pkg-config --cflags --libs opencv`
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
// addresses from vivado block design
off_t axi_bram_ctrl_0_addr = 0x40000000;
off_t axi_bram_ctrl_1_addr = 0x42000000;
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
off_t axi_gpio_2_addr = 0x41220000;
off_t axi_gpio_3_addr = 0x41230000;
off_t axi_gpio_4_addr = 0x41240000;
off_t axi_gpio_5_addr = 0x41260000;
off_t axi_gpio_6_addr = 0x41270000;
off_t axi_gpio_7_addr = 0x41250000;
long int *axi_bram_ctrl_0;
long int *axi_bram_ctrl_1;
long int *axi_gpio_0;
long int *axi_gpio_1;
long int *axi_gpio_2;
long int *axi_gpio_3;
long int *axi_gpio_4;
long int *axi_gpio_5;
long int *axi_gpio_6;
long int *axi_gpio_7;
int main()
{
    // known object location (x,y)
    int a = 1669 / 4;
    int b = 514 / 4;
    int c = 1888 / 4;
    int d = 664 / 4;
    // load image file
    int IMREAD_REDUCED_GRAYSCALE_8 = 64;
    cv::Mat img = cv::imread("/root/hsa-soc-local/img/dices4.jpg", cv::IMREAD_GRAYSCALE);
    std::cout << "Cols: " << img.cols << std::endl;
    std::cout << "Rows: " << img.rows << std::endl;
    // draw target
    cv::Mat img0 = img.clone();
    cv::Point pt1(a, b);
    cv::Point pt2(c, d);
    cv::rectangle(img0, pt1, pt2, cv::Scalar(0, 255, 0));
    cv::imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
    // region of interest
    cv::Rect rect = cv::Rect(a, b, c - a, d - b);
    cv::Mat imgr = img(rect);
    imgr.convertTo(imgr, CV_32S);
    long int *t = (long int *)imgr.data;
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
        axi_gpio_7 = (long int *)mmap(NULL, gpio_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, axi_gpio_7_addr);
        // init BRAM
        int length = bram_size / 4;
        for (long int i = 0; i < length; i++)
        {
            axi_bram_ctrl_0[i] = i;
            axi_bram_ctrl_1[i] = i;
        }
        printf("Data written\n");
        // Constants
        long int CLEAR = 0b1;
        long int SQUARED = 0b10;
        long int WORK = 0b100;
        long int WAIT = 0b000;
        // Clear previous data
        axi_gpio_4[0] = CLEAR | WAIT;
        axi_gpio_4[0] = CLEAR | WORK;
        axi_gpio_2[0] = 0;
        axi_gpio_3[0] = 0;
        // Acc average
        printf("Start acc i: %ld\n", axi_gpio_7[0]);
        printf("Start acc t: %ld\n", axi_gpio_5[0]);
        printf("Start acc cross: %ld\n", axi_gpio_6[0]);
        long int i = 0;
        axi_gpio_0[0] = i << 2;
        axi_gpio_1[0] = i << 2;
        axi_gpio_4[0] = WAIT;
        axi_gpio_4[0] = WORK;
        printf("End acc i: %ld\n", axi_gpio_7[0]);
        printf("End acc t: %ld\n", axi_gpio_5[0]);
        printf("End acc cross: %ld\n", axi_gpio_6[0]);
        // Acc average
        printf("Start acc i: %ld\n", axi_gpio_7[0]);
        printf("Start acc t: %ld\n", axi_gpio_5[0]);
        printf("Start acc cross: %ld\n", axi_gpio_6[0]);
        i = length - 1;
        axi_gpio_0[0] = i << 2;
        axi_gpio_1[0] = i << 2;
        axi_gpio_4[0] = WAIT;
        axi_gpio_4[0] = WORK;
        printf("End acc i: %ld\n", axi_gpio_7[0]);
        printf("End acc t: %ld\n", axi_gpio_5[0]);
        printf("End acc cross: %ld\n", axi_gpio_6[0]);
        // end
        close(fd);
    }
    return 0;
}