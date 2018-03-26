/**************************************************************************
 *
 *        Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C. 
 *
 **************************************************************************/
#define EVTBL_TEST_CODE 0
#define READ16(_reg_)           (*((volatile UINT16 *)(_reg_)))


#define MODEL_VERSION "$unp_tg_mi1600-0.00.01"


#include "common.h"
#include "../flow/hal_front.h"
#if SPCA5110 /****TODO*5110*tggpio temp workaround*/
#include "../flow/sysregs.h"
#else
#include "hal/hal_gpio.h"
#endif
#include "../flow/sensor_model.h"

/* Sensor image size definition here */
#define NO             0
#define NOUSE          0
#define DISABLE        0xFFFF



/*****TODO***multiple preview mode for VGA*/

#define PREVIEW_VGA_HSIZE  720		/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  540		/* preview vertical size*/

#define PREVIEW_XGA_HSIZE  1280 	/* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  960		/* preview vertical size*/

#define CAPTURE_HSIZE  1440		/* capture horiz. size*/
#define CAPTURE_VSIZE  1080		/* capture vertical size*/


/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL   1453		/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   352		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  573		/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  4			/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     719         	/* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     4			/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         140 		/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         141		/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         DISABLE 	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         DISABLE 	/* preview Vd reshap rise*/

/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_XGA_LINETOTAL   2016	/* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   713	/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL 966	/* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  70	/* preview Vd sync length*/
#define PREVIEW_XGA_HOFFSET     719		/* preview Hd valid pixel data offset position*/
#define PREVIEW_XGA_VOFFSET     4		/* preview Vd valid line data offset position*/
/* Preview resh_XGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL         140 		/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE         141		/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL         DISABLE 	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE         DISABLE 	/* preview Vd reshap rise*/

/* Capture default definition here */
#define CAPTURE_LINETOTAL   2188		/* capture Hd length*/
#define CAPTURE_LINEBLANK   718			/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  1105		/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  70			/* capture Vd sync length*/
#define CAPTURE_HOFFSET     723         		/* capture Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     6   			/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         140	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         141	/* capture Hd reshap rise*/
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
#define MCLKDIV		4			/*MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV		0	     	/*pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE	0			/*master clk phase*/
#define PIXELPHASE	6   		/*pixel clk phase*/
#define ADCKPHASE	0	    	/*adck clk phase*/
#define RGBADJ		0	     	/*RGB input data sample phase adj "1"negedge "0"posedge*/

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_CFAPATTERN	BAYER_GRBG          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_CFAPATTERN		BAYER_GRBG         /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0					/*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1					/*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	1					/*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		1					/*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/	
#define	SNAP_FRAME_EARLY_NUM		2	 		/*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/		

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */ 
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */ 
#define	SKIP_SNAP_READOUTVD_EN  		0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  	1		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 				0		/*sony FALSE;panasonic/ITG TRUE*/

#define	CAPTURE_HDNUM_FRAME_START		11		/*the front interrupt line number of the snap new frame */
#define	PREVIEW_HDNUM_FRAME_START   	11		/*the front interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_END	     1082   	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT		400   	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END	542		/*the rear interrupt line number of the preview new frame*/
#define   PREVIEW_VGA_HDNUM_CMDPORT	30		/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_XGA_HDNUM_FRAME_END	964/*inproper cause no AE msg 962*//*966*/		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/

