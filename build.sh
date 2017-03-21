#!/bin/bash
#Compile kernel with a build script to make things simple
mkdir -p out
export CROSS_COMPILE=/home/stas/Android/utility/gcc6.2/bin/arm-linux-gnueabi-
export USE_CCACHE=1
export ARCH=arm ARCH_MTK_PLATFORM=mt6580
make -C /home/stas/Android/ZC500TG O=/home/stas/Android/ZC500TG/out ARCH=arm zc500tg_defconfig
#Edit the number according to the number of CPUs you have in your PC:
make -j2 -C /home/stas/Android/ZC500TG O=/home/stas/Android/ZC500TG/out ARCH=arm

#Сборка образа и прошивка
cp /home/stas/Android/ZC500TG/out/arch/arm/boot/zImage-dtb /home/stas/Android/CarlivImageKitchen64/recoveryCWM/recovery.img-kernel
cd /home/stas/Android/CarlivImageKitchen64/
./carliv
cp /home/stas/Android/CarlivImageKitchen64/output/CWM.img /home/stas/CWM.img 
read -s -n 1 -p "Press any key to flashing recovery...";
fastboot flash recovery /home/stas/CWM.img 
read -s -n 1 -p "Press any key to take dmesg...";
adb shell dmesg > dmesgN.log


