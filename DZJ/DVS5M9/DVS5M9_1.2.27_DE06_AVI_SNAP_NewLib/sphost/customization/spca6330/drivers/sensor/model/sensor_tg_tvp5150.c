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
 *  01.01.01, 20130701 Chris                                              *
 *            Data Rate:                                                  *
 *            sensor mode:                                                *
 *              0x30:            fps                                      *
 *              0x31:            fps                                      *
 *              0x32:            fps                                      *
 *              0x40:            fps                                      *
 *                                                                        *
 *            Note:                                                       *
 *              1. I2C ID control by the SID pin                          *
 *                   1: 0xBA                                              *
 *                   0: 0xB9                                              *
 *              2.TVEncoder                                               *
 **************************************************************************/
#define MODEL_VERSION "$unp_tg_tvp5150-01.01.01"
#define BUILD_TIME        __DATE__  "-"  __TIME__
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

#define TVP5150_CMD          (0)
#define SENSOR_PROF_LOG_EN   (1)
#define PROF_ID              (3)
#define EVTBL_TEST_CODE      (1) /* use static exp table not from resource.*/
void tvp5150RegDump();
#if SENSOR_PROF_LOG_EN
    #define _SENSOR_DBG_ 1
    #define SENSOR_PROF_LOG_ADD(str)                profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)      profLogPrintf(PROF_ID, fmt, ##arg)
    #define WRITELINE (*((volatile UINT16 *)(0xb0009064))  = (1))
    #define READLINE  (*((volatile UINT16 *)(0xb0009064)))
    #define PROF_LOG_LINE_COUNT(str,args...)\
    do{\
        WRITELINE;\
        profLogPrintf(0,"[5150] %s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(1,"[5150] %s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(2,"[5150] %s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(3,"[5150] %s_"str" @ %d",__func__, ##args, READLINE);\
    }while(0)
    static UINT32 snap_time_log[7];
    #define TIME_TAG(id)   (snap_time_log[id])
    #define TIME_LOG(id)    snap_time_log[id]=tmrTimeClockGet()
    #define TIME_DIFF(a,b) (snap_time_log[a]-snap_time_log[b])
#else
    #define _SENSOR_DBG_ 0
    #define SENSOR_PROF_LOG_ADD(str)
    #define SENSOR_PROF_LOG_PRINT(fmt,arg...)
    #define PROF_LOG_LINE_COUNT(str,args...)
    #define TIME_TAG(id)
    #define TIME_LOG(id)
    #define TIME_DIFF(a,b)
#endif
#if(_SENSOR_DBG_)
    /*debug by TGGPIO2*/
    #define DBG_GPIO(x) if(x){WRITE8(0xb0009032, (READ8(0xb0009032)|(0x02)));}else{WRITE8(0xb0009032, (READ8(0xb0009032)&(~0x02)));}
#else
    #define DBG_GPIO(x)
#endif
#define EVB                        (1)
#define SUSPEND_ENABLE             (0)
#define STREAMOF_ENABLE            (0)
#define HCROP_ENABLE               (0)
#define MODE_SYNC_EARLY            (0)
static UINT32 g_IsSnap2Pv;
/*static UINT8  is_aborted;*/
static BOOL   fpvsusp;
#if SUSPEND_ENABLE
static UINT32 sensorSuspended;
#endif

static UINT8 g_curChannel = 0x00 ; /* mapping to register 0x00 */
static UINT8 g_curInput   = 0x00 ; /* mapping to register 0x8C */
static UINT8 g_Reset      = 0    ;
#if 0
#define SENSOR_FREX          (1<<15)
#define SENSOR_PWRDN         (1<<14)
#define SENSOR_RESET         (1<<12)
#define SENSOR_STROBE        (1<<13)
#define OUT_SENSOR_GPIO_MASK     SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR      SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define SENOR_GPIO_GRP           SP5K_GPIO_GRP_LMI
#endif
/*****************************************************************************************/
#define SENSOR_MCLK      (27000000)
/* SPCA6330 */
#define PIX_100KHZ       ( 27000)
#define SPCA_PCLK1X      (300000)
#define MCLK2MCLK(clk)     (clk*SPCA_PCLK1X*100/SENSOR_MCLK)
/* Sensor image size definition here */
/************************** Sensor image size definition here ****************************/
#define NO             			   (0)
#define BY_PIXEL                   (1)
#define BY_HD                      (0)
#define NOUSE          			   (0)
#define DISABLE        			   (0xFFFF)
/* PIXELS PER FREQUENCY 13.5MHz */
#define PREVIEW_NTSC_HOUTPUT        ( 720) /* PIXELS PER LINES 858 , ACTIVE PIXELS PER LINES 720 */
#define PREVIEW_NTSC_VOUTPUT        ( 240) /* 525 LINES PER FRAME */
#define PREVIEW_NTSC_PIXELSTOTAL    ( 858)

#define PREVIEW_PAL_HOUTPUT         ( 720) /* PIXELS PER LINES 864 */
#define PREVIEW_PAL_VOUTPUT         ( 288) /* 625 288 */
#define PREVIEW_PAL_PIXELSTOTAL     ( 864)

#define PREVIEW_EXTEND_LINES        (0)

#define PREVIEW_NTSC_HSIZE          (PREVIEW_NTSC_HOUTPUT)
#define PREVIEW_NTSC_VSIZE          (PREVIEW_NTSC_VOUTPUT)

#define PREVIEW_PAL_HSIZE           (PREVIEW_PAL_HOUTPUT)
#define PREVIEW_PAL_VSIZE           (PREVIEW_PAL_VOUTPUT)

#define CAPTURE_HSIZE               (PREVIEW_NTSC_HSIZE)
#define CAPTURE_VSIZE               (PREVIEW_NTSC_VSIZE)
/*****************************************************************************************/
/* #if SPCA6330 */
#define PREVIEW_NTSC_LINETOTAL                   (NOUSE)
#define PREVIEW_NTSC_LINEBLANK                   (NOUSE)
#define PREVIEW_NTSC_FRAMETOTAL                  (NOUSE)
#define PREVIEW_NTSC_FRAMEBLANK                  (NOUSE)
#define PREVIEW_NTSC_HOFFSET                     (  128)
#define PREVIEW_NTSC_HCROP_OFFSET                (   NO)
#define PREVIEW_NTSC_VOFFSET                     (   20)
#define PREVIEW_NTSC_HRESHAPE_FALL               (   10)
#define PREVIEW_NTSC_HRESHAPE_RISE               (   20)
#define PREVIEW_NTSC_VRESHAPE_COUNTER_SEL        (   NO)
#define PREVIEW_NTSC_VRESHAPE_FALL               (   16)
#define PREVIEW_NTSC_VRESHAPE_RISE               (   18)

#define PREVIEW_PAL_LINETOTAL                    (NOUSE)
#define PREVIEW_PAL_LINEBLANK                    (NOUSE)
#define PREVIEW_PAL_FRAMETOTAL                   (NOUSE)
#define PREVIEW_PAL_FRAMEBLANK                   (NOUSE)
#define PREVIEW_PAL_HOFFSET                      (  134)
#define PREVIEW_PAL_HCROP_OFFSET                 (   NO)
#define PREVIEW_PAL_VOFFSET                      (   22)
#define PREVIEW_PAL_HRESHAPE_FALL                (   24)
#define PREVIEW_PAL_HRESHAPE_RISE                (   34)
#define PREVIEW_PAL_VRESHAPE_COUNTER_SEL         (   NO)
#define PREVIEW_PAL_VRESHAPE_FALL                (   16)
#define PREVIEW_PAL_VRESHAPE_RISE                (   18)

#define CAPTURE_LINETOTAL                        (NOUSE)
#define CAPTURE_LINEBLANK                        (NOUSE)
#define CAPTURE_FRAMETOTAL                       (NOUSE)
#define CAPTURE_FRAMEBLANK                       (NOUSE)
#define CAPTURE_HOFFSET                          (  138)
#define CAPTURE_HCROP_OFFSET                     (   NO)
#define CAPTURE_VOFFSET                          (   24)
#define CAPTURE_HRESHAPE_FALL                    (   10)
#define CAPTURE_HRESHAPE_RISE                    (   20)
#define CAPTURE_VRESHAPE_COUNTER_SEL             (   NO)
#define CAPTURE_VRESHAPE_FALL                    (   16)
#define CAPTURE_VRESHAPE_RISE                    (   18)

#define	PREVIEW_HDNUM_FRAME_START                 (20)  /* unit : line */
#define	PREVIEW_NTSC_HDNUM_FRAME_END              (2) /* unit : line */
#define	PREVIEW_NTSC_HDNUM_CMDPORT                (230) /* unit : line */

#define	PREVIEW_PAL_HDNUM_FRAME_END               (2)
#define	PREVIEW_PAL_HDNUM_CMDPORT                 (230)

#define	CAPTURE_HDNUM_FRAME_START                 (20)
#define	CAPTURE_HDNUM_FRAME_END                   (2)
#define	CAPTURE_HDNUM_CMDPORT                     (230)
/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV     1        /* mclk drivng, '1' : 8mA, '0' : 4mA */
#define MCLKMODE	0	     /*mclk mode,'0'from I/O Pad,'1'from internal*/
#define PCLKMODE	1	     /*pclk mode,'0'from I/O Pad,'1'from internal*/
#define TGPLLEN   	0	     /*enable TGPLL,'0'disable,'1'enable*/
#define TGPLLCLKSET	8	     /*TGPLL clk 0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,
					                     5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/
#define MCLKDIV		1	     /*MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV		2	     /*pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE	0     	 /*master clk phase*/
#define PIXELPHASE	0	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/
#define RGBADJ		0	     /*RGB input data sample phase adj "1"negedge "0"posedge*/
#define	PCLKNXSEL_INT	0x00
#define	PCLKNXSEL_LVDS	0x01
#define	PCLKNXSEL_EXT	0x02
#define	PCLK1XSEL_NX	0x00
#define	PCLK1XSEL_NX_2	0x04
#define	PCLK1XSEL_INT	0x08
/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		              TYPE_CMOS	          /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                                    TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
									  /* TYPE_TVIN_DEC */
#define	TGTYPE			              TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						                                    TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
														  /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
									  /* TG_TVIN_DEC */
#define PREVIEW_NTSC_CFAPATTERN       BAYER_RGGB
#define PREVIEW_PAL_CFAPATTERN        BAYER_RGGB
#define	SNAP_CFAPATTERN		          BAYER_RGGB
#define	SNAP_DUMMYVD_NUM	          0             /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM		      1             /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM		      1             /*for typical CCD==n_dummy + n_readout + 1*/
#define	SNAP_EARLY_NUM		          0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		  0             /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	          0		        /*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM			 	  0             /*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN        0            	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  1	            /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	          0		        /*sony FALSE;panasonic/ITG TRUE*/
#define	EXP_FRAME_NUM       	      2	            /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/
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
/* TVP5150 the I2C ID is controlled by the YOUT7/I2CSEL pin
   YOUT7/I2CSEL High: 0xBA , Low:0xB8                       */
#define	TG_SPI_I2C_MODE  0xB8     	                                         /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	 0		                                             /*don't change*/
#define	TG_SPEED         FRONT_SSC_CLK_384K
 					                              /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						                            FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#define	TG_NBIT_REGADDR  8		                  /*ssc/spi register address numbers */
#define	TG_NBIT_REGDATA  8		                  /*ssc/spi register data numbers */
/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
#if STREAMOF_ENABLE
static void sensorDoFuncStreamOff(UINT32 abort);
static void sensorDoFuncStreamOn (UINT32 abort);
#else
/*static void sensorDoFuncStreamOff();
static void sensorDoFuncStreamOn();*/
#endif
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewCCIR601Begin();
static void sensorDoFuncPreviewCCIR656Begin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
/*static void sensorDoFuncSnapDummy0();*/
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
#if 0
static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapEarly1();
#endif
static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();
static void sensorDoFuncSnapNext(UINT32 next_mode);
static UINT32 sensorResumeExecuteLoad();
static UINT32 sensorSuspendExecuteLoad();
void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);

extern UINT32 sensorModeGet();
extern UINT32 tmrTimeClockGet();
extern UINT32 halPllFrontClkSet(UINT32 pclkSel,UINT32 mclk);
/*-----------------------------------------------------------------------------------------------*/
static const UINT8 cmd_stream_off[][2]SENSOR_SIF1DMA_BUF={
    {0x03,0x05},
};

static const UINT8 cmd_stream_on[][2]SENSOR_SIF1DMA_BUF={
    {0x03,0x0d},
};

static const UINT8 cmd_init_code[][2]SENSOR_SIF1DMA_BUF={
	/*-----------------------------------------------
	; MCLK: 24Mhz
	; PCLK: 27Mhz
	; SystemCLK     : xxx->MHz
	; FPS	        : xxx->240
	; HTS		    : xxx->
	; VTS		    : xxx->
	; Tline 	    : xxx->
	; Max exp line	: xxx->
	;-------------------------------------------------*/
    {0x00,0x00},    /* bit1 0: AIP1A(J2), 1:AIP1B(J1)   */
    {0x03,0x0d},    /* TVPOE, HsyncVsync OE, CLK OE     */
    {0x04,0xc0},
	{0x0d,0x40},
};
static const UINT8 cmd_still_code[][2]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; PCLK: 27Mhz
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->240
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/
    {0x03,0x0d},
};
static const UINT8 cmd_CCIR601_code[][2]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; PCLK: 27Mhz
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->240
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/
    {0x00,0x00},    /* bit1 0: AIP1A(J2), 1:AIP1B(J1)   */
    {0x03,0x0d},    /* TVPOE, HsyncVsync OE, CLK OE     */
    {0x04,0xc0},
	{0x0d,0x40},
};
static const UINT8 cmd_CCIR656_code[][2]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; PCLK: 27Mhz
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->240
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/
    {0x00,0x00},
    {0x03,0x0b},
};
/*-----------------------------------------------------------------------------------------------*/
/*sensor cfg*/
static sensorTgPreviewCfg_t sensorTgPreviewCCIR601Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewCCIR601Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = sensorDoFuncPreviewShortExp,
		.previewLongExpFollow = sensorDoFuncPreviewShortExp,
		.previewLongExpEnd = sensorDoFuncPreviewShortExp,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
