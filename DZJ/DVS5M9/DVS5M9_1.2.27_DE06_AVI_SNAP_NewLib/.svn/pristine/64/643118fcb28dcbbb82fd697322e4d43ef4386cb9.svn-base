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

#define MODEL_VERSION "$unp_tg_sonyimx083-0.00.00"

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
#include "api/sp5k_sensor_api.h"
#include <stdio.h>
#include "middleware/proflog.h"
/*#include "../../../../include/hal/hal_glob.h"*/
#include "../flow/sensor_model_snap.h"
#include "../include/customization/solution_def.h"

#define LANE_2_EN 0

/*#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)*/

#define MASK_EXP_CMD 0
#define BURST_CAP_EN 1
#define SENSOR_1080P_CROP_MODE 0/*enable or disable 1920x1080 crop mode.*/


/*default preview mode ...*/
#if 1
#define DEF_INIT_AFE_CODE     afe_vga_code
#define DEF_SYNC_SIG_CFG      sensorSyncSigPreviewVGACfg
#define DEF_SYNC_SIG_GEN_CFG  sensorSyncSigGenPreviewVGACfg
#define DEF_PREV_CFG_TBL      sensorTgPreviewVGACfg
#else
#define DEF_INIT_AFE_CODE     afe_1080P_code
#define DEF_SYNC_SIG_CFG      sensorSyncSigPreview1080PCfg
#define DEF_SYNC_SIG_GEN_CFG  sensorSyncSigGenPreview1080PCfg
#define DEF_PREV_CFG_TBL      sensorTgPreview1080PCfg
#endif
void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);

/* Sensor image size definition here */
#define NO             0
#define BY_PIXEL       1
#define BY_HD          0
#define NOUSE          0
#define DISABLE        0xFFFF
#define DONTCHANE      0x7FFFFFFF


#if 0 /*USE LMI0 as debug signal */
#define GPIO_DBG_PULSE()    if(1){sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,1<<0,1<<0);sp5kGpioWrite(SP5K_GPIO_GRP_LMI,1<<0,1);sp5kGpioWrite(SP5K_GPIO_GRP_LMI,1<<0,0);}
#else
#define GPIO_DBG_PULSE()
#endif

#define SENSOR_MCLK      (72000000)

#if(CPU_CLK==6)/*CPU_CLK = 300MHz PCLK 1X = 300MHz*/
#define MCLK2MCLK(a)     a
#define FACTOR_MCLK(a)   (a*417/100) /*300MHz/72MHz = 4.17*/
#elif (CPU_CLK==8)/*CPU_CLK = 360MHz MCLK = 240MHz*/
#define MCLK2MCLK(a)     (a*240/300) /*300MHz/240MHz for reshape timing*/
#define FACTOR_MCLK(a)   (a*333/100) /*240MHz/72MHz = 3.33*/
#else
#warning not support lower clock
#define MCLK2MCLK(a)     (a)
#define FACTOR_MCLK(a)   (a)
#endif


#define MODE_SYNC_EARLY  (600)

/*****TODO***multiple preview mode for VGA*/

/* For  preview size 640*480, 30fps */
#define PREVIEW_VGA_HSIZE  1536  /* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  1152  /* preview vertical size*/
#define PREVIEW_VGA_VTOTAL 3575  /* preview vertical size*/

/*For preview size, 1920*1080, 60fps*/
#define PREVIEW_1080P_HSIZE  2304    /* preview horiz. size*/
#define PREVIEW_1080P_VSIZE  1296    /* preview vertical size*/
#define PREVIEW_1080P_VTOTAL 2730    /* preview vertical size*/


/* For  preview size 1280*720, 60fps, cropping */
#define PREVIEW_720P_HSIZE  1536  /* preview horiz. size*/
#define PREVIEW_720P_VSIZE   864  /* preview vertical size*/
#define PREVIEW_720P_VTOTAL  2730  /* preview vertical size*/

/* For  preview size 640*480, 120 fps */
/* Orginal is 1536x384 */
/* for bandwidth issue => down scale to 1280x320 */
#define PREVIEW_QVGA_HSIZE  (1280)  /* preview horiz. size*/
#define PREVIEW_QVGA_VSIZE  320  /* preview vertical size*/
#define PREVIEW_QVGA_VTOTAL 825   /* preview vertical size*/