#define	EXP_FRAME_NUM       	2      		/*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/
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
#define	TG_SPI_I2C_MODE  0xB8    	       /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF /*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#if SPCA5210
#define	TG_SPEED         FRONT_SSC_CLK_96K
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_96K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  16		/*ssc/spi register data numbers */
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
	#if 1
	&sensorTgPreviewVGACfg, &sensorTgPreviewXGACfg,
	#else
	&sensorTgPreviewXGACfg, &sensorTgPreviewVGACfg,
	#endif
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
		.psnapEarly = (sensorDoFunc_t []) { 
			sensorDoFuncSnapEarly0,
			sensorDoFuncSnapEarly1,
		},
		.snapExpBegin = NULL,
		.snapBshutterBegin = NULL,
		.snapBshutterFollow = NULL,
		.snapBshutterEnd = NULL,
		.snapLongExpBegin = NULL,
		.snapLongExpFollow = NULL,
		.snapLongExpEnd = NULL,
		.snapShortExp = NULL,
		.snapExpEnd = NULL,
		.psnapDummy = NULL,
		.psnapReadoutField = (sensorDoFunc_t []) {
			sensorDoFuncSnapReadoutField0,
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
	.conf_vdpol      = 1,
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
sensorTg_mi1600_Install()
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
	static const UINT8 regdata[][3] = {

		{0x0D, 0x00, 0x01}, /* reset */
		{0x0D, 0x00, 0x00}, /* reset */

		{0x10, 0x00, 0x51},/* PLL_CONTROL */
		{0x10, 0x00, 0x50},/* PLL_CONTROL */
		{0x10, 0x00, 0x51},/* PLL_CONTROL */
		{0x11, 0x3f, 0x03},/* PLL_CONTROL_1  PCLK(25MHz) = MCLK * High_8Bit /(Low_6bit + 1)/6*/
		{0x10, 0x00, 0x53},/* PLL_CONTROL */

		{0x20, 0xc0, 0x47}, /* READ_MODE_2	mirror */ 
		{0x41, 0x00, 0x00}, /* CLAMP_VOLTAGE */ 
		{0x42, 0x00, 0x03}, /* ADC_REF_LO_VOLTAGE */ 
		{0x43, 0x00, 0x03}, /* ADC_REF_HI_VOLTAGE */ 
		{0x7F, 0x00, 0x00}, /* RD_EN_VLN_SH_TIMING */ 
	
		{0x1E, 0xc0, 0x06}, /*Set parallel mode *//*MT9M002-REV2 */
		{0x9F, 0x30, 0x70}, /*Parallel data and clock out */ 
		{0x9E, 0x11, 0x1E}, /*FV_LV timing adjustment */ 	
		
		{0x0B, 0x00, 0x01},  /*restart */ 	
	};

	static const UINT8 regdata2[][3] = {
		{0x0B, 0x00, 0x00}, /*restart */		
	};

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	/*VD,HD input*/ /*default in, do nothing*/
	/*MCLK*/ /*tggpio7*/
	pfrontReg0->mclktosen2en = 1;
	pfrontReg0->tggpioosel &= ~0x0800;
	pfrontReg0->tggpiooe  |= 0x0800;

	sensorTgPllClkExtSet(16000000);
	halFrontClkSet(TGPLLEN,TGPLLCLKSET, MCLKDIV, PCLKDIV); /* Set master clock & pixel clock */
	halFrontClkCfg(MCLKMODE, PCLKMODE);

	/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);

	/*****TODO*****front reset after these???*/
	tmrUsWait(2000);
	cmdportid = sensorTgCfg.cmdportid_xtg;

	sensorSerialAsyncWrite(cmdportid, 1, regdata[0]);  /* Sensor Soft Reset */
	sensorSerialFlush(cmdportid);
	tmrUsWait(10000);  /* delay 100 ms for reset*/
	sensorSerialBulkWrite(cmdportid,(sizeof(regdata)/sizeof(regdata[0])) - 1, regdata[1],0); /*sensor initial cmd*/
	tmrUsWait(5000);  /* delay 100 ms for reset*/
	sensorSerialAsyncWrite(cmdportid, (sizeof(regdata2)/sizeof(regdata2[0])) , regdata2[0]);   /* Sensor restart */

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	printf(" MI1600 DoFunction Init \n");

}

static void
sensorDoFuncStop()
{
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {
		{ 0x01, 0x00, 0x00,}, /* mode_select */
	};

	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, 1, regdata[0]);
	sensorSerialFlush(cmdportid);
}


static void
sensorDoFuncPreviewVGABegin()
{
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {
		{0x11, 0x4B, 0x07},/* PLL_CONTROL_1    PCLK(25MHz) = MCLK * High_8Bit /(Low_6bit + 1)/6*/
	
		{0x01, 0x00, 0x26},/*Row Start = 38  */		 
		{0x02, 0x00, 0x10},/*Column Start = 16 */ 	 
		{0x03, 0x04, 0x4f},/*Row Size = 1103	*/		
		{0x04, 0x05, 0xaf},/*Column Size = 1455	 */  
		{0x05, 0x01, 0x61},/*Horz. Blank = 353 */    
		{0x06, 0x00, 0x17},/*//Vert. Blank = 759*/		

		{0x22, 0x00, 0x01},/*Row Mode = 0x1	   skip2x */		 
		{0x23, 0x00, 0x01},/*Column Mode = 0x1   skip2x*/    
		{0x35, 0x00, 0x2d},	 /*gain*/

	};

	profLogAdd(0, "VGABegin");

	sensor_event_wait(0x04);
	sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialBulkWrite(cmdportid, 
		sizeof(regdata)/sizeof(regdata[0]), regdata[0], 0);


}

