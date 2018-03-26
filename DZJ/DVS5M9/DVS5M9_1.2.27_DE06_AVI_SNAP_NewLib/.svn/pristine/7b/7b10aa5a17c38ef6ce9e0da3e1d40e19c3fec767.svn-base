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

#define MODEL_VERSION "$unp_tg_ov5633-0.00.01"

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


/*s Modify by Aries 10/02/04*/
#ifdef _EVB_SPE7001_HW_
#define SENSOR_FREX     BIT6       /* for A900 */
/*#define SENSOR_FREX     BIT4 */          /* for EVB */
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
/*e Modify by Aries 10/02/04*/


/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HSIZE  640		/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  480		/* preview vertical size*/

#define PREVIEW_XGA_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  960		/* preview vertical size*/

#define CAPTURE_HSIZE  (2592-32)		/* capture horiz. size*/
#define CAPTURE_VSIZE  (1944-32)		/* capture vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL   1550	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   910		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  516		/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  36		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     909   /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     11		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/

#define PREVIEW_XGA_LINETOTAL   1800/*1600*/	/* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   520/*630*/		/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL  1000/*996*/	    /* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  36		/* preview Vd sync length*/
#ifdef _CATHY_HW_
/*s Modify by Aries 10/03/19*/
#define PREVIEW_XGA_HOFFSET     520/*518*/     /* preview Hd valid pixel data offset position*/
/*e Modify by Aries 10/03/19*/
#else
#define PREVIEW_XGA_HOFFSET     517/*491*/     /* preview Hd valid pixel data offset position*/
#endif
/*s Modify by Aries 10/03/19*/
#define PREVIEW_XGA_VOFFSET     16/*9*/      /* preview Vd valid line data offset position*/
/*e Modify by Aries 10/03/19*/
/* Preview resh_XGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL         200/*2*/ 	/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE         201/*3*/	/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/
                                
/* Capture default definition here */
#define CAPTURE_LINETOTAL   3232	/* capture Hd length*/
#define CAPTURE_LINEBLANK   640		/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  1980	/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  36		/* capture Vd sync length*/

#if 1
#define CAPTURE_HOFFSET     640    /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     2    	/* capture Vd valid line data offset position*/
#else
#define CAPTURE_HOFFSET     640    /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     11    	/* capture Vd valid line data offset position*/
#endif
/*#define CAPTURE_VOFFSET     (11+4) */	/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         352	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         354	/* capture Hd reshap rise*/

#if 1
#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_PIXEL /* capture Vd reshap control*/
  #if 0
  
    #define CAPTURE_VRESHAPE_FALL         8000	/* capture Vd reshap fall , count by pixel*/
    #define CAPTURE_VRESHAPE_RISE         8100	/* capture Vd reshap rise , count by pixel*/
  #else
    #define CAPTURE_VRESHAPE_FALL		  DISABLE	/* capture Vd reshap fall , count by pixel*/
    #define CAPTURE_VRESHAPE_RISE		  DISABLE	/* capture Vd reshap rise , count by pixel*/  
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
#define PIXELPHASE	3	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/


#define PREVIEW_VGA_PCLK        48000
#define PREVIEW_XGA_PCLK        48000
#define CAPTURE_PCLK            24000


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
#ifdef _CATHY_HW_
#define	PREVIEW_XGA_CFAPATTERN	BAYER_BGGR/*BAYER_RGGB*/          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#else
#define	PREVIEW_XGA_CFAPATTERN	BAYER_RGGB          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#endif
#define PREVIEW_VGA_CFAPATTERN  BAYER_GBRG
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
#define	CAPTURE_HDNUM_FRAME_START	(11+24)	/*the front interrupt line number of the snap new frame */
#define	PREVIEW_HDNUM_FRAME_START  11		/*the front interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_END	1946/*1970*/    	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	1930/*1940*//*1971*/    	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END	507		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT	508		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_FRAME_END	980/*975*/  	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_CMDPORT	800 /*581*//*981*//*976*/		/*the command port interrupt line number of the preview new frame*/

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
#if (_CATHY_HW_)
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF | SENSOR_SERIAL_FORMAT_SSC_SIF2 /*don't change*/
#else
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#endif
#define	TG_FORMAT3  	 0		      /*don't change*/
#if SPCA5210
#define	TG_SPEED         FRONT_SSC_CLK_96K
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */

