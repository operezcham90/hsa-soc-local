# HSA SoC Local Server

This is a collection of scripts to perform HSA on a SoC-FPGA. This project is developed for the [ZC706 board](https://www.xilinx.com/products/boards-and-kits/ek-z7-zc706-g.html) that has a [Zynq-7000 SoC](https://www.xilinx.com/products/silicon-devices/soc/zynq-7000.html). The Processing System (PS) has two [ARM A9](https://developer.arm.com/ip-products/processors/cortex-a/cortex-a9) CPUs and the Programmable Logic (PL) has the properties of an [Artix-7](https://www.xilinx.com/products/silicon-devices/fpga/artix-7.html) FPGA. [Vivado HLx 2017.3](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html) was used.

# SoC Setup and OS Installation

An SD card is used as root file system. Three sections are created using gparted (Ubuntu tool). The three sections have the following properties:

* unallocated space of 4 MiB.
* fat32 partition, label `BOOT`, size of 36 MiB
* ext4 partition, label `rootfs`, size of 7.36 GiB (all remaining space)

The root folder structure is downloaded and written to the SD card.

```
wget http://releases.linaro.org/archive/12.12/ubuntu/vexpress/linaro-precise-ubuntu-desktop-20121124-560.tar.gz
sudo tar --strip-components=3 -C /media/ubuntu/rootfs -xzpf linaro-precise-ubuntu-desktop-20121124-560.tar.gz binary/boot/filesystem.dir
ls /media/ubuntu/rootfs/
```

Download the `boot` folder of this repository and copy the contents to the `BOOT` partition.

Make sure that the configuration switch SW11 is set to SD program mode (00110).

![SW11](https://i.imgur.com/8GRIOfN.png)

# Software Installation

Install SSH if remote access is required.

```
sudo apt-get install openssh-server
sudo /etc/init.d/ssh start
```

To change the root passwrod:

```
passwd
```

Upgrade GCC compiler.

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc g++ gcc-5 g++-5 gcc-6 g++-6
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 80 --slave /usr/bin/g++ g++ /usr/bin/g++-6
```

Install NodeJS and NPM.

```
wget https://nodejs.org/dist/latest-v9.x/node-v9.11.2-linux-armv7l.tar.gz
tar -xzf node-v9.11.2-linux-armv7l.tar.gz
cd node*
sudo cp -R * /usr/local/
node -v
npm -v
```

Install Git and clone this repository.

```
sudo apt-get install git
git clone https://github.com/operezcham90/hsa-soc-local.git
```

Upgrade CMake.

```
git clone https://github.com/Kitware/CMake.git
cd CMake
git checkout tags/v3.0.0
./configure
make
sudo make install
```

Install OpenCV.

```
git clone --branch 2.4 https://github.com/opencv/opencv.git
cd opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
```

Building OpenCV can be time consuming. An alternative is to download the precompiled `build` folder. Download link: [opencv_zynq7000.tar.gz](https://github.com/operezcham90/opencv_zynq7000/releases/download/release/opencv_zynq7000.tar.gz).

Find the full path to the OpenCV libraries. It may be `/usr/local/lib/`.

```
sudo find / -name "libopencv_core.so.2.4*"
```

Write the path you found to the OpenCV configuration file.

```
sudo nano /etc/ld.so.conf.d/opencv.conf
```

Load the configuration file.

```
sudo ldconfig -v
```

Install Curl.

```
sudo apt-get install curl
```

Modify the `config.js` file to set the `SECRET` environment variable (use the same value of the remote server).

```
sudo nano hsa-soc-local/js/config.js
```

Set the startup script to run on boot.

```
npm install -g pm2
pm2 start hsa-soc-local/js/server.js
pm2 save
pm2 startup
```