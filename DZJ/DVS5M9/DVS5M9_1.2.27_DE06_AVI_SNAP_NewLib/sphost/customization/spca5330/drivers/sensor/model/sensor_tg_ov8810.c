/**************************************************************************
 *                                                                        *
 *       Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.          *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan                                                      *
 *                                                                        *
 *  Author: Vincent Tsao                                                    *
 *                                                                        *
 *  Version: 0.01, 2011.5.9:
 *  Disable the auto AWB control of ISP in capture mode. *
 *  (Auto AWB control of ISP is both disable in preview and captrue mode now.) *
 *
 *  Version: 0.02, 2011.5.10: 
 *  (1)Disable the group enable capability in capture mode when setting exposure index.
 *       (Just only use the group enable capability in preview mode because it is needed
 *       in setting dummy line.)
 *  (2)Fine tune the signal Vfall position to 20, Vrise position to 220.
 *      (Applied to preview, capture and 720P mode.)
 *
 *  Version: 0.03, 2011.5.19: 
 *  (1)Update the capture mode micro code to change the burst frame rate from 5 to 10.
 *      (Change the pixel clock of capture mode from 48MHz to 96MHz)
 *  (2)Remove the dummy field of capture mode to speed up the capture timing.
 *  (3)Modify the preview mode config. 
 *      VGA mode -> size 816*612, 60fps. (pixel clock 96MHz)
 *      XGA mode -> size 1632*1224, 30fps. (pixel clock 96MHz)
 *      720P  mode -> size 1280*720, 60fps. (pixel clock 96MHz)
 *
 *  Version: 0.04, 2011.5.23:
 *  (1)Modify the preview mode configuration.
 *      VGA mode -> size 1632*1224, 30fps. (pixel clock 96MHz) 
 *      XGA mode -> size 816*612, 60fps. (pixel clock 96MHz)
 *
 *  Version: 0.05, 2011.5.26:
 *  (1)Update the micro code settings of initialization , preview mode (1632*1224, 816*612, 720P) 
 *      and capture mode.  Only load the key settings for resolution exchange.
 *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_ov8810-0.00.00"

/*HISTORY
 * 0.00.01 20090626 cslin
 *   first draft
 *
 */

#include "common.h"
#include "../flow/sysregs.h"
#include "../flow/hal_front.h"
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
#include "../sensor_util.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include <stdio.h>
#include "middleware/proflog.h"
/*#include "../../../../include/hal/hal_glob.h"*/
#include "../flow/sensor_model_snap.h"
#include "../include/customization/solution_def.h"


/*#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)*/

#define MASK_EXP_CMD 0

#define OV8810_AUTO_EXP_GAIN_CTRL   0

#define SNAP_EARLY0_NEEDED  0

#define _OV_EXP_DUMMY_LINE_SETTING_ORDER_  1

#define _Snap_ExpTime_Use_FrameLength_Adjustment_  1

#define CaptureDefFrameLen   2484

/*void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);*/

/* Sensor image size definition here */
#define NO             0
#define BY_PIXEL       1
#define BY_HD          0
#define NOUSE          0
#define DISABLE        0xFFFF

#if 1
/*#define SENSOR_FREX     BIT6*/       /* for A900 */
#define SENSOR_FREX     (1<<15)           /* for EVB */
#define SENSOR_PWRDN    (1<<14)
#define SENSOR_RESET    (1<<12)

#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define SENOR_GPIO_GRP SP5K_GPIO_GRP_LMI
#endif


/*****TODO***multiple preview mode for VGA*/

/* For  preview size 1632*1224, 30fps */
#define PREVIEW_VGA_HSIZE  1632  /* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  1224  /* preview vertical size*/

/*For preview size, 816*612, 60fps*/
#define PREVIEW_XGA_HSIZE  816    /* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  612    /* preview vertical size*/

/* For  preview size 1280*720, 60fps, cropping */
#define PREVIEW_720P_HSIZE  1280  /* preview horiz. size*/
#define PREVIEW_720P_VSIZE    720  /* preview vertical size*/

#define CAPTURE_HSIZE  (3264)		/* capture horiz. size*/
#define CAPTURE_VSIZE  (2448)		/* capture vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/

/* For  preview size 1632*1224, 30fps */
#define PREVIEW_VGA_LINETOTAL   (1632*2)	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  1225	    /* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  1225		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     266/**/     /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     (1)    /* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         65 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         265	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  BY_PIXEL	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         20 /*12*//*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE        220 /*13*/ /*7*/	/* preview Vd reshap rise*/

/*For preview size, 816*612, 60fps*/
#define PREVIEW_XGA_LINETOTAL   (816*2)	/* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL  613	/* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  613	/* preview Vd sync length*/
#define PREVIEW_XGA_HOFFSET     266   	/* preview Hd valid pixel data offset position*/
#define PREVIEW_XGA_VOFFSET     1		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL          65 	/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE         265	/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  BY_PIXEL	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL          20 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE         220 /*7*/	/* preview Vd reshap rise*/

/*For preview size, 1280*720, 60fps, cropping*/
#define PREVIEW_720P_LINETOTAL   (1280*2)	/* preview Hd length*/
#define PREVIEW_720P_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_720P_FRAMETOTAL  721	/* preview Vd length*/
#define PREVIEW_720P_FRAMEBLANK  721	/* preview Vd sync length*/
#define PREVIEW_720P_HOFFSET     266   	/* preview Hd valid pixel data offset position*/
#define PREVIEW_720P_VOFFSET     1		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_720P_HRESHAPE_FALL          65 	/* preview Hd reshap fall*/
#define PREVIEW_720P_HRESHAPE_RISE         265	/* preview Hd reshap rise*/
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL  BY_PIXEL	/* preview Vd reshap control*/
#define PREVIEW_720P_VRESHAPE_FALL          20 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_720P_VRESHAPE_RISE         220 /*7*/	/* preview Vd reshap rise*/
                                
