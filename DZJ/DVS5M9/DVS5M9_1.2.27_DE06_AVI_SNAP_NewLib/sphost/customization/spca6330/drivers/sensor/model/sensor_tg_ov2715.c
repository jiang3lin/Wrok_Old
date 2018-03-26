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

#define READ16(_reg_)           (*((volatile UINT16 *)(_reg_)))

#define MODEL_VERSION "$unp_tg_ov2715-1.00.00"

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
#include "../flow/hal_front.h"
#if SPCA5110 /****TODO*5110*tggpio temp workaround*/
#include "../flow/sysregs.h"
#else
#include "hal/hal_gpio.h"
#endif
#include "../flow/sensor_model.h"
#include "../flow/sensor_model_snap.h"
#include "api/sp5k_global_api.h"
#include "middleware/proflog.h"

#define BLC_TARGET 0x10  /*OV BLC Target*/


/*#define I2C_BUF SENSOR_SIF1DMA_BUF */

#define I2C_BUF

#define SNAP_PREVIEW_DELAY_FRAME_NUM  2


/* Sensor image size definition here */
#define NO             0
#define NOUSE          0
#define DISABLE        0xFFFF

/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_QVGA_HSIZE  320		/* preview horiz. size*/
#define PREVIEW_QVGA_VSIZE  240		/* preview vertical size*/

#define PREVIEW_VGA_HSIZE  640		/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  480		/* preview vertical size*/

#define PREVIEW_HD_HSIZE   1280		/* preview horiz. size*/
#define PREVIEW_HD_VSIZE   720		/* preview vertical size*/

#define PREVIEW_FHD_HSIZE  1920		/* preview horiz. size*/
#define PREVIEW_FHD_VSIZE  1080		/* preview vertical size*/

#define PREVIEW_SVGA_HSIZE  810		/* preview horiz. size*/
#define PREVIEW_SVGA_VSIZE  540		/* preview vertical size*/

#define CAPTURE_HSIZE  1920		/* capture horiz. size*/
#define CAPTURE_VSIZE  1080		/* capture vertical size*/

/* Preview default definition here */
/* VGA mode  */
#define PREVIEW_VGA_LINETOTAL   1344	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   300		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  496		/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     652     /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     5		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         DISABLE 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         DISABLE	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL         DISABLE /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         DISABLE /*7*/	/* preview Vd reshap rise*/

#define	PREVIEW_VGA_HDNUM_FRAME_START  20	/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_FRAME_END	  486	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT	  486	/*the command port interrupt line number of the preview new frame*/


/* QVGA mode  */
#define PREVIEW_QVGA_LINETOTAL   1344	/* preview Hd length*/
#define PREVIEW_QVGA_LINEBLANK   300	/* preview Hd sync length*/
#define PREVIEW_QVGA_FRAMETOTAL  496	/* preview Vd length*/
#define PREVIEW_QVGA_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_QVGA_HOFFSET     342    /* preview Hd valid pixel data offset position*/
#define PREVIEW_QVGA_VOFFSET     5		/* preview Vd valid line data offset position*/
/* Preview resh_QVGA_pe sync signal definition here */
#define PREVIEW_QVGA_HRESHAPE_FALL         DISABLE 	/* preview Hd reshap fall*/
#define PREVIEW_QVGA_HRESHAPE_RISE         DISABLE	/* preview Hd reshap rise*/
#define PREVIEW_QVGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_QVGA_VRESHAPE_FALL         DISABLE /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_QVGA_VRESHAPE_RISE         DISABLE /*7*/	/* preview Vd reshap rise*/

#define	PREVIEW_QVGA_HDNUM_FRAME_START  20		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_FRAME_END	486		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_CMDPORT	    486		/*the command port interrupt line number of the preview new frame*/


/* HD mode 720P */
#define PREVIEW_HD_LINETOTAL   2024 	/* preview Hd length*/
#define PREVIEW_HD_LINEBLANK   300		/* preview Hd sync length*/
#define PREVIEW_HD_FRAMETOTAL  768		/* preview Vd length*/
#define PREVIEW_HD_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_HD_HOFFSET     460      /* preview Hd valid pixel data offset position*/
#define PREVIEW_HD_VOFFSET     14		/* preview Vd valid line data offset position*/
/* Preview resh_HD_pe sync signal definition here */
#define PREVIEW_HD_HRESHAPE_FALL         DISABLE 	/* preview Hd reshap fall*/
#define PREVIEW_HD_HRESHAPE_RISE         DISABLE	/* preview Hd reshap rise*/
#define PREVIEW_HD_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_HD_VRESHAPE_FALL         DISABLE /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_HD_VRESHAPE_RISE         DISABLE /*7*/	/* preview Vd reshap rise*/

#define	PREVIEW_HD_HDNUM_FRAME_START  20		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_HD_HDNUM_FRAME_END	738		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_HD_HDNUM_CMDPORT	738		/*the command port interrupt line number of the preview new frame*/

/*Full HD mode 1080P */
#define PREVIEW_FHD_LINETOTAL   2420	/* preview Hd length*/
#define PREVIEW_FHD_LINEBLANK   352		/* preview Hd sync length*/
#define PREVIEW_FHD_FRAMETOTAL  1140		/* preview Vd length*/
#define PREVIEW_FHD_FRAMEBLANK  4		/* preview Vd sync length*/
#define PREVIEW_FHD_HOFFSET     448         /* preview Hd valid pixel data offset position*/
#define PREVIEW_FHD_VOFFSET     14		/* preview Vd valid line data offset position*/
/* Preview resh_QVGA_pe sync signal definition here */
#define PREVIEW_FHD_HRESHAPE_FALL         DISABLE 	/* preview Hd reshap fall*/
#define PREVIEW_FHD_HRESHAPE_RISE         DISABLE	/* preview Hd reshap rise*/
#define PREVIEW_FHD_VRESHAPE_COUNTER_SEL  NO	    /* preview Vd reshap control*/
#define PREVIEW_FHD_VRESHAPE_FALL         DISABLE    /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_FHD_VRESHAPE_RISE         DISABLE   /*7*/	/* preview Vd reshap rise*/

