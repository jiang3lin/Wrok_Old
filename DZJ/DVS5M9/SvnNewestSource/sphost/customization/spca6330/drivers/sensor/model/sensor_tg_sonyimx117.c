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
 *  Author: Allen Liu                                                    *
 *                                                                        *
 *  01.01.01, 20130409 Allen
 *  Create a new tg.c for sonyimx117
 *  0x30(FHD):size 2000X1126 30fps, mode 2, siggen 462*2(*2)X2600
 *  -> Add Hdorg(fix Hsize, add Hblank) from original 462 to 462*2, besides frame rate reduces from original 60 to 30, this method could add Hblank to reduce dsp loading.
 *  0x31(HD):size 1332X998 60fps, mode 3, siggen 385(*3)X3120
 *  -> Modify lvds output from original 2 lanes to 4 lanes, but fix Hdorg 385 pixels and fix frame rate 60 fps, this method could add Hblank to reduce dsp loading.
 *  0x32(VGA):size 1332X332 120fps, mode 5, siggen 420(*2)X715
 *  0x33(QVGA):size 1332X174 120fps, mode 7, siggen 385(*2)X390
 *  0x40(Rolling):size 4000X3000 13.99fps, mode 0, siggen 1650(*1)X3120
 *  -> Modify lvds output from original 10 lanes to 4 lanes.
 *
 **************************************************************************/
#define MODEL_VERSION "$unp_tg_sonyimx117-01.01.01"

#include "common.h"
#include "../flow/sysregs.h"
#include "../flow/hal_front.h"
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include <stdio.h>
#include "middleware/proflog.h"
#include "../flow/sensor_model_snap.h"
#include "../include/customization/solution_def.h"

/*#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)*/

/* define flag to switch */
#define MASK_EXP_CMD 	0
#define BURST_CAP_EN 	1
#define CAP_FRAME_MODE  	0	/* Frame mode or Rolling mode */
#define CAP_BREAK_MODE		1

/* default preview mode ... */
#define DEF_INIT_AFE_CODE     afe_fhd_code
#define DEF_SYNC_SIG_CFG      sensorSyncSigPreviewFHDCfg
#define DEF_SYNC_SIG_GEN_CFG  sensorSyncSigGenPreviewFHDCfg
#define DEF_PREV_CFG_TBL      sensorTgPreviewFHDCfg

/* define GPIO */
#define SENSOR_RESET_GRP	GPIO_GRP_TGL
#define SENSOR_RESET_GPIO   (1<<4)	/* don't use fmgpio to reset, because sensor interface is 1.8V , FM is 3.3V*/

/* Sensor image size definition here */
#define NO             	0
#define BY_PIXEL       	1
#define BY_HD          	0
#define NOUSE          	0
#define DISABLE        	0xFFFF
#define DONTCHANE   	0x7FFFFFFF

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

#define MODE_SYNC_EARLY  (50)

/* For (mode 2) preview size 2000*1126, 60fps */
#define PREVIEW_FHD_HSIZE  			2000	/* preview horiz. size*/
#define PREVIEW_FHD_VSIZE  			1126	/* preview vertical size*/
#define PREVIEW_FHD_VTOTAL 		1300  	/* preview vertical total*/
/* Preview default definition here */
#define PREVIEW_FHD_LINETOTAL   	FACTOR_MCLK(950)		/* preview Hd length*/
#define PREVIEW_FHD_LINEBLANK   	6						/* preview Hd sync length*/
#define PREVIEW_FHD_FRAMETOTAL  	PREVIEW_FHD_VTOTAL	    	/* preview Vd length*/
#define PREVIEW_FHD_FRAMEBLANK  	PREVIEW_FHD_VTOTAL		/* preview Vd sync length*/
#define PREVIEW_FHD_HOFFSET     	(PREVIEW_FHD_LINETOTAL-PREVIEW_FHD_HSIZE-16)  /* preview Hd valid pixel data offset position*/
#define PREVIEW_FHD_VOFFSET     	11    	/* preview Vd valid line data offset position*/
#define PREVIEW_FHD_HCROP_OFFSET (32-1)	/*6330 BUG workaround: if (4n) use (4n-1)*/
/* Preview reshape sync signal definition here */
#define PREVIEW_FHD_HRESHAPE_FALL         		MCLK2MCLK(4340)  		/* preview Hd reshap fall*/
#define PREVIEW_FHD_HRESHAPE_RISE         		MCLK2MCLK(4440)			/* preview Hd reshap rise*/
#define PREVIEW_FHD_VRESHAPE_COUNTER_SEL  	BY_HD					/* preview Vd reshap control*/
#define PREVIEW_FHD_VRESHAPE_FALL        		(PREVIEW_FHD_VOFFSET-2) /* preview Vd reshap fall*/
#define PREVIEW_FHD_VRESHAPE_RISE        		(PREVIEW_FHD_VOFFSET-1) /* preview Vd reshap rise*/