/* Capture default definition here */
#define CAPTURE_LINETOTAL   (3264*2)	/* capture Hd length*/
#define CAPTURE_LINEBLANK   6			/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  2449		/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  2449		/* mipi use > vsize is more safe*/

#define CAPTURE_HOFFSET     (500)/*256*/    /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     (1)    		/* capture Vd valid line data offset position*/
/*#define CAPTURE_VOFFSET     (11+4) */	/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL        300	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE        400	/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_PIXEL /* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL		   20	/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_VRESHAPE_RISE		 220/* capture Vd reshap rise , count by pixel*/ 


/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV     1        /* mclk drivng, '1' : 8mA, '0' : 4mA */
#define MCLKMODE	1	     /*mclk mode,'0'from I/O Pad,'1'from internal*/
#define PCLKMODE	2	     /*pclk mode,'0'from I/O Pad,'1'from internal*/
#define TGPLLEN   	1	     /*enable TGPLL,'0'disable,'1'enable*/
#define TGPLLCLKSET	8	     /*TGPLL clk 0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,
					5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/
#define MCLKDIV		4	     /*MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV		2	     /*pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE	0     	     /*master clk phase*/
#define PIXELPHASE	3	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/

#define	PCLKNXSEL_INT	0x00
#define	PCLKNXSEL_LVDS	0x01
#define	PCLKNXSEL_EXT	0x02

#define	PCLK1XSEL_NX	0x00
#define	PCLK1XSEL_NX_2	0x04
#define	PCLK1XSEL_INT	0x08


#define PREVIEW_VGA_PCLK        48000
#define PREVIEW_XGA_PCLK        54000
#define CAPTURE_PCLK            24000

#define RGBADJ		0	     /*RGB input data sample phase adj "1"negedge "0"posedge*/

#if PCLKMODE
#define PCLK_SRC    0        /* '0': from master clock, '1' from LVDS, '2' from external , '3' from external divided by 2 */
#else
#define PCLK_SRC    2
#endif
/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_XGA_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_VGA_CFAPATTERN   BAYER_BGGR
#define PREVIEW_720P_CFAPATTERN  BAYER_BGGR

#define	SNAP_CFAPATTERN		BAYER_BGGR       /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	1	            /*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		        1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM	  1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/
#define	CAPTURE_HDNUM_FRAME_START  110	/*the front interrupt line number of the snap new frame */

#define	PREVIEW_HDNUM_FRAME_START  110		/*the front interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_END	 2000 /*1970*/    	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	2000/*3250*//*3281*//*1971*/    	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END	400		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT	400		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_FRAME_END	400		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_CMDPORT	300		/*3281*//*976*/		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_FRAME_END	400		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_CMDPORT	400		/*the command port interrupt line number of the preview new frame*/

#define	EXP_FRAME_NUM       	2	    /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/
/**************create the command port condition  define ****************************/
/*UINT8 port_id is dynamic allocated by sensor_serial.c::sensorSerialCreate()
 * stored in sensor{Tg/Afe}Cfg.cmdportid_{xtg,exp,afe,agc,dcoffset}
 */
/*structure of physical definitions
 * mode:[0]H spi/L ssc
 *   ssc [7:1]slave addr
 *   spi [7]sensel;[6]sengpio;[5:4]spi type;[3:1]cdsini;
 *     ([7:6]==LL:sensel1,HL:sensel2,LH:sengpio,HH:no sen pin)
 * ssc
 *   format2:[1:0]readCtr(no [4] go)
 *   format3:(stopdelay and clkien??)
 * spi
 *   format2:sensel gpio [7:4]sensel H/L(idle,open,active,close);[3:0]gpio_grp
 *   format3:[5:0]sensel gpio gpio_pin
 */
#define	TG_SPI_I2C_MODE  0x6c     	       /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#if 0/*SPCA5210*/
#define	TG_SPEED         FRONT_SSC_CLK_192K
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */

#define CAP_FRAME_MODE  0        /* Frame mode or Rolling mode */


/*prototypes of sensor model methods*/

static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreviewXGABegin();
static void sensorDoFuncPreview720PBegin();

static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();

#if ( SNAP_EARLY0_NEEDED == 1 )
static void sensorDoFuncSnapEarly0();
#endif
static void sensorDoFuncSnapEarly1();

static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();

/*no definition */
/*#define sp5kSnapFlashCfgGet(a) 0*/

static const UINT8 afe_init_code_SWReset[][3]={
{0x30,0x12,0x80}
};

/* [Jim] 
#define OV8810_USE_25FPS */