#define	PREVIEW_FHD_HDNUM_FRAME_START  20		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_FHD_HDNUM_FRAME_END	1095		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_FHD_HDNUM_CMDPORT	1095		/*the command port interrupt line number of the preview new frame*/

/* SVGA mode  */
#define PREVIEW_SVGA_LINETOTAL   	2420	/* preview Hd length*/
#define PREVIEW_SVGA_LINEBLANK   	NOUSE	/* preview Hd sync length*/
#define PREVIEW_SVGA_FRAMETOTAL  	1190	/* preview Vd length*/
#define PREVIEW_SVGA_FRAMEBLANK  	NOUSE	/* preview Vd sync length*/
#define PREVIEW_SVGA_HOFFSET     	1544     	/* preview Hd valid pixel data offset position*/
#define PREVIEW_SVGA_VOFFSET     	5		/* preview Vd valid line data offset position*/
/* Preview resh_SVGA_pe sync signal definition here */
#define PREVIEW_SVGA_HRESHAPE_FALL         DISABLE 	/* preview Hd reshap fall*/
#define PREVIEW_SVGA_HRESHAPE_RISE         DISABLE	/* preview Hd reshap rise*/
#define PREVIEW_SVGA_VRESHAPE_COUNTER_SEL  NO	/* preview Vd reshap control*/
#define PREVIEW_SVGA_VRESHAPE_FALL         DISABLE 	/* preview Vd reshap fall*/
#define PREVIEW_SVGA_VRESHAPE_RISE         DISABLE 	/* preview Vd reshap rise*/

#define	PREVIEW_SVGA_HDNUM_FRAME_START  	20	/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_SVGA_HDNUM_FRAME_END		900	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_SVGA_HDNUM_CMDPORT	  		900	/*the command port interrupt line number of the preview new frame*/

/* Capture default definition here */
#define CAPTURE_LINETOTAL   2420	/* capture Hd length*/
#define CAPTURE_LINEBLANK   352		/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  1140		/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  4		/* capture Vd sync length*/
#define CAPTURE_HOFFSET     448         /* capture Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     14   	/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         DISABLE	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         DISABLE	/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  NO	/* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL         DISABLE	/* capture Vd reshap fall*/
#define CAPTURE_VRESHAPE_RISE         DISABLE 	/* capture Vd reshap rise*/

#define	CAPTURE_HDNUM_FRAME_START	20		/*the front interrupt line number of the snap new frame */
#define	CAPTURE_HDNUM_FRAME_END	1095    	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	1095    	/*the command port interrupt line number of the snap new frame*/

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
#define PIXELPHASE	0	     /*pixel clk phase*/
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
#define	PREVIEW_VGA_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	PREVIEW_QVGA_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	PREVIEW_HD_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	PREVIEW_FHD_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define PREVIEW_SVGA_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define	SNAP_CFAPATTERN		BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/

#define	SNAP_EARLY_NUM		0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/

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
#define	TG_SPI_I2C_MODE  0x6C      	       /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF /*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#if SPCA5210
#define	TG_SPEED         2
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  8		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */


#define BIT0 (1)
#define BIT10 (1<<10)

#define SENSOR_RESET   BIT0
#define SENSOR_PWDN    BIT10

extern UINT32 sensor_event_wait(UINT32 event);

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreviewSVGABegin();
static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreviewQVGABegin();
static void sensorDoFuncPreviewHDBegin();
static void sensorDoFuncPreviewFHDBegin();
static void sensorDoFuncPreviewFHD43Begin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
/*static void sensorDoFuncSnapEarly0();*/
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
/*static void sensorDoFuncSnapDummy0();*/

static void sensorDoFuncSnapReadoutField0();
/*
static void sensorDoFuncSnapReadoutField1();
static void sensorDoFuncSnapReadoutField2();
*/
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();

/*sensor cfg*/

static const sensorTgPreviewCfg_t sensorTgPreviewSVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewSVGABegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_SVGA_HSIZE,
		.ySize = PREVIEW_SVGA_VSIZE,
	},
	.hsize = PREVIEW_SVGA_HSIZE,
	.vsize = PREVIEW_SVGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_SVGA_CFAPATTERN,
	.hratio=1620,
	.vratio=1080,	
	.fps_mhz = 29940000,
	.sensor_pclk = 80000000,
	.sensor_htotal = 2592,
	.pix_100khz =800,
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGABegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},

	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
	.hsize = PREVIEW_VGA_HSIZE,
	.vsize = PREVIEW_VGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
	.hratio=4000,
	.vratio=3000,	
	.fps_mhz = 120000000, /*125630000,*/
	.sensor_pclk = 80000000,
	.sensor_htotal = 1344,
	.pix_100khz =800,

};
static const sensorTgPreviewCfg_t sensorTgPreviewQVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewQVGABegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_QVGA_HSIZE,
	.vsize = PREVIEW_QVGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_QVGA_CFAPATTERN,
	.hratio=4000,
	.vratio=3000,	
	.pix_100khz =800,

};

static const sensorTgPreviewCfg_t sensorTgPreviewHDCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewHDBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},

	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_HD_HSIZE,
		.ySize = PREVIEW_HD_VSIZE,
	},

	.hsize = PREVIEW_HD_HSIZE,
	.vsize = PREVIEW_HD_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_HD_CFAPATTERN,
	.hratio=3200,
	.vratio=1800,	
	.fps_mhz = 57380000, /*62814800,*/
	.sensor_pclk = 80000000,
	.sensor_htotal = 1792,
	.pix_100khz =800,

};

