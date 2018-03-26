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
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/

/*#define EVTBL_TEST_CODE 1
#define SNESOR_API_EN 1
#define PREVIEW_XGA_15FPS 1
 
 
*/ 
#define SNESOR_API_EN 1
#define READ16(_reg_)           (*((volatile UINT16 *)(_reg_)))
#define DBG_PRINT(str,args...) profLogPrintf(1,str,##args)

#define MODEL_VERSION "$unp_tg_ov5653-1.00.02"
#define BUILD_TIME        __DATE__  "-"  __TIME__

/*HISTORY
 * 1.00.02 20071221 anmin
 *   updating installed struct
 * prerelease
 *   modify code for the preview initial for switch XGA 06/06/20
 *   modify code for the VGA coverage ratio 06/05/22
 *   modify code for the SOP 06/03/27
 *   Ver0A 060220 add snap_cfa_pattern = BAYER_RGGB,halFrontImgSrcCfgSet(FRONT_IMG_SENSOR_FIELD_SEL,0x01);modify I2C SPI slave address,
 */
#include <stdio.h>
#include "common.h"
#include "../flow/sysregs.h"
#include "../flow/hal_front.h"
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
#include "api/sp5k_global_api.h"
#include "../flow/sensor_model_snap.h"
#include "middleware/proflog.h"



/*#define _EVB_SPE7001_HW_ 1 */
#define _CATHY_HW_ 1

/* Sensor image size definition here */
#define NO             0
#define BY_PIXEL       1
#define BY_HD          0
#define NOUSE          0
#define DISABLE        0xFFFF

#define BIT0 (1<<0)
#define BIT1 (1<<1)
#define BIT2 (1<<2)
#define BIT3 (1<<3)
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT9 (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)


#define BIT29 (1<<29)
static UINT32 g_IsSnap2Pv;   
static BOOL   fpvsusp;

#ifdef _EVB_SPE7001_HW_
#define SENSOR_FREX     BIT6
#define SENSOR_PWRDN    BIT0
#define SENSOR_RESET    BIT9

#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#endif

#ifdef _CATHY_HW_
#define SENSOR_FREX     BIT0
#define SENSOR_PWRDN    BIT10
#define SENSOR_RESET    BIT4

#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#endif


/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HSIZE  640		/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  480		/* preview vertical size*/

#define PREVIEW_VGA_120FPS_HSIZE  640		/* preview horiz. size*/
#define PREVIEW_VGA_120FPS_VSIZE  480		/* preview vertical size*/

#define PREVIEW_XGA_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  960		/* preview vertical size*/

#define PREVIEW_720_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_720_VSIZE  720		/* preview vertical size*/

#define PREVIEW_720_60FPS_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_720_60FPS_VSIZE  720		/* preview vertical size*/

#define PREVIEW_1080_HSIZE  (1920)		/* preview horiz. size*/
#define PREVIEW_1080_VSIZE  (1080)		/* preview vertical size*/

#define CAPTURE_HSIZE  (2592)		/* capture horiz. size*/
#define CAPTURE_VSIZE  (1944)		/* capture vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL   1066	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   426		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  750		/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  262		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     (426+13)  /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     2		/* preview Vd valid line data offset position*/

#define PREVIEW_VGA_120FPS_LINETOTAL   1066	/*1587*/	/* preview Hd length*/
#define PREVIEW_VGA_120FPS_LINEBLANK   426	/*900*/		/* preview Hd sync length*/
#define PREVIEW_VGA_120FPS_FRAMETOTAL  750	/*504*/		/* preview Vd length*/
#define PREVIEW_VGA_120FPS_FRAMEBLANK  262	/*10*/		/* preview Vd sync length*/
#define PREVIEW_VGA_120FPS_HOFFSET     (426+13)		/*941*/  /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_120FPS_VOFFSET     2		/* preview Vd valid line data offset position*/

#define PREVIEW_XGA_LINETOTAL   2161	/* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   848 	/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL  1018  	/* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  42	/* preview Vd sync length*/
#define PREVIEW_XGA_HOFFSET     (881+22)		 /* preview Hd valid pixel data offset position*/
#define PREVIEW_XGA_VOFFSET     2      	/* preview Vd valid line data offset position*/

#define PREVIEW_720_LINETOTAL   2150	/* preview Hd length*/
#define PREVIEW_720_LINEBLANK   848		/* preview Hd sync length*/
#define PREVIEW_720_FRAMETOTAL  744 /* preview Vd length*/
#define PREVIEW_720_FRAMEBLANK  20	/* preview Vd sync length*/
#define PREVIEW_720_HOFFSET     (870+14)   /* preview Hd valid pixel data offset position*/
#define PREVIEW_720_VOFFSET     2      /* preview Vd valid line data offset position*/

#define PREVIEW_720_60FPS_LINETOTAL   2150	/* preview Hd length*/ 
#define PREVIEW_720_60FPS_LINEBLANK   848		/* preview Hd sync length*/
#define PREVIEW_720_60FPS_FRAMETOTAL  744 /* preview Vd length*/
#define PREVIEW_720_60FPS_FRAMEBLANK  20	/* preview Vd sync length*/
#define PREVIEW_720_60FPS_HOFFSET     (870+14)   /* preview Hd valid pixel data offset position*/
#define PREVIEW_720_60FPS_VOFFSET     2      /* preview Vd valid line data offset position*/

#define PREVIEW_1080_LINETOTAL   2898  /* preview Hd length*/
#define PREVIEW_1080_LINEBLANK   990    /* preview Hd sync length*/
#define PREVIEW_1080_FRAMETOTAL  1104 /* preview Vd length*/
#define PREVIEW_1080_FRAMEBLANK  10	/* preview Vd sync length*/
#define PREVIEW_1080_HOFFSET     (794)    /* preview Hd valid pixel data offset position*/
#define PREVIEW_1080_VOFFSET     2       /* preview Vd valid line data offset position*/

/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/ 
#define PREVIEW_VGA_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/

/* Preview resh_VGA_120fps sync signal definition here */
#define PREVIEW_VGA_120FPS_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/ 
#define PREVIEW_VGA_120FPS_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_VGA_120FPS_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_VGA_120FPS_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_120FPS_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/

/* Preview resh_XGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/
                                
/* Preview resh_720_pe sync signal definition here */
#define PREVIEW_720_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_720_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_720_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_720_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_720_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/

/* Preview resh_720_60fps sync signal definition here */
#define PREVIEW_720_60FPS_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_720_60FPS_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_720_60FPS_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_720_60FPS_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_720_60FPS_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/

/* Preview resh_1080_pe sync signal definition here */
#define PREVIEW_1080_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_1080_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_1080_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_1080_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_1080_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/


/* Capture default definition here */
#define CAPTURE_LINETOTAL   3252	/* capture Hd length*/
#define CAPTURE_LINEBLANK   660		/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  1984	/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  40		/* capture Vd sync length*/

#define CAPTURE_HOFFSET     682    /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     2    	/* capture Vd valid line data offset position*/

/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         2	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         4	/* capture Hd reshap rise*/

#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_PIXEL /* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL		  DISABLE	/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_VRESHAPE_RISE		  DISABLE	/* capture Vd reshap rise , count by pixel*/  
    
/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV     1        /* mclk drivng, '1' : 8mA, '0' : 4mA */
#define MCLKMODE	1	     /*mclk mode,'0'from I/O Pad,'1'from internal*/
#define PCLKMODE	0	     /*pclk mode,'0'from I/O Pad,'1'from internal*/
#define TGPLLEN   	1	     /*enable TGPLL,'0'disable,'1'enable*/
#define TGPLLCLKSET	8	     /*TGPLL clk 0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,
					5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/
#define MCLKDIV		4	     /*MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV		2	     /*pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE	0     	     /*master clk phase*/
#define PIXELPHASE	2	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/

