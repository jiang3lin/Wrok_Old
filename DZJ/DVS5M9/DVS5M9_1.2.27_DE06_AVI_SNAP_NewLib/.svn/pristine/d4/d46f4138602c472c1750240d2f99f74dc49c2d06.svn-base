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
 *  Author: Vincent Tsao                                                 *
 *                                                                        *
 *  Ver. 0.01  Initial version.
 *
 *  Ver. 0.02  Add the XGA mode(1832*1374, 30fps, pixel clock 120MHz), 
 *                Modify the VGA mode(912*684, 30fps, pixel clock 60MHz),
 *                Modify the capture mode (3664*2748, 13fps, pixel clock 120MHz, must be used under 
 *                DRAM clock = 297MHz.) 
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_mt9j003-0.00.00"

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
#include "api/sp5k_sensor_api.h"
#include <stdio.h>
#include "middleware/proflog.h"
#include "../flow/hal_front.h"
#include "../flow/sensor_model_snap.h"

#define BURST_CAP_EN     	1

#if BURST_CAP_EN
#define S2S_TIME_LOG 		1
#define DDR_CLOCK_297MHz    1
#else
#define S2S_TIME_LOG 		0
#define DDR_CLOCK_297MHz    0
#endif

#define I2C_Batch_Write  	0
#define SHUTTER_LAG_IMPROVE 0

#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)
#define EVTBL_TEST_CODE   1

void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);

/* Sensor image size definition here */
#define NO             0
#define BY_PIXEL       1
#define BY_HD          0
#define NOUSE          0
#define DISABLE        0xFFFF

/*#define SENSOR_FREX     BIT6*/       /* for A900 */
#define SENSOR_FREX     (1<<15)           /* for EVB */
#define SENSOR_PWRDN    (1<<14)
#define SENSOR_RESET    (1<<12)
#define SENSOR_TEST     (1<<3)

#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define SENOR_GPIO_GRP SP5K_GPIO_GRP_LMI


/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HSIZE   912 /* preview horiz. size*/
#define PREVIEW_VGA_VSIZE   684  /* preview vertical size*/

#define PREVIEW_XGA_HSIZE  1832 /* preview horiz. size*/
#define PREVIEW_XGA_VSIZE  1374   /* preview vertical size*/

#define PREVIEW_720P_HSIZE  1280 /* preview horiz. size*/
#define PREVIEW_720P_VSIZE   720   /* preview vertical size*/

#define CAPTURE_HSIZE  3664/*3648/*3664 	/* capture horiz. size*/
#define CAPTURE_VSIZE  2744/*2736/*2748 	/* capture vertical size*/

/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL  (912*2)/* preview Hd length*/
#define PREVIEW_VGA_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  686 	/* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  686	/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     600   	/* preview Hd valid pixel data offset position*/
#define PREVIEW_VGA_VOFFSET     (1)		/* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL        1400 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE        1500	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  BY_PIXEL	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL        900 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE         1000 /*7*/	/* preview Vd reshap rise*/

#define PREVIEW_XGA_LINETOTAL   (1832*2) /* preview Hd length*/
#define PREVIEW_XGA_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_XGA_FRAMETOTAL  1376    /* preview Vd length*/
#define PREVIEW_XGA_FRAMEBLANK  1376	/* preview Vd sync length*/
#define PREVIEW_XGA_HOFFSET     600     /* preview Hd valid pixel data offset position*/
#define PREVIEW_XGA_VOFFSET     (1)    /* preview Vd valid line data offset position*/
/* Preview resh_XGA_pe sync signal definition here */
#define PREVIEW_XGA_HRESHAPE_FALL         1100 	/* preview Hd reshap fall*/
#define PREVIEW_XGA_HRESHAPE_RISE          1200	/* preview Hd reshap rise*/
#define PREVIEW_XGA_VRESHAPE_COUNTER_SEL  BY_PIXEL	/* preview Vd reshap control*/
#define PREVIEW_XGA_VRESHAPE_FALL          1100 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_XGA_VRESHAPE_RISE          1200 /*7*/	/* preview Vd reshap rise*/

#define PREVIEW_720P_LINETOTAL   (1280*2) /* preview Hd length*/
#define PREVIEW_720P_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_720P_FRAMETOTAL  792    /* preview Vd length*/
#define PREVIEW_720P_FRAMEBLANK  792	/* preview Vd sync length*/
#define PREVIEW_720P_HOFFSET     600     /* preview Hd valid pixel data offset position*/
#define PREVIEW_720P_VOFFSET     (1)    /* preview Vd valid line data offset position*/
/* Preview resh_720P_pe sync signal definition here */
#define PREVIEW_720P_HRESHAPE_FALL         100 	/* preview Hd reshap fall*/
#define PREVIEW_720P_HRESHAPE_RISE         200	/* preview Hd reshap rise*/
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL  BY_PIXEL	/* preview Vd reshap control*/
#define PREVIEW_720P_VRESHAPE_FALL         300 /*4*/	/* preview Vd reshap fall*/
#define PREVIEW_720P_VRESHAPE_RISE         400 /*7*/	/* preview Vd reshap rise*/
                                
/* Capture default definition here */
#define CAPTURE_LINETOTAL   (3664*2)	/* capture Hd length*/
#define CAPTURE_LINEBLANK   6		/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL   2750	/* capture Vd length*/
#define CAPTURE_FRAMEBLANK   2750		/* mipi use > vsize is more safe*/

#define CAPTURE_HOFFSET     600    /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET     1    /* capture Vd valid line data offset position*/
/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL          60	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE          70	/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_PIXEL /* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL		  1500/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_VRESHAPE_RISE		  1600/* capture Vd reshap rise , count by pixel*/ 

/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV     1        /* mclk drivng, '1' : 8mA, '0' : 4mA */
#define MCLKMODE	1	     /*mclk mode,'0'from I/O Pad,'1'from internal*/
#define PCLKMODE	2	     /*pclk mode,'0'from I/O Pad,'1'from internal*/
#define TGPLLEN   	1	     /*enable TGPLL,'0'disable,'1'enable*/
#define TGPLLCLKSET	8	     /*TGPLL clk 0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,
					5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/
#define MCLKDIV		4	     /*MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV		2	     /*pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE	0     	     /*master clk phase*/
#define PIXELPHASE	3	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/

#define	PCLKNXSEL_INT	0x00
#define	PCLKNXSEL_LVDS	0x01
#define	PCLKNXSEL_EXT	0x02

#define	PCLK1XSEL_NX	0x00
#define	PCLK1XSEL_NX_2	0x04
#define	PCLK1XSEL_INT	0x08

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_XGA_CFAPATTERN	BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define   PREVIEW_VGA_CFAPATTERN  BAYER_BGGR
#define	SNAP_CFAPATTERN		BAYER_BGGR        /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	1	            /*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		        0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		2	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */

#if (SHUTTER_LAG_IMPROVE)
#define	SKIP_SNAP_READOUTVD_EN   0x10 /*0*/		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#else
#define	SKIP_SNAP_READOUTVD_EN   0 /*0x10*/ /*0*/		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#endif

#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/