/* For (mode 3) preview size 1332*998, 60fps */
#define PREVIEW_HD_HSIZE  			1332	/* preview horiz. size*/
#define PREVIEW_HD_VSIZE  			998		/* preview vertical size*/
#define PREVIEW_HD_VTOTAL 			1040  	/* preview vertical total*/
/* Preview default definition here */
#define PREVIEW_HD_LINETOTAL   		FACTOR_MCLK(630)		/* preview Hd length*/
#define PREVIEW_HD_LINEBLANK   		6						/* preview Hd sync length*/
#define PREVIEW_HD_FRAMETOTAL  	PREVIEW_HD_VTOTAL	    	/* preview Vd length*/
#define PREVIEW_HD_FRAMEBLANK  	PREVIEW_HD_VTOTAL		/* preview Vd sync length*/
#define PREVIEW_HD_HOFFSET     		(PREVIEW_HD_LINETOTAL-PREVIEW_HD_HSIZE-16)  /* preview Hd valid pixel data offset position*/
#define PREVIEW_HD_VOFFSET     		11    	/* preview Vd valid line data offset position*/
#define PREVIEW_HD_HCROP_OFFSET 	(32-1)	/*6330 BUG workaround: if (4n) use (4n-1)*/
/* Preview reshape sync signal definition here */
#define PREVIEW_HD_HRESHAPE_FALL         		MCLK2MCLK(2360)  		/* preview Hd reshap fall*/
#define PREVIEW_HD_HRESHAPE_RISE         		MCLK2MCLK(2460)		/* preview Hd reshap rise*/
#define PREVIEW_HD_VRESHAPE_COUNTER_SEL  	BY_HD					/* preview Vd reshap control*/
#define PREVIEW_HD_VRESHAPE_FALL        			(PREVIEW_HD_VOFFSET-2) 	/* preview Vd reshap fall*/
#define PREVIEW_HD_VRESHAPE_RISE        			(PREVIEW_HD_VOFFSET-1) 	/* preview Vd reshap rise*/

/* For (mode 5) preview size 1332*332, 240fps */
#define PREVIEW_VGA_HSIZE  			1332	/* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  			332		/* preview vertical size*/
#define PREVIEW_VGA_VTOTAL 		357  	/* preview vertical total*/
/* Preview default definition here */
#define PREVIEW_VGA_LINETOTAL   	FACTOR_MCLK(950)		/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   	6						/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  	PREVIEW_VGA_VTOTAL	    	/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  	PREVIEW_VGA_VTOTAL		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     	(PREVIEW_VGA_LINETOTAL-PREVIEW_VGA_HSIZE-16)  /* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     	11    	/* preview Vd valid line data offset position*/
#define PREVIEW_VGA_HCROP_OFFSET 	(32-1)	/*6330 BUG workaround: if (4n) use (4n-1)*/
/* Preview reshape sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         		MCLK2MCLK(1100)  		/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         		MCLK2MCLK(1200)		/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  	BY_HD					/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL        		(PREVIEW_VGA_VOFFSET-2) /* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE        		(PREVIEW_VGA_VOFFSET-1) /* preview Vd reshap rise*/

/* For (mode 7) preview size 1332*174, 480fps */
#define PREVIEW_QVGA_HSIZE  		1332	/* preview horiz. size*/
#define PREVIEW_QVGA_VSIZE  		174		/* preview vertical size*/
#define PREVIEW_QVGA_VTOTAL 		195  	/* preview vertical total*/
/* Preview default definition here */
#define PREVIEW_QVGA_LINETOTAL   	FACTOR_MCLK(950)		/* preview Hd length*/
#define PREVIEW_QVGA_LINEBLANK   	6						/* preview Hd sync length*/
#define PREVIEW_QVGA_FRAMETOTAL  	PREVIEW_QVGA_VTOTAL	/* preview Vd length*/
#define PREVIEW_QVGA_FRAMEBLANK  	PREVIEW_QVGA_VTOTAL	/* preview Vd sync length*/
#define PREVIEW_QVGA_HOFFSET     	(PREVIEW_QVGA_LINETOTAL-PREVIEW_QVGA_HSIZE-16)  /* preview Hd valid pixel data offset position*/
#define PREVIEW_QVGA_VOFFSET     	11    	/* preview Vd valid line data offset position*/
#define PREVIEW_QVGA_HCROP_OFFSET (32-1)	/*6330 BUG workaround: if (4n) use (4n-1)*/
/* Preview reshape sync signal definition here */
#define PREVIEW_QVGA_HRESHAPE_FALL         		MCLK2MCLK(440)  		   /* preview Hd reshap fall*/
#define PREVIEW_QVGA_HRESHAPE_RISE         		MCLK2MCLK(540)			   /* preview Hd reshap rise*/
#define PREVIEW_QVGA_VRESHAPE_COUNTER_SEL  	BY_HD					   /* preview Vd reshap control*/
#define PREVIEW_QVGA_VRESHAPE_FALL        		(PREVIEW_QVGA_VOFFSET-2) /* preview Vd reshap fall*/
#define PREVIEW_QVGA_VRESHAPE_RISE        		(PREVIEW_QVGA_VOFFSET-1) /* preview Vd reshap rise*/

/* For (mode 0) 12M output. */
#define CAPTURE_HSIZE  			4000	/* capture horiz. size*/
#define CAPTURE_VSIZE  			3000	/* capture vertical size*/
#define CAPTURE_VTOTAL 			3120    	/* capture vertical total*/
/* Capture default definition here */
#define CAPTURE_LINETOTAL   	FACTOR_MCLK(950)	/* capture Hd length*/
#define CAPTURE_LINEBLANK   	6						/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  	CAPTURE_VTOTAL			/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  	CAPTURE_VTOTAL			/* mipi use > vsize is more safe*/
#define CAPTURE_HOFFSET     		(CAPTURE_LINETOTAL-CAPTURE_HSIZE-16)   /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET    		11    	/* capture Vd valid line data offset position*/
#define CAPTURE_HCROP_OFFSET 	(96-1)	/*6330 BUG workaround: if (4n) use (4n-1)*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL        	MCLK2MCLK(320)		/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE       		MCLK2MCLK(420)		/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL BY_HD 				/* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL		   	(CAPTURE_VOFFSET-2)	/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_VRESHAPE_RISE		 	(CAPTURE_VOFFSET-1)	/* capture Vd reshap rise , count by pixel*/

