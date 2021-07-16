// https://github.com/operezcham90/VideoAbejasGPU/blob/master/ncc/gpu_clean/fitness.c
// https://github.com/operezcham90/VideoAbejasGPU/blob/master/ncc/gpu_clean/fitness_kernel_bee.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <math.h>
#define I_FILE "/root/hsa-soc-local/img/temp_i.txt"
#define T_FILE "/root/hsa-soc-local/img/temp_t.txt"
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
float *data;
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
unsigned long int num_elem_real;
unsigned int parallel_units = 4;
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
Mat t_img_roi_resize;
Mat i_img_roi_resize;
Mat res;
Rect rect;
ofstream result;
std::string i_path;
std::string t_path;
//unsigned long int *gamma_arr;
unsigned long int time_write_t = 0;
unsigned long int time_write_i = 0;
unsigned long int time_read_res = 0;
unsigned long int time_read_file = 0;
unsigned long int time_slice_data = 0;
unsigned long int time_work = 0;
unsigned long int tests = 0;
// bees
CvRNG rng = cvRNG(0xffffffff);
double *mu_e_bees;
signed long int *mu_e_obj;
double *lambda_e_bees;
signed long int *lambda_e_obj;
double *mu_f_bees;
signed long int *mu_f_obj;
double *lambda_f_bees;
signed long int *lambda_f_obj;
double *mu_lambda_bees;
signed long int *mu_lambda_obj;
int *mu_lambda_order;
int num_bees = 64;
int num_bees_comp = 64 * 2;
int *recruiter;
double eta_m = 25;
double eta_c = 2;
int max_gen = 2;
int rate_rand = 0;
int rate_mut = 0;
int rate_cross = 0;
double get_beta(double u)
{
    double beta;
    u = 1 - u;
    double p = 1.0 / (eta_c + 1.0);

    if (u <= 0.5)
    {
        beta = pow(2.0 * u, p);
    }
    else
    {
        beta = pow((1.0 / (2.0 * (1.0 - u))), p);
    }
    return beta;
}
double get_delta(double u)
{
    double delta;
    if (u <= 0.5)
    {
        delta = pow(2.0 * u, (1.0 / (eta_m + 1.0))) - 1.0;
    }
    else
    {
        delta = 1.0 - pow(2.0 * (1.0 - u), (1.0 / (eta_m + 1.0)));
    }
    return delta;
}
void mutation(double *bees, int bee, double *limits)
{
    double x, delta;
    int site;

    // Mutation for each component of the individual
    for (site = 0; site < 2; site++)
    {
        // Get limits, based on component
        double upper = limits[site * 4 + 2];
        double lower = limits[site * 4 + 3];

        // Get the value
        x = bees[bee * 2 + site];

        // Get delta
        delta = get_delta(cvRandReal(&rng));

        // Apply mutation
        if (delta >= 0)
        {
            bees[bee * 2 + site] += delta * (upper - x);
        }
        else
        {
            bees[bee * 2 + site] += delta * (x - lower);
        }

        // Absolute limits
        if (bees[bee * 2 + site] < lower)
            bees[bee * 2 + site] = lower;
        if (bees[bee * 2 + site] > upper)
            bees[bee * 2 + site] = upper;
    }
}
void create_children(
    double p1,
    double p2,
    double *c1,
    double *c2,
    double low,
    double high)
{
    double beta = get_beta(cvRandReal(&rng));

    double v2 = 0.5 * ((1 - beta) * p1 + (1 + beta) * p2);
    double v1 = 0.5 * ((1 + beta) * p1 + (1 - beta) * p2);

    if (v2 < low)
        v2 = low;
    if (v2 > high)
        v2 = high;
    if (v1 < low)
        v1 = low;
    if (v1 > high)
        v1 = high;

    *c2 = v2;
    *c1 = v1;
}
void cross_over(double *mu_bees, double *lambda_bees, int parent1, int parent2, int child1, int child2, double *limits)
{
    int site;
    int nvar_real = 2;
    for (site = 0; site < nvar_real; site++)
    {
        double lower = limits[site * 4 + 3];
        double upper = limits[site * 4 + 2];

        create_children(
            mu_bees[parent1 * 2 + site],
            mu_bees[parent2 * 2 + site],
            &lambda_bees[child1 * 2 + site],
            &lambda_bees[child2 * 2 + site],
            lower,
            upper);
    }
}
void initial_random_pop(double *mu_bees, double *limits, int first, int last)
{
    for (int bee = first; bee <= last; bee++)
    {
        double a = cvRandReal(&rng);
        double up = limits[0];
        double low = limits[1];
        if (up > limits[2])
            up = limits[2];
        if (low < limits[3])
            low = limits[3];
        mu_bees[bee * 2] = (a * (up - low)) + low;

        double b = cvRandReal(&rng);
        up = limits[4];
        low = limits[5];
        if (up > limits[6])
            up = limits[6];
        if (low < limits[7])
            low = limits[7];
        mu_bees[bee * 2 + 1] = (b * (up - low)) + low;
    }
}
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
    //auto start = high_resolution_clock::now();
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_0[6] = DDR_0_ADDR;
    axi_cdma_0[8] = BRAM_0_ADDR;
    axi_cdma_0[10] = num_elem;
    while (!(axi_cdma_0[1] & DONE_CDMA))
    {
        axi_cdma_0[0] = STANDBY_CDMA;
    }
    axi_cdma_0[0] = STANDBY_CDMA;
    /*auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_write_t += duration.count();*/
}
void write_i_data()
{
    //auto start = high_resolution_clock::now();
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
        axi_cdma_0[0] = STANDBY_CDMA;
        axi_cdma_1[0] = STANDBY_CDMA;
        axi_cdma_2[0] = STANDBY_CDMA;
        axi_cdma_3[0] = STANDBY_CDMA;
    }

    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;

    /*axi_cdma_0[0] = CLEAR_CDMA;
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
        axi_cdma_0[0] = STANDBY_CDMA;
        axi_cdma_1[0] = STANDBY_CDMA;
        axi_cdma_2[0] = STANDBY_CDMA;
        axi_cdma_3[0] = STANDBY_CDMA;
    }

    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;*/

    /*auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_write_i += duration.count();*/
}
void work(int idx)
{
    //auto start = high_resolution_clock::now();
    axi_gpio_2[0] = idx;
    axi_gpio_1[0] = num_elem;
    while (axi_gpio_3[0] != ZNCC_DONE)
    {
        axi_gpio_1[0] = num_elem;
    }
    axi_gpio_1[0] = num_elem;
    /*auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_work += duration.count();*/
}
void read_data()
{
    //auto start = high_resolution_clock::now();
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
        axi_cdma_0[0] = STANDBY_CDMA;
        axi_cdma_1[0] = STANDBY_CDMA;
        axi_cdma_2[0] = STANDBY_CDMA;
        axi_cdma_3[0] = STANDBY_CDMA;
    }

    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;

    /*axi_cdma_0[0] = CLEAR_CDMA;
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
        axi_cdma_0[0] = STANDBY_CDMA;
        axi_cdma_1[0] = STANDBY_CDMA;
        axi_cdma_2[0] = STANDBY_CDMA;
        axi_cdma_3[0] = STANDBY_CDMA;
    }

    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;*/

    /*auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_read_res += duration.count();*/
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
    //auto start = high_resolution_clock::now();

    fstream file_i(I_FILE, std::ios_base::in);
    fstream file_t(T_FILE, std::ios_base::in);

    file_i >> i_path;
    file_t >> t_path;

    // 0.299 R + 0.587 G + 0.114 B
    i_img = cv::imread(i_path, cv::IMREAD_GRAYSCALE);
    t_img = cv::imread(t_path, cv::IMREAD_GRAYSCALE);

    file_i.close();
    file_t.close();

    // global
    w = t_img.cols;
    h = t_img.rows;
    h_minus_m = h - m;
    w_minus_n = w - n;
    res_bytes_per_unit = num_bees * 4 / parallel_units;

    // draw the target for inspection
    Mat img0 = t_img.clone();
    Point pt1(a, b);
    Point pt2(c, d);
    rectangle(img0, pt1, pt2, cv::Scalar(0, 0, 0));
    cv::imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
    img0.release();

    // result
    res = Mat(h_minus_m, w_minus_n, CV_32FC1, cv::Scalar(128, 128, 128));
    data = (float *)res.data;

    /*auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_read_file += duration.count();*/
}
void region_of_interest(int x, int y, int unit)
{
    //auto start = high_resolution_clock::now();
    if (x < 0 || y < 0 || x >= w_minus_n || y >= h_minus_m)
    {
        rect = cv::Rect(u, v, n, m);
        t_img_roi = t_img(rect);
        if (num_elem_real > BRAM_BYTES)
        {
            cv::resize(t_img_roi, t_img_roi_resize, cv::Size(128, 64), 0, 0, CV_INTER_LINEAR);
        }
        else
        {
            t_img_roi_resize = t_img_roi.clone();
        }
        t_img_roi_resize.convertTo(t_img_roi_resize, CV_8U);
        memcpy(data_t, t_img_roi_resize.data, n_times_m);
    }
    else
    {
        rect = cv::Rect(x, y, n, m);
        i_img_roi = i_img(rect);
        if (num_elem_real > BRAM_BYTES)
        {
            cv::resize(i_img_roi, i_img_roi_resize, cv::Size(128, 64), 0, 0, CV_INTER_LINEAR);
        }
        else
        {
            i_img_roi_resize = i_img_roi.clone();
        }
        i_img_roi_resize.convertTo(i_img_roi_resize, CV_8U);
    }
    if (unit == 0)
    {
        memcpy(data_i_0, i_img_roi_resize.data, n_times_m);
    }
    else if (unit == 1)
    {
        memcpy(data_i_1, i_img_roi_resize.data, n_times_m);
    }
    else if (unit == 2)
    {
        memcpy(data_i_2, i_img_roi_resize.data, n_times_m);
    }
    else if (unit == 3)
    {
        memcpy(data_i_3, i_img_roi_resize.data, n_times_m);
    }
    else if (unit == 4)
    {
        memcpy(data_i_4, i_img_roi_resize.data, n_times_m);
    }
    else if (unit == 5)
    {
        memcpy(data_i_5, i_img_roi_resize.data, n_times_m);
    }
    else if (unit == 6)
    {
        memcpy(data_i_6, i_img_roi_resize.data, n_times_m);
    }
    else if (unit == 7)
    {
        memcpy(data_i_7, i_img_roi_resize.data, n_times_m);
    }
    /*auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_slice_data += duration.count();*/
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

    file_a.close();
    file_b.close();
    file_c.close();
    file_d.close();

    u = a;
    v = b;
    n = c - a;
    m = d - b;
    n_times_m = n * m;
    num_elem_real = n_times_m;
    if (num_elem_real > BRAM_BYTES)
    {
        exit(1);
        num_elem = BRAM_BYTES;
    }
    else
    {
        num_elem = num_elem_real;
    }
}
void eval_pop(double *bees, signed long int *obj, double *limits)
{
    idx = 0;
    for (int bee = 0; bee < num_bees; bee++)
    {
        // Point in frame 2
        int a = bees[bee * 2];
        int b = bees[bee * 2 + 1];

        // Check limits, just in case
        if (a > limits[2])
            a = limits[2];
        if (a < limits[3])
            a = limits[3];
        if (b > limits[6])
            b = limits[6];
        if (b < limits[7])
            b = limits[7];

        // Get fitness value of point
        int unit_index = bee % parallel_units;
        if (unit_index == 0)
        {
            clear_signal();
        }
        region_of_interest(a, b, unit_index);
        if (unit_index == 3)
        {
            write_i_data();
            work(idx);
            idx += 4;
            tests += parallel_units;
        }
    }
    // read results
    read_data();
    for (int bee = 0; bee < num_bees; bee++)
    {
        // Point in frame 2
        int a = bees[bee * 2];
        int b = bees[bee * 2 + 1];

        // Check limits, just in case
        if (a > limits[2])
            a = limits[2];
        if (a < limits[3])
            a = limits[3];
        if (b > limits[6])
            b = limits[6];
        if (b < limits[7])
            b = limits[7];

        int unit_index = bee % parallel_units;
        int bram_index = bee / parallel_units;
        if (unit_index == 0)
        {
            data[a + b * w_minus_n] = ((float)results_0[bram_index] * 255.0) / 65536.0;
            obj[bee] = results_0[bram_index];
        }
        if (unit_index == 1)
        {
            data[a + b * w_minus_n] = ((float)results_1[bram_index] * 255.0) / 65536.0;
            obj[bee] = results_1[bram_index];
        }
        if (unit_index == 2)
        {
            data[a + b * w_minus_n] = ((float)results_2[bram_index] * 255.0) / 65536.0;
            obj[bee] = results_2[bram_index];
        }
        if (unit_index == 3)
        {
            data[a + b * w_minus_n] = ((float)results_3[bram_index] * 255.0) / 65536.0;
            obj[bee] = results_3[bram_index];
        }
    }
}
void generate_new_pop(double *mu_bees, signed long int *mu_obj,
                      double *lambda_bees, signed long int *lambda_obj, double *limits,
                      int first, int last)
{
    int mate1, mate2, num_cross, num_mut, num_rand;

    for (int bee = first; bee <= last; bee++)
    {
        // Mutation
        // bees from 0 to rate_mut - 1
        // Only core 0 of each bee
        if (bee >= 0 + first && bee <= rate_mut - 1 + first)
        {
            // Tournament
            int a = (cvRandInt(&rng) % (last - first + 1)) + first;
            int b = (cvRandInt(&rng) % (last - first + 1)) + first;
            if (mu_obj[a] > mu_obj[b])
                mate1 = a;
            else
                mate1 = b;

            // Copy the individual
            lambda_bees[bee * 2] = mu_bees[mate1 * 2];
            lambda_bees[bee * 2 + 1] = mu_bees[mate1 * 2 + 1];

            // Polinomial Mutation
            mutation(lambda_bees, bee, limits);
        }

        // Crossover
        // bees from first_bee + rate_mut to first_bee + rate_mut + rate_cross - 1
        // rate_mut must be even if crossover happens since two sons are generated
        if (bee >= rate_mut + first &&
            bee <= rate_mut + rate_cross - 1 + first &&
            bee % 2 == 0)
        {
            //Tournament
            int a = (cvRandInt(&rng) % (last - first + 1)) + first;
            int b = (cvRandInt(&rng) % (last - first + 1)) + first;
            int c = (cvRandInt(&rng) % (last - first + 1)) + first;
            int d = (cvRandInt(&rng) % (last - first + 1)) + first;
            if (mu_obj[a] > mu_obj[b])
                mate1 = a;
            else
                mate1 = b;
            if (mu_obj[c] > mu_obj[d])
                mate2 = c;
            else
                mate2 = d;

            // crossover SBX
            cross_over(mu_bees, lambda_bees, mate1, mate2, bee, bee + 1, limits);
        }

        // Random
        // bees from first_bee + rate_mut + rate_cross to
        // first_bee + rate_mut + rate_cross + rate_rand - 1
        if (bee >= rate_mut + rate_cross + first &&
            bee <= rate_mut + rate_cross + rate_rand - 1 + first)
        {
            int nvar_real = 2;
            float lower;
            float upper;
            for (int j = 0; j < nvar_real; j++)
            {
                upper = limits[j * 4 + 2];
                lower = limits[j * 4 + 3];

                lambda_bees[bee * nvar_real + j] =
                    cvRandReal(&rng) * (upper - lower) + lower;
            }
        }
    }
}
void merge_pop(double *mu_bees, signed long int *mu_obj,
               double *lambda_bees, signed long int *lambda_obj)
{
    for (int bee = 0; bee < num_bees; bee++)
    {
        // Copy mu bee
        int mu_lambda_bee = bee * 2;
        mu_lambda_obj[mu_lambda_bee] = mu_obj[bee];
        mu_lambda_bees[mu_lambda_bee * 2] = mu_bees[bee * 2];
        mu_lambda_bees[mu_lambda_bee * 2 + 1] = mu_bees[bee * 2 + 1];

        // Copy lambda bee
        mu_lambda_bee++;
        mu_lambda_obj[mu_lambda_bee] = lambda_obj[bee];
        mu_lambda_bees[mu_lambda_bee * 2] = lambda_bees[bee * 2];
        mu_lambda_bees[mu_lambda_bee * 2 + 1] = lambda_bees[bee * 2 + 1];
    }
}
void best_mu(double *mu_bees, signed long int *mu_obj)
{
    for (int bee = 0; bee < num_bees; bee++)
    {
        // The actual index of the ith best bee
        // a very rudimentary insertion sort
        int best = 0;
        signed long int best_val = -1000;
        for (int i = 0; i < num_bees * 2; i++)
        {
            if (best_val < mu_lambda_obj[i])
            {
                best = i;
                best_val = mu_lambda_obj[i];
            }
        }
        mu_lambda_obj[best] = -1000;

        // Copy the ith best bee to the mu population
        mu_obj[bee] = best_val;
        mu_bees[bee * 2] = mu_lambda_bees[best * 2];
        mu_bees[bee * 2 + 1] = mu_lambda_bees[best * 2 + 1];
    }
}
int main()
{
    auto start = high_resolution_clock::now();

    // general
    open_mem();
    set_names();
    print_version();

    // initial pop
    mu_e_bees = (double *)malloc(num_bees_comp * sizeof(double));
    mu_e_obj = (signed long int *)malloc(num_bees * sizeof(signed long int));
    lambda_e_bees = (double *)malloc(num_bees_comp * sizeof(double));
    lambda_e_obj = (signed long int *)malloc(num_bees * sizeof(signed long int));
    mu_f_bees = (double *)malloc(num_bees_comp * sizeof(double));
    mu_f_obj = (signed long int *)malloc(num_bees * sizeof(signed long int));
    lambda_f_bees = (double *)malloc(num_bees_comp * sizeof(double));
    lambda_f_obj = (signed long int *)malloc(num_bees * sizeof(signed long int));
    mu_lambda_bees = (double *)malloc(num_bees_comp * 2 * sizeof(double));
    mu_lambda_obj = (signed long int *)malloc(num_bees * 2 * sizeof(signed long int));

    load_init_file();
    load_image_file();

    // template
    region_of_interest(-1, -1, -1);
    write_t_data();

    double limits[8];
    // First component
    limits[0] = u + n / 8;
    limits[1] = u - n / 8;
    limits[2] = w - n;
    limits[3] = 0;

    // Second component
    limits[4] = v + m / 8;
    limits[5] = v - m / 8;
    limits[6] = h - m;
    limits[7] = 0;

    // EXPLORATION PHASE
    // Generate random initial exploration individuals
    initial_random_pop(mu_e_bees, limits, 0, num_bees - 1);

    for (int generation = 0; generation < max_gen; generation++)
    {
        // Evaluate parent population
        eval_pop(mu_e_bees, mu_e_obj, limits);

        // Generate lamdba population
        rate_mut = 39;
        rate_cross = 6;
        rate_rand = 19;
        generate_new_pop(mu_e_bees, mu_e_obj, lambda_e_bees, lambda_e_obj, limits, 0, num_bees - 1);

        // Evaluate new population
        eval_pop(lambda_e_bees, lambda_e_obj, limits);

        // Mu + Lambda
        merge_pop(mu_e_bees, mu_e_obj, lambda_e_bees, lambda_e_obj);

        // Select best mu
        best_mu(mu_e_bees, mu_e_obj);
    }

    // RECRUITMENT PHASE
    int *recruits = (int *)malloc(num_bees * sizeof(int));
    recruiter = (int *)malloc(num_bees * sizeof(int));
    int last_recruiter;
    int min_u;
    int max_u;
    int min_v;
    int max_v;

    double sum = 0.0;
    int recruited_bees = 0;

    // Get accumulated fitness value
    for (int i = 0; i < num_bees; i++)
    {
        // 0 or negative values don't contribute
        if (mu_e_obj[i] > 0.0f)
            sum += mu_e_obj[i];
    }

    // Decide resources for each recruiter
    if (sum > 0.0)
    {
        for (int i = 0; i < num_bees; i++)
        {
            // 0 or negative bees have no recruits
            if (mu_e_obj[i] >= 0.0f)
            {
                recruits[i] = (mu_e_obj[i] / sum) * num_bees;
                recruited_bees += recruits[i];
            }
            else
            {
                recruits[i] = 0;
            }
        }
    }
    else
    {
        // Since the last search gave no results
        // make a second normal search
        // using all cores
        recruits[0] = num_bees;
        mu_e_bees[0] = u;
        mu_e_bees[1] = v;
        for (int i = 1; i < num_bees; i++)
        {
            recruits[i] = 0;
        }
        recruited_bees = num_bees;
    }

    // All cores should have some work
    // Give the difference to the best explorer bee
    if (recruited_bees < num_bees)
    {
        recruits[0] = recruits[0] + (num_bees - recruited_bees);
    }

    // Assign bees
    int current_recruiter = 0;
    for (int i = 0; i < num_bees; i++)
    {
        recruiter[i] = current_recruiter;
        recruits[current_recruiter]--;
        if (recruits[current_recruiter] <= 0)
            current_recruiter++;
    }

    // Count the real recruited bees
    for (int i = 0; i < num_bees; i++)
    {
        recruits[i] = 0;
    }
    for (int i = 0; i < num_bees; i++)
    {
        recruits[recruiter[i]]++;
    }

    // Get new boundaries
    min_u = mu_e_bees[0];
    max_u = mu_e_bees[0];
    min_v = mu_e_bees[1];
    max_v = mu_e_bees[1];
    for (int i = 1; i < num_bees; i++)
    {
        if (mu_e_bees[recruiter[i] * 2] < min_u)
            min_u = mu_e_bees[recruiter[i] * 2];
        if (mu_e_bees[recruiter[i] * 2] > max_u)
            max_u = mu_e_bees[recruiter[i] * 2];
        if (mu_e_bees[recruiter[i] * 2 + 1] < min_v)
            min_v = mu_e_bees[recruiter[i] * 2 + 1];
        if (mu_e_bees[recruiter[i] * 2 + 1] > max_v)
            max_v = mu_e_bees[recruiter[i] * 2 + 1];
    }

    // FORAGING PHASE
    int first = 0;
    int last = 0;
    for (int r = 0; r < num_bees; r++)
    {
        if (recruits[r] < 1)
        {
            break;
        }
        last += recruits[r] - 1;
        // New limits based on the recruiter
        // First component
        int bee = 0;
        limits[0] = mu_e_bees[r * 2] + 1;
        limits[1] = mu_e_bees[r * 2] - 1;
        limits[2] = max_u + n / 8;
        limits[3] = min_u - n / 8;
        if (limits[2] > w - n)
            limits[2] = w - n;
        if (limits[3] < 0)
            limits[3] = 0;

        // Second component
        limits[4] = mu_e_bees[r * 2 + 1] + 1;
        limits[5] = mu_e_bees[r * 2 + 1] - 1;
        limits[6] = max_v + m / 8;
        limits[7] = min_v - m / 8;
        if (limits[6] > h - m)
            limits[6] = h - m;
        if (limits[7] < 0)
            limits[7] = 0;

        // Generate random initial individuals
        initial_random_pop(mu_f_bees, limits, first, last);
        first += recruits[r];
    }

    for (int generation = 0; generation < max_gen / 2; generation++)
    {
        // Evaluate parent population
        eval_pop(mu_f_bees, mu_f_obj, limits);

        // Generate lamdba population
        rate_mut = 39;
        rate_cross = 20;
        rate_rand = 5;
        generate_new_pop(mu_f_bees, mu_f_obj, lambda_f_bees, lambda_f_obj, limits, 0, num_bees - 1);

        // Evaluate new population
        eval_pop(lambda_f_bees, lambda_f_obj, limits);

        // Mu + Lambda
        merge_pop(mu_f_bees, mu_f_obj, lambda_f_bees, lambda_f_obj);

        // Select best mu
        best_mu(mu_f_bees, mu_f_obj);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    unsigned long full_time = duration.count();

    imwrite("/root/hsa-soc-local/img/dices1.jpg", res);

    cout << "Write t: " << time_write_t << " us\n";
    cout << "Write i: " << time_write_i << " us\n";
    cout << "Read res: " << time_read_res << " us\n";
    cout << "Read file: " << time_read_file << " us\n";
    cout << "Slice data: " << time_slice_data << " us\n";
    cout << "Work: " << time_work << " us\n";
    cout << "Tests: " << tests << "\n";
    cout << "Max: " << mu_f_obj[0] << "\n";
    cout << "u: " << mu_f_bees[0] << "\n";
    cout << "v: " << mu_f_bees[1] << "\n";
    cout << "n: " << n << "\n";
    cout << "m: " << m << "\n";
    cout << "full time: " << full_time << " us\n";

    return 0;
}