#define	CAPTURE_HDNUM_FRAME_START  110	/*the front interrupt line number of the snap new frame */
#define	CAPTURE_HDNUM_FRAME_END	 (CAPTURE_VOFFSET+CAPTURE_VSIZE+1) /*2000*/    /*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	 (CAPTURE_VOFFSET+CAPTURE_VSIZE+1)/*2000*/	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_HDNUM_FRAME_START    110/*110*/		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_FRAME_END	(PREVIEW_VGA_VOFFSET+PREVIEW_VGA_VSIZE+1)		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT	400		/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_XGA_HDNUM_FRAME_END	(PREVIEW_XGA_VOFFSET+PREVIEW_XGA_VSIZE+1)		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_XGA_HDNUM_CMDPORT	 1300		/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_720P_HDNUM_FRAME_END	(PREVIEW_720P_VOFFSET+PREVIEW_720P_VSIZE+1)		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_CMDPORT	 520		/*the command port interrupt line number of the preview new frame*/

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
#define	TG_SPI_I2C_MODE  0x20     	       /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	 0		      /*don't change*/
#define	TG_SPEED         FRONT_SSC_CLK_192K
 					/*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#define	TG_NBIT_REGDATA  16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */

#define CAP_FRAME_MODE  0        /* Frame mode or Rolling mode */

static UINT8 g_Is_snap_mode=0;
static UINT32 g_time_log[10];
static UINT32 g_cap_count=0;
#define MI_REG(a,b) {(a>>8),(a&0xFF),(b>>8),(b&0xFF)}

extern UINT32 tmrTimeClockGet();
/*prototypes of sensor model methods*/

static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreviewXGABegin();
static void sensorDoFuncPreview720PBegin();

static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();

static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();

static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapEarly1();


static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();

static void sensorDoFuncSnap_dark_BshutterEnd();

static void sensorDoFuncSnapNext(UINT32 next_mode);


#define DELAY_US 0xFF00
#define END_CMD  0xFFFF


#define DELAY_ONE_FRAME 0xFFF0


/* Aptina MT9J003 setting define */

static UINT16 cmd_init[][2] SENSOR_SIF1DMA_BUF=
{
/*New initial code*/
{0x301A, 0x0001},
{DELAY_US,300 },

/*Clock settings*/
{0x0300,0x0003},
{0x0302, 0x0002},
{0x0304,0x0002},
{0x0306, 0x005A},
{0x0308,0x000C},
{0x030A, 0x0002},
/*Clock settings*/

{0x3016,0x0121},
{0x3ECC,0x0FE4},
{0x316C,0x0429},
{0x3174,0x8000},
{0x3E40,0xDC05},
{0x3E42,0x6E22},
{0x3E44,0xDC22},
{0x3E46,0xFF00},
{0x3ED4,0xF998},
{0x3ED6,0x9789},
{0x3EDE,0xE41A},
{0x3EE0,0xA43F},
{0x3EE2,0xA4BF},
{0x3EEC,0x1C21},
{0x3ECE,0x141C},
{0x3ED8,0x9803},
{0x3EDA,0xD7C3},
{0x3EDC,0xD7E4},
{0x31AE,0x0304},
{0x31C6,0x0402},
{0x3064, 0x0805},

/*Default use the VGA resolution settings*/
{0x301A, 0x8018},
{0x3EDC,0xD7E4},
{0x3178,0x0070},
{0x0400, 0x0002},
{0x306E, 0x90B0},
{0x3004,0x0070},
{0x3008,0x0EB9},
{0x3002,0x0008},
{0x3006,0x0AC1},
{0x3040,0x2BC7},
{0x0404,0x0010},
{0x034C, 0x0394},
{0x034E,0x02B0},
{0x300C, 0x0966},
{0x300A, 0x033F},
{0x3010,0x0048},
{0x3014,0x020A},
{0x3018,0x0000},
{0x3012, 0x0336},
{0x3EDE, 0xE412},
{0x3170, 0x0071},
{0x30D4,0xB080},

{DELAY_US, 10000},

{0x301A, 0x001E},
{0x301A, 0x001C},
/*Default use the VGA resolution settings*/

/*New initial code*/

	{END_CMD,END_CMD}
};


#if (I2C_Batch_Write==1)

#else  /*I2C Batch write*/

static UINT8 cmd_720P_init[][4] SENSOR_SIF1DMA_BUF =
{
/*New code*/

	MI_REG(0x301A, 0x8018),

#if(DDR_CLOCK_297MHz)
	MI_REG(0x0300, 0x0003), 	
	MI_REG(0x0302, 0x0002),
	MI_REG(0x0304, 0x0002), 	
	MI_REG(0x0306, 0x005A),
	MI_REG(0x0308, 0x000C), 	
	MI_REG(0x030A, 0x0002), 	
	MI_REG(0x3016, 0x0121),
#endif
	
	MI_REG(0x3EDC, 0xD7E4),
	MI_REG(0x3178, 0x0070),
	MI_REG(0x0400,	0x0002),
	MI_REG(0x306E, 0x90B0),
	MI_REG(0x3004, 0x0070),
	MI_REG(0x3008, 0x0EBD),
	MI_REG(0x3002, 0x0160),
	MI_REG(0x3006, 0x0969),
	MI_REG(0x3040, 0x28C3),
	MI_REG(0x0404,	0x0010),
	MI_REG(0x034C, 0x0728),
	MI_REG(0x034E, 0x0406),
	MI_REG(0x300C, 0x0D4C),
	MI_REG(0x300A, 0x0497),
	MI_REG(0x3010, 0x009C),
	MI_REG(0x3012, 0x048B),
	MI_REG(0x3014, 0x03F2),
	MI_REG(0x3018,	0x0000),
	MI_REG(0x3EDE, 0xE412),
	MI_REG(0x3170, 0x0071),
	MI_REG(0x30D4,	0x9080),
      /*Move these two commands to stream on and frame restart function*/
	/*
	MI_REG(0x301A, 0x001E),
	MI_REG(0x301A,	0x001C),

	*/
/*New code*/
};

static UINT8 cmd_vga_init[][4] SENSOR_SIF1DMA_BUF =
{
/*New code*/
MI_REG(0x301A, 0x8018),

#if(DDR_CLOCK_297MHz)
	MI_REG(0x0300, 0x0003), 	
MI_REG(0x0302, 0x0002),
	MI_REG(0x0304, 0x0002), 	
MI_REG(0x0306, 0x005A),
	MI_REG(0x0308, 0x000C), 	
	MI_REG(0x030A, 0x0002), 	
	MI_REG(0x3016, 0x0121),
#endif
	
	MI_REG(0x3EDC,0xD7E4),
	MI_REG(0x3178,0x0070),
MI_REG(0x0400, 0x0002),
MI_REG(0x306E, 0x90B0),
	MI_REG(0x3004,0x0070),
	MI_REG(0x3008,0x0EB9),
	MI_REG(0x3002,0x0008),
	MI_REG(0x3006,0x0AC1),
	MI_REG(0x3040,0x2BC7),
	MI_REG(0x0404,0x0010),
MI_REG(0x034C, 0x0394),
	MI_REG(0x034E,0x02B0),
MI_REG(0x300C, 0x0966),
MI_REG(0x300A, 0x033F),
	MI_REG(0x3010,0x0048),
	MI_REG(0x3014,0x020A),
	MI_REG(0x3018,0x0000),
MI_REG(0x3012, 0x0336),
MI_REG(0x3EDE, 0xE412),
MI_REG(0x3170, 0x0071),
	MI_REG(0x30D4,0xB080),

/*Move these two commands to stream on and frame restart function*/
/*
MI_REG(0x301A, 0x001E),
MI_REG(0x301A, 0x001C),
*/
/*New code*/
};