/* For 16M output. */
#define CAPTURE_HSIZE  (4608)		/* capture horiz. size*/
#define CAPTURE_VSIZE  (3456)		/* capture vertical size*/
#define CAPTURE_VTOTAL (3516)    /* preview vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/

/* For  preview size 640*480, 30fps */
#define PREVIEW_VGA_LINETOTAL   FACTOR_MCLK(1320)	/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  PREVIEW_VGA_VTOTAL	    /* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  PREVIEW_VGA_VTOTAL		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     (PREVIEW_VGA_LINETOTAL-PREVIEW_VGA_HSIZE-16)  /* preview Hd valid pixel data offset position*/

#define PREVIEW_VGA_HCROP_OFFSET (32-1)/*6330 BUG workaround: if (4n) use (4n-1)*/  /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     11    /* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         MCLK2MCLK(1500)  /* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         MCLK2MCLK(1600)	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL        (PREVIEW_VGA_VOFFSET-2) /*12*//*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE        (PREVIEW_VGA_VOFFSET-1) /*13*/ /*7*/	/* preview Vd reshap rise*/

/* For  preview size 320*240, 30fps */
#define PREVIEW_QVGA_LINETOTAL   FACTOR_MCLK(728)	/* preview Hd length*/
#define PREVIEW_QVGA_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_QVGA_FRAMETOTAL  PREVIEW_QVGA_VTOTAL	    /* preview Vd length*/
#define PREVIEW_QVGA_FRAMEBLANK  PREVIEW_QVGA_VTOTAL		/* preview Vd sync length*/
#define PREVIEW_QVGA_HOFFSET      (PREVIEW_QVGA_LINETOTAL-PREVIEW_QVGA_HSIZE-16)        /* preview Hd valid pixel data offset position*/

#define PREVIEW_QVGA_HCROP_OFFSET (32-1)/*6330 BUG workaround: if (4n) use (4n-1)*/
#define PREVIEW_QVGA_VOFFSET     11    /* preview Vd valid line data offset position*/
/* Preview resh_QVGA_pe sync signal definition here */
#define PREVIEW_QVGA_HRESHAPE_FALL         400 	/* preview Hd reshap fall*/
#define PREVIEW_QVGA_HRESHAPE_RISE         500	/* preview Hd reshap rise*/
#define PREVIEW_QVGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_QVGA_VRESHAPE_FALL         PREVIEW_QVGA_VOFFSET-2 /*12*//*4*/	/* preview Vd reshap fall*/
#define PREVIEW_QVGA_VRESHAPE_RISE        PREVIEW_QVGA_VOFFSET-1 /*13*/ /*7*/	/* preview Vd reshap rise*/

/*For preview size, 1280*720, 60fps, cropping*/
#define PREVIEW_720P_LINETOTAL   FACTOR_MCLK(1400)	/* preview Hd length*/
#define PREVIEW_720P_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_720P_FRAMETOTAL  PREVIEW_720P_VTOTAL	/* preview Vd length*/
#define PREVIEW_720P_FRAMEBLANK  PREVIEW_720P_VTOTAL	/* preview Vd sync length*/
#define PREVIEW_720P_HOFFSET     (PREVIEW_720P_LINETOTAL-PREVIEW_720P_HSIZE-16)      	/* preview Hd valid pixel data offset position*/

#define PREVIEW_720P_HCROP_OFFSET (32-1)/*6330 BUG workaround: if (4n) use (4n-1)*/
#define PREVIEW_720P_VOFFSET     11		/* preview Vd valid line data offset position*/
/* Preview resh_720P_pe sync signal definition here */
#define PREVIEW_720P_HRESHAPE_FALL         MCLK2MCLK(2900) 	/* preview Hd reshap fall*/
#define PREVIEW_720P_HRESHAPE_RISE         MCLK2MCLK(3000)  /* preview Hd reshap rise*/
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_720P_VRESHAPE_FALL         PREVIEW_720P_VOFFSET-2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_720P_VRESHAPE_RISE         PREVIEW_720P_VOFFSET-1 /*7*/	/* preview Vd reshap rise*/

/*For preview 1080P size, 1920*1080, 30fps*/
#define PREVIEW_1080P_LINETOTAL   FACTOR_MCLK(1940)/*FACTOR_MCLK(1760)*/	/* preview Hd length*/
#define PREVIEW_1080P_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_1080P_FRAMETOTAL  PREVIEW_1080P_VTOTAL	/* preview Vd length*/
#define PREVIEW_1080P_FRAMEBLANK  PREVIEW_1080P_VTOTAL	/* preview Vd sync length*/
#define PREVIEW_1080P_HOFFSET     (PREVIEW_1080P_LINETOTAL-PREVIEW_1080P_HSIZE-16)    	/* preview Hd valid pixel data offset position*/

