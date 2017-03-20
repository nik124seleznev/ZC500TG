#!/bin/bash

#Mysteryagr
#Compile kernel with a build script to make things simple

mkdir -p out

#Change toolchain path before using build script!
export CROSS_COMPILE=/home/stas/Android/utility/gcc6.2/bin/arm-linux-gnueabi-

#Enable when needed:
export USE_CCACHE=1

export ARCH=arm ARCH_MTK_PLATFORM=mt6580

#Enable only when needed:
#make clean
#make mrproper
#Or simply delete out directory to clean source


make -C /home/stas/Android/ZC500TG O=/home/stas/Android/ZC500TG/out ARCH=arm zc500tg_defconfig

#Edit the number according to the number of CPUs you have in your PC:
make -j2 -C /home/stas/Android/ZC500TG O=/home/stas/Android/ZC500TG/out ARCH=arm
