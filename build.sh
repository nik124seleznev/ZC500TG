#!/bin/bash
#Compile kernel with a build script to make things simple
mkdir -p out
export CROSS_COMPILE=/home/nik-kst/Android/utility/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-
export USE_CCACHE=1
export ARCH=arm ARCH_MTK_PLATFORM=mt6580
make -C /home/nik-kst/Android/ZC500TG O=/home/nik-kst/Android/ZC500TG/out ARCH=arm zc500tg_defconfig
#Edit the number according to the number of CPUs you have in your PC:
make -j4 -C /home/nik-kst/Android/ZC500TG O=/home/nik-kst/Android/ZC500TG/out ARCH=arm

#Сборка образа
cp /home/nik-kst/Android/ZC500TG/out/arch/arm/boot/zImage-dtb /home/nik-kst/Android/utility/CarlivImageKitchen64/recovery/recovery.img-kernel
cd /home/nik-kst/Android/utility/CarlivImageKitchen64/
./carliv
