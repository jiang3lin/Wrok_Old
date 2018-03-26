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


#define MODEL_VERSION "$unp_tg_ov5610-1.00.02"

/*HISTORY
 * 1.00.02 20071221 anmin
 *   updating installed struct
 * prerelease
 *   modify code for the preview initial for switch XGA 06/06/20
 *   modify code for the VGA coverage ratio 06/05/22
 *   modify code for the SOP 06/03/27
 *   Ver0A 060220 add snap_cfa_pattern = BAYER_RGGB,halFrontImgSrcCfgSet(FRONT_IMG_SENSOR_FIELD_SEL,0x01);modify I2C SPI slave address,
 */

#include "common.h"
#include "../flow/hal_front.h"
#if SPCA5110 /****TODO*5110*tggpio temp workaround*/
#include "../flow/sysregs.h"
#else
#include "hal/hal_gpio.h"
#endif
#include "../flow/sensor_model.h"

/*#define I2C_BUF SENSOR_SIF1DMA_BUF */

#define I2C_BUF

#define SNAP_PREVIEW_DELAY_FRAME_NUM  2

/* Sensor image size definition here */
#define NO             0
#define NOUSE          0
#define DISABLE        0xFFFF

/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  480		/* preview vertical size*/

#define PREVIEW_XGA_HSIZE  1280		/* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  960		/* preview vertical size*/

#define CAPTURE_HSIZE  2592		/* capture horiz. size*/
#define CAPTURE_VSIZE  1944		/* capture vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL   1632	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   352		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  490		/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     308         /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     3		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         DISABLE 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         DISABLE	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         DISABLE /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         DISABLE /*7*/	/* preview Vd reshap rise*/

#define PREVIEW_XGA_LINETOTAL   1632	/* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   352		/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL  980		/* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_XGA_HOFFSET     308         /* preview Hd valid pixel data offset position*/
#define PREVIEW_XGA_VOFFSET     3		/* preview Vd valid line data offset position*/
/* Preview resh_XGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL         DISABLE 	/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE         DISABLE	/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL         DISABLE /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE         DISABLE /*7*/	/* preview Vd reshap rise*/

/* Capture default definition here */
#define CAPTURE_LINETOTAL   3000	/* capture Hd length*/
#define CAPTURE_LINEBLANK   408		/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  2000		/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  4		/* capture Vd sync length*/
#define CAPTURE_HOFFSET     252         /* capture Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     11   	/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         DISABLE	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         DISABLE	/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  NO	/* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL         DISABLE	/* capture Vd reshap fall*/
#define CAPTURE_VRESHAPE_RISE         DISABLE 	/* capture Vd reshap rise*/

/****************************Front Set ****************************/
/* tg,gpio function enable/in/out/mask signal definition here */
#define TGPIOAMASK	0x14000000   /*tggpio 0~31 reserve pin mask,'0'no use,'1'reserve*/
#define TGPIOBMASK	0x030c	     /*tggpio 32~47 reserve pin mask,'0'no use,'1'reserve*/
#define TGPIOAFUNC	0x00000000   /*tggpio 0~31 func set,'0'tg,'1'gipo*/
#define TGPIOBFUNC	0x0000	     /*tggpio 32~47 func set,'0'tg,'1'gipo*/
#define TGPIOADIR	0x04000000   /*tggpio 0~31 dir set,'0'in,'1'out*/
#define TGPIOBDIR	0x0300	     /*tggpio 32~47 dir set,'0'in,'1'out*/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
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
#ifdef FPGA
#define RGBADJ		1	     /*RGB input data sample phase adj "1"negedge "0"posedge*/
#else
#define RGBADJ		0	     /*RGB input data sample phase adj "1"negedge "0"posedge*/
#endif
/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_CFAPATTERN		BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	1                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	2	            /*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  1		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/
#define	CAPTURE_HDNUM_FRAME_START	11		/*the front interrupt line number of the snap new frame */
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
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF /*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#if SPCA5210
#define	TG_SPEED         2
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  8		/*ssc/spi register address numbers */


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
static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapDummy0();