/* OV8M setting define */
static const UINT8 afe_init_code[][3]={
/*preview size 1632*1224*/
/*{0x30,0x12,0x80},*/
{0x36,0x0b,0x0c},
{0x31,0x00,0x06},
{0x30,0xfa,0x00},
{0x30,0x0f,0x04},
{0x30,0x0e,0x05},
{0x30,0x10,0x28},
{0x30,0x11,0x22},
{0x30,0x00,0x30},
{0x30,0x02,0x04},		/* 0x3002, 0x3003 = 0x4B2 (1202) Coarse exp. time */
{0x30,0x03,0xb2},
{0x33,0x02,0x20},
{0x30,0xb2,0x10},
{0x30,0xa0,0x40},
{0x30,0x98,0x24},
{0x30,0x99,0x81},
{0x30,0x9a,0x64},
{0x30,0x9b,0x00},
{0x30,0x9d,0x64},
{0x30,0x9e,0x2d},
{0x33,0x21,0x02},
{0x33,0x22,0x04},
{0x33,0x28,0x40},
{0x33,0x29,0xe3},
{0x33,0x06,0x00},
{0x33,0x16,0x03},
{0x30,0x79,0x0a},
{0x30,0x68,0x08},
{0x30,0x58,0x01},
{0x30,0x59,0xa0},
{0x30,0x6b,0x00},
{0x30,0x65,0x50},
{0x30,0x67,0x40},
{0x30,0x69,0x80},
{0x30,0x71,0x40},
{0x33,0x00,0xef},
{0x33,0x34,0x02},
{0x33,0x31,0x80},
{0x33,0x32,0x80},
{0x33,0x33,0x41},
{0x30,0x91,0x00},
{0x30,0x06,0x00},
{0x30,0x82,0x80},
{0x33,0x1e,0x94},
{0x33,0x1f,0x6e},
{0x30,0x94,0x01},
{0x30,0xab,0x44},
{0x30,0x95,0x0a},
{0x30,0x8d,0x00},
{0x30,0x82,0x00},
{0x30,0x80,0x40},
{0x30,0xaa,0x59},
{0x30,0xa9,0x00},
{0x30,0xbe,0x08},
{0x30,0x9f,0x23},
{0x30,0x65,0x40},
{0x30,0x68,0x00},
{0x30,0xbf,0x80},
{0x30,0x9c,0x00},
{0x30,0x84,0x44},
{0x30,0x16,0x03},
{0x30,0xe9,0x09},
{0x30,0x75,0x29},
{0x30,0x76,0x29},
{0x30,0x77,0x29},
{0x30,0x78,0x29},
{0x30,0x6a,0x05},
{0x30,0x00,0x00},
{0x30,0x15,0x33},
{0x30,0x18,0x44},
{0x30,0x19,0x38},
{0x30,0x1a,0x82},
{0x30,0x13,0xc0},
{0x33,0x20,0xc2},
{0x33,0x3e,0x00},
{0x30,0x00,0x1f},
{0x30,0x87,0x41},
{0x30,0x9e,0x1b},
{0x30,0xe3,0x0e},
{0x30,0xf2,0x00},
{0x30,0xf4,0x90},
{0x30,0x9e,0x09},
{0x30,0x71,0x40},
{0x33,0x47,0x00},
{0x30,0x71,0x40},
{0x33,0x47,0x00},
{0x30,0xF3,0xA7},
{0x30,0x8d,0x02},
#ifdef OV8810_USE_25FPS
{0x30,0x20,0x5 },		/* 0x3020, 0x3021 = 0x5EF (1519) frame length lines 5f4=1524 */
{0x30,0x21,0xf4},
#else
{0x30,0x20,0x4 },		/* 0x3020, 0x3021 = 0x4F4 (1268) frame length lines */
{0x30,0x21,0xf4},
#endif
{0x30,0x22,0x9 },		/* 0x3022, 0x3023 = 0x9E0 (2528) line length pck */
{0x30,0x23,0xe0},
{0x30,0x24,0x0 },
{0x30,0x25,0x0 },
{0x30,0x26,0x0 },
{0x30,0x27,0x0 },
{0x30,0x28,0xc },
{0x30,0x29,0xdf},
{0x30,0x2a,0x9 },
{0x30,0x2b,0x9f},
{0x30,0x2c,0x6 },
{0x30,0x2d,0x60},
{0x30,0x2e,0x4 },
{0x30,0x2f,0xc8},
{0x30,0x72,0xd },
{0x30,0x7e,0x0 },
{0x30,0x8a,0x2 },
{0x30,0x90,0x99},
{0x30,0x92,0x0 },
{0x30,0xb3,0x8 },
{0x30,0xf0,0x10},
{0x30,0xf1,0x56},
{0x30,0xfb,0x8e},
{0x30,0xf8,0x45},
{0x33,0x01,0xb },
{0x33,0x19,0x4 },
{0x33,0x1c,0x0 },
{0x33,0x1d,0x0 },
{0x33,0xe5,0x0 },
{0x31,0x00,0x88},
{0x31,0x01,0x77},
{0x36,0x01,0x16},
{0x30,0x0f,0x14},
{0x30,0x00,0x30},
{0x30,0x02,0x04},		/* 0x3002, 0x3003 = 0x4A0 (1184) Coarse exp. time */
{0x30,0x03,0xa0},
{0x30,0xfa,0x01}
};


static const UINT8 afe_vga_code[][3]={
/*preview size 1632*1224*/
#ifdef OV8810_USE_25FPS
{0x30,0x20,0x5 },		/* 0x3020, 0x3021 = 0x5EF (1519) frame length lines, 5f4=1524 */
{0x30,0x21,0xf4},
#else
{0x30,0x20,0x4 },		/* 0x3020, 0x3021 = 0x4F4 (1268) frame length lines */
{0x30,0x21,0xf4},
#endif
{0x30,0x22,0x9 },		/* 0x3022, 0x3023 = 0x9E0 (2528) line length pck */
{0x30,0x23,0xe0},
{0x30,0x24,0x0 },
{0x30,0x25,0x0 },
{0x30,0x26,0x0 },
{0x30,0x27,0x0 },
{0x30,0x28,0xc },
{0x30,0x29,0xdf},
{0x30,0x2a,0x9 },
{0x30,0x2b,0x9f},
{0x30,0x2c,0x6 },
{0x30,0x2d,0x60},
{0x30,0x2e,0x4 },
{0x30,0x2f,0xc8},
{0x30,0x72,0xd },
{0x30,0x7e,0x0 },
{0x30,0x8a,0x2 },
{0x30,0x90,0x99},
{0x30,0x92,0x0 },
{0x30,0xb3,0x8 },
{0x30,0xf0,0x10},
{0x30,0xf1,0x56},
{0x30,0xfb,0x8e},
{0x30,0xf8,0x45},
{0x33,0x01,0xb },
{0x33,0x19,0x4 },
{0x33,0x1c,0x0 },
{0x33,0x1d,0x0 },
{0x33,0xe5,0x0 },
{0x31,0x00,0x88},
{0x31,0x01,0x77},
{0x36,0x01,0x16},
{0x30,0x00,0x30},
{0x30,0x0f,0x14}
};