#if HCROP_ENABLE
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_NTSC_HSIZE,
		.ySize = PREVIEW_NTSC_VSIZE,
	},
#else
    .crop = NULL,
#endif
	.hsize = PREVIEW_NTSC_HSIZE,
	.vsize = PREVIEW_NTSC_VSIZE,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_YUV,
 	.cfa_pattern = PREVIEW_NTSC_CFAPATTERN,
    .hratio = PREVIEW_NTSC_HSIZE,     /* 16:9 fixed hratio = 3200 , vratio = 1800 */
    .vratio = PREVIEW_NTSC_VSIZE,     /* 4:3  fixed hratio = 4000 , vratio = 3000 */
    .fps_mhz = 50100000,              /* frame rate * 1000000 */
    .sensor_pclk = SENSOR_MCLK,       /* Hz */
    .sensor_htotal = PREVIEW_NTSC_PIXELSTOTAL , /* per line of pixel HW , htotal/sensor_pclk */
    .pix_100khz = PIX_100KHZ/100,    /* pclk1x speed Datarate * lanes / bit x HSize / HTotal */
};
static const sensorTgPreviewCfg_t sensorTgPreviewCCIR656Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewCCIR656Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = sensorDoFuncPreviewShortExp,
		.previewLongExpFollow = sensorDoFuncPreviewShortExp,
		.previewLongExpEnd = sensorDoFuncPreviewShortExp,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