static void sensorDoFuncSnapReadoutField0();
/*
static void sensorDoFuncSnapReadoutField1();
static void sensorDoFuncSnapReadoutField2();
*/
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();


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


#if PREVIEW_XGA_15FPS
static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	&sensorTgPreviewXGACfg,&sensorTgPreviewVGACfg, 
};
#else
static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	&sensorTgPreviewVGACfg,&sensorTgPreviewXGACfg, 
};
#endif


static sensorDoFunc_t sensorDoFuncSnapDummyTbl[] = {
#if !SNAP_DUMMYVD_NUM
	NULL,
#else
	sensorDoFuncSnapDummy0,
#endif
};

static sensorDoFunc_t sensorDoFuncSnapEarlyTbl[] = {
#if !SNAP_EARLY_NUM
	NULL,
#else
	sensorDoFuncSnapEarly0,
#endif
};

static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = sensorDoFuncSnapEarlyTbl,
		.snapExpBegin = sensorDoFuncSnapExpBegin,
		.snapBshutterBegin = NULL, /*sensorDoFuncSnapBshutterBegin,*/
		.snapBshutterFollow = NULL, /*sensorDoFuncSnapBshutterFollow,*/
		.snapBshutterEnd = NULL, /*sensorDoFuncSnapBshutterEnd,*/
		.snapLongExpBegin = NULL,
		.snapLongExpFollow = NULL,
		.snapLongExpEnd = NULL,
		.snapShortExp = sensorDoFuncSnapShortExp,
		.snapExpEnd = sensorDoFuncSnapExpEnd,
		.psnapDummy = sensorDoFuncSnapDummyTbl,
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
		.psnapEarly = NULL, /*sensorDoFuncSnapEarlyTbl,*/
		.snapExpBegin = NULL, /*sensorDoFuncSnapExpBegin,*/
		.snapBshutterBegin = NULL, /*sensorDoFuncSnapBshutterBegin,*/
		.snapBshutterFollow = NULL, /*sensorDoFuncSnapBshutterFollow,*/
		.snapBshutterEnd = NULL, /*sensorDoFuncSnapBshutterEnd,*/
		.snapLongExpBegin = NULL,
		.snapLongExpFollow = NULL,
		.snapLongExpEnd = NULL,
		.snapShortExp = NULL,
		.snapExpEnd = NULL, /*sensorDoFuncSnapExpEnd,*/
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

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg, &sensorTgSnapFrameCfg,
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
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
		.crop = NULL, /****TODO***/
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreviewVGACfg,
	.verStr = MODEL_VERSION,
};

void
sensorTg_ov5610_Install()
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
	static const UINT8 regdata[][2] I2C_BUF= {
		{0x12, 0x80,}, /*reset*/
		{0x12, 0x20,},
		{0x11, 0x80,},
		/*{0x03, 0x44,},*/
		{0x16, 0x00,},
		{0x17, 0x14,},
		{0x18, 0x64,},
		{0x19, 0x00,},
		/*{0x1a, 0x3c,},*/
		{0x09, 0x00,},
		{0x13, 0xc0,},
		{0x14, 0xc6,},
		#if 0
		{0x15, 0x42,}, /*anmin*/
		#else
		{0x15, 0x40,}, /*cslin*/
		#endif
		{0x0e, 0x00,},
		{0x1b, 0x00,},
		{0x32, 0x39,},
		{0x39, 0x00,},
		{0x4b, 0x08,},
	};

	/* TGGPIO configuration here */
	printf("@@@@Init OV5610 Enter.....\n");
