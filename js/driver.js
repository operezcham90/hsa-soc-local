const fs = require('fs')
let units = 4
const path = '/root/hsa-soc-local/cpp/test_bee_auto.cpp'
const gpio = [
    ['axi_gpio_0', '0x41200000', 4],
    ['axi_gpio_1', '0x41210000', 4],
    ['axi_gpio_2', '0x41220000', 4],
    ['axi_gpio_3', '0x41230000', 4]
]
const cdma = [
    ['axi_cdma_0', '0x7E200000', 40],
    ['axi_cdma_1', '0x7E210000', 40],
    ['axi_cdma_2', '0x7E220000', 40],
    ['axi_cdma_3', '0x7E230000', 40]
]
const mem = [
    ['axi_cdma_0', 't', '0xC0000000', '0x0E000000', 8192],
    ['axi_cdma_0', 'i0', '0xC2000000', '0x0E000000', 8192],
    ['axi_cdma_0', 'r0', '0xC4000000', '0x0F000000', 8192],
    ['axi_cdma_0', 'i4', '0xC6000000', '0x0E000000', 8192],
    ['axi_cdma_0', 'r4', '0xC8000000', '0x0F000000', 8192],
    ['axi_cdma_0', 'i8', '0xCA000000', '0x0E000000', 8192],
    ['axi_cdma_0', 'i9', '0xCC000000', '0x0E000000', 8192],
    ['axi_cdma_0', 'r8', '0xCE000000', '0x0F000000', 8192],
    ['axi_cdma_0', 'r9', '0xD0000000', '0x0F000000', 8192],

    ['axi_cdma_1', 'i1', '0xC0000000', '0x10000000', 8192],
    ['axi_cdma_1', 'i5', '0xC4000000', '0x10000000', 8192],
    ['axi_cdma_1', 'r5', '0xC6000000', '0x11000000', 8192],
    ['axi_cdma_1', 'r1', '0xC2000000', '0x11000000', 8192],
    ['axi_cdma_1', 'ib', '0xCA000000', '0x10000000', 8192],
    ['axi_cdma_1', 'ia', '0xC8000000', '0x10000000', 8192],
    ['axi_cdma_1', 'ra', '0xCC000000', '0x11000000', 8192],
    ['axi_cdma_1', 'rb', '0xCE000000', '0x11000000', 8192],

    ['axi_cdma_2', 'ic', '0xC8000000', '0x12000000', 8192],
    ['axi_cdma_2', 'id', '0xCA000000', '0x12000000', 8192],
    ['axi_cdma_2', 'rc', '0xCC000000', '0x13000000', 8192],
    ['axi_cdma_2', 'rd', '0xCE000000', '0x13000000', 8192],
    ['axi_cdma_2', 'i2', '0xC0000000', '0x12000000', 8192],
    ['axi_cdma_2', 'r2', '0xC2000000', '0x13000000', 8192],
    ['axi_cdma_2', 'i6', '0xC4000000', '0x12000000', 8192],
    ['axi_cdma_2', 'r6', '0xC6000000', '0x13000000', 8192],

    ['axi_cdma_3', 'r3', '0xC2000000', '0x15000000', 8192],
    ['axi_cdma_3', 'i3', '0xC0000000', '0x14000000', 8192],
    ['axi_cdma_3', 'i7', '0xC4000000', '0x14000000', 8192],
    ['axi_cdma_3', 'r7', '0xC6000000', '0x15000000', 8192],
    ['axi_cdma_3', 'ie', '0xC8000000', '0x14000000', 8192],
    ['axi_cdma_3', 're', '0xCC000000', '0x15000000', 8192]
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
int num_bees = 64;
int num_bees_comp = 64 * 2;
int *recruiter;
int *recruits;
double eta_m = 25;
double eta_c = 2;
int max_gen = 2;
int rate_rand = 0;
int rate_mut = 0;
int rate_cross = 0;
`
code += `unsigned int parallel_units = ${units};
`
for (let i = 0; i < gpio.length; i++) {
    let name = gpio[i][0]
    code += `unsigned long int *${name};
    `
}
for (let i = 0; i < cdma.length; i++) {
    let name = cdma[i][0]
    code += `unsigned long int *${name};
    `
}
for (let i = 0; i < mem.length; i++) {
    let name = mem[i][1]
    code += `unsigned long int *${name};
    `
}
fs.writeFile(path, code, err => {
    if (err) {
        console.error(err)
    }
})

