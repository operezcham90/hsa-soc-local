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
#define I_FILE "/root/hsa-soc-local/img/temp_i.jpg"
#define T_FILE "/root/hsa-soc-local/img/temp_t.jpg"
#define TOP_L_X_FILE "/root/hsa-soc-local/img/temp_tlx.txt"
#define TOP_L_Y_FILE "/root/hsa-soc-local/img/temp_tly.txt"
#define BOTTOM_R_X_FILE "/root/hsa-soc-local/img/temp_brx.txt"
#define BOTTOM_R_Y_FILE "/root/hsa-soc-local/img/temp_bry.txt"
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
#define DDR_9_ADDR 0x17000000
#define DDR_10_ADDR 0x18000000
#define DDR_11_ADDR 0x19000000
#define DDR_12_ADDR 0x1A000000
#define DDR_13_ADDR 0x1B000000
#define DDR_14_ADDR 0x1C000000
#define DDR_15_ADDR 0x1D000000
#define DDR_16_ADDR 0x1E000000
#define BRAM_0_ADDR 0xC0000000
#define BRAM_1_ADDR 0xC2000000
#define BRAM_2_ADDR 0xC4000000
#define BRAM_3_ADDR 0xC0000000
#define BRAM_4_ADDR 0xC2000000
#define BRAM_5_ADDR 0xC0000000
#define BRAM_6_ADDR 0xC2000000
#define BRAM_7_ADDR 0xC0000000
#define BRAM_8_ADDR 0xC2000000
#define BRAM_9_ADDR 0xC6000000
#define BRAM_10_ADDR 0xC8000000
#define BRAM_11_ADDR 0xC4000000
#define BRAM_12_ADDR 0xC6000000
#define BRAM_13_ADDR 0xC4000000
#define BRAM_14_ADDR 0xC6000000
#define BRAM_15_ADDR 0xC4000000
#define BRAM_16_ADDR 0xC6000000
#define CLEAR_CDMA 0x4
#define DONE_CDMA 0x2
#define STANDBY_CDMA 0x0
#define CLEAR_ZNCC 0x0
#define ZNCC_DONE 0x7
using namespace cv;
using namespace std;
using namespace std::chrono;
int fd;
unsigned char *data_t;
unsigned char *data_i_0;
unsigned char *data_i_1;
unsigned char *data_i_2;
unsigned char *data_i_3;
unsigned char *data_i_4;
unsigned char *data_i_5;
unsigned char *data_i_6;
unsigned char *data_i_7;
unsigned long int *results_0;
unsigned long int *results_1;
unsigned long int *results_2;
unsigned long int *results_3;
unsigned long int *results_4;
unsigned long int *results_5;
unsigned long int *results_6;
unsigned long int *results_7;
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
unsigned long int *ddr_9;
unsigned long int *ddr_10;
unsigned long int *ddr_11;
unsigned long int *ddr_12;
unsigned long int *ddr_13;
unsigned long int *ddr_14;
unsigned long int *ddr_15;
unsigned long int *ddr_16;
unsigned long int idx;
unsigned long int num_elem;
unsigned int parallel_units = 8;
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
int n_times_m;
int h_minus_m;
int w_minus_n;
int res_bytes_per_unit;
Mat i_img;
Mat t_img;
Mat i_img_roi;
Mat t_img_roi;
Mat res;
Rect rect;
ofstream result;
unsigned long int time_write_t = 0;
unsigned long int time_write_i = 0;
unsigned long int time_read_res = 0;
unsigned long int time_read_file = 0;
unsigned long int time_slice_data = 0;
unsigned long int time_work = 0;
unsigned long int tests = 0;
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
    ddr_9 = map_mem(BRAM_BYTES, DDR_9_ADDR);
    ddr_10 = map_mem(BRAM_BYTES, DDR_10_ADDR);
    ddr_11 = map_mem(BRAM_BYTES, DDR_11_ADDR);
    ddr_12 = map_mem(BRAM_BYTES, DDR_12_ADDR);
    ddr_13 = map_mem(BRAM_BYTES, DDR_13_ADDR);
    ddr_14 = map_mem(BRAM_BYTES, DDR_14_ADDR);
    ddr_15 = map_mem(BRAM_BYTES, DDR_15_ADDR);
    ddr_16 = map_mem(BRAM_BYTES, DDR_16_ADDR);
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
    auto start = high_resolution_clock::now();
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_0[6] = DDR_0_ADDR;
    axi_cdma_0[8] = BRAM_0_ADDR;
    axi_cdma_0[10] = num_elem;
    while (!(axi_cdma_0[1] & DONE_CDMA))
    {
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_write_t += duration.count();
}
void write_i_data()
{
    auto start = high_resolution_clock::now();
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
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_1[0] = CLEAR_CDMA;
    axi_cdma_2[0] = CLEAR_CDMA;
    axi_cdma_3[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;
    axi_cdma_0[6] = DDR_9_ADDR;
    axi_cdma_0[8] = BRAM_9_ADDR;
    axi_cdma_1[6] = DDR_11_ADDR;
    axi_cdma_1[8] = BRAM_11_ADDR;
    axi_cdma_2[6] = DDR_13_ADDR;
    axi_cdma_2[8] = BRAM_13_ADDR;
    axi_cdma_3[6] = DDR_15_ADDR;
    axi_cdma_3[8] = BRAM_15_ADDR;
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
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_write_i += duration.count();
}
void work(int idx)
{
    auto start = high_resolution_clock::now();
    axi_gpio_2[0] = idx;
    axi_gpio_1[0] = num_elem;
    while (axi_gpio_3[0] != ZNCC_DONE)
    {
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_work += duration.count();
}
void read_data()
{
    auto start = high_resolution_clock::now();
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
    axi_cdma_0[10] = res_bytes_per_unit;
    axi_cdma_1[10] = res_bytes_per_unit;
    axi_cdma_2[10] = res_bytes_per_unit;
    axi_cdma_3[10] = res_bytes_per_unit;
    while (
        !(axi_cdma_0[1] & DONE_CDMA) &&
        !(axi_cdma_1[1] & DONE_CDMA) &&
        !(axi_cdma_2[1] & DONE_CDMA) &&
        !(axi_cdma_3[1] & DONE_CDMA))
    {
    }
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_1[0] = CLEAR_CDMA;
    axi_cdma_2[0] = CLEAR_CDMA;
    axi_cdma_3[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;
    axi_cdma_0[6] = BRAM_10_ADDR;
    axi_cdma_0[8] = DDR_10_ADDR;
    axi_cdma_1[6] = BRAM_12_ADDR;
    axi_cdma_1[8] = DDR_12_ADDR;
    axi_cdma_2[6] = BRAM_14_ADDR;
    axi_cdma_2[8] = DDR_14_ADDR;
    axi_cdma_3[6] = BRAM_16_ADDR;
    axi_cdma_3[8] = DDR_16_ADDR;
    axi_cdma_0[10] = res_bytes_per_unit;
    axi_cdma_1[10] = res_bytes_per_unit;
    axi_cdma_2[10] = res_bytes_per_unit;
    axi_cdma_3[10] = res_bytes_per_unit;
    while (
        !(axi_cdma_0[1] & DONE_CDMA) &&
        !(axi_cdma_1[1] & DONE_CDMA) &&
        !(axi_cdma_2[1] & DONE_CDMA) &&
        !(axi_cdma_3[1] & DONE_CDMA))
    {
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_read_res += duration.count();
}
void print_results()
{
    int pix_idx = 0;
    int row = w_minus_n * q;
    float *data = (float *)res.data;
    int pix = 0;
    for (pix = 0; pix <= w_minus_n; pix += parallel_units)
    {
        data[row + pix] = ((float)results_0[pix_idx]) * 0.00389099121;     //results_0[pix_idx] >> 9;
        data[row + pix + 1] = ((float)results_1[pix_idx]) * 0.00389099121; //results_1[pix_idx] >> 9;
        data[row + pix + 2] = ((float)results_2[pix_idx]) * 0.00389099121; //results_2[pix_idx] >> 9;
        data[row + pix + 3] = ((float)results_3[pix_idx]) * 0.00389099121; //results_3[pix_idx] >> 9;
        data[row + pix + 4] = ((float)results_4[pix_idx]) * 0.00389099121; //results_4[pix_idx] >> 9;
        data[row + pix + 5] = ((float)results_5[pix_idx]) * 0.00389099121; //results_5[pix_idx] >> 9;
        data[row + pix + 6] = ((float)results_6[pix_idx]) * 0.00389099121; //results_6[pix_idx] >> 9;
        data[row + pix + 7] = ((float)results_7[pix_idx]) * 0.00389099121; //results_7[pix_idx] >> 9;
        pix_idx++;
    }
    for (pix -= 8; pix <= w_minus_n; pix++)
    {
        data[row + pix] = 0.0;
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
    data_i_4 = (unsigned char *)ddr_9;
    data_i_5 = (unsigned char *)ddr_11;
    data_i_6 = (unsigned char *)ddr_13;
    data_i_7 = (unsigned char *)ddr_15;
    results_0 = ddr_2;
    results_1 = ddr_4;
    results_2 = ddr_6;
    results_3 = ddr_8;
    results_4 = ddr_10;
    results_5 = ddr_12;
    results_6 = ddr_14;
    results_7 = ddr_16;
}
int load_image_file()
{
    auto start = high_resolution_clock::now();
    i_img = cv::imread(I_FILE, cv::IMREAD_GRAYSCALE);
    t_img = cv::imread(T_FILE, cv::IMREAD_GRAYSCALE);
    // draw the target for inspection
    Mat img0 = t_img.clone();
    Point pt1(a, b);
    Point pt2(c, d);
    rectangle(img0, pt1, pt2, cv::Scalar(0, 0, 0));
    cv::imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
    img0.release();
    w = t_img.cols;
    h = t_img.rows;
    h_minus_m = h - m;
    w_minus_n = w - n;
    res_bytes_per_unit = w_minus_n * 4 / parallel_units;
    res = Mat(h_minus_m, w_minus_n, CV_32FC1, cv::Scalar(0, 0, 0));
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_read_file += duration.count();
}
void region_of_interest(int x, int y, int unit)
{
    auto start = high_resolution_clock::now();
    if (x < 0 || y < 0)
    {
        rect = cv::Rect(u, v, n, m);
        t_img_roi = t_img(rect);
        t_img_roi.convertTo(t_img_roi, CV_8U);
        memcpy(data_t, t_img_roi.data, n_times_m);
    }
    else if (x >= w_minus_n || y >= h_minus_m)
    {
        i_img_roi = Mat(h, w, CV_8U, cv::Scalar(0, 0, 0));
    }
    else
    {
        rect = cv::Rect(x, y, n, m);
        i_img_roi = i_img(rect);
        i_img_roi.convertTo(i_img_roi, CV_8U);
    }
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
    else if (unit == 4)
    {
        memcpy(data_i_4, i_img_roi.data, n_times_m);
    }
    else if (unit == 5)
    {
        memcpy(data_i_5, i_img_roi.data, n_times_m);
    }
    else if (unit == 6)
    {
        memcpy(data_i_6, i_img_roi.data, n_times_m);
    }
    else if (unit == 7)
    {
        memcpy(data_i_7, i_img_roi.data, n_times_m);
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_slice_data += duration.count();
}
int load_init_file()
{
    fstream file_a(TOP_L_X_FILE, std::ios_base::in);
    fstream file_b(TOP_L_Y_FILE, std::ios_base::in);
    fstream file_c(BOTTOM_R_X_FILE, std::ios_base::in);
    fstream file_d(BOTTOM_R_Y_FILE, std::ios_base::in);
    file_a >> a;
    file_b >> b;
    file_c >> c;
    file_d >> d;
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
    region_of_interest(-1, -1, -1);
    write_t_data();
    for (q = 0; q < h_minus_m; q++)
    {
        idx = 0;
        for (p = 0; p < w_minus_n; p += 8)
        {
            // image parts
            clear_signal();
            region_of_interest(p, q, 0);
            region_of_interest(p + 1, q, 1);
            region_of_interest(p + 2, q, 2);
            region_of_interest(p + 3, q, 3);
            region_of_interest(p + 4, q, 4);
            region_of_interest(p + 5, q, 5);
            region_of_interest(p + 6, q, 6);
            region_of_interest(p + 7, q, 7);
            write_i_data();
            work(idx);
            idx += 4;
            tests += 8;
        }
        // present results
        read_data();
        print_results();
        //cout << "row: " << q << "\n";
    }
    close_mem();
    Point min_loc;
    Point max_loc;
    double min;
    double max;
    minMaxLoc(res, &min, &max, &min_loc, &max_loc);
    close_mem();
    cout << "Write t: " << time_write_t << " us\n";
    cout << "Write i: " << time_write_i << " us\n";
    cout << "Read res: " << time_read_res << " us\n";
    cout << "Read file: " << time_read_file << " us\n";
    cout << "Slice data: " << time_slice_data << " us\n";
    cout << "Work: " << time_work << " us\n";
    cout << "Tests: " << tests << "\n";
    cout << "Max: " << max << "\n";
    cout << "u: " << max_loc.x << "\n";
    cout << "v: " << max_loc.y << "\n";
    return 0;
}