/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKMODE		1	/*mclk mode,'0'from I/O Pad,'1'from internal*/
#define MASTERPHASE	0  	/*master clk phase*/
#define PIXELPHASE		3	/*pixel clk phase*/
#define ADCKPHASE		0	/*adck clk phase*/

#define PCLKNXSEL_INT	0x00
#define PCLKNXSEL_LVDS	0x01
#define PCLKNXSEL_EXT	0x02

#define PCLK1XSEL_NX	0x00
#define PCLK1XSEL_NX_2	0x04
#define PCLK1XSEL_INT	0x08

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL	TYPE_CMOS	    			/*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      							TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define TGTYPE			TG_CMOS_SP5K_MASTER  	/*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      							TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define PREVIEW_FHD_CFAPATTERN	BAYER_GBRG /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_HD_CFAPATTERN		BAYER_GBRG /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_VGA_CFAPATTERN	BAYER_GBRG /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_QVGA_CFAPATTERN	BAYER_GBRG /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define	SNAP_CFAPATTERN			BAYER_GBRG	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM		0                   	/*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM		1	            	/*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM		1	            	/*for typical CCD==n_dummy + n_readout + 1*/
#define	SNAP_EARLY_NUM		      	1	            	/*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM	1	            	/*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM					0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM					0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  			0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  	2		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 					0		/*sony FALSE;panasonic/ITG TRUE*/

#define	PREVIEW_FHD_HDNUM_FRAME_START 110		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_FHD_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_FHD_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_HD_HDNUM_FRAME_START  	110		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_HD_HDNUM_FRAME_END		2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_HD_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_START 110		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT		300		/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_QVGA_HDNUM_FRAME_START  	50		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_FRAME_END		2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_CMDPORT			150		/*the command port interrupt line number of the preview new frame*/

#define	CAPTURE_HDNUM_FRAME_START  	110		/*the front interrupt line number of the snap new frame */
#define	CAPTURE_HDNUM_FRAME_END	2 		/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT		2800	/*the command port interrupt line number of the snap new frame*/

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
#define	TG_SPI_I2C_MODE  	0x11	/*Set TG SPI interface*/
#define	TG_FORMAT2  	 	0		/*don't change*/
#define	TG_FORMAT3  	 	0		/*don't change*/
#define	TG_SPEED    	 		10
#define	TG_NBIT_REGDATA  	16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  	16		/*ssc/spi register address numbers */

/**************global variable *******************/
static const sensorSyncSigCfg_t *p_size_pcg;
static sensorSyncSigGenSizeCfg_t *p_gen_pcg;
static UINT32 g_hoffset;
static UINT32 cmdportid_tg;
static UINT32 g_last_sub_line;
static UINT8 g_last_smode;
static UINT8 g_last_svr;
static UINT8 g_last_spl;
static UINT8 g_IsSnap2Pv=0;

extern UINT32 sensorModeGet();
extern void sensorSyncSigGenInit(const sensorSyncSigGenSizeCfg_t *pcfg	);

void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewFHDBegin();
static void sensorDoFuncPreviewHDBegin();
static void sensorDoFuncPreviewVGABegin();
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
{0x81,0x00,0x00,0x04},
};

static const UINT8 afe_breakmode[][4]={
{0x81,0x00,0x02,0x01}, /*Break Mode*/
};

/*sony 12M setting define*/
static const UINT8 afe_init_code[][4]={
#if 0
{0x81,0x00,0x08,0x00}, /*rolling shutter*/
{0x81,0x00,0x1A,0x00}, /*vertical direction*/
{0x81,0x00,0x45,0x32}, /*black level*/
#endif
{0x81,0x00,0x03,0x33}, /*LVDS 4 channels*/
{0x81,0x00,0x0B,0x27}, /*SHR start pos(temp exp), unit:line(low-byte)*/
{0x81,0x00,0x0C,0x05}, /*SHR start pos(temp exp), unit:line(high-byte)*/
{0x81,0x00,0x0D,0x00}, /*SVR (can reduce framerate)*/
{0x81,0x00,0x0F,0x00}, /*SPL*/
};

static const UINT8 afe_fhd_code[][4]={
/*sensor output size 2000X1126*/
/*siggen 462*2(*2)X2600*/
/*size   2000X1126*/
/*mode 2*/
{0x81,0x00,0x04,0x19}, /*MDSEL1*/
{0x81,0x00,0x05,0x0D}, /*MDSEL2*/
{0x81,0x00,0x06,0x00}, /*MDSEL3*/
{0x81,0x00,0x07,0x00}, /*MDSEL4*/
{0x81,0x00,0x26,0x74}, /*MDSEL13*/
{0x81,0x00,0x27,0x74}, /*MDSEL14*/
{0x81,0x00,0x28,0x74}, /*MDSEL15*/
{0x81,0x00,0x7E,0x20}, /*MDSEL5 low-byte*/
{0x81,0x00,0x7F,0x01}, /*MDSEL5 high-byte*/
{0x81,0x00,0xB6,0x67}, /*MDSEL6*/
{0x81,0x00,0xB7,0x00}, /*MDSEL7*/
{0x81,0x00,0xB8,0x00}, /*MDSEL8*/
{0x81,0x00,0xB9,0x00}, /*MDSEL9*/
{0x81,0x00,0xBA,0x00}, /*MDSEL10*/
{0x81,0x00,0xBB,0x00}, /*MDSEL11*/
{0x81,0x00,0xCE,0x0E}, /*MDSEL12*/
};

