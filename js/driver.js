const fs = require('fs')
let units = 4
let bees = 64
let dmas = 4
const path = '/root/hsa-soc-local/cpp/test_bee_auto.cpp'
const gpio = [
    ['axi_gpio_0', '0x41200000', 4],
    ['axi_gpio_1', '0x41210000', 4],
    ['axi_gpio_2', '0x41220000', 4],
    ['axi_gpio_3', '0x41230000', 4],
    ['axi_gpio_4', '0x41240000', 4],
    ['axi_gpio_5', '0x41250000', 4]
]
const cdma = [
    ['axi_cdma_0', '0x7E200000', 40],
    ['axi_cdma_1', '0x7E210000', 40],
    ['axi_cdma_2', '0x7E220000', 40],
    ['axi_cdma_3', '0x7E230000', 40]
]
const mem = [
    ['axi_cdma_0', 't', '0xC2000000', '0x0E000000', 8192, 0x0],

    ['axi_cdma_0', 'i0', '0xC0000000', '0x0E010000', 8192, 0x0],
    ['axi_cdma_1', 'i1', '0xC0000000', '0x0E020000', 8192, 0x1],
    ['axi_cdma_2', 'i2', '0xC0000000', '0x0E030000', 8192, 0x2],
    ['axi_cdma_3', 'i3', '0xC0000000', '0x0E040000', 8192, 0x3],

    ['axi_cdma_0', 'i4', '0xC6000000', '0x0E050000', 8192, 0x4],
    ['axi_cdma_1', 'i5', '0xC4000000', '0x0E060000', 8192, 0x5],
    ['axi_cdma_2', 'i6', '0xC4000000', '0x0E070000', 8192, 0x6],
    ['axi_cdma_3', 'i7', '0xC4000000', '0x0E080000', 8192, 0x7],

    ['axi_cdma_0', 'i8', '0xCA000000', '0x0E090000', 8192, 0x8],
    ['axi_cdma_1', 'ia', '0xC8000000', '0x0E0A0000', 8192, 0xA],
    ['axi_cdma_2', 'ic', '0xC8000000', '0x0E0B0000', 8192, 0xC],
    ['axi_cdma_3', 'ie', '0xC8000000', '0x0E0C0000', 8192, 0xE],

    ['axi_cdma_0', 'i9', '0xCC000000', '0x0E0D0000', 8192, 0x9],
    ['axi_cdma_1', 'ib', '0xCA000000', '0x0E0E0000', 8192, 0xB],
    ['axi_cdma_2', 'id', '0xCA000000', '0x0E0F0000', 8192, 0xD],
    [null, null, '0xC8000000', '0x0E0C0000', 8192, 0xF],

    /*['axi_cdma_0', 'r0', '0xC4000000', '0x0E100000', 8192, 0x0],
    ['axi_cdma_1', 'r1', '0xC2000000', '0x0E110000', 8192, 0x1],
    ['axi_cdma_2', 'r2', '0xC2000000', '0x0E120000', 8192, 0x2],
    ['axi_cdma_3', 'r3', '0xC2000000', '0x0E130000', 8192, 0x3],

    ['axi_cdma_0', 'r4', '0xC8000000', '0x0E140000', 8192, 0x4],
    ['axi_cdma_1', 'r5', '0xC6000000', '0x0E150000', 8192, 0x5],
    ['axi_cdma_2', 'r6', '0xC6000000', '0x0E160000', 8192, 0x6],
    ['axi_cdma_3', 'r7', '0xC6000000', '0x0E170000', 8192, 0x7],

    ['axi_cdma_0', 'r8', '0xCE000000', '0x0E180000', 8192, 0x8],
    ['axi_cdma_1', 'ra', '0xCC000000', '0x0E190000', 8192, 0xA],
    ['axi_cdma_2', 'rc', '0xCC000000', '0x0E1A0000', 8192, 0xC],
    ['axi_cdma_3', 're', '0xCC000000', '0x0E1B0000', 8192, 0xE],

    ['axi_cdma_0', 'r9', '0xD0000000', '0x0E1C0000', 8192, 0x9],
    ['axi_cdma_1', 'rb', '0xCE000000', '0x0E1D0000', 8192, 0xB],
    ['axi_cdma_2', 'rd', '0xCE000000', '0x0E1E0000', 8192, 0xD],
    [null, null, '0xCC000000', '0x0E1B0000', 8192, 0xF]*/
]
let code = ''
code += `#include <stdio.h>
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
#define CLEAR_CDMA 0x4
#define DONE_CDMA 0x2
#define STANDBY_CDMA 0x0
#define CLEAR_ZNCC 0x0
#define ZNCC_DONE 0x7
#define BRAM_BYTES 8192
using namespace cv;
using namespace std;
using namespace std::chrono;
int fd;
unsigned long int idx;
unsigned long int num_elem;
unsigned long int num_elem_real;
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
Rect rect;
ofstream result;
std::string i_path;
std::string t_path;
unsigned long int time_write_t = 0;
unsigned long int time_write_i = 0;
unsigned long int time_read_res = 0;
unsigned long int time_read_file = 0;
unsigned long int time_slice_data = 0;
unsigned long int time_work = 0;
unsigned long int tests = 0;
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
int num_bees = ${bees};
int num_bees_comp = ${bees} * 2;
int *recruiter;
int *recruits;
double eta_m = 25;
double eta_c = 2;
int max_gen = 2;
int rate_rand = 0;
int rate_mut = 0;
int rate_cross = 0;
unsigned int parallel_units = ${units};
`
for (let i = 0; i < gpio.length; i++) {
    let name = gpio[i][0]
    code += `unsigned long int *${name};`
    code += '\n'
}
for (let i = 0; i < cdma.length; i++) {
    let name = cdma[i][0]
    code += `unsigned long int *${name};`
    code += '\n'
}
for (let i = 0; i < mem.length; i++) {
    let name = mem[i][1]
    if (name) {
        code += `unsigned long int *${name};`
        code += '\n'
    }
}
code += `double get_beta(double u)
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
`
let list = ''
for (let i = 0; i < gpio.length; i++) {
    let name = gpio[i][0]
    let addr = gpio[i][1]
    let bytes = gpio[i][2]
    list += `    ${name} = map_mem(${bytes}, ${addr});`
    list += '\n'
}
for (let i = 0; i < cdma.length; i++) {
    let name = cdma[i][0]
    let addr = cdma[i][1]
    let bytes = cdma[i][2]
    list += `    ${name} = map_mem(${bytes}, ${addr});`
    list += '\n'
}
for (let i = 0; i < mem.length; i++) {
    let name = mem[i][1]
    let addr = mem[i][3]
    let bytes = mem[i][4]
    let unit = mem[i][5]
    if (name && unit < units) {
        list += `    ${name} = map_mem(${bytes}, ${addr});`
        list += '\n'
    }
}
code += `int open_mem()
{
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        exit(1);
    }
${list}
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
    recruits = (int *)malloc(num_bees * sizeof(int));
    recruiter = (int *)malloc(num_bees * sizeof(int));
}
int close_mem()
{
    close(fd);
}
void clear_signal()
{
    axi_gpio_1[0] = CLEAR_ZNCC;
}
void write_t_data()
{
    while (!(axi_cdma_0[1] & DONE_CDMA))
    {
        axi_cdma_0[0] = CLEAR_CDMA;
    }
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_0[6] = ${mem[0][3]};
    axi_cdma_0[8] = ${mem[0][2]};
    axi_cdma_0[10] = num_elem;
    while (!(axi_cdma_0[1] & DONE_CDMA))
    {
        axi_cdma_0[0] = STANDBY_CDMA;
    }
    axi_cdma_0[0] = STANDBY_CDMA;
}
`
let dma = ''
let count = (units - (units % dmas)) / dmas
if (units % dmas > 0) count++
for (let i = 0; i < count; i++) {
    origin1 = mem[i * dmas + 1][3]
    destiny1 = mem[i * dmas + 1][2]
    origin2 = mem[i * dmas + 2][3]
    destiny2 = mem[i * dmas + 2][2]
    origin3 = mem[i * dmas + 3][3]
    destiny3 = mem[i * dmas + 3][2]
    origin4 = mem[i * dmas + 4][3]
    destiny4 = mem[i * dmas + 4][2]
    dma += `    while (
    !(axi_cdma_0[1] & DONE_CDMA) &&
    !(axi_cdma_1[1] & DONE_CDMA) &&
    !(axi_cdma_2[1] & DONE_CDMA) &&
    !(axi_cdma_3[1] & DONE_CDMA))
{
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_1[0] = CLEAR_CDMA;
    axi_cdma_2[0] = CLEAR_CDMA;
    axi_cdma_3[0] = CLEAR_CDMA;
}
axi_cdma_0[0] = CLEAR_CDMA;
axi_cdma_1[0] = CLEAR_CDMA;
axi_cdma_2[0] = CLEAR_CDMA;
axi_cdma_3[0] = CLEAR_CDMA;
axi_cdma_0[0] = STANDBY_CDMA;
axi_cdma_1[0] = STANDBY_CDMA;
axi_cdma_2[0] = STANDBY_CDMA;
axi_cdma_3[0] = STANDBY_CDMA;
axi_cdma_0[6] = ${origin1};
axi_cdma_0[8] = ${destiny1};
axi_cdma_1[6] = ${origin2};
axi_cdma_1[8] = ${destiny2};
axi_cdma_2[6] = ${origin3};
axi_cdma_2[8] = ${destiny3};
axi_cdma_3[6] = ${origin4};
axi_cdma_3[8] = ${destiny4};
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
`
}
code += `void write_i_data()
{
${dma}
}
void work(int idx)
{
    //axi_gpio_2[0] = idx;
    axi_gpio_0[0] = num_elem;
    while (axi_gpio_5[0] != ZNCC_DONE)
    {
        axi_gpio_0[0] = num_elem;
    }
    axi_gpio_0[0] = num_elem;
}
`
dma = ''
/*for (let i = 0; i < count; i++) {
    origin1 = mem[i * dmas + 17][2]
    destiny1 = mem[i * dmas + 17][3]
    origin2 = mem[i * dmas + 18][2]
    destiny2 = mem[i * dmas + 18][3]
    origin3 = mem[i * dmas + 19][2]
    destiny3 = mem[i * dmas + 19][3]
    origin4 = mem[i * dmas + 20][2]
    destiny4 = mem[i * dmas + 20][3]
    dma += `while (
        !(axi_cdma_0[1] & DONE_CDMA) &&
        !(axi_cdma_1[1] & DONE_CDMA) &&
        !(axi_cdma_2[1] & DONE_CDMA) &&
        !(axi_cdma_3[1] & DONE_CDMA))
    {
        axi_cdma_0[0] = CLEAR_CDMA;
        axi_cdma_1[0] = CLEAR_CDMA;
        axi_cdma_2[0] = CLEAR_CDMA;
        axi_cdma_3[0] = CLEAR_CDMA;
    }
    axi_cdma_0[0] = CLEAR_CDMA;
    axi_cdma_1[0] = CLEAR_CDMA;
    axi_cdma_2[0] = CLEAR_CDMA;
    axi_cdma_3[0] = CLEAR_CDMA;
    axi_cdma_0[0] = STANDBY_CDMA;
    axi_cdma_1[0] = STANDBY_CDMA;
    axi_cdma_2[0] = STANDBY_CDMA;
    axi_cdma_3[0] = STANDBY_CDMA;
axi_cdma_0[6] = ${origin1};
axi_cdma_0[8] = ${destiny1};
axi_cdma_1[6] = ${origin2};
axi_cdma_1[8] = ${destiny2};
axi_cdma_2[6] = ${origin3};
axi_cdma_2[8] = ${destiny3};
axi_cdma_3[6] = ${origin4};
axi_cdma_3[8] = ${destiny4};
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
    `
}*/
code += `void read_data()
{
${dma}
}
int load_image_file()
{
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
}
`
let conditions = ''
for (let i = 0; i < units; i++) {
    let name = mem[i + 1][1]
    let unit = mem[i + 1][5]
    if (name) {
        conditions += `    if (unit == ${unit})
    {
        memcpy(${name}, i_img_roi_resize.data, num_elem);
    }
    `
    }
}
code += `void region_of_interest(int x, int y, int unit)
{
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
        memcpy(t, t_img_roi_resize.data, num_elem);
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
    ${conditions}
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
        num_elem = BRAM_BYTES;
    }
    else
    {
        num_elem = num_elem_real;
    }
}
`
conditions = ''
for (let i = 0; i < units; i++) {
    //let name = mem[i + 17][1]
    //let unit = mem[i + 17][5]
    //if (name) {
    let unit = i
    conditions += `if (unit_index == ${unit})
    {
        obj[bee] = axi_gpio_${unit + 1}[0];
    }
    `
    //}
}
code += `void eval_pop(double *bees, signed long int *obj, double *limits)
{
    idx = 0;
    for (int bee = 0; bee < num_bees; bee++)
    {
        // Point in frame 2
        int a_temp = bees[bee * 2];
        int b_temp = bees[bee * 2 + 1];
        // Check limits, just in case
        if (a_temp > limits[2])
        {
            a_temp = limits[2];
            bees[bee * 2] = (double)limits[2];
        }
        if (a_temp < limits[3])
        {
            a_temp = limits[3];
            bees[bee * 2] = (double)limits[3];
        }
        if (b_temp > limits[6])
        {
            b_temp = limits[6];
            bees[bee * 2 + 1] = (double)limits[6];
        }
        if (b_temp < limits[7])
        {
            b_temp = limits[7];
            bees[bee * 2 + 1] = (double)limits[7];
        }
        // Get fitness value of point
        int unit_index = bee % parallel_units;
        if (unit_index == 0)
        {
            clear_signal();
        }
        region_of_interest(a_temp, b_temp, unit_index);
        if (unit_index == ${units - 1})
        {
            write_i_data();
            work(idx);
            idx += 4;
            tests += parallel_units;

            // read results
    //read_data();
    for (int bee = 0; bee < num_bees; bee++)
    {
        int unit_index = bee % parallel_units;
        int bram_index = bee / parallel_units;
        ${conditions}
    }
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
int a_temp = (cvRandInt(&rng) % (last - first + 1)) + first;
int b_temp = (cvRandInt(&rng) % (last - first + 1)) + first;
if (mu_obj[a_temp] > mu_obj[b_temp])
mate1 = a_temp;
else
mate1 = b_temp;
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
int a_temp = (cvRandInt(&rng) % (last - first + 1)) + first;
int b_temp = (cvRandInt(&rng) % (last - first + 1)) + first;
int c_temp = (cvRandInt(&rng) % (last - first + 1)) + first;
int d_temp = (cvRandInt(&rng) % (last - first + 1)) + first;
if (mu_obj[a_temp] > mu_obj[b_temp])
mate1 = a_temp;
else
mate1 = b_temp;
if (mu_obj[c_temp] > mu_obj[d_temp])
mate2 = c_temp;
else
mate2 = d_temp;
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
// general
open_mem();
//set_names();
load_init_file();
load_image_file();
auto start = high_resolution_clock::now();
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
//imwrite("/root/hsa-soc-local/img/dices1.jpg", res);
// Point in frame 2
int a_temp = mu_f_bees[0];
int b_temp = mu_f_bees[1];
if (a_temp > limits[2])
{
a_temp = limits[2];
}
if (a_temp < limits[3])
{
a_temp = limits[3];
}
if (b_temp > limits[6])
{
b_temp = limits[6];
}
if (b_temp < limits[7])
{
b_temp = limits[7];
}
cout << "Write t: " << time_write_t << " us" << endl;
cout << "Write i: " << time_write_i << " us" << endl;
cout << "Read res: " << time_read_res << " us" << endl;
cout << "Read file: " << time_read_file << " us" << endl;
cout << "Slice data: " << time_slice_data << " us" << endl;
cout << "Work: " << time_work << " us" << endl;
cout << "Tests: " << tests << endl;
cout << "Max: " << mu_f_obj[0] << endl;
cout << "u: " << a_temp << endl;
cout << "v: " << b_temp << endl;
cout << "n: " << n << endl;
cout << "m: " << m << endl;
cout << "full time: " << full_time << " us" << endl;
cout << "u0: " << u << endl;
cout << "v0: " << v << endl;
close_mem();
/*free(mu_e_bees);
free(mu_e_obj);
free(lambda_e_bees);
free(lambda_e_obj);
free(mu_f_bees);
free(mu_f_obj);
free(lambda_f_bees);
free(lambda_f_obj);
free(mu_lambda_bees);
free(mu_lambda_obj);
free(mu_lambda_order);
free(recruiter);*/
i_img.release();
t_img.release();
i_img_roi.release();
t_img_roi.release();
t_img_roi_resize.release();
i_img_roi_resize.release();
//res.release();
return 0;
}
`
fs.writeFile(path, code, err => {
    if (err) {
        console.error(err)
    }
})
