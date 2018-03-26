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
 *  Author: CS Lin                                                   *
 *                                                                        *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_ov5620-0.00.01"

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
/*#include "../sensor_util.h"*/
#include "api/sp5k_global_api.h"
/*#include "../../../HostFw/APP/app_Gpio.h"*/
#include "../sensor_util.h"


#undef _EVB_SPE7001_HW_

#define _EVB_SPE7001_HW_ 1

#define SHUT_CTRL_TIMER  0


/* Sensor image size definition here */
#define NO             0
#define NOUSE          0
#define DISABLE        0xFFFF

#define BIT0 (1<<0)
#define BIT1 (1<<1)
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT29 (1<<29)

#ifdef _EVB_SPE7001_HW_
#define SENSOR_FREX     BIT1
#define SENSOR_EXP_STB  BIT4
#define SENSOR_PWRDN   BIT0
#define SENSOR_RESET   BIT29
#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_EXP_STB|SENSOR_PWRDN
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_EXP_STB|SENSOR_PWRDN
#endif


/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  480		/* preview vertical size*/

#define PREVIEW_XGA_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  960		/* preview vertical size*/

#define CAPTURE_HSIZE  (2592)		/* capture horiz. size*/
#define CAPTURE_VSIZE  (1944)		/* capture vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL   1612	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   352		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  505		/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     273         /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     13		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/

#define PREVIEW_XGA_LINETOTAL   1670	/* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   352		/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL  978		/* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_XGA_HOFFSET     273         /* preview Hd valid pixel data offset position*/
#define PREVIEW_XGA_VOFFSET     9		/* preview Vd valid line data offset position*/
/* Preview resh_XGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL         2 	/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE         3	/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL         2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE         3 /*7*/	/* preview Vd reshap rise*/
                                
/* Capture default definition here */
#define CAPTURE_LINETOTAL   3252	/* capture Hd length*/
#define CAPTURE_LINEBLANK   408		/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  1987		/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  4		/* capture Vd sync length*/
#define CAPTURE_HOFFSET     273         /* capture Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     (11+4)   	/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         52	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         54	/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  NO	/* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL         2	/* capture Vd reshap fall*/
#define CAPTURE_VRESHAPE_RISE         3 	/* capture Vd reshap rise*/

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
#define PIXELPHASE	6	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/

#define PREVIEW_VGA_PCLK        24000
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
#define	PREVIEW_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_CFAPATTERN		BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	1	            /*for typical CCD==n_dummy + n_readout + 1*/

/*s Modify by Aries 09/08/28*/
#define	SNAP_EARLY_NUM		1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
/*e Modify by Aries 09/08/28*/
#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/
#define	CAPTURE_HDNUM_FRAME_START	(11+24)	/*the front interrupt line number of the snap new frame */
#define	PREVIEW_HDNUM_FRAME_START  11		/*the front interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_END	1957    	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	1958    	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END	486		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT	400		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_FRAME_END	966		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_CMDPORT	900		/*the command port interrupt line number of the preview new frame*/

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
#define	TG_SPI_I2C_MODE  0x60      	       /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#if SPCA5210
#define	TG_SPEED         2
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  8		/*ssc/spi register address numbers */

#define CAP_FRAME_MODE  1


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
/*s Mask by Aries 09/08/28*/
/*static void sensorDoFuncSnapEarly1();*/
/*e Mask by Aries 09/08/28*/

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
	.cfa_pattern = PREVIEW_CFAPATTERN,
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
	.cfa_pattern = PREVIEW_CFAPATTERN,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	  &sensorTgPreviewXGACfg, &sensorTgPreviewVGACfg, 
};

static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0,
			/*sensorDoFuncSnapEarly1,*/
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
};

/****TODO****frame snap mode*/
static const sensorTgSnapCfg_t sensorTgSnapFrameCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0,
			/*sensorDoFuncSnapEarly1,*/
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

void
sensorTg_ov5620_Install()
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
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
	tmrUsWait(10000);  /*1ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_FML,
				  SENSOR_PWRDN|SENSOR_FREX|SENSOR_EXP_STB,0x00000000);

	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_RST_ON);
	
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, SENSOR_RESET, 0x00000000);
	tmrUsWait(1000);  /*1ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, SENSOR_RESET, 0xFFFFFFFF);
	
	/****TODO****sensor power*/
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_ON);
}