static const UINT8 afe_hd_code[][4]={
/*sensor output size 1332X998*/
/*siggen 385(*3)X3120*/
/*size   1332X998*/
/*mode 3*/
{0x81,0x00,0x04,0x2A}, /*MDSEL1*/
{0x81,0x00,0x05,0x1D}, /*MDSEL2*/
{0x81,0x00,0x06,0x00}, /*MDSEL3*/
{0x81,0x00,0x07,0x00}, /*MDSEL4*/
{0x81,0x00,0x26,0x74}, /*MDSEL13*/
{0x81,0x00,0x27,0x74}, /*MDSEL14*/
{0x81,0x00,0x28,0x74}, /*MDSEL15*/
{0x81,0x00,0x7E,0x20}, /*MDSEL5 low-byte*/
{0x81,0x00,0x7F,0x01}, /*MDSEL5 high-byte*/
{0x81,0x00,0xB6,0x77}, /*MDSEL6*/
{0x81,0x00,0xB7,0x00}, /*MDSEL7*/
{0x81,0x00,0xB8,0x00}, /*MDSEL8*/
{0x81,0x00,0xB9,0x77}, /*MDSEL9*/
{0x81,0x00,0xBA,0x00}, /*MDSEL10*/
{0x81,0x00,0xBB,0x00}, /*MDSEL11*/
{0x81,0x00,0xCE,0x10}, /*MDSEL12*/
};

static const UINT8 afe_vga_code[][4]={
/*sensor output size 1332X332*/
/*siggen 420(*2)X715*/
/*size   1332X332*/
/*mode 5*/
{0x81,0x00,0x04,0x51}, /*MDSEL1*/
{0x81,0x00,0x05,0x1D}, /*MDSEL2*/
{0x81,0x00,0x06,0x00}, /*MDSEL3*/
{0x81,0x00,0x07,0x00}, /*MDSEL4*/
{0x81,0x00,0x26,0x74}, /*MDSEL13*/
{0x81,0x00,0x27,0x74}, /*MDSEL14*/
{0x81,0x00,0x28,0x74}, /*MDSEL15*/
{0x81,0x00,0x7E,0x20}, /*MDSEL5 low-byte*/
{0x81,0x00,0x7F,0x01}, /*MDSEL5 high-byte*/
{0x81,0x00,0xB6,0x09}, /*MDSEL6*/
{0x81,0x00,0xB7,0x14}, /*MDSEL7*/
{0x81,0x00,0xB8,0x14}, /*MDSEL8*/
{0x81,0x00,0xB9,0x09}, /*MDSEL9*/
{0x81,0x00,0xBA,0x14}, /*MDSEL10*/
{0x81,0x00,0xBB,0x14}, /*MDSEL11*/
{0x81,0x00,0xCE,0x10}, /*MDSEL12*/
};

static const UINT8 afe_qvga_code[][4]={
/*sensor output size 1332X174*/
/*siggen 385(*2)X390*/
/*size   1332X174*/
/*mode 7*/
{0x81,0x00,0x04,0x61}, /*MDSEL1*/
{0x81,0x00,0x05,0x1D}, /*MDSEL2*/
{0x81,0x00,0x06,0x00}, /*MDSEL3*/
{0x81,0x00,0x07,0x00}, /*MDSEL4*/
{0x81,0x00,0x26,0x74}, /*MDSEL13*/
{0x81,0x00,0x27,0x74}, /*MDSEL14*/
{0x81,0x00,0x28,0x74}, /*MDSEL15*/
{0x81,0x00,0x7E,0x20}, /*MDSEL5 low-byte*/
{0x81,0x00,0x7F,0x01}, /*MDSEL5 high-byte*/
{0x81,0x00,0xB6,0x09}, /*MDSEL6*/
{0x81,0x00,0xB7,0x00}, /*MDSEL7*/
{0x81,0x00,0xB8,0x28}, /*MDSEL8*/
{0x81,0x00,0xB9,0x09}, /*MDSEL9*/
{0x81,0x00,0xBA,0x00}, /*MDSEL10*/
{0x81,0x00,0xBB,0x28}, /*MDSEL11*/
{0x81,0x00,0xCE,0x10}, /*MDSEL12*/
};

static const UINT8 afe_still_code[][4]={
/*sensor output size 4000X3000*/
/*siggen 1650(*1)X3120*/
/*size   4000X3000*/
/*mode 0*/
#if 0
{0x81,0x00,0x0B,0xDC}, /*SHR start pos(temp exp), unit:line(low byte)*/
{0x81,0x00,0x0C,0x0A}, /*SHR start pos(temp exp), unit:line(high byte)*/
{0x81,0x00,0x0D,0x00}, /*SVR*/
{0x81,0x00,0x0F,0x00}, /*SPL*/
#endif
{0x81,0x00,0x04,0x00}, /*MDSEL1*/
{0x81,0x00,0x05,0x07}, /*MDSEL2*/
{0x81,0x00,0x06,0x00}, /*MDSEL3*/
{0x81,0x00,0x07,0x00}, /*MDSEL4*/
{0x81,0x00,0x26,0x74}, /*MDSEL13*/
{0x81,0x00,0x27,0x74}, /*MDSEL14*/
{0x81,0x00,0x28,0x74}, /*MDSEL15*/
{0x81,0x00,0x7E,0x20}, /*MDSEL5 low-byte*/
{0x81,0x00,0x7F,0x01}, /*MDSEL5 high-byte*/
{0x81,0x00,0xB6,0x47}, /*MDSEL6*/
{0x81,0x00,0xB7,0x00}, /*MDSEL7*/
{0x81,0x00,0xB8,0x00}, /*MDSEL8*/
{0x81,0x00,0xB9,0x00}, /*MDSEL9*/
{0x81,0x00,0xBA,0x00}, /*MDSEL10*/
{0x81,0x00,0xBB,0x00}, /*MDSEL11*/
{0x81,0x00,0xCE,0x0E}, /*MDSEL12*/
};

