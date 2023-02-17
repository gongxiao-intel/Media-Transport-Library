@page build_linux Build in Linux
# Build Guide

The build required 3 parts, build the DPDK library, the Kahawai library on top of DPDK, and the sample application.

## 1. Install the build dependency
#### 1.1 Ubuntu/Debian:
```bash
sudo apt-get install git gcc meson python3 python3-pip pkg-config libnuma-dev libjson-c-dev libpcap-dev libgtest-dev libsdl2-dev libsdl2-ttf-dev libssl-dev
sudo pip install pyelftools ninja
```
#### 1.2 Centos:
```bash
sudo yum install git gcc meson python3 python3-pip pkg-config libnuma-devel json-c-devel libpcap-devel gtest-devel SDL2-devel SDL2_ttf-devel openssl-devel
sudo pip install pyelftools ninja
```

## 2. DPDK build and install:

#### 2.1 Get DPDK i225 launch time enabled source code
```bash
git clone  https://github.com/intel-innersource/networking.dataplane.dpdk.next-net-intel.git dpdk
cd dpdk
git checkout 916ce02a6d6ec41d212c49aab391c0630f3bd019
```

#### 2.2 Apply the DPDK patches required to run kahawai.
Note: $dpdk_st_kahawai point to source code of ST DPDK kahawai.
```bash
git am $dpdk_st_kahawai/patches/dpdk/22.07/0001-pcapng-add-ns-timestamp-for-copy-api.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0002-net-af_xdp-parse-numa-node-id-from-sysfs.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0003-net-iavf-refine-queue-rate-limit-configure.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0004-net-ice-revert-PF-ICE-rate-limit-to-non-queue-group-.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0005-net-iavf-support-max-burst-size-configuration.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0006-net-ice-support-max-burst-size-configuration.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0007-Add-support-for-i225-IT-ethernet-device-into-igc-pmd.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0008-Change-to-enable-PTP.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0009-ice-fix-ice_interrupt_handler-panic-when-stop.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0010-igc-enable-launch-time-transmit-feature.patch
git am $dpdk_st_kahawai/patches/dpdk/22.07/0011-enable-qbv-and-extend-packet-buffer-size.patch
```

#### 2.3 Build and install DPDK library
```bash
meson -Dexamples=timesync build
ninja -C build
cd build
sudo ninja install
pkg-config --cflags libdpdk
pkg-config --libs libdpdk
pkg-config --modversion libdpdk
```

## 3. DPDK ST kahawai lib and app:
```bash
./build.sh
```