#if HCROP_ENABLE
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_NTSC_HSIZE,
		.ySize = PREVIEW_NTSC_VSIZE,
	},
#else
    .crop = NULL,
#endif
	.hsize = PREVIEW_NTSC_HSIZE,
	.vsize = PREVIEW_NTSC_VSIZE,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_YUV,
 	.cfa_pattern = PREVIEW_NTSC_CFAPATTERN,
    .hratio = PREVIEW_NTSC_HSIZE,     /* 16:9 fixed hratio = 3200 , vratio = 1800 */
    .vratio = PREVIEW_NTSC_VSIZE,     /* 4:3  fixed hratio = 4000 , vratio = 3000 */
    .fps_mhz = 50100000,              /* frame rate * 1000000 */
    .sensor_pclk = SENSOR_MCLK,       /* Hz */
    .sensor_htotal = PREVIEW_NTSC_PIXELSTOTAL , /* per line of pixel HW , htotal/sensor_pclk */
    .pix_100khz = PIX_100KHZ/100,    /* pclk1x speed Datarate * lanes / bit x HSize / HTotal */
};
static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &sensorTgPreviewCCIR601Cfg, 	/* 0x30 - index 0 (601) */
    &sensorTgPreviewCCIR656Cfg      /* 0x31 - index 1 (656) */
};
#if SNAP_DUMMYVD_NUM
static sensorDoFunc_t sensorDoFuncSnapDummyTbl[] = {
	sensorDoFuncSnapDummy0,
};
#endif
static sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
        #if 0
        .psnapEarly = (sensorDoFunc_t []){
            sensorDoFuncSnapEarly0,
            sensorDoFuncSnapEarly1,
            },
        #else
        .psnapEarly = NULL,
        #endif
        .snapExpBegin = sensorDoFuncSnapExpBegin,
        .snapBshutterBegin = sensorDoFuncSnapBshutterBegin,
        .snapBshutterFollow = sensorDoFuncSnapBshutterFollow,
        .snapBshutterEnd = sensorDoFuncSnapBshutterEnd,
        .snapLongExpBegin = NULL,
        .snapLongExpFollow = NULL,
        .snapLongExpEnd = NULL,
        .snapShortExp = sensorDoFuncSnapShortExp,
        .snapExpEnd = sensorDoFuncSnapExpEnd,
    #if SNAP_DUMMYVD_NUM
        .psnapDummy = sensorDoFuncSnapDummyTbl,
    #else
        .psnapDummy = NULL,
    #endif
        .psnapReadoutField = (sensorDoFunc_t []) {
            sensorDoFuncSnapReadoutField0,
            /* sensorDoFuncSnapReadoutField1,
            sensorDoFuncSnapReadoutField2, */
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
    .pix_100khz = PIX_100KHZ/100,
    .hratio=CAPTURE_HSIZE,
    .vratio=CAPTURE_VSIZE,
};
/****TODO****frame snap mode*/
static sensorTgSnapCfg_t sensorTgSnapFrameCfg = {
	.fpdo = &(const sensorSnapDo_t) {
        #if 0
        .psnapEarly = (sensorDoFunc_t []){
            sensorDoFuncSnapEarly0,
            sensorDoFuncSnapEarly1,
            },
        #else
        .psnapEarly = NULL,
        #endif
        .snapExpBegin = sensorDoFuncSnapExpBegin,
        .snapBshutterBegin = sensorDoFuncSnapBshutterBegin,
        .snapBshutterFollow = sensorDoFuncSnapBshutterFollow,
        .snapBshutterEnd = sensorDoFuncSnapBshutterEnd,
        .snapLongExpBegin = NULL,
        .snapLongExpFollow = NULL,
        .snapLongExpEnd = NULL,
        .snapShortExp = sensorDoFuncSnapShortExp,
        .snapExpEnd = sensorDoFuncSnapExpEnd,
    #if SNAP_DUMMYVD_NUM
        .psnapDummy = sensorDoFuncSnapDummyTbl,
    #else
        .psnapDummy = NULL,
    #endif
        .psnapReadoutField = (sensorDoFunc_t []) {
            sensorDoFuncSnapReadoutField0,
            /* sensorDoFuncSnapReadoutField1,
            sensorDoFuncSnapReadoutField2, */
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
    .pix_100khz = PIX_100KHZ/100,
    .hratio=CAPTURE_HSIZE,
    .vratio=CAPTURE_VSIZE,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	 &sensorTgSnapRollCfg,  /* 0x40 index 0 */
     &sensorTgSnapFrameCfg, /* 0x41 index 1 */
};

static sensorSyncSigCfg_t sensorSyncSigPreviewCCIR601Cfg = {
	.linetotal       = PREVIEW_NTSC_LINETOTAL,
	.lineblank       = PREVIEW_NTSC_LINEBLANK,
	.frametotal      = PREVIEW_NTSC_FRAMETOTAL,
	.frameblank      = PREVIEW_NTSC_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_NTSC_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_NTSC_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_NTSC_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_NTSC_VRESHAPE_RISE,
	.hoffset         = PREVIEW_NTSC_HOFFSET,
	.voffset         = PREVIEW_NTSC_VOFFSET,
	.hsize           = PREVIEW_NTSC_HSIZE,
	.vsize           = PREVIEW_NTSC_VSIZE,
	.lastlinepix     = PREVIEW_NTSC_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_NTSC_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_NTSC_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_NTSC_HDNUM_CMDPORT,
};
static const sensorSyncSigCfg_t sensorSyncSigPreviewCCIR656Cfg = {
	.linetotal       = PREVIEW_NTSC_LINETOTAL,
	.lineblank       = PREVIEW_NTSC_LINEBLANK,
	.frametotal      = PREVIEW_NTSC_FRAMETOTAL,
	.frameblank      = PREVIEW_NTSC_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_NTSC_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_NTSC_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_NTSC_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_NTSC_VRESHAPE_RISE,
	.hoffset         = PREVIEW_NTSC_HOFFSET,
	.voffset         = PREVIEW_NTSC_VOFFSET,
	.hsize           = PREVIEW_NTSC_HSIZE,
	.vsize           = PREVIEW_NTSC_VSIZE,
	.lastlinepix     = PREVIEW_NTSC_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_NTSC_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_NTSC_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_NTSC_HDNUM_CMDPORT,
};
static sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
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
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
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
#if STREAMOF_ENABLE
	.conf_has_abort = STREAMOF_ENABLE,
#endif
};
static const sensorTgDev_t sensorTgDev = {
	.tgCfg = &sensorTgCfg,
	.tgSvc = &(const sensorTgSvc_t) {
		.resload = sensorCfgInit,
		.resstop = sensorCfgStop,
		.init = sensorDoFuncInit,
		.stop = sensorDoFuncStop,                  /****TODO***/
		.pwrsave = NULL,                           /****TODO***/
		.expwrite = sensor_cmdport_exp_write,
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
        .crop = NULL,                              /****TODO***/
#if STREAMOF_ENABLE
		.streamoff = sensorDoFuncStreamOff,
		.streamon  = sensorDoFuncStreamOn,
#endif
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
    .defSyncSigCfg  = &sensorSyncSigPreviewCCIR601Cfg,
	.verStr = MODEL_VERSION,
};
void
sensorTg_tvp5150_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
#if 1
    sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
#else
	sensorTgDevInstall(SENSOR_DEV_ID_TV_IN,&sensorTgDev);
#endif
}
static UINT8
sensorRead(
   UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[2];
    cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr&0xFF;
	sensorSerialBlockRead(cmdportid,1,regread,0);
	return regread[1];
}

#define VIDEO_FORMAT_CHANGE (2)


static UINT32
tvdecVideoTypeChange(UINT8 rdata)
{
	 sensorSyncSigCfg_t *pssg;
	 sensorTgPreviewCfg_t *ppcfg;
	 sensorTgSnapCfg_t *pscfg;

    pssg = &sensorSyncSigPreviewCCIR601Cfg;
    ppcfg = &sensorTgPreviewCCIR601Cfg;
    pscfg = &sensorTgSnapRollCfg;

    switch ((rdata >> 1) & 0x07) {
    case 0x0:
    case 0x4:
        /* 60Hz => NTSC */
        printf("NTSC video source.\n");
        pssg->hsize  = PREVIEW_NTSC_HSIZE;
        pssg->vsize  = PREVIEW_NTSC_VSIZE;
        ppcfg->hsize = PREVIEW_NTSC_HSIZE;
        ppcfg->vsize = PREVIEW_NTSC_VSIZE;
        pscfg->hsize = PREVIEW_NTSC_HSIZE;
        pscfg->vsize = PREVIEW_NTSC_VSIZE;
        break;
    case 0x1:
    case 0x2:
    case 0x3:
        pssg->hsize  = PREVIEW_PAL_HSIZE;
        pssg->vsize  = PREVIEW_PAL_VSIZE;
        ppcfg->hsize = PREVIEW_PAL_HSIZE;
        ppcfg->vsize = PREVIEW_PAL_VSIZE;
        pscfg->hsize = PREVIEW_PAL_HSIZE;
        pscfg->vsize = PREVIEW_PAL_VSIZE;
        /* 50Hz => PAL */
        printf("PAL video source.\n");
        break;
    case 0x5:
        printf("SECAM video source.\n");
        break;
    default:
        printf("Unknown video source.\n");
        break;
    }

    sensorSyncSigInit(&sensorSyncSigPreviewCCIR601Cfg);
}


static UINT32
tvdecVideoTypeDetect(void)
{
	UINT8 rdata[2];
    UINT8 status;
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialFlush(cmdportid);

    /* sensorEventVvalidWait(1); */

	rdata[0] = 0x88; /* the status register of TVP5150 */
	sensorSerialBlockRead(cmdportid, 1, rdata, 0);
    status = rdata[1] ;
    rdata[0] = 0x8C; /* the status register of TVP5150 for the type of video input source */
    sensorSerialBlockRead(cmdportid, 1, rdata, 0);
    if (!((status & 0x0E) && (rdata[1] & 0x01))) {
        printf("No video source detected.\n");
        g_curInput = 0xFF ;
        g_Reset = 1 ;
        return FAIL ;
    } else {
        if ( g_curInput == rdata[1]) {
            return SUCCESS ;
        }
        tvdecVideoTypeChange(rdata[1]);

        if (status & (1<<5)) {
            printf("Field rate 50Hz\n");
        } else {
            printf("Field rate 60Hz\n");
        }
        g_curInput = rdata[1] ;
        g_Reset = 1 ;
        return VIDEO_FORMAT_CHANGE ;
    }

}

#if 0
static void
sensor_Reset(void)
{
    UINT32 cmdportid;
    UINT8 wdata[2];
    UINT8 rdata[2];
    UINT8 status;
    static UINT8 regData[][2] ={
        {0x00,0x00},    /* bit1 0: AIP1A(J2), 1:AIP1B(J1)   */
        {0x03,0x0d},    /* TVPOE, HsyncVsync OE, CLK OE     */
        {0x04,0xc0},
        {0x0d,0x40},
    };



    cmdportid = sensorTgCfg.cmdportid_xtg;
    regData[0][1] = g_curChannel ;
    sensorSerialFlush(cmdportid);

    wdata[0] = 0x05;
    wdata[1] = 0x01; /* #define TVP5150_SoftReset_Reg 0x05  == 0x1 */
    sensorSerialBulkWrite(cmdportid,1,wdata,0);

    sensorSerialBulkWrite(cmdportid,(sizeof(regData)/sizeof(regData[0])), regData[0],0);

    rdata[0] = 0x88; /* the status register of TVP5150 */
    sensorSerialBlockRead(cmdportid, 1, rdata, 0);
    status = rdata[1] ;
    rdata[0] = 0x8C; /* the status register of TVP5150 for the type of video input source */
    sensorSerialBlockRead(cmdportid, 1, rdata, 0);
    if (!((status & 0x0E) && (rdata[1] & 0x01))) {
        printf("No video source detected.\n");
        g_curInput = 0xFF ;
    } else {

        tvdecVideoTypeChange(rdata[1]);

        if (status & (1<<5)) {
            printf("Field rate 50Hz\n");
        } else {
            printf("Field rate 60Hz\n");
        }
        g_curInput = rdata[1] ;
    }

}
#endif


UINT32
tvdecVideoChannelChange(
   UINT8 channel
)
{
    UINT32 cmdportid;
    UINT8 setChannel ;
    UINT8 wdata[2];
    UINT32 ret ;

    cmdportid = sensorTgCfg.cmdportid_xtg;
    wdata[0] = 0x00;

    sensorSerialFlush(cmdportid);
    setChannel = ( g_curChannel & 0xF5 ) | ( (channel & 0x1) << 1 ) ;
    if ( setChannel !=  g_curChannel ) {
        wdata[1] = setChannel ; /* (g_curChannel & 0xF5 ) | ( (channel & 0x1) << 1 ) ; */
        sensorSerialBulkWrite(cmdportid,1,wdata,0);
        g_curInput = 0xFF ;
        g_curChannel = wdata[1] ;
        g_Reset = 1 ;
    }

    ret = tvdecVideoTypeDetect();

    if ( g_Reset ) {
        /* sensor_Reset(); */
        g_Reset = 0 ;
    }

    return  ret;

}

static void
sensor_Init(void)
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
	SENSOR_PROF_LOG_PRINT("TVP5150 model-ID 0x80=0x%x",  sensorRead(0x80));
    SENSOR_PROF_LOG_PRINT("TVP5150 model-ID 0x81=0x%x",  sensorRead(0x81));
    sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code)/sizeof(cmd_init_code[0])), cmd_init_code[0],0);

    SENSOR_PROF_LOG_PRINT("TVP5150 Horizontal Sync Start = 0x%x", sensorRead(0x16));
    SENSOR_PROF_LOG_PRINT("TVP5150 Vertical Blanking Start = 0x%x\n", sensorRead(0x18));
    SENSOR_PROF_LOG_PRINT("TVP5150 Vertical Blanking Stop = 0x%x\n", sensorRead(0x19));
    SENSOR_PROF_LOG_PRINT("TVP5150 Vertical Line Count =%d",  (sensorRead(0x84) << 8 ) | sensorRead(0x85));


}
static void
sensorReportsize()
{
    PROF_LOG_LINE_COUNT("----- sensorReportsize -----");
	PROF_LOG_LINE_COUNT("mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	PROF_LOG_LINE_COUNT("front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	PROF_LOG_LINE_COUNT("imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	PROF_LOG_LINE_COUNT("rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	WRITE16(0xb0009064,1);
	PROF_LOG_LINE_COUNT("line count            = (%d)",READ16(0xb0009064));
    PROF_LOG_LINE_COUNT("----------------------------");
#if 0
    PROF_LOG_PRINT(3," front test size (%d %d)",READ16(0xb00090cc),READ16(0xb00090ce));
    PROF_LOG_PRINT(3," front 2 cdsp output size     = (%d,%d)",READ16(0xb00020B4),READ16(0xb00020B6));
    PROF_LOG_PRINT(3," desensor h size comein(%d) after(%d) ",READ16(0xb0002274),READ16(0xb0002278));
    PROF_LOG_PRINT(3," crophstart = %d",READ16(0xb0002252));
    PROF_LOG_PRINT(3," crophend   = %d",READ16(0xb0002254));
    PROF_LOG_PRINT(3," cropvstart = %d",READ16(0xb0002256));
    PROF_LOG_PRINT(3," cropvend   = %d",READ16(0xb0002258));
    PROF_LOG_PRINT(3," imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
    PROF_LOG_PRINT(3," rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
    PROF_LOG_PRINT(3," rgbhscale2 output size= (%d,%d)",READ16(0xb00020A8),READ16(0xb00020AA));
    PROF_LOG_PRINT(3," rawmux output size    = (%d,%d)",READ16(0xb0002090),READ16(0xb0002092));
    PROF_LOG_PRINT(3,"sig gen (ht hb vt vb) = (%d,%d,%d,%d)",READ16(0xb000909E),READ16(0xb000908C),READ16(0xb000908E),READ16(0xb00090A2));
    PROF_LOG_PRINT(3," yuv mux output size   = (%d,%d)",READ16(0xb00020B0),READ16(0xb00020B2));
    PROF_LOG_PRINT(3," yuvhshalf input size  = (%d,%d)",READ16(0xb0002094),READ16(0xb0002096));
    PROF_LOG_PRINT(3," yuvvs output size     = (%d,%d)",READ16(0xb0002098),READ16(0xb000209A));
    PROF_LOG_PRINT(3," ahd output size       = (%d,%d)",READ16(0xb000209C),READ16(0xb000209E));
    WRITE16(0xb0009064,0x01);
    PROF_LOG_PRINT(3," v_count     = (%d)",READ16(0xb0009064));
    PROF_LOG_PRINT(3," Report size complete");
#endif
}
static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
    UINT32 ret = 0 ;
    SENSOR_PROF_LOG_PRINT("________Sensor Driver MODEL_VERSION <<%s>>", sensorTgDev.verStr);
	SENSOR_PROF_LOG_PRINT("________Sensor Driver Build Time    <<%s>>", BUILD_TIME);

    tmrUsWait(200000);

    #if SPCA6330 /* SPCA6330 */
	/* Master clock and Pixel clock configuration here */
    halFrontInputGate(0x0000);
	halFrontClkCfg(MCLKMODE,1);
    ret = sensorTgPllClkExtSet(24000000);
	SENSOR_PROF_LOG_PRINT("TG PLL Clock = %d",ret);
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
    tmrUsWait(2000);
	/* Internal clock phase initialization here */
    halFrontSyncSigCfgSet(FRONT_SYNC_HDVD_IN_POLAR,1,1);
    halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
    halFrontClkCfg(MCLKMODE, PCLKMODE);
    halFrontClkSet(TGPLLEN,TGPLLCLKSET, MCLKDIV, PCLKDIV); /* Set master clock & pixel clock */
    sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);
    #endif

#if 0
	/* Sensor Power On Sequence */
	/* HW Reset PDN = 1 , RESETB = 0 */
	sp5kGpioCfgSet(SENOR_GPIO_GRP,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	/* TVP5150 PDN low active !! */
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,0);            /* PWD Low   */
	/* TVP5150 RESET low active !! */
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0);            /* RESET Low */
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High   */
	tmrUsWait(30000); /* 20 ms */
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High  */
	tmrUsWait(200); /* 200 us */
#endif
    /*I2C Init code Here */
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensor_Init();
    tvp5150RegDump();
    tvdecVideoTypeDetect();
    sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	sensorResumeInstall(sensorResumeExecuteLoad);
	sensorSuspendInstall(sensorSuspendExecuteLoad);
#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif
#if STREAMOF_ENABLE
	sensorStsCfg(SENSOR_SYS_CFG_FLOW_USE_ABORT, STREAMOF_ENABLE);
#endif
    SENSOR_PROF_LOG_PRINT("sensorDoFuncInit done");
#if (_SENSOR_DBG_)
    /*debug by TGGPIO2*/
    WRITE8(0xb0009030, (READ8(0xb0009030)|(0x02)));
    DBG_GPIO(0);
#endif
}
static void
sensorDoFuncStop()
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	UINT8 wdata[2];
    SENSOR_PROF_LOG_PRINT("sensorDoFuncStop");
	wdata[0] = 0x03;
	wdata[1] = 0x00; /* Set the YUV data & clock to high impedance */
	sensorSerialAsyncWrite(cmdportid, 1, &wdata[0]);
}
static void
sensorDoFuncPreviewCCIR601Begin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_LINE_COUNT("sensorDoFuncPreviewCCIR601Begin");
#if 0
    if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