static UINT8 cmd_xga_init[][4] SENSOR_SIF1DMA_BUF =
{
	MI_REG(0x301A, 0x8018),

#if(DDR_CLOCK_297MHz)
	MI_REG(0x0300, 0x0003),
	MI_REG(0x0302, 0x0002),
	MI_REG(0x0304, 0x0002),
	MI_REG(0x0306, 0x005A),
	MI_REG(0x0308, 0x000C),
	MI_REG(0x030A, 0x0002),
	MI_REG(0x3016, 0x0121),	
#endif
	
	MI_REG(0x3EDC, 0xD7E4),
	MI_REG(0x3178, 0x0070),
	MI_REG(0x0400,0x0002),
	MI_REG(0x306E, 0x90B0),
	MI_REG(0x3004, 0x0070),
	MI_REG(0x3008, 3775),
	MI_REG(0x3002, 0x0008),
	MI_REG(0x3006, 2755),
	MI_REG(0x3040, 0x28C3),
	MI_REG(0x0404, 0x0010),
	MI_REG(0x034C, 1832),
	MI_REG(0x034E, 1374),
	MI_REG(0x300C, 0x0CD6),       /*0xcd6=3286 ,24fps*/
	/*MI_REG(0x300C, 2628),*/     /*30fps*/
	MI_REG(0x300A, 1522),
	MI_REG(0x3010, 0x009C),
	MI_REG(0x3014, 0x03F2),
	MI_REG(0x3012, 0x04B5),
	MI_REG(0x3018, 0x0000),
	MI_REG(0x3EDE, 0xE41A),
	MI_REG(0x3170, 0x00E5),
	MI_REG(0x30D4, 0x9080),

      /*Move these two commands to stream on and frame restart function*/	
	/*
	MI_REG(0x301A, 0x001E),
	MI_REG(0x301A,0x001C),
	*/
};

static UINT8 cmd_still_init[][4] SENSOR_SIF1DMA_BUF =
{
/*New code*/

/*Move the stream off to the snap exp setting function.*/
/*Before setting the snap exposure function, stream will be off.*/

#if(!SHUTTER_LAG_IMPROVE)
MI_REG(0x301A, 0x8018),
#endif

#if(DDR_CLOCK_297MHz)
	MI_REG(0x0300, 0x0003), 	
	MI_REG(0x0302, 0x0001), 	
	MI_REG(0x0304, 0x0002), 	
MI_REG(0x0306, 0x0036),
	MI_REG(0x0308, 0x000C), 	
	MI_REG(0x030A, 0x0001), 	
MI_REG(0x3016, 0x0121),
#endif


	MI_REG(0x3EDC,0xD7E4),
	MI_REG(0x3178,0x0000),
	MI_REG(0x0400,0x0002),
	MI_REG(0x306E,0x9080),
	MI_REG(0x3004,0x0070),
	MI_REG(0x3008,0x0EBF),
	MI_REG(0x3002,0x0008),
	MI_REG(0x3006,0x0AC3),
	MI_REG(0x3040,0x0041),
	MI_REG(0x0404,0x0010),
	MI_REG(0x034C,0x0E50),
	MI_REG(0x034E,0x0ABC),
/*MI_REG(0x0342, 0x0EC0),*/
#if BURST_CAP_EN
	/*bandwidth issue*/
	MI_REG(0x0342, (3976+600)),
#else
	MI_REG(0x0342,3976),
#endif
	MI_REG(0x0340,0x0B4C),
	MI_REG(0x3010,0x009C),
	MI_REG(0x3014,0x03F2),
	MI_REG(0x3018,0x0000),
MI_REG(0x3EDE, 0xE41A),
MI_REG(0x3170, 0x00E5),
MI_REG(0x30D4, 0xB080),

/*Move these two commands to stream on and frame restart function*/
/*
MI_REG(0x301A, 0x001E),
MI_REG(0x301A, 0x001C),
*/

/*New code*/
};

static UINT8 cmd_stream_on[][4] SENSOR_SIF1DMA_BUF =
{
MI_REG(0x301A, 0x001E),
	MI_REG(0x301A,0x001C),
};

#endif /*I2C Batch write*/


/*5330 definition smaples.*/
/*sensor HiSPi cfg*/
static const sensorLVDSCfg_t sensorHiSpiCfg = 
{
	.wordcntsel 	 =1 ,	/*Hispi hsize from 0x90a4*/
	.Lvdsmode		 =1 ,	/*12bit mode*/
	.Lvdslanenum	 =0 ,	/*4 lane mode*/
	.Lvdslanemode	 =0 ,	/*enable lane3 in 4 lanes mode*/
	.Lvdslaneswap	 =0 ,	/*MSB first */
	.Lvdsraw		 =1 ,	/*raw 12 */
	.Lvdseofsel 	 =2 ,	/*detect eof sync code*/
	.Lvdseolsel 	 =1 ,	/*detect eol sync code*/
	.lvdsvdsrc		 =1 ,	/*1: decode emedded data header sof as vd source*/
	.lvds_dphy_sel	 =1 ,	/*MIPI D-PHY*/
	.Ctrlword_sol	 =0x800,
	.Ctrlword_sof	 =0xc00,/* Hd  mode = 0xc00 full resoultion mode =0xc80*/
	.Ctrlword_eol	 =0xa00,
	.Ctrlword_eof	 =0xe00,
	.syncwrod0		 =0 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	/*HW signal spec */
	.vmode			 =0 ,	
	.S2pphase		 =0 ,	
	.tsettle		 =3 ,	
	.teot			 =7 ,	
	/*Extention Part */		
	.Exthd_num		 =10	, 
	.Exthd_pol		 =0 ,
	.Exthd_mode 	 =0 
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
	.crop = NULL,
	.hsize = PREVIEW_VGA_HSIZE,
	.vsize = PREVIEW_VGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
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
#if 0 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio = 3200,/*16:9 sensor*/
	.vratio = 1800,
#endif
	
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
	.hsize = PREVIEW_720P_HSIZE,
	.vsize = PREVIEW_720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
&sensorTgPreviewVGACfg, &sensorTgPreviewXGACfg, &sensorTgPreview720PCfg,
};


static sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			#if 0
			.psnapEarly = (sensorDoFunc_t []){
				sensorDoFuncSnapEarly0,    
			},
			#endif
			.psnapEarly = NULL,
			
			.snapExpBegin = sensorDoFuncSnapExpBegin,
			.snapBshutterBegin = sensorDoFuncSnapBshutterBegin,
			.snapBshutterFollow = sensorDoFuncSnapBshutterFollow,
			.snapBshutterEnd = sensorDoFuncSnap_dark_BshutterEnd,
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
	

/****TODO****frame snap mode*/
static  sensorTgSnapCfg_t sensorTgSnapFrameCfg = {
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
	.n_snap_early = SNAP_FRAME_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_FRAME_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
};


static sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	 &sensorTgSnapRollCfg,&sensorTgSnapFrameCfg,
	
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
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
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
	.lastlinepix     = PREVIEW_XGA_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_XGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_XGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_XGA_HDNUM_CMDPORT,
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
	.hsize           = PREVIEW_720P_HSIZE,
	.vsize           = PREVIEW_720P_VSIZE,
	.lastlinepix     = PREVIEW_720P_LINETOTAL,
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
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_720P_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
};