#define PREVIEW_VGA_PCLK       960
#define PREVIEW_VGA_120FPS_PCLK       960
#define PREVIEW_XGA_PCLK        960
#define PREVIEW_720_PCLK        960
#define PREVIEW_720_60FPS_PCLK    960
#define PREVIEW_1080_PCLK       960
#define CAPTURE_PCLK            	    960
/*
#define PREVIEW_VGA_PCLK       24000
#define PREVIEW_VGA_120FPS_PCLK       72000
#define PREVIEW_XGA_PCLK        66000
#define PREVIEW_720_PCLK        48000
#define PREVIEW_720_60FPS_PCLK    96000
#define PREVIEW_1080_PCLK       96000
#define CAPTURE_PCLK            	    96000
*/
#ifdef FPGA
#define RGBADJ		1	     /*RGB input data sample phase adj "1"negedge "0"posedge*/
#else
#define RGBADJ		0	     /*RGB input data sample phase adj "1"negedge "0"posedge*/
#endif
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

#define   PREVIEW_VGA_CFAPATTERN  BAYER_BGGR
#define   PREVIEW_VGA_120FPS_CFAPATTERN  BAYER_BGGR						      
#define	PREVIEW_XGA_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define   PREVIEW_720_CFAPATTERN  BAYER_BGGR
#define   PREVIEW_720_60FPS_CFAPATTERN  BAYER_BGGR
#define   PREVIEW_1080_CFAPATTERN  BAYER_BGGR
#define	SNAP_CFAPATTERN		BAYER_BGGR         /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define	SNAP_DUMMYVD_NUM	0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	1	            /*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		        1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		2	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/

#define	PREVIEW_HDNUM_FRAME_START  11		/*the front interrupt line number of the preview new frame*/
#define	CAPTURE_HDNUM_FRAME_START	(11+24)	/*the front interrupt line number of the snap new frame */

#define	PREVIEW_VGA_HDNUM_FRAME_END	(PREVIEW_VGA_VOFFSET+PREVIEW_VGA_VSIZE+1)		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT		PREVIEW_VGA_HDNUM_FRAME_END-50		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_120FPS_HDNUM_FRAME_END	(PREVIEW_VGA_120FPS_VOFFSET+PREVIEW_VGA_120FPS_VSIZE+1)		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_120FPS_HDNUM_CMDPORT		PREVIEW_VGA_120FPS_HDNUM_FRAME_END-50		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_FRAME_END	(PREVIEW_XGA_VOFFSET+PREVIEW_XGA_VSIZE+1) 		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_CMDPORT		PREVIEW_XGA_HDNUM_FRAME_END-50		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_720_HDNUM_FRAME_END	(PREVIEW_720_VOFFSET+PREVIEW_720_VSIZE+1)  	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_720_HDNUM_CMDPORT		PREVIEW_720_HDNUM_FRAME_END-50		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_720_60FPS_HDNUM_FRAME_END	(PREVIEW_720_60FPS_VOFFSET+PREVIEW_720_60FPS_VSIZE+1)  	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_720_60FPS_HDNUM_CMDPORT		PREVIEW_720_60FPS_HDNUM_FRAME_END-50		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_1080_HDNUM_FRAME_END	(PREVIEW_1080_VOFFSET+PREVIEW_1080_VSIZE+1)  	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080_HDNUM_CMDPORT		PREVIEW_1080_HDNUM_FRAME_END-50	/*the command port interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_END	(CAPTURE_VOFFSET+CAPTURE_VSIZE+1)    	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT		CAPTURE_HDNUM_FRAME_END-50   	/*the command port interrupt line number of the snap new frame*/

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

#if SPCA5210
#define	TG_SPEED         FRONT_SSC_CLK_384K
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_192K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */

#define CAP_FRAME_MODE  0

extern UINT32 sensor_event_wait(UINT32 event);
extern UINT32 sensorModeGet(void);

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensorDoPowerSave(UINT32 mode);
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreviewVGA120FPSBegin();
static void sensorDoFuncPreviewXGABegin();
static void sensorDoFuncPreview720PBegin();
static void sensorDoFuncPreview720P60FPSBegin();
static void sensorDoFuncPreview1080Begin();

static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
/*
static void sensorDoFuncSnapReadoutField1();
static void sensorDoFuncSnapReadoutField2();
*/
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();

static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapEarly1();


static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();

static UINT32 g_last_exp_line;
static UINT32 g_last_smode = 0x00;
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
	.pix_100khz =PREVIEW_VGA_PCLK,
	.hratio=640,
	.vratio=480,
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGA120FPSCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGA120FPSBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_VGA_120FPS_HSIZE,
	.vsize = PREVIEW_VGA_120FPS_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_VGA_120FPS_CFAPATTERN,
	.pix_100khz =PREVIEW_VGA_120FPS_PCLK,
	.hratio=640,
	.vratio=480,
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
	.crop = &( const sensorTgCrop_t ) {
			.xOffset = 0,
			.yOffset = 0,
			.xSize = PREVIEW_XGA_HSIZE,
			.ySize = PREVIEW_XGA_VSIZE,
		},
	.hsize = PREVIEW_XGA_HSIZE,
	.vsize = PREVIEW_XGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_XGA_CFAPATTERN,
	.pix_100khz =PREVIEW_XGA_PCLK,
	.hratio=4000,
	.vratio=3000,
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
	.hsize = PREVIEW_720_HSIZE,
	.vsize = PREVIEW_720_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_720_CFAPATTERN,
	.pix_100khz =PREVIEW_720_PCLK,
	.hratio=1280,
	.vratio=720,
};

static const sensorTgPreviewCfg_t sensorTgPreview720P60FPSCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720P60FPSBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_720_60FPS_HSIZE,
	.vsize = PREVIEW_720_60FPS_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_720_60FPS_CFAPATTERN,
	.pix_100khz =PREVIEW_720_60FPS_PCLK,
	.hratio=1280,
	.vratio=720,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_1080_HSIZE,
	.vsize = PREVIEW_1080_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_1080_CFAPATTERN,
	.pix_100khz =PREVIEW_1080_PCLK,
	.hratio=1920,
	.vratio=1080,
};


static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	  &sensorTgPreviewXGACfg,
	  &sensorTgPreview1080Cfg,
	  &sensorTgPreview720PCfg, 
	  &sensorTgPreview720P60FPSCfg,
	  &sensorTgPreviewVGA120FPSCfg,
	  &sensorTgPreviewVGACfg,
};


static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
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
	.pix_100khz= CAPTURE_PCLK,
	.hratio=4000,
	.vratio=3000,
};

/****TODO****frame snap mode*/
static const sensorTgSnapCfg_t sensorTgSnapFrameCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0,
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
	.pix_100khz= CAPTURE_PCLK,
};