static const sensorTgPreviewCfg_t sensorTgPreviewFHDCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewFHDBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
		.crop = &( const sensorTgCrop_t ) {
			.xOffset = 0,
			.yOffset = 0,
			.xSize = PREVIEW_FHD_HSIZE,
			.ySize = PREVIEW_FHD_VSIZE,
		},

	.hsize = PREVIEW_FHD_HSIZE,
	.vsize = PREVIEW_FHD_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_FHD_CFAPATTERN,
	.hratio=3200,
	.vratio=1800,	
	.fps_mhz = 29000000, /*30356500,*/
	.sensor_pclk = 80000000,
	.sensor_htotal = 2420,
	.pix_100khz =800,

};

static const sensorTgPreviewCfg_t sensorTgPreviewFHD43Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewFHD43Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
		.crop = &( const sensorTgCrop_t ) {
			.xOffset = 0,
			.yOffset = 0,
			.xSize = (PREVIEW_FHD_HSIZE-480),
			.ySize = PREVIEW_FHD_VSIZE,
		},

	.hsize = (PREVIEW_FHD_HSIZE-480),
	.vsize = PREVIEW_FHD_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_FHD_CFAPATTERN,
	.hratio=4000,
	.vratio=3000,	
	.fps_mhz = 29000000, /*30356500,*/
	.sensor_pclk = 80000000,
	.sensor_htotal = 2420,
	.pix_100khz =800,

};


static sensorDoFunc_t sensorDoFuncSnapDummyTbl[] = {
	NULL/*sensorDoFuncSnapDummy0*/,
};

static sensorDoFunc_t sensorDoFuncSnapEarlyTbl[] = {
	NULL/*sensorDoFuncSnapEarly0*/,
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
	.n_snap_early = SNAP_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
	.hratio=3200,
	.vratio=1800,
	.pix_100khz =800,
};