#define PREVIEW_1080P_HCROP_OFFSET (48-1)/*6330 BUG workaround: if (4n) use (4n-1)*/
#define PREVIEW_1080P_VOFFSET     11		/* preview Vd valid line data offset position*/
/* Preview resh_1080P_pe sync signal definition here */
#define PREVIEW_1080P_HRESHAPE_FALL         MCLK2MCLK(3000) /* preview Hd reshap fall*/
#define PREVIEW_1080P_HRESHAPE_RISE         MCLK2MCLK(3100)	/* preview Hd reshap rise*/
#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_1080P_VRESHAPE_FALL         PREVIEW_1080P_VOFFSET-2 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_1080P_VRESHAPE_RISE         PREVIEW_1080P_VOFFSET-1 /*7*/	/* preview Vd reshap rise*/


/* Capture default definition here */
#define CAPTURE_LINETOTAL   FACTOR_MCLK(1820*2)	/* capture Hd length*/
#define CAPTURE_LINEBLANK   6			/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  CAPTURE_VTOTAL		/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  CAPTURE_VTOTAL		/* mipi use > vsize is more safe*/
#define CAPTURE_HOFFSET     (CAPTURE_LINETOTAL-CAPTURE_HSIZE-16)  /*256*/    /*Capturee Hd valid pixel data offset position*/

#define CAPTURE_HCROP_OFFSET (96-1)/*6330 BUG workaround: if (4n) use (4n-1)*/
#define CAPTURE_VOFFSET     23    		/* capture Vd valid line data offset position*/
/*#define CAPTURE_VOFFSET     (11+4) */	/* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL        MCLK2MCLK(100)	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE        MCLK2MCLK(150)	/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_HD /* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL		   CAPTURE_VOFFSET-2	/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_VRESHAPE_RISE		 CAPTURE_VOFFSET-1/* capture Vd reshap rise , count by pixel*/


/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKMODE	1	     /*mclk mode,'0'from I/O Pad,'1'from internal*/

#define MASTERPHASE	0     	     /*master clk phase*/
#define PIXELPHASE	3	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/

#define	PCLKNXSEL_INT	0x00
#define	PCLKNXSEL_LVDS	0x01
#define	PCLKNXSEL_EXT	0x02

#define	PCLK1XSEL_NX	0x00
#define	PCLK1XSEL_NX_2	0x04
#define	PCLK1XSEL_INT	0x08


#define PREVIEW_VGA_PCLK        300000
#define PREVIEW_1080P_PCLK      300000
#define CAPTURE_PCLK            300000

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_MASTER  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_1080P_CFAPATTERN	BAYER_GBRG          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_VGA_CFAPATTERN      BAYER_GBRG
#define PREVIEW_720P_CFAPATTERN     BAYER_GBRG
#define PREVIEW_QVGA_CFAPATTERN     BAYER_GBRG



#define	SNAP_CFAPATTERN		BAYER_GBRG       /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	1	            /*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		      0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM	  1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/
#define	CAPTURE_HDNUM_FRAME_START  110	/*the front interrupt line number of the snap new frame */

#define	PREVIEW_HDNUM_FRAME_START  110		/*the front interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_END	 	2 /*1970*/    	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT		3400/*3250*//*3281*//*1971*/    	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END		2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_CMDPORT		300		/*3281*//*976*/		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_CMDPORT		200		/*the command port interrupt line number of the preview new frame*/

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
#define	TG_SPI_I2C_MODE  0x11				/*TG SPI 2*/
#define	TG_FORMAT2  	 0					/*don't change*/
#define	TG_FORMAT3  	 0					/*don't change*/
#define	TG_SPEED    	 10
#define	TG_NBIT_REGDATA  16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */

#define CAP_FRAME_MODE  0        /* Frame mode or Rolling mode */

#define BIT0 (1)

#define SENSOR_RESET   BIT0

/*prototypes of sensor model methods*/
extern void sensorSyncSigGenInit(const sensorSyncSigGenSizeCfg_t *pcfg	);
extern UINT32 sensorModeGet(void);

void sesnorTestFunc();