#else
    sensorEventVvalidWait(1);
#endif
    g_IsSnap2Pv=0;
    fpvsusp=1;

#if 0
    sensorSerialFlush(cmdportid);
    sensorSerialBulkWrite(cmdportid,(sizeof(cmd_CCIR601_code)/sizeof(cmd_CCIR601_code[0])), cmd_CCIR601_code[0],0);
    tvdecVideoTypeDetect();
#endif

#if 0
    if(is_aborted){
        SENSOR_PROF_LOG_PRINT("sensorSyncSigInit");
        sensorSyncSigInit(&sensorSyncSigPreviewCCIR601Cfg);
    } else {
        SENSOR_PROF_LOG_PRINT("sensorSyncSigSet");
        sensorSyncSigSet(&sensorSyncSigPreviewCCIR601Cfg);
    }
#else
    sensorSyncSigInit(&sensorSyncSigPreviewCCIR601Cfg);
#endif

#if 0
    SP5K_SET_LVDS_HCROP(PREVIEW_720P_HCROP_OFFSET);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
#endif


    PROF_LOG_LINE_COUNT("CCIR601 Mode done");
}

static void
sensorDoFuncPreviewCCIR656Begin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_LINE_COUNT("sensorDoFuncPreviewCCIR656Begin");
#if 0
    if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
