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
 *  Author: Will Chiu                                                     *
 *                                                                        *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_ov9712-0.00.01"

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
#include "api/sp5k_global_api.h"
#include "../flow/sensor_model_snap.h"
#include "../sensor_util.h"


#undef _EVB_SPE7001_HW_

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

#ifdef _EVB_SPE7001_HW_
#define SENSOR_FREX     BIT6
#define SENSOR_PWRDN    BIT0
#define SENSOR_RESET    BIT9

#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#endif

#ifdef _CATHY_HW_
#define SENSOR_FREX     BIT3
#define SENSOR_PWRDN    BIT11
#define SENSOR_RESET    BIT9

#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#endif


/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HSIZE  640		/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  400		/* preview vertical size*/

#define PREVIEW_XGA_HSIZE  1280/*1260*/ /*1280*/ 	/* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  720/*792*//*790*/	/*800*/	/* preview vertical size*/

#define CAPTURE_HSIZE  1280/*(1248)*/		/* capture horiz. size*/
#define CAPTURE_VSIZE  720/*(792)*/		/* capture vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL   1688	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   410 /*408*/		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  414		/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  6		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     204  /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     1/*2*/		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         6/*12*/ /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         7/*13*/ /*7*/	/* preview Vd reshap rise*/

#define PREVIEW_XGA_LINETOTAL   1688	/* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   410/*408*/		/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL  828/*996*/	    /* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  20		/* preview Vd sync length*/
#define PREVIEW_XGA_HOFFSET     408/*411*//*303*//*491*/     /* preview Hd valid pixel data offset position*/
#define PREVIEW_XGA_VOFFSET     4/*25*//*31*/      /* preview Vd valid line data offset position*/
/* Preview resh_XGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL         6/*4*/ 	/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE         7/*5*/	/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL         4 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE         7 /*7*/	/* preview Vd reshap rise*/
                                
/* Capture default definition here */
#define CAPTURE_LINETOTAL   1688	/* capture Hd length*/
#define CAPTURE_LINEBLANK   410/*408*/		/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  828	/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  20		/* capture Vd sync length*/

#define CAPTURE_HOFFSET     408    /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     4    	/* capture Vd valid line data offset position*/

/*#define CAPTURE_VOFFSET     (11+4) */	/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         6	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         7	/* capture Hd reshap rise*/

#if 1
#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_HD /* capture Vd reshap control*/
  #if 0
    #define CAPTURE_VRESHAPE_FALL         8000	/* capture Vd reshap fall , count by pixel*/
    #define CAPTURE_VRESHAPE_RISE         8100	/* capture Vd reshap rise , count by pixel*/
  #else
    #define CAPTURE_VRESHAPE_FALL		  4	/* capture Vd reshap fall , count by pixel*/
    #define CAPTURE_VRESHAPE_RISE		  7	/* capture Vd reshap rise , count by pixel*/  
  #endif
#else
#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_HD /* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL         2	    /* capture Vd reshap fall*/
#define CAPTURE_VRESHAPE_RISE         3 	/* capture Vd reshap rise*/
#endif
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
#define PIXELPHASE	8	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/


#define PREVIEW_VGA_PCLK        48000
#define PREVIEW_XGA_PCLK        48000
#define CAPTURE_PCLK            48000


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
#define	PREVIEW_XGA_CFAPATTERN	BAYER_BGGR/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define   PREVIEW_VGA_CFAPATTERN  BAYER_BGGR
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
#define	CAPTURE_HDNUM_FRAME_START	(11)	/*the front interrupt line number of the snap new frame */
#define	PREVIEW_HDNUM_FRAME_START  11		/*the front interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_END	800/*1970*/    	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	300/*1971*/    	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END	401		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT	200		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_FRAME_END	800/*975*/  	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_CMDPORT	500/*976*/		/*the command port interrupt line number of the preview new frame*/

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
#define	TG_SPI_I2C_MODE  0x60     	       /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF | SENSOR_SERIAL_FORMAT_SSC_SIF2  /*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#if SPCA5210
#define	TG_SPEED         2
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  8		/*ssc/spi register address numbers */

#define CAP_FRAME_MODE  0


/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreviewXGABegin();
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
	.pix_100khz =PREVIEW_XGA_PCLK,
	.hratio=1280,
	.vratio=720,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	  &sensorTgPreviewXGACfg, &sensorTgPreviewVGACfg, 
	/*  &sensorTgPreviewVGACfg, &sensorTgPreviewXGACfg,*/  /* Eric Lee use VGA mode 091221 */
	  	
};


