/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"
#include <cust_adc.h>    	// dongteng add for lcm detect
// dongteng add for lcm detect ,read adc voltage
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mt-plat/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)

#define MIN_VOLTAGE (200)     // dongteng add for lcm detect
#define MAX_VOLTAGE (1200)     // dongteng add for lcm detect
#define COMPARE_BY_ADC   0

#define REGFLAG_DELAY          	0XFE
#define REGFLAG_END_OF_TABLE  	0xFA  // END OF REGISTERS MARKER

#define LCM_ID_OTM1287 (0x1287)

#define LCM_DSI_CMD_MODE									0
#define MIPI_VIDEO_MODE

#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static unsigned int lcm_compare_id(void);

static struct LCM_setting_table lcm_initialization_setting[] = {
{0x00,1,{0x00}}, 
{0xFF,3,{0x12,0x83,0x01}},
  
  
{0x00,1,{0x80}}, 
{0xFF,2,{0x12,0x83}},
     
     
     
{0x00,1,{0x80}},  
{0xC0,9,{0x00,0x64,0x00,0x0f,0x11,0x00,0x64,0x0f,0x11}},
     
     
{0x00,1,{0x90}}, 
{0xC0,6,{0x00,0x55,0x00,0x01,0x00,0x04}},
    
    
{0x00,1,{0xA4}},  
{0xC0,1,{0x00}},
     
     
{0x00,1,{0xB3}}, 
{0xC0,2,{0x00,0x50}},// 10 1+2dot 50 liefanzhuan
     
     
{0x00,1,{0x81}}, 
{0xC1,1,{0x55}},
    
    
{0x00,1,{0x81}}, 
{0xC4,1,{0x82}},
     
{0x00,1,{0x82}}, 
{0xC4,1,{0x02}},    
     
     
{0x00,1,{0x90}},
{0xC4,1,{0x49}},
   
{0x00,1,{0xc6}},
{0xb0,1,{0x03}},  
     
{0x00,1,{0x8b}}, 
{0xC4,1,{0x40}},

{0x00,1,{0x90}},  
{0xf5,4,{0x02,0x11,0x02,0x11}},
     
     
{0x00,1,{0x90}},  
{0xc5,1,{0x50}},
     
     
{0x00,1,{0x94}},  
{0xc5,1,{0x66}},//7d
     
     
{0x00,1,{0xb2}}, 
{0xf5,2,{0x00,0x00}},
     
     
     
{0x00,1,{0xB4}}, 
{0xf5,2,{0x00,0x00}},
     
     
{0x00,1,{0xB6}},  
{0xf5,2,{0x00,0x00}},
     
     
     
{0x00,1,{0xb8}},  
{0xf5,2,{0x00,0x00}},
    
    
{0x00,1,{0x94}},  
{0xf5,1,{0x02}},
     
     
     
{0x00,1,{0xba}},  
{0xf5,1,{0x03}},
     
     
{0x00,1,{0xb2}}, 
{0xc5,1,{0x40}},
     
{0x00,1,{0xb4}}, 
{0xc5,1,{0xc0}},   
  
      
{0x00,1,{0xa0}},  
{0xCB,14,{0xc4,0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10}},
  
{0x00,1,{0xb0}}, 
{0xc4,1,{0x00,0x00}},    
     
{0x00,1,{0x91}},

{0xc5,2,{0x19,0x50}},

{0x00,1,{0x00}},             //GVDD=4.87V, NGVDD=-4.87V
{0xd8,2,{0xbc,0xbc}},

{0x00,1,{0xb0}},             //VDD_18V=1.6V, LVDSVDD=1.55V
{0xc5,1,{0x04,0xb8}},

{0x00,1,{0xbb}},             //LVD voltage level setting
{0xc5,1,{0x80}},

//-------------------- control setting ---------------------------------------------------//
{0x00,1,{0x00}},             //ID1
{0xd0,1,{0x40}},

{0x00,1,{0x00}},             //ID2, ID3
{0xd1,2,{0x00,0x00}},

//-------------------- GAMMA TUNING ------------------------------------------//
{0x00,1,{0x00}},            
{0xE1,16,{0x05,0x0E,0x14,0x0E,0x06,0x11,0x0B,0x0A,0x00,0x05,0x0A,0x03,0x0E,0x15,0x10,0x05}},

{0x00,1,{0x00}},            
{0xE2,16,{0x05,0x0E,0x14,0x0E,0x06,0x0F,0x0D,0x0c,0x04,0x07,0x0E,0x0B,0x10,0x15,0x10,0x05}},	

{0x00,1,{0x00}},            
{0xD9,1,{0x60}},//73

//-------------------- panel timing state control ------------------------------------------//
{0x00,1,{0x80}},             //panel timing state control
{0xcb,11,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0x90}},             //panel timing state control
{0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xa0}},             //panel timing state control
{0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xb0}},             //panel timing state control
{0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xc0}},             //panel timing state control
{0xcb,15,{0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xd0}},             //panel timing state control
{0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00}},