#warning _DO_NOT_ turn on mclk directly!! app possibly uses external mclk

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	/*VD,HD input*/ /*default in, do nothing*/
	/*MCLK*/ /*tggpio7*/
	pfrontReg0->mclktosen2en = 1;
	pfrontReg0->tggpioosel &= ~0x0800;
	pfrontReg0->tggpiooe  |= 0x0800;
	printf("@@@@Init OV5610 Enter 2.....\n");


	#if SNESOR_API_EN /*USE sensor api*/
	sensorTgPllClkExtSet(24000000);
	#else
	*(volatile UINT8 *)0xb00000C8 = 0x01 ;
	*(volatile UINT8 *)0xb00000C9 = 0x03 ;
	*(volatile UINT16 *)0xb00000CA = 72 ;
	*(volatile UINT8 *)0xb00000CC = 2 ;
	*(volatile UINT8 *)0xb00000CD = 0x00 ;
	*(volatile UINT8 *)0xb00000CE = 0x00 ;
	*(volatile UINT8 *)0xb00000CF = 0x02 ;
	*(volatile UINT8 *)0xb00000C8 = 0x03 ;
	#endif
	*(volatile UINT8 *)0xb00000B4 = 0x02 ;/*COB: 0x01  Real chip:0x02*/


/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	halFrontClkSet(TGPLLEN,TGPLLCLKSET, MCLKDIV, PCLKDIV); /* Set master clock & pixel clock */
	halFrontClkCfg(MCLKMODE, PCLKMODE);
	printf("@@@@Init OV5610 Enter 3.....\n");
/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
	/*****TODO*****front reset after these???*/
	tmrUsWait(2000);
    printf("@@@@Init OV5610 Enter 4.....\n");
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, 1, regdata[0]);
	sensorSerialFlush(cmdportid);
	tmrUsWait(10000);
	sensorSerialBulkWrite(cmdportid,
		sizeof(regdata)/sizeof(regdata[0]) - 1, regdata[1],0);

	printf("@@@@@@@@@  initial OV5610 \n");
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	halFrontClkCfg(MCLKMODE, PCLKMODE);
	printf("@@@@@@@@@  initial OV5610 \n");
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
	static const UINT8 regdata[] I2C_BUF = {
	#if 0 /*always HSYNC, no HREF*/
		0x15, 0x42,/*RD19 0x02??*/
	#else
		0x15, 0x40,
	#endif
/*****TODO***multiple preview mode for VGA*/
		0x12, 0x20,
		0x1a, 0x3c,
		0x17, 0x14,
		0x18, 0x64,
		0x19, 0x00,
		0x03, 0x4c,
		0x1b, 0x00,
	};
	printf("sensorDoFuncPreviewVGABegin++\n");
	sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/2, &regdata[0]);
#if SNESOR_API_EN /*Desensor bayer mode*/
			sensor_event_wait(0x04);
			sensorDsenExtSet();
#endif	
	printf("sensorDoFuncPreviewVGABegin--\n");
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
	static const UINT8 regdata[] I2C_BUF= {
	#if 0 /*always HSYNC, no HREF*/
		0x15, 0x42,/*RD19 0x02??*/
	#else
		0x15, 0x40,
	#endif
/*****TODO***multiple preview mode for VGA*/
		0x12, 0x40,
		0x1a, 0x78,
		0x17, 0x14,
		0x18, 0x64,
		0x19, 0x00,
		0x03, 0x4c,
		0x1b, 0x00,
	};

	printf("sensorDoFuncPreviewXGABegin++\n");
	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/2, &regdata[0]);
#if SNESOR_API_EN /*Desensor bayer mode*/
			sensor_event_wait(0x04);
			sensorDsenExtSet();