#if 0
static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapFrameCfg, &sensorTgSnapRollCfg, 
};
#else
static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg, 
};
#endif

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
	.lastlinepix     = PREVIEW_VGA_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_VGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_VGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVGA120FPSCfg = {
	.linetotal       = PREVIEW_VGA_120FPS_LINETOTAL,
	.lineblank       = PREVIEW_VGA_120FPS_LINEBLANK,
	.frametotal      = PREVIEW_VGA_120FPS_FRAMETOTAL,
	.frameblank      = PREVIEW_VGA_120FPS_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_VGA_120FPS_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_VGA_120FPS_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_VGA_120FPS_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_VGA_120FPS_VRESHAPE_RISE,
	.hoffset         = PREVIEW_VGA_120FPS_HOFFSET,
	.voffset         = PREVIEW_VGA_120FPS_VOFFSET,
	.hsize           = PREVIEW_VGA_120FPS_HSIZE,
	.vsize           = PREVIEW_VGA_120FPS_VSIZE,
	.lastlinepix     = PREVIEW_VGA_120FPS_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_120FPS_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_VGA_120FPS_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_VGA_120FPS_HDNUM_CMDPORT,
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
	.lastlinepix     = PREVIEW_XGA_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_XGA_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_XGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_XGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview720Cfg = {
	.linetotal       = PREVIEW_720_LINETOTAL,
	.lineblank       = PREVIEW_720_LINEBLANK,
	.frametotal      = PREVIEW_720_FRAMETOTAL,
	.frameblank      = PREVIEW_720_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_720_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_720_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_720_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_720_VRESHAPE_RISE,
	.hoffset         = PREVIEW_720_HOFFSET,
	.voffset         = PREVIEW_720_VOFFSET,
	.hsize           = PREVIEW_720_HSIZE,
	.vsize           = PREVIEW_720_VSIZE,
	.lastlinepix     = PREVIEW_720_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_720_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_720_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_720_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview720P60FPSCfg = {
	.linetotal       = PREVIEW_720_60FPS_LINETOTAL,
	.lineblank       = PREVIEW_720_60FPS_LINEBLANK,
	.frametotal      = PREVIEW_720_60FPS_FRAMETOTAL,
	.frameblank      = PREVIEW_720_60FPS_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_720_60FPS_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_720_60FPS_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_720_60FPS_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_720_60FPS_VRESHAPE_RISE,
	.hoffset         = PREVIEW_720_60FPS_HOFFSET,
	.voffset         = PREVIEW_720_60FPS_VOFFSET,
	.hsize           = PREVIEW_720_60FPS_HSIZE,
	.vsize           = PREVIEW_720_60FPS_VSIZE,
	.lastlinepix     = PREVIEW_720_60FPS_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_720_60FPS_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_720_60FPS_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_720_60FPS_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080Cfg = {
	.linetotal       = PREVIEW_1080_LINETOTAL,
	.lineblank       = PREVIEW_1080_LINEBLANK,
	.frametotal      = PREVIEW_1080_FRAMETOTAL,
	.frameblank      = PREVIEW_1080_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_1080_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_1080_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_1080_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_1080_VRESHAPE_RISE,
	.hoffset         = PREVIEW_1080_HOFFSET,
	.voffset         = PREVIEW_1080_VOFFSET,
	.hsize           = PREVIEW_1080_HSIZE,
	.vsize           = PREVIEW_1080_VSIZE,
	.lastlinepix     = PREVIEW_1080_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_1080_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_1080_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_1080_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
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
	.lastlinepix     = CAPTURE_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_yuvhdpol   =0, 
	.conf_yuvvdpol   =0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = CAPTURE_VRESHAPE_COUNTER_SEL,
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
		.pwrsave = sensorDoPowerSave, /*install power save function*/
		.expwrite = sensor_cmdport_exp_write,
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
		.crop = NULL, /****TODO***/
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreviewXGACfg,
	.verStr = MODEL_VERSION,
};

void
sensorTg_ov5653_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

static void
TGStartUpProc(void)
{
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,0x00000000); /* PWD Low*/
	tmrUsWait(25000); /*25ms*/
	return;

	/****TODO****sensor power*/
		
		sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0xFFFFFFFF);
		tmrUsWait(10000);  /*10ms*/
		tmrUsWait(10000);  /*10ms*/
		tmrUsWait(10000);  /*10ms*/		
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0x00000000);
		tmrUsWait(1000);  /*1ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,0xFFFFFFFF); /* PWD High*/
		tmrUsWait(10000);  /*10ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,0x00000000); /* PWD Low*/
		tmrUsWait(1000);  /*1ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0xFFFFFFFF);

		tmrUsWait(25000); /*25ms*/

		printf(" 5653 start on sensor \n");
	
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX,0x00000000);
	
}


/*------method definitions-----*/
		
static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {		

	{0x30, 0x08, 0x82},
	{0x30, 0x08, 0x42},
	{0x31, 0x03, 0x93},
	{0x3b, 0x07, 0x0c},
	{0x30, 0x17, 0xff},
	{0x30, 0x18, 0xfc},
	{0x37, 0x06, 0x41},
	{0x37, 0x03, 0xe6},
	{0x36, 0x13, 0x44},
	{0x36, 0x30, 0x22},
	{0x36, 0x05, 0x04},
	{0x36, 0x06, 0x3f},
	{0x37, 0x12, 0x13},
	{0x37, 0x0e, 0x00},
	{0x37, 0x0b, 0x40},
	{0x36, 0x00, 0x54},
	{0x36, 0x01, 0x05},
	{0x37, 0x13, 0x22},
	{0x37, 0x14, 0x27},
	{0x36, 0x31, 0x22},
	{0x36, 0x12, 0x1a},
	{0x36, 0x04, 0x40},
	{0x37, 0x05, 0xda},
	{0x37, 0x0a, 0x80},
	{0x37, 0x0c, 0x00},
	{0x37, 0x10, 0x28},
	{0x37, 0x02, 0x3a},
	{0x37, 0x04, 0x18},
	{0x3a, 0x18, 0x00},
	{0x3a, 0x19, 0xf8},
	{0x3a, 0x00, 0x38},
	{0x38, 0x00, 0x02},
	{0x38, 0x01, 0x54},
	{0x38, 0x03, 0x0c},
	{0x38, 0x0c, 0x0c},
	{0x38, 0x0d, 0xb4},
	{0x38, 0x0e, 0x07},
	{0x38, 0x0f, 0xb0},
	{0x38, 0x30, 0x50},
	{0x3a, 0x08, 0x12},
	{0x3a, 0x09, 0x70},
	{0x3a, 0x0a, 0x0f},
	{0x3a, 0x0b, 0x60},
	{0x3a, 0x0d, 0x06},
	{0x3a, 0x0e, 0x06},
	{0x3a, 0x13, 0x54},
	{0x38, 0x15, 0x82},
	{0x50, 0x59, 0x80},
	{0x36, 0x15, 0x52},
	{0x50, 0x5a, 0x0a},
	{0x50, 0x5b, 0x2e},
	{0x3a, 0x1a, 0x06},
	{0x35, 0x03, 0x00},
	{0x36, 0x23, 0x01},
	{0x36, 0x33, 0x24},
	{0x3c, 0x01, 0x34},
	{0x3c, 0x04, 0x28},
	{0x3c, 0x05, 0x98},
	{0x3c, 0x07, 0x07},
	{0x3c, 0x09, 0xc2},
	{0x40, 0x00, 0x05},
	{0x40, 0x1d, 0x28},
	{0x40, 0x01, 0x02},
	{0x40, 0x1c, 0x46},
	{0x50, 0x46, 0x01},
	{0x38, 0x10, 0x40},
	{0x38, 0x36, 0x41},
	{0x50, 0x5f, 0x04},
	{0x50, 0x00, 0x00},
	{0x50, 0x01, 0x00},
	{0x50, 0x02, 0x00},
	{0x50, 0x3d, 0x00},
	{0x59, 0x01, 0x00},
	{0x58, 0x5a, 0x01},
	{0x58, 0x5b, 0x2c},
	{0x58, 0x5c, 0x01},
	{0x58, 0x5d, 0x93},
	{0x58, 0x5e, 0x01},
	{0x58, 0x5f, 0x90},
	{0x58, 0x60, 0x01},
	{0x58, 0x61, 0x0d},
	{0x51, 0x80, 0xc0},
	{0x51, 0x84, 0x00},
	{0x47, 0x0a, 0x00},
	{0x47, 0x0b, 0x00},
	{0x47, 0x0c, 0x00},
	{0x30, 0x0f, 0x8e},
	{0x36, 0x03, 0xa7},
	{0x36, 0x32, 0x55},
	{0x36, 0x20, 0x56},
	{0x36, 0x21, 0x2f},
	{0x38, 0x1a, 0x3c},
	{0x38, 0x18, 0xc0},
	{0x36, 0x31, 0x36},
	{0x36, 0x32, 0x5f},
	{0x37, 0x11, 0x24},

	/*Sensor Original setting.*/
	/*
	{0x36, 0x13, 0xc4},
	{0x36, 0x21, 0xaf},*//*binning (H binning/sub-sample enable ) */

	/*Modify the 0x3613 bit 7 and 0x3621 bit6 to use vertical/horizontal binning average.*/
	{0x36, 0x13, 0x44},/*bit 7 , 1: vertical binning sum, 0: vertical binning average.*/
	{0x36, 0x21, 0xef},/*bit 6 , 1: horizontal binning average, 0: horizontal binning sum.*/
	
	{0x37, 0x03, 0x9a},
	{0x37, 0x05, 0xdc},
	{0x37, 0x0a, 0x81},
	{0x37, 0x0c, 0xc8},
	{0x37, 0x0d, 0x42},
	{0x37, 0x13, 0x92},
	{0x37, 0x14, 0x17}, 

	/* VGA resolution */ 

     /*isp top  */
	{0x50, 0x02, 0x02},/* vap (enable) */
	{0x59, 0x01, 0x04},/* vap (sub-sample 2) */

       /*blc  */
	{0x40, 0x1c, 0x42},/*rsvd */

	{0x40, 0x1f, 0x03},
	{0x30, 0x08, 0x02},

	/* PCLK=96MHZ  */
	{0x30, 0x0e, 0x18},
	{0x30, 0x0f, 0x8e},
	
	{0x30, 0x10, 0x10},
	{0x30, 0x11, 0x08},	/* 10 = 96MHz, 08 = 48MHz */
	{0x30, 0x12, 0x02},
	/* disable AGC/AEC */
	{0x35, 0x03, 0x17},
	{0x35, 0x00, 0x00},
	{0x35, 0x01, 0x10},
	{0x35, 0x02, 0x00},
	{0x35, 0x0a, 0x00},
	{0x35, 0x0b, 0x10},
	/* disable AWB */
	{0x34, 0x06, 0x01},
	{0x34, 0x00, 0x04},
	{0x34, 0x01, 0x00},
	{0x34, 0x02, 0x04},
	{0x34, 0x03, 0x00},
	{0x34, 0x04, 0x04},
	/*{0x30, 0x30, 0x20},*/
	{0x30, 0x30, 0x00},

	{0x47, 0x00, 0x01},  /*HSYNC mode enable*/
	/*{0x47, 0x08, 0x0a}, */ /* V_SYNC WIDTH */

	{0x47, 0x01, 0x0a},  /* V_SYNC WIDTH */


	{0x38, 0x00, 0x02},/*HS 596 */
	{0x38, 0x01, 0x12},/*HS */

	{0x38, 0x02, 0x00},/*VS 28*/
	{0x38, 0x03, 0x04},/*VS */

	{0x38, 0x04, 0x05},/*HW 1280 */
	{0x38, 0x05, 0x00},/*HW */

	{0x38, 0x06, 0x03},/*VH 976 */
	{0x38, 0x07, 0xd0},/*VH */

	{0x38, 0x08, 0x05},/*DVP H output size 1280*/
	{0x38, 0x09, 0x00},/*DVP H output size */

	{0x38, 0x0a, 0x03},/*DVP V output size 976 */
	{0x38, 0x0b, 0xd0},/*DVP V output size */

	{0x38, 0x0C, 0x08},/*THS 2132*/
	{0x38, 0x0D, 0x54},/*THS */

	{0x38, 0x0E, 0x03},/*TVS 1018 */
	{0x38, 0x0F, 0xfa},/*TVS */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},
		
	};
	
	profLogAdd(0, __FUNCTION__);
	profLogPrintf(0, "________Sensor driver ver <<%s>>", MODEL_VERSION);
	profLogPrintf(0, "________Sensor driver build time <<%s>>", BUILD_TIME);

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	sensorTgPllClkExtSet(24000000);/*Lowest input clock provided from ASIC is 16MHz*/

	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN,1,SENSOR_TGPLL_DRV_8MA,NOUSE,NOUSE);
	halFrontClkCfg(MCLKMODE, PCLKMODE);

	/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);

	#ifdef _EVB_SPE7001_HW_
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	#endif
	
	TGStartUpProc();

	#if SPCA5210
