// g++ test.cpp -o test `pkg-config --cflags --libs opencv`
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <math.h>
#define GPIO_BYTES 4
#define CDMA_BYTES 40
#define BRAM_BYTES 8192
#define AXI_GPIO_0_ADDR 0x41200000
#define AXI_GPIO_1_ADDR 0x41210000
#define AXI_GPIO_2_ADDR 0x41220000
#define AXI_GPIO_3_ADDR 0x41230000
#define AXI_CDMA_0_ADDR 0x7E200000
#define AXI_CDMA_1_ADDR 0x7E210000
#define AXI_CDMA_2_ADDR 0x7E220000
#define AXI_CDMA_3_ADDR 0x7E230000
#define DDR_0_ADDR 0x0E000000
#define DDR_1_ADDR 0x0F000000
#define DDR_2_ADDR 0x10000000
#define DDR_3_ADDR 0x11000000
#define DDR_4_ADDR 0x12000000
#define DDR_5_ADDR 0x13000000
#define DDR_6_ADDR 0x14000000
#define DDR_7_ADDR 0x15000000
#define DDR_8_ADDR 0x16000000
#define BRAM_0_ADDR 0xC0000000
#define BRAM_1_ADDR 0xC2000000
#define BRAM_2_ADDR 0xC4000000
#define BRAM_3_ADDR 0xC0000000
#define BRAM_4_ADDR 0xC2000000
#define BRAM_5_ADDR 0xC0000000
#define BRAM_6_ADDR 0xC2000000
#define BRAM_7_ADDR 0xC0000000
#define BRAM_8_ADDR 0xC2000000
#define CLEAR_CDMA 0x4
#define DONE_CDMA 0x2
#define STANDBY_CDMA 0x0
#define CLEAR_ZNCC 0x0
using namespace cv;
using namespace std;
using namespace std::chrono;
int fd;
unsigned char *data_t;
unsigned char *data_i_0;
unsigned char *data_i_1;
unsigned char *data_i_2;
unsigned char *data_i_3;
unsigned long int *results_0;
unsigned long int *results_1;
unsigned long int *results_2;
unsigned long int *results_3;
unsigned long int *axi_gpio_0;
unsigned long int *axi_gpio_1;
unsigned long int *axi_gpio_2;
unsigned long int *axi_gpio_3;
unsigned long int *axi_cdma_0;
unsigned long int *axi_cdma_1;
unsigned long int *axi_cdma_2;
unsigned long int *axi_cdma_3;
unsigned long int *ddr_0;
unsigned long int *ddr_1;
unsigned long int *ddr_2;
unsigned long int *ddr_3;
unsigned long int *ddr_4;
unsigned long int *ddr_5;
unsigned long int *ddr_6;
unsigned long int *ddr_7;
unsigned long int *ddr_8;
unsigned long int idx = 4;
unsigned long int num_elem = 4;
int a;
int b;
int c;
int d;
int u;
int v;
int n;
int m;
int w;
int h;
int p;
int q;
unsigned int n_times_m;
Mat i_img;
Mat t_img;
Mat i_img_roi;
Mat t_img_roi;
Mat res;
Rect rect;
ofstream result;
unsigned long int *map_mem(unsigned int bytes, off_t addr)
{
    return (unsigned long int *)mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
}
int open_mem()
{
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        exit(1);
    }
    axi_gpio_0 = map_mem(GPIO_BYTES, AXI_GPIO_0_ADDR);
    axi_gpio_1 = map_mem(GPIO_BYTES, AXI_GPIO_1_ADDR);
    axi_gpio_2 = map_mem(GPIO_BYTES, AXI_GPIO_2_ADDR);
    axi_gpio_3 = map_mem(GPIO_BYTES, AXI_GPIO_3_ADDR);
    axi_cdma_0 = map_mem(CDMA_BYTES, AXI_CDMA_0_ADDR);
    axi_cdma_1 = map_mem(CDMA_BYTES, AXI_CDMA_1_ADDR);
    axi_cdma_2 = map_mem(CDMA_BYTES, AXI_CDMA_2_ADDR);
    axi_cdma_3 = map_mem(CDMA_BYTES, AXI_CDMA_3_ADDR);
    ddr_0 = map_mem(BRAM_BYTES, DDR_0_ADDR);
    ddr_1 = map_mem(BRAM_BYTES, DDR_1_ADDR);
    ddr_2 = map_mem(BRAM_BYTES, DDR_2_ADDR);
    ddr_3 = map_mem(BRAM_BYTES, DDR_3_ADDR);
    ddr_4 = map_mem(BRAM_BYTES, DDR_4_ADDR);
    ddr_5 = map_mem(BRAM_BYTES, DDR_5_ADDR);
    ddr_6 = map_mem(BRAM_BYTES, DDR_6_ADDR);
    ddr_7 = map_mem(BRAM_BYTES, DDR_7_ADDR);
    ddr_8 = map_mem(BRAM_BYTES, DDR_8_ADDR);
}
int close_mem()
{
    close(fd);
}
void print_version()
{
    cout << "version:" << axi_gpio_0[0] << "\n";
}
void clear_signal()
{
    axi_gpio_1[0] = CLEAR_ZNCC;
}
void write_t_data()
{
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_0[6] = DDR_0_ADDR;
    axi_cdma_0[8] = BRAM_0_ADDR;
    axi_cdma_0[10] = num_elem;
    while (!(axi_cdma_0[1] & DONE_CDMA))
    {
    }
}
void write_i_data()
{
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_1[0] = CLEAR_CDMA;
    axi_cdma_2[0] = CLEAR_CDMA;
    axi_cdma_3[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;
    axi_cdma_0[6] = DDR_1_ADDR;
    axi_cdma_0[8] = BRAM_1_ADDR;
    axi_cdma_1[6] = DDR_3_ADDR;
    axi_cdma_1[8] = BRAM_3_ADDR;
    axi_cdma_2[6] = DDR_5_ADDR;
    axi_cdma_2[8] = BRAM_5_ADDR;
    axi_cdma_3[6] = DDR_7_ADDR;
    axi_cdma_3[8] = BRAM_7_ADDR;
    axi_cdma_0[10] = num_elem;
    axi_cdma_1[10] = num_elem;
    axi_cdma_2[10] = num_elem;
    axi_cdma_3[10] = num_elem;
    while (
        !(axi_cdma_0[1] & DONE_CDMA) &&
        !(axi_cdma_1[1] & DONE_CDMA) &&
        !(axi_cdma_2[1] & DONE_CDMA) &&
        !(axi_cdma_3[1] & DONE_CDMA))
    {
    }
}
void set_index(int idx)
{
    axi_gpio_2[0] = idx;
}
void start_signal()
{
    axi_gpio_1[0] = num_elem;
}
void wait_done()
{
    while (axi_gpio_3[0] != 0x7)
    {
    }
}
void read_data()
{
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_1[0] = CLEAR_CDMA;
    axi_cdma_2[0] = CLEAR_CDMA;
    axi_cdma_3[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;
    axi_cdma_0[6] = BRAM_2_ADDR;
    axi_cdma_0[8] = DDR_2_ADDR;
    axi_cdma_1[6] = BRAM_4_ADDR;
    axi_cdma_1[8] = DDR_4_ADDR;
    axi_cdma_2[6] = BRAM_6_ADDR;
    axi_cdma_2[8] = DDR_6_ADDR;
    axi_cdma_3[6] = BRAM_8_ADDR;
    axi_cdma_3[8] = DDR_8_ADDR;
    axi_cdma_0[10] = (w - n) * 4;
    axi_cdma_1[10] = (w - n) * 4;
    axi_cdma_2[10] = (w - n) * 4;
    axi_cdma_3[10] = (w - n) * 4;
    while (
        !(axi_cdma_0[1] & DONE_CDMA) &&
        !(axi_cdma_1[1] & DONE_CDMA) &&
        !(axi_cdma_2[1] & DONE_CDMA) &&
        !(axi_cdma_3[1] & DONE_CDMA))
    {
    }
}
void print_results()
{
    for (int pix = 0; pix < w - n; pix++)
    {
        res.data[(q * w) + pix] = results_0[pix] / 257;
        res.data[(q * w) + pix] = results_1[pix] / 257;
        res.data[(q * w) + pix] = results_2[pix] / 257;
        res.data[(q * w) + pix] = results_3[pix] / 257;
    }
    imwrite("/root/hsa-soc-local/img/dices1.jpg", res);
}
void set_names()
{
    data_t = (unsigned char *)ddr_0;
    data_i_0 = (unsigned char *)ddr_1;
    data_i_1 = (unsigned char *)ddr_3;
    data_i_2 = (unsigned char *)ddr_5;
    data_i_3 = (unsigned char *)ddr_7;
    results_0 = ddr_2;
    results_1 = ddr_4;
    results_2 = ddr_6;
    results_3 = ddr_8;
}
int load_image_file()
{
    i_img = cv::imread("/root/hsa-soc-local/img/dices.jpg", cv::IMREAD_GRAYSCALE);
    t_img = cv::imread("/root/hsa-soc-local/img/dices.jpg", cv::IMREAD_GRAYSCALE);
    // draw the target for inspection
    res = Mat(w - n, h - m, CV_8U, cv::Scalar(0, 0, 0));
    Mat img0 = t_img.clone();
    Point pt1(a, b);
    Point pt2(c, d);
    rectangle(img0, pt1, pt2, cv::Scalar(0, 0, 0));
    imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
    img0.release();
    w = t_img.cols;
    h = t_img.rows;
}
void region_of_interest(int x, int y, int unit)
{
    if (x < 0 || y < 0 || x >= w - n || y >= h - m)
    {
        rect = cv::Rect(u, v, n, m);
        t_img_roi = t_img(rect);
        t_img_roi.convertTo(t_img_roi, CV_8U);
        memcpy(data_t, t_img_roi.data, n_times_m);
    }
    else
    {
        rect = cv::Rect(x, y, n, m);
        i_img_roi = i_img(rect);
        i_img_roi.convertTo(i_img_roi, CV_8U);
        if (unit == 0)
        {
            memcpy(data_i_0, i_img_roi.data, n_times_m);
        }
        else if (unit == 1)
        {
            memcpy(data_i_1, i_img_roi.data, n_times_m);
        }
        else if (unit == 2)
        {
            memcpy(data_i_2, i_img_roi.data, n_times_m);
        }
        else if (unit == 3)
        {
            memcpy(data_i_3, i_img_roi.data, n_times_m);
        }
    }
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
    n_times_m = n * m;
    num_elem = n_times_m;
}
int main()
{
    // general
    open_mem();
    set_names();
    print_version();
    // template
    load_init_file();
    load_image_file();
    region_of_interest(-1, -1, 0);
    write_t_data();
    for (q = 0; q < h - m; q++)
    {
        idx = 0;
        for (p = 0; p < w - n; p++)
        {
            // image parts
            clear_signal();
            region_of_interest(p, q, 0);
            region_of_interest(p, q, 1);
            region_of_interest(p, q, 2);
            region_of_interest(p, q, 3);
            write_i_data();
            set_index(idx);
            start_signal();
            wait_done();
            idx += 4;
        }
        // present results
        read_data();
        print_results();
    }
    close_mem();
    return 0;
}