static const UINT8 afe_still_code[][3]={

/* Capture mode 10fps micro code*/
{0x30,0x20,0x9 },		/* 0x3020, 0x3021 = 0x9B4 (2484) frame length lines */
{0x30,0x21,0xb4},
{0x30,0x22,0xf },		/* 0x3022, 0x3023 = 0xF78 (3960) line length pck */
{0x30,0x23,0x78},
{0x30,0x24,0x0 },
{0x30,0x25,0x0 },
{0x30,0x26,0x0 },
{0x30,0x27,0x0 },
{0x30,0x28,0xc },
{0x30,0x29,0xdf},
{0x30,0x2a,0x9 },
{0x30,0x2b,0x9f},
{0x30,0x2c,0xc },
{0x30,0x2d,0xc0},
{0x30,0x2e,0x9 },
{0x30,0x2f,0x90},

/*Add to reset the dummy line*/
{0x30,0x1e,0x00},		/* 0x301E, 0x301F = Dummy lines */
{0x30,0x1f,0x00},
/*Add to reset the dummy line*/

{0x30,0x72,0x1 },
{0x30,0x7e,0x0 },
{0x30,0x8a,0x1 },
{0x30,0x90,0x99},
{0x30,0x92,0x0 },
{0x30,0xb3,0x8 },
{0x30,0xf0,0x10},
{0x30,0xf1,0x56},
{0x30,0xfb,0x8e},
{0x30,0xf8,0x40},
{0x33,0x01,0xb },
{0x33,0x19,0x8 },
{0x33,0x1c,0x28},
{0x33,0x1d,0x21},
{0x33,0xe5,0x0 },
{0x31,0x00,0x88},
{0x31,0x01,0x77},
{0x36,0x01,0x16},
{0x30,0x00,0x1f},
{0x30,0x0f,0x14}
};

static const UINT8 afe_xga_code[][3]={
/*preview size 816*612*/	
{0x30,0x20,0x2 },
{0x30,0x21,0xaa},
{0x30,0x22,0x9 },
{0x30,0x23,0x28},
{0x30,0x24,0x0 },
{0x30,0x25,0x0 },
{0x30,0x26,0x0 },
{0x30,0x27,0x0 },
{0x30,0x28,0xc },
{0x30,0x29,0xdf},
{0x30,0x2a,0x9 },
{0x30,0x2b,0x9f},
{0x30,0x2c,0x3 },
{0x30,0x2d,0x30},
{0x30,0x2e,0x2 },
{0x30,0x2f,0x64},
{0x30,0x72,0xd },
{0x30,0x7e,0x0 },
{0x30,0x8a,0x1 },
{0x30,0x90,0x97},
{0x30,0x92,0x8 },
{0x30,0xb3,0x9 },
{0x30,0xf0,0x0 },
{0x30,0xf1,0x46},
{0x30,0xfb,0xc9},
{0x30,0xf8,0x4a},
{0x33,0x01,0xf },
{0x33,0x19,0x2 },
{0x33,0x1c,0x0 },
{0x33,0x1d,0x0 },
{0x33,0xe5,0x1 },
{0x31,0x00,0x88},
{0x31,0x01,0x77},
{0x36,0x01,0x16},
{0x30,0x00,0x30},
{0x30,0x0f,0x24}
};	

static const UINT8 afe_720P_code[][3]={
/*preview size 1280*720*/
{0x30,0x20,0x2 },
{0x30,0x21,0xfc},
{0x30,0x22,0x8 },
{0x30,0x23,0x30},
{0x30,0x24,0x1 },
{0x30,0x25,0x64},
{0x30,0x26,0x1 },
{0x30,0x27,0xf4},
{0x30,0x28,0xb },
{0x30,0x29,0x7b},
{0x30,0x2a,0x7 },
{0x30,0x2b,0xab},
{0x30,0x2c,0x5 },
{0x30,0x2d,0x0 },
{0x30,0x2e,0x2 },
{0x30,0x2f,0xd0},
{0x30,0x72,0xd },
{0x30,0x7e,0x3 },
{0x30,0x8a,0x2 },
{0x30,0x90,0x99},
{0x30,0x92,0x0 },
{0x30,0xb3,0x8 },
{0x30,0xf0,0x10},
{0x30,0xf1,0x56},
{0x30,0xfb,0x8e},
{0x30,0xf8,0x45},
{0x33,0x01,0xb },
{0x33,0x19,0x4 },
{0x33,0x1c,0x0 },
{0x33,0x1d,0x0 },
{0x33,0xe5,0x0 },
{0x31,0x00,0x88},
{0x31,0x01,0x77},
{0x36,0x01,0x16},
{0x30,0x00,0x18},
{0x30,0x0f,0x14}
};

#if SPCA5330
/*5330 definition smaples.*/
/*
static const sensorLVDSCfg_t sensorLVDSCfg = 
{
	.wordcntsel 	 	=0x00, 				 
	.Lvdsmode		=0x00, 				 
	.Lvdslanenum	 	=0x00, 				 
	.Lvdslanemode	=0x00, 				 
	.Lvdslaneswap	 	=0x00, 				 
	.Lvdsraw		 	=0x00, 				 
	.Lvdsoutsel 	 	=0x00, 				 
	.Lvdseofsel 	 	=0x00, 				 
	.Lvdseolsel 	 	=0x00, 				 
	.vmode			=0x00, 				 
	.lvdsvdsrc		=0x00, 				 
	.lvds_dphy_sel	=0x00, 				 
	.S2pphase		=0x00, 				 
	.tsettle		 	=0x00, 				 
	.teot				=0x00, 				 
	.r_con			=0x00, 				 
	.c_con			=0x00, 				 
	.Ctrlword_sol	 	=0x00, 				 
	.Ctrlword_sof	 	=0x00, 				 
	.Ctrlword_eol	 	=0x00, 				 
	.Ctrlword_eof	 	=0x00, 				 
	.syncwrod0		=0x00, 				 
	.syncwrod1		=0x00, 				 
	.syncwrod2		=0x00, 				 
	.Exthd_num		=0x00, 				 
	.Exthd_pol		=0x00, 				 
	.Exthd_mode 	 	=0x00, 				 
}

*/