/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
	#endif

	/*****TODO*****front reset after these???*/
	tmrUsWait(2000);

	cmdportid = sensorTgCfg.cmdportid_xtg;

	sensorSerialAsyncWrite(cmdportid, 1, regdata[0]);  /* Sensor Soft Reset */
	sensorSerialFlush(cmdportid);
	tmrUsWait(5000);  /* delay 100 ms for reset*/

	sensorSerialBulkWrite(cmdportid,
		(sizeof(regdata)/sizeof(regdata[0])) - 1, regdata[1],0);	
	
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);	
	tmrUsWait(10000);  /* delay 100 ms for reset*/
}

static void
sensorDoFuncStop()
{
}

static const UINT8 regDataBlc[][3] = {
	{0x40, 0x1d, 0x28},
};

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
	static const UINT8 regdata[][3] = {

/********NOT READY ******************/

	/*;--------------------------------
	   ;	640x480 30fps key setting	
	   ;	input	:	24MHZ
	   ;	output	:	24MHZ
	   ;	fps		:	30fps
	   ; 	HTS		: 	1066
	   ; 	VTS		:	750
	   ;-------------------------------- */

#if 1
	{0x30, 0x10, 0x10},
	{0x30, 0x11, 0x08},		/*PCLK  0x08 = 48MHz, 0x0b = 66MHz */ 
	/* group start */
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},

	/* resolution */
	/*Sensor Original setting.*/
	/*{0x36, 0x13, 0xc4},
	{0x36, 0x21, 0xaf},*/

	/*Modify the 0x3613 bit 7 and 0x3621 bit6 to use vertical/horizontal binning average.*/
	{0x36, 0x13, 0x44},/*bit 7 , 1: vertical binning sum, 0: vertical binning average.*/
	{0x36, 0x21, 0xef},/*bit 6 , 1: horizontal binning average, 0: horizontal binning sum.*/
	
	{0x37, 0x03, 0x9a},
	{0x37, 0x05, 0xdc},
	{0x37, 0x0a, 0x81},
	{0x37, 0x0c, 0xc8},
	{0x37, 0x0d, 0x42},
	{0x37, 0x13, 0x92},
	{0x37, 0x14, 0x17}, 

	{0x38, 0x00, 0x02},/*HS 594 */
	{0x38, 0x01, 0x54},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0a},/*VS */

	{0x38, 0x04, 0x05},/*HW 1280 */
	{0x38, 0x05, 0x00},/*HW */

	{0x38, 0x06, 0x01},/*VH 480 */
	{0x38, 0x07, 0xe8},/*VH */

	{0x38, 0x08, 0x02},/*DVP H output size 640*/
	{0x38, 0x09, 0x80},/*DVP H output size */

	{0x38, 0x0a, 0x01},/*DVP V output size 480 */
	{0x38, 0x0b, 0xe8},/*DVP V output size */

	{0x38, 0x0C, 0x08},/*THS 2132*/
	{0x38, 0x0D, 0x54},/*THS */

	{0x38, 0x0E, 0x02},/*TVS 750 */
	{0x38, 0x0F, 0xee},/*TVS */

	{0x38, 0x18, 0xc2},
	{0x38, 0x1a, 0x00},
	{0x38, 0x1c, 0x20},
	{0x38, 0x1d, 0x0a},
	{0x38, 0x1e, 0x01},
	{0x38, 0x1f, 0x20},
	{0x38, 0x20, 0x00},
	{0x38, 0x21, 0x00},
	{0x38, 0x24, 0x23},
	{0x38, 0x25, 0x20},
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x08},	

	{0x50, 0x02, 0x02},
	{0x59, 0x01, 0x04},

	{0x40, 0x1c, 0x42},
	{0x40, 0x1d, 0x08},

	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},

#endif

	};

	profLogAdd(0, "Sensor VGA init");
	if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
    g_IsSnap2Pv=0;
    fpvsusp=1;		
	sensor_event_wait(0x04);

	sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	sensorSerialFlush(cmdportid);
	sensor_event_wait(0x04);
	sensorSerialAsyncWrite(cmdportid, sizeof(regDataBlc)/sizeof(regDataBlc[0]), regDataBlc[0]);
	sensorSerialFlush(cmdportid);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	printf(" \033[32mOV5653 VGA  init \033[0m\n");
	
}