static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly1,
		},
		.snapExpBegin = sensorDoFuncSnapExpBegin,
		.snapBshutterBegin = NULL, /*sensorDoFuncSnapBshutterBegin,*/
		.snapBshutterFollow = NULL, /*sensorDoFuncSnapBshutterFollow,*/
		.snapBshutterEnd = NULL, /*sensorDoFuncSnapBshutterEnd,*/
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
	.hratio = 1280,
	.vratio = 960,
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
	.hratio = 1280,
	.vratio = 960,
};

#if CAP_FRAME_MODE
static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapFrameCfg, &sensorTgSnapRollCfg, 
};
#else
static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg, &sensorTgSnapFrameCfg, 
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

/*s Added by Eric 100601 */
UINT8 Dummydata[4];
UINT8 lastDummydata[2];
UINT8 AgcDummySyncWrite;
/*e Added by Eric 100601 */

void
sensorTg_ov9712_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}


static void
TGStartUpProc(void)
{
	/****TODO****sensor power*/
		
		sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);

		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0x00000000);
		tmrUsWait(1000);  /*1ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,0xFFFFFFFF); /* PWD High*/
		tmrUsWait(10000);  /*10ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,0x00000000); /* PWD Low*/
		tmrUsWait(1000);  /*1ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0xFFFFFFFFF);

		tmrUsWait(25000); /*25ms*/
	/*	
		sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
		tmrUsWait(10000); */ /*10ms*/

		printf(" 9710 start on sensor \n");
	
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX,0x00000000);
	
		/*sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_RST_ON);*/
#if 0
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0xFFFFFFFF);  /* hardware reset */
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0x00000000);
		tmrUsWait(1000);  /*1ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0xFFFFFFFF);
#endif
		/****TODO****sensor power*/
		/*sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_ON);*/

		/*s Added by Eric 100408 */
		lastDummydata[0]=0;
		lastDummydata[1]=0;
		AgcDummySyncWrite=0;
		/*e Added by Eric 100408 */

}


/*------method definitions-----*/
		
static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
	static const UINT8 regdata[][2] = {		

	/*Slave_ID=0x60;	*/			
	{0x12,0x80}, /*  Reset */  
	                         
	/*   Core Settings  */    
/*	{0x12,0x00},*/
	{0x1e,0x07},            
	{0x5f,0x18},            
	{0x69,0x04},            
	{0x65,0x2a},            
	{0x68,0x0a},            
	{0x39,0x28},            
	{0x4d,0x90},            
	{0xc1,0x80},
	{0x0c,0x30},   /*ov*/
	{0x6d,0x02},   /*ov*/

	/*   DSP   */                               
	{0x96,0x01},  /*ISP disable, WB disable */
	{0xbc,0x68},                              

	/*   YAVG  */                         
	{0x4e,0x55}, /*   AVERAGE */          
	{0x4f,0x55},                       
	{0x50,0x55},                       
	{0x51,0x55},                       
	{0x24,0x55}, /*  Exposure windows  */
	{0x25,0x40},                       
	{0x26,0xa1},                       

	/*   General */                             
	{0x13,0x88}, /* disable AEC/AGC, enable LAEC(fine AE) */                            
	{0x14,0x40},   /*  Gain Ceiling 8 */        


	};

/* TGGPIO configuration here */
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	/*VD,HD input*/ /*default in, do nothing*/
	/*MCLK*/ /*tggpio7*/
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	/*Pixel Clock = 27MHz /27*48/2 = 24 MHz, XGA= 15fps VGA=30fps*/
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_DIV,1,27,(24*2),NOUSE);
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN,1,SENSOR_TGPLL_DRV_8MA,NOUSE,NOUSE);
	halFrontClkCfg(MCLKMODE, PCLKMODE);

	#ifdef _EVB_SPE7001_HW_
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	#endif
	
	TGStartUpProc();


/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	/*s Modify by Aries 09/12/13*/
	/*halFrontClkSet(TGPLLEN,TGPLLCLKSET, MCLKDIV, PCLKDIV);*/
	/*halPllFrontClkSet((MCLKDRV<<4)|PCLK_SRC, 24000);*//* Set master clock & pixel clock */
	/*halFrontClkCfg(MCLKMODE, PCLKMODE);*/
	/*e Modify by Aries 09/12/13*/
	#if SPCA5210
/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
	#endif

	/*****TODO*****front reset after these???*/
	tmrUsWait(2000);

	cmdportid = sensorTgCfg.cmdportid_xtg;