static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreview1080PBegin();
static void sensorDoFuncPreview720PBegin();
static void sensorDoFuncPreviewQVGABegin();
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

static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();

static void sensorDoFuncSnapNext(UINT32 next_mode);


/*no definition */
/*#define sp5kSnapFlashCfgGet(a) 0*/


static const UINT8 afe_init_code_SWReset[][4]={
{0x81,0x00,0x00,0x38}, /* 0x0000*/ /* reset and standby*/
};

/* sony 16 M setting define */
static const UINT8 afe_init_code[][4]={
{0x81,0x00,0x03,0x13}, /* 0x0003*/ /* 4Lane */
{0x81,0x00,0x0f,0x02}, /* 0x000f*/ /* mode 2 */
{0x81,0x00,0x0c,0xff}, /* temp exp*/
{0x81,0x00,0x05,0x60}, /* temp gain*/
};

static const UINT8 afe_vga_code[][4]={
/*preview size 1536*1154*/
/*siggen 4032x3575*/
/*size   1536x1152*/
{0x81,0x00,0x0f,0x05}, /* 0x000f*/ /* mode 3 */
};
static const UINT8 afe_qvga_code[][4]={
/*preview size 1536*384*/
/*siggen 2912x825*/
/*size   1536x384*/
{0x81,0x00,0x0f,0x09}, /* 0x000f*/ /* mode 4 */
};

static const UINT8 afe_720P_code[][4]={
/*preview size 1536*864*/
/*siggen 5280x2730*/
/*size   1536x864*/
 {0x81,0x00,0x0f,0x04}, /* 0x000f*/ /* mode 3A */
};
static const UINT8 afe_1080P_code[][4]={
/*preview size 2304*1296*/
/*siggen 7040x2730*/
/*size   2304x1296*/
{0x81,0x00,0x0f,0x02}, /* 0x000f*/ /* mode 2 */

};

static const UINT8 afe_still_code[][4]={
/*preview size 4608*3456*/
/*siggen 7280x3600*/
/*size   4608x3456*/
{0x81,0x00,0x0f,0x00}, /* 0x000f*/ /* mode 0 */
};


/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg =
{
	.wordcntsel 	 =0 ,
	.Lvdsmode		 =1 ,
#if LANE_2_EN
	.Lvdslanenum	 =1 ,/*2 lane mode*/
#else
	.Lvdslanenum	 =0 ,/*4 lane mode*/
#endif
	.Lvdslanemode	 =0 ,
	.Lvdslaneswap	 =0 , /*LSB*/
	.Lvdsraw		 =0 ,
	.Lvdsoutsel 	 =0 ,
	.Lvdseofsel 	 =2 ,/*detect EOF syn code.*/
	.Lvdseolsel 	 =1 ,/*0: do not detect sync code if no crop enable, 1: use detect sync code if crop enable*/
	.vmode			 =1 ,
	.lvdsvdsrc		 =1 ,
	.lvds_dphy_sel	 =0 ,/*d phy*/
	.S2pphase		 =4 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	.Ctrlword_sol	 =0x800,
	.Ctrlword_eol    =0x9D0,
	.Ctrlword_sof	 =0x800,
	.Ctrlword_eof    =0xB60,
	.syncwrod0		 =0xFFF,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	.Exthd_num		 =5 ,
	.Exthd_pol		 =0 ,
	.Exthd_mode 	 =0 ,
	.cropenable 	 =1 ,
};

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
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
	.hsize = PREVIEW_VGA_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_VGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 29970170,
	.sensor_pclk = 72000000,
	.sensor_htotal = 336*3,	
	.pix_100khz = 1480,
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
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_QVGA_HSIZE,
		.ySize = PREVIEW_QVGA_VSIZE,
	},
	.hsize = PREVIEW_QVGA_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_QVGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_QVGA_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 239858195,
	.sensor_pclk = 72000000,
	.sensor_htotal = 364*2,	
	.pix_100khz = 1480,
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
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_720P_HSIZE,
		.ySize = PREVIEW_720P_VSIZE,
	},
	.hsize = PREVIEW_720P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_720P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 59950960,
	.sensor_pclk = 72000000,
	.sensor_htotal = 440*3,	
	.pix_100khz = 1480,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	#if SENSOR_1080P_CROP_MODE
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = (PREVIEW_1080P_HSIZE - 1920)/2,
		.yOffset = (PREVIEW_1080P_VSIZE - 1080)/2,
		.xSize = /*PREVIEW_1080P_HSIZE*/1920,
		.ySize = /*PREVIEW_1080P_VSIZE*/1080,
		.hratio = 1920,
		.vratio = 1080,
		.yuvW = 1920,
		.yuvH = 1080,

	},
	#else
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_1080P_HSIZE,
		.ySize = PREVIEW_1080P_VSIZE,
	},
	#endif
	.hsize = PREVIEW_1080P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 29970170,
	.sensor_pclk = 72000000,
	.sensor_htotal = 880*2,	
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
		.xSize = CAPTURE_HSIZE/2,
		.ySize = CAPTURE_VSIZE,
	},
	.hsize = CAPTURE_HSIZE/2 + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = CAPTURE_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = SNAP_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz =10000000,
	.pix_100khz = 1480,
};