static void
sensorDoFuncPreviewVGA120FPSBegin() /*OV5653 sensor VGA mode max frame rate is 90fps*/
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {

/********NOT READY ******************/

	/*;--------------------------------
	   ;	640x480 30fps key setting	
	   ;	input	:	24MHZ
	   ;	output	:	72MHZ
	   ;	fps		:	90fps
	   ; 	HTS		: 	1066
	   ; 	VTS		:	750
	   ;-------------------------------- */

#if 0
	{0x30, 0x10, 0x30},		/*24Mhz , 30fps*/
	{0x30, 0x11, 0x10},		
#else
	{0x30, 0x10, 0x00},	/*72Mhz , 90fps*/
	{0x30, 0x11, 0x0c}, 	
#endif
	#if 1
	/* group start */
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},

	/* resolution */

	/*Sensor Original setting.*/
	/*
	{0x36, 0x13, 0xc4},
	{0x36, 0x21, 0xaf},*//* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */

	/*Modify the 0x3613 bit 7 and 0x3621 bit6 to use vertical/horizontal binning average.*/
	{0x36, 0x13, 0x44},/*bit 7 , 1: vertical binning sum, 0: vertical binning average.*/
	{0x36, 0x21, 0xef},/*bit 6 , 1: horizontal binning average, 0: horizontal binning sum.*/
	
	{0x37, 0x03, 0x9a},
	{0x37, 0x05, 0xdc},
	{0x37, 0x0a, 0x81},
	{0x37, 0x0c, 0xc8},
	{0x37, 0x0d, 0x42},/* for v binning on [6] = 1 */
	{0x37, 0x13, 0x92},
	{0x37, 0x14, 0x17}, 

	{0x38, 0x00, 0x02},/*HS 594 */
	{0x38, 0x01, 0x54},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0a},/*VS */

	{0x38, 0x04, 0x05},/*HW 1280 */
	{0x38, 0x05, 0x00},/*HW */

	{0x38, 0x06, 0x01},/*VH 480 */
	{0x38, 0x07, 0xe8},/*VH */

	{0x38, 0x08, 0x02},/*DVP H output size 640*/
	{0x38, 0x09, 0x80},/*DVP H output size */

	{0x38, 0x0a, 0x01},/*DVP V output size 480 */
	{0x38, 0x0b, 0xe8},/*DVP V output size */

	{0x38, 0x0C, 0x08},/*THS 2132*/
	{0x38, 0x0D, 0x54},/*THS */

	{0x38, 0x0E, 0x02},/*TVS 750 */
	{0x38, 0x0F, 0xee},/*TVS */

	{0x38, 0x18, 0xc2},/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp on [5] */
	{0x38, 0x1a, 0x00},
	{0x38, 0x1c, 0x20},
	{0x38, 0x1d, 0x0a},
	{0x38, 0x1e, 0x01},
	{0x38, 0x1f, 0x20},
	{0x38, 0x20, 0x00},
	{0x38, 0x21, 0x00},
	{0x38, 0x24, 0x23},
	{0x38, 0x25, 0x20},
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x08}, 

	{0x50, 0x02, 0x02},
	{0x59, 0x01, 0x04},/*H sub-sample coefficient , bit[3:2] , 00:sub-sample 1, 01:sub-sample 2x , 1x:sub-sample 4x*/

	{0x40, 0x1c, 0x42},
	{0x40, 0x1d, 0x08},

	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},
	
	#else
	/* group start */
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},

	/* resolution */
	{0x36, 0x13, 0xc4},
	{0x36, 0x21, 0xaf},	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	{0x37, 0x03, 0x9a},
	{0x37, 0x05, 0xdc},
	{0x37, 0x0a, 0x81},
	{0x37, 0x0c, 0xc8},
	{0x37, 0x0d, 0x42},	/* for v binning on [6] = 1 */
	{0x37, 0x13, 0x92},
	{0x37, 0x14, 0x17}, 

	{0x38, 0x00, 0x02},/*HS 594 */
	{0x38, 0x01, 0x22},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0a},/*VS */

	{0x38, 0x04, 0x05},/*HW 1280 */
	{0x38, 0x05, 0x10},/*HW */

	{0x38, 0x06, 0x01},/*VH 480 */
	{0x38, 0x07, 0xe8},/*VH */

	{0x38, 0x08, 0x02},/*DVP H output size 640*/
	{0x38, 0x09, 0x88},/*DVP H output size */

	{0x38, 0x0a, 0x01},/*DVP V output size 480 */
	{0x38, 0x0b, 0xe8},/*DVP V output size */

	{0x38, 0x0C, 0x0c},/*THS 3174*/
	{0x38, 0x0D, 0x66},/*THS */

	{0x38, 0x0E, 0x01},/*TVS 504 */
	{0x38, 0x0F, 0xf8},/*TVS */

	{0x50, 0x5a, 0x0a},/* mirror & vfilp check */
	{0x50, 0x5b, 0x2e},/* mirror & vfilp check */

	{0x38, 0x18, 0xc2},/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp on [5] */
	{0x38, 0x1a, 0x00},
	{0x38, 0x1c, 0x20},
	{0x38, 0x1d, 0x0a},
	{0x38, 0x1e, 0x01},
	{0x38, 0x1f, 0x20},
	{0x38, 0x20, 0x00},
	{0x38, 0x21, 0x00},
	{0x38, 0x24, 0x23},
	{0x38, 0x25, 0x20},
	
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x0c},

	{0x50, 0x02, 0x02},
	{0x59, 0x01, 0x04},/*H sub-sample coefficient , bit[3:2] , 00:sub-sample 1, 01:sub-sample 2x , 1x:sub-sample 4x*/

	{0x40, 0x1c, 0x42},
	{0x40, 0x1d, 0x28},

	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},
