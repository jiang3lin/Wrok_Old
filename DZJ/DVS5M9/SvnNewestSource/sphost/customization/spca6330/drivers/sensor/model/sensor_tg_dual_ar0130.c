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
 *  Author: Allen Liu                                                 *
 *
 *  01.01.01, 20121221 Allen
 *  0x30 & 0x40 (960p) : 1920X960 side by side mode 30fps
 *  0x31 & 0x41 (FULL) : 1280X960 by pass mode (sensor A) 30fps
 *  0x32 & 0x42 (FULL) : 1280X960 by pass mode (sensor B) 30fps
 *
 *  02.02.02, 20130110 Allen
 *  0x30 & 0x40 (960p) : 1920X960 side by side mode 30fps
 *  0x31 & 0x41 (720p60) : 1280X720 by pass mode (sensor A) 60fps (host scale up 1080p)
 *  0x32 & 0x42 (720p60) : 1280X720 by pass mode (sensor B) 60fps (host scale up 1080p)
 *  0x33 & 0x43 (720p30) : 1280X720 by pass mode (sensor A) 30fps (host scale up 1080p)
 *  0x34 & 0x44 (720p30) : 1280X720 by pass mode (sensor B) 30fps (host scale up 1080p)
 *  0x35 & 0x45 (FULL) : 1280X960 by pass mode (sensor A) 30fps
 *  0x36 & 0x46 (FULL) : 1280X960 by pass mode (sensor B) 30fps
 *
 *  03.02.03, 20130308 Allen
 *  Correct the fps to solve H264 assert issue
 **************************************************************************/
#define MODEL_VERSION "$unp_tg_dual_ar0130-03.02.03"
#define BUILD_TIME        __DATE__  "-"  __TIME__
#include <string.h>
#include "common.h"
#include "../flow/sysregs.h"
#include "../flow/hal_front.h"
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include <stdio.h>
#include "middleware/proflog.h"
#include "../flow/hal_front.h"
#include "../flow/sensor_model_snap.h"

#define MODE_SYNC_EARLY	50

#define DualSensor	1	/*0:disable dual-sensor function, 1:enable*/
#define RealKit		0

#if DualSensor
#define DualAE		1
#define OutputWidth	2	/*1:by pass, 2:side by side for ZT3010*/
#else
#define DualAE		0	/*Fixed*/
#define OutputWidth	1	/*Fixed*//*if DualSensor=0, it musts be 1*/
#endif

#define SENSOR_IRQ_SVC_FRAME_END       	(1)
#define SENSOR_IRQ_SVC_NEG_VD          	(10)
#define SENSOR_IRQ_SVC_ORG_VD          	(12)
#define SENSOR_EVENT_NEW_FRAME		(1UL<<2)
#define SENSOR_EVENT_VVALID_END       	(1UL<<13)
#define SENSOR_EVENT_CDSP_EOF         	(1UL<<16)

#if RealKit
#define ZT3010_RESET_GRP		GPIO_GRP_TGL
#define ZT3010_RESET_GPIO		(1<<1)
#else
#define ZT3010_RESET_GRP		GPIO_GRP_FML
#define ZT3010_RESET_GPIO		(1<<23)
#endif
#define SENSOR_RESET_GRP		GPIO_GRP_TGL
#define SENSORA_RESET_GPIO		(1<<0)
#define SENSORB_RESET_GPIO		(1<<4)

#define NO              	0          
#define BY_PIXEL   	1        
#define BY_HD         	0        
#define NOUSE         	0
#define DISABLE        	0xFFFF

/*****Hsize X Vsize*****/
#define _960P_HSIZE     (960*OutputWidth)          
#define _960P_VSIZE     (960)

#define _720P_HSIZE     (1280)          
#define _720P_VSIZE     (720)

#define FULL_HSIZE       (1280)          
#define FULL_VSIZE       (960)

/*960P default definition here */
#define _960P_LINETOTAL   	(_960P_HSIZE*5)	/* preview Hd length*/
#define _960P_LINEBLANK   	NOUSE			/* preview Hd sync length*/
#define _960P_FRAMETOTAL  	(_960P_VSIZE) 	/* preview Vd length*/
#define _960P_FRAMEBLANK  	NOUSE			/* preview Vd sync length*/
#if (OutputWidth==1)
#define _960P_HOFFSET          	472  			/* preview Hd valid pixel data offset position*/
#define _960P_VOFFSET     	6				/* preview Vd valid line data offset position*/
#else
#define _960P_HOFFSET          	16 
#define _960P_VOFFSET     	3
#endif

/*720P default definition here */
#define _720P_LINETOTAL   	(_720P_HSIZE*5)	/* preview Hd length*/
#define _720P_LINEBLANK   	NOUSE			/* preview Hd sync length*/
#define _720P_FRAMETOTAL  	(_720P_VSIZE) 	/* preview Vd length*/
#define _720P_FRAMEBLANK  	NOUSE 			/* preview Vd sync length*/
#define _720P_HOFFSET         	112   			/* preview Hd valid pixel data offset position*/
#define _720P_VOFFSET     	6				/* preview Vd valid line data offset position*/

/*FULL default definition here */
#define FULL_LINETOTAL   	(FULL_HSIZE*5)	/* preview Hd length*/
#define FULL_LINEBLANK   	NOUSE			/* preview Hd sync length*/
#define FULL_FRAMETOTAL  	(FULL_VSIZE) 		/* preview Vd length*/
#define FULL_FRAMEBLANK  	NOUSE 			/* preview Vd sync length*/
#define FULL_HOFFSET          	368   			/* preview Hd valid pixel data offset position*/
#define FULL_VOFFSET     		6				/* preview Vd valid line data offset position*/

#define HDNUM_FRAME_START  	110		/*the front interrupt line number of the preview new frame*/
#define HDNUM_FRAME_END		2		/*the rear interrupt line number of the preview new frame*/
#define HDNUM_CMDPORT			400		/*the command port interrupt line number of the preview new frame*/

/* Reshape sync signal definition here */
#define HRESHAPE_FALL         DISABLE		/* Hd reshap fall*/
#define HRESHAPE_RISE         DISABLE		/* Hd reshap rise*/
#define VRESHAPE_COUNTER_SEL  NO		/* Vd reshap control*/
#define VRESHAPE_FALL         DISABLE		/* Vd reshap fall*/
#define VRESHAPE_RISE         DISABLE 		/* Vd reshap rise*/