#if 1
	sensorSerialAsyncWrite(cmdportid, 1, regdata[0]);  /* Sensor Soft Reset */
	sensorSerialFlush(cmdportid);
	tmrUsWait(50000);  /* delay 100 ms for reset*/

	sensorSerialBulkWrite(cmdportid,
		(sizeof(regdata)/sizeof(regdata[0])) - 1, regdata[1],0);

#endif
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

#if 0
	*(volatile UINT8 *)0xb00000ba =0x08;     /* output HValid */
	*(volatile UINT8 *)0xb00000bb =0x00;
	*(volatile UINT8 *)0xb00000b8 =0x09;
	*(volatile UINT8 *)0xb00090f8 =0x00;
#endif	
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
	static const UINT8 regdata[][2] = {

	/*  Resolution and Format  */
	{0x12,0x40}, 
	{0x3b,0x01}, 
	{0x97,0x80}, 
	{0x17,0x25},  /*HStart MSB  = 37*8+7=303*/   
	{0x18,0xA2},  /*HSize MSB = A2*8+ 0=1296*/
	{0x19,0x01},  /*VStart MSB = 1*8+ 2 = 10 */ 
	{0x1a,0x64},  /*VSize MSB = 100*4+0 =400 */  
	{0x03,0x02},  /*VStart [1:0]LSB, VSize [3:2]LSB */ 
	{0x32,0x07},  /*HStart [2:0]LSB, HSize [5:3]LSB */ 
	{0x98,0x00}, 
	{0x99,0x00}, 
	{0x9a,0x00}, 
	{0x57,0x00}, /*DSP output HSize[4:2] VSize[1:0] LSB */  
	{0x58,0x64}, /*DSP output VSize MSB = 100*4+0 = 400 */  
	{0x59,0x50}, /*DSP output HSize MSB = 80*8+0 = 640 */   
	{0x4b,0x9a}, 
	{0x4c,0x09}, 
	{0x3d,0x9e}, 
	{0x3e,0x01}, 
	{0xbd,0x50}, 
	{0xbe,0x64}, 
	{0x2c,0x60}, 
	{0x23,0x10}, 

	/*  Clock  */
	{0x5c,0x59},   
	{0x5d,0x00},   
	{0x11,0x01},  /* 01=30fps, 00=60fps */  
	{0x2a,0x98},   
	{0x2b,0x06},   
	{0x2d,0x00},   /*Dummy line LSB */
	{0x2e,0x00},   /*Dummy line MSB */

	{0x00,0x20},   /*AGC default */

	};
	sensor_event_wait(0x04);
	/*s Modify by Aries 09/12/13*/
	/*halPllFrontClkSet((MCLKDRV<<4)|PCLK_SRC, 24000);*//* Set master clock & pixel clock */
	/*e Modify by Aries 09/12/13*/
	sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);

	
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
	static const UINT8 regdata[][2] = {

	/*  Resolution and Format  */
	{0x12,0x00},              
	{0x3b,0x00},              
	{0x97,0x80},              
	{0x17,0x25},   /*HStart MSB  = 37*8+7=303*/           
	{0x18,0xA2},   /*HSize MSB = A2*8+ 0=1296*/           
	{0x19,0x01},   /*VStart MSB = 1*8+ 2 = 10 */           
	{0x1a,0xCA},   /*VSize MSB = CA*4+2 =810 */           
	{0x03,0x0A},   /*VStart [1:0]LSB, VSize [3:2]LSB */           
	{0x32,0x07},   /*HStart [2:0]LSB, HSize [5:3]LSB */           
	{0x98,0x00},              
	{0x99,0x00},              
	{0x9a,0x00},              
	{0x57,0x00},   /*DSP output HSize[4:2] VSize[1:0] LSB */           
	{0x58,0xC8},   /*DSP output VSize MSB = C8*4+0 = 800 */         
	{0x59,0xA0},   /*DSP output HSize MSB = A0*8+0 = 1280 */           
	{0x4c,0x13},              
	{0x4b,0x36},              
	{0x3d,0x3C},              
	{0x3e,0x03},              
	{0xbd,0xA0},              
	{0xbe,0xC8},              
#if 1

	/*  Clock  */
	{0x5c,0x59},   
	{0x5d,0x00},   
	{0x11,0x00},   
	{0x2a,0x98},   
	{0x2b,0x06},   
	{0x2d,0x00},   /*Dummy line LSB */
	{0x2e,0x00},   /*Dummy line MSB */    	

	{0x00,0x20},   /*AGC default */

#endif

	
	};  
        
	sensor_event_wait(0x04);
	/*s Modify by Aries 09/12/13*/
	/*halPllFrontClkSet((MCLKDRV<<4)|PCLK_SRC, 24000);*//* Set master clock & pixel clock */
	/*e Modify by Aries 09/12/13*/
	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);  

	printf(" OV9712 XGA register init \n");

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

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0); /* trace */
	
}