static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &DEF_PREV_CFG_TBL,	/**&sensorTgPreviewVGACfg,*//*20111230@Sam VGA not ready use 1080P 30FPS to replace..*/
	&sensorTgPreviewQVGACfg,
    &sensorTgPreview720PCfg,   /*720P 60 fps.*/  
    &sensorTgPreview1080PCfg,
	&sensorTgPreview720PCfg,   /*720P 30 fps.*/
    &sensorTgPreviewFULLCfg,   /*FULL Scan mode not ready*/
};


static  sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
				sensorDoFuncSnapEarly0,
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
			},
			.snapFieldEnd = sensorDoFuncSnapFieldEnd,
			.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
			.snapNext = sensorDoFuncSnapNext,
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

#if 0  /*Not implement Fame mode yet*/
/****TODO****frame snap mode*/
static  sensorTgSnapCfg_t sensorTgSnapFrameCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0,
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
#endif 

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
&sensorTgSnapRollCfg,
/*&sensorTgSnapFrameCfg,    */ /*Not implement Fame mode yet*/
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
	.hsize           = PREVIEW_VGA_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
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
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
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
	.hsize           = PREVIEW_QVGA_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = PREVIEW_QVGA_VSIZE,
	.lastlinepix     = PREVIEW_QVGA_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_QVGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_QVGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_QVGA_HDNUM_CMDPORT,
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
	.hsize           = PREVIEW_720P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
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
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_720P_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080PCfg = {
	.linetotal       = PREVIEW_1080P_LINETOTAL,
	.lineblank       = PREVIEW_1080P_LINEBLANK,
	.frametotal      = PREVIEW_1080P_FRAMETOTAL,
	.frameblank      = PREVIEW_1080P_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_1080P_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_1080P_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_1080P_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_1080P_VRESHAPE_RISE,
	.hoffset         = PREVIEW_1080P_HOFFSET,
	.voffset         = PREVIEW_1080P_VOFFSET,
	.hsize           = PREVIEW_1080P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = PREVIEW_1080P_VSIZE,
	.lastlinepix     = PREVIEW_1080P_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_1080P_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_1080P_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_1080P_HDNUM_CMDPORT,
};


static const sensorSyncSigCfg_t sensorSyncSigPreviewFULLCfg = {
    .linetotal       = CAPTURE_LINETOTAL,
	.lineblank       = CAPTURE_LINEBLANK,
	.hreshape_fall   = CAPTURE_HRESHAPE_FALL,
	.hreshape_rise   = CAPTURE_HRESHAPE_RISE,
	.hoffset         = CAPTURE_HOFFSET,

	.frametotal      = CAPTURE_FRAMETOTAL,
	.frameblank      = CAPTURE_VSIZE,
	.vreshape_fall   = CAPTURE_VRESHAPE_FALL,
	.vreshape_rise   = CAPTURE_VRESHAPE_RISE,
	.voffset         = CAPTURE_VOFFSET,
	.hsize           = CAPTURE_HSIZE/2 + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
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
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
};