/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV                  1        /* mclk drivng, '1' : 8mA, '0' : 4mA */                                                                                                                                                                                              
#define MCLKMODE		   1	     /* mclk mode,'0'from I/O Pad,'1'from internal*/                                                                                                                                                                                      
#define PCLKMODE		   0	     /* pclk mode,'0'from I/O Pad,'1'from internal*/                                                                                                                                                                                      
#define TGPLLEN   		   1	     /* enable TGPLL,'0'disable,'1'enable*/                                                                                                                                                                                               
#define TGPLLCLKSET		   8	     /* TGPLL clk 0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,                                                                                                                                                                           
						                          5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/                                                                                                                                                                
#define MCLKDIV			   4	     /* MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/                                                                                                                                            
#define PCLKDIV			   2	     /* pixel clock = (master clock) / pclkdiv*/                                                                                                                                                                                          
#define MASTERPHASE	   0       /* master clk phase*/
#define PIXELPHASE		   0	     /* pixel clk phase*/
#define ADCKPHASE		   0	     /* adck clk phase*/                                                                                                                                                                                                                  

#define RGBADJ		          0	      /*RGB input data sample phase adj "1"negedge "0"posedge*/

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      					TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE				TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						     							 TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/

#define	_960P_CFAPATTERN	BAYER_BGGR       	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	_720P_CFAPATTERN	BAYER_BGGR       	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	FULL_CFAPATTERN	BAYER_BGGR      	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define	SNAP_DUMMYVD_NUM		0            /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM		1	       /*snap readout Vd need numbers*/
#define	SNAP_EARLY_NUM			0	       /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM			0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM			0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  	0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 			0		/*sony FALSE;panasonic/ITG TRUE*/
#define	EXP_FRAME_NUM       			1	    	/*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/
#define 	SNAP_PREVIEW_DELAY_FRAME_NUM  2
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
#define	TG_SPI_I2C_MODE  	0x30     	           /*I2C slave addr for write*/
#define	TG_FORMAT2  	 	FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	    	0		           /*don't change*/
#define	TG_SPEED    	 FRONT_SSC_CLK_384K    /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						                             	FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#define	TG_NBIT_REGDATA    16		           /*ssc/spi register data numbers */           
#define	TG_NBIT_REGADDR    16		           /*ssc/spi register address numbers */      

#define	ZT_SPI_I2C_MODE  	0x7E
#define	ZT_NBIT_REGDATA    8		           /*ssc/spi register data numbers */           
#define	ZT_NBIT_REGADDR    8		           /*ssc/spi register address numbers */      

/**************global variable *******************/
UINT32 cmdportid_zt;
UINT32 cmdportid_tg;
UINT8 g_dualAE=0;
static UINT32 g_IsSnap2Pv=0;
static UINT8 g_Is_trigger=0;
/*static UINT8 g_rotate=0;*/


extern UINT32 sensorModeGet(void);
extern void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param); 

/**************prototypes of sensor model methods *******************/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp2_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreview960PBegin();
static void sensorDoFuncPreview720PBegin();
static void sensorDoFuncPreviewFULLBegin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
static void sensorDoFuncSnapEarly0();

/******************** AR0130 uCode ****************************/
static const UINT8 cmd_init_code_SWReset[][4]={
{0x30,0x1A,0x00,0x01}
};

static const UINT8 cmd_StreamOn[][4]={
{0x30,0x1A,0x10,0xDC}
};

static const UINT8 cmd_StreamOff[][4]={
{0x30,0x1A,0x10,0xD8}
};

static const UINT8 cmd_flipmirror[][4]={
{0x30,0x40,0x00,0x00}
};

static const UINT8 cmd_flipmirror_Restore[][4]={
{0x30,0x40,0xC0,0x00}
};

static const UINT8 cmd_DisColumnCorrection[][4]={
{0x30,0xD4,0x60,0x07}
};

static const UINT8 cmd_EnColumnCorrection[][4]={
{0x30,0xD4,0xE0,0x07}
};

static const UINT8 cmd_LinearSequencer[][4]={
{0x30,0x88,0x80,0x00},
{0x30,0x86,0x02,0x25},
{0x30,0x86,0x50,0x50},
{0x30,0x86,0x2D,0x26},
{0x30,0x86,0x08,0x28},
{0x30,0x86,0x0D,0x17},
{0x30,0x86,0x09,0x26},
{0x30,0x86,0x00,0x28},
{0x30,0x86,0x05,0x26},
{0x30,0x86,0xA7,0x28},
{0x30,0x86,0x07,0x25},
{0x30,0x86,0x80,0x80},
{0x30,0x86,0x29,0x17},
{0x30,0x86,0x05,0x25},
{0x30,0x86,0x00,0x40},
{0x30,0x86,0x27,0x02},
{0x30,0x86,0x16,0x16},
{0x30,0x86,0x27,0x06},
{0x30,0x86,0x17,0x36},
{0x30,0x86,0x26,0xA6},
{0x30,0x86,0x17,0x03},
{0x30,0x86,0x26,0xA4},
{0x30,0x86,0x17,0x1F},
{0x30,0x86,0x28,0x05},
{0x30,0x86,0x26,0x20},
{0x30,0x86,0x28,0x04},
{0x30,0x86,0x25,0x20},
{0x30,0x86,0x20,0x27},
{0x30,0x86,0x00,0x17},
{0x30,0x86,0x1E,0x25},
{0x30,0x86,0x00,0x20},
{0x30,0x86,0x21,0x17},
{0x30,0x86,0x10,0x28},
{0x30,0x86,0x05,0x1B},
{0x30,0x86,0x17,0x03},
{0x30,0x86,0x27,0x06},
{0x30,0x86,0x17,0x03},
{0x30,0x86,0x17,0x47},
{0x30,0x86,0x26,0x60},
{0x30,0x86,0x17,0xAE},
{0x30,0x86,0x25,0x00},
{0x30,0x86,0x90,0x27},
{0x30,0x86,0x00,0x26},
{0x30,0x86,0x18,0x28},
{0x30,0x86,0x00,0x2E},
{0x30,0x86,0x2A,0x28},
{0x30,0x86,0x08,0x1E},
{0x30,0x86,0x08,0x31},
{0x30,0x86,0x14,0x40},
{0x30,0x86,0x40,0x14},
{0x30,0x86,0x20,0x20},
{0x30,0x86,0x14,0x10},
{0x30,0x86,0x10,0x34},
{0x30,0x86,0x14,0x00},
{0x30,0x86,0x10,0x14},
{0x30,0x86,0x00,0x20},
{0x30,0x86,0x14,0x00},
{0x30,0x86,0x40,0x13},
{0x30,0x86,0x18,0x02},
{0x30,0x86,0x14,0x70},
{0x30,0x86,0x70,0x04},
{0x30,0x86,0x14,0x70},
{0x30,0x86,0x70,0x03},
{0x30,0x86,0x14,0x70},
{0x30,0x86,0x70,0x17},
{0x30,0x86,0x20,0x02},
{0x30,0x86,0x14,0x00},
{0x30,0x86,0x20,0x02},
{0x30,0x86,0x14,0x00},
{0x30,0x86,0x50,0x04},
{0x30,0x86,0x14,0x00},
{0x30,0x86,0x20,0x04},
{0x30,0x86,0x14,0x00},
{0x30,0x86,0x50,0x22},
{0x30,0x86,0x03,0x14},
{0x30,0x86,0x00,0x20},
{0x30,0x86,0x03,0x14},
{0x30,0x86,0x00,0x50},
{0x30,0x86,0x2C,0x2C},
{0x30,0x86,0x2C,0x2C},
{0x30,0x9E,0x00,0x00},
{0x30,0xE4,0x63,0x72},
{0x30,0xE2,0x72,0x53},
{0x30,0xE0,0x54,0x70},
{0x30,0xE6,0xC4,0xCC},
{0x30,0xE8,0x80,0x50}
};