static void
sensorDoFuncSnapEarly0()
{


}

static void
sensorDoFuncSnapEarly1()
{
    UINT32 cmdportid;
	static const UINT8 regdata[][2] = {

	/*  Resolution and Format  */
	{0x12,0x00},              
	{0x3b,0x00},              
	{0x97,0x80},              
	{0x17,0x25},   /*HStart MSB  = 37*8+7=303*/           
	{0x18,0xA2},   /*HSize MSB = A2*8+ 0=1296*/           
	{0x19,0x01},   /*VStart MSB = 1*8+ 2 = 10 */           
	{0x1a,0xCA},   /*VSize MSB = CA*4+2 =810 */           
	{0x03,0x0A},   /*VStart [1:0]LSB, VSize [3:2]LSB */           
	{0x32,0x07},   /*HStart [2:0]LSB, HSize [5:3]LSB */           
	{0x98,0x00},              
	{0x99,0x00},              
	{0x9a,0x00},              
	{0x57,0x00},   /*DSP output HSize[4:2] VSize[1:0] LSB */           
	{0x58,0xC8},   /*DSP output VSize MSB = C8*4+0 = 800 */         
	{0x59,0xA0},   /*DSP output HSize MSB = A0*8+0 = 1280 */           
	{0x4c,0x13},              
	{0x4b,0x36},              
	{0x3d,0x3C},              
	{0x3e,0x03},              
	{0xbd,0xA0},              
	{0xbe,0xC8},              

	/*  Clock  */
	{0x5c,0x59},   
	{0x5d,0x00},   
	{0x11,0x00},   
	{0x2a,0x98},   
	{0x2b,0x06},   
	{0x2d,0x00},   /*Dummy line LSB */
	{0x2e,0x00},   /*Dummy line MSB */

	};

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX); /* trace */
	
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;	
	sensorSerialAsyncWrite(cmdportid,sizeof(regdata)/sizeof(regdata[0]), regdata[0]); 

	profLogAdd(0, "SensorChgReg");
	printf(" OV9712 Capture Register Setting \n");


}


void sensor_frame_orig_vd_neg_cb_reg(void *f, UINT32 param);

static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX); 
    
	
	if (appStillCapFlashStatusGet()==1){
		halFrontFlashCfgSet(0x00000000,0);
		halFrontFlashTrig();
	}

 #if 0

    /**(volatile UINT8 *)0xb00090AF =0x10;*/	  /* VD  Polarity */
	/**(volatile UINT8 *)0xb00090AE =0x31;*/	 /* Reshpen Enable and ReVD base on pixel Clock */
	/**(volatile UINT8 *)0xb0009082 =0x00;*/	 /* internal VD/HD */
    
	*(volatile UINT8 *)0xb0002250  =0x01;	 /* CDSP Crop */
	*(volatile UINT16 *)0xb0002252 =0x00;
	*(volatile UINT16 *)0xb0002254 =0xa1f;
	*(volatile UINT16 *)0xb0002256 =0x00;
	*(volatile UINT16 *)0xb0002258 =0x793;
  
 #endif	

}

static void
sensorDoFuncSnapBshutterBegin()
{
	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);

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


	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0);
}

static void
sensorDoFuncSnapShortExp()
{
	/*front_snap_short_exp_frame_set()*//*slow shutter, 1 longer frame*/
	/*front_snap_short_exp_tg_set*/
	/*do 1 longer frame exp, historical term "bshutter", if needed*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0); /* trace */

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

		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX); /* trace */

}

static void
sensorDoFuncSnapReadoutField0()
{
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
/*	
	*(volatile UINT8 *)0xb00090ae =0x51;
	*(volatile UINT8 *)0xb0009082 =0x22;
*/	
	/*printf("\n&&&& ReadOut &&&&\n");*/

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0); /*trace*/ 

}