/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg = 
{

	.mipi_nockrsten  =0 ,
	.wordcntsel 	 =0 ,
	.Lvdsmode		 =0 ,
	.Lvdslanenum	 =1 ,/*2 lane mode*/
	.Lvdslanemode	 =0 ,
	.Lvdslaneswap	 =1 ,
	.Lvdsraw		 =0 ,
	.Lvdsoutsel 	 =0 ,
	.Lvdseofsel 	 =1 ,
	.Lvdseolsel 	 =0 ,
	.vmode			 =0 ,
	.lvdsvdsrc		 =1 ,
	.lvds_dphy_sel	 =1 ,
	.S2pphase		 =0 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	.Ctrlword_sol	 =0x2B,
	.Ctrlword_sof	 =0x00 ,
	.syncwrod0		 =0xB8 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	.Exthd_num		 =7 ,
	.Exthd_pol		 =0 ,
	.Exthd_mode 	 =0 
};
#else

/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg = 
{
	.mipi_nockrsten  = 0,
	.wordcntsel 	 = 0,
	.Lvdsmode		 = 0,
	.vmode			 = 0,
	.lvdsvdsrc		 = 1,
	.lvds_dphy_sel	 = 1,
	.Ctrlword_sol	 = 0x2b,
	.S2pphase		 = 0,
	.tsettle		 = 3,
	.teot			 = 7,
	.r_con			 = 0,
	.c_con			 = 0,
	.Ctrlword_sof	 = 0x00,
	.syncwrod0		 = 0xB8,
	.syncwrod1		 = 0x00,
	.syncwrod2		 = 0x00,
	.HDextend		 = 0x00	
};

#endif




/*sensor cfg*/
static const sensorTgPreviewCfg_t sensorTgPreviewVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGABegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_VGA_HSIZE,
	.vsize = PREVIEW_VGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
};

static const sensorTgPreviewCfg_t sensorTgPreviewXGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewXGABegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_XGA_HSIZE,
	.vsize = PREVIEW_XGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_XGA_CFAPATTERN,
};

static const sensorTgPreviewCfg_t sensorTgPreview720PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_720P_HSIZE,
	.vsize = PREVIEW_720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_720P_CFAPATTERN,
};

/*static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
&sensorTgPreviewVGACfg,&sensorTgPreviewXGACfg,&sensorTgPreview720PCfg, 	  
};*/

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
&sensorTgPreviewVGACfg,&sensorTgPreviewXGACfg, 	  
};

static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
				#if ( SNAP_EARLY0_NEEDED == 1 )
				sensorDoFuncSnapEarly0,
				#endif
				sensorDoFuncSnapEarly1, 	
			},
			.snapExpBegin = sensorDoFuncSnapExpBegin,
			.snapBshutterBegin = sensorDoFuncSnapBshutterBegin,
			.snapBshutterFollow = sensorDoFuncSnapBshutterFollow,
			.snapBshutterEnd = sensorDoFuncSnapBshutterEnd,
			.snapLongExpBegin = NULL,
			.snapLongExpFollow = NULL,
			.snapLongExpEnd = NULL,
			.snapShortExp = sensorDoFuncSnapShortExp,
			.snapExpEnd = sensorDoFuncSnapExpEnd,
			.psnapDummy = NULL,
			.psnapReadoutField = (sensorDoFunc_t []) {
				sensorDoFuncSnapReadoutField0,
				/* sensorDoFuncSnapReadoutField1,
				sensorDoFuncSnapReadoutField2, */
			},
			.snapFieldEnd = sensorDoFuncSnapFieldEnd,
			.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
			.snapNext = NULL,		
		},
		.crop = NULL, /****TODO***/
		.hsize = CAPTURE_HSIZE,
		.vsize = CAPTURE_VSIZE,
		.sensor_model = SENSORMODEL,
		.cfa_pattern = SNAP_CFAPATTERN,
		.n_dummy = SNAP_DUMMYVD_NUM,
		.n_readout = SNAP_READOUTVD_NUM,
		.n_capvd = SNAP_CAPTUREVD_NUM,
		.n_snap_early = SNAP_EARLY_NUM,
		.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
		.fldsel_start = FLDSEL_START_NUM,
		.fldsel_end = FLDSEL_END_NUM,
	};
	