static const UINT8 cmd_OptimizedSettings[][4]={
{0x30,0x82,0x00,0x29},
{0x3E,0xE6,0x43,0x03},
{0x30,0x1E,0x00,0x7E},/*black level offset*/
{0x3E,0xDA,0x0F,0x03},
{0x3E,0xDE,0xC0,0x05},
{0x3E,0xD8,0x09,0xEF},
{0x3E,0xE2,0xA4,0x6B},
{0x3E,0xE0,0x04,0x7D},
{0x3E,0xDC,0x00,0x70},
{0x30,0x44,0x04,0x04},
{0x3E,0xE4,0xD2,0x08},
{0x3E,0xD6,0x00,0xBD}
};

static const UINT8 cmd_ColumnRetriggering[][4]={
{0x30,0xB0,0x13,0x00},
{0x30,0x40,0xC0,0x00},/*Flip and mirror*/
{0x30,0xD4,0xE0,0x07},
{0x30,0xBA,0x00,0x08}/*Digital_CTRL*/
};

static const UINT8 cmd_init_code[][4]={
{0x30,0x44,0x04,0x00},
{0x30,0x12,0x03,0x41},
{0x31,0xD0,0x00,0x01},
{0x30,0x64,0x18,0x02},
/*[960x976 30FPS Setup]*/
{0x30,0x32,0x00,0x00},
{0x30,0x02,0x00,0x02},
{0x30,0x04,0x00,0xA0},
{0x30,0x06,0x03,0xD1},
{0x30,0x08,0x04,0x5F},
{0x30,0x0A,0x03,0xEE},
{0x30,0x0C,0x05,0x96},
/*[PLL Enabled 24Mhz to 43.102Mhz]*/
{0x30,0x2C,0x00,0x01},
{0x30,0x2A,0x00,0x0E},
{0x30,0x2E,0x00,0x07},
{0x30,0x30,0x00,0xB0},
{0x30,0xB0,0x13,0x00}
};

static const UINT8 cmd_960P_code[][4]={
/*Stream off*/
{0x30,0x1A,0x10,0xD8},
{0x30,0x12,0x03,0x41},
/*[960x976 30FPS Setup]*/
{0x30,0x32,0x00,0x00},
{0x30,0x02,0x00,0x02},
{0x30,0x04,0x00,0xA0},
{0x30,0x06,0x03,0xD1},
{0x30,0x08,0x04,0x5F},
{0x30,0x0A,0x03,0xEE},
{0x30,0x0C,0x05,0x96},
/*[PLL Enabled 24Mhz to 43.102Mhz]*/
{0x30,0x2C,0x00,0x01},
{0x30,0x2A,0x00,0x0E},
{0x30,0x2E,0x00,0x07},
{0x30,0x30,0x00,0xB0},
{0x30,0xB0,0x13,0x00},
/*Stream on*/
{0x30,0x1A,0x10,0xDC}
};

static const UINT8 cmd_720P60_code[][4]={
/*Stream off*/
{0x30,0x1A,0x10,0xD8},
{0x30,0x12,0x03,0x41},
/*[1280X733 60FPS Setup]*/
{0x30,0x32,0x00,0x00},
{0x30,0x02,0x00,0x76},
{0x30,0x04,0x00,0x02},
{0x30,0x06,0x03,0x51},
{0x30,0x08,0x05,0x01},
{0x30,0x0A,0x03,0x7B},
{0x30,0x0C,0x05,0x6E},
/*[PLL Enabled 24Mhz to 74.25Mhz]*/
{0x30,0x2C,0x00,0x01},
{0x30,0x2A,0x00,0x08},
{0x30,0x2E,0x00,0x08},
{0x30,0x30,0x00,0xC6},
{0x30,0xB0,0x13,0x00},
/*Stream on*/
{0x30,0x1A,0x10,0xDC}
};

static const UINT8 cmd_720P30_code[][4]={
/*Stream off*/
{0x30,0x1A,0x10,0xD8},
{0x30,0x12,0x03,0x41},
/*[1280X733 30FPS Setup]*/
{0x30,0x32,0x00,0x00},
{0x30,0x02,0x00,0x76},
{0x30,0x04,0x00,0x02},
{0x30,0x06,0x03,0x51},
{0x30,0x08,0x05,0x01},
{0x30,0x0A,0x03,0x7B},
{0x30,0x0C,0x05,0x6E},
/*[PLL Enabled 24Mhz to 37.125Mhz]*/
{0x30,0x2C,0x00,0x01},
{0x30,0x2A,0x00,0x10},
{0x30,0x2E,0x00,0x08},
{0x30,0x30,0x00,0xC6},
{0x30,0xB0,0x13,0x00},
/*Stream on*/
{0x30,0x1A,0x10,0xDC}
};

static const UINT8 cmd_FULL_code[][4]={
/*Stream off*/
{0x30,0x1A,0x10,0xD8},
{0x30,0x12,0x03,0x41},
/*[Full Resolution 30FPS Setup]*/
{0x30,0x32,0x00,0x00},
{0x30,0x02,0x00,0x02},
{0x30,0x04,0x00,0x00},
{0x30,0x06,0x03,0xD1},
{0x30,0x08,0x04,0xFF},
{0x30,0x0A,0x03,0xEE},
{0x30,0x0C,0x06,0x72},
/*[PLL Enabled 24Mhz to 49.714Mhz]*/
{0x30,0x2C,0x00,0x01},
{0x30,0x2A,0x00,0x08},
{0x30,0x2E,0x00,0x07},
{0x30,0x30,0x00,0x74},
{0x30,0xB0,0x13,0x00},
/*Stream on*/
{0x30,0x1A,0x10,0xDC}
};

