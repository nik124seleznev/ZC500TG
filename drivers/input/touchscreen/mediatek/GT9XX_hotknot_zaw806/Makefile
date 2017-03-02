ccflags-y += -Wno-unused-variable -g
ccflags-y += -Wno-unused-function -g

# Linux driver folder

ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9XX_hotknot_zaw806/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/

ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/accdet/$(MTK_PLATFORM)/

obj-y	+=  goodix_tool.o
obj-y	+=  gt9xx_driver.o
obj-y	+=  gt9xx_update.o