static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	/*&sensorTgPreviewVGACfg,&sensorTgPreviewQVGACfg, &sensorTgPreviewHDCfg,&sensorTgPreviewFHDCfg*/
	&sensorTgPreviewFHDCfg,&sensorTgPreviewHDCfg,&sensorTgPreviewHDCfg,&sensorTgPreviewVGACfg,&sensorTgPreviewVGACfg, &sensorTgPreviewFHD43Cfg,&sensorTgPreviewSVGACfg,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewSVGACfg = {
	.linetotal       = PREVIEW_SVGA_LINETOTAL,
	.lineblank       = PREVIEW_SVGA_LINEBLANK,
	.frametotal      = PREVIEW_SVGA_FRAMETOTAL,
	.frameblank      = PREVIEW_SVGA_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_SVGA_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_SVGA_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_SVGA_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_SVGA_VRESHAPE_RISE,
	.hoffset         = PREVIEW_SVGA_HOFFSET,
	.voffset         = PREVIEW_SVGA_VOFFSET,
	.hsize           = PREVIEW_SVGA_HSIZE,
	.vsize           = PREVIEW_SVGA_VSIZE+1,
	.lastlinepix     = PREVIEW_SVGA_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_SVGA_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_SVGA_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_SVGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_SVGA_HDNUM_CMDPORT,
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
	.hdnum_frame_start = PREVIEW_VGA_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_VGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_VGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQVGACfg = {
	.linetotal       = PREVIEW_QVGA_LINETOTAL,
	.lineblank       = PREVIEW_QVGA_LINEBLANK,
	.frametotal      = PREVIEW_QVGA_FRAMETOTAL,
	.frameblank      = PREVIEW_QVGA_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_QVGA_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_QVGA_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_QVGA_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_QVGA_VRESHAPE_RISE,
	.hoffset         = PREVIEW_QVGA_HOFFSET,
	.voffset         = PREVIEW_QVGA_VOFFSET,
	.hsize           = PREVIEW_QVGA_HSIZE,
	.vsize           = PREVIEW_QVGA_VSIZE,
	.lastlinepix     = PREVIEW_QVGA_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_QVGA_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_QVGA_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_QVGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_QVGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewHDCfg = {
	.linetotal       = PREVIEW_HD_LINETOTAL,
	.lineblank       = PREVIEW_HD_LINEBLANK,
	.frametotal      = PREVIEW_HD_FRAMETOTAL,
	.frameblank      = PREVIEW_HD_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_HD_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_HD_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_HD_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_HD_VRESHAPE_RISE,
	.hoffset         = PREVIEW_HD_HOFFSET,
	.voffset         = PREVIEW_HD_VOFFSET,
	.hsize           = PREVIEW_HD_HSIZE,
	.vsize           = PREVIEW_HD_VSIZE,
	.lastlinepix     = PREVIEW_HD_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_HD_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_HD_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_HD_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_HD_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFHDCfg = {
	.linetotal       = PREVIEW_FHD_LINETOTAL,
	.lineblank       = PREVIEW_FHD_LINEBLANK,
	.frametotal      = PREVIEW_FHD_FRAMETOTAL,
	.frameblank      = PREVIEW_FHD_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_FHD_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_FHD_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_FHD_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_FHD_VRESHAPE_RISE,
	.hoffset         = PREVIEW_FHD_HOFFSET,
	.voffset         = PREVIEW_FHD_VOFFSET,
	.hsize           = PREVIEW_FHD_HSIZE,
	.vsize           = PREVIEW_FHD_VSIZE,
	.lastlinepix     = PREVIEW_FHD_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_FHD_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_FHD_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_FHD_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_FHD_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFHD43Cfg = {
	.linetotal       = PREVIEW_FHD_LINETOTAL,
	.lineblank       = PREVIEW_FHD_LINEBLANK,
	.frametotal      = PREVIEW_FHD_FRAMETOTAL,
	.frameblank      = PREVIEW_FHD_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_FHD_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_FHD_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_FHD_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_FHD_VRESHAPE_RISE,
	.hoffset         = (PREVIEW_FHD_HOFFSET+240),
	.voffset         = PREVIEW_FHD_VOFFSET,
	.hsize           = (PREVIEW_FHD_HSIZE-480),
	.vsize           = PREVIEW_FHD_VSIZE,
	.lastlinepix     = PREVIEW_FHD_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_FHD_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_FHD_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_FHD_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_FHD_HDNUM_CMDPORT,
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
	.defSyncSigCfg  = &sensorSyncSigPreviewFHDCfg,
	.verStr = MODEL_VERSION,
};
static const UINT8 cmd_reset_code[][3]={
{0x30,0x08,0x82}
};

static const UINT8 cmd_init_code[][3]={
{0x31,0x03,0x93},
{0x30,0x08,0x82},
{0x30,0x17,0x7f},
{0x30,0x18,0xfc},
/*AE OFF mode*/
{0x35,0x03,0x07},
{0x37,0x06,0x61},
{0x37,0x12,0x0c},
{0x36,0x30,0x6d},
{0x38,0x01,0xb4},
{0x36,0x21,0x04},
{0x36,0x04,0x60},
{0x36,0x03,0xa7},
{0x36,0x31,0x26},
{0x36,0x00,0x04},
{0x36,0x20,0x37},
{0x36,0x23,0x00},
{0x37,0x02,0x9e},
{0x37,0x03,0x5c},
{0x37,0x04,0x40},
{0x37,0x0d,0x0f},
{0x37,0x13,0x9f},
{0x37,0x14,0x4c},
{0x37,0x10,0x9e},
{0x38,0x01,0xc4},
{0x36,0x05,0x05},
{0x36,0x06,0x3f},
{0x30,0x2c,0x42},
{0x30,0x2d,0x90},
{0x37,0x0b,0x40},
{0x37,0x16,0x31},
{0x37,0x07,0x52},
{0x38,0x0d,0x74},
/*LENC BPC OFF*/
{0x50,0x00,0x00},
{0x51,0x81,0x20},
{0x51,0x8f,0x00},
{0x43,0x01,0xff},
{0x43,0x03,0x00},
{0x3a,0x00,0x78},
{0x30,0x0f,0x88},
{0x30,0x11,0x28},
/*control PLL div*/
{0x30,0x12,0x00}, 
{0x3a,0x1a,0x06},
{0x3a,0x18,0x00},
{0x3a,0x19,0x7a},
{0x3a,0x13,0x54},
{0x38,0x2e,0x0f},
{0x38,0x1a,0x1a},
{0x40,0x1d,0x02},
/*HSYNC mode*/
{0x47,0x00,0x05},
{0x56,0x88,0x03},
{0x56,0x84,0x07},
{0x56,0x85,0xa0},
{0x56,0x86,0x04},
{0x56,0x87,0x43},
{0x3a,0x0f,0x40},
{0x3a,0x10,0x38},
{0x3a,0x1b,0x48},
{0x3a,0x1e,0x30},
{0x3a,0x11,0x90},
{0x3a,0x1f,0x10},

{0x30,0x10,0x00},	
};


static const UINT8 cmd_qvga_code[][3]={
	
{0x31,0x03,0x93},
{0x30,0x08,0x82},
{0x30,0x17,0x7f},
{0x30,0x18,0xfc},
/*AE OFF mode*/
{0x35,0x03,0x07},
{0x37,0x06,0x61},
{0x37,0x12,0x0c},
{0x36,0x30,0x6d},
{0x38,0x01,0xb4},
{0x36,0x21,0x04},
{0x36,0x04,0x60},
{0x36,0x03,0xa7},
{0x36,0x31,0x26},
{0x36,0x00,0x04},
{0x36,0x20,0x37},
{0x36,0x23,0x00},
{0x37,0x02,0x9e},
{0x37,0x03,0x5c},
{0x37,0x04,0x40},
{0x37,0x0d,0x0f},
{0x37,0x13,0x9f},
{0x37,0x14,0x4c},
{0x37,0x10,0x9e},
{0x38,0x01,0xc4},
{0x36,0x05,0x05},
{0x36,0x06,0x3f},
{0x30,0x2d,0x90},
{0x37,0x0b,0x40},
{0x37,0x16,0x31},
{0x37,0x07,0x52},
{0x38,0x0d,0x74},
{0x51,0x81,0x20},
{0x51,0x8f,0x00},
{0x43,0x01,0xff},
{0x43,0x03,0x00},
{0x3a,0x00,0x78},
{0x30,0x0f,0x88},
{0x30,0x11,0x28},
/*control PLL div*/
{0x30,0x12,0x00}, 
{0x3a,0x1a,0x06},
{0x3a,0x18,0x00},
{0x3a,0x19,0x7a},
{0x3a,0x13,0x54},
{0x38,0x2e,0x0f},
{0x38,0x1a,0x1a},
{0x40,0x1d,0x02},
/*HSYNC mode*/
{0x47,0x00,0x05},
{0x38,0x03,0x06},
{0x38,0x1c,0x10},
{0x38,0x1d,0x46},
{0x38,0x1e,0x03},
{0x38,0x1f,0xc8},
{0x38,0x20,0x0a},
{0x38,0x21,0x29},
{0x38,0x04,0x02},
{0x38,0x05,0x80},
{0x38,0x06,0x01},
{0x38,0x07,0xe0},
{0x38,0x08,0x01},
{0x38,0x09,0x40},
{0x38,0x0a,0x00},
{0x38,0x0b,0xf0},
{0x38,0x10,0x10},
{0x38,0x11,0x00},
{0x38,0x18,0x81},
{0x36,0x21,0xc4},
{0x38,0x0e,0x01},
{0x38,0x0f,0xf0},
{0x38,0x0c,0x05},
{0x38,0x0d,0x40},
/*LENC BPC OFF*/
{0x50,0x00,0x00},
/*AWB OFF*/
{0x50,0x01,0x4e},
{0x50,0x02,0xe6},
{0x59,0x01,0x05},
{0x3a,0x08,0x12},
{0x3a,0x09,0xe6},
{0x3a,0x0a,0x0f},
{0x3a,0x0b,0xc0},
{0x3a,0x0e,0x00},
{0x3a,0x0d,0x01},
{0x40,0x1c,0x04},
{0x3a,0x0f,0x40},
{0x3a,0x10,0x38},
{0x3a,0x1b,0x48},
{0x3a,0x1e,0x30},
{0x3a,0x11,0x90},
{0x3a,0x1f,0x10},

{0x30,0x10,0x00},	
};

static const UINT8 cmd_vga_code[][3]={
{0x31,0x03,0x93},
{0x30,0x08,0x82},
{0x30,0x17,0x7f},
{0x30,0x18,0xfc},
/*AE OFF mode*/
{0x35,0x03,0x07},
{0x37,0x06,0x61},
{0x37,0x12,0x0c},
{0x36,0x30,0x6d},
{0x38,0x01,0xb4},
{0x36,0x21,0x04},
{0x36,0x04,0x60},
{0x36,0x03,0xa7},
{0x36,0x31,0x26},
{0x36,0x00,0x04},
{0x36,0x20,0x37},
{0x36,0x23,0x00},
{0x37,0x02,0x9e},
{0x37,0x03,0x5c},
{0x37,0x04,0x40},
{0x37,0x0d,0x0f},
{0x37,0x13,0x9f},
{0x37,0x14,0x4c},
{0x37,0x10,0x9e},
{0x38,0x01,0xc4},
{0x36,0x05,0x05},
{0x36,0x06,0x3f},
{0x30,0x2d,0x90},
{0x37,0x0b,0x40},
{0x37,0x16,0x31},
{0x37,0x07,0x52},
{0x38,0x0d,0x74},
/*BLC Target to 0 */
{0x40,0x07,BLC_TARGET},
{0x51,0x81,0x20},
{0x51,0x8f,0x00},
{0x43,0x01,0xff},
{0x43,0x03,0x00},
{0x3a,0x00,0x78},
{0x30,0x0f,0x88},
{0x30,0x11,0x28},
/*control PLL div*/
{0x30,0x12,0x00}, 
{0x3a,0x1a,0x06},
{0x3a,0x18,0x00},
{0x3a,0x19,0x7a},
{0x3a,0x13,0x54},
{0x38,0x2e,0x0f},
{0x38,0x1a,0x1a},
{0x40,0x1d,0x02},
/*HSYNC mode*/
{0x47,0x00,0x05},
{0x38,0x03,0x06},
{0x38,0x1c,0x10},
{0x38,0x1d,0x42},
{0x38,0x1e,0x03},
{0x38,0x1f,0xc8},
{0x38,0x20,0x0a},
{0x38,0x21,0x29},
{0x38,0x04,0x02},
{0x38,0x05,0x80},
{0x38,0x06,0x01},
{0x38,0x07,0xe0},
{0x38,0x08,0x02},
{0x38,0x09,0x80},
{0x38,0x0a,0x01},
{0x38,0x0b,0xe0},
{0x38,0x10,0x10},
{0x38,0x11,0x02},
{0x38,0x18,0x81},
{0x36,0x21,0xc4},
{0x38,0x0e,0x01},
{0x38,0x0f,0xf0},
{0x38,0x0c,0x05},
{0x38,0x0d,0x40},
/*LENC BPC OFF*/
{0x50,0x00,0x00},
/*AWB OFF*/
{0x50,0x01,0x4e},
{0x3a,0x08,0x24},
{0x3a,0x09,0xe6},
{0x3a,0x0a,0x1e},
{0x3a,0x0b,0xc0},
{0x3a,0x0e,0x00},
{0x3a,0x0d,0x01},
{0x40,0x1c,0x04},
{0x3a,0x0f,0x40},
{0x3a,0x10,0x38},
{0x3a,0x1b,0x48},
{0x3a,0x1e,0x30},
{0x3a,0x11,0x90},
{0x3a,0x1f,0x10},

{0x30,0x10,0x00},	
};


static const UINT8 cmd_hd_code[][3]={
{0x31,0x03,0x93},
{0x30,0x08,0x82},
{0x30,0x17,0x7f},
{0x30,0x18,0xfc},
/*AE OFF mode*/
{0x35,0x03,0x07},
{0x37,0x06,0x61},
{0x37,0x12,0x0c},
{0x36,0x30,0x6d},
{0x38,0x01,0xb4},
{0x36,0x21,0x04},
{0x36,0x04,0x60},
{0x36,0x03,0xa7},
{0x36,0x31,0x26},
{0x36,0x00,0x04},
{0x36,0x20,0x37},
{0x36,0x23,0x00},
{0x37,0x02,0x9e},
{0x37,0x03,0x5c},
{0x37,0x04,0x40},
{0x37,0x0d,0x0f},
{0x37,0x13,0x9f},
{0x37,0x14,0x4c},
{0x37,0x10,0x9e},
{0x38,0x01,0xc4},
{0x36,0x05,0x05},
{0x36,0x06,0x3f},
{0x30,0x2d,0x90},
{0x37,0x0b,0x40},
{0x37,0x16,0x31},
{0x37,0x07,0x52},
{0x38,0x0d,0x74},
{0x51,0x81,0x20},
{0x51,0x8f,0x00},
{0x43,0x01,0xff},
{0x43,0x03,0x00},
{0x3a,0x00,0x78},
{0x30,0x0f,0x88},
{0x30,0x11,0x28},
/*control PLL div*/
{0x30,0x12,0x00}, 
{0x3a,0x1a,0x06},
{0x3a,0x18,0x00},
{0x3a,0x19,0x7a},
{0x3a,0x13,0x54},
{0x38,0x2e,0x0f},
{0x38,0x1a,0x1a},
{0x40,0x1d,0x02},
/*BLC Target to 0 */
{0x40,0x07,BLC_TARGET},
/*HSYNC mode*/
{0x47,0x00,0x05},
{0x38,0x1c,0x10},
{0x38,0x1d,0xb8},
{0x38,0x1e,0x02},
{0x38,0x1f,0xdc},
{0x38,0x20,0x0a},
{0x38,0x21,0x29},
{0x38,0x04,0x05},
{0x38,0x05,0x00},
{0x38,0x06,0x02},
{0x38,0x07,0xd0},
{0x38,0x08,0x05},
{0x38,0x09,0x00},
{0x38,0x0a,0x02},
{0x38,0x0b,0xd0},
{0x38,0x0e,0x02},
{0x38,0x0f,0xe8},
{0x38,0x0c,0x07},
{0x38,0x0d,0x00},
{0x38,0x10,0x10},
/*LENC BPC OFF*/
{0x50,0x00,0x00},
/*AWB OFF*/
{0x50,0x01,0x4e},
{0x56,0x88,0x03},
{0x56,0x84,0x05},
{0x56,0x85,0x00},
{0x56,0x86,0x02},
{0x56,0x87,0xd0},
{0x3a,0x08,0x1b},
{0x3a,0x09,0xe6},
{0x3a,0x0a,0x17},
{0x3a,0x0b,0x40},
{0x3a,0x0e,0x01},
{0x3a,0x0d,0x02},
{0x3a,0x0f,0x40},
{0x3a,0x10,0x38},
{0x3a,0x1b,0x48},
{0x3a,0x1e,0x30},
{0x3a,0x11,0x90},
{0x3a,0x1f,0x10},
{0x3a,0x1f,0x10},
{0x30,0x10,0x00},
};


static const UINT8 cmd_svga_code[][3]={
{0x31,0x03,0x93},
{0x30,0x08,0x82},
{0x30,0x17,0x7f},
{0x30,0x18,0xfc},
/*AE OFF mode*/
{0x35,0x03,0x07},
{0x37,0x06,0x61},
{0x37,0x12,0x0c},
{0x36,0x30,0x6d},
{0x38,0x01,0xb4},
{0x36,0x21,0x04},
{0x36,0x04,0x60},
{0x36,0x03,0xa7},
{0x36,0x31,0x26},
{0x36,0x00,0x04},
{0x36,0x20,0x37},
{0x36,0x23,0x00},
{0x37,0x02,0x9e},
{0x37,0x03,0x5c},
{0x37,0x04,0x40},
{0x37,0x0d,0x0f},
{0x37,0x13,0x9f},
{0x37,0x14,0x4c},
{0x37,0x10,0x9e},
/*{0x38,0x01,0xc4},*/
{0x36,0x05,0x05},
{0x36,0x06,0x3f},
{0x30,0x2d,0x90},
{0x37,0x0b,0x40},
{0x37,0x16,0x31},
{0x37,0x07,0x52},
{0x51,0x81,0x20},
{0x51,0x8f,0x00},
{0x43,0x01,0xff},
{0x43,0x03,0x00},
{0x3a,0x00,0x78},
{0x30,0x0f,0x88},
{0x30,0x11,0x28},
/*control PLL div*/
{0x30,0x12,0x00}, 
{0x3a,0x1a,0x06},
{0x3a,0x18,0x00},
{0x3a,0x19,0x7a},
{0x3a,0x13,0x54},
{0x38,0x2e,0x0f},
{0x38,0x1a,0x1a},
{0x40,0x1c,0x04},
{0x40,0x1d,0x02},
/*BLC Target to 0 */
{0x40,0x07,BLC_TARGET},
/*HSYNC mode*/
{0x47,0x00,0x05},
{0x38,0x03,0x06},
{0x38,0x04,0x03},/*816*/
{0x38,0x05,0x30},
{0x38,0x06,0x02},/*540*/
{0x38,0x07,0x1c},
{0x38,0x08,0x03},/*816*/
{0x38,0x09,0x30},
{0x38,0x0a,0x02},/*540*/
{0x38,0x0b,0x1c},
{0x38,0x0c,0x09},
{0x38,0x0d,0x74},
{0x38,0x0e,0x04},
{0x38,0x0f,0xa6},
{0x36,0x21,0xc4},
{0x36,0x22,0x08},
{0x37,0x0d,0x4f},
{0x38,0x00,0x03},
{0x38,0x01,0x06},
{0x38,0x10,0x08},
{0x38,0x11,0x02},
{0x38,0x18,0x81},
{0x38,0x1C,0x21},
{0x38,0x1D,0x50},
{0x38,0x1E,0x01},
{0x38,0x1F,0x20},
{0x38,0x20,0x00},
{0x38,0x21,0x00},
{0x38,0x01,0x50},/*align center*/
/*LENC BPC OFF*/
{0x50,0x00,0x00},
/*AWB OFF*/
{0x50,0x01,0x4e},
{0x56,0x88,0x03},
{0x56,0x84,0x03},
{0x56,0x85,0xC4},
{0x56,0x86,0x02},
{0x56,0x87,0x20},
{0x3a,0x0f,0x40},
{0x3a,0x10,0x38},
{0x3a,0x1b,0x48},
{0x3a,0x1e,0x30},
{0x3a,0x11,0x90},
{0x3a,0x1f,0x10},
{0x30,0x10,0x00},	
};

static const UINT8 cmd_fhd_code[][3]={
{0x31,0x03,0x93},
{0x30,0x08,0x82},
{0x30,0x17,0x7f},
{0x30,0x18,0xfc},
/*AE OFF mode*/
{0x35,0x03,0x07},
{0x37,0x06,0x61},
{0x37,0x12,0x0c},
{0x36,0x30,0x6d},
{0x38,0x01,0xb4},
{0x36,0x21,0x04},
{0x36,0x04,0x60},
{0x36,0x03,0xa7},
{0x36,0x31,0x26},
{0x36,0x00,0x04},
{0x36,0x20,0x37},
{0x36,0x23,0x00},
{0x37,0x02,0x9e},
{0x37,0x03,0x5c},
{0x37,0x04,0x40},
{0x37,0x0d,0x0f},
{0x37,0x13,0x9f},
{0x37,0x14,0x4c},
{0x37,0x10,0x9e},
{0x38,0x01,0xc4},
{0x36,0x05,0x05},
{0x36,0x06,0x3f},
{0x30,0x2c,0x42},
{0x30,0x2d,0x90},
{0x37,0x0b,0x40},
{0x37,0x16,0x31},
{0x37,0x07,0x52},
{0x38,0x0d,0x74},
{0x38,0x10,0x10},
/*LENC BPC OFF*/
{0x50,0x00,0x00},
/*AWB OFF*/
{0x50,0x01,0x4e},
{0x51,0x81,0x20},
{0x51,0x8f,0x00},
{0x43,0x01,0xff},
{0x43,0x03,0x00},
{0x3a,0x00,0x78},
{0x30,0x0f,0x88},
{0x30,0x11,0x28},
/*control PLL div*/
{0x30,0x12,0x00},
{0x3a,0x1a,0x06},
{0x3a,0x18,0x00},
{0x3a,0x19,0x7a},
{0x3a,0x13,0x54},
{0x38,0x2e,0x0f},
{0x38,0x1a,0x1a},
{0x40,0x1d,0x02},
/*BLC Target to 0 */
{0x40,0x07,BLC_TARGET},
/*HSYNC mode*/
{0x47,0x00,0x05},
{0x56,0x88,0x03},
{0x56,0x84,0x07},
{0x56,0x85,0xa0},
{0x56,0x86,0x04},
{0x56,0x87,0x43},
{0x3a,0x0f,0x40},
{0x3a,0x10,0x38},
{0x3a,0x1b,0x48},
{0x3a,0x1e,0x30},
{0x3a,0x11,0x90},
{0x3a,0x1f,0x10},

{0x30,0x10,0x00},	
};


static const UINT8 cmd_still_code[][3]={
{0x31,0x03,0x93},
{0x30,0x08,0x82},
{0x30,0x17,0x7f},
{0x30,0x18,0xfc},
/*AE OFF mode*/
{0x35,0x03,0x07},
{0x37,0x06,0x61},
{0x37,0x12,0x0c},
{0x36,0x30,0x6d},
{0x38,0x01,0xb4},
{0x36,0x21,0x04},
{0x36,0x04,0x60},
{0x36,0x03,0xa7},
{0x36,0x31,0x26},
{0x36,0x00,0x04},
{0x36,0x20,0x37},
{0x36,0x23,0x00},
{0x37,0x02,0x9e},
{0x37,0x03,0x5c},
{0x37,0x04,0x40},
{0x37,0x0d,0x0f},
{0x37,0x13,0x9f},
{0x37,0x14,0x4c},
{0x37,0x10,0x9e},
{0x38,0x01,0xc4},
{0x36,0x05,0x05},
{0x36,0x06,0x3f},
{0x30,0x2d,0x90},
{0x37,0x0b,0x40},
{0x37,0x16,0x31},
{0x37,0x07,0x52},
{0x38,0x0d,0x74},
{0x38,0x10,0x10},
/*LENC BPC OFF*/
{0x50,0x00,0x00},
/*AWB OFF*/
{0x50,0x01,0x4e},
{0x51,0x81,0x20},
{0x51,0x8f,0x00},
{0x43,0x01,0xff},
{0x43,0x03,0x00},
{0x3a,0x00,0x78},
{0x30,0x0f,0x88},
{0x30,0x11,0x28},
/*control PLL div*/
{0x30,0x12,0x00},
{0x3a,0x1a,0x06},
{0x3a,0x18,0x00},
{0x3a,0x19,0x7a},
{0x3a,0x13,0x54},
{0x38,0x2e,0x0f},
{0x38,0x1a,0x1a},
{0x40,0x1d,0x02},
/*BLC Target to 0 */
{0x40,0x07,BLC_TARGET},
/*HSYNC mode*/
{0x47,0x00,0x05},
{0x56,0x88,0x03},
{0x56,0x84,0x07},
{0x56,0x85,0xa0},
{0x56,0x86,0x04},
{0x56,0x87,0x43},
{0x3a,0x0f,0x40},
{0x3a,0x10,0x38},
{0x3a,0x1b,0x48},
{0x3a,0x1e,0x30},
{0x3a,0x11,0x90},
{0x3a,0x1f,0x10},

{0x30,0x10,0x00},
};



void
sensorTg_ov2715_Install()
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
	/* TGGPIO configuration here */
	profLogPrintf(1,"@@@@@@@@@  initial OV2715  +++ \n");
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_PWDN|SENSOR_RESET,SENSOR_PWDN|SENSOR_RESET);

	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWDN,SENSOR_PWDN); /* PWDN High*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/

	tmrUsWait(10000); /*10ms*/

	halFrontClkCfg(MCLKMODE,1);

    sensorTgPllClkExtSet(17200000);
	/*sensorTgPllClkExtSet(16750000);*/

/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
	/*****TODO*****front reset after these???*/
	tmrUsWait(2000);

	cmdportid = sensorTgCfg.cmdportid_xtg;

	tmrUsWait(10000);  /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWDN, 0);
	tmrUsWait(50000); /*50ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); /* RESET Low*/
	tmrUsWait(10000);  /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWDN, SENSOR_PWDN);
	tmrUsWait(10000);  /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWDN, 0);
	tmrUsWait(10000);  /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(25000); /*25ms*/

	sensorSerialAsyncWrite(cmdportid, 1, cmd_reset_code[0]);
	sensorSerialFlush(cmdportid);
	tmrUsWait(10000);  /*10ms*/
	/*initial code*/
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_init_code)/sizeof(cmd_init_code[0]), cmd_init_code[0],0);

	tmrUsWait(10000);
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	halFrontClkCfg(MCLKMODE, PCLKMODE);
	profLogPrintf(1,"@@@@@@@@@  initial OV2715  fin\n");

}
static void
sensorDoFuncStop()
{
}