/******************** ZT3010 uCode ****************************/
static const UINT8 cmd_zt_reset[][2]={
{0x00,0x02}
};

static const UINT8 cmd_zt_enable[][2]={
{0x00,0x01}
};

static const UINT8 cmd_zt_bypassA[][2]={
{0xA8,0x3C}/*choose sensor A*/
};

static const UINT8 cmd_zt_bypassB[][2]={
{0xA8,0x3D}/*choose sensor B*/
};

static const UINT8 cmd_zt_sidebyside[][2]={
{0xA8,0x3F}
};

static const UINT8 cmd_zt_sensorA[][2]={
{0xb0,0x16}/*command to sensor A*/
};

static const UINT8 cmd_zt_sensorB[][2]={
{0xb0,0x19}/*command to sensor B*/
};

static const UINT8 cmd_zt_sensorAB[][2]={
{0xb0,0x00}
};

static const UINT8 cmd_zt_bypass_code[][2]={
{0x00,0x02},
{0xA8,0x3c},
{0x00,0x02},
{0x00,0x01}
};

static const UINT8 cmd_zt_sidebyside_code1[][2]={
{0x00, 0x02},
{0x00, 0x00},
{0xA2, 0x00},
{0xA3, 0x19},
{0xA4, 0x00},
{0xA8, 0x3F},
{0x05, 0x0B},
{0x04, 0x00},
{0x06, 0x11},
{0x00, 0x02}  /*delay for detect*/
};

static const UINT8 cmd_zt_sidebyside_code2[][2]={
{0x0C, 0x10},
{0x0D, 0x00},
{0x0E, 0x20},
{0x0F, 0x00},
/*SBUTGV SBU Timing Generator Vsyn [31:16]VE+[15:0]VS*/
{0x10, 0x30},
{0x11, 0x00},
{0x12, 0x40},
{0x13, 0x08},
/*Sensor 0 Horizontal window*/
{0x14, 0xD6}, /*start 0x1D6 = 470*/
{0x15, 0x01},
{0x16, 0x95}, /*0x595 = 1429*/
{0x17, 0x05}, 
/*Sensor 0 Vertical window*/
{0x18, 0x02},
{0x19, 0x00},
{0x1A, 0xD1}, /*0x3D1 = 977*/
{0x1B, 0x03},
/*Sensor 1 Horizontal window*/
{0x1C, 0xD6}, /*start 0x1D6 = 470*/
{0x1D, 0x01},
{0x1E, 0x95}, /*0x595 = 1429*/
{0x1F, 0x05},
/*WD : 1429 - 470 + 1= 960*/
/*Sensor 1 Vertical window*/
{0x20, 0x02},
{0x21, 0x00},
{0x22, 0xD1}, /*0x3D1 = 977*/
{0x23, 0x03},
/*CSI SBU output window Horizontal*/
{0x24, 0x00},
{0x25, 0x00},
{0x26, 0x7F},
{0x27, 0x07},
/*CSI SBU output window Vertical*/
{0x28, 0x00},
{0x29, 0x00},
{0x2A, 0xCF}, /*0x3D0 = 976*/
{0x2B, 0x03},
/*CSI IO sync H/V*/
{0x2C, 0x80},	 /*should be set WD*/
{0x2D, 0x07},
{0x2E, 0xFF}, /*should be set 0xFFFF*/
{0x2F, 0xFF},
{0x30, 0xC0}, /*half of WD*/
{0x31, 0x03},
{0x32, 0x00},
{0x00, 0x01}
};

typedef struct {
	UINT8  id;/*sensor id*/
	UINT8  valid;
	UINT8  nreg;
	UINT8  data[16];
} ae_data_t;
ae_data_t reg_exp[2],reg_agc[2];

/***************************sensor tg cfg******************************/
static const sensorTgPreviewCfg_t sensorTgPreview960PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview960PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = _960P_HSIZE,
		.ySize = _960P_VSIZE,
	},
	.hsize = _960P_HSIZE,
	.vsize = _960P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = _960P_CFAPATTERN,
	.hratio=1000*OutputWidth,
	.vratio=1000,
	.fps_mhz = 29960000,
	.sensor_pclk = 43102000*OutputWidth,
	.sensor_htotal = 1430*OutputWidth,
	.pix_100khz = 1480,
};

static const sensorTgPreviewCfg_t sensorTgPreview720P60Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = _720P_HSIZE,
		.ySize = _720P_VSIZE,
	},
	.hsize = _720P_HSIZE,
	.vsize = _720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = _720P_CFAPATTERN,
	.hratio=1600,
	.vratio=900,	
	.fps_mhz = 59950000,
	.sensor_pclk = 74250000,
	.sensor_htotal = 1390,
	.pix_100khz = 1480,
};

static const sensorTgPreviewCfg_t sensorTgPreview720P30Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = _720P_HSIZE,
		.ySize = _720P_VSIZE,
	},
	.hsize = _720P_HSIZE,
	.vsize = _720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = _720P_CFAPATTERN,
	.hratio=1600,
	.vratio=900,	
	.fps_mhz = 29980000,
	.sensor_pclk = 37125000,
	.sensor_htotal = 1390,
	.pix_100khz = 1480,
};

static const sensorTgPreviewCfg_t sensorTgPreviewFULLCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewFULLBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = FULL_HSIZE,
		.ySize = FULL_VSIZE,
	},
	.hsize = FULL_HSIZE,
	.vsize = FULL_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = FULL_CFAPATTERN,
	.hratio=4000,
	.vratio=3000,	
	.fps_mhz = 29950000,
	.sensor_pclk = 49714000,
	.sensor_htotal = 1650,
	.pix_100khz = 1480,
};

static const sensorTgSnapCfg_t sensorTgSnap960PCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0, 
		},
		.snapExpBegin = sensorDoFuncSnapExpBegin,
		.snapBshutterBegin = NULL,
		.snapBshutterFollow = NULL,
		.snapBshutterEnd = NULL,
		.snapLongExpBegin = NULL,
		.snapLongExpFollow = NULL,
		.snapLongExpEnd = NULL,
		.snapShortExp = sensorDoFuncSnapShortExp,
		.snapExpEnd = sensorDoFuncSnapExpEnd,
		.psnapDummy = NULL,
		.psnapReadoutField = (sensorDoFunc_t []) {
			sensorDoFuncSnapReadoutField0,
		},
		.snapFieldEnd = sensorDoFuncSnapFieldEnd,
		.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
	},
	.crop = NULL,
	.hsize = _960P_HSIZE,
	.vsize = _960P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = _960P_CFAPATTERN,
	.n_dummy = SNAP_DUMMYVD_NUM,
	.n_readout = SNAP_READOUTVD_NUM,
	.n_snap_early = SNAP_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
	.pix_100khz =1480,
};