/*sensor lvds cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg =
{
	.wordcntsel 	=0,
	.Lvdsmode	=1,
	.Lvdslanenum	 =0, /*0:4 lane mode, 1:2 lane mode*/
	.Lvdslanemode =0,
	.Lvdslaneswap	 =0, /*LSB*/
	.Lvdsraw		 =0,
	.Lvdsoutsel 	 =0,
	.Lvdseofsel 	 =2, /*detect EOF syn code.*/
	.Lvdseolsel 	 =1, /*0: do not detect sync code if no crop enable, 1: use detect sync code if crop enable*/
	.vmode		 =1,
	.lvdsvdsrc	 =1,
	.lvds_dphy_sel =0,
	.S2pphase	 =4,
	.tsettle		 =3,
	.teot			 =7,
	.Ctrlword_sol	 =0x800,
	.Ctrlword_eol   =0x9D0,
	.Ctrlword_sof	 =0x800,
	.Ctrlword_eof   =0xB60,
	.syncwrod0	 =0xFFF,
	.syncwrod1	 =0,
	.syncwrod2	 =0,
	.Exthd_num	 =5,
	.Exthd_pol	 =0,
	.Exthd_mode 	 =0,
	.cropenable 	 =1,
};

/*sensor cfg*/
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
	.hsize = PREVIEW_FHD_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_FHD_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_FHD_CFAPATTERN,
	.hratio = 2000,
	.vratio = 1126,
	.fps_mhz = 29970000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 462*2*2,
	.pix_100khz = 1480,
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
	.hsize = PREVIEW_HD_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_HD_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_HD_CFAPATTERN,
	.hratio = 1332,
	.vratio = 998,
	.fps_mhz = 59950000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 385*3,	
	.pix_100khz = 1480,
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
	.hsize = PREVIEW_VGA_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_VGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
	.hratio = 1332,
	.vratio = 332,
	.fps_mhz = 119870000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 420*2,	
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
	.hratio = 1332,
	.vratio = 174,
	.fps_mhz = 119860000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 385*2,	
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
		.xSize = CAPTURE_HSIZE,
		.ySize = CAPTURE_VSIZE,
	},
	.hsize = CAPTURE_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = CAPTURE_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = SNAP_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 13990000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 1650*1,
	.pix_100khz = 1480,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &DEF_PREV_CFG_TBL, &sensorTgPreviewHDCfg, &sensorTgPreviewVGACfg, &sensorTgPreviewQVGACfg, &sensorTgPreviewFULLCfg
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

#if 0
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
&sensorTgSnapRollCfg,/*&sensorTgSnapFrameCfg,*/
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
	.hsize           = PREVIEW_FHD_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = PREVIEW_FHD_VSIZE,
	.lastlinepix     = PREVIEW_FHD_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_FHD_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_FHD_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_FHD_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_FHD_HDNUM_CMDPORT,
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
	.hsize           = PREVIEW_HD_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = PREVIEW_HD_VSIZE,
	.lastlinepix     = PREVIEW_HD_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_HD_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HD_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_HD_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_HD_HDNUM_CMDPORT,
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
	.hdnum_frame_start = PREVIEW_QVGA_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_QVGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_QVGA_HDNUM_CMDPORT,
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

/*MODE 2 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewFHDCfg = {
 	  .hblank 	=64,
	  .htotal 	=462*2, /*add Hblank to reduce dsp loading*/
	  .vblank 	=16,
	  .vtotal 	=2600,
};

