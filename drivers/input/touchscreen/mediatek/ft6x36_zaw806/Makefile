ccflags-y += -Wno-unused-variable -g
ccflags-y += -Wno-unused-function -g

# Linux driver folder

ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/ft6x36_zaw806/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/

ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/accdet/$(MTK_PLATFORM)/

obj-y	+=  focaltech_driver.o
obj-y	+=  focaltech_ctl.o
obj-y	+=  focaltech_ex_fun.o
