# HSA SoC Local Server
This is a collection of scripts to perform HSA on a SoC-FPGA.

# Installation

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