/*MODE 3 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewHDCfg = {
 	  .hblank 	=64,
	  .htotal 	=385,
	  .vblank 	=16,
	  .vtotal 	=3120,
};

/*MODE 5 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewVGACfg = {
 	  .hblank 	=64,
	  .htotal 	=420,
	  .vblank 	=16,
	  .vtotal 	=715,
};

/*MODE 7 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewQVGACfg = {
 	  .hblank 	=64,
	  .htotal 	=385,
	  .vblank 	=16,
	  .vtotal 	=390,
};

/*MODE 0 */
static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenCaptureFullCfg = {
 	  .hblank 	=64,
	  .htotal 	=1650,
	  .vblank 	=16,
	  .vtotal 	=3120,
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

static void sensorReportsize()
{
	profLogPrintf(1,"mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	profLogPrintf(1,"front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	profLogPrintf(1,"imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	profLogPrintf(1,"rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	profLogPrintf(1,"rawmux output size    = (%d,%d)",READ16(0xb0002090),READ16(0xb0002092));
	profLogPrintf(1,"sig gen (ht hb vt vb) = (%d,%d,%d,%d)",READ16(0xb000909E),READ16(0xb000908C),READ16(0xb000908E),READ16(0xb00090A2));
}

static void sensorSnapModeAsyncCb(void)
{
	profLogAdd(1,"AsyncSet");
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,g_hoffset,DONTCHANE,DONTCHANE,DONTCHANE);
	sensorSyncSigInit(p_size_pcg);
	sensorSyncSigGenInit(p_gen_pcg);
}

static void sensorSnapModeAsyncSet(const sensorSyncSigCfg_t *pspcg ,sensorSyncSigGenSizeCfg_t *pgpcg,UINT32 hoffset)
{
	p_size_pcg=pspcg;
	p_gen_pcg=pgpcg;
	g_hoffset=hoffset;

	if(g_IsSnap2Pv){
		sensor_frame_cb_reg(12,sensorSnapModeAsyncCb,0);/*SENSOR_IRQ_SVC_ORG_VD*/
#if !CAP_BREAK_MODE
		if(++g_IsSnap2Pv>2) /*make snap2pv and pv2snap could wait Vdorg*/
#endif
			g_IsSnap2Pv=0;
	}
	else{
		sensorSnapModeAsyncCb();
	}
}

static void expline_convert_wr()
{
	UINT32 line_time1,exp_line1,frame_length1;/*last mode info*/
	UINT32 line_time2,exp_line2,frame_length2;/*present mode info*/
	UINT32 smode;
	UINT8 svr,spl;/*present mode info*/

	static UINT8 regdata[][4] = {
		{0x81,0x00,0x0B,0x00}, /*SHR start pos(temp exp), unit:line(low byte)*/
		{0x81,0x00,0x0C,0x00}, /*SHR start pos(temp exp), unit:line(high byte)*/
		{0x81,0x00,0x0D,0x00}, /*SVR*/
		{0x81,0x00,0x0F,0x00}, /*SPL*/
	};

	smode=sensorModeGet();

	if(g_last_smode==0x30){
		frame_length1=sensorSyncSigGenPreviewFHDCfg.vtotal;
		line_time1=sensorSyncSigGenPreviewFHDCfg.htotal;
	}
	else if(g_last_smode==0x31){
		frame_length1=sensorSyncSigGenPreviewHDCfg.vtotal;
		line_time1=sensorSyncSigGenPreviewHDCfg.htotal;
	}
	else if(g_last_smode==0x32){
		frame_length1=sensorSyncSigGenPreviewVGACfg.vtotal;
		line_time1=sensorSyncSigGenPreviewVGACfg.htotal;
	}
	else if(g_last_smode==0x33){
		frame_length1=sensorSyncSigGenPreviewQVGACfg.vtotal;
		line_time1=sensorSyncSigGenPreviewQVGACfg.htotal;
	}
	else{
		return;
	}

	exp_line1=(g_last_svr-g_last_spl+1)*frame_length1-g_last_sub_line;

	if(smode==0x30){
		line_time2=sensorSyncSigGenPreviewFHDCfg.htotal;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=sensorSyncSigGenPreviewFHDCfg.vtotal;
		spl=0;
	}
	else if(smode==0x31){
		line_time2=sensorSyncSigGenPreviewHDCfg.htotal;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=sensorSyncSigGenPreviewHDCfg.vtotal;
		spl=0;
	}
	else if(smode==0x32){
		line_time2=sensorSyncSigGenPreviewVGACfg.htotal;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=sensorSyncSigGenPreviewVGACfg.vtotal;
		spl=1;
	}
	else if(smode==0x33){
		line_time2=sensorSyncSigGenPreviewQVGACfg.htotal;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=sensorSyncSigGenPreviewQVGACfg.vtotal;
		spl=3;
	}
	else{
		return;
	}	

	profLogPrintf(1,"=> 0x%x (%d)=>(%d) (%d)=>(%d)",smode,exp_line1,exp_line2,frame_length1,frame_length2);
	svr=exp_line2/frame_length2;/*divisor*/
	exp_line2-=svr*frame_length2;/*remainder*/
	if(svr<spl){
		if(spl>1){
			spl-=svr;/*ex: exp 1 frame => svr=1&spl=3 => svr=3&spl=2*/
		}
		svr+=spl;
	}
	else{
		spl=0;
	}
	
	regdata[0][3]=(frame_length2-exp_line2)&0xFF;
	regdata[1][3]=(frame_length2-exp_line2)>>8;
	regdata[2][3]=svr;
	regdata[3][3]=spl;
	sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg, 4, regdata[0],0);
}

void SonyHtotalFineTune(sensorSyncSigGenSizeCfg_t *pcfg)
{
	UINT32 cdspclk,tgpllclk,mclk;
	tgpllclk=sensorTgPllClkExtGet()/1000; /*kHz*/
	halCdspMasterClockGet(&cdspclk,&mclk); /*kHz*/
	profLogPrintf(1,"htotal=%d++",pcfg->htotal);
	pcfg->htotal=pcfg->htotal*mclk/tgpllclk; /*Sony imx117 Htotal*/
	profLogPrintf(1,"htotal=%d--",pcfg->htotal);
}

void sensorTg_SONYIMX117_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

static void sensorDoFuncInit()
{
	printf("SENSOR TG AFE Version %s\n",MODEL_VERSION );
	PROF_LOG_ADD(1, "sensorDoFuncInit()___enter");

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	halFrontClkCfg(MCLKMODE,0); /*internal clock*/

	/*Reset sensor*/
	sp5kGpioCfgSet(SENSOR_RESET_GRP, SENSOR_RESET_GPIO, SENSOR_RESET_GPIO);
	sp5kGpioWrite(SENSOR_RESET_GRP, SENSOR_RESET_GPIO, 0); /* RESET LOW*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SENSOR_RESET_GRP, SENSOR_RESET_GPIO, SENSOR_RESET_GPIO); /* RESET High*/

	sensorTgPllClkExtSet(SENSOR_MCLK);/*72MHz*/
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN,1,SENSOR_TGPLL_DRV_8MA,2,0);

	/*Master clock and Pixel clock configuration here*/
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);

	/*HW suggestion keep it temporay */
	WRITE8(0xb000010a,0x18);
   	tmrUsWait(2000);
	WRITE8(0xb0000135, 0xff); /* I/O driving SDO/SCK*/
	WRITE8(0xb0000134, 0xff); /* I/O driving SDO/SCK*/

	sensorSerialBulkWrite(cmdportid_tg,(sizeof(afe_init_code_SWReset)/sizeof(afe_init_code_SWReset[0])), afe_init_code_SWReset[0],0);
  	tmrUsWait(10000);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(afe_init_code)/sizeof(afe_init_code[0])), afe_init_code[0],0);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(DEF_INIT_AFE_CODE)/sizeof(DEF_INIT_AFE_CODE[0])), DEF_INIT_AFE_CODE[0],0);

	/*TG PLL=MCLK out=24MHz*/
	sensorLVDSCfgSet(&sensorLVDSCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);
	tmrUsWait(10000);

