# HSA SoC Local Server
This is a collection of scripts to perform HSA on a SoC-FPGA.

# Installation

Setup GCC compiler.

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc g++ gcc-5 g++-5 gcc-6 g++-6
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 80 --slave /usr/bin/g++ g++ /usr/bin/g++-6
```

Setup NodeJS and NPM (GCC is needed).

```
wget https://nodejs.org/dist/latest-v9.x/node-v9.11.2-linux-armv7l.tar.gz
tar -xzf node-v9.11.2-linux-armv7l.tar.gz
cd node*
sudo cp -R * /usr/local/
node -v
npm -v
```

Setup Git and clone this repository.

```
sudo apt-get install git
git clone https://github.com/operezcham90/hsa-soc-local.git
```

Setup CMake.
```
git clone https://github.com/Kitware/CMake.git
cd CMake
git checkout tags/v3.0.0
./configure
make
sudo make install
```

Setup Curl.

```
sudo apt-get install curl
```

Modify the config.js file to set the SECRET environment variable (use the same value of the remote server).

Set the startup script to run on boot.

```
npm install -g pm2
pm2 start hsa-soc-local/js/server.js
pm2 save
pm2 startup
```