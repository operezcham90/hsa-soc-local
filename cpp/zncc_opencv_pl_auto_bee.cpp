// g++ zncc_opencv_pl_auto_bee.cpp -o zncc_opencv_pl_auto_bee `pkg-config --cflags --libs opencv`
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
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;
using namespace std::chrono;
// AXI pointers
unsigned long int *axi_bram_ctrl_0;
unsigned long int *axi_bram_ctrl_1;
unsigned long int *axi_bram_ctrl_2;
unsigned long int *axi_gpio_0;
unsigned long int *axi_gpio_1;
unsigned long int *axi_gpio_2;
// AXI addresses
off_t axi_bram_ctrl_0_addr = 0x40000000;
off_t axi_bram_ctrl_1_addr = 0x42000000;
off_t axi_bram_ctrl_2_addr = 0x44000000;
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
off_t axi_gpio_2_addr = 0x41220000;
// dev mem
int fd;
// constants of PL design
unsigned int bram_bytes = 2048 * 4;
unsigned int gpio_bytes = 4;
long int bram_length = 2048 * 4;
// zncc variables
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
unsigned int n_times_m;
unsigned int data_len;
long int *t_data;
long int *i_data;
Mat i_img;
Mat t_img;
Mat i_img_roi;
Mat t_img_roi;
Mat res;
Rect rect;
// bees
CvRNG rng = cvRNG(0xffffffff);
double *mu_bees;
signed long int *mu_obj;
double *lambda_bees;
signed long int *lambda_obj;
double *mu_lambda_bees;
signed long int *mu_lambda_obj;
int *mu_lambda_order;
int num_bees = 64;
int num_bees_comp = 64 * 2;
double eta_m;
double eta_c;
int max_gen = 2;
// report
ofstream result;
unsigned long int time_clear = 0;
unsigned long int time_write = 0;
unsigned long int time_work = 0;
// functions
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
double ec_distance(double x1, double y1, double x2, double y2)
{
    double a = x1 - x2;
    double b = y1 - y2;
    double d = sqrt(a * a + b * b);
    return d;
}
void initial_random_pop(double *limits)
{
    for (int bee = 0; bee < num_bees; bee++)
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
void fitness_function(int x, int y)
{
    init_zncc(x, y);

    // copy blocks of data to PL
    auto start = high_resolution_clock::now();
    memcpy(axi_bram_ctrl_0 + 4, i_data, data_len);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    time_write += duration.count();
    result << duration.count() << ",";

    // copy data length
    auto start2 = high_resolution_clock::now();
    axi_bram_ctrl_0[0] = data_len;
    // wait for standby signal
    while (axi_gpio_1[0] != 0)
    {
    }
    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    time_work += duration2.count();
    result << duration2.count() << "\n";
}
void eval_pop(double *bees, signed long int *obj, double *limits)
{
    // reset the system
    axi_bram_ctrl_0[0] = 0xFFFFFFFF;

    auto stop0 = high_resolution_clock::now();
    auto duration0 = duration_cast<microseconds>(stop0 - start0);
    time_clear += duration0.count();

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
        fitness_function(a, b);
    }
    // read results
    memcpy(obj, axi_bram_ctrl_2, num_bees * sizeof(signed long int));
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
void cross_over(int parent1, int parent2, int child1, int child2, double *limits)
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
void generate_new_pop(double *limits)
{
    int mate1, mate2, num_cross, num_mut, num_rand;

    for (int bee = 0; bee < num_bees; bee++)
    {
        // Mutation
        // bees from 0 to rate_mut - 1
        // Only core 0 of each bee
        if (bee >= 0 && bee <= rate_mut - 1)
        {
            // Tournament
            int a = cvRandInt(&rng) % num_bees;
            int b = cvRandInt(&rng) % num_bees;
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
        if (bee >= rate_mut &&
            bee <= rate_mut + rate_cross - 1 &&
            bee % 2 == 0)
        {
            //Tournament
            int a = cvRandInt(&rng) % num_bees;
            int b = cvRandInt(&rng) % num_bees;
            int c = cvRandInt(&rng) % num_bees;
            int d = cvRandInt(&rng) % num_bees;
            if (mu_obj[a] > mu_obj[b])
                mate1 = a;
            else
                mate1 = b;
            if (mu_obj[c] > mu_obj[d])
                mate2 = c;
            else
                mate2 = d;

            // crossover SBX
            cross_over(mate1, mate2, bee, bee + 1, limits);
        }

        // Random
        // bees from first_bee + rate_mut + rate_cross to
        // first_bee + rate_mut + rate_cross + rate_rand - 1
        if (bee >= rate_mut + rate_cross && bee <= rate_mut + rate_cross + rate_rand - 1)
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
void merge_pop()
{
    for (int bee = 0; bee < num_bees; bee++)
    {
        // Copy mu bee
        int mu_lambda_bee = bee * 2;
        mu_lambda_obj[mu_lambda_bee] = mu_obj[bee];
        mu_lambda_bees[mu_lambda_bee * 2] = mu_bees[bee * 2];
        mu_lambda_bees[mu_lambda_bee * 2 + 1] = mu_bees[bee * 2 + 1];
        //mu_lambda_order[mu_lambda_bee] = mu_lambda_bee;

        // Copy lambda bee
        mu_lambda_bee++;
        mu_lambda_obj[mu_lambda_bee] = lambda_obj[bee];
        mu_lambda_bees[mu_lambda_bee * 2] = lambda_bees[bee * 2];
        mu_lambda_bees[mu_lambda_bee * 2 + 1] = lambda_bees[bee * 2 + 1];
        //mu_lambda_order[mu_lambda_bee] = mu_lambda_bee;
    }
}
void best_mu()
{
    for (int bee = 0; bee < num_bees; bee++)
    {
        // The actual index of the ith best bee
        int best = 0;
        unsigned long int best_val = -1000;
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
int load_image_file(int x, int y)
{
    if (x < 0 || y < 0)
    {
        auto start = high_resolution_clock::now();
        i_img = cv::imread("/root/hsa-soc-local/img/dices4.jpg", cv::IMREAD_GRAYSCALE);
        t_img = cv::imread("/root/hsa-soc-local/img/dices4.jpg", cv::IMREAD_GRAYSCALE);
        // draw the target for inspection
        res = i_img.clone();
        w = t_img.cols;
        h = t_img.rows;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Read: " << duration.count() << " us\n";

        Mat img0 = t_img.clone();
        Point pt1(a, b);
        Point pt2(c, d);
        rectangle(img0, pt1, pt2, cv::Scalar(0, 0, 0));
        imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
        img0.release();
    }
}
int region_of_interest(int x, int y)
{
    if (x < 0 || y < 0 || x >= w || y >= h)
    {
        rect = cv::Rect(u, v, n, m);
        t_img_roi = t_img(rect);
        t_img_roi.convertTo(t_img_roi, CV_8U);
        t_data = (long int *)t_img_roi.data;
    }
    else
    {
        rect = cv::Rect(x, y, n, m);
        i_img_roi = i_img(rect);
        i_img_roi.convertTo(i_img_roi, CV_8U);
        i_data = (long int *)i_img_roi.data;
    }
}
int load_init_file(int x, int y)
{
    if (x < 0 || y < 0)
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
        data_len = n_times_m;
    }
}
int init_zncc(int x, int y)
{
    load_init_file(x, y);
    load_image_file(x, y);
    region_of_interest(x, y);
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
    axi_bram_ctrl_0 = map_mem(bram_bytes, axi_bram_ctrl_0_addr);
    axi_bram_ctrl_1 = map_mem(bram_bytes, axi_bram_ctrl_1_addr);
    axi_bram_ctrl_2 = map_mem(bram_bytes, axi_bram_ctrl_2_addr);
    axi_gpio_0 = map_mem(gpio_bytes, axi_gpio_0_addr);
    axi_gpio_1 = map_mem(gpio_bytes, axi_gpio_1_addr);
    axi_gpio_2 = map_mem(gpio_bytes, axi_gpio_2_addr);
}
int close_mem()
{
    close(fd);
}
int main()
{
    // report
    result.open("result.csv");

    open_mem();

    mu_bees = (double *)malloc(num_bees_comp * sizeof(double));
    mu_obj = (signed long int *)malloc(num_bees * sizeof(signed long int));
    lambda_bees = (double *)malloc(num_bees_comp * sizeof(double));
    lambda_obj = (signed long int *)malloc(num_bees * sizeof(signed long int));
    mu_lambda_bees = (double *)malloc(num_bees_comp * 2 * sizeof(double));
    mu_lambda_obj = (signed long int *)malloc(num_bees * 2 * sizeof(signed long int));

    // template
    init_zncc(-1, -1);
    cout << "data len: " << data_len << " bytes\n";
    memcpy(axi_bram_ctrl_1 + 4, t_data, data_len);
    result << "write,work\n";
    auto start0 = high_resolution_clock::now();

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
    initial_random_pop(limits);

    for (int generation = 0; generation < max_gen; generation++)
    {
        // Evaluate parent population
        eval_pop(mu_bees, mu_obj, limits);

        // Generate lamdba population
        generate_new_pop(limits);

        // Evaluate new population
        eval_pop(lambda_bees, lambda_obj, limits);

        // Mu + Lambda
        merge_pop();

        // Select best mu
        best_mu();
    }

    cout << "time clear: " << time_clear << " us\n";
    cout << "time write: " << time_write << " us\n";
    cout << "time work: " << time_work << " us\n";
    result << "sum write,sum work,sum clean\n";
    result << time_write << "," << time_work << "," << time_clear << "\n";

    /*double time_clear_avg = (double)time_clear / (double)test_cycles;
    double time_write_avg = (double)time_write / (double)actual_tests;
    double time_work_avg = (double)time_work / (double)actual_tests;

    cout << "time clear per cycle: " << time_clear_avg << " us\n";
    cout << "time write per test: " << time_write_avg << " us\n";
    cout << "time work per test: " << time_work_avg << " us\n";
    result << "avg write,avg work,avg clean\n";
    result << time_write_avg << "," << time_work_avg << "," << time_clear_avg << "\n";

    int i = best_test + best_test_cycle * test_count;
    int u1 = i % (w - n);
    int v1 = i / (w - n);

    cout << "location: (" << u1 << "," << v1 << ")\n";
    cout << "prev: (" << u << "," << v << ")\n";*/

    close_mem();
    result.close();
    return 0;
}