#else
    sensorEventVvalidWait(1);
#endif
    g_IsSnap2Pv=0;
    fpvsusp=1;
    sensorSerialFlush(cmdportid);
    sensorSerialBulkWrite(cmdportid,(sizeof(cmd_CCIR656_code)/sizeof(cmd_CCIR656_code[0])), cmd_CCIR656_code[0],0);
#if 0
    if(is_aborted){
        SENSOR_PROF_LOG_PRINT("sensorSyncSigInit");
        sensorSyncSigInit(&sensorSyncSigPreviewCCIR656Cfg);
    }else{
        SENSOR_PROF_LOG_PRINT("sensorSyncSigSet");
        sensorSyncSigSet(&sensorSyncSigPreviewCCIR656Cfg);
    }
#else
    sensorSyncSigInit(&sensorSyncSigPreviewCCIR656Cfg);
#endif

#if 0
    SP5K_SET_LVDS_HCROP(PREVIEW_720P_HCROP_OFFSET);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
#endif

    PROF_LOG_LINE_COUNT("CCIR656 Mode done");
}

static void
sensorDoFuncPreviewShortExp()
{
}

static void
sensorDoFuncPreviewExpEnd()
{
}

#if 0
static void
sensorDoFuncSnapEarly0()
{
}
static void
sensorDoFuncSnapEarly1()
{
}
#endif
static void
sensorDoFuncSnapBshutterBegin()
{
}