static  sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
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
	.conf_yuvvdpol	 = 1, /*yuvvd just before vdres 0:active low*/
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


static void
sensorStreamOnAndFrameRestart(void)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	tmrUsWait(100000);/*This delay is needed before streaming on.*/
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0]), cmd_stream_on[0],0);
}

static void 
sensorReportsize()
{
	profLogPrintf(1,"mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	profLogPrintf(1,"front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	profLogPrintf(1,"imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	profLogPrintf(1,"rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	profLogPrintf(1,"rawmux output size    = (%d,%d)",READ16(0xb0002090),READ16(0xb0002092));
}

void
sensorSingleWrite(UINT16 addr,UINT16 data)
{
	UINT32 cmdportid;
	UINT8 regdata[4];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regdata[0]=addr>>8;
	regdata[1]=addr&0xFF;
	regdata[2]=data>>8;
	regdata[3]=data&0xFF;
	sensorSerialBulkWrite(cmdportid,1,regdata,0);
}

static void
sensorBatchWrite(UINT16 *pdata)
{
	UINT32 cmdportid;
	UINT8 regdata[4];
	UINT16 i;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	for(i=0;i<1000;i++)
	{
		if(pdata[i*2]==END_CMD)
			break;

		if(pdata[i*2]==DELAY_US)
		{
			/*tmrUsWait(300000);*/
			tmrUsWait(pdata[i*2+1]);	
		}
		else
		{
			regdata[0]=pdata[i*2]>>8;
			regdata[1]=pdata[i*2]&0xFF;
			regdata[2]=pdata[i*2+1]>>8;
			regdata[3]=pdata[i*2+1]&0xFF;
			/*profLogPrintf(1,"[%x %x %x %x]",regdata[0],regdata[1],regdata[2],regdata[3]);*/			
			sensorSerialBulkWrite(cmdportid,1,regdata,0);
		}
	}
}

static void
sensorGroupWrite(UINT8 *pdata, UINT32 a_byteCnt)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;

	PROF_LOG_PRINT(0, "ByteCnt=%d",a_byteCnt);

	sensorSerialBulkWrite(cmdportid,a_byteCnt,pdata,0);
}

static UINT16	
sensorRead(UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[4];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	sensorSerialBlockRead(cmdportid,1,regread,0);
	return (regread[3]|(regread[2]<<8));
}


void
sensorTg_mt9J003_Install()
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
	};
    PROF_LOG_ADD(0, "sensorDoFuncInit()___enter");
    /* sensorSerialSif1dmaForceOff(1);*/

   /* TGGPIO configuration here */    

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;
	profLogAdd(1,"MT 9J003");

	sensorTgPllClkExtSet(16000000);/*Lowest input clock provided from ASIC is 16MHz*/


/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
/*****TODO*****front reset after these???*/

#if 0
	sp5kGpioCfgSet(SENOR_GPIO_GRP,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);

/*Micron Power up sequence Here*/

	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,0); /* PWD LOW*/
	tmrUsWait(50000); /*50ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(25000); /*25ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
#endif

   	cmdportid = sensorTgCfg.cmdportid_xtg;
   	tmrUsWait(2000);
	profLogAdd(1,"MT 9J003  001");

	/*I2C Init code Here */
	profLogPrintf(1,"sensor DoFunInit model-ID =%x",sensorRead(0x3000));
	printf("sensor DoFunInit model-ID =%x\n ",sensorRead(0x3000));

	/*TG PLL=MCLK out=24MHz*/
	profLogAdd(1,"cmd_init");
sensorBatchWrite(cmd_init[0]);	
	sensorLVDSCfgSet(&sensorHiSpiCfg);	

	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

	/*tmrUsWait(100000);*/	/* delay 100 ms*/
tmrUsWait(100000);	
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

	#if (DDR_CLOCK_297MHz)
	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,148500); /* 121 MHz  CDSP 243MHz*/
	#else
	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121000); /* 121 MHz  CDSP 243MHz*/
	#endif
	tmrUsWait(10000);

/*	sensorBatchWrite(cmd_init[0]); */
	printf("Sensor_mt9J003 init finish \n");

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
	  
	  UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
  
	  /*SEND I2C COMMAND here */

	  sensor_event_wait(0x04);

	  profLogAdd(1,"cmd_vga_init");

	  sensorSerialBulkWrite(cmdportid,sizeof(cmd_vga_init)/sizeof(cmd_vga_init[0]), cmd_vga_init[0],0);

	  /*sensorSerialAsyncWrite(cmdportid,sizeof(cmd_vga_init)/sizeof(cmd_vga_init[0]), cmd_vga_init[0]);*/
	  sensorStreamOnAndFrameRestart();
  
	  sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);      
	  sensorDsenExtSet();
  
  	  g_Is_snap_mode=0;
	  
	  PROF_LOG_ADD(0, "HiSPi vga done");

  /*Change TG Mode Here*/

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
	
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;

	sensor_event_wait(0x04);
	profLogAdd(1,"cmd_xga_init");

	sensorSerialBulkWrite(cmdportid,sizeof(cmd_xga_init)/sizeof(cmd_xga_init[0]), cmd_xga_init[0],0);

	/*sensorSerialAsyncWrite(cmdportid,sizeof(cmd_xga_init)/sizeof(cmd_xga_init[0]), cmd_xga_init[0]);*/
	sensorStreamOnAndFrameRestart();

	sensorSyncSigSet(&sensorSyncSigPreviewXGACfg);
	sensorDsenExtSet();	

	g_Is_snap_mode=0;
	
	PROF_LOG_ADD(0, "HiSPi xga done");
   
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
	
	  UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
  
	  /*SEND I2C COMMAND here */

	  sensor_event_wait(0x04);

	  profLogAdd(1,"cmd_720P_init");

	  sensorSerialBulkWrite(cmdportid,sizeof(cmd_720P_init)/sizeof(cmd_720P_init[0]), cmd_720P_init[0],0);

	  /*sensorSerialAsyncWrite(cmdportid,sizeof(cmd_720P_init)/sizeof(cmd_720P_init[0]), cmd_720P_init[0]);*/
	  sensorStreamOnAndFrameRestart();

	  sensorSyncSigSet(&sensorSyncSigPreview720PCfg);      
	sensorDsenExtSet();
  
  	g_Is_snap_mode=0;
	  PROF_LOG_ADD(0, "HiSPi 720P done");

  /*Change TG Mode Here*/
   
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

	PROF_LOG_ADD(0, "sensorDoFuncSnapExpBegin");	
	

#if CAP_FRAME_MODE
	/*config frame_end & comport position before frame exposure mode. */
	static  sensorSyncSigHdnumCfg_t earlyHdCfg;
	earlyHdCfg.hdnum_cmdport=200 /*PREVIEW_XGA_HDNUM_CMDPORT*/;
	earlyHdCfg.hdnum_frame_end=625 /*PREVIEW_XGA_HDNUM_FRAME_END*/;
	earlyHdCfg.hdnum_frame_start=PREVIEW_HDNUM_FRAME_START;
	sensorSyncSigHdnumSet(&earlyHdCfg);
#else

	#if(!SHUTTER_LAG_IMPROVE)
		UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
		sensorSyncSigSet(&sensorSyncSigSnapCfg);		
		sensorDsenExtSet();

		sensorSerialBulkWrite(cmdportid,sizeof(cmd_still_init)/sizeof(cmd_still_init[0]), cmd_still_init[0],0);
		sensorStreamOnAndFrameRestart();
		PROF_LOG_ADD(0, "sensorDoFuncSnapExpBegin, StillMicroCodeSentDone");
	#endif
	
