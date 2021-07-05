// g++ zncc_opencv_pl_auto.cpp -o zncc_opencv_pl_auto `pkg-config --cflags --libs opencv`
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
long int *t_data;
long int *i_data;
Mat i_img;
Mat t_img;
Mat i_img_roi;
Mat t_img_roi;
Mat res;
Rect rect;
// report
ofstream result;
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
        Mat img0 = t_img.clone();
        Point pt1(a, b);
        Point pt2(c, d);
        rectangle(img0, pt1, pt2, cv::Scalar(0, 0, 0));
        imwrite("/root/hsa-soc-local/img/dices0.jpg", img0);
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

    // template
    init_zncc(-1, -1);
    unsigned int data_len = n_times_m;
    if (bram_length - 4 < n_times_m)
    {
        data_len = bram_length - 4;
    }
    cout << "data len: " << data_len << " bytes\n";
    memcpy(axi_bram_ctrl_1 + 4, t_data, data_len);

    // settings
    int test_count = (bram_length / 4) - 1;
    int actual_tests = (w - n) * (h - m);
    int test_cycles = (actual_tests / test_count) + 1;
    int real_test_index = 0;
    int x = 0;
    int y = 0;
    cout << "actual tests: " << actual_tests << "\n";
    cout << "test cycles: " << test_cycles << "\n";

    unsigned long int time_clear = 0;
    unsigned long int time_write = 0;
    unsigned long int time_work = 0;

    signed long int max_zncc = -1000;
    int best_test_cycle = 0;
    int best_test = 0;

    result << "write,work\n";
    for (int test_cycle = 0; test_cycle < test_cycles; test_cycle++)
    {
        auto start0 = high_resolution_clock::now();
        if (test_cycle > 0)
        {
            unsigned long int i = axi_gpio_0[0];
            signed long int z = (signed long int)axi_bram_ctrl_2[i];
            if (max_zncc < z)
            {
                max_zncc = z;
                best_test_cycle = test_cycle - 1;
                best_test = i;
            }
        }

        // reset the system
        axi_bram_ctrl_0[0] = 0xFFFFFFFF;

        auto stop0 = high_resolution_clock::now();
        auto duration0 = duration_cast<microseconds>(stop0 - start0);
        time_clear += duration0.count();

        for (int i = 0; i < test_count; i++)
        {
            if (real_test_index >= actual_tests)
            {
                break;
            }
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

            // progress
            real_test_index++;
            x++;
            if (x >= w - n)
            {
                y++;
                x = 0;
            }
        }
        cout << "cycle: " << test_cycle << "/" << test_cycles << " ";
        cout << "max zncc: " << max_zncc << "\n";
    }

    cout << "time clear: " << time_clear << " us\n";
    cout << "time write: " << time_write << " us\n";
    cout << "time work: " << time_work << " us\n";
    result << "sum write,sum work,sum clean\n";
    result << time_write << "," << time_work << "," << time_clear << "\n";

    double time_clear_avg = (double)time_clear / (double)test_cycles;
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
    cout << "prev: (" << u << "," << v << ")\n";

    close_mem();
    result.close();
    return 0;
}