static void
sensorDoFuncSnapBshutterFollow()
{
}

static void
sensorDoFuncSnapBshutterEnd()
{
}

/*static void
FrontReset()
{
    WRITE8(0xb0009010,1);
    WRITE8(0xb0009010,0);
}*/

static void
sensorDoFuncSnapExpBegin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
#if (_SENSOR_DBG_)
    DBG_GPIO(1);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(0);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(1);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(0);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(1);
#endif
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
    PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(0);
    if(!g_IsSnap2Pv){
        g_IsSnap2Pv=1;
        PROF_LOG_LINE_COUNT("g_IsSnap2Pv");
        sensorSerialFlush(cmdportid);
        PROF_LOG_LINE_COUNT("SerialFlush(cmdportid)");
        #if 0
        sensorEventVvalidWait(1);  /* for better performance */
        #endif

#if 0
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_off)/sizeof(cmd_stream_off[0])), cmd_stream_off[0],0);
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_still_code)/sizeof(cmd_still_code[0])), cmd_still_code[0],0);
#endif

        PROF_LOG_LINE_COUNT("cmd_still_code");
        #if 0
        PROF_LOG_LINE_COUNT("sensorSyncSigInit");
        sensorSyncSigInit(&sensorSyncSigSnapCfg);
        #else
        PROF_LOG_LINE_COUNT("sensorSyncSigSet");
        sensorSyncSigSet(&sensorSyncSigSnapCfg);
        #endif
#if 0
        SP5K_SET_LVDS_HCROP(PREVIEW_720P_HCROP_OFFSET);
        sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0])), cmd_stream_on[0],0);
#endif

    }
    PROF_LOG_LINE_COUNT("e");
#if (_SENSOR_DBG_)
    DBG_GPIO(0);
#endif
}

static void
sensorDoFuncSnapShortExp()
{
}

#if (_SENSOR_DBG_)
static void
DEBUGIO(UINT8 level)
{
    DBG_GPIO(level);
}
#endif

static void
sensorDoFuncSnapExpEnd()
{
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(1);
    PROF_LOG_LINE_COUNT("e");
}

#if SNAP_DUMMYVD_NUM
static void
sensorDoFuncSnapDummy0()
{
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
    PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(2);
    PROF_LOG_LINE_COUNT("e");
}
#endif