{0x00,1,{0xe0}},             //panel timing state control
{0xcb,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05}},

{0x00,1,{0xf0}},             //panel timing state control
{0xcb,11,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},

//-------------------- panel pad mapping control --------------------//
{0x00,1,{0x80}},            //panel pad mapping control
{0xcc,15,{0x0a,0x0c,0x0e,0x10,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0x90}},             //panel pad mapping control
{0xcc,15,{0x00,0x00,0x00,0x00,0x00,0x2e,0x2d,0x09,0x0b,0x0d,0x0f,0x01,0x03,0x00,0x00}},

{0x00,1,{0xa0}},           //panel pad mapping control
{0xcc,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e,0x2d}},

{0x00,1,{0xb0}},            //panel pad mapping control
{0xcc,15,{0x0F,0x0D,0x0B,0x09,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 

{0x00,1,{0xc0}},            //panel pad mapping control
{0xcc,15,{0x00,0x00,0x00,0x00,0x00,0x2d,0x2e,0x10,0x0E,0x0C,0x0A,0x04,0x02,0x00,0x00}}, 

{0x00,1,{0xd0}},             //panel pad mapping control
{0xcc,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2d,0x2e}}, 

//-------------------- panel timing setting --------------------//
{0x00,1,{0x80}},            //panel VST setting
{0xce,12,{0x8d,0x03,0x00,0x8c,0x03,0x00,0x8b,0x03,0x00,0x8a,0x03,0x00}},

{0x00,1,{0xa0}},             //panel CLKA1/2 setting
{0xce,14,{0x38,0x0b,0x04,0xfc,0x00,0x10,0x10,0x38,0x0a,0x04,0xfd,0x00,0x10,0x10}},

{0x00,1,{0xb0}},            //panel CLKA3/4 setting
{0xce,14,{0x38,0x09,0x04,0xfe,0x00,0x10,0x10,0x38,0x08,0x04,0xff,0x00,0x10,0x10}},

{0x00,1,{0xc0}},           //panel CLKb1/2 setting
{0xce,14,{0x38,0x07,0x05,0x00,0x00,0x10,0x10,0x38,0x06,0x05,0x01,0x00,0x10,0x10}},

{0x00,1,{0xd0}},             //panel CLKb3/4 setting
{0xce,14,{0x38,0x05,0x05,0x02,0x00,0x10,0x10,0x38,0x04,0x05,0x03,0x00,0x10,0x10}},

{0x00,1,{0x80}},             //panel CLKc1/2 setting
{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0x90}},             //panel CLKc3/4 setting
{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xa0}},             //panel CLKd1/2 setting
{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xb0}},             //panel CLKd3/4 setting
{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xc0}},             //panel ECLK setting, gate pre. ena.
{0xcf,11,{0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0x08}},

{0x00,1,{0xb5}},             //TCON_GOA_OUT Setting
{0xc5,6,{0x33,0xf1,0xff,0x33,0xf1,0xff}},  //normal output with VGH/VGL

{0x00,1,{0x00}},           
{0xFF,3,{0xFF,0xFF,0xFF}},


{0x11,1,{ 0x00 }},
{REGFLAG_DELAY, 120, {}},

{0x29,1,{ 0x00 }},

};