#endif
	printf("sensorDoFuncPreviewXGABegin--\n");
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
static void
sensorDoFuncSnapEarly0()
{
	UINT32 cmdportid;
	static const UINT8 regdata[] I2C_BUF= {
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
#if 0
static void
sensorDoFuncSnapBshutterBegin()
{
	/*front_snap_bshutter_begin_frame_set*/
	/*front_snap_bshutter_begin_tg_set*/
}
static void
sensorDoFuncSnapBshutterFollow()
{
	/*front_snap_bshutter_follow_frame_set*/
	/*front_snap_bshutter_follow_tg_set*/
}
static void
sensorDoFuncSnapBshutterEnd()
{
	/*front_snap_bshutter_end_frame_set*/
	/*front_snap_bshutter_end_tg_set*/
}
#endif

static void
sensorDoFuncSnapShortExp()
{
	/*front_snap_short_exp_frame_set()*//*slow shutter, 1 longer frame*/
	/*front_snap_short_exp_tg_set*/
	/*do 1 longer frame exp, historical term "bshutter", if needed*/
#if 1	
	UINT32 cmdportid;
	static const UINT8 regdata[] I2C_BUF= {
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

#if SNESOR_API_EN /*Desensor bayer mode*/
	sensorDsenExtSet();
#else
	*(volatile UINT8 *)0xb0002271 = 0x00;
	*(volatile UINT8 *)0xb0002270 = 0x00;
	*(volatile UINT16 *)0xb0002272 = 0x10;
	*(volatile UINT16 *)0xb0002276 = CAPTURE_LINEBLANK/2;
	*(volatile UINT16 *)0xb0002278 = CAPTURE_HSIZE;
	*(volatile UINT8 *)0xb000227F = 0x01;
	*(volatile UINT8 *)0xb0002271 = 0x01;	
	*(volatile UINT8 *)0xb0002270 = 0x81;
#endif


	*(volatile UINT8 *)0xb0002004 = 0xFF;

	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(regdata)/2, &regdata[0]);
	sensorSyncSigSet(&sensorSyncSigSnapCfg);		
#endif

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
}
static void
sensorDoFuncSnapDummy0()
{
}

static void
sensorDoFuncSnapReadoutField0()
{
	
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/

}

static void
sensorDoFuncSnapFieldEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapEnd()*/
	/*throw user msg??*/
}

/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{
	/*panasonic TG TRG reset*/
	/*direct m-sh open*/
	/*direct vsubctl off*/
	/*immediate inform readout all end*/
	profLogPrintf(1,"TEST SIZE (%d x %d)",READ16(0xb00090cc),READ16(0xb00090ce));
}

/*what data structure should sensor_do blocks be???
 * a direct function call? no,
 * 1. need to provide function code for compile even if no such block
 * 2. need extra code for all the do'es to check if target function is null
 */

/*****TODO***/
/*exp tables*/
#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
static const UINT8 exptbl_stream[]
COMMON_ALIGNED = {
0x02,0x05,0xa0,0x00,
0x30,
8,0x10,/*idx0 */
0x02,0x10,0x00,
0x04,0x00,
0x45,0x00,
0x2d,0x00,
0x2e,0x00,
53,0x04,0x00,0x05,
0x00,0x03,0x08,0x08,
0x01,0x00,0x08,0x03,
0x02,0x0b,0x08,0x05,
0x03,0x10,0x08,0x08,
0x04,0x18,0x08,0x08,
/*ev table sec 0 53*/
0xa0,0x12,0xb6,0x10,0xd6,0x11,0xec,0x0f,0x15,0x11,0x2b,0x0f,0x5b,0x10,0x71,0x0e,0xaa,0x0f,0xc0,0x0d,0x00,0x0f,0x16,0x0d,
0x5d,0x0e,0x73,0x0c,0xc1,0x0d,0xd7,0x0b,0x2c,0x0d,0x42,0x0b,0x9d,0x0c,0xb3,0x0a,0x14,0x0c,0x2a,0x0a,0x91,0x0b,0xa7,0x09,
0x13,0x0b,0x29,0x09,0x9b,0x0a,0xb1,0x08,0x28,0x0a,0x3e,0x08,0xba,0x09,0xd0,0x07,0x50,0x09,0x66,0x07,0xeb,0x08,0x01,0x07,
0x8b,0x08,0xa1,0x06,0x2e,0x08,0x44,0x06,0xd5,0x07,0xeb,0x05,0x80,0x07,0x96,0x05,0x2f,0x07,0x45,0x05,0xe1,0x06,0xf7,0x04,
0x96,0x06,0xac,0x04,0x4f,0x06,0x65,0x04,0x0a,0x06,0x20,0x04,0xc9,0x05,0xdf,0x03,0x8a,0x05,0xa0,0x03,0x4e,0x05,0x64,0x03,
0x14,0x05,0x2a,0x03,0xdd,0x04,0xf3,0x02,0xa8,0x04,0xbe,0x02,0x76,0x04,0x8c,0x02,0x46,0x04,0x5c,0x02,0x17,0x04,0x2d,0x02,
0xeb,0x03,0x01,0x02,0xc0,0x03,0xd6,0x01,0x98,0x03,0xae,0x01,0x71,0x03,0x87,0x01,0x4b,0x03,0x61,0x01,0x28,0x03,0x3e,0x01,
0x05,0x03,0x1b,0x01,0xe5,0x02,0xfb,0x00,0xc5,0x02,0xdb,0x00,0xa7,0x02,0xbd,0x00,0x8a,0x02,0xa0,0x00,0x6f,0x02,0x85,0x00,
0x54,0x02,0x6a,0x00,0x3b,0x02,0x51,0x00,0x23,0x02,0x39,0x00,0x0c,0x02,0x22,0x00,0xf6,0x01,0x0c,0x00,

107,0x02,0x35,0x02,
0x00,0x03,0x08,0x08,
0x01,0x00,0x08,0x03,
/*ev table sec1 107*/
0xe0,0x01,0xcc,0x01,0xb9,0x01,0xa6,0x01,0x94,0x01,0x83,0x01,0x73,0x01,0x63,0x01,0x54,0x01,0x45,0x01,0x38,0x01,0x2a,0x01,
0x1e,0x01,0x12,0x01,0x06,0x01,0xfb,0x00,0xf0,0x00,0xe6,0x00,0xdd,0x00,0xd3,0x00,0xca,0x00,0xc2,0x00,0xba,0x00,0xb2,0x00,
0xaa,0x00,0xa3,0x00,0x9c,0x00,0x95,0x00,0x8f,0x00,0x89,0x00,0x83,0x00,0x7e,0x00,0x78,0x00,0x73,0x00,0x6f,0x00,0x6a,0x00,
0x65,0x00,0x61,0x00,0x5d,0x00,0x59,0x00,0x55,0x00,0x52,0x00,0x4e,0x00,0x4b,0x00,0x48,0x00,0x45,0x00,0x42,0x00,0x3f,0x00,
0x3c,0x00,0x3a,0x00,0x38,0x00,0x35,0x00,0x33,0x00,0x31,0x00,0x2f,0x00,0x2d,0x00,0x2b,0x00,0x29,0x00,0x27,0x00,0x26,0x00,
0x24,0x00,0x23,0x00,0x21,0x00,0x20,0x00,0x1e,0x00,0x1d,0x00,0x1c,0x00,0x1b,0x00,0x1a,0x00,0x19,0x00,0x18,0x00,0x17,0x00,
0x16,0x00,0x15,0x00,0x14,0x00,0x13,0x00,0x12,0x00,0x12,0x00,0x11,0x00,0x10,0x00,0x0f,0x00,0x0f,0x00,0x0e,0x00,0x0e,0x00,
0x0d,0x00,0x0d,0x00,0x0c,0x00,0x0c,0x00,0x0b,0x00,0x0b,0x00,0x0a,0x00,0x0a,0x00,0x09,0x00,0x09,0x00,0x09,0x00,0x08,0x00,
0x08,0x00,0x08,0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x05,0x00,0x01,0x00,
/*Vtable 160*/
0x50,0x00,0x00,0x02,
160,0x00,0x40,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
0x01,0x01,0x01,0x01,0x01,0x00,0x01,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x02,0x01,0x02,0x01,0x02,0x00,0x01,0x02,
0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x01,0x05,0x04,0x02,0x00,0x04,0x02,0x06,0x03,0x07,0x03,0x07,0x03,0x07,0x03,
0x07,0x01,0x05,0x09,0x02,0x06,0x0a,0x02,0x06,0x0a,0x0e,0x03,0x07,0x0b,0x0f,0x03,
/*still mode 161*/
161,0x00,
0x40,
0x00,0x10,
0x03,0x10,0x00,
0x04,0x00,
0x45,0x00,
0x2d,0x00,
0x2e,0x00,
/*ev table sec0 48*/
48,0x02,0x20,0x02,
0x00,0x03,0x08,0x08,0x01,0x00,0x08,0x03,
0xd0,0x07,0x7b,0x07,0x2a,0x07,0xdc,0x06,0x92,0x06,0x4a,0x06,0x06,0x06,0xc5,0x05,0x86,0x05,0x4a,0x05,0x11,0x05,0xda,0x04,
0xa5,0x04,0x73,0x04,0x43,0x04,0x14,0x04,0xe8,0x03,0xbe,0x03,0x95,0x03,0x6e,0x03,0x49,0x03,0x25,0x03,0x03,0x03,0xe2,0x02,
0xc3,0x02,0xa5,0x02,0x88,0x02,0x6d,0x02,0x53,0x02,0x39,0x02,0x21,0x02,0x0a,0x02,0xf4,0x01,0xdf,0x01,0xcb,0x01,0xb7,0x01,
0xa4,0x01,0x93,0x01,0x82,0x01,0x71,0x01,0x62,0x01,0x53,0x01,0x44,0x01,0x36,0x01,0x29,0x01,0x1d,0x01,0x11,0x01,0x05,0x01,
/*ev table sec1 81*/
81,0x01,0x50,0x02,
0x00,0x03,0x08,0x08,0x01,0x00,0x08,0x03,
0xfa,0xef,0xe5,0xdc,0xd2,0xc9,0xc1,0xb9,0xb1,0xa9,0xa2,0x9b,0x95,0x8e,0x88,0x83,0x7d,0x78,0x73,0x6e,0x69,0x65,0x60,0x5c,
0x58,0x55,0x51,0x4e,0x4a,0x47,0x44,0x41,0x3f,0x3c,0x39,0x37,0x35,0x32,0x30,0x2e,0x2c,0x2a,0x29,0x27,0x25,0x24,0x22,0x21,
0x1f,0x1e,0x1d,0x1b,0x1a,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x13,0x12,0x11,0x10,0x10,0x0f,0x0e,0x0e,0x0d,0x0d,0x0c,0x0c,
0x0b,0x0b,0x0a,0x0a,0x09,0x09,0x09,0x08,0x08,
/*ev table sec1 32*/
32,0x02,0x00,0x02,
0x00,0x03,0x08,0x08,0x01,0x00,0x08,0x03,
0xd0,0x07,0x7b,0x07,0x2a,0x07,0xdc,0x06,0x92,0x06,0x4a,0x06,0x06,0x06,0xb1,0x07,0x5e,0x07,0x0e,0x07,0xc1,0x06,0x78,0x06,
0x32,0x06,0xee,0x05,0xae,0x05,0x70,0x05,0xd0,0x07,0x7b,0x07,0x2a,0x07,0xdc,0x06,0x92,0x06,0x4a,0x06,0x06,0x06,0xc5,0x05,
0x86,0x05,0x4a,0x05,0x11,0x05,0xda,0x04,0xa5,0x04,0x73,0x04,0x43,0x04,0x14,0x04,
/*Vtable 32*/
0x01,0x00,32,0x01,0x00,0x02,
0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x00,0x00,0x40,
};
#endif

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
	printf("sensorCfgInit++\n");
	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);
	/* if support burst
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_BURST; */
	/* if support seq
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_SEQ; */

	/*call this if this sensor driver supports exp table*/
#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
	sensor_exptbl_create(exptbl_stream, sizeof(exptbl_stream));
#else
	sensor_exptbl_init();
#endif
	printf("sensorCfgInit--\n");
}
static void
sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;

	printf("sensorCfgStop++\n");
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	printf("sensorCfgStop--\n");
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
}

#if SP5K_SENSOR_OV5610
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