static void
sensorDoFuncSnapFieldEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapEnd()*/
	/*throw user msg??*/
	/*printf("\n&&&& Field End &&&&\n");*/
  #if 0
	UINT8 tempH,tempL;
	UINT32 OB_R,OB_GR,OB_B,OB_GB;

    tempL=*(volatile UINT8 *)0xb00022  ;
    tempH=*(volatile UINT8 *)0xb0002215 ;
	OB_R=tempL+tempH*256;
	
    tempL=*(volatile UINT8 *)0xb0002216 ;
    tempH=*(volatile UINT8 *)0xb0002217 ;
	OB_GR=tempL+tempH*256;

    tempL=*(volatile UINT8 *)0xb0002218 ;
    tempH=*(volatile UINT8 *)0xb0002219 ;
    OB_B=tempL+tempH*256;

    tempL=*(volatile UINT8 *)0xb000221a ;
    tempH=*(volatile UINT8 *)0xb000221b ;
    OB_GB=tempL+tempH*256;

    profLogPrintf(0,"$$$ will test OB [R,GR,B,GB]=%d,%d,%d,%d \n",OB_R,OB_GR,OB_B,OB_GB);
  #endif

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX); /*trace*/ 
  
}

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
	if (appStillCapFlashStatusGet()==2){
		halFrontFlashCfgSet(0x00000000,0);
		halFrontFlashTrig();
	}
	/*e Add by Aries 09/07/27*/

	
/*
	UINT16 Size;
    UINT32 Add;

	Size=*(volatile UINT16 *)0xb0002088; 
	profLogPrintf(0,"$$$ will test 0x2088 = %x\n",Size);

	Size=*(volatile UINT16*)0xb000208a; 
    profLogPrintf(0,"$$$ will test 0x208a = %x\n",Size); 

    Size=*(volatile UINT16*)0xb000208c; 
    profLogPrintf(0,"$$$ will test 0x208c = %x\n",Size); 

	Size=*(volatile UINT16*)0xb000208e; 
	profLogPrintf(0,"$$$ will test 0x208e = %x\n",Size); 

    Add=*(volatile UINT32*)0xb0007180; 
    profLogPrintf(0,"$$$ will test 0x7180 = %x\n",Add); 

	Size=*(volatile UINT16*)0xb000210e; 
	profLogPrintf(0,"$$$ will test 0x210e = %x\n",Size); 

    UINT8 value;

	value=*(volatile UINT8*)0xb000906c;
	profLogPrintf(0,"$$$ will test 0x906c = %x\n",value); 
	value=*(volatile UINT8*)0xb000906e;
	profLogPrintf(0,"$$$ will test 0x906e = %x\n",value); 
*/

/*	
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
*/ 

#if 1
		/**(volatile UINT8 *)0xb0009082 =0x00;*/	 /* internal VD/HD */
		/**(volatile UINT8 *)0xb00090AE =0x11;*/	 /* Reshpen Enable and ReVD base on HD */
#endif	

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0); /*trace*/ 

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
		/*s modified by Eric Lee 100601 */
		if (((lastDummydata[1]!=0)&&(pdata[11]==0)) || ((lastDummydata[1]>0)&&(pdata[17]>0)))
		{
			/* in this case, AGC and dummy line set timing need to synchronize */

			Dummydata[0]=pdata[8];
			Dummydata[1]=pdata[9];
			Dummydata[2]=pdata[10];
			Dummydata[3]=pdata[11];

			AgcDummySyncWrite =1;	
			lastDummydata[0]= pdata[10];
			lastDummydata[1]= pdata[11]; 

			nreg =4; 
			
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
			printf(" Agc - Dummy - Sync Write 2 \n"); 
		/*	printf(" to 30fps \n"); */

			#if 0
			printf(" data [0] = %d \n", pdata[0]);
			printf(" data [1] = %d \n", pdata[1]);
			printf(" data [2] = %d \n", pdata[2]);
			printf(" data [3] = %d \n", pdata[3]);
			printf(" data [4] = %d \n", pdata[4]);
			printf(" data [5] = %d \n", pdata[5]);
			printf(" data [6] = %d \n", pdata[6]);
			printf(" data [7] = %d \n", pdata[7]);
			printf(" data [8] = %d \n", pdata[8]);
			printf(" data [9] = %d \n", pdata[9]);
			printf(" data [10] = %d \n", pdata[10]);
			printf(" data [11] = %d \n", pdata[11]);
			#endif


		}
		else
		{
			AgcDummySyncWrite =0;	
			lastDummydata[0]= pdata[10];			
			lastDummydata[1]= pdata[11];	
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
			printf(" Normal one \n");
		}
		/*s modified by Eric Lee 100601 */
#else
sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);

	#endif
}
static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
	#if 0
	if(vtbl_type > 0x3f) exposure_time = vcount;
	exposure_time = 1000;

	static UINT8 vcount_save; /*0*/
	static UINT8 regdata[] = {
		0x11,0,
	};
	if (vtbl_type == 0) vcount = 0;
	else if (vtbl_type != 2 || vcount > 0x3f) return;
	if (vcount == vcount_save) return;
	vcount_save = vcount;
	regdata[1] = 0x80 | vcount;
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regdata);
	#endif
}