static  sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
      .linetotal       = CAPTURE_LINETOTAL,
      .lineblank       = CAPTURE_LINEBLANK,
      .hreshape_fall   = CAPTURE_HRESHAPE_FALL,
      .hreshape_rise   = CAPTURE_HRESHAPE_RISE,
      .hoffset         = CAPTURE_HOFFSET,

      .frametotal      = CAPTURE_FRAMETOTAL,
      .frameblank      = CAPTURE_VSIZE,
      .vreshape_fall   = CAPTURE_VRESHAPE_FALL,
      .vreshape_rise   = CAPTURE_VRESHAPE_RISE,
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
      .conf_yuvhdpol   = 0, /*yuvhd just before hdres 0:active low*/
      .conf_yuvvdpol   = 0, /*yuvvd just before vdres 0:active low*/
      .hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
      .hdnum_frame_end = CAPTURE_HDNUM_FRAME_END,
      .hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
};
/*MODE 3 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewVGACfg = {
 	  .hblank =64,
	  .htotal =336,
	  .vblank =16,
	  .vtotal =3575,
};
/*MODE 4 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewQVGACfg = {
 	  .hblank =64,
	  .htotal =364,
	  .vblank =16,
	  .vtotal =825,
};
/*MODE 3A*/
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreview720PCfg = {
 	  .hblank =64,
	  .htotal =440,
	  .vblank =16,
	  .vtotal =2730,
};
/*MODE 2*/
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreview1080PCfg = {
 	  .hblank =64,
	  .htotal =880,
	  .vblank =16,
	  .vtotal =2730,
};
/*MODE 0 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenCaptureFullCfg = {
 	  .hblank =64,
	  .htotal =1820,
	  .vblank =16,
	  .vtotal =3600,
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
	.defSyncSigCfg  = &DEF_SYNC_SIG_CFG,
	.verStr = MODEL_VERSION,
};



static void
sensorReportsize()
{
#if 0
	profLogPrintf(1,"mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	profLogPrintf(1,"front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	profLogPrintf(1,"imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	profLogPrintf(1,"rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	profLogPrintf(1,"rawmux output size    = (%d,%d)",READ16(0xb0002090),READ16(0xb0002092));
	profLogPrintf(1,"sig gen (ht hb vt vb) = (%d,%d,%d,%d)",READ16(0xb000909E),READ16(0xb000908C),READ16(0xb000908E),READ16(0xb00090A2));
#endif
}

static const sensorSyncSigCfg_t *p_size_pcg;
static sensorSyncSigGenSizeCfg_t *p_gen_pcg;
static UINT32 g_hoffset;

static void
sensorSnapModeAsyncCb(void)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	profLogAdd(1,"AsyncSet");
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,g_hoffset,DONTCHANE,DONTCHANE,DONTCHANE);
	sensorSyncSigInit(p_size_pcg);
	sensorSyncSigGenInit(p_gen_pcg);
}

static void
sensorSnapModeAsyncSet(const sensorSyncSigCfg_t *pspcg ,sensorSyncSigGenSizeCfg_t *pgpcg,UINT32 hoffset)
{
	p_size_pcg=pspcg;
	p_gen_pcg=pgpcg;
	g_hoffset=hoffset;
	sensor_frame_cb_reg(12,sensorSnapModeAsyncCb,0);/*SENSOR_IRQ_SVC_ORG_VD*/
}

void
sesnorTestFunc(void)
{
	sensorDoFuncPreviewQVGABegin();
}

void
SonyHtotalFineTune(sensorSyncSigGenSizeCfg_t *pcfg)
{
	UINT32 cdspclk,tgpllclk,mclk;
	tgpllclk=sensorTgPllClkExtGet()/1000;/*kHz*/
	halCdspMasterClockGet(&cdspclk,&mclk);/*kHz*/
	profLogPrintf(1,"htotal=%d++",pcfg->htotal);
	pcfg->htotal=pcfg->htotal*mclk/tgpllclk; /*Sony imx083 Htotal*/
	profLogPrintf(1,"htotal=%d--",pcfg->htotal);
}

/*UINT16
PanaSensorRead(UINT16 addr)
{
}*/

void
sensorTg_SONYIMX083_Install()
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
    PROF_LOG_ADD(1, "sensorDoFuncInit()___enter");

   /* TGGPIO configuration here */

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	halFrontClkCfg(MCLKMODE,0); /*internal clock*/

#if defined(HW_EVB_SPE7001) /*For EVB GPIO control */
	/* don't use fmgpio to reset, because sensor interface is 1.8V , FM is 3.3V*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,1<<4,1<<4);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,1<<4,0); /* RESET LOW*/
	tmrUsWait(25000); /*25ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,1<<4,1<<4); /* RESET High*/
#else /*For RDK USE*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,1<<0,1<<0);
	tmrUsWait(10000);  /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000);  /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); /* RESET Low*/
	tmrUsWait(10000);  /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(25000); /*25ms*/