#if (_CATHY_HW_)
#define CAP_FRAME_MODE  0
#else
#define CAP_FRAME_MODE  1
#endif

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
	.crop = NULL,
	.hsize = PREVIEW_XGA_HSIZE,
	.vsize = PREVIEW_XGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_XGA_CFAPATTERN,
	.pix_100khz =PREVIEW_XGA_PCLK,
	.hratio=1280,
	.vratio=960,
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
	.pix_100khz =CAPTURE_PCLK,
	.hratio=2592,
	.vratio=1944,
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
	.pix_100khz =CAPTURE_PCLK,
	.hratio=2592,
	.vratio=1944,
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

/*s Modify by Aries 10/03/19*/
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
	.conf_yuvhdpol   =0, 
	.conf_yuvvdpol   =1,
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
/*e Modify by Aries 10/03/19*/

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

/*s Added by Eric 100413 */
UINT8 Dummydata[6];
UINT8 lastDummydata[2];
UINT8 AgcDummySyncWrite;
/*e Added by Eric 100413 */

void
sensorTg_ov5633_Install()
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
}


/*------method definitions-----*/
		
static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {		

    	{0x30, 0x12, 0x80},  /* Soft Reset delay 100ms */

		{0x30, 0xfa, 0x00},
		{0x30, 0x0f, 0x00},
		{0x30, 0xb2, 0x32},
		{0x30, 0x84, 0x40},
		{0x30, 0x16, 0x01},
		{0x30, 0x8a, 0x25},
		{0x30, 0x13, 0xff},
		{0x30, 0x15, 0x03},
		{0x30, 0xbf, 0x02},
		{0x30, 0x69, 0x00},
		{0x30, 0x65, 0x50},
		{0x30, 0x68, 0x08},
		{0x30, 0xac, 0x05},
		{0x30, 0x9d, 0x00},
		{0x30, 0x9e, 0x24},
		{0x30, 0x98, 0x5c},
		{0x30, 0x91, 0x04},
		{0x30, 0x75, 0x22},
		{0x30, 0x76, 0x23},
		{0x30, 0x77, 0x24},
		{0x30, 0x78, 0x25},
		{0x30, 0xb5, 0x0c},
		{0x30, 0x90, 0x6f},
		{0x30, 0xf9, 0x11},
		{0x33, 0x11, 0x80},
		{0x33, 0x12, 0x1f},
		{0x31, 0x03, 0x10},
		{0x30, 0x5c, 0x01},
		{0x30, 0x5d, 0x29},
		{0x30, 0x5e, 0x00},
		{0x30, 0x5f, 0xf7},
		{0x30, 0x8d, 0x0b},
		{0x30, 0xad, 0x20},
		{0x30, 0x72, 0x0d},
		{0x30, 0x8b, 0x82},
		{0x33, 0x17, 0x9c},
		{0x33, 0x18, 0x22},
		{0x30, 0x25, 0x20},
		{0x30, 0x27, 0x08},
		{0x30, 0x29, 0x3f},
		{0x30, 0x2b, 0xa3},
		{0x33, 0x19, 0x22},
		{0x30, 0xa1, 0xc4},
		{0x30, 0x6a, 0x05},
		{0x33, 0x15, 0x22},
		{0x30, 0xae, 0x25},
		{0x33, 0x04, 0x40},
		{0x30, 0x99, 0x49},
		{0x30, 0xbe, 0x50},
		{0x30, 0x92, 0x02},
		{0x30, 0x0e, 0xb0},
		{0x30, 0x0f, 0x00},
		{0x30, 0x10, 0x03},
		{0x30, 0x11, 0x40},
		{0x30, 0xf8, 0x00},
		{0x30, 0xf8, 0x01},
		{0x30, 0xf8, 0x09},
		{0x30, 0x83, 0x00},
		{0x30, 0x86, 0x24},
		{0x30, 0x8c, 0x88},
		{0x30, 0xf9, 0x13},
		{0x30, 0xfb, 0x81},
		{0x30, 0x8a, 0x26},
		{0x30, 0x16, 0x02},
		                 
		{0x30, 0x69, 0x80},  /* BLC */ 
		{0x30, 0x9d, 0x04},
		{0x30, 0x9e, 0x24},
		{0x30, 0x71, 0x34},
		{0x33, 0x1f, 0x22},
		{0x30, 0x98, 0x54},
		{0x33, 0x1e, 0x07},
		{0x33, 0x22, 0x40},
		{0x33, 0x23, 0x40},
		{0x33, 0x1e, 0x05},
		{0x30, 0x48, 0x1f},
		{0x30, 0x49, 0x4e},
		{0x30, 0x4a, 0x20},
		{0x30, 0x4b, 0x02},
		{0x30, 0x4c, 0x00},
		{0x30, 0x4d, 0x20},
		{0x30, 0x4e, 0x42},
		{0x30, 0xa9, 0x02},
		{0x30, 0xa4, 0x11},
		{0x30, 0x13, 0xf7},
		{0x30, 0x14, 0x44},
		{0x30, 0x5c, 0x01},
		{0x30, 0x5d, 0x29},
		{0x30, 0x5e, 0x00},
		{0x30, 0x5f, 0xf7},
		{0x30, 0x4b, 0x00},
		{0x30, 0x4e, 0x62},
		{0x30, 0x4a, 0x30},
		{0x30, 0x4d, 0xff},
		{0x30, 0x58, 0x07},
		{0x30, 0x59, 0x16},
		{0x30, 0x5a, 0x1a},
		{0x30, 0xe9, 0x04},
		{0x30, 0x90, 0x6f},
		{0x30, 0x84, 0x44},
		{0x30, 0xe9, 0x04},
		{0x30, 0xb5, 0x1c},
		{0x33, 0x1f, 0x22},
		{0x30, 0x20, 0x02},
		{0x30, 0x21, 0x04},
		{0x30, 0x22, 0x07},
		{0x30, 0x23, 0x80},
		{0x30, 0x24, 0x00},
		{0x30, 0x25, 0x2c},
		{0x30, 0x26, 0x00},
		{0x30, 0x27, 0x14},
		{0x30, 0x28, 0x0a},
		{0x30, 0x29, 0x2f},
		{0x30, 0x2a, 0x07},
		{0x30, 0x2b, 0xa3},
		{0x30, 0x2c, 0x02},
		{0x30, 0x2d, 0x80},
		{0x30, 0x2e, 0x01},
		{0x30, 0x2f, 0xe0},
		{0x30, 0xb3, 0x09},
		{0x33, 0x01, 0xc1},
		{0x33, 0x13, 0xf1},
		{0x33, 0x14, 0x05},
		{0x33, 0x15, 0x04},
		{0x33, 0x16, 0x01},
		{0x33, 0x17, 0xe4},
		{0x33, 0x18, 0x20},
		{0x30, 0x0f, 0x01},
		{0x30, 0x22, 0x08},
		{0x30, 0x23, 0x10},
		                  
    		{0x33, 0x00, 0xd3},  /*disable AEC/AGC/AWB  */
		{0x33, 0x02, 0xe0},  
		{0x33, 0x08, 0x04},
		{0x33, 0x09, 0x00},
		{0x33, 0x0a, 0x04},
		{0x33, 0x0b, 0x00},
		{0x33, 0x0c, 0x04},
		{0x33, 0x0d, 0x00},
		{0x30, 0xae, 0x15},
		{0x33, 0x04, 0x4c},
		{0x33, 0x11, 0x81},
		{0x33, 0x22, 0x10},
		{0x33, 0x23, 0x10},
		{0x30, 0xfa, 0x01},
						 
		{0x30, 0x13, 0x00},													  

    		{0x30, 0xbf, 0x11},												
																		
		{0x30, 0x00, 0x30},												
		{0x30, 0x02, 0x02},												
																		
		{0x35, 0x06, 0x30},	/* 30=Hsync out , 20=HREF out */				
		{0x35, 0x00, 0x00},	/* [2]= PCLK, [1]=Href, [0]=Vsync polarity	*/
	
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

	sensorSerialAsyncWrite(cmdportid, 1, regdata[0]);  /* Sensor Soft Reset */
	sensorSerialFlush(cmdportid);
	tmrUsWait(100000);  /* delay 100 ms for reset*/

	sensorSerialBulkWrite(cmdportid,
		(sizeof(regdata)/sizeof(regdata[0])) - 1, regdata[1],0);

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
printf("~~~~~~~~~~power on OV5633\n");
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
	static const UINT8 regdata[][3] = {
		{0x30, 0xfa, 0x00},	 /*sw sleep on		 */

		{0x30, 0x20, 0x02},    /* VD width 516 lines*/ 			
		{0x30, 0x21, 0x04},

		{0x30, 0x22, 0x0c},    /* HD width 3100/2=1550 lines*/ 			
		{0x30, 0x23, 0x1c},
    	
		{0x30, 0x24, 0x00},					   
		{0x30, 0x25, 0x2c},					   
		{0x30, 0x26, 0x00},					   
		{0x30, 0x27, 0x14},					   
		{0x30, 0x28, 0x0a},					   
		{0x30, 0x29, 0x2f},					   
		{0x30, 0x2a, 0x07},					   
		{0x30, 0x2b, 0xa3},
		
		{0x30, 0x2c, 0x02},     /* Image Size */					   
		{0x30, 0x2d, 0x80},	       				   
		{0x30, 0x2e, 0x01},					   
		{0x30, 0x2f, 0xe0},					   
		
		{0x30, 0xb3, 0x09},					   
		{0x33, 0x01, 0xc1},					   
		{0x33, 0x13, 0xf1},					   
		{0x33, 0x14, 0x05},					   
		{0x33, 0x15, 0x04},					   
		{0x33, 0x16, 0x01},					   
		{0x33, 0x17, 0xe4},					   
		{0x33, 0x18, 0x20},					   
								 
		{0x30, 0xf8, 0x09},					   
		{0x30, 0x83, 0x00},					   
		{0x30, 0x86, 0x24},					   
		{0x30, 0x8c, 0x88},					   
		{0x30, 0xf9, 0x13},					   
		{0x30, 0xfb, 0x81},					   
		{0x30, 0x8a, 0x26},					   
		{0x30, 0x16, 0x02},					   
							 
		{0x30, 0xbf, 0x11},					   
		{0x30, 0x9a, 0xff},					   
		{0x30, 0xa2, 0x77},					   
		{0x30, 0xa8, 0x04},					   
		{0x30, 0x9d, 0x54},					   
		
		/* PLL settings */
		{0x30, 0x0e, 0xb0}, 			 
		{0x30, 0x0f, 0x11},     /* 00 for 30fps PCLK=96MHz , 10 for 15fps PCLK=48MHz, 20 for 10fps PCLK= 32 MHz, 30 for 7.5fps PCLK=24MHz*/
		{0x30, 0x10, 0x07}, 			 
		{0x30, 0x11, 0x40}, 			 							  
		
		{0x30, 0x89, 0x0a},     /* bayer pattern */
		
		{0x30, 0xb4, 0x00},  
		{0x35, 0x06, 0x30},     /* HSYNC Mode */
		{0x35, 0x01, 0x01}, 		
    	
		{0x35, 0x0e, 0x40},     /* Vsync from Sensor */
    	
    		{0x30, 0x71, 0x34},     /* OB adjust while changing gain */
    	    	/*s Modify by Aries 09/12/13*/
      	 	{0x30, 0x00, 0x00}, 
		{0x30, 0x02, 0x01},
		{0x30, 0x03, 0x08},	
		/*e Modify by Aries 09/12/13*/
    		{0x30, 0xfa, 0x01},     /* sw sleep off*/
	};
	sensor_event_wait(0x04);
	/*s Modify by Aries 09/12/13*/
	/*halPllFrontClkSet((MCLKDRV<<4)|PCLK_SRC, 24000);*//* Set master clock & pixel clock */
	/*e Modify by Aries 09/12/13*/
	sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
   /*s Add by Eric for wait OB to become normal 091221*/
	sensor_event_wait(0x04);
	sensor_event_wait(0x04);
   /*e Add by Eric for wait OB to become normal 091221*/
	
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
		
        {0x30, 0xfa, 0x00},	 /*sw sleep on		 */             
	  	{0x30, 0x20, 0x03}, /* VD width 1000 lines*/ 			
	    	{0x30, 0x21, 0xe8}, 			
	    	{0x30, 0x22, 0x07},  /* HD width 1800 pclk*/ 	/* Eric Lee remove left bar error 091221 */	
	    	{0x30, 0x23, 0x08}, 		
		
		{0x30, 0x24, 0x00},      /*Eric 100816 top left, (48,16) -> (16, 8) */                                                                                                      		
		{0x30, 0x25, 0x10},                                                                                                            		 			
		{0x30, 0x26, 0x00},                                                                                                            		
		{0x30, 0x27, 0x08},                                                                                                                	
		{0x30, 0x28, 0x0a},      /*Eric 100816 right bottom (2607, 1943) */                                                                                                      		
		{0x30, 0x29, 0x2f},                                                                                                            		
		{0x30, 0x2a, 0x07},                                                                                                            		
		{0x30, 0x2b, 0x97},
		
		{0x30, 0x2c, 0x05},  /* Image Size */				                                                                                                           		
		{0x30, 0x2d, 0x00},                                                                                                            		
		{0x30, 0x2e, 0x03},                                                                                                            		
		{0x30, 0x2f, 0xc2},      /*Eric 100816 960 -> 962 (compensate XGA_Voffset =2) */                                                                                                     		

		{0x30, 0xb3, 0x00},                                                                                                            		
		{0x33, 0x01, 0xc0},                                                                                                            			   
		{0x33, 0x13, 0xf0},                                                                                                            		
		{0x33, 0x14, 0x05},                                                                                                            		
		{0x33, 0x15, 0x02},                                                                                                            		
		{0x33, 0x16, 0x03},                                                                                                            		
		{0x33, 0x17, 0xc4},                                                                                                            		
		{0x33, 0x18, 0x22},                                                                                                            			                                                                                                                  				
		#if 0/*(_CATHY_HW_)*/ /*no flip mirro for 720bf Leo@20100619*/
		/*{0x30, 0xf8, 0x85},      */
		{0x30, 0xf8,0xc5},
		{0x30, 0x70,0x26},
		{0x30, 0x92,0x22},
		#else
		{0x30, 0xf8, 0x05},      
		#endif                                                                                                        		
		{0x30, 0x83, 0x08},                                                                                                           		
		{0x30, 0x86, 0x00},                                                                                                           		
		{0x30, 0x8c, 0x80},                                                                                                           		
		{0x30, 0xf9, 0x11},                                                                                                           		
		{0x30, 0xfb, 0x80},                                                                                                           		
		{0x30, 0x8a, 0x25},                                                                                                           		
		{0x30, 0x16, 0x01},                                                                                                           		
		                                                                                                                  		            
		{0x30, 0xbf, 0x11},					   
		{0x30, 0x9a, 0xff},					   
		{0x30, 0xa2, 0x77},					   
		{0x30, 0xa8, 0x04},					   
		{0x30, 0x9d, 0x54},					   
		/*s Add by Arie 10/04/30*/
    {0x30, 0x1e, 0x00},	 /*dummy line off		 */
    {0x30, 0x1f, 0x00},	 /*dummy line off		 */     			
		/*e Add by Arie 10/04/30*/

		                                                                                                                  		            
		/* PLL settings */
		{0x30, 0x0e, 0xae}, 			 
		{0x30, 0x0f, 0x11},     /* 00 for 30fps PCLK=96MHz , 10 for 15fps PCLK=48MHz, 20 for 10fps PCLK= 32 MHz, 30 for 7.5fps PCLK=24MHz*/
		{0x30, 0x10, 0x07}, 			 
		{0x30, 0x11, 0x40}, 			 						

        {0x30, 0x89, 0x0a},     /* bayer pattern */
        
		{0x30, 0xb4, 0x00},  
		{0x35, 0x06, 0x30},     /* HSYNC Mode */
		{0x35, 0x01, 0x01}, 		
    	
		{0x35, 0x0e, 0x40},     /* Vsync from Sensor */
    	
    	{0x30, 0x71, 0x34},     /* OB adjust while changing gain */

		{0x30, 0xfa, 0x01},     /* sw sleep off*/
    	
	};  
        
	sensor_event_wait(0x04);
	/*s Modify by Aries 09/12/13*/
	/*halPllFrontClkSet((MCLKDRV<<4)|PCLK_SRC, 24000);*//* Set master clock & pixel clock */
	/*e Modify by Aries 09/12/13*/
	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0]);

   /*s Add by Eric for wait OB to become normal 090921*/
	sensor_event_wait(0x04);
	sensor_event_wait(0x04);
   /*e Add by Eric for wait OB to become normal 090921*/
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
}

