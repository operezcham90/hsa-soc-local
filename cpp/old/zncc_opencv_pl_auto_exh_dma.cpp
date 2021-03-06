// g++ zncc_opencv_pl_auto_exh_dma.cpp -o zncc_opencv_pl_auto_exh_dma `pkg-config --cflags --libs opencv`
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
// AXI pointers
unsigned long int *axi_cdma_0;
unsigned long int *axi_gpio_0;
unsigned long int *axi_gpio_1;
unsigned long int *bram_r;
unsigned long int *bram_t;
unsigned long int *bram_i_0;
// AXI addresses
off_t axi_cdma_0_addr = 0x7E200000;
off_t axi_bram_ctrl_0_addr = 0xC0000000;
off_t axi_bram_ctrl_t_addr = 0xC2000000;
off_t axi_bram_ctrl_r_0_addr = 0xC4000000;
off_t axi_gpio_0_addr = 0x41200000;
off_t axi_gpio_1_addr = 0x41210000;
// dev mem
int fd;
// constants of PL design
unsigned int bram_bytes = 2048 * 4;
unsigned int gpio_bytes = 4;
long int bram_length = 2048 * 4;
unsigned int cdma_bytes = 40;
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
unsigned long int *t_data;
unsigned long int *i_data;
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
        t_data = (unsigned long int *)t_img_roi.data;
    }
    else
    {
        rect = cv::Rect(x, y, n, m);
        i_img_roi = i_img(rect);
        i_img_roi.convertTo(i_img_roi, CV_8U);
        i_data = (unsigned long int *)i_img_roi.data;
    }
    //cout << "ROI: done\n";
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
    //cout << "Init: done\n";
}
int init_zncc(int x, int y)
{
    load_init_file(x, y);
    load_image_file(x, y);
    region_of_interest(x, y);
    //cout << "Begin ZNCC: done\n";
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
    axi_cdma_0 = map_mem(cdma_bytes, axi_cdma_0_addr);
    axi_gpio_0 = map_mem(gpio_bytes, axi_gpio_0_addr);
    axi_gpio_1 = map_mem(gpio_bytes, axi_gpio_1_addr);
    bram_r = map_mem(bram_bytes, 0x0e000000);
    bram_t = map_mem(bram_bytes, 0x0f000000);
    bram_i_0 = map_mem(bram_bytes, 0x10000000);
    cout << "Mem: open\n";
}
int close_mem()
{
    close(fd);
    cout << "Mem: closed\n";
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
    memcpy(bram_t + 4, t_data, data_len);
    bram_t[0] = 0;
    // reset
    while ((axi_cdma_0[1] & 0x00000002) == 0)
    {
        axi_cdma_0[0] = 0x00000004;
    }
    // set CDMA
    axi_cdma_0[6] = 0x0f000000;
    axi_cdma_0[8] = axi_bram_ctrl_t_addr;
    axi_cdma_0[10] = data_len + 4;
    // wait transfer
    while ((axi_cdma_0[1] & 0x00000002) == 0)
    {
    }

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
            // set CDMA
            axi_cdma_0[6] = axi_bram_ctrl_r_0_addr;
            axi_cdma_0[8] = 0x0e000000;
            axi_cdma_0[10] = bram_bytes;
            // wait transfer
            while ((axi_cdma_0[1] & 0x00000002) == 0)
            {
            }
            unsigned long int i = axi_gpio_1[0];
            signed long int z = (signed long int)bram_r[i];
            cout << "max " << i << " : " << z;
            if (max_zncc < z)
            {
                max_zncc = z;
                best_test_cycle = test_cycle - 1;
                best_test = i;
            }
        }

        unsigned long int reset = 0xFFFFFFFF;
        // reset the system
        // set CDMA
        bram_i_0[0] = reset;
        axi_cdma_0[6] = 0x10000000;
        axi_cdma_0[8] = axi_bram_ctrl_0_addr;
        axi_cdma_0[10] = 4;
        // wait transfer
        while ((axi_cdma_0[1] & 0x00000002) == 0)
        {
        }

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
            memcpy(bram_i_0 + 4, i_data, data_len);
            bram_i_0[0] = 0;
            // set CDMA
            axi_cdma_0[6] = 0x10000000;
            axi_cdma_0[8] = axi_bram_ctrl_0_addr;
            axi_cdma_0[10] = data_len + 4;
            // wait transfer
            while ((axi_cdma_0[1] & 0x00000002) == 0)
            {
            }
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            time_write += duration.count();
            result << duration.count() << ",";

            // copy data length
            auto start2 = high_resolution_clock::now();
            bram_i_0[0] = data_len;
            // set CDMA
            axi_cdma_0[6] = 0x10000000;
            axi_cdma_0[8] = axi_bram_ctrl_0_addr;
            axi_cdma_0[10] = 4;
            // wait transfer
            while ((axi_cdma_0[1] & 0x00000002) == 0)
            {
            }
            // wait for standby signal
            while (axi_gpio_0[0] != 0)
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