#endif
	};

	profLogAdd(0, "Sensor VGA 120FPS init");
	if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
    g_IsSnap2Pv=0;
    fpvsusp=1;		
	sensor_event_wait(0x04);

	sensorSyncSigSet(&sensorSyncSigPreviewVGA120FPSCfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	sensorSerialFlush(cmdportid);

	sensor_event_wait(0x04);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorSerialAsyncWrite(cmdportid, sizeof(regDataBlc)/sizeof(regDataBlc[0]), regDataBlc[0]);
	sensorSerialFlush(cmdportid);

	profLogAdd(1," \033[32mOV5653 VGA 120FPS  init \033[0m\n");
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
	static const UINT8 regdata[][3] = {
		
	/*;--------------------------------
	   ;	1280x960 30fps key setting	
	   ;	input	:	24MHZ
	   ;	output	:	66MHZ
	   ;	fps		:	30fps
	   ; 	HTS		: 	2161
	   ; 	VTS		:	1018
	   ;-------------------------------- */
	   
	{0x30, 0x10, 0x10},
	{0x30, 0x11, 0x0b}, 	/*PCLK	0x08 = 48MHz, 0x0b = 66MHz */ 
	/* group start */
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},
 
	/* resolution */
	/*Sensor Original setting.*/
	/*{0x36, 0x13, 0xc4},
	{0x36, 0x21, 0xaf},*/

	/*Modify the 0x3613 bit 7 and 0x3621 bit6 to use vertical/horizontal binning average.*/
	{0x36, 0x13, 0x44},/*bit 7 , 1: vertical binning sum, 0: vertical binning average.*/
	{0x36, 0x21, 0xef},/*bit 6 , 1: horizontal binning average, 0: horizontal binning sum.*/

	{0x37, 0x03, 0x9a},
	{0x37, 0x05, 0xdb},
	{0x37, 0x0a, 0x81},
	{0x37, 0x0c, 0x00},
	{0x37, 0x0d, 0x42},
	{0x37, 0x13, 0x92},
	{0x37, 0x14, 0x17}, 

	{0x38, 0x00, 0x02},/*HS 594 */
	{0x38, 0x01, 0x52},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0a},/*VS */

	{0x38, 0x04, 0x05},/*HW 1280 */
	{0x38, 0x05, 0x00},/*HW */

	{0x38, 0x06, 0x03},/*VH 976 */
	{0x38, 0x07, 0xd0},/*VH */

	{0x38, 0x08, 0x05},/*DVP H output size 1280*/
	{0x38, 0x09, 0x00},/*DVP H output size */

	{0x38, 0x0a, 0x03},/*DVP V output size 976 */
	{0x38, 0x0b, 0xd0},/*DVP V output size */

	{0x38, 0x0C, 0x08},/*THS 2161*/
	{0x38, 0x0D, 0x71},/*THS */

	{0x38, 0x0E, 0x03},/*TVS 1018 */
	{0x38, 0x0F, 0xfa},/*TVS */

	{0x38, 0x18, 0xc1},
	{0x38, 0x1a, 0x00},
	{0x38, 0x1c, 0x10},
	{0x38, 0x1d, 0x0e},
	{0x38, 0x1e, 0x07},
	{0x38, 0x1f, 0x8c},
	{0x38, 0x20, 0x00},
	{0x38, 0x21, 0x20},
	{0x38, 0x24, 0x23},
	{0x38, 0x25, 0x2a},
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x0c}, 

	{0x50, 0x02, 0x00},
	{0x59, 0x01, 0x00},

	{0x40, 0x1c, 0x42},
	{0x40, 0x1d, 0x08},
	{0x50, 0x5a, 0x0a}, /* mirror & vfilp check */
	{0x50, 0x5b, 0x2e}, /* mirror & vfilp check */

	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},

	};  

	
	/*printf("OV5653 XGA  init \n");*/
	
	if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
		g_IsSnap2Pv=0;
    }    
    fpvsusp=1;
	/*sensor_event_wait(0x04);
    profLogAdd(0, "SensorXGAinit-uCode-s");*/
	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	
	sensor_event_wait(0x04);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    /*sensorEventVvalidWait(1);*/
	sensorSerialAsyncWrite(cmdportid, sizeof(regDataBlc)/sizeof(regDataBlc[0]), regDataBlc[0]);
	sensorSerialFlush(cmdportid);
    profLogAdd(0, "SensorXGAinit-e");
	/*printf(" \033[32mOV5653 XGA  init \033[0m\n");*/

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
	static const UINT8 regdata[][3] = {
		
	/*;--------------------------------
	   ;	1280x720 30fps key setting	
	   ;	input	:	24MHZ
	   ;	output	:	48MHZ
	   ;	fps		:	30fps
	   ; 	HTS		: 	2150
	   ; 	VTS		:	744
	   ;-------------------------------- */
	
#if 1	   
	{0x30, 0x10, 0x10},
	{0x30, 0x11, 0x08},		/*PCLK  0x08 = 48MHz, 0x0b = 66MHz */ 
	/* group start */
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},

	/* resolution */
	/*Sensor Original setting.*/
	/*{0x36, 0x13, 0xc4},
	{0x36, 0x21, 0xaf},*/

	/*Modify the 0x3613 bit 7 and 0x3621 bit6 to use vertical/horizontal binning average.*/
	{0x36, 0x13, 0x44},/*bit 7 , 1: vertical binning sum, 0: vertical binning average.*/
	{0x36, 0x21, 0xef},/*bit 6 , 1: horizontal binning average, 0: horizontal binning sum.*/

	{0x37, 0x03, 0x9a},
	{0x37, 0x05, 0xdb},
	{0x37, 0x0a, 0x81},
	{0x37, 0x0c, 0x00},
	{0x37, 0x0d, 0x42},
	{0x37, 0x13, 0x92},
	{0x37, 0x14, 0x17}, 

	{0x38, 0x00, 0x02},/*HS 596 */
	{0x38, 0x01, 0x54},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0c},/*VS */

	{0x38, 0x04, 0x05},/*HW 1288 */
	{0x38, 0x05, 0x08},/*HW */

	{0x38, 0x06, 0x02},/*VH 976 */
	{0x38, 0x07, 0xd8},/*VH */

	{0x38, 0x08, 0x05},/*DVP H output size 1280*/
	{0x38, 0x09, 0x08},/*DVP H output size */

	{0x38, 0x0a, 0x02},/*DVP V output size 976 */
	{0x38, 0x0b, 0xd8},/*DVP V output size */

	{0x38, 0x0C, 0x08},/*THS 2150*//**/
	{0x38, 0x0D, 0x66},/*THS */

	{0x38, 0x0E, 0x02},/*TVS 744 *//**/
	{0x38, 0x0F, 0xe8},/*TVS */

	{0x38, 0x18, 0xc1},
	{0x38, 0x1a, 0x00},
	{0x38, 0x1c, 0x30},/**/
	{0x38, 0x1d, 0xfa},/**/
	{0x38, 0x1e, 0x05},
	{0x38, 0x1f, 0xd0},
	{0x38, 0x20, 0x00},
	{0x38, 0x21, 0x20},
	{0x38, 0x24, 0x23},
	{0x38, 0x25, 0x26},
	
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x0c},	

	{0x50, 0x02, 0x00},
	{0x59, 0x01, 0x00},

	{0x40, 0x1c, 0x42},
	{0x40, 0x1d, 0x08},
	
	{0x50, 0x5a, 0x0a}, /* mirror & vfilp check */
	{0x50, 0x5b, 0x2e}, /* mirror & vfilp check */
	/*{0x40, 0x06, 0x00},*/
	/*{0x40, 0x07, 0x05},*/


	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},

#endif
	
	};  

	profLogAdd(0, "Sensor 720p init");
	 if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
    g_IsSnap2Pv=0;
    fpvsusp=1;	
	sensor_event_wait(0x04);
	sensorSyncSigSet(&sensorSyncSigPreview720Cfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	sensorSerialFlush(cmdportid);
	sensor_event_wait(0x04);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	
	sensorSerialAsyncWrite(cmdportid, sizeof(regDataBlc)/sizeof(regDataBlc[0]), regDataBlc[0]);
	sensorSerialFlush(cmdportid);

	printf(" \033[32mOV5653 720p  init \033[0m\n");

}


static void
sensorDoFuncPreview720P60FPSBegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {
		
	/*;--------------------------------
	   ;	1280x720 30fps key setting	
	   ;	input	:	24MHZ
	   ;	output	:	96MHZ
	   ;	fps		:	60fps
	   ; 	HTS		: 	2150
	   ; 	VTS		:	744
	   ;-------------------------------- */
	
#if 1	   
	{0x30, 0x10, 0x10},
	{0x30, 0x11, 0x10},		/*PCLK  0x08 = 48MHz, 0x0b = 66MHz */ 
	/* group start */
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},

	/* resolution */
	/*Sensor Original setting.*/
	/*{0x36, 0x13, 0xc4},
	{0x36, 0x21, 0xaf},*/

	/*Modify the 0x3613 bit 7 and 0x3621 bit6 to use vertical/horizontal binning average.*/
	{0x36, 0x13, 0x44},/*bit 7 , 1: vertical binning sum, 0: vertical binning average.*/
	{0x36, 0x21, 0xef},/*bit 6 , 1: horizontal binning average, 0: horizontal binning sum.*/

	{0x37, 0x03, 0x9a},
	{0x37, 0x05, 0xdb},
	{0x37, 0x0a, 0x81},
	{0x37, 0x0c, 0x00},
	{0x37, 0x0d, 0x42},
	{0x37, 0x13, 0x92},
	{0x37, 0x14, 0x17}, 

	{0x38, 0x00, 0x02},/*HS 596 */
	{0x38, 0x01, 0x54},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0c},/*VS */

	{0x38, 0x04, 0x05},/*HW 1288 */
	{0x38, 0x05, 0x08},/*HW */

	{0x38, 0x06, 0x02},/*VH 976 */
	{0x38, 0x07, 0xd8},/*VH */

	{0x38, 0x08, 0x05},/*DVP H output size 1280*/
	{0x38, 0x09, 0x08},/*DVP H output size */

	{0x38, 0x0a, 0x02},/*DVP V output size 976 */
	{0x38, 0x0b, 0xd8},/*DVP V output size */

	{0x38, 0x0C, 0x08},/*THS 2150*/
	{0x38, 0x0D, 0x66},/*THS */

	{0x38, 0x0E, 0x02},/*TVS 744 */
	{0x38, 0x0F, 0xe8},/*TVS */

	{0x38, 0x18, 0xc1},
	{0x38, 0x1a, 0x00},
	{0x38, 0x1c, 0x30},
	{0x38, 0x1d, 0xfa},
	{0x38, 0x1e, 0x05},
	{0x38, 0x1f, 0xd0},
	{0x38, 0x20, 0x00},
	{0x38, 0x21, 0x20},
	{0x38, 0x24, 0x23},
	{0x38, 0x25, 0x26},
	
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x0c},	

	{0x50, 0x02, 0x00},
	{0x59, 0x01, 0x00},

	{0x40, 0x1c, 0x42},
	{0x40, 0x1d, 0x08},
	
	/*{0x40, 0x06, 0x00},*/
	/*{0x40, 0x07, 0x05},*/

	{0x50, 0x5a, 0x0a},	/* mirror & vfilp check */
	{0x50, 0x5b, 0x2e},	/* mirror & vfilp check */

	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},

