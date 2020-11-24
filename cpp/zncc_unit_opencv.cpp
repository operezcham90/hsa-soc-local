// g++ zncc_unit_opencv.cpp -o zncc_unit_opencv `pkg-config --cflags --libs opencv`
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
// AXI addresses
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
// AXI pointers
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
// dev mem
int fd;
// constants of PL design
long int conf_clear = 0b1;
long int conf_squared = 0b10;
long int conf_not_squared = 0b00;
long int conf_work = 0b100;
long int conf_wait = 0b000;
unsigned int bram_bytes = 2048 * 4;
unsigned int gpio_bytes = 4;
long int bram_length = 2048;
// zncc variables
int a;
int b;
int c;
int d;
int u;
int v;
int n;
int m;
long int *t_data;
long int *i_data;
cv::Mat img;
// PL variables
long int acc_i;
long int acc_t;
long int acc_cross;
// functions
int load_image_file()
{
    img = cv::imread("/root/hsa-soc-local/img/dices4.jpg", cv::IMREAD_GRAYSCALE);
    // draw the target for inspection
    cv::Mat img0 = img.clone();
    cv::Point pt1(a, b);
    cv::Point pt2(c, d);
    cv::rectangle(img0, pt1, pt2, cv::Scalar(0, 255, 0));
    cv::imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
    img0.release();
}
int region_of_interest()
{
    cv::Rect rect = cv::Rect(a, b, n, m);
    cv::Mat img0 = img(rect);
    // convert chars to long int
    img0.convertTo(img0, CV_32S);
    t_data = (long int *)img0.data;
    i_data = (long int *)img0.data;
}
int load_init_file()
{
    a = 1669 / 4;
    b = 514 / 4;
    c = 1888 / 4;
    d = 664 / 4;
    u = a;
    v = b;
    n = c - a;
    m = d - b;
}
int init_zncc()
{
    load_init_file();
    load_image_file();
    region_of_interest();
}
long int *map_mem(unsigned int bytes, off_t addr)
{
    return (long int *)mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
}
int open_mem()
{
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        exit(1);
    }
    axi_bram_ctrl_0 = map_mem(bram_bytes, axi_bram_ctrl_0_addr);
    axi_bram_ctrl_1 = map_mem(bram_bytes, axi_bram_ctrl_1_addr);
    axi_gpio_0 = map_mem(gpio_bytes, axi_gpio_0_addr);
    axi_gpio_1 = map_mem(gpio_bytes, axi_gpio_1_addr);
    axi_gpio_2 = map_mem(gpio_bytes, axi_gpio_2_addr);
    axi_gpio_3 = map_mem(gpio_bytes, axi_gpio_3_addr);
    axi_gpio_4 = map_mem(gpio_bytes, axi_gpio_4_addr);
    axi_gpio_5 = map_mem(gpio_bytes, axi_gpio_5_addr);
    axi_gpio_6 = map_mem(gpio_bytes, axi_gpio_6_addr);
    axi_gpio_7 = map_mem(gpio_bytes, axi_gpio_7_addr);
}
int close_mem()
{
    close(fd);
}
int clear_acc()
{
    axi_gpio_4[0] = conf_clear | conf_wait;
    axi_gpio_4[0] = conf_clear | conf_work;
    acc_i = axi_gpio_7[0];
    acc_t = axi_gpio_5[0];
    acc_cross = axi_gpio_6[0];
}
int set_avg(long int i_avg, long int t_avg)
{
    axi_gpio_2[0] = i_avg;
    axi_gpio_3[0] = t_avg;
}
long int get_acc(long int squared_or_not)
{
    int img_length = n * m;
    int rounds = img_length / bram_length;
    int remain = img_length - (bram_length * rounds);
    // for each round
    for (int r = 0; r < rounds + 1; r++)
    {
        int limit = bram_length;
        if (r == rounds)
        {
            limit = remain;
        }
        // copy a block of data to PL
        memcpy(axi_bram_ctrl_0, i_data + (r * bram_length), limit * 4);
        memcpy(axi_bram_ctrl_1, t_data + (r * bram_length), limit * 4);
        // get acc, still not parallel
        for (long int i = 0; i < limit; i++)
        {
            axi_gpio_0[0] = i << 2;
            axi_gpio_1[0] = i << 2;
            axi_gpio_4[0] = conf_wait | squared_or_not;
            axi_gpio_4[0] = conf_work | squared_or_not;
        }
    }
    acc_i = axi_gpio_7[0];
    acc_t = axi_gpio_5[0];
    acc_cross = axi_gpio_6[0];
}
int main()
{
    init_zncc();
    open_mem();
    // get averages
    clear_acc();
    set_avg(0, 0);
    get_acc(conf_not_squared);
    long int avg_i = acc_i / (m * n);
    long int avg_t = acc_t / (m * n);
    // get sum of squared err and cross correlation
    clear_acc();
    set_avg(avg_i, avg_t);
    get_acc(conf_squared);
    double err_i = acc_i * 1.0;
    double err_t = acc_t * 1.0;
    double corr = acc_cross * 1.0;
    double zncc = corr / sqrt(err_i * err_t);
    std::cout << "zncc: " << zncc << "\n";
    close_mem();
    return 0;
}