static void
sensorDoFuncPreviewSVGABegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	profLogAdd(1,"sensorDoFuncPreviewSVGABegin+++");
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorEventVvalidWait (1);
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_svga_code)/ sizeof(cmd_svga_code[0]),cmd_svga_code[0]);
	sensorSerialFlush(cmdportid);
	sensorSyncSigInit(&sensorSyncSigPreviewSVGACfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorEventVvalidWait (1);
	profLogAdd(1,"sensorDoFuncPreviewSVGABegin---");
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
	profLogAdd(1,"sensorDoFuncPreviewVGABegin+++");
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorEventVvalidWait (1);
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_vga_code)/ sizeof(cmd_vga_code[0]),cmd_vga_code[0]);
	sensorSerialFlush(cmdportid);
	sensorSyncSigInit(&sensorSyncSigPreviewVGACfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorEventVvalidWait (1);
	profLogAdd(1,"sensorDoFuncPreviewVGABegin+++");
	
}
static void
sensorDoFuncPreviewQVGABegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	profLogAdd(1,"sensorDoFuncPreviewQVGABegin+++");
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorEventVvalidWait (1);
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_qvga_code)/ sizeof(cmd_qvga_code[0]),cmd_qvga_code[0]);
	sensorSerialFlush(cmdportid);
	sensorSyncSigInit(&sensorSyncSigPreviewQVGACfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorEventVvalidWait (1);
	profLogAdd(1,"sensorDoFuncPreviewQVGABegin+++");
}