#endif
	
	};  

	profLogAdd(0, "Sensor 720p60FPs init");
	 if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
    g_IsSnap2Pv=0;
    fpvsusp=1;	
	sensor_event_wait(0x04);
	sensorSyncSigSet(&sensorSyncSigPreview720P60FPSCfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	sensorSerialFlush(cmdportid);
	sensor_event_wait(0x04);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialAsyncWrite(cmdportid, sizeof(regDataBlc)/sizeof(regDataBlc[0]), regDataBlc[0]);
	sensorSerialFlush(cmdportid);

	printf(" \033[32mOV5653 720p 60Fps init \033[0m\n");

}


static void 
sensorDoFuncPreview1080Begin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {
		
	/*;--------------------------------
	   ;	1920x1080 30fps key setting	
	   ;	input	:	24MHZ
	   ;	output	:	96MHZ
	   ;	fps 	:	30fps
	   ;	HTS 	:	2898
	   ;	VTS 	:	1104
	   ;-------------------------------- */
#if 1	
	#if 0
	{0x30, 0x10, 0x10},	/*48M read out 15fps*/
	{0x30, 0x11, 0x08}, 	
	#else
	{0x30, 0x10, 0x10}, 	/*96M read out 30fps*/
	{0x30, 0x11, 0x10}, 
	#endif
	/* group start */
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},

	/* resolution */
	{0x36, 0x13, 0x44},
	{0x36, 0x21, 0x2f},
	{0x37, 0x03, 0xe6},
	{0x37, 0x05, 0xda},
	{0x37, 0x0a, 0x80},
	{0x37, 0x0c, 0x00},
	{0x37, 0x0d, 0x04},
	{0x37, 0x13, 0x22},
	{0x37, 0x14, 0x27}, 

	{0x38, 0x00, 0x02},/*HS 660 */
	{0x38, 0x01, 0x94},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0c},/*VS */

	{0x38, 0x04, 0x07},/*HW 1920 */
	{0x38, 0x05, 0x80},/*HW */

	{0x38, 0x06, 0x04},/*VH 1088 */
	{0x38, 0x07, 0x40},/*VH */

	{0x38, 0x08, 0x07},/*DVP H output size 1920*/
	{0x38, 0x09, 0x80},/*DVP H output size */

	{0x38, 0x0a, 0x04},/*DVP V output size 1088 */
	{0x38, 0x0b, 0x40},/*DVP V output size */

	{0x38, 0x0C, 0x0a},/*THS 2692*/
	{0x38, 0x0D, 0x84},/*THS */

	{0x38, 0x0E, 0x04},/*TVS 1188 */
	{0x38, 0x0F, 0xa4},/*TVS */

	{0x38, 0x18, 0xc0},
	{0x38, 0x1a, 0x1c},
	{0x38, 0x1c, 0x31},
	{0x38, 0x1d, 0xbe},
	{0x38, 0x1e, 0x04},
	{0x38, 0x1f, 0x60},
	{0x38, 0x20, 0x05},
	{0x38, 0x21, 0x19},
	{0x38, 0x24, 0x22},
	{0x38, 0x25, 0xE0},
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x0c}, 

	{0x50, 0x02, 0x00},
	{0x59, 0x01, 0x00},

	{0x40, 0x1c, 0x46},
	{0x40, 0x1d, 0x08},
	{0x50, 0x5a, 0x0a}, /* mirror & vfilp check */
	{0x50, 0x5b, 0x2e}, /* mirror & vfilp check */

	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},

#endif
		
	};	

	profLogAdd(0, "Sensor1080init");
	 if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
    g_IsSnap2Pv=0;
    fpvsusp=1;	
	sensor_event_wait(0x04);
	sensorSyncSigSet(&sensorSyncSigPreview1080Cfg);
	/*printf("H size(%u),V size(%u)\n",sensorSyncSigPreview1080Cfg.hsize,sensorSyncSigPreview1080Cfg.vsize);
	printf("H off(%u),V off(%u)\n",sensorSyncSigPreview1080Cfg.hoffset,sensorSyncSigPreview1080Cfg.voffset);*/
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	sensorSerialFlush(cmdportid);
	sensor_event_wait(0x04);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialAsyncWrite(cmdportid, sizeof(regDataBlc)/sizeof(regDataBlc[0]), regDataBlc[0]);
	sensorSerialFlush(cmdportid);
	printf(" \033[32mOV5653 1080  init \033[0m\n");

}

static void
sensorDoFuncPreviewShortExp()
{
}
static void
sensorDoFuncPreviewExpEnd()
{
}

static void
sensorDoFuncSnapExpBegin()
{
    profLogAdd(0, "sensorDoFuncSnapExpBegin");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 
}
static void
sensorDoFuncSnapEarly0()
{
#if 0
	UINT32 cmdportid;
	static const UINT8 regdata[][3]= {
		0x12,0x00, /*this code change SVGA/UXGA immediately*/
		0x1a,0xf4,
		0x17,0x0f,
		0x18,0xb3,
		0x19,0x00,
		0x03,0x44,
		0x1b,0x01,
#if 0 /*always HSYNC, no HREF*/
		0x15,0x82,/*RD19 0x02??*/
#else
		0x15,0x40,
#endif
	};
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/2, &regdata[0]);
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
#endif
    profLogAdd(0, "sensorDoFuncSnapEarly0");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 
}
static void
sensorDoFuncSnapEarly1()
{
    UINT32 cmdportid;
	static const UINT8 regdata[][3] = {

	/*;--------------------------------
	   ;	2592X1948 10fps key setting	
	   ;	input	:	24MHZ
	   ;	output:		96MHZ
	   ;	fps		:	15 fps
	   ; HTS		: 	3252
	   ; VTS		:	1968
	   ;--------------------------------
	//0x30, 0x10, 0x10,//96M*/
	
#if 0
	{0x30, 0x10, 0x10}, /*48M   read out 7.5fps*/
	{0x30, 0x11, 0x08}, 
#else
	{0x30, 0x10, 0x10}, /*96M read out 15fps*/
	{0x30, 0x11, 0x10}, 
#endif
	
    	/* group start  */ 
	{0x32, 0x00, 0x00},
	{0x32, 0x01, 0xff},
	{0x32, 0x02, 0xff},
	{0x32, 0x03, 0xff},
	{0x32, 0x12, 0x00},
	/* resolution   */
	{0x36, 0x13, 0x44},
	{0x36, 0x21, 0x2f},
	{0x37, 0x03, 0xe6},
	{0x37, 0x05, 0xda},
	{0x37, 0x0a, 0x80},
	{0x37, 0x0c, 0x00},
	{0x37, 0x0d, 0x04},
	{0x37, 0x13, 0x22},
	{0x37, 0x14, 0x27}, 

	{0x38, 0x00, 0x02},/*HS  596*/ 
	{0x38, 0x01, 0x54},/*HS */

	{0x38, 0x02, 0x00},/*VS 10*/
	{0x38, 0x03, 0x0a},/*VS */

	{0x38, 0x04, 0x0a},/*HW 2592 */
	{0x38, 0x05, 0x20},/*HW */

	{0x38, 0x06, 0x07},/*VH 1948 */
	{0x38, 0x07, 0x9c},/*VH */

	{0x38, 0x08, 0x0a},/*DVP H output size 2592*/
	{0x38, 0x09, 0x20},/*DVP H output size */

	{0x38, 0x0a, 0x07},/*DVP V output size 1964*/
	{0x38, 0x0b, 0xAc},/*DVP V output size */

	{0x38, 0x0C, 0x0c},/*THS 3252 */
	{0x38, 0x0D, 0xb4},/*THS */

	{0x38, 0x0E, 0x07},/*TVS 1984 */
	{0x38, 0x0F, 0xC0},/*TVS */

	{0x38, 0x18, 0xc0},
	{0x38, 0x1a, 0x3c},
	{0x38, 0x1c, 0x20},
	{0x38, 0x1d, 0x0a},
	{0x38, 0x1e, 0x01},
	{0x38, 0x1f, 0x20},
	{0x38, 0x20, 0x00},
	{0x38, 0x21, 0x00},
	{0x38, 0x24, 0x01},
	{0x38, 0x25, 0xb4},
	{0x38, 0x26, 0x00},
	{0x38, 0x27, 0x0a},	

	{0x50, 0x02, 0x00},
	{0x59, 0x01, 0x00},

	{0x40, 0x1c, 0x46},
	{0x40, 0x1d, 0x08},/*BLC update by frame*/
	
	/*{0x40, 0x06, 0x00},*/
	/*{0x40, 0x07, 0x10},*/

	/*group launch */
	{0x32, 0x12, 0x10},
	{0x32, 0x12, 0xa0},

#if CAP_FRAME_MODE
	/* Frame Mode FREX mode 1 */
	{0x3b, 0x07, 0x08},
	{0x30, 0x17, 0x7f},
	{0x30, 0x16, 0x02},
	{0x37, 0x05, 0xd0},
	{0x37, 0x0e, 0x08},
	{0x37, 0x09, 0x10},
	{0x30, 0x2c, 0x00},
	/* Frame mode Frex mode 1 end */
#endif

	};
	if(!g_IsSnap2Pv){
	  g_IsSnap2Pv=1;
	}
	profLogAdd(0, "SensorCaptureInit");
	cmdportid = sensorTgCfg.cmdportid_xtg;	
	sensorSerialAsyncWrite(cmdportid,sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
    sensorSerialFlush(cmdportid);
   	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	/*sensorSyncSigSet(&sensorSyncSigSnapCfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;	
	sensorSerialAsyncWrite(cmdportid,sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	sensorSerialFlush(cmdportid);*/

	/*profLogAdd(0, "sensorDoFuncSnapEarly1");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 
	printf(" \033[32mOV5653 Capture init \033[0m\n");
	
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	cmdportid = sensorTgCfg.cmdportid_xtg;	
	sensorSerialAsyncWrite(cmdportid,sizeof(regdata)/sizeof(regdata[0]), regdata[0]);*/
	profLogAdd(0, "sensorDoFuncSnapEarly1");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 
	/*printf(" \033[32mOV5653 Capture init \033[0m\n");*/
}


void sensor_frame_orig_vd_neg_cb_reg(void *f, UINT32 param);
#if 0
static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{
#if CAP_FRAME_MODE
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX);
#endif	
}
#endif