static void lcm_init_registers(void)
{
	unsigned int data_array[16];
	
	data_array[0] = 0x00002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00042902;
	data_array[1] = 0x018712FF;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x80002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x008712ff;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x92002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x000220ff;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x80002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000a2902;
	data_array[1] = 0x006400c0;
	data_array[2] = 0x64001010;
	data_array[3] = 0x00001010;
	dsi_set_cmdq(data_array, 4, 1);

	data_array[0] = 0x90002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00072902;
	data_array[1] = 0x004b00c0;
	data_array[2] = 0x00040001;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0xb3002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x005500c0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x81002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x000055c1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0xa0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x041005c4;
	data_array[2] = 0x11150502;
	data_array[3] = 0x02071005;
	data_array[4] = 0x00111505;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xb0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x000000c4;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x91002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x00d2a6c5;
	dsi_set_cmdq(data_array, 2, 1);

	
	data_array[0] = 0x00002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x00B7B7d8;
	dsi_set_cmdq(data_array, 2, 1);


	data_array[0] = 0xb3002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x000084c5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0xbb002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x00008ac5;
	dsi_set_cmdq(data_array, 2, 1);

	

	data_array[0] = 0x82002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x00000ac4;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0xc6002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x000003b0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x000040d0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x000000d1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0xb2002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x000000f5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0xb6002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x000000f5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x94002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x000000f5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0xd2002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00032902;
	data_array[1] = 0x001506f5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0xb4002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x0000ccc5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x90002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00052902;
	data_array[1] = 0x021102f5;	
	data_array[2] = 0x00000015;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x90002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x000050c5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x94002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902;
	data_array[1] = 0x000066c5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x80002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000c2902;
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	dsi_set_cmdq(data_array, 4, 1);

	data_array[0] = 0x90002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xA0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xB0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xC0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x050505cb;
	data_array[2] = 0x05050505;
	data_array[3] = 0x05000505;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xD0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000500;
	data_array[3] = 0x05050505;
	data_array[4] = 0x05050505;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xE0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x050005cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000005;
	dsi_set_cmdq(data_array, 5, 1);


	data_array[0] = 0xf0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000c2902;
	data_array[1] = 0xffffffcb;
	data_array[2] = 0xffffffff;
	data_array[3] = 0xffffffff;
	dsi_set_cmdq(data_array, 4, 1);

	data_array[0] = 0x80002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x0a2a29cc;
	data_array[2] = 0x12100e0c;
	data_array[3] = 0x08000614;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0x90002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x000000cc;
	data_array[2] = 0x00000200;
	data_array[3] = 0x0b092a29;
	data_array[4] = 0x13110f0d;
	dsi_set_cmdq(data_array, 5, 1);

	

	data_array[0] = 0xa0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x070005cc;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000001;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xb0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x132a29cc;
	data_array[2] = 0x0b0d0f11;
	data_array[3] = 0x07000109;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xc0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00102902;
	data_array[1] = 0x000000cc;
	data_array[2] = 0x00000500;
	data_array[3] = 0x12142a29;
	data_array[4] = 0x0a0c0e10;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xd0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x080002cc;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000006;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0x80002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000D2902;
	data_array[1] = 0x000589ce;
	data_array[2] = 0x00000588;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0x90002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x00fc54ce;
	data_array[2] = 0x5500fd54;
	data_array[3] = 0x01550000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xA0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x050758ce;
	data_array[2] = 0x00000009;
	data_array[3] = 0x0a050658;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xB0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x050558ce;
	data_array[2] = 0x0000000b;
	data_array[3] = 0x0c050458;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xc0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x050358ce;
	data_array[2] = 0x0000000d;
	data_array[3] = 0x0e050258;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xd0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x050158ce;
	data_array[2] = 0x0000000f;
	data_array[3] = 0x10050058;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0x80002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x050050cf;
	data_array[2] = 0x00000011;
	data_array[3] = 0x12050150;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0x90002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x050250cf;
	data_array[2] = 0x00000013;
	data_array[3] = 0x14050350;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xa0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x000000cf;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xb0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000f2902;
	data_array[1] = 0x000000cf;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);

	data_array[0] = 0xc0002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x000c2902;
	data_array[1] = 0x203939cf;
	data_array[2] = 0x01000020;
	data_array[3] = 0x00002001;
	dsi_set_cmdq(data_array, 4, 1);

	data_array[0] = 0xb5002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00072902;
	data_array[1] = 0xff950bc5;
	data_array[2] = 0x00ff950b;
	dsi_set_cmdq(data_array, 3, 1);

//gamma 2.2+
	data_array[0] = 0x00002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00152902;
	data_array[1] = 0x302511e1;
	data_array[2] = 0x59584b3c;
	data_array[3] = 0x7b887081;
	data_array[4] = 0x51547968;
	data_array[5] = 0x1f263543;
	data_array[6] = 0x00000008;
	dsi_set_cmdq(data_array, 7, 1);

//gamma2.2-
	data_array[0] = 0x00002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00152902;
	data_array[1] = 0x302511e2;
	data_array[2] = 0x58584b3c;
	data_array[3] = 0x7c887081;
	data_array[4] = 0x51547967;
	data_array[5] = 0x1f263543;
	data_array[6] = 0x00000008;
	dsi_set_cmdq(data_array, 7, 1);

	//data_array[0] = 0xa0002300;
	//dsi_set_cmdq(&data_array, 1, 1);
	//data_array[0] = 0x00022902;
	//data_array[1] = 0x0000c2c1;
	//dsi_set_cmdq(&data_array, 2, 1);

	data_array[0] = 0x00352300;
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00002300;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00042902;
	data_array[1] = 0xffffffff;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(150);

	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
	
}