#endif
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_ON);

	/*Debug only */

	/*Pixel Clock = 27MHz /27*48/2 = 24 MHz, 1080P= 15fps VGA=30fps*/
	sensorTgPllClkExtSet(SENSOR_MCLK);/*16MHz*/
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN,1,SENSOR_TGPLL_DRV_8MA,2,0);

	/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);

	/*****TODO*****front reset after these???*/

	/*HW suggestion keep it temporay */
	WRITE8(0xb000010a,0x18);
   	tmrUsWait(2000);
	WRITE8(0xb0000135, 0xff); /* I/O driving SDO/SCK*/
	WRITE8(0xb0000134, 0xff); /* I/O driving SDO/SCK*/


   	cmdportid = sensorTgCfg.cmdportid_xtg;
	tmrUsWait(2000);
	sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code_SWReset)/sizeof(afe_init_code_SWReset[0])), afe_init_code_SWReset[0],0);
  	tmrUsWait(20000);
	sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code)/sizeof(afe_init_code[0])), afe_init_code[0],0);

	sensorSerialBulkWrite(cmdportid,(sizeof(DEF_INIT_AFE_CODE)/sizeof(DEF_INIT_AFE_CODE[0])), DEF_INIT_AFE_CODE[0],0);

	/*TG PLL=MCLK out=24MHz*/
	sensorLVDSCfgSet(&sensorLVDSCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

	tmrUsWait(100000);	/* delay 100 ms*/

	if(1){
#if (CPU_CLK==6)		
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,300000); /* 300MHz */
#else
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,240000); /* 240MHz */
#endif
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9*/
	}
	else{
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,150000); /* 150 MHz */
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,0); /*enable preview mode 9*/
	}


	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/

	/*fine tune htotal*/
	SonyHtotalFineTune(&sensorSyncSigGenPreview1080PCfg);
	SonyHtotalFineTune(&sensorSyncSigGenPreview720PCfg);
	SonyHtotalFineTune(&sensorSyncSigGenPreviewVGACfg);
	SonyHtotalFineTune(&sensorSyncSigGenPreviewQVGACfg);
	SonyHtotalFineTune(&sensorSyncSigGenCaptureFullCfg);

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	sensorSyncSigGenInit(&DEF_SYNC_SIG_GEN_CFG);

	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	PROF_LOG_ADD(1, "sensorDoFuncInit()___done");

	tmrUsWait(10000);

	#if MODE_SYNC_EARLY
		sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
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
  cmdportid = sensorTgCfg.cmdportid_xtg;
  sensorSerialBulkWrite(cmdportid, sizeof(afe_vga_code)/sizeof(afe_vga_code[0]), afe_vga_code[0],0);
  GPIO_DBG_PULSE()
  sensorSnapModeAsyncSet(&sensorSyncSigPreviewVGACfg,&sensorSyncSigGenPreviewVGACfg,PREVIEW_VGA_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "mipi VGA cmd done");
}
static void
sensorDoFuncPreviewQVGABegin()
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialBulkWrite(cmdportid, sizeof(afe_qvga_code)/sizeof(afe_qvga_code[0]), afe_qvga_code[0],0);
	GPIO_DBG_PULSE()
	sensorSnapModeAsyncSet(&sensorSyncSigPreviewQVGACfg,&sensorSyncSigGenPreviewQVGACfg,PREVIEW_QVGA_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	PROF_LOG_ADD(1, "mipi QVGA cmd done");
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
  sensorSerialBulkWrite(cmdportid, sizeof(afe_720P_code)/sizeof(afe_720P_code[0]), afe_720P_code[0],0);
  GPIO_DBG_PULSE()
  sensorSnapModeAsyncSet(&sensorSyncSigPreview720PCfg,&sensorSyncSigGenPreview720PCfg,PREVIEW_720P_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "mipi 720P cmd done");

}

