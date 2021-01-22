// g++ zncc_unit_opencv_plcontrol.cpp -o zncc_unit_opencv_plcontrol `pkg-config --cflags --libs opencv`
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
using namespace std;
using namespace std::chrono;
// AXI addresses
off_t axi_bram_ctrl_0_addr = 0x40000000;
off_t axi_bram_ctrl_1_addr = 0x42000000;
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
off_t axi_gpio_2_addr = 0x41220000;
off_t axi_gpio_3_addr = 0x41230000;
off_t axi_gpio_4_addr = 0x41240000;
off_t axi_gpio_5_addr = 0x41250000;
// AXI pointers
long int *axi_bram_ctrl_0;
long int *axi_bram_ctrl_1;
long int *axi_gpio_0;
long int *axi_gpio_1;
long int *axi_gpio_2;
long int *axi_gpio_3;
long int *axi_gpio_4;
long int *axi_gpio_5;
// dev mem
int fd;
// constants of PL design
long int conf_clear = 0b10;
long int conf_squared = 0b100;
long int conf_not_squared = 0b000;
long int conf_work = 0b1;
long int conf_wait = 0b0;
long int conf_save = 0b1000;
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
int n_times_m;
long int *t_data;
long int *i_data;
cv::Mat i_img;
cv::Mat t_img;
cv::Mat i_img_roi;
cv::Mat t_img_roi;
cv::Mat res;
cv::Rect rect;
// PL variables
long int acc_i;
long int acc_t;
long int acc_cross;
// functions
int load_image_file(int x, int y)
{
    if (x < 0 || y < 0)
    {
        auto start = high_resolution_clock::now();
        i_img = cv::imread("/root/hsa-soc-local/img/dices4.jpg", cv::IMREAD_GRAYSCALE);
        t_img = cv::imread("/root/hsa-soc-local/img/dices4.jpg", cv::IMREAD_GRAYSCALE);
        // draw the target for inspection
        res = i_img.clone();
        cv::Mat img0 = t_img.clone();
        cv::Point pt1(a, b);
        cv::Point pt2(c, d);
        cv::rectangle(img0, pt1, pt2, cv::Scalar(0, 0, 0));
        cv::imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
        img0.release();
        w = t_img.cols;
        h = t_img.rows;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Read: " << duration.count() << " us\n";
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
    }
}
int init_zncc(int x, int y)
{
    load_init_file(x, y);
    load_image_file(x, y);
    region_of_interest(x, y);
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
}
int close_mem()
{
    close(fd);
}
int clear_acc()
{
    cout << "Clear acc\n";
    axi_gpio_1[0] = conf_clear;
    axi_gpio_1[0] = conf_wait;
}
int set_avg(long int i_avg, long int t_avg)
{
    axi_gpio_0[0] = i_avg;
    axi_gpio_1[0] = conf_save;
    axi_gpio_1[0] = conf_wait;
    axi_gpio_0[0] = t_avg;
    axi_gpio_1[0] = conf_save;
    axi_gpio_1[0] = conf_wait;
}
int set_offset(long int offset)
{
    axi_gpio_0[0] = offset;
    axi_gpio_1[0] = conf_save;
    axi_gpio_1[0] = conf_wait;
}
long int get_acc(long int squared_or_not, long int avg_i, long int avg_t)
{
    clear_acc();
    int rounds = n_times_m / bram_length;
    int remain = n_times_m - (bram_length * rounds);
    acc_i = 0;
    acc_t = 0;
    acc_cross = 0;
    // for each round
    for (int r = 0; r < rounds + 1; r++)
    {
        cout << "round " << r << " of " << rounds << "\n";
        int limit = bram_length;
        if (r == rounds)
        {
            limit = remain;
        }
        cout << "limit " << limit << "\n";
        auto start = high_resolution_clock::now();
        // copy a block of data to PL
        memcpy(axi_bram_ctrl_0, i_data + (r * bram_length), limit);
        memcpy(axi_bram_ctrl_1, t_data + (r * bram_length), limit);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Write: " << duration.count() << " us\n";
        // get acc
        start = high_resolution_clock::now();
        int units = 2;
        int limit_fraction = limit / units;
        set_avg(avg_i, avg_t);
        set_offset(limit_fraction);
        cout << "limit_fraction " << limit_fraction << "\n";
        cout << "count: " << axi_gpio_5[0] << "\n";
        cout << "acci: " << axi_gpio_2[0] << "\n";
        cout << "acct: " << axi_gpio_3[0] << "\n";
        cout << "acccross: " << axi_gpio_4[0] << "\n";
        axi_gpio_1[0] = conf_work | squared_or_not;
        while (axi_gpio_5[0] < limit_fraction)
        {
            cout << "count: " << axi_gpio_5[0] << "\n";
            cout << "acci: " << axi_gpio_2[0] << "\n";
            cout << "acct: " << axi_gpio_3[0] << "\n";
            cout << "acccross: " << axi_gpio_4[0] << "\n";
        }
        acc_i += axi_gpio_2[0];
        acc_t += axi_gpio_3[0];
        acc_cross += axi_gpio_4[0];
        cout << acc_i << " " << acc_t << " " << acc_cross << "\n";
        clear_acc();
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Process: " << duration.count() << " us\n";
    }
}
int main()
{
    double zncc = 0;
    double max_zncc = -3.0;
    int u1 = 0;
    int v1 = 0;
    open_mem();
    init_zncc(-1, -1);
    //for (int x = 0; x < w - n; x++)
    for (int x = u; x < u + 1; x++)
    {
        //for (int y = 0; y < h - m; y++)
        for (int y = v; y < v + 1; y++)
        {
            std::cout << "(" << x << "," << y << ")\n";
            init_zncc(x, y);
            // get averages
            std::cout << "averages \n";
            get_acc(conf_not_squared, 0, 0);
            long int avg_i = acc_i / n_times_m;
            long int avg_t = acc_t / n_times_m;
            std::cout << avg_i << " " << avg_t << "\n";
            // get sum of squared err and cross correlation
            std::cout << "stats \n";
            get_acc(conf_squared, avg_i, avg_t);
            double err_i = acc_i * 1.0;
            double err_t = acc_t * 1.0;
            double corr = acc_cross * 1.0;
            // get zncc
            zncc = corr / sqrt(err_i * err_t);
            if (max_zncc < zncc)
            {
                max_zncc = zncc;
                u1 = x;
                v1 = y;
                if (max_zncc > 0.8)
                {
                    // draw candidate in result
                    cv::Point pt1(u1, v1);
                    cv::Point pt2(u1 + n, v1 + m);
                    cv::rectangle(res, pt1, pt2, cv::Scalar(0, 255, 0));
                    cv::imwrite("/root/hsa-soc-local/img/dices1.jpg", res);
                }
            }
            std::cout << "max zncc: " << max_zncc << "\n";
        }
    }
    close_mem();
    return 0;
}