static void
sensorDoFuncSnapField0VvldEndCB()
{
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    PROF_LOG_LINE_COUNT("front output size = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
    TIME_LOG(3);
    PROF_LOG_LINE_COUNT("e");
}
static void
sensorDoFuncSnapReadoutField0()
{
    sensor_frame_cb_reg(2/*SENSOR_IRQ_SVC_VVALID_END*/,sensorDoFuncSnapField0VvldEndCB,0);
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(4);
}

static void
sensorDoFuncSnapFieldEnd()
{
#if (_SENSOR_DBG_)
    DEBUGIO(1);
#endif
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(5);
#if (_SENSOR_DBG_)
    DEBUGIO(0);
#endif
}

static void
sensorDoFuncSnapNext(UINT32 next_mode)
{
#if (_SENSOR_DBG_)
    DEBUGIO(1);
#endif
    PROF_LOG_LINE_COUNT("s");
    if((next_mode&0xf0)==SENSOR_MODE_SNAP){
        PROF_LOG_LINE_COUNT("@@@@ get sensor next mode is snap");
    }else{
        PROF_LOG_LINE_COUNT("@@@@ get sensor next mode is preview");
    }
    PROF_LOG_LINE_COUNT("e");
#if (_SENSOR_DBG_)
    DEBUGIO(0);
#endif
}

static void
sensorDoFuncSnapReadoutEnd()
{
    TIME_LOG(6);
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    PROF_LOG_LINE_COUNT("SnapExpS--->ExpEnd:    %d.%d ms",TIME_DIFF(1,0)/1000,TIME_DIFF(1,0)%1000);
    PROF_LOG_LINE_COUNT("ExpEnd----->Dummy0:    %d.%d ms",TIME_DIFF(2,1)/1000,TIME_DIFF(2,1)%1000);
    PROF_LOG_LINE_COUNT("Dummy0----->Field0:    %d.%d ms",TIME_DIFF(4,2)/1000,TIME_DIFF(4,2)%1000);
    PROF_LOG_LINE_COUNT("Field0----->FieldEnd:  %d.%d ms",TIME_DIFF(5,4)/1000,TIME_DIFF(5,4)%1000);
	PROF_LOG_LINE_COUNT("FieEnd----->ReadoutEnd:%d.%d ms",TIME_DIFF(6,5)/1000,TIME_DIFF(6,5)%1000);
    #if SENSOR_PROF_LOG_EN
        sensorReportsize();
    #endif
}
#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
	static const UINT8 exptbl_stream[]
	COMMON_ALIGNED = {
	3, /*n_tbl*/
	1, /*nreg*/
	/*Tbl 0x30 ++*/
	32, /*n_entry*/ 0, /*tblidx_min*/	0x30, /*sensorMode of this tbl*/80, /*expidx0_tvscale*/16, /*exp_tvscale*/1,/*n_burst_tbl*/
	0x01,0x02,/*reg tmplate*/
	32,/*n_entry_burst*/1,/*nbyte_entry*/0,/*burstidx_min*/1,/*nreg_involved*/
	0,/*reg# in template*/0,/*lsb right shift*/8,/*lsb left shift*/8,/*nbit_mask*/
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,/*end contains*/
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,/*end contains*/
	1,1,/*vtable,n_vtbl,def_vtype(0:no used)*/32, /*for n_vtbl*/1,0,/*Min Table index in evtbl*/1,
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,/*vtbl contains*/
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,/*vtbl contains*/
	0,0,64,/*error table*/
	/*Tbl 0x30 --*/
	/*Tbl 0x31 ++*/
	32, /*n_entry*/ 0, /*tblidx_min*/	0x31, /*sensorMode of this tbl*/80, /*expidx0_tvscale*/16, /*exp_tvscale*/1,/*n_burst_tbl*/
	0x01,0x02,/*reg tmplate*/
	32,/*n_entry_burst*/1,/*nbyte_entry*/0,/*burstidx_min*/1,/*nreg_involved*/
	0,/*reg# in template*/0,/*lsb right shift*/8,/*lsb left shift*/8,/*nbit_mask*/
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,/*end contains*/
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,/*end contains*/
	1,1,/*vtable,n_vtbl,def_vtype(0:no used)*/32, /*for n_vtbl*/1,0,/*Min Table index in evtbl*/1,
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,/*vtbl contains*/
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,/*vtbl contains*/
	0,0,64,/*error table*/
	/*Tbl 0x31 --*/
	/*Tbl 0x40 ++*/
	32, /*n_entry*/ 0, /*tblidx_min*/	0x40, /*sensorMode of this tbl*/80, /*expidx0_tvscale*/16, /*exp_tvscale*/1,/*n_burst_tbl*/
	0x01,0x02,/*reg tmplate*/
	32,/*n_entry_burst*/1,/*nbyte_entry*/0,/*burstidx_min*/1,/*nreg_involved*/
	0,/*reg# in template*/0,/*lsb right shift*/8,/*lsb left shift*/8,/*nbit_mask*/
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,/*end contains*/
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,/*end contains*/
	1,1,/*vtable,n_vtbl,def_vtype(0:no used)*/32, /*for n_vtbl*/1,0,/*Min Table index in evtbl*/1,
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,/*vtbl contains*/
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,/*vtbl contains*/
	0,0,64,/*error table*/
	/*Tbl 0x40 --*/
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
	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);
	/* if support burst
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_BURST; */
	/* if support seq
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_SEQ; */
    sensor_bshut_init();  /*bshut must init before exptable*/

#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
    printf("@@@@@@@@@@@@@@@@create exp tbl...");
	sensor_exptbl_create(exptbl_stream, sizeof(exptbl_stream));
#else
	sensor_exptbl_init();
#endif
}

static void
sensorCfgStop()
{
#if 1
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
    /*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
#endif
}

static void
sensor_cmdport_exp_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{
    /*NOP*/
}

static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
	/*NOP*/
}
#if 0
static void
sensorDoFuncStreamOff(void)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
#if 0
    #if SENSOR_PROF_LOG_EN    /*debug*/
        profLogInit(PROF_ID, 0x10000);
        profLogModeSet(PROF_ID, 0);
        /* profLogInit(2, 0x8000);
        profLogModeSet(2, 0); */
    #endif
#endif
    PROF_LOG_LINE_COUNT("sensorModeGet= %x", sensorModeGet());
	PROF_LOG_LINE_COUNT("s");
    sensorSerialFlush(cmdportid);
	sensorSerialAsyncWrite(cmdportid,(sizeof(cmd_stream_off)/sizeof(cmd_stream_off[0])), cmd_stream_off[0]);
    sensorSerialFlush(cmdportid);
 	PROF_LOG_LINE_COUNT("e");
}

static void
sensorDoFuncStreamOn(void)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_LINE_COUNT("s");
    sensorSerialFlush(cmdportid);
	sensorSerialAsyncWrite(cmdportid,(sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0])), cmd_stream_on[0]);
    sensorSerialFlush(cmdportid);
	PROF_LOG_LINE_COUNT("e");
}
#endif