static void
sensorDoFuncPreview1080PBegin()
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
	sensorSerialBulkWrite(cmdportid, sizeof(afe_1080P_code)/sizeof(afe_1080P_code[0]), afe_1080P_code[0],0);
	GPIO_DBG_PULSE()
	sensorSnapModeAsyncSet(&sensorSyncSigPreview1080PCfg,&sensorSyncSigGenPreview1080PCfg,PREVIEW_1080P_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	PROF_LOG_ADD(1, "mipi 1080P done");

}
static void
sensorDoFuncPreviewFULLBegin()
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
  sensorSerialBulkWrite(cmdportid, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0],0);
  GPIO_DBG_PULSE()
  sensorSnapModeAsyncSet(&sensorSyncSigPreviewFULLCfg,&sensorSyncSigGenCaptureFullCfg,CAPTURE_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "mipi FULL cmd done");
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
    PROF_LOG_ADD(1, "SnapExpBegin()");
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0]);
	sensorSerialFlush(cmdportid);
	GPIO_DBG_PULSE()
	sensorSnapModeAsyncSet(&sensorSyncSigSnapCfg,&sensorSyncSigGenCaptureFullCfg,CAPTURE_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
	sensorReportsize();
}


static void
sensorDoFuncSnapEarly0()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapEarly0");
    PROF_LOG_ADD(1, "sensorDoFuncSnapEarly0");
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialAsyncWrite(cmdportid, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0]);
	sensorSerialFlush(cmdportid);
	sensorSnapModeAsyncSet(&sensorSyncSigSnapCfg,&sensorSyncSigGenCaptureFullCfg,CAPTURE_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
	sensorReportsize();
}

/* Flash trigger */
#if 1
void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param);
#if 0
static void
FlashDirectTrig(void *param)
{
    halFrontFlashCfgSet(0, 0);
    halFrontFlashTrig();
}
#endif
static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapFrexSet");
}
#endif

static void
sensorDoFuncSnapBshutterBegin()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapBshutterBegin()");
    sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);
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
}

static void
sensorDoFuncSnapShortExp()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapShortExp()");
    PROF_LOG_ADD(1, "sensorDoFuncSnapShortExp()");
}
static void
sensorDoFuncSnapExpEnd()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapExpEnd()");
    PROF_LOG_ADD(1, "sensorDoFuncSnapExpEnd()");
}

static void
sensorDoFuncSnapReadoutField0()
{
    PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutField0()");
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialFlush(cmdportid);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	sensorReportsize();
}
static void
sensorDoFuncSnapFieldEnd()
{
    PROF_LOG_ADD(1, "sensorDoFuncSnapFieldEnd()");
	GPIO_DBG_PULSE()
	sensorReportsize();
}

/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{
    PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutEnd()");
	GPIO_DBG_PULSE()
	sensorReportsize();
}

static void
sensorDoFuncSnapNext(UINT32 mode)
{
#if BURST_CAP_EN
	if(mode==0x40) {
		PROF_LOG_ADD(0,"@@@@ get sensor next mode is snap");
		sensorTgSnapRollCfg.skip_snap_readout_vd = 3;/*20110830 @ BASEFW change to 3*/
	}
	else {
		PROF_LOG_ADD(0,"@@@@ get sensor next mode is preview");
		sensorTgSnapRollCfg.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN;
	}
#endif

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
#if MASK_EXP_CMD
#else
		if((sensorModeGet()&0x40)==0x40)
			profLogAdd(1,"snap exp wr");
       	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
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

/******TODO****/

#if SP5K_SENSOR_SONYIMX083
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	UINT32 sub_line,vtotal;

	static UINT8 regdata[][4] = {
		{0x81,0x00,0x04,0x00},
		{0x81,0x00,0x05,0x00},
		{0x81,0x00,0x06,0x00},
		{0x81,0x00,0x08,0x00},
	};
	if(sensorModeGet(0x30)==0x30)
		vtotal=sensorSyncSigGenPreviewVGACfg.vtotal;
	else if(sensorModeGet(0x30)==0x31)
		vtotal=sensorSyncSigGenPreviewQVGACfg.vtotal;
	else if(sensorModeGet(0x30)==0x32)
		vtotal=sensorSyncSigGenPreview720PCfg.vtotal;
	else if(sensorModeGet(0x30)==0x33)
		vtotal=sensorSyncSigGenPreview1080PCfg.vtotal;
	else
		vtotal=sensorSyncSigGenCaptureFullCfg.vtotal;

	sub_line=vtotal-expLine;

	if((sub_line<10)||(expLine>vtotal))
	{
		printf("[ERROR] exp line out of range (%d~%d)\n ",10,vtotal);
		return;
	}
	PROF_LOG_PRINT(1,"expLine=%d sub_line=%d vtotal=%d",expLine,sub_line,vtotal);
	regdata[0][3]=sub_line&0xFF;
	regdata[1][3]=sub_line>>8;
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 4, regdata[0]);
}
#endif