static const sensorTgSnapCfg_t sensorTgSnap720PCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0, 
		},
		.snapExpBegin = sensorDoFuncSnapExpBegin,
		.snapBshutterBegin = NULL,
		.snapBshutterFollow = NULL,
		.snapBshutterEnd = NULL,
		.snapLongExpBegin = NULL,
		.snapLongExpFollow = NULL,
		.snapLongExpEnd = NULL,
		.snapShortExp = sensorDoFuncSnapShortExp,
		.snapExpEnd = sensorDoFuncSnapExpEnd,
		.psnapDummy = NULL,
		.psnapReadoutField = (sensorDoFunc_t []) {
			sensorDoFuncSnapReadoutField0,
		},
		.snapFieldEnd = sensorDoFuncSnapFieldEnd,
		.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
	},
	.crop = NULL,
	.hsize = _720P_HSIZE,
	.vsize = _720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = _720P_CFAPATTERN,
	.n_dummy = SNAP_DUMMYVD_NUM,
	.n_readout = SNAP_READOUTVD_NUM,
	.n_snap_early = SNAP_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
	.pix_100khz =1480,
};

static const sensorTgSnapCfg_t sensorTgSnapFULLCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0, 
		},
		.snapExpBegin = sensorDoFuncSnapExpBegin,
		.snapBshutterBegin = NULL,
		.snapBshutterFollow = NULL,
		.snapBshutterEnd = NULL,
		.snapLongExpBegin = NULL,
		.snapLongExpFollow = NULL,
		.snapLongExpEnd = NULL,
		.snapShortExp = sensorDoFuncSnapShortExp,
		.snapExpEnd = sensorDoFuncSnapExpEnd,
		.psnapDummy = NULL,
		.psnapReadoutField = (sensorDoFunc_t []) {
			sensorDoFuncSnapReadoutField0,
		},
		.snapFieldEnd = sensorDoFuncSnapFieldEnd,
		.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
	},
	.crop = NULL,
	.hsize = FULL_HSIZE,
	.vsize = FULL_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = FULL_CFAPATTERN,
	.n_dummy = SNAP_DUMMYVD_NUM,
	.n_readout = SNAP_READOUTVD_NUM,
	.n_snap_early = SNAP_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
	.pix_100khz =1480,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	&sensorTgPreview960PCfg, &sensorTgPreview720P60Cfg, &sensorTgPreview720P60Cfg, &sensorTgPreview720P30Cfg, &sensorTgPreview720P30Cfg, &sensorTgPreviewFULLCfg, &sensorTgPreviewFULLCfg,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnap960PCfg, &sensorTgSnap720PCfg, &sensorTgSnap720PCfg, &sensorTgSnap720PCfg, &sensorTgSnap720PCfg, &sensorTgSnapFULLCfg, &sensorTgSnapFULLCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview960PCfg = {
	.linetotal       = _960P_LINETOTAL,
	.lineblank       = _960P_LINEBLANK,
	.frametotal      = _960P_FRAMETOTAL,
	.frameblank      = _960P_FRAMEBLANK,
	.hreshape_fall   = HRESHAPE_FALL,
	.hreshape_rise   = HRESHAPE_RISE,
	.vreshape_fall   = VRESHAPE_FALL,
	.vreshape_rise   = VRESHAPE_RISE,
	.hoffset         = _960P_HOFFSET,
	.voffset         = _960P_VOFFSET,
	.hsize           = _960P_HSIZE,
	.vsize           = _960P_VSIZE,
	.lastlinepix     = _960P_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
#if (OutputWidth==1)
	.conf_hdpclkedge = 0,
#else
	.conf_hdpclkedge = 1,	/*0:pclkphase=0, 1:pclkphase=8*/
#endif
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = HDNUM_FRAME_START,
	.hdnum_frame_end = HDNUM_FRAME_END,
	.hdnum_cmdport   = HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview720PCfg = {
	.linetotal       = _720P_LINETOTAL,
	.lineblank       = _720P_LINEBLANK,
	.frametotal      = _720P_FRAMETOTAL,
	.frameblank      = _720P_FRAMEBLANK,
	.hreshape_fall   = HRESHAPE_FALL,
	.hreshape_rise   = HRESHAPE_RISE,
	.vreshape_fall   = VRESHAPE_FALL,
	.vreshape_rise   = VRESHAPE_RISE,
	.hoffset         = _720P_HOFFSET,
	.voffset         = _720P_VOFFSET,
	.hsize           = _720P_HSIZE,
	.vsize           = _720P_VSIZE,
	.lastlinepix     = _720P_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = HDNUM_FRAME_START,
	.hdnum_frame_end = HDNUM_FRAME_END,
	.hdnum_cmdport   = HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFULLCfg = {
	.linetotal       = FULL_LINETOTAL,
	.lineblank       = FULL_LINEBLANK,
	.frametotal      = FULL_FRAMETOTAL,
	.frameblank      = FULL_FRAMEBLANK,
	.hreshape_fall   = HRESHAPE_FALL,
	.hreshape_rise   = HRESHAPE_RISE,
	.vreshape_fall   = VRESHAPE_FALL,
	.vreshape_rise   = VRESHAPE_RISE,
	.hoffset         = FULL_HOFFSET,
	.voffset         = FULL_VOFFSET,
	.hsize           = FULL_HSIZE,
	.vsize           = FULL_VSIZE,
	.lastlinepix     = FULL_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = HDNUM_FRAME_START,
	.hdnum_frame_end = HDNUM_FRAME_END,
	.hdnum_cmdport   = HDNUM_CMDPORT,
};

static sensorTgCfg_t sensorTgCfg = {
	.tg_type = TGTYPE,
	.n_preview_submode = sizeof(sensorTgPreviewCfgTbl) /
			sizeof(sensorTgPreviewCfgTbl[0]),
	.n_snap_submode = sizeof(sensorTgSnapCfgTbl) /
			sizeof(sensorTgSnapCfgTbl[0]),
	.vshutter_need_resend = VSHUTTER_RESEND,
	.exp_effect_frame_count = EXP_FRAME_NUM,
	.n_snap_pv_delay = SNAP_PREVIEW_DELAY_FRAME_NUM,
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
		.exp2write = sensor_cmdport_exp2_write,
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
		.crop = NULL, /****TODO***/
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreview960PCfg,
	.verStr = MODEL_VERSION,
};

void sensorTg_ar0130_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

static void sensorReportsize()
{
	profLogPrintf(1,"mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	profLogPrintf(1,"front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	profLogPrintf(1,"imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	profLogPrintf(1,"rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	WRITE16(0xb0009064,1);
	profLogPrintf(1,"line count            = (%d)",READ16(0xb0009064));
}

static UINT8 ZT3010Read(UINT8 regaddr)
{
	static UINT8 reg[2];
	reg[0]=regaddr;

	tmrUsWait(30000);/*30ms*/
	sensorSerialBlockRead(cmdportid_zt,1,reg,0);
	profLogPrintf(1,"register 0x%x=0x%x \n",reg[0], reg[1]);
	return reg[1];
}

static void sensorDoFuncInit()
{
	UINT8 zt3010reg;
	printf("SENSOR TG AFE Version %s\n",MODEL_VERSION );
	profLogPrintf(1,"@@@@@@@@@  initial AR0130 begin \n");

    	/* TGGPIO configuration here */    
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	halFrontClkCfg(MCLKMODE,1);

    	sensorTgPllClkExtSet(24000000);

	/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);

	/*Reset ZT3010+AR0130*/
	sp5kGpioCfgSet(ZT3010_RESET_GRP, ZT3010_RESET_GPIO, ZT3010_RESET_GPIO); /*ZT3010 RESET*/
	sp5kGpioCfgSet(SENSOR_RESET_GRP, SENSORA_RESET_GPIO, SENSORA_RESET_GPIO); /*SENSOR A RESET*/
	sp5kGpioCfgSet(SENSOR_RESET_GRP, SENSORB_RESET_GPIO, SENSORB_RESET_GPIO); /*SENSOR B RESET*/
#if !DualSensor
	sp5kGpioWrite(ZT3010_RESET_GRP, ZT3010_RESET_GPIO, (0));
	tmrUsWait(10000);
	sp5kGpioWrite(ZT3010_RESET_GRP, ZT3010_RESET_GPIO, ZT3010_RESET_GPIO);
	tmrUsWait(10000);
#else
	/*****Reset ZT3010 Low******/
	sp5kGpioWrite(ZT3010_RESET_GRP, ZT3010_RESET_GPIO, (0));
	tmrUsWait(10000);
	sp5kGpioWrite(SENSOR_RESET_GRP, SENSORA_RESET_GPIO|SENSORB_RESET_GPIO, (0));
	tmrUsWait(10000);
	sp5kGpioWrite(SENSOR_RESET_GRP, SENSORA_RESET_GPIO|SENSORB_RESET_GPIO, SENSORA_RESET_GPIO|SENSORB_RESET_GPIO);
	tmrUsWait(10000);
#endif

	sensorSerialBulkWrite(cmdportid_tg,(sizeof(cmd_init_code_SWReset)/sizeof(cmd_init_code_SWReset[0])), cmd_init_code_SWReset[0],0);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(cmd_StreamOff)/sizeof(cmd_StreamOff[0])), cmd_StreamOff[0],0);
	tmrUsWait(10000);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(cmd_LinearSequencer)/sizeof(cmd_LinearSequencer[0])), cmd_LinearSequencer[0],0);
	tmrUsWait(20000);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(cmd_OptimizedSettings)/sizeof(cmd_OptimizedSettings[0])), cmd_OptimizedSettings[0],0);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(cmd_ColumnRetriggering)/sizeof(cmd_ColumnRetriggering[0])), cmd_ColumnRetriggering[0],0);
	tmrUsWait(50000);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(cmd_init_code)/sizeof(cmd_init_code[0])), cmd_init_code[0],0);
	tmrUsWait(10000);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(cmd_StreamOn)/sizeof(cmd_StreamOn[0])), cmd_StreamOn[0],0);

	tmrUsWait(10000);
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	halFrontClkCfg(MCLKMODE, PCLKMODE);
#if DualSensor	
	/*****Reset ZT3010 high******/
	sp5kGpioWrite(ZT3010_RESET_GRP, ZT3010_RESET_GPIO, ZT3010_RESET_GPIO);
	tmrUsWait(10000);

	/*****Init ZT3010******/