/****TODO****frame snap mode*/
static  sensorTgSnapCfg_t sensorTgSnapFrameCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			#if ( SNAP_EARLY0_NEEDED == 1 )
			sensorDoFuncSnapEarly0,
			#endif
			sensorDoFuncSnapEarly1, 	
		},
		.snapExpBegin = sensorDoFuncSnapExpBegin,
		.snapBshutterBegin = sensorDoFuncSnapBshutterBegin,
		.snapBshutterFollow = sensorDoFuncSnapBshutterFollow,
		.snapBshutterEnd = sensorDoFuncSnapBshutterEnd,
		.snapLongExpBegin = NULL,
		.snapLongExpFollow = NULL,
		.snapLongExpEnd = NULL,
		.snapShortExp = sensorDoFuncSnapShortExp,
		.snapExpEnd = sensorDoFuncSnapExpEnd,
		.psnapDummy = NULL,
		.psnapReadoutField = (sensorDoFunc_t []) {
			sensorDoFuncSnapReadoutField0,
			/* sensorDoFuncSnapReadoutField1,
			sensorDoFuncSnapReadoutField2, */
		},
		.snapFieldEnd = sensorDoFuncSnapFieldEnd,
		.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
		.snapNext = NULL,		
	},
	.crop = NULL, /****TODO***/
	.hsize = CAPTURE_HSIZE,
	.vsize = CAPTURE_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = SNAP_CFAPATTERN,
	.n_dummy = SNAP_DUMMYVD_NUM,
	.n_readout = SNAP_READOUTVD_NUM,
	.n_capvd = SNAP_CAPTUREVD_NUM,
	.n_snap_early = SNAP_FRAME_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_FRAME_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg,&sensorTgSnapFrameCfg, 
	
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVGACfg = {
	.linetotal       = PREVIEW_VGA_LINETOTAL,
	.lineblank       = PREVIEW_VGA_LINEBLANK,
	.frametotal      = PREVIEW_VGA_FRAMETOTAL,
	.frameblank      = PREVIEW_VGA_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_VGA_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_VGA_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_VGA_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_VGA_VRESHAPE_RISE,
	.hoffset         = PREVIEW_VGA_HOFFSET,
	.voffset         = PREVIEW_VGA_VOFFSET,
	.hsize           = PREVIEW_VGA_HSIZE,
	.vsize           = PREVIEW_VGA_VSIZE,
	.lastlinepix     = PREVIEW_VGA_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_VGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_VGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewXGACfg = {
	.linetotal       = PREVIEW_XGA_LINETOTAL,
	.lineblank       = PREVIEW_XGA_LINEBLANK,
	.frametotal      = PREVIEW_XGA_FRAMETOTAL,
	.frameblank      = PREVIEW_XGA_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_XGA_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_XGA_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_XGA_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_XGA_VRESHAPE_RISE,
	.hoffset         = PREVIEW_XGA_HOFFSET,
	.voffset         = PREVIEW_XGA_VOFFSET,
	.hsize           = PREVIEW_XGA_HSIZE,
	.vsize           = PREVIEW_XGA_VSIZE,
	.lastlinepix     = PREVIEW_XGA_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_XGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_XGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_XGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview720PCfg = {
	.linetotal       = PREVIEW_720P_LINETOTAL,
	.lineblank       = PREVIEW_720P_LINEBLANK,
	.frametotal      = PREVIEW_720P_FRAMETOTAL,
	.frameblank      = PREVIEW_720P_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_720P_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_720P_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_720P_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_720P_VRESHAPE_RISE,
	.hoffset         = PREVIEW_720P_HOFFSET,
	.voffset         = PREVIEW_720P_VOFFSET,
	.hsize           = PREVIEW_720P_HSIZE,
	.vsize           = PREVIEW_720P_VSIZE,
	.lastlinepix     = PREVIEW_720P_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_720P_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_720P_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
};

static  sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
	.linetotal       = CAPTURE_LINETOTAL,
	.lineblank       = CAPTURE_LINEBLANK,
	.frametotal      = CAPTURE_FRAMETOTAL,
	.frameblank      = CAPTURE_FRAMEBLANK,
	.hreshape_fall   = CAPTURE_HRESHAPE_FALL,
	.hreshape_rise   = CAPTURE_HRESHAPE_RISE,
	.vreshape_fall   = CAPTURE_VRESHAPE_FALL,
	.vreshape_rise   = CAPTURE_VRESHAPE_RISE,
	.hoffset         = CAPTURE_HOFFSET,
	.voffset         = CAPTURE_VOFFSET,
	.hsize           = CAPTURE_HSIZE,
	.vsize           = CAPTURE_VSIZE,
	.lastlinepix     = CAPTURE_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = CAPTURE_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
};



static sensorTgCfg_t sensorTgCfg = {
	.tg_type = TGTYPE,
	.n_preview_submode = sizeof(sensorTgPreviewCfgTbl) /
			sizeof(sensorTgPreviewCfgTbl[0]),
	.n_snap_submode = sizeof(sensorTgSnapCfgTbl) /
			sizeof(sensorTgSnapCfgTbl[0]),
	.vshutter_need_resend = VSHUTTER_RESEND,
	.exp_effect_frame_count = EXP_FRAME_NUM,

};

static const sensorTgDev_t sensorTgDev = {
	.tgCfg = &sensorTgCfg,
	.tgSvc = &(const sensorTgSvc_t) {
		.resload = sensorCfgInit,
		.resstop = sensorCfgStop,
		.init = sensorDoFuncInit,
		.stop = sensorDoFuncStop, /****TODO***/
		.pwrsave = NULL, /****TODO***/
		.expwrite = sensor_cmdport_exp_write,
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
		.crop = NULL, /****TODO***/
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreviewVGACfg,
	.verStr = MODEL_VERSION,
};
UINT8 
OVsensorRead(UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[3];
	/*UINT8 readval=0;*/
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	sensorSerialBlockRead(cmdportid,1,regread,0);
	printf("\nREAD [%x] = [%x]\n",addr,regread[2]);
	return regread[2];
}


void
sensorTg_ov8810_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

/*------method definitions-----*/
		
static void
sensorDoFuncInit()
{

	UINT32 cmdportid;
    PROF_LOG_ADD(0, "sensorDoFuncInit()___enter");

   /* TGGPIO configuration here */    

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	/*Debug only */

	/*Pixel Clock = 27MHz /27*48/2 = 24 MHz, XGA= 15fps VGA=30fps*/
#if SPCA5330
	/*sensorTgPllClkExtSet(12000000);*//*16MHz*/
	sensorTgPllClkExtSet(24000000);
#else
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_DIV,1,27,(16*2),NOUSE);
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN,1,SENSOR_TGPLL_DRV_8MA,NOUSE,NOUSE);
#endif

	/*halPllFrontClkSet(0x00,94500); /* 94.5 MHz */
	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121500); /* 121.5MHz */

/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);

/*****TODO*****front reset after these???*/


#if 1 /*GPIO control */

	sp5kGpioCfgSet(SENOR_GPIO_GRP,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);

	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,0); /* PWD LOW*/
	tmrUsWait(50000); /*50ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(25000); /*25ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
#endif
	
   	cmdportid = sensorTgCfg.cmdportid_xtg;
   	tmrUsWait(2000);
	OVsensorRead(0x300a); /*sensor ID */

        sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code_SWReset)/sizeof(afe_init_code_SWReset[0])), afe_init_code_SWReset[0],0);	
	tmrUsWait(2000);
	sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code)/sizeof(afe_init_code[0])), afe_init_code[0],0);

	/*TG PLL=MCLK out=24MHz*/
	sensorLVDSCfgSet(&sensorLVDSCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

	tmrUsWait(100000);	/* delay 100 ms*/
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	/*halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121500); /* 121.5MHz */
	PROF_LOG_ADD(0, "sensorDoFuncInit()___done");
	
	tmrUsWait(10000);

}