static void
sensorDoFuncSnapEarly0()
{


}

static void
sensorDoFuncSnapEarly1()
{
    UINT32 cmdportid;
	static const UINT8 regdata[][3] = {

		#if 0 /* (_CATHY_HW_) */
		/*{0x30, 0xf8, 0x80},*/
		{0x30, 0xf8, 0xc0}, 
		{0x30, 0x70,0x26},
		{0x30, 0x92,0x22},      
		#else
		{0x30, 0xf8, 0x00}, 
		#endif
		{0x30, 0x83, 0x08}, 
		{0x30, 0x86, 0x00}, 
		{0x30, 0x8c, 0x82}, 
		{0x30, 0xf9, 0x11}, 
		{0x30, 0xfb, 0x84}, 
		{0x30, 0x8a, 0x25}, 
		{0x30, 0x16, 0x01}, 
				  
		{0x30, 0x20, 0x07}, /* VD width 1988 lines*/		   
		{0x30, 0x21, 0xc4},		   
		{0x30, 0x22, 0x0c}, /* HD width 3240 pclk*/
		{0x30, 0x23, 0xa8},	
	 
		{0x30, 0x24, 0x00},		   
		{0x30, 0x25, 0x12},		 
		{0x30, 0x26, 0x00},		   
		{0x30, 0x27, 0x02},		   
		{0x30, 0x28, 0x0a},		   
		{0x30, 0x29, 0x3d}, 
		{0x30, 0x2a, 0x07},		   
		{0x30, 0x2b, 0xa5},		   
		
		{0x30, 0x2c, 0x0a}, /*image size */		   
		{0x30, 0x2d, 0x28},		   
		{0x30, 0x2e, 0x07},		   
		{0x30, 0x2f, 0xa0}, 
			
		{0x30, 0xb3, 0x00},		   
		{0x33, 0x01, 0xc0},			
		{0x33, 0x13, 0xf0},		   
		{0x33, 0x14, 0x0a},		   
		{0x33, 0x15, 0x2c},		   
		{0x33, 0x16, 0x07},		   
		{0x33, 0x17, 0xa4},		   
		{0x33, 0x18, 0x22}, 
	
	
		
		{0x30, 0x13, 0x00}, 
		/*0x30, 0xbf, 0x11,*/ 
	
	
		/* Frame Mode */
	
	
		{0x30, 0x13, 0x00}, 					  
		{0x30, 0x02, 0xff}, 					  
		{0x30, 0x03, 0xff}, 					  
		{0x30, 0x89, 0x0a}, 					  
		{0x30, 0xb1, 0x3f}, 					  
		{0x30, 0xb2, 0x02}, 					  
		{0x30, 0xb4, 0x20}, 					  
		{0x30, 0xe4, 0x01}, 					  
		{0x30, 0xe7, 0x00}, 					  
		
		{0x30, 0xec, 0x28}, 					  
		{0x30, 0xf6, 0xb0}, 					  
		{0x35, 0x0e, 0x44}, 					  
		{0x30, 0x71, 0x20},  /* OB adjust every frame */					  
	
		{0x30, 0xe9, 0x05}, 					  
		{0x33, 0x19, 0x22}, 					  
		{0x30, 0x1e, 0x00}, 					  
	                     
		{0x30, 0x1f, 0x40}, 					  
		 	
		{0x30, 0x93, 0x40},  
	    {0x30, 0x90, 0x62}, 
				
		{0x35, 0x06, 0x20}, /*HREF Mode*/ 
		{0x35, 0x01, 0x00},

	};
	profLogAdd(0, "sensorDoFuncSnapEarly1 S");
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;	
	sensorSerialAsyncWrite(cmdportid,sizeof(regdata)/sizeof(regdata[0]), regdata[0]);
	sensorSerialFlush(cmdportid);
/*
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
    sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
*/	
	profLogAdd(0, "sensorDoFuncSnapEarly1 E");

}