#if (OutputWidth==1)
	sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_bypass_code)/sizeof(cmd_zt_bypass_code[0])), cmd_zt_bypass_code[0],0);
#else
	g_dualAE=1;
	sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sidebyside_code1)/sizeof(cmd_zt_sidebyside_code1[0])), cmd_zt_sidebyside_code1[0],0);
	while(1)
	{
		zt3010reg=ZT3010Read(0x02)&0x1;
		if (zt3010reg==1)
			break;
	}
	sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sidebyside_code2)/sizeof(cmd_zt_sidebyside_code2[0])), cmd_zt_sidebyside_code2[0],0);
#endif
#endif

#if MODE_SYNC_EARLY /*config pre command IRQ */
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif

	sensorStsCfg(SENSOR_SYS_CFG_DUAL_SENSOR_AE_EN,DualAE);/*dual sensor ae enable*/

#if DualAE
	sensorSyncSigDualSelSet(SP5K_DUAL_SENOR_BOTH);
	memset(&reg_exp,sizeof(ae_data_t),0);
	memset(&reg_agc,sizeof(ae_data_t),0);
#else
	sensorSyncSigDualSelSet(SP5K_DUAL_SENOR_DISABLE);
#endif

	profLogPrintf(1,"@@@@@@@@@  initial AR0130 end\n");

	/*sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, 1<<15, 1<<15);
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<15, 0);*/
}

static void sensorDoFuncStop()
{
}

static void sensorDoFuncPreview960PBegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	profLogAdd(1,"sensorDoFuncPreview960PBegin+++");
	sensorEventVvalidWait(1);
	
	if(g_IsSnap2Pv==1)
	{
		/*sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<15, 0);*/
		g_IsSnap2Pv=0;
	}
	else
	{
		UINT8 zt3010reg;
		sensorSerialBulkWrite(cmdportid_tg,sizeof(cmd_960P_code)/sizeof(cmd_960P_code[0]), cmd_960P_code[0],0);
		sensorSyncSigSet(&sensorSyncSigPreview960PCfg);
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
#if DualSensor
#if (OutputWidth==2)
		sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sidebyside)/sizeof(cmd_zt_sidebyside[0])), cmd_zt_sidebyside[0],0);
		g_dualAE=1;
		sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_reset)/sizeof(cmd_zt_reset[0])), cmd_zt_reset[0],0);
		while(1)
		{
			zt3010reg=ZT3010Read(0x02)&0x1;
			if (zt3010reg==1)
				break;
		}
		sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_enable)/sizeof(cmd_zt_enable[0])), cmd_zt_enable[0],0);