static void
sensorDoFuncStop()
{
}
static void
sensorDoFuncPreviewVGABegin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/

 
  UINT32 cmdportid;

  cmdportid = sensorTgCfg.cmdportid_xtg;
  sensor_event_wait(0x04);

  sensorSerialBulkWrite(cmdportid, sizeof(afe_vga_code)/sizeof(afe_vga_code[0]), afe_vga_code[0],0);
  sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
  sensorDsenExtSet();
  PROF_LOG_ADD(0, "mipi VGA cmd done");

}
static void
sensorDoFuncPreviewXGABegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;

	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensor_event_wait(0x04);

	sensorSerialBulkWrite(cmdportid, sizeof(afe_xga_code)/sizeof(afe_xga_code[0]), afe_xga_code[0],0);
	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);
	sensorDsenExtSet();
	PROF_LOG_ADD(0, "mipi xga done");
   
}
static void
sensorDoFuncPreview720PBegin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/

 
  UINT32 cmdportid;

  cmdportid = sensorTgCfg.cmdportid_xtg;
  sensor_event_wait(0x04);

  sensorSerialBulkWrite(cmdportid, sizeof(afe_720P_code)/sizeof(afe_720P_code[0]), afe_720P_code[0],0);
  sensorSyncSigSet(&sensorSyncSigPreview720PCfg);
  sensorDsenExtSet();
  PROF_LOG_ADD(0, "mipi 720P cmd done");

}
        
static void
sensorDoFuncPreviewShortExp()
{       
	/*front_preview_short_exp_frame_set*/

}       
static void
sensorDoFuncPreviewExpEnd()
{       
	
	
	/*front_preview_exp_end_frame_set*/
}


/*frame settings*/
/*tg*/
static void
sensorDoFuncSnapExpBegin()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapExpBegin()*/
	/*throw user msg??*/
	/*snap TG exp e-shutter set*/
	/*flash frontsync trig*/
	/*image buf prepare*/
	/*MMU prepare*/
	/*front snap prepare*/
	/*cdsp snap prepare*/
	/*printf("***********snap expsoure begin\n");*/
}

#if 0
static void
sensorSnapFrameDummySet(
)
{
/*I2C set dummy line to 0 */
	
}
#endif

static void
sensorSnapFrameModeSet(
)
{

	UINT32 cmdportid;

	PROF_LOG_ADD(0, "sensorSnapFrameModeSet");
	cmdportid = sensorTgCfg.cmdportid_xtg;

	sensorSerialBulkWrite(cmdportid,(sizeof(afe_still_code)/sizeof(afe_still_code[0])), afe_still_code[0],0);
}

#if ( SNAP_EARLY0_NEEDED  == 1 )
static void
sensorDoFuncSnapEarly0()
{

    /*printf("sensorDoFuncSnapEarly1()---1\n");*/
	PROF_LOG_ADD(0, "sensorDoFuncSnapEarly1");
#if 1 /*register Frame end position */
		static  sensorSyncSigHdnumCfg_t earlyHdCfg;
		earlyHdCfg.hdnum_cmdport=200; /*PREVIEW_XGA_HDNUM_CMDPORT*/
		earlyHdCfg.hdnum_frame_end=300; /*PREVIEW_XGA_HDNUM_FRAME_END*/
		earlyHdCfg.hdnum_frame_start=PREVIEW_HDNUM_FRAME_START;
		sensorSyncSigHdnumSet(&earlyHdCfg);
#endif


}
#endif

static void
sensorDoFuncSnapEarly1()
{
	/*printf("sensorDoFuncSnapEarly1()\n");*/
	PROF_LOG_ADD(0, "sensorDoFuncSnapEarly0");
	sensorSnapFrameModeSet();
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	sensorDsenExtSet();
}

/* add this */ 
#if 0
void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param); 

static void 
FlashDirectTrig(void *param) 
{
    halFrontFlashCfgSet(0, 0); 
    halFrontFlashTrig(); 
} 

static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{

	PROF_LOG_ADD(0, "sensorDoFuncSnapFrexSet");
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_FREX,SENSOR_FREX);
  	sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, SENSOR_FREX);
	/* add this */ 
	if (sp5kSnapFlashCfgGet(SP5K_SNAP_FLASH_CFG_MODE))
		tmrWaitListInstall(2, 53, FlashDirectTrig, 0);/*FlashDirectTrig(0);*/
}
#endif

static void
sensorDoFuncSnapBshutterBegin()
{


	PROF_LOG_ADD(0, "sensorDoFuncSnapBshutterBegin()");

	/* sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);*/
	/*sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);*/

	
}

static void
sensorDoFuncSnapBshutterFollow()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapBshutterFollow()");
}

static void
sensorDoFuncSnapBshutterEnd()
{
	
	PROF_LOG_ADD(0, "sensorDoFuncSnapBshutterEnd()");
/*	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_FREX,SENSOR_FREX);
	sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);*/
	
}

static void
sensorDoFuncSnapShortExp()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapShortExp()");
}
static void
sensorDoFuncSnapExpEnd()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapExpEnd()_b");
	/*sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_FREX,SENSOR_FREX);
	sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);*/
	PROF_LOG_ADD(0, "sensorDoFuncSnapExpEnd()");

}

static void
sensorDoFuncSnapReadoutField0()
{

	/*sensor_frame_vd_cb_reg(sensorDoFuncSnapFrexSet,0);*/
	PROF_LOG_ADD(0, "sensorDoFuncSnapReadoutField0()");
}
static void
sensorDoFuncSnapFieldEnd()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapFieldEnd()");
}

/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapReadoutEnd()");
}