static void
sensorDoFuncPreviewHDBegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	profLogAdd(1,"sensorDoFuncPreviewHDBegin+++");
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorEventVvalidWait (1);
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_hd_code)/ sizeof(cmd_hd_code[0]),cmd_hd_code[0]);
	sensorSerialFlush(cmdportid);
	sensorSyncSigInit(&sensorSyncSigPreviewHDCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorEventVvalidWait (1);
	profLogAdd(1,"sensorDoFuncPreviewHDBegin+++");
}
static void
sensorDoFuncPreviewFHDBegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	profLogAdd(1,"sensorDoFuncPreviewFHDBegin+++");
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorEventVvalidWait (1);
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_fhd_code)/ sizeof(cmd_fhd_code[0]),cmd_fhd_code[0]);
	sensorSerialFlush(cmdportid);
	sensorSyncSigInit(&sensorSyncSigPreviewFHDCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorEventVvalidWait (1);
	profLogAdd(1,"sensorDoFuncPreviewFHDBegin---");
}

static void
sensorDoFuncPreviewFHD43Begin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	UINT32 cmdportid;
	printf("sensorDoFuncPreviewFHD43Begin+++");
	cmdportid = sensorTgCfg.cmdportid_xtg,
	sensorEventVvalidWait (1);
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_fhd_code)/ sizeof(cmd_fhd_code[0]),cmd_fhd_code[0]);
	sensorSerialFlush(cmdportid);
	sensorSyncSigInit(&sensorSyncSigPreviewFHD43Cfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorEventVvalidWait (1);
	printf("sensorDoFuncPreviewFHD43Begin---");
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

#if 0
static void
sensorDoFuncSnapEarly0()
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_still_code)/ sizeof(cmd_still_code[0]),cmd_still_code[0]);
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
}
#endif


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
}

/*static void
sensorDoFuncSnapDummy0()
{
}*/

static void
sensorDoFuncSnapReadoutField0()
{
	
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);


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
	sensor_exptbl_init();
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
}

#if SP5K_SENSOR_OV2715
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regData[3][3] ={
		{0x35,0x00,0x00},
		{0x35,0x01,0x00},
		{0x35,0x02,0x00}
		
	};
	regData[0][2] = (expLine>>12)&0xf;
	regData[1][2] = (expLine>>4 )&0xff;
	regData[2][2] = (expLine&0xf)<<4;

	profLogPrintf(1,"expLine(reg0) = %d [%x %x %x]",expLine,regData[0][0],regData[0][1],regData[0][2]);
	profLogPrintf(1,"expLine(reg1) = %d [%x %x %x]",expLine,regData[1][0],regData[1][1],regData[1][2]);
	profLogPrintf(1,"expLine(reg2) = %d [%x %x %x]",expLine,regData[2][0],regData[2][1],regData[2][2]);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,3, regData[0]);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
	sensor_event_wait(0x04);
	expLine = expLine;
}
#endif