#if (CPU_CLK==6)		
	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,300000); /* 300MHz */
#else
	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,240000); /* 240MHz */
#endif
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/

	/*fine tune htotal*/
	SonyHtotalFineTune(&sensorSyncSigGenPreviewFHDCfg);
	SonyHtotalFineTune(&sensorSyncSigGenPreviewHDCfg);
	SonyHtotalFineTune(&sensorSyncSigGenPreviewVGACfg);
	SonyHtotalFineTune(&sensorSyncSigGenPreviewQVGACfg);
	SonyHtotalFineTune(&sensorSyncSigGenCaptureFullCfg);

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	sensorSyncSigGenInit(&DEF_SYNC_SIG_GEN_CFG);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif

	PROF_LOG_ADD(1, "sensorDoFuncInit()___done");

	/*sp5kGpioCfgSet(GPIO_GRP_GEN, 1<<15, 1<<15);
	sp5kGpioWrite(GPIO_GRP_GEN, 1<<15, 0);*/
}

static void sensorDoFuncStop()
{
}

static void sensorDoFuncPreviewFHDBegin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/
  if(!g_IsSnap2Pv)
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_breakmode)/sizeof(afe_breakmode[0]), afe_breakmode[0],0);
  sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_fhd_code)/sizeof(afe_fhd_code[0]), afe_fhd_code[0],0);
  expline_convert_wr();
  sensorSnapModeAsyncSet(&sensorSyncSigPreviewFHDCfg,&sensorSyncSigGenPreviewFHDCfg,PREVIEW_FHD_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "lvds FHD cmd done");
}

static void sensorDoFuncPreviewHDBegin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/
  if(!g_IsSnap2Pv)
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_breakmode)/sizeof(afe_breakmode[0]), afe_breakmode[0],0);
  sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_hd_code)/sizeof(afe_hd_code[0]), afe_hd_code[0],0);
  expline_convert_wr();
  sensorSnapModeAsyncSet(&sensorSyncSigPreviewHDCfg,&sensorSyncSigGenPreviewHDCfg,PREVIEW_HD_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "lvds HD cmd done");
}

static void sensorDoFuncPreviewVGABegin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/
  if(!g_IsSnap2Pv)
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_breakmode)/sizeof(afe_breakmode[0]), afe_breakmode[0],0);
  sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_vga_code)/sizeof(afe_vga_code[0]), afe_vga_code[0],0);
  expline_convert_wr();
  sensorSnapModeAsyncSet(&sensorSyncSigPreviewVGACfg,&sensorSyncSigGenPreviewVGACfg,PREVIEW_VGA_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "lvds VGA cmd done");
}

static void sensorDoFuncPreviewQVGABegin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/
  if(!g_IsSnap2Pv)
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_breakmode)/sizeof(afe_breakmode[0]), afe_breakmode[0],0);
  sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_qvga_code)/sizeof(afe_qvga_code[0]), afe_qvga_code[0],0);
  expline_convert_wr();
  sensorSnapModeAsyncSet(&sensorSyncSigPreviewQVGACfg,&sensorSyncSigGenPreviewQVGACfg,PREVIEW_QVGA_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "lvds QVGA cmd done");
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
  if(!g_IsSnap2Pv)
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_breakmode)/sizeof(afe_breakmode[0]), afe_breakmode[0],0);
  sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0],0);
  sensorSnapModeAsyncSet(&sensorSyncSigSnapCfg,&sensorSyncSigGenCaptureFullCfg,CAPTURE_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
  sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  PROF_LOG_ADD(1, "lvds FULL cmd done");
}

static void sensorDoFuncPreviewShortExp()
{
	/*front_preview_short_exp_frame_set*/
}

static void sensorDoFuncPreviewExpEnd()
{
	/*front_preview_exp_end_frame_set*/
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
#if !SNAP_EARLY_NUM
   	PROF_LOG_ADD(1, "SnapExpBegin()");
#if !CAP_BREAK_MODE
	g_IsSnap2Pv=1;
#endif
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_breakmode)/sizeof(afe_breakmode[0]), afe_breakmode[0],0);
	sensorSerialAsyncWrite(cmdportid_tg, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0]);
	sensorSerialFlush(cmdportid_tg);
	sensorSnapModeAsyncSet(&sensorSyncSigSnapCfg,&sensorSyncSigGenCaptureFullCfg,CAPTURE_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
#if CAP_BREAK_MODE
	g_IsSnap2Pv=1;
#endif
#endif
}