static void
sensorCfgInit()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	/*xtg registers for separate serial port*/
	static const sensorCmdport_t sensor_cmdport_xtg_cfg = {
		.mode         = TG_SPI_I2C_MODE,
		.format2      = TG_FORMAT2,
		.format3      = TG_FORMAT3,
		.speed        = TG_SPEED,
		.nbit_regdata = TG_NBIT_REGDATA,
		.nbit_regaddr = TG_NBIT_REGADDR,
	};
	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);
	/* if support burst
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_BURST; */
	/* if support seq
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_SEQ; */
	sensor_bshut_init();  /*bshut must init before exptable*/
	sensor_exptbl_init();
	sensorLVDSEnSet(1);	
	
}
static void
sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	sensorLVDSEnSet(0);
	/*Disable mipi interface*/
}
static void
sensor_cmdport_exp_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{

	/*printf("sensor_cmdport_exp_write\n");*/

	/*****TODO******ITG fine exp set, value and timing???*/
	/*exp setting via serial port*/
	
	/*UINT8 i;*/
	
	/*

	if((sensorModeGet()&0xF0)==SENSOR_MODE_SNAP){

	 profLogPrintf(2,"EnterSnapMode");

	 for(i=0;i<nreg;i++)	
        profLogPrintf(2,"$$$ %x,%x,%x,%x\n",nreg,pdata[3*i],pdata[3*i+1],pdata[3*i+2]);
	 
	  printf("Set snap mode ExpTbl\n");
	}
	*/

#if (_OV_EXP_DUMMY_LINE_SETTING_ORDER_ == 0)

      sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);

#else

	static UINT8 regdata1[][3] SENSOR_SIF1DMA_VARBUF= { 
	  {0x30, 0x03, 0x00},
	  {0x30, 0x02, 0x00},	  
	  {0x30, 0xb7, 0x8c},
	  {0x30, 0x1f, 0x00},
	  {0x30, 0x1e, 0x00},
	  {0x30, 0xb7, 0x84},			
	  {0x30, 0xff, 0xff}
	}; 

	if((sensorModeGet()&0xF0) == SENSOR_MODE_SNAP)
	{
		memcpy(regdata1,pdata,12);       
	}
	else
	{
		memcpy(regdata1,pdata,6);   		
		memcpy(&regdata1[3],&pdata[6],6);
		regdata1[2][0]=0x30;
		regdata1[2][1]=0xb7;
		regdata1[2][2]=0x8c;
	}

/*
      for(i=0;i<sizeof(regdata1)/sizeof(regdata1[0]);i++)  
	   profLogPrintf(1,"$$$ Idx_%x,%x,%x,%x\n",i,regdata1[i][0],regdata1[i][1],regdata1[i][2]); 
*/

#if (_Snap_ExpTime_Use_FrameLength_Adjustment_ == 1)
	UINT16 sTotalLine,sNewFrameLen;
	UINT16 sFrameLenLimit = 0xf000;	
	UINT16 sFrameLenBuffer = 10;

	if((sensorModeGet()&0xF0) == SENSOR_MODE_SNAP){
		/*Snap mode to change the frame length,0x3020,0x3021*/
		regdata1[2][1] = 0x21;
		regdata1[3][1] = 0x20;
		sTotalLine = (pdata[2] + (pdata[5]<<8)) + (pdata[8] + (pdata[11]<<8));
/*
	      profLogPrintf(1,"$$$ pdata[2]:%x,pdata[5]:%x,pdata[8]:%x,pdata[11]:%x\n",pdata[2],pdata[5],pdata[8],pdata[11]);
 	      profLogPrintf(1,"$$$ TotalLine:%x\n",sTotalLine);
*/
		if(sTotalLine < CaptureDefFrameLen)
			sNewFrameLen =  CaptureDefFrameLen;
		else
		{
			sNewFrameLen = sTotalLine + sFrameLenBuffer;   

			if(sNewFrameLen >= sFrameLenLimit)
				sNewFrameLen = sFrameLenLimit;

			if((sNewFrameLen-2) < sTotalLine)
			 	sTotalLine = sNewFrameLen-2;
		}

/*
		profLogPrintf(1,"$$$ TotalLine:%x,FrameLen:%x\n",sTotalLine,sNewFrameLen);	 
*/
		regdata1[0][2] = sTotalLine%256;
		regdata1[1][2] = sTotalLine>>8;			
		regdata1[2][2] = sNewFrameLen%256;
		regdata1[3][2] = sNewFrameLen>>8;
		
	}
#endif /*#if (_Snap_ExpTime_Use_FrameLength_Adjustment_ == 1)*/

/*
	for(i=0;i<sizeof(regdata1)/sizeof(regdata1[0]);i++)	 
		profLogPrintf(1,"$$$ Idx_%x,%x,%x,%x\n",i,regdata1[i][0],regdata1[i][1],regdata1[i][2]);     
*/

	sensorSerialDirectBufSnoop(regdata1, sizeof(regdata1));

	if((sensorModeGet()&0xF0) == SENSOR_MODE_SNAP)
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 4, regdata1);
	else	
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, sizeof(regdata1)/sizeof(regdata1[0]), regdata1);

#endif
  
#if MASK_EXP_CMD
#else
      #if 0
	if((sensorModeGet()&0xF0)==SENSOR_MODE_SNAP) /*don't set exp time& dummy in snap mode*/
		sensorSnapFrameDummySet();
	else
	{
	      profLogPrintf(0,"Send Cmd,CmdPortid=%d\n",sensorTgCfg.cmdportid_exp);
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
	}
	#endif	
#endif
	
}
static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
	/*printf("sensor_cmdport_exp_vtbl_do\n");*/
	/*PROF_LOG_PRINTF(0,"sensor_cmdport_exp_vtbl_do\n");*/
}

#if SP5K_SENSOR_OV8810
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regData[3][3] ={
		{0x30,0x02,0x00},
		{0x30,0x03,0x00},
		
	};
	regData[0][2] = (expLine>>8)&0xff;
	regData[1][2] =  expLine&0xff;
	
	profLogPrintf(1,"expLine(reg0) = %d [%x %x %x]",expLine,regData[0][0],regData[0][1],regData[0][2]);
	profLogPrintf(1,"expLine(reg1) = %d [%x %x %x]",expLine,regData[1][0],regData[1][1],regData[1][2]);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,2, regData[0]);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
	sensor_event_wait(0x04);/*wait new frame */
	expLine = expLine;
}
#endif


/******TODO****/