#if DualAE
		sensorSyncSigDualSelSet(SP5K_DUAL_SENOR_BOTH);
#endif
#endif
#endif
	}
	profLogAdd(1,"sensorDoFuncPreview960PBegin---");
}

static void sensorDoFuncPreview720PBegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	profLogAdd(1,"sensorDoFuncPreview720PBegin+++");
	sensorEventVvalidWait(1);
	
	if(g_IsSnap2Pv==1)
	{
		g_IsSnap2Pv=0;
	}
	else
	{
		UINT32 smode=sensorModeGet();
		if (smode==0x31 || smode==0x32)
			sensorSerialBulkWrite(cmdportid_tg,sizeof(cmd_720P60_code)/sizeof(cmd_720P60_code[0]), cmd_720P60_code[0],0);
		else
			sensorSerialBulkWrite(cmdportid_tg,sizeof(cmd_720P30_code)/sizeof(cmd_720P30_code[0]), cmd_720P30_code[0],0);
		
		sensorSyncSigSet(&sensorSyncSigPreview720PCfg);
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
#if DualSensor
		g_dualAE=0;
		if (smode==0x31 || smode==0x33)
		{
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_bypassA)/sizeof(cmd_zt_bypassA[0])), cmd_zt_bypassA[0],0);
#if DualAE
			sensorSyncSigDualSelSet(SP5K_DUAL_SENOR_PRI);
#endif
		}
		else{
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_bypassB)/sizeof(cmd_zt_bypassB[0])), cmd_zt_bypassB[0],0);
#if DualAE
			sensorSyncSigDualSelSet(SP5K_DUAL_SENOR_SEC);
#endif
		}
#endif
	}
	profLogAdd(1,"sensorDoFuncPreview720PBegin---");
}

static void sensorDoFuncPreviewFULLBegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	profLogAdd(1,"sensorDoFuncPreviewFULLBegin+++");
	sensorEventVvalidWait(1);
	
	if(g_IsSnap2Pv==1)
	{
		g_IsSnap2Pv=0;
	}
	else
	{
		UINT32 smode=sensorModeGet();
		sensorSerialBulkWrite(cmdportid_tg,sizeof(cmd_FULL_code)/sizeof(cmd_FULL_code[0]), cmd_FULL_code[0],0);
		sensorSyncSigSet(&sensorSyncSigPreviewFULLCfg);
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
#if DualSensor
		g_dualAE=0;
		if (smode==0x35)
		{
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_bypassA)/sizeof(cmd_zt_bypassA[0])), cmd_zt_bypassA[0],0);
#if DualAE
			sensorSyncSigDualSelSet(SP5K_DUAL_SENOR_PRI);
#endif
		}
		else
		{
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_bypassB)/sizeof(cmd_zt_bypassB[0])), cmd_zt_bypassB[0],0);
#if DualAE
			sensorSyncSigDualSelSet(SP5K_DUAL_SENOR_SEC);
#endif
		}
#endif
	}
	profLogAdd(1,"sensorDoFuncPreviewFULLBegin---");
}

static void sensorDoFuncPreviewShortExp()
{
	/*front_preview_short_exp_frame_set*/
}

static void sensorDoFuncPreviewExpEnd()
{
	/*front_preview_exp_end_frame_set*/
}

static void sensorDoFuncSnapEarly0()
{
}

static void sensorDoFuncSnapExpBegin()
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

	/*sensor_event_wait(SENSOR_EVENT_VVALID_END);*/
	/*sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<15, 1<<15);*/
	g_IsSnap2Pv=1;
	profLogAdd(1,"sensorDoFuncSnapExpBegin");
}

static void sensorDoFuncSnapShortExp()
{
	/*front_snap_short_exp_frame_set()*//*slow shutter, 1 longer frame*/
	/*front_snap_short_exp_tg_set*/
	/*do 1 longer frame exp, historical term "bshutter", if needed*/
}

static void sensorDoFuncSnapExpEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapExpEnd*/
	/*throw user msg??*/
	/*flash rearsync trig*/
	/*m-sh close trig*/
}

static void sensorDoFuncSnapReadoutField0()
{
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
	/*sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<15, 0);*/
	profLogAdd(1,"sensorDoFuncSnapReadoutField0");
}

static void sensorDoFuncSnapFieldEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapEnd()*/
	/*throw user msg??*/
	/*sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<15, 1<<15);*/
	profLogAdd(1,"sensorDoFuncSnapFieldEnd");
}

static void sensorDoFuncSnapReadoutEnd()
{
	/*panasonic TG TRG reset*/
	/*direct m-sh open*/
	/*direct vsubctl off*/
	/*immediate inform readout all end*/
	sensorReportsize();
	/*sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<15, 0);*/
	profLogAdd(1,"sensorDoFuncSnapReadoutEnd");
}

/*****TODO*****/
static void sensorCfgInit()
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
	/*zt registers for separate serial port*/
	static const sensorCmdport_t sensor_cmdport_zt_cfg = {
		.mode         = ZT_SPI_I2C_MODE,
		.format2      = TG_FORMAT2,
		.format3      = TG_FORMAT3,
		.speed        = TG_SPEED,
		.nbit_regdata = ZT_NBIT_REGDATA,
		.nbit_regaddr = ZT_NBIT_REGADDR,
	};
	printf("sensorCfgInit++\n");
	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);
	cmdportid_tg = ptcfg->cmdportid_xtg;
	cmdportid_zt = sensorSerialCreate(&sensor_cmdport_zt_cfg);
	/* if support burst
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_BURST; */
	/* if support seq
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_SEQ; */

	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_init();
	printf("sensorCfgInit--\n");
}

static void sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;

	printf("sensorCfgStop++\n");
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	printf("sensorCfgStop--\n");
}

void sec_sensor_delay_wr(void)
{
	profLogAdd(1,"sec_sensor_delay_wr");
	
	if(reg_exp[1].valid){

		sensorSerialAsyncWrite(cmdportid_tg,reg_exp[1].nreg, reg_exp[1].data);
		sensorSerialFlush(cmdportid_tg);
		reg_exp[1].valid=0;
		profLogAdd(1,"sec exp wr!!!");
	}

	if(reg_agc[1].valid){
		sensorSerialAsyncWrite(cmdportid_tg,reg_agc[1].nreg, reg_agc[1].data);
		sensorSerialFlush(cmdportid_tg);
		reg_agc[1].valid=0;
		profLogAdd(1,"sec agc wr!!!");
	}
	/*sensorSerialAsyncWrite(cmdportid_zt,1,cmd_zt_sensorA);*/
	sensorSerialAsyncWrite(cmdportid_zt,1,cmd_zt_sensorAB[0]);
	sensorSerialFlush(cmdportid_zt);

	g_Is_trigger=0;/*clear trigger flag*/
}