#endif	

}

/*Dummy line settings before enter frame mode */
static void
sensorSnapFrameDummySet(
)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;	
	/*SEND I2C COMMAND here */
	
}

static void
sensorSnapFrameModeSet(
)
{
	/*SEND I2C COMMAND here */
}

static void
sensorDoFuncSnapEarly0()
{
}
static void
sensorDoFuncSnapEarly1()
{
}


/* add this */ 
void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param); 

static void 
FlashDirectTrig(void *param) 
{
    halFrontFlashCfgSet(0, 0); 
    halFrontFlashTrig(); 
} 

static void
sensorDoFuncSnapFrexSet(
	UINT32 param
)
{

	PROF_LOG_ADD(0, "sensorDoFuncSnapFrexSet");

  	sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, SENSOR_FREX);
	/* add this */ 
	if (sp5kSnapFlashCfgGet(SP5K_SNAP_FLASH_CFG_MODE))
		tmrWaitListInstall(2, 53, FlashDirectTrig, 0);/*FlashDirectTrig(0);*/
}


static void
sensorDoFuncSnapBshutterBegin()
{
	/* sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);*/
	/*sensor_frame_orig_vd_neg_cb_reg(sensorDoFuncSnapFrexSet,0);*/
}

static void
sensorDoFuncSnapBshutterFollow()
{
}

static void
sensorDoFuncSnapBshutterEnd()
{
	/*sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);*/
}

static void sensorDoFuncSnap_dark_BshutterEnd()
{
	/*sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);*/
}


static void
sensorDoFuncSnapShortExp()
{

}
static void
sensorDoFuncSnapExpEnd()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapExpEnd()");
	/*sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);*/
}

static void
sensorDoFuncSnapReadoutField0()
{
#if 0
#if CAP_FRAME_MODE
	sensorSnapFrameModeSet();
	sensor_frame_vd_cb_reg(sensorDoFuncSnapFrexSet,0);
#endif
	PROF_LOG_ADD(0, "sensorDoFuncSnapReadoutField0()");
#endif		
		g_cap_count++;

		PROF_LOG_ADD(0, "sensorDoFuncSnapReadField0");
		PROF_LOG_PRINT(1, "sensorDoFuncSnapReadField0 count=%d",g_cap_count);

#if CAP_FRAME_MODE
		/*config frame_end & comport position before frame exposure mode. */
		static	sensorSyncSigHdnumCfg_t earlyHdCfg;
		earlyHdCfg.hdnum_cmdport=200 /*PREVIEW_XGA_HDNUM_CMDPORT*/;
		earlyHdCfg.hdnum_frame_end=625 /*PREVIEW_XGA_HDNUM_FRAME_END*/;
		earlyHdCfg.hdnum_frame_start=PREVIEW_HDNUM_FRAME_START;
		sensorSyncSigHdnumSet(&earlyHdCfg);
#else

		#if(SHUTTER_LAG_IMPROVE)	
			UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;		
			sensorSyncSigSet(&sensorSyncSigSnapCfg);		
			sensorDsenExtSet();
	
			sensorSerialBulkWrite(cmdportid,sizeof(cmd_still_init)/sizeof(cmd_still_init[0]), cmd_still_init[0],0);
			sensorStreamOnAndFrameRestart();
		
			PROF_LOG_ADD(0, "sensorDoFuncSnapReadField0, StillMicroCodeSentDone");
		#endif
		
#endif
	g_Is_snap_mode=1;

}

static void
sensorDoFuncSnapFieldEnd()
{
	PROF_LOG_PRINT(0, "sensorDoFuncSnapFieldEnd  CDSPGating=0x%x,SnapTrig=%d",READ8(0xb0009080),READ8(0xb0009068));
	PROF_LOG_PRINT(1, "sensorDoFuncSnapFieldEnd  CDSPGating=0x%x,SnapTrig=%d count=%d",READ8(0xb0009080),READ8(0xb0009068),g_cap_count);
	#if S2S_TIME_LOG
	g_time_log[7]=tmrTimeClockGet();
	#endif
}

/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{
	UINT32 t1;
	PROF_LOG_ADD(0, "sensorDoFuncSnapReadoutEnd");
	#if S2S_TIME_LOG	
	g_time_log[8]=tmrTimeClockGet();
	PROF_LOG_PRINT(1,"sensorDoFuncSnapReadoutEnd count=%d ==> ReadEnd2ReadEnd=%d us",g_cap_count,g_time_log[8]-g_time_log[9]);
	PROF_LOG_PRINT(1,"sensorDoFuncSnapReadoutEnd count=%d ==> Start2ReadEnd=%d us"  ,g_cap_count,g_time_log[8]-g_time_log[7]);
	g_time_log[9]=tmrTimeClockGet();
	#endif
}