#if STREAMOF_ENABLE
static void
sensorDoFuncStreamOff(UINT32 abort)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==0){
        PROF_LOG_LINE_COUNT("sensorModeGet= %x", sensorModeGet());
		PROF_LOG_LINE_COUNT("s");
        sensorSerialFlush(cmdportid);
		sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_off)/sizeof(cmd_stream_off[0])), cmd_stream_off[0],0);
 		PROF_LOG_LINE_COUNT("e");
		is_aborted = 1;
	}
}

static void
sensorDoFuncStreamOn(UINT32 abort)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==1 ){
	    PROF_LOG_LINE_COUNT("s");
        sensorSerialFlush(cmdportid);
		sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0])), cmd_stream_on[0],0);
		PROF_LOG_LINE_COUNT("e");
		is_aborted = 0;
	}
}
#endif

static UINT32 sensorResumeExecuteLoad()
{
    #if SUSPEND_ENABLE
    	if(sensorSuspended ==1){
            /* sensorResumeExecute() */
	    	PROF_LOG_LINE_COUNT("senResu s");
            sensorDoFuncStreamOn();
            tmrUsWait(5000);/*5ms*/
 	    	PROF_LOG_LINE_COUNT("senResu e");
            sensorSuspended = 0;
	    }
	#endif
	return SUCCESS;
}

static UINT32 sensorSuspendExecuteLoad()
{
    #if SUSPEND_ENABLE
        if(fpvsusp == 0) return SUCCESS;
		if(sensorSuspended ==0){
	    	/* sensorSuspendExecute() */
	    	PROF_LOG_LINE_COUNT("senSusp s");
            sensorDoFuncStreamOff();
            tmrUsWait(5000);/*5ms*/
	    	PROF_LOG_LINE_COUNT("senSusp e");
	    	sensorSuspended = 1;
	    }
	#endif
	return SUCCESS;
}

#if SP5K_SENSOR_TVP5150 /* SP5K_TVINDEC_TVP5150 */
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	/*NOP*/
}
#endif

void tvp5150RegDump()
{
    UINT8 i;
    for(i=0; i<0x1E; i++){
        printf("reg(%2x):%2x \n",i,sensorRead(i));
    }

    /* appStillSpeedyCapture command speedy */

}




#if TVP5150_CMD

#include "api\cmd.h"
UINT32 appTimerIsrReg(void *pfun,UINT8 numTick /* refer tmrIsrTickInterval for time interval per tick */);
UINT32 appTimerIsrTickIntervalGet(void);
void appModeSet(UINT32 mode);
UINT32 sp5kModeGet(UINT32 *pmode);
UINT32 sp5kModeCfgSet(UINT8 cfg, ...);

static UINT32 timrHdlr = 0xFFFFFFFF;
static UINT8 channel = 0;

void
_tvp5150_cmd_channel_background(
	void
)
{
    UINT32 result ;
    UINT32 mode;

    channel = channel & 1 ;
    printf("change channel %d\n",channel);
    result = tvdecVideoChannelChange(channel);

    if ( result == FAIL ) {
        sp5kModeGet(&mode);
        printf("mode 0x%x ----> in FAIL\n",mode);
        if ( mode != 0x88 ) {
            sp5kModeSet(0x88 /* SP5K_MODE_STANDBY */);
            sp5kModeWait(0x88 /* SP5K_MODE_STANDBY */);
        }
    }

    if ( result == VIDEO_FORMAT_CHANGE ) {

        sp5kModeGet(&mode);
        printf("mode 0x%x ----> in VIDEO_FORMAT_CHANGE\n",mode);
        if ( mode != 0x88 ) {
            printf("change to SP5K_MODE_STANDBY sp5kModeSet\n");
            sp5kModeSet(0x88 /* SP5K_MODE_STANDBY */);
            printf("change to SP5K_MODE_STANDBY sp5kModeWait\n");
            sp5kModeWait(0x88 /* SP5K_MODE_STANDBY */);
        }
        printf("change to SP5K_MODE_STILL_PREVIEW sp5kModeSet\n");
        sp5kModeSet(0x11 /* SP5K_MODE_STILL_PREVIEW */);
        printf("change to SP5K_MODE_STILL_PREVIEW sp5kModeWait\n");
        sp5kModeWait(0x11 /* SP5K_MODE_STILL_PREVIEW */);
    }


    if ( result == SUCCESS ) {
        channel = channel + 1 ;
    }
}

static void _tvp5150_cmd_channel(
   int argc,
   char *argv[]
)
{

    if (argc < 1) {
		return;
	}

    channel = strtoul(argv[0], NULL, 0) ;
    printf("TVP5150 change channel to %d\n",channel);

    if (timrHdlr == 0xFFFFFFFF) {
        timrHdlr = appTimerIsrReg(_tvp5150_cmd_channel_background,( 1000 /appTimerIsrTickIntervalGet()));
        if (timrHdlr == 0xFFFFFFFF)
        {
            printf("[ERR] background timer register failed!!!\n\n");
        }
    }
}

static void _tvp5150_cmd_detect(
   int argc,
   char *argv[]
)
{
    tvdecVideoTypeDetect();
}


static cmd_t subtvp5150cmds[] = {
	{"detect" , _tvp5150_cmd_detect,  "tvp5150 input detection."},
    {"channel", _tvp5150_cmd_channel, "tvp5150 input channel change."},
	{NULL,  NULL, NULL, NULL}
};

static void _tvp5150_cmd_not_found(
   int argc,
   char *argv[]
)
{
	printf("available commands:\n");

	cmd_t *pcmd;
	const char *spaces = " : ";

	for (pcmd = subtvp5150cmds; pcmd->cmd; ++pcmd) {
		printf(" %s %s %s\n", pcmd->cmd, spaces , pcmd->phelp);
	}
	printf("\n");
}


static void _tvp5150_cmd_main(
   int argc,
   char *argv[]
)
{
	static cmd_t *psubcmds = NULL;
    if (!psubcmds) {
		psubcmds = cmdSubBatchRegister(0, &subtvp5150cmds[0]);
	}

    cmdSubProcess(argc, argv, psubcmds, _tvp5150_cmd_not_found);
}


static cmd_t tvp5150cmds[] = {
	{ "tvp5150" , _tvp5150_cmd_main , "Custom TVP5150 test command" , NULL, },
	{ NULL      , NULL              , NULL                          , NULL, }, /* null terminator */
};


void
cmdTVP5150Init(
   void
)
{
	cmdBatchRegister(tvp5150cmds);
	SENSOR_PROF_LOG_PRINT("%s[%s] TVP5150 Test CMD init done.\n",__FILE__,__FUNCTION__);
}
#endif