/*------method definitions-----*/
static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
	static const UINT8 regdata[] = {
			0x12, 0x80,
			0x12, 0x10,
			0x13, 0x00,
			0x11, 0x00,
			0x15, 0x40,
			0x10, 0x1E,
			0x3B, 0x07,
			0x5F, 0x02,
			0x60, 0xA2,
			0x61, 0xF5,
			0x5B, 0x40,
			0x39, 0x07,
			0x53, 0x02,
			0x54, 0x60,
			0x04, 0x20,
			0x27, 0x04,
			0x3D, 0x40,
			0x36, 0x00,
			0xC5, 0x04,
			0x4E, 0x00,
			0x4F, 0x93,
			0x50, 0x7B,
			0xCA, 0x0C,
			0xCB, 0x0F,
		
			0x39, 0x07,
			0x4A, 0x10,
			0x3E, 0x0A,
			0x3D, 0x00,
		
			0x89, 0x20,
			0x83, 0x80,
			0xB7, 0x9D,
			0xB6, 0x11,
			0xB5, 0x55,
			0xB4, 0x00,
			0xA9, 0xF0,
			0xA8, 0x0A,
			0xB8, 0xF0,
			0xB9, 0xF0,
			0xBA, 0xF0,
		
			0x81, 0x07,
			0x63, 0x44,
		
			0x14, 0x60,
			0x33, 0x75,
			0x2C, 0x00,
			0x09, 0x00,
		
			0x35, 0x30,
			0x27, 0x04,
			0x3C, 0x07,
			0x3A, 0x0A,
			0x3B, 0x07,
			0x01, 0x40,
			0x02, 0x40,
			
			0x16, 0x40,
			0x52, 0xB0,
			0x51, 0x83,
		
			0x21, 0xBB,
			0x22, 0x10,
			0x23, 0x03,
			0x35, 0x38,
		
			0x20, 0x90,
			0x28, 0x30,
			0x73, 0xE1,
			0x6C, 0x00,
			0x6D, 0x80,
			0x6E, 0x00,
			0x70, 0x04,
			0x71, 0x00,
			0x8D, 0x04,
			0x64, 0x00,
			0x65, 0x00,
			0x66, 0x00,
			0x67, 0x00,
			0x68, 0x00,
			0x69, 0x00,
			0x6A, 0x00,
			0x6B, 0x00,
		
			0x71, 0x94,
			0x74, 0x20,
		
			0x80, 0x09,
		
			0x85, 0xC0,
	};

/* TGGPIO configuration here */
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	/*VD,HD input*/ /*default in, do nothing*/
	/*MCLK*/ /*tggpio7*/
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	TGStartUpProc();

	#ifdef _EVB_SPE7001_HW_
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	#endif

/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	halFrontClkSet(TGPLLEN,TGPLLCLKSET, MCLKDIV, PCLKDIV);
	halPllFrontClkSet((MCLKDRV<<4)|PCLK_SRC, 24000);/* Set master clock & pixel clock */
	/*halFrontClkCfg(MCLKMODE, PCLKMODE);*/
	#if SPCA5210
/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
	#endif

	/*****TODO*****front reset after these???*/
	tmrUsWait(2000);

	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, 1, &regdata[0]);
	sensorSerialFlush(cmdportid);
	tmrUsWait(10000);
	sensorSerialBulkWrite(cmdportid,
		sizeof(regdata)/sizeof(regdata[0])/2 - 1, &regdata[2],0);

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
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
	static const UINT8 regdata[] = {
			0x12, 0x10,
					   
			0x38, 0x10,
			0x2A, 0x10,
			0x2B, 0x4C,
					   
		/*	0x0C, 0x0A,*/
			0x0C, 0x0E,
			0x5F, 0x02,
			0x61, 0x3D,
					   
			0x4E, 0x00,
			0x4F, 0x94,
			0x50, 0x7C,
			0xCA, 0x02,
			0xCB, 0x03,
					   
			0x0D, 0x96,
			0x17, 0x12,
			0x19, 0x01,
			0x03, 0x40,
			0x53, 0x0E,
			0x54, 0x61,
			0x46, 0x11,
			0x37, 0x00,
			0x44, 0xC0,
					   
		/*	0x04, 0x30,*/
			0x8D, 0x64,
		/*	0x04, 0xB0,
			0x04, 0x60, */
			0x04, 0x20,
			0x15, 0x40,
			0x11, 0x00,
			
			/*		   
			0x3E, 0x0A,
			0x3D, 0x00,
			0x8D, 0x44,
			0x10, 0x38,
			0x14, 0x66,
			*/

	};
	sensor_event_wait(0x04);
	sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/2, &regdata[0]);
}
static void
sensorDoFuncPreviewXGABegin()
{
 /*s Modify by Aries 09/08/28*/
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	static const UINT8 regdata[] = {
		0x12, 0x40,

		0x38, 0x10,
		0x2A, 0x10,
		0x2B, 0x86,
		0x4E, 0x10,
		0x4F, 0x24,
		0x50, 0xF4,
		0xCA, 0x02,
		0xCB, 0x03,
		0x0C, 0x0A,
			   
		0x0d, 0xc6,
		0x17, 0x12,
		0x19, 0x01,
		0x03, 0x40,
				   
		0x46, 0x11,
			   
		0x37, 0x00,
		0x44, 0x48,
			   
		0x04, 0x30,
		0x8d, 0x64,

		/*0x04, 0xB0,
		0x04, 0x60,*/
		0x15, 0x40,
		0x11, 0x00,

		/*
		0x3E, 0x14,
		0x3D, 0x00,
		0x8D, 0x44,
		0x10, 0x38,
		0x14, 0x66,	
		*/

		/*	DBLC   */	
		
		0x70, 0x04,
		0x71, 0x00,
		0x8D, 0x04,
		0x64, 0x00,
		0x65, 0x00,
		0x66, 0x00,
		0x67, 0x00,
		0x68, 0x00,
		0x69, 0x00,
		0x6A, 0x00,
		0x6B, 0x00,
		
		0x71, 0x94, 
		0x74, 0x20,		
	};

	sensor_event_wait(0x04);
	halPllFrontClkSet((MCLKDRV<<4)|PCLK_SRC, 24000);/* Set master clock & pixel clock */
	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/2, &regdata[0]);