void pri_sensor_delay_wr(void)
{
	profLogAdd(1,"pri_sensor_delay_wr");

	if(reg_exp[0].valid){
		sensorSerialAsyncWrite(cmdportid_tg,reg_exp[0].nreg, reg_exp[0].data);
		sensorSerialFlush(cmdportid_tg);
		reg_exp[0].valid=0;
		profLogAdd(1,"pri exp wr!!!");
	}

	if(reg_agc[0].valid){
		sensorSerialAsyncWrite(cmdportid_tg,reg_agc[0].nreg, reg_agc[0].data);
		sensorSerialFlush(cmdportid_tg);
		reg_agc[0].valid=0;
		profLogAdd(1,"pri agc wr!!!");
	}
	sensorSerialAsyncWrite(cmdportid_zt,1,cmd_zt_sensorB[0]);
	sensorSerialFlush(cmdportid_zt);
	tmrWaitListInstall(2,1000,sec_sensor_delay_wr,0);/*1ms*/
}

void dual_sensor_wr_porc()
{
	if(g_Is_trigger)
		return;
	else
		g_Is_trigger=1;
	
	sensorSerialAsyncWrite(cmdportid_zt,1,cmd_zt_sensorA[0]);
	sensorSerialFlush(cmdportid_zt);
	profLogAdd(1,"dual_sensor_wr_porc");
	tmrWaitListInstall(2,2000,pri_sensor_delay_wr,0);/*2ms*/
}

static void sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
	/*****TODO******ITG fine exp set, value and timing???*/
	/*exp setting via serial port*/
	profLogPrintf(1,"exp1_ar0130: %x [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);
	if (g_dualAE)
	{
		reg_exp[0].valid=1;
		reg_exp[0].nreg=nreg;
		memcpy(reg_exp[0].data,pdata,nreg*4);
		dual_sensor_wr_porc();
		return;
	}
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
}

static void sensor_cmdport_exp2_write(UINT32 nreg, const UINT8 *pdata)
{
	/*****TODO******ITG fine exp set, value and timing???*/
	/*exp setting via serial port*/
	profLogPrintf(1,"exp2_ar0130: %x [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);
	if (g_dualAE)
	{
		reg_exp[1].valid=1;
		reg_exp[1].nreg=nreg;
		memcpy(reg_exp[1].data,pdata,nreg*4);
		dual_sensor_wr_porc();
		return;
	}
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
}

static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
}

#if SP5K_SENSOR_AR0130
void ZT3010CmdWrite(UINT8 regaddr, UINT8 regdata)
{
	static UINT8 reg[2];
	reg[0]=regaddr;
	reg[1]=regdata;
	sensorSerialBulkWrite(cmdportid_zt,1,reg,0);
	tmrUsWait(100000);/*100ms*/
	printf("WRITE [0x%x] = [0x%x]\n",reg[0],reg[1]);
}

void ZT3010CmdRead(UINT8 regaddr)
{
	static UINT8 reg[2];
	reg[0]=regaddr;
	sensorSerialBlockRead(cmdportid_zt,1,reg,0);
	tmrUsWait(100000);/*100ms*/
	printf("READ [0x%x] = [0x%x]\n",reg[0],reg[1]);
}

/*0:A formard, 1:A backward, 2:B forward, 3:B backward*/
void AR0130Rotate(UINT8 RotateStatus)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;

	switch(RotateStatus)
	{
		case 0:
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sensorA)/sizeof(cmd_zt_sensorA[0])), cmd_zt_sensorA[0],0);
			tmrUsWait(10000);/*10ms*/
			sensorSerialBulkWrite(cmdportid,sizeof(cmd_flipmirror_Restore)/sizeof(cmd_flipmirror_Restore[0]), cmd_flipmirror_Restore[0],0);
			break;
		case 1:
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sensorA)/sizeof(cmd_zt_sensorA[0])), cmd_zt_sensorA[0],0);
			tmrUsWait(10000);/*10ms*/
			sensorSerialBulkWrite(cmdportid,sizeof(cmd_flipmirror)/sizeof(cmd_flipmirror[0]), cmd_flipmirror[0],0);
			break;
		case 2:
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sensorB)/sizeof(cmd_zt_sensorB[0])), cmd_zt_sensorB[0],0);
			tmrUsWait(10000);/*10ms*/
			sensorSerialBulkWrite(cmdportid,sizeof(cmd_flipmirror_Restore)/sizeof(cmd_flipmirror_Restore[0]), cmd_flipmirror_Restore[0],0);
			break;
		case 3:
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sensorB)/sizeof(cmd_zt_sensorB[0])), cmd_zt_sensorB[0],0);
			tmrUsWait(10000);/*10ms*/
			sensorSerialBulkWrite(cmdportid,sizeof(cmd_flipmirror)/sizeof(cmd_flipmirror[0]), cmd_flipmirror[0],0);
			break;
		default:
			sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sensorAB)/sizeof(cmd_zt_sensorAB[0])), cmd_zt_sensorAB[0],0);
			break;
	}

	if (RotateStatus<4)
	{
		g_rotate=RotateStatus;
		tmrUsWait(10000);/*10ms*/
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_DisColumnCorrection)/sizeof(cmd_DisColumnCorrection[0]), cmd_DisColumnCorrection[0],0);
		tmrUsWait(50000);/*50ms*/
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_EnColumnCorrection)/sizeof(cmd_EnColumnCorrection[0]), cmd_EnColumnCorrection[0],0);
		tmrUsWait(10000);/*10ms*/
		sensorSerialBulkWrite(cmdportid_zt,(sizeof(cmd_zt_sensorAB)/sizeof(cmd_zt_sensorAB[0])), cmd_zt_sensorAB[0],0);
	}
}

void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regData[][4] ={
		{0x30,0x12,0x00,0x00}
	};
	regData[0][2] = (expLine>>8)&0xff;
	regData[0][3] = (expLine)&0xff;

	profLogPrintf(1,"expLine = %d [%x %x %x %x]",expLine,regData[0][0],regData[0][1],regData[0][2],regData[0][3]);
	sensorEventNewFrameWait(1);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,1, regData[0]);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
}
#endif