static void
sensorDoFuncSnapNext(UINT32 mode)
{
#if	BURST_CAP_EN
	if(mode==0x40)
	{
		PROF_LOG_ADD(0,"@@@@ get sensor next mode is snap");
		PROF_LOG_PRINT(1,"@@@@ get sensor next mode is snap snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
		sensorTgSnapRollCfg.skip_snap_readout_vd=4;
	}
	else
	{
		PROF_LOG_ADD(0,"@@@@ get sensor next mode is preview");
		PROF_LOG_PRINT(1,"@@@@ get sensor next mode is preview snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
		sensorTgSnapRollCfg.skip_snap_readout_vd=0;
	}
#endif

}

#if EVTBL_TEST_CODE
static const UINT8 exptbl_stream[]
COMMON_ALIGNED = {
/* Create by Exposure Table generator tool Version: 1.07*/
/* CCD : MT9J003*/
/*  TG : MT9J003*/
0x3,
0x1,
/* Sensor Mode 0 ,0x30 */
/* ExptblCfg Content (0 n_entry,1 tblidx_min,2 sensorMode,3 exptbl_idx0_unit,4 exptbl_idx_unit,5 n_burstsec) */
0xb1,
0x0 ,
0x30,
32  ,
0x10,
0x1 ,
/* T,emplate [0]  */
0x30,
0x12,
0x0 ,
0x0 ,
/* M,ODE 0 SEC 0  */
/* E,vBurstCfg Content (0 n_burstidx,1 nbyte_entry,2 burstidx_min,3 n_op) */
0xb1,
0x2 ,
0x0 ,
0x2 ,
/* M,ODE 0 SEC 0 BurstProtocolCfg [0] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x8 ,
0x10,
0x8 ,
/* M,ODE 0 SEC 0 BurstProtocolCfg [1] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x0 ,
0x18,
0x8 ,
/* M,ODE 0 SEC 0 EV Table Data BEGIN (total entry = 177 , byte entry = 2) */
0x5a,
0x18,
0x52,
0x17,
0x55,
0x16,
0x63,
0x15,
0x7a,
0x14,
0x9c,
0x13,
0xc7,
0x12,
0xfc,
0x11,
0x38,
0x11,
0x7d,
0x10,
0xcb,
0xf ,
0x1f,
0xf ,
0x7b,
0xe ,
0xde,
0xd ,
0x47,
0xd ,
0xb7,
0xc ,
0x2d,
0xc ,
0xa9,
0xb ,
0x2a,
0xb ,
0xb1,
0xa ,
0x3d,
0xa ,
0xce,
0x9 ,
0x64,
0x9 ,
0xfe,
0x8 ,
0x9c,
0x8 ,
0x3f,
0x8 ,
0xe5,
0x7 ,
0x90,
0x7 ,
0x3e,
0x7 ,
0xef,
0x6 ,
0xa4,
0x6 ,
0x5c,
0x6 ,
0x17,
0x6 ,
0xd5,
0x5 ,
0x95,
0x5 ,
0x59,
0x5 ,
0x1f,
0x5 ,
0xe7,
0x4 ,
0xb2,
0x4 ,
0x7f,
0x4 ,
0x4e,
0x4 ,
0x1f,
0x4 ,
0xf3,
0x3 ,
0xc8,
0x3 ,
0x9f,
0x3 ,
0x77,
0x3 ,
0x52,
0x3 ,
0x2e,
0x3 ,
0xb ,
0x3 ,
0xea,
0x2 ,
0xcb,
0x2 ,
0xac,
0x2 ,
0x8f,
0x2 ,
0x74,
0x2 ,
0x59,
0x2 ,
0x3f,
0x2 ,
0x27,
0x2 ,
0x10,
0x2 ,
0xf9,
0x1 ,
0xe4,
0x1 ,
0xcf,
0x1 ,
0xbc,
0x1 ,
0xa9,
0x1 ,
0x97,
0x1 ,
0x86,
0x1 ,
0x75,
0x1 ,
0x65,
0x1 ,
0x56,
0x1 ,
0x48,
0x1 ,
0x3a,
0x1 ,
0x2c,
0x1 ,
0x20,
0x1 ,
0x14,
0x1 ,
0x8 ,
0x1 ,
0xfd,
0x0 ,
0xf2,
0x0 ,
0xe8,
0x0 ,
0xde,
0x0 ,
0xd4,
0x0 ,
0xcb,
0x0 ,
0xc3,
0x0 ,
0xbb,
0x0 ,
0xb3,
0x0 ,
0xab,
0x0 ,
0xa4,
0x0 ,
0x9d,
0x0 ,
0x96,
0x0 ,
0x90,
0x0 ,
0x8a,
0x0 ,
0x84,
0x0 ,
0x7e,
0x0 ,
0x79,
0x0 ,
0x74,
0x0 ,
0x6f,
0x0 ,
0x6a,
0x0 ,
0x66,
0x0 ,
0x61,
0x0 ,
0x5d,
0x0 ,
0x59,
0x0 ,
0x56,
0x0 ,
0x52,
0x0 ,
0x4e,
0x0 ,
0x4b,
0x0 ,
0x48,
0x0 ,
0x45,
0x0 ,
0x42,
0x0 ,
0x3f,
0x0 ,
0x3c,
0x0 ,
0x3a,
0x0 ,
0x37,
0x0 ,
0x35,
0x0 ,
0x33,
0x0 ,
0x31,
0x0 ,
0x2f,
0x0 ,
0x2d,
0x0 ,
0x2b,
0x0 ,
0x29,
0x0 ,
0x27,
0x0 ,
0x26,
0x0 ,
0x24,
0x0 ,
0x22,
0x0 ,
0x21,
0x0 ,
0x20,
0x0 ,
0x1e,
0x0 ,
0x1d,
0x0 ,
0x1c,
0x0 ,
0x1b,
0x0 ,
0x19,
0x0 ,
0x18,
0x0 ,
0x17,
0x0 ,
0x16,
0x0 ,
0x15,
0x0 ,
0x14,
0x0 ,
0x14,
0x0 ,
0x13,
0x0 ,
0x12,
0x0 ,
0x11,
0x0 ,
0x10,
0x0 ,
0x10,
0x0 ,
0xf ,
0x0 ,
0xe ,
0x0 ,
0xe ,
0x0 ,
0xd ,
0x0 ,
0xd ,
0x0 ,
0xc ,
0x0 ,
0xc ,
0x0 ,
0xb ,
0x0 ,
0xb ,
0x0 ,
0xa ,
0x0 ,
0xa ,
0x0 ,
0x9 ,
0x0 ,
0x9 ,
0x0 ,
0x9 ,
0x0 ,
0x8 ,
0x0 ,
0x8 ,
0x0 ,
0x8 ,
0x0 ,
0x7 ,
0x0 ,
0x7 ,
0x0 ,
0x7 ,
0x0 ,
0x6 ,
0x0 ,
0x6 ,
0x0 ,
0x6 ,
0x0 ,
0x6 ,
0x0 ,
0x5 ,
0x0 ,
0x5 ,
0x0 ,
0x5 ,
0x0 ,
0x5 ,
0x0 ,
0x4 ,
0x0 ,
0x4 ,
0x0 ,
0x4 ,
0x0 ,
0x4 ,
0x0 ,
0x4 ,
0x0 ,
0x4 ,
0x0 ,
0x3 ,
0x0 ,
0x3 ,
0x0 ,
0x3 ,
0x0 ,
0x3 ,
0x0 ,
/* M,ODE 0 SEC 0 EV Table Data END */
/* M,ODE 0 VTable ExpVtblCfg  Content (0 n_vtblsec,1 def_vtype,2 n_vidx,3 nbyte_entry,4 vidx_min,5 vtbl_type)*/
0x1 ,
0x2 ,
0x0 ,
0x0 ,
0x0 ,
0x2 ,
/* M,ODE 0 V Table Data BEGIN (total entry = 0 , byte entry = 0) */
/* M,ODE 0 V Table Data END */
/* M,ODE 0 Error Table EvErrorCfg Content (0 n_eidx,1 eidx_min,2 err_unit)*/
0x0 ,
0x0 ,
0x40,
/* M,ODE 0 Error Table Data BEGIN (total entry = 0 ) */
/* M,ODE 0 Error Table Data END */
/* S,ensor Mode 1 ,0x31 */
/* E,xptblCfg Content (0 n_entry,1 tblidx_min,2 sensorMode,3 exptbl_idx0_unit,4 exptbl_idx_unit,5 n_burstsec) */
0xb1,
0x0 ,
0x31,
32  ,
0x10,
0x1 ,
/* T,emplate [0]  */
0x30,
0x12,
0x0 ,
0x0 ,
/* M,ODE 1 SEC 0  */
/* E,vBurstCfg Content (0 n_burstidx,1 nbyte_entry,2 burstidx_min,3 n_op) */
0xb1,
0x2 ,
0x0 ,
0x2 ,
/* M,ODE 1 SEC 0 BurstProtocolCfg [0] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x8 ,
0x10,
0x8 ,
/* M,ODE 1 SEC 0 BurstProtocolCfg [1] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x0 ,
0x18,
0x8 ,
/* M,ODE 1 SEC 0 EV Table Data BEGIN (total entry = 177 , byte entry = 2) */
0xaa,
0x23,
0x27,
0x22,
0xb4,
0x20,
0x51,
0x1f,
0xfd,
0x1d,
0xb8,
0x1c,
0x80,
0x1b,
0x55,
0x1a,
0x38,
0x19,
0x26,
0x18,
0x20,
0x17,
0x25,
0x16,
0x35,
0x15,
0x4e,
0x14,
0x72,
0x13,
0x9f,
0x12,
0xd5,
0x11,
0x13,
0x11,
0x5a,
0x10,
0xa8,
0xf ,
0xff,
0xe ,
0x5c,
0xe ,
0xc0,
0xd ,
0x2b,
0xd ,
0x9c,
0xc ,
0x13,
0xc ,
0x90,
0xb ,
0x12,
0xb ,
0x9a,
0xa ,
0x27,
0xa ,
0xb9,
0x9 ,
0x4f,
0x9 ,
0xea,
0x8 ,
0x8a,
0x8 ,
0x2d,
0x8 ,
0xd4,
0x7 ,
0x7f,
0x7 ,
0x2e,
0x7 ,
0xe0,
0x6 ,
0x95,
0x6 ,
0x4e,
0x6 ,
0x9 ,
0x6 ,
0xc8,
0x5 ,
0x89,
0x5 ,
0x4d,
0x5 ,
0x14,
0x5 ,
0xdc,
0x4 ,
0xa8,
0x4 ,
0x75,
0x4 ,
0x45,
0x4 ,
0x16,
0x4 ,
0xea,
0x3 ,
0xc0,
0x3 ,
0x97,
0x3 ,
0x70,
0x3 ,
0x4b,
0x3 ,
0x27,
0x3 ,
0x5 ,
0x3 ,
0xe4,
0x2 ,
0xc5,
0x2 ,
0xa7,
0x2 ,
0x8a,
0x2 ,
0x6e,
0x2 ,
0x54,
0x2 ,
0x3b,
0x2 ,
0x22,
0x2 ,
0xb ,
0x2 ,
0xf5,
0x1 ,
0xe0,
0x1 ,
0xcb,
0x1 ,
0xb8,
0x1 ,
0xa5,
0x1 ,
0x93,
0x1 ,
0x82,
0x1 ,
0x72,
0x1 ,
0x62,
0x1 ,
0x53,
0x1 ,
0x45,
0x1 ,
0x37,
0x1 ,
0x2a,
0x1 ,
0x1d,
0x1 ,
0x11,
0x1 ,
0x6 ,
0x1 ,
0xfb,
0x0 ,
0xf0,
0x0 ,
0xe6,
0x0 ,
0xdc,
0x0 ,
0xd3,
0x0 ,
0xca,
0x0 ,
0xc1,
0x0 ,
0xb9,
0x0 ,
0xb1,
0x0 ,
0xaa,
0x0 ,
0xa2,
0x0 ,
0x9c,
0x0 ,
0x95,
0x0 ,
0x8f,
0x0 ,
0x89,
0x0 ,
0x83,
0x0 ,
0x7d,
0x0 ,
0x78,
0x0 ,
0x73,
0x0 ,
0x6e,
0x0 ,
0x69,
0x0 ,
0x65,
0x0 ,
0x61,
0x0 ,
0x5c,
0x0 ,
0x59,
0x0 ,
0x55,
0x0 ,
0x51,
0x0 ,
0x4e,
0x0 ,
0x4a,
0x0 ,
0x47,
0x0 ,
0x44,
0x0 ,
0x41,
0x0 ,
0x3f,
0x0 ,
0x3c,
0x0 ,
0x39,
0x0 ,
0x37,
0x0 ,
0x35,
0x0 ,
0x32,
0x0 ,
0x30,
0x0 ,
0x2e,
0x0 ,
0x2c,
0x0 ,
0x2a,
0x0 ,
0x29,
0x0 ,
0x27,
0x0 ,
0x25,
0x0 ,
0x24,
0x0 ,
0x22,
0x0 ,
0x21,
0x0 ,
0x1f,
0x0 ,
0x1e,
0x0 ,
0x1d,
0x0 ,
0x1b,
0x0 ,
0x1a,
0x0 ,
0x19,
0x0 ,
0x18,
0x0 ,
0x17,
0x0 ,
0x16,
0x0 ,
0x15,
0x0 ,
0x14,
0x0 ,
0x13,
0x0 ,
0x13,
0x0 ,
0x12,
0x0 ,
0x11,
0x0 ,
0x10,
0x0 ,
0x10,
0x0 ,
0xf ,
0x0 ,
0xe ,
0x0 ,
0xe ,
0x0 ,
0xd ,
0x0 ,
0xd ,
0x0 ,
0xc ,
0x0 ,
0xc ,
0x0 ,
0xb ,
0x0 ,
0xb ,
0x0 ,
0xa ,
0x0 ,
0xa ,
0x0 ,
0x9 ,
0x0 ,
0x9 ,
0x0 ,
0x9 ,
0x0 ,
0x8 ,
0x0 ,
0x8 ,
0x0 ,
0x7 ,
0x0 ,
0x7 ,
0x0 ,
0x7 ,
0x0 ,
0x7 ,
0x0 ,
0x6 ,
0x0 ,
0x6 ,
0x0 ,
0x6 ,
0x0 ,
0x6 ,
0x0 ,
0x5 ,
0x0 ,
0x5 ,
0x0 ,
0x5 ,
0x0 ,
0x5 ,
0x0 ,
0x4 ,
0x0 ,
/* M,ODE 1 SEC 0 EV Table Data END */
/* M,ODE 1 VTable ExpVtblCfg  Content (0 n_vtblsec,1 def_vtype,2 n_vidx,3 nbyte_entry,4 vidx_min,5 vtbl_type)*/
0x1 ,
0x2 ,
0x0 ,
0x0 ,
0x0 ,
0x2 ,
/* M,ODE 1 V Table Data BEGIN (total entry = 0 , byte entry = 0) */
/* M,ODE 1 V Table Data END */
/* M,ODE 1 Error Table EvErrorCfg Content (0 n_eidx,1 eidx_min,2 err_unit)*/
0x0 ,
0x0 ,
0x40,
/* M,ODE 1 Error Table Data BEGIN (total entry = 0 ) */
/* M,ODE 1 Error Table Data END */
/* S,ensor Mode 2 ,0x40 */
/* E,xptblCfg Content (0 n_entry,1 tblidx_min,2 sensorMode,3 exptbl_idx0_unit,4 exptbl_idx_unit,5 n_burstsec) */
0xb1,
0x0 ,
0x40,
0   ,
0x10,
0x1 ,
/* T,emplate [0]  */
0x30,
0x12,
0x0 ,
0x0 ,
/* M,ODE 2 SEC 0  */
/* E,vBurstCfg Content (0 n_burstidx,1 nbyte_entry,2 burstidx_min,3 n_op) */
0xb1,
0x2 ,
0x0 ,
0x2 ,
/* M,ODE 2 SEC 0 BurstProtocolCfg [0] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x8 ,
0x10,
0x8 ,
/* M,ODE 2 SEC 0 BurstProtocolCfg [1] Content (0 tmpl_reg,1 entry_lsb,2 reg_lsb,3 reg_mask_nbits)*/
0x0 ,
0x0 ,
0x18,
0x8 ,
/* M,ODE 2 SEC 0 EV Table Data BEGIN (total entry = 177 , byte entry = 2) */
0x24,
0x7c,
0xe0,
0x76,
0xd6,
0x71,
0x3 ,
0x6d,
0x63,
0x68,
0xf6,
0x63,
0xb9,
0x5f,
0xab,
0x5b,
0xc8,
0x57,
0xf ,
0x54,
0x7f,
0x50,
0x15,
0x4d,
0xd0,
0x49,
0xaf,
0x46,
0xb0,
0x43,
0xd1,
0x40,
0x12,
0x3e,
0x70,
0x3b,
0xeb,
0x38,
0x81,
0x36,
0x32,
0x34,
0xfb,
0x31,
0xdd,
0x2f,
0xd5,
0x2d,
0xe4,
0x2b,
0x7 ,
0x2a,
0x3f,
0x28,
0x8a,
0x26,
0xe8,
0x24,
0x58,
0x23,
0xd8,
0x21,
0x69,
0x20,
0x9 ,
0x1f,
0xb8,
0x1d,
0x76,
0x1c,
0x41,
0x1b,
0x19,
0x1a,
0xfe,
0x18,
0xee,
0x17,
0xeb,
0x16,
0xf2,
0x15,
0x4 ,
0x15,
0x20,
0x14,
0x45,
0x13,
0x74,
0x12,
0xac,
0x11,
0xec,
0x10,
0x34,
0x10,
0x84,
0xf ,
0xdc,
0xe ,
0x3b,
0xe ,
0xa0,
0xd ,
0xc ,
0xd ,
0x7f,
0xc ,
0xf7,
0xb ,
0x75,
0xb ,
0xf9,
0xa ,
0x82,
0xa ,
0x10,
0xa ,
0xa3,
0x9 ,
0x3a,
0x9 ,
0xd6,
0x8 ,
0x76,
0x8 ,
0x1a,
0x8 ,
0xc2,
0x7 ,
0x6e,
0x7 ,
0x1d,
0x7 ,
0xd0,
0x6 ,
0x86,
0x6 ,
0x3f,
0x6 ,
0xfc,
0x5 ,
0xbb,
0x5 ,
0x7c,
0x5 ,
0x41,
0x5 ,
0x8 ,
0x5 ,
0xd1,
0x4 ,
0x9d,
0x4 ,
0x6b,
0x4 ,
0x3b,
0x4 ,
0xd ,
0x4 ,
0xe1,
0x3 ,
0xb7,
0x3 ,
0x8f,
0x3 ,
0x68,
0x3 ,
0x43,
0x3 ,
0x20,
0x3 ,
0xfe,
0x2 ,
0xdd,
0x2 ,
0xbe,
0x2 ,
0xa0,
0x2 ,
0x84,
0x2 ,
0x69,
0x2 ,
0x4f,
0x2 ,
0x35,
0x2 ,
0x1d,
0x2 ,
0x7 ,
0x2 ,
0xf1,
0x1 ,
0xdc,
0x1 ,
0xc7,
0x1 ,
0xb4,
0x1 ,
0xa2,
0x1 ,
0x90,
0x1 ,
0x7f,
0x1 ,
0x6f,
0x1 ,
0x5f,
0x1 ,
0x50,
0x1 ,
0x42,
0x1 ,
0x34,
0x1 ,
0x27,
0x1 ,
0x1b,
0x1 ,
0xf ,
0x1 ,
0x3 ,
0x1 ,
0xf8,
0x0 ,
0xee,
0x0 ,
0xe4,
0x0 ,
0xda,
0x0 ,
0xd1,
0x0 ,
0xc8,
0x0 ,
0xbf,
0x0 ,
0xb7,
0x0 ,
0xb0,
0x0 ,
0xa8,
0x0 ,
0xa1,
0x0 ,
0x9a,
0x0 ,
0x94,
0x0 ,
0x8d,
0x0 ,
0x87,
0x0 ,
0x82,
0x0 ,
0x7c,
0x0 ,
0x77,
0x0 ,
0x72,
0x0 ,
0x6d,
0x0 ,
0x68,
0x0 ,
0x64,
0x0 ,
0x60,
0x0 ,
0x5c,
0x0 ,
0x58,
0x0 ,
0x54,
0x0 ,
0x50,
0x0 ,
0x4d,
0x0 ,
0x4a,
0x0 ,
0x47,
0x0 ,
0x44,
0x0 ,
0x41,
0x0 ,
0x3e,
0x0 ,
0x3b,
0x0 ,
0x39,
0x0 ,
0x37,
0x0 ,
0x34,
0x0 ,
0x32,
0x0 ,
0x30,
0x0 ,
0x2e,
0x0 ,
0x2c,
0x0 ,
0x2a,
0x0 ,
0x28,
0x0 ,
0x27,
0x0 ,
0x25,
0x0 ,
0x23,
0x0 ,
0x22,
0x0 ,
0x20,
0x0 ,
0x1f,
0x0 ,
0x1e,
0x0 ,
0x1c,
0x0 ,
0x1b,
0x0 ,
0x1a,
0x0 ,
0x19,
0x0 ,
0x18,
0x0 ,
0x17,
0x0 ,
0x16,
0x0 ,
0x15,
0x0 ,
0x14,
0x0 ,
0x13,
0x0 ,
0x12,
0x0 ,
0x12,
0x0 ,
0x11,
0x0 ,
0x10,
0x0 ,
0x10,
0x0 ,
/* M,ODE 2 SEC 0 EV Table Data END */
/* M,ODE 2 VTable ExpVtblCfg  Content (0 n_vtblsec,1 def_vtype,2 n_vidx,3 nbyte_entry,4 vidx_min,5 vtbl_type)*/
0x1 ,
0x2 ,
0x0 ,
0x0 ,
0x0 ,
0x2 ,
/* M,ODE 2 V Table Data BEGIN (total entry = 0 , byte entry = 0) */
/* M,ODE 2 V Table Data END */
/* M,ODE 2 Error Table EvErrorCfg Content (0 n_eidx,1 eidx_min,2 err_unit)*/
0x0 ,
0x0 ,
0x40,
/* MODE 2 Error Table Data BEGIN (total entry = 0 ) */
/* MODE 2 Error Table Data END */

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
		sensor_exptbl_create(exptbl_stream, sizeof(exptbl_stream));
	#else
		sensor_exptbl_init();
	#endif
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
	/*****TODO******ITG fine exp set, value and timing???*/
	/*exp setting via serial port*/

	static UINT8 regdata[][4] SENSOR_SIF1DMA_VARBUF= { 
	  {0x30, 0x1A, 0x80, 0x18},/*Stream off firstly*/
	  {0x30, 0x12, 0x00, 0x00}
	}; 
	
#if 1
	if((sensorModeGet()&0xF0)==0x40) /*don't set exp time& dummy in snap mode*/
	{
		/*profLogPrintf(0,"snap [nreg=%d][%x %x %x %x] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7]);*/

		#if(SHUTTER_LAG_IMPROVE)
			regdata[1][2] = pdata[2];
			regdata[1][3] = pdata[3];		
			profLogPrintf(1,"snapReg [nreg=%d][%x %x %x %x] [%x %x %x %x]",
					sizeof(regdata)/sizeof(regdata[0]),regdata[0][0],regdata[0][1],regdata[0][2],regdata[0][3],
					regdata[1][0],regdata[1][1],regdata[1][2],regdata[1][3]);		
		
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, sizeof(regdata)/sizeof(regdata[0]), regdata);
		#else
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
		#endif
		
		/*sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,nreg, pdata);*/
	}
	else
	{
		/*profLogPrintf(0,"prev [nreg=%d][%x %x %x %x] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7]);*/
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
	}
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
#if SP5K_SENSOR_MT9J003
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	UINT8 regData[4] ={
		0x30,0x12,0x00,0x00
	};
	regData[2] = (expLine>>8)&0xff;
	regData[3] = expLine&0xff;
	printf("expLine = %d\n",expLine);
	sensor_event_wait(0x04);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regData);
}
#endif
/******TODO****/