/*e Modify by Aries 09/08/28*/	
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


/*s Modify by Aries 09/08/28*/
static void
sensorDoFuncSnapEarly0()
{
	static const UINT8 regdata[] = {
		0x12, 0x00,
		0x17, 0x12,
		0x18, 0xFF,
		0x19, 0x00,
		0x1A, 0xFF,
		0x03, 0x07,
		0x11, 0x00, /*0x11, 0x40,*/
		
		0x3E, 0x0A,
		0x3D, 0x00,
		
		0x0C, 0x0D,
		0x38, 0x90,
		
		0x46, 0x30,
		0x5F, 0x02,
		0x60, 0xA2,
		0x61, 0xF5,
		0x0D, 0x06,
		
		0x05, 0x11,
		0x06, 0x11,
		0x07, 0x11,
		0x08, 0x11,
		
		0x2A, 0x00,
		0x2B, 0x00,
		
		0x37, 0x04,
		0x44, 0x00,
		0x45, 0x00,
		0x46, 0x00,
		
		0x4F, 0x93,
		0x50, 0x7B,
		
		0x53, 0x02,
		0x54, 0x60,
		
		0x64, 0x00,
		0x65, 0x00,
		0x66, 0x00,
		0x67, 0x00,
		
		0x7F, 0x00,
		0x8D, 0x64, /*0x8D, 0x04,*/
		0xCA, 0x0C,
		0xCB, 0x0F,

        0x2D, 0x00,  /* dummy line =0 on frame mode */
        0x2E, 0x00, 

		
	};
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_xtg,sizeof(regdata)/2, &regdata[0]);
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
}
/*e Modify by Aries 09/08/28*/


#if SHUT_CTRL_TIMER
void
sensorDoFuncSnapLensClose()
{
	lensMShutSnapCloseCBF(0,0,NULL);
}

void
sensorDoFuncSnapFrexDisable()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0);   
}
#endif


void
sensorDoFuncSnapStbLow()
{
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_EXP_STB, 0x00);
}

void sensor_frame_orig_vd_neg_cb_reg(void *f, UINT32 param);

static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_EXP_STB, SENSOR_EXP_STB);

  sp5kTimerUsOneShotReg(0, 200,sensorDoFuncSnapStbLow);   /* will: real exposure start ; 2009/8/21 */

	#if SHUT_CTRL_TIMER
	sp5kTimerUsOneShotReg(0, appStillCapExpTimeGet()*10,sensorDoFuncSnapFrexDisable);
	sp5kTimerUsOneShotReg(0, appStillCapShutCloseTimeGet()*10,sensorDoFuncSnapLensClose);
	#endif


	if (appStillCapFlashStatusGet()==1){
		halFrontFlashCfgSet(0x00000000,0);
		halFrontFlashTrig();
	}
	/*profLogAdd(0, "sensorDoFuncSnapFrexSet4");*/

}

static void
sensorDoFuncSnapBshutterBegin()
{
	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);
	#if 0
	sensor_event_wait(0x04);
	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);
	#endif
}

static void
sensorDoFuncSnapBshutterFollow()
{
	#if 0
	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);

	#endif
	/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, SENSOR_FREX);*/
	/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_EXP_STB, SENSOR_EXP_STB);*/
	/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_EXP_STB, 0);*/
}

static void
sensorDoFuncSnapBshutterEnd()
{
	#if KIT_SHUT_CTRL_TIMER

	#else
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

}

static void
sensorDoFuncSnapReadoutField0()
{
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
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

	sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 0);
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, 1, 1);
	
	if (appStillCapFlashStatusGet()==2){
		halFrontFlashCfgSet(0x00000000,0);
		halFrontFlashTrig();
	}

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
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
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