static void
sensorDoFuncSnapBshutterBegin()
{
#if CAP_FRAME_MODE
	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);
#endif
}

static void
sensorDoFuncSnapBshutterFollow()
{
	#if 0
	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);
	#endif
}

static void
sensorDoFuncSnapBshutterEnd()
{
#if CAP_FRAME_MODE
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0);
#endif
}

static void
sensorDoFuncSnapShortExp()
{
	/*front_snap_short_exp_frame_set()*//*slow shutter, 1 longer frame*/
	/*front_snap_short_exp_tg_set*/
	/*do 1 longer frame exp, historical term "bshutter", if needed*/

}
static void
sensorDoFuncSnapExpEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapExpEnd*/
	/*throw user msg??*/
	/*flash rearsync trig*/
	/*m-sh close trig*/
	/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0);*/

	/*printf("\n&&&& sensorDoFuncSnapExpEnd &&&&\n");*/
    profLogAdd(0, "sensorDoFuncSnapExpEnd");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 

}

static void
sensorDoFuncSnapReadoutField0()
{
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
	/*sensorSyncSigSet(&sensorSyncSigSnapCfg);*/

	profLogAdd(0, "SensorReadoutField0");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 

    
    
	/*printf("\n&&&& ReadOut &&&&\n");*/

}

static void
sensorDoFuncSnapFieldEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapEnd()*/
	/*throw user msg??*/
	/*printf("\n&&&& Field End &&&&\n");*/
    profLogAdd(0, "sensorDoFuncSnapFieldEnd");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 
}
#if 0
static void 
sensorReportsize()
{
    profLogAdd(0, "----- sensorReportsize -----");
	profLogPrintf(0, "mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	profLogPrintf(0, "front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	profLogPrintf(0, "imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	profLogPrintf(0, "rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	WRITE16(0xb0009064,1);
	profLogPrintf(0, "line count            = (%d)",READ16(0xb0009064));
    profLogPrintf(0, "----------------------------");
    profLogPrintf(0, "front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	profLogPrintf(0, "snap trig         = (%d)",READ8(0xb0009068)&0x01); 
}
#endif

/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{
	/*panasonic TG TRG reset*/
	/*direct m-sh open*/
	/*direct vsubctl off*/
	/*immediate inform readout all end*/
	/*s Add by Aries 09/07/27*/
/*
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
*/	
/*	if (appStillCapFlashStatusGet()==2){
		halFrontFlashCfgSet(0x00000000,0);
		halFrontFlashTrig();
	}
*/	/*e Add by Aries 09/07/27*/

	profLogAdd(0, "SensorReadoutEnd"); 
	


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

	sensor_exptbl_init();
	sensor_bshut_init();
	
}
static void
sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
}
static void
sensor_cmdport_exp_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{
	/*****TODO******ITG fine exp set, value and timing???*/
	/*exp setting via serial port*/
	#if 1
	/*UINT8 i;
	printf("nreg=%d\n",nreg);
	for(i = 0 ; i < nreg ; i++){
		printf("\033[32m%d . %#x%x = %#x\033[0m\n",i,pdata[i*3+0],pdata[i*3+1],pdata[i*3+2]);
	} 
		
	return;*/

	UINT32 smode;
	smode=sensorModeGet();
	if(smode==0x30)
	{
		g_last_exp_line   =(pdata[3*0+2]<<16)+(pdata[3*1+2]<<8)+(pdata[3*2+2]);		
		g_last_smode = smode;
		/*profLogPrintf(1,"[%d %x]",g_last_exp_line,g_last_smode);*/
	}
	#endif
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
}
static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
}

#if SP5K_SENSOR_OV5653
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	expLine = expLine;
	#if 1
	UINT8 regData[][3] ={
		{0x35,0x0c,0x00},
		{0x35,0x0d,0x00},
		{0x35,0x00,0x00},
		{0x35,0x01,0x00},
		{0x35,0x02,0x00},
	};
	
	regData[4][2] = (UINT8)((expLine & 0x0f)<<4);
	regData[3][2] = (UINT8)((expLine & 0x0ff0)>>4);
	

	printf("expLine = %d\n",expLine);
	sensor_event_wait(0x04);
	/*sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regData);*/
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(regData)/sizeof(regData[0]), regData[0]);
	#endif
}
#endif

#define DONTCHANE    	0x7FFFFFFF
static UINT32 sensorSuspended;
static UINT32 sensorSuspendExecuteLoad()
{
	printf("sensorSuspended = %d \n", sensorSuspended);
	if(sensorSuspended == 0)
	{
		profLogPrintf(1, "senSusp s");
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI,SENSOR_PWRDN,SENSOR_PWRDN);
		halFrontClkCfg(MCLKMODE,1);/*internal clock*/
		sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN, 0,DONTCHANE,DONTCHANE,DONTCHANE);/*disable tgpll clock*/
		/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); if have reset pin control, don't mask this*/
		profLogPrintf(1, "senSusp e");
		sensorSuspended = 1;
	}
 
	return SUCCESS;
}
 
static UINT32 sensorResumeExecuteLoad()
{
	printf("sensorResumed = %d \n", sensorSuspended);
	if(sensorSuspended == 1)
	{
		profLogPrintf(1, "senResu s");
		sensorDoFuncInit(); /*sensor init */
		profLogPrintf(1, "senResu e");
		sensorSuspended = 0;
	}
 
	return SUCCESS;
}
 
static void sensorDoPowerSave(UINT32 mode)
{
	printf("sensorDoPowerSave = %d \n", mode);
	if(mode == SENSOR_MODE_POWERSAVE)
	{
		sensorSuspendExecuteLoad();
	}
	else
	{
		sensorResumeExecuteLoad();
	}
}