static void sensorDoFuncSnapEarly0()
{
    	PROF_LOG_ADD(1, "sensorDoFuncSnapEarly0");
#if !CAP_BREAK_MODE
	g_IsSnap2Pv=1;
#endif
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_breakmode)/sizeof(afe_breakmode[0]), afe_breakmode[0],0);
	sensorSerialAsyncWrite(cmdportid_tg, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0]);
	sensorSerialFlush(cmdportid_tg);
	sensorSnapModeAsyncSet(&sensorSyncSigSnapCfg,&sensorSyncSigGenCaptureFullCfg,CAPTURE_HCROP_OFFSET);/*Syncsig /Master HDVD/ H crop offset*/
#if CAP_BREAK_MODE
	g_IsSnap2Pv=1;
#endif
}

/* Flash trigger */
#if 0
static void FlashDirectTrig(void *param)
{
    halFrontFlashCfgSet(0, 0);
    halFrontFlashTrig();
}
#endif

static void sensorDoFuncSnapFrexSet(UINT32 param)
{
	PROF_LOG_ADD(1, "sensorDoFuncSnapFrexSet");
}


static void sensorDoFuncSnapBshutterBegin()
{
	PROF_LOG_ADD(1, "sensorDoFuncSnapBshutterBegin()");
    	sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);
}

static void sensorDoFuncSnapBshutterFollow()
{
	PROF_LOG_ADD(1, "sensorDoFuncSnapBshutterFollow()");
}

static void sensorDoFuncSnapBshutterEnd()
{
	PROF_LOG_ADD(1, "sensorDoFuncSnapBshutterEnd()");
}

static void sensorDoFuncSnapShortExp()
{
    	PROF_LOG_ADD(1, "sensorDoFuncSnapShortExp()");
}

static void sensorDoFuncSnapExpEnd()
{
    	PROF_LOG_ADD(1, "sensorDoFuncSnapExpEnd()");
}

static void sensorDoFuncSnapReadoutField0()
{
    	PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutField0()");
	sensorSerialFlush(cmdportid_tg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
}

static void sensorDoFuncSnapFieldEnd()
{
    	PROF_LOG_ADD(1, "sensorDoFuncSnapFieldEnd()");
}

static void sensorDoFuncSnapReadoutEnd()
{
    	PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutEnd()");
	sensorReportsize();
}

static void sensorDoFuncSnapNext(UINT32 mode)
{
#if BURST_CAP_EN
	if(mode==0x40) {
		PROF_LOG_ADD(1,"@@@@ get sensor next mode is snap");
		sensorTgSnapRollCfg.skip_snap_readout_vd = 3;/*20110830 @ BASEFW change to 3*/
	}
	else {
		PROF_LOG_ADD(1,"@@@@ get sensor next mode is preview");
		sensorTgSnapRollCfg.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN;
	}
#endif
}

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
	cmdportid_tg =
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

static void sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	sensorLVDSEnSet(0); /*Disable mipi interface*/
}

static void sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
#if !MASK_EXP_CMD
	UINT32 smode=sensorModeGet();

	if((smode&0xF0)==0x30){
		g_last_sub_line=(pdata[4*1+3]<<8)|(pdata[4*0+3]);
		g_last_svr=pdata[4*2+3];
		g_last_spl=pdata[4*3+3];
		g_last_smode=smode;
		profLogPrintf(1,"[%d %d %d 0x%x]", g_last_sub_line, g_last_svr, g_last_spl, g_last_smode);
	}
	else{
		profLogAdd(1,"snap exp wr");
	}
   	sensorSerialAsyncWrite(cmdportid_tg, nreg, pdata);
#endif
}

static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
	/*PROF_LOG_PRINTF(1,"sensor_cmdport_exp_vtbl_do\n");*/
}

#if SP5K_SENSOR_SONYIMX117
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	UINT32 sub_line,vtotal,smode;
	UINT8 min_line,min_dif;

	static UINT8 regdata[][4] = {
		{0x81,0x00,0x0B,0x00},
		{0x81,0x00,0x0C,0x00},
		{0x81,0x00,0x0D,0x00},/*SVR*/
		{0x81,0x00,0x0F,0x00},/*SPL*/
	};

	smode=sensorModeGet();
	
	if(smode==0x30){
		vtotal=sensorSyncSigGenPreviewFHDCfg.vtotal;
		min_dif=4;/*min(framelength-expline)*/
		min_line=10;/*min SHR*/
	}
	else if(smode==0x31){
		vtotal=sensorSyncSigGenPreviewHDCfg.vtotal;
		min_dif=8;
		min_line=15;
	}
	else if(smode==0x32){
		vtotal=sensorSyncSigGenPreviewVGACfg.vtotal;
		min_dif=2;
		min_line=4;
		regdata[2][3]=0x1;
		regdata[3][3]=0x1;
	}
	else if(smode==0x33){
		vtotal=sensorSyncSigGenPreviewQVGACfg.vtotal;
		min_dif=2;
		min_line=4;
		regdata[2][3]=0x3;
		regdata[3][3]=0x3;
	}
	else /*if(smode==0x40)*/{
		vtotal=sensorSyncSigGenCaptureFullCfg.vtotal;
		min_dif=4;
		min_line=8;
	}

	sub_line=vtotal-expLine;

	if((sub_line<min_line)||(expLine>(vtotal-min_dif)))
	{
		printf("[ERROR] exp line out of range (%d~%d)\n ",min_line,(vtotal-min_dif));
		return;
	}
	PROF_LOG_PRINT(1,"expLine=%d sub_line=%d vtotal=%d",expLine,sub_line,vtotal);
	regdata[0][3]=sub_line&0xFF;
	regdata[1][3]=sub_line>>8;
	sensorEventNewFrameWait(1);
	sensorSerialAsyncWrite(cmdportid_tg, 4, regdata[0]);
	sensorSerialFlush(cmdportid_tg);
}
#endif