static void
sensorDoFuncPreviewXGABegin()
{
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {/*1280*960*/
		{0x11, 0x58, 0x03},/* PLL_CONTROL_1 , 58666666Hz*/
		/*{0x11, 0x2d, 0x01},/* PLL_CONTROL_1 , 60MHz*/
		
		{0x01, 0x00, 0x76},/*Row Start = 32   */   
		{0x02, 0x00, 0x4c},/*/Column Start = 16   */	
		{0x03, 0x03, 0xC4},/*/Row Size = 767*/
		{0x04, 0x05, 0x04},/*/Column Size = 1295  */ 
		{0x05, 0x01, 0x62},/*/Horz. Blank = 354*/
		{0x06, 0x00, 0x09},/*/0xf7,//Vert. Blank = 8   */ 
		
		{0x22, 0x00, 0x00}, 	/*/Row Mode = 0x1		skip2x	*/		  
		{0x23, 0x00, 0x00}, 	/*/Column Mode = 0x1   skip2x	*/	 

		/*{0x35, 0x08, 0x7c},    /*gain*/
		/*{0x09, 0x02, 0x26}, /*exposure line*/

	};

	printf(" MI1600 XGA preview Begin \n");
	profLogAdd(0, "XGABegin");

	sensor_event_wait(0x04);
	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);

	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialBulkWrite(cmdportid, 
		sizeof(regdata)/sizeof(regdata[0]), regdata[0], 0); 

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
	/*profLogAdd(0, "sensorDoFuncSnapExpBegin");*/

}

static void
sensorDoFuncSnapEarly1()
{
	UINT32 cmdportid;
	static const UINT8 regdata[][3] = {
	
		{0x11, 0x4B, 0x03},/* PLL_CONTROL_1 */
		
		{0x01, 0x00, 0x26},/*/Row Start = 38   */		
		{0x02, 0x00, 0x10},/*/Column Start = 16  */ 	
		{0x03, 0x04, 0x4f},/*/Row Size = 1103		 */
		{0x04, 0x05, 0xaf},/*/Column Size = 1455	  */
		{0x05, 0x01, 0x61},/*/Horz. Blank = 353 	*/
		{0x06, 0x00, 0x08},/*/0xf7,//Vert. Blank = 759	 */    
		
		{0x22, 0x00, 0x00}, 		/*/Row Mode = 0x0  */		   
		{0x23, 0x00, 0x00}, 		/*/Column Mode = 0x0  */	   

	};
/*	HLPrintf(" MI1600 XGA preview Begin \n");*/

	profLogAdd(0, "Early1"); 

	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid,sizeof(regdata)/sizeof(regdata[0]), regdata[0]); 
}

static void
sensorDoFuncSnapEarly0()
{
}

static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{
	profLogAdd(0, "FrexSet");
}


static void
sensorDoFuncSnapBshutterBegin()
{
	profLogAdd(0, "BshutterBegin");
	/*front_snap_bshutter_begin_tg_set*/
 	 /* sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);	*/
}
static void
sensorDoFuncSnapBshutterFollow()
{
	profLogAdd(0, "BshutterFollow");
	/*front_snap_bshutter_follow_tg_set*/
}
static void
sensorDoFuncSnapBshutterEnd()
{
	profLogAdd(0, "BshutterEnd");
	/*front_snap_bshutter_end_tg_set*/
}


static void
sensorDoFuncSnapShortExp()
{
	/*front_snap_short_exp_frame_set()*//*slow shutter, 1 longer frame*/
	/*front_snap_short_exp_tg_set*/
	/*do 1 longer frame exp, historical term "bshutter", if needed*/
	/*profLogAdd(0, "sensorDoFuncSnapShortExp");*/

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
	/*profLogAdd(0, "sensorDoFuncSnapExpEnd");*/

}

static void
sensorDoFuncSnapReadoutField0()
{
/*	HLPrintf("sensorDoFuncSnapReadoutField0");*/
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
	/*profLogAdd(0, "sensorDoFuncSnapReadoutField0");*/

}

static void
sensorDoFuncSnapFieldEnd()
{
/*	HLPrintf("sensorDoFuncSnapFieldEnd");*/
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapEnd()*/
	/*throw user msg??*/
	/*profLogAdd(0, "sensorDoFuncSnapFieldEnd");*/
}

/*at the readout end of the last field*/

static void
sensorDoFuncSnapReadoutEnd()
{
	/*panasonic TG TRG reset*/
	/*direct m-sh open*/
	/*direct vsubctl off*/
	/*immediate inform readout all end*/
	/*profLogAdd(0, "ReadoutEnd");*/

	printf("TEST SIZE (%d x %d)",READ16(0xb00090cc),READ16(0xb00090ce));
	profLogPrintf(1,"TEST SIZE (%d x %d)",READ16(0xb00090cc),READ16(0xb00090ce));

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
	profLogAdd(0,"\033[32msensorCfgInit\033[0m");
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
}

static void
sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	profLogAdd(0,"\033[32msensorCfgStop\033[0m");
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
#if 1
	/*****TODO******ITG fine exp set, value and timing???*/
	/*exp setting via serial port*/

	/*printf("EXP[%d] = %d  %d" , pdata[0] ,pdata[1],pdata[2]);*/
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
#endif
}

static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
)
{

}

#if SP5K_SENSOR_MI1600
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