/*
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/

static struct LCM_setting_table lcm_sleep_out_setting[] = {

    // Sleep Out
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 10, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 0, {0x00}},
	{REGFLAG_DELAY, 100, {}},

    // Sleep Mode On
	{0x10, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


//int vcom=0xa7;
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
			//case 0xd9 :
			//	table[i].para_list[0]=vcom;
			//	dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
			//	vcom-=2;
            //    break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		params->physical_width = 63;
		params->physical_height = 111;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED; //LCM_DBI_TE_MODE_VSYNC_ONLY;
//		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
		
		params->dsi.mode   = SYNC_PULSE_VDO_MODE; //BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
	
		// DSI
		/* Command mode setting */
		//1 Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_THREE_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=720*3;
		params->dsi.vertical_active_line=1280;	

		
		params->dsi.vertical_sync_active				= 10; //10;// 3    2
		params->dsi.vertical_backporch					= 8; //10; //44;// 20   1
		params->dsi.vertical_frontporch					= 32;//16; //10; //44; // 1  12
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;// 50  2
		params->dsi.horizontal_backporch				= 62;
		params->dsi.horizontal_frontporch				= 62;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	    //params->dsi.LPX=8; 

		 params->dsi.PLL_CLOCK=276;   //272 modify by taoxiaodong for sheping 
                params->dsi.ssc_range=8;
                params->dsi.ssc_disable=1;


		// Bit rate calculation
		//1 Every lane speed
		//params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		//params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4	
		//params->dsi.fbk_div =17;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(20); 
	SET_RESET_PIN(0);
	MDELAY(20); 
	
	SET_RESET_PIN(1);
	MDELAY(200);

	lcm_init_registers();

	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}



static void lcm_suspend(void)
{
/*
       unsigned int data_array[16];

       data_array[0]=0x00280500; // Display Off^M
       dsi_set_cmdq(data_array, 1, 1);
               
       data_array[0] = 0x00100500; // Sleep In^M
       dsi_set_cmdq(data_array, 1, 1);

       SET_RESET_PIN(1);
       SET_RESET_PIN(0);
       MDELAY(1);
       SET_RESET_PIN(1);
*/
	SET_RESET_PIN(1);
	MDELAY(10); 
	SET_RESET_PIN(0);
	MDELAY(10); 
	
	SET_RESET_PIN(1);
	MDELAY(150);

//push_table(lcm_sleep_mode_in_setting, sizeof(lcm_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_resume(void)
{
/*
   //1 do lcm init again to solve some display issue^M
       SET_RESET_PIN(1);
       SET_RESET_PIN(0);
       MDELAY(1);

       SET_RESET_PIN(1);
       MDELAY(20);

       init_lcm_registers();
*/
		lcm_init_registers();

//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}



static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}




static unsigned int lcm_compare_id(void)
{
	unsigned int id0,id1,id=0;
        unsigned char buffer[5];
        unsigned int array[16];

        SET_RESET_PIN(1);
        SET_RESET_PIN(0);
        MDELAY(1);

        SET_RESET_PIN(1);
        MDELAY(20);

        array[0] = 0x00053700;          // read id return two byte,version and id
        dsi_set_cmdq(array, 1, 1);

        read_reg_v2(0xA1, buffer, 5);   //018B1283ff
        id0 = buffer[2];
        id1 = buffer[3];
        id=(id0<<8)|id1;

    #ifdef BUILD_LK
                printf("%s,LK TCL_OTM1287A id = 0x%08x\n", __func__, id);
    #else
                printk(KERN_ERR "%s,LK TCL_OTM1287A id = 0x%08x\n", __func__, id);   //0x1287a
    #endif

    if(id == LCM_ID_OTM1287)
    {	
		int data[4] = {0,0,0,0};
		int res = 0;
		int rawdata = 0;
		int lcm_vol = 0;

	return 1;

		res = 123;//IMM_GetOneChannelValue(0, data, &rawdata);
		if(res < 0) {
		#ifdef BUILD_LK
			printf("[adc_uboot]:boyi_BOE res= %d\n",res);
		#else
			printk("[adc_kernel]:boyi_BOE res= %d\n",res);
		#endif
			return 1;
		}
		lcm_vol = data[0] * 1000 + data[1] * 10;



		if(lcm_vol < MIN_VOLTAGE) {
		#ifdef BUILD_LK
			printf("[adc_uboot]:boyi_BOE lcm_vol= %d\n",lcm_vol);
		#else
			printk("[adc_kernel]:boyi_BOE lcm_vol= %d\n",lcm_vol);
		#endif
			return 1;
		}
		else
		{
			return 1;
		}
    }
    else
        return 0;


}



LCM_DRIVER zaw806_otm1287a_hd720_dsi_vdo_lcm_drv = 
{
        .name			= "zaw806_otm1287a_hd720_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,	
#if (LCM_DSI_CMD_MODE)
	//.set_backlight	= lcm_setbacklight,
     //  .update         = lcm_update,
#endif
    };