void sensor_frame_orig_vd_neg_cb_reg(void *f, UINT32 param);

static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{
	profLogAdd(0, "sensorDoFuncSnapFrexSet S");	
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX);

  
	if (appStillCapFlashStatusGet()==1){
		halFrontFlashCfgSet(0x00000000,0);
		halFrontFlashTrig();
	}
	profLogAdd(0, "sensorDoFuncSnapFrexSet E");	
}

static void
sensorDoFuncSnapBshutterBegin()
{
	profLogAdd(0, "sensorDoFuncSnapBshutterBegin S");	

	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);
	profLogAdd(0, "sensorDoFuncSnapBshutterBegin E");
    
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

}

static void
sensorDoFuncSnapReadoutField0()
{
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
    profLogAdd(0, "sensorDoFuncSnapReadoutField0 S");	
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	profLogAdd(0, "sensorDoFuncSnapReadoutField0 E");	
/*	
	*(volatile UINT8 *)0xb00090ae =0x51;
	*(volatile UINT8 *)0xb0009082 =0x22;
*/	
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
		profLogAdd(0, "sensorDoFuncSnapReadoutEnd S");	

	if (appStillCapFlashStatusGet()==2){
		halFrontFlashCfgSet(0x00000000,0);
		halFrontFlashTrig();
	}
	profLogAdd(0, "sensorDoFuncSnapReadoutEnd E");	
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
	printf("~~~~~~pow 5633\n");
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
/*s modified by Eric Lee 100413 */
if (((lastDummydata[1]!=0)&&(pdata[17]==0)) || ((lastDummydata[1]>0)&&(pdata[17]>0)))
{
	/* in this case, AGC and dummy line set timing need to synchronize */

/*	Dummydata[0]=pdata[6];
	Dummydata[1]=pdata[7];
	Dummydata[2]=pdata[8];
	Dummydata[3]=pdata[9];
	Dummydata[4]=pdata[10];
	Dummydata[5]=pdata[11];
*/
	Dummydata[0]=pdata[12];
	Dummydata[1]=pdata[13];
	Dummydata[2]=pdata[14];
	Dummydata[3]=pdata[15];
	Dummydata[4]=pdata[16];
	Dummydata[5]=pdata[17];



	AgcDummySyncWrite =1;	
	lastDummydata[1]= pdata[17]; 

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
	lastDummydata[1]= pdata[17];	
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
}
/*s modified by Eric Lee 100413 */

}
static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
}

#if SP5K_SENSOR_OV5633
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	expLine = expLine;
	#if 0
	UINT8 regData[4] ={
		0x30,0x12,0x00,0x00
	};
	regData[2] = (expLine>>8)&0xff;
	regData[3] = expLine&0xff;

	printf("expLine = %d\n",expLine);
	sensor_event_wait(0x04);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regData);
	#endif
}
#endif

