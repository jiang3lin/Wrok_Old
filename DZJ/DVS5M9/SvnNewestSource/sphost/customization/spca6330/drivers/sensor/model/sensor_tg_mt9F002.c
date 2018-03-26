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

#define MODEL_VERSION     "$unp_tg_mt9f002-0.00.22"
#define BUILD_TIME        __DATE__  "-"  __TIME__

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
#include "debug_mod/dbg_api.h"

#define  SENSOR_PROF_LOG_EN   (1)
#define  _SENSOR_DBG_         (0)

#if SENSOR_PROF_LOG_EN
    #define SENSOR_PROF_LOG_ADD(id, str)			profLogAdd(id, str)
    #define SENSOR_PROF_LOG_PRINT(id, fmt, arg...)	profLogPrintf(id, fmt, ##arg)
#else
    #define SENSOR_PROF_LOG_ADD(id, str)
    #define SENSOR_PROF_LOG_PRINT(id, fmt,arg...)
#endif

#if(_SENSOR_DBG_)
    /*debug by TGGPIO2*/
    #define DBG_GPIO(x)    if(x){WRITE8(0xb0009032, (READ8(0xb0009032)|(0x02)));}else{WRITE8(0xb0009032, (READ8(0xb0009032)&(~0x02)));}
    #define DBG_GPIO_PULSE()    {WRITE8(0xb0009032, (READ8(0xb0009032)|(0x02)));WRITE8(0xb0009032, (READ8(0xb0009032)&(~0x02)));}
    #define DBG_ASSERT(expr) \
	if ( !(expr) ) { \
		dbgAssert(__FILE__, __LINE__, NULL); \
	}

    #if 0
        #define GPIO_DBG_PULSE() if(1){sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,1<<0,1<<0);sp5kGpioWrite(SP5K_GPIO_GRP_LMI,1<<0,1);sp5kGpioWrite(SP5K_GPIO_GRP_LMI,1<<0,0);}
        #define GPIO_DBG_HI()    if(1){sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,1<<0,1<<0);sp5kGpioWrite(SP5K_GPIO_GRP_LMI,1<<0,1);}
        #define GPIO_DBG_LO()    if(1){sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,1<<0,1<<0);sp5kGpioWrite(SP5K_GPIO_GRP_LMI,1<<0,0);}
    #endif
#else
    #define DBG_GPIO(x)
#endif

static BOOL  fCap2Pv=0;
static BOOL  fpvsusp=0;

static BOOL  is_aborted=0;
static const sensorSyncSigCfg_t   *p_size_pcg;
static UINT32 snap_time_log[5];
static UINT8 OTPM=0;

#define TIME_TAG(id)   (snap_time_log[id])
#define TIME_LOG(id)    snap_time_log[id]=tmrTimeClockGet()
#define TIME_DIFF(a,b) (snap_time_log[a]-snap_time_log[b])

#define CAP_FRAME_MODE_BY_GPIO	       (0)
#define STREAMOF_ENABLE                (0)
#define SUSPEND_ENABLE                 (1)
#define SENSOR_IRQ_SVC_VD              (0)
#define SENSOR_IRQ_SVC_FRAME_END       (1)
#define SENSOR_IRQ_SVC_NEG_VD          (10)
#define SENSOR_IRQ_SVC_ORG_VD          (12)
#define SENSOR_EVENT_VVALID_END        (1UL<<13)
#define SENSOR_EVENT_SNAP_DONE         (1UL<<15)

/* Sensor image size definition here */
#define NO              0
#define BY_PIXEL        1
#define BY_HD           0
#define NOUSE			0
#define DISABLE			0xFFFF
#define DONTCHANE    	0x7FFFFFFF
#define DELAY_US 		0xFF00
#define END_CMD  		0xFFFF

/*#define SENSOR_FREX     BIT6*/          /* for A900 */
#define EVB             (0)
#define Boqy_5330       (0)
#define Boqy_6330       (1)

#if EVB
#define SENSOR_FREX     (1<<15)           /* for EVB  */
#define SENSOR_PWRDN    (1<<14)
#define SENSOR_RESET    (1<<12)
#define SENSOR_TEST     (1<<3)
#define SENSOR_PWR		(1<<12)
#define OUT_SENSOR_GPIO_MASK    SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define CFG_SENSOR_GPIO_DIR     SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define SENOR_GPIO_GRP SP5K_GPIO_GRP_LMI
#elif Boqy_5330
#define SENSOR_FREX     (1<<15)
#define SENSOR_PWRDN    (1<<4)    /*unuse now*/
#define SENSOR_RESET    (1<<10)   /*RC reset now*/ /*TG*/
#define SENSOR_TEST     (1<<3)
#define SENSOR_PWR		(1<<12)  /*GEN*/
#define OUT_TGGPIO_MASK     SENSOR_PWRDN|SENSOR_RESET
#define CFG_TGGPIO_DIR      SENSOR_PWRDN|SENSOR_RESET
#define OUT_GENGPIO_MASK    SENSOR_PWR
#define CFG_GENGPIO_DIR     SENSOR_PWR
#define SENOR_GPIO_GRP SP5K_GPIO_GRP_GEN
#elif Boqy_6330
#define SENSOR_FREX     (1<<15)           /* for EVB */
#define SENSOR_PWRDN    (1<<15)
#define SENSOR_RESET    (1<<4)
#define SENSOR_TEST     (1<<3)
#define SENSOR_PWR		(1<<15)           /*LMI*/
#define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define SENOR_GPIO_GRP SP5K_GPIO_GRP_LMI
#endif

#define FREX(x)                 if(x){sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, SENSOR_FREX);}else{sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);}

#define MCLK2MCLK(a)     (a)  /*NOP */

/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HSIZE_30FPS (1096)
#define PREVIEW_VGA_VSIZE_30FPS	(820)
#define PREVIEW_VGA_HSIZE_60FPS (1096)
#define PREVIEW_VGA_VSIZE_60FPS	(820)
#define PREVIEW_1080P_HSIZE 	(2304)
#define PREVIEW_1080P_VSIZE  	(1294)
#define CAPTURE_HSIZE_CALIB     (4608)
#define CAPTURE_HSIZE  			(4384)
#define CAPTURE_VSIZE  			(3288)

/* Preview default definition here */
#define PREVIEW_VGA_30FPS_LINETOTAL             MCLK2MCLK(0xC00)
#define PREVIEW_VGA_30FPS_LINEBLANK             (6)
#define PREVIEW_VGA_30FPS_FRAMETOTAL            (822)
#define PREVIEW_VGA_30FPS_FRAMEBLANK            (822)
#define PREVIEW_VGA_30FPS_HOFFSET               MCLK2MCLK(0x700)
#define PREVIEW_VGA_30FPS_HCROP_OFFSET          (0)/*6330 BUG workaround 4n-1*/
#define PREVIEW_VGA_30FPS_VOFFSET               (16)
#define PREVIEW_VGA_30FPS_HRESHAPE_FALL         MCLK2MCLK(0x500)
#define PREVIEW_VGA_30FPS_HRESHAPE_RISE         MCLK2MCLK(0x600)
#define PREVIEW_VGA_30FPS_VRESHAPE_COUNTER_SEL  (BY_HD)
#define PREVIEW_VGA_30FPS_VRESHAPE_FALL         (PREVIEW_VGA_30FPS_VOFFSET-3)
#define PREVIEW_VGA_30FPS_VRESHAPE_RISE         (PREVIEW_VGA_30FPS_VOFFSET-1)

#define PREVIEW_VGA_60FPS_LINETOTAL             MCLK2MCLK(0xC00)
#define PREVIEW_VGA_60FPS_LINEBLANK             (6)
#define PREVIEW_VGA_60FPS_FRAMETOTAL            (822)
#define PREVIEW_VGA_60FPS_FRAMEBLANK            (822)
#define PREVIEW_VGA_60FPS_HOFFSET               MCLK2MCLK(0x700)
#define PREVIEW_VGA_60FPS_HCROP_OFFSET          (0)/*6330 BUG workaround 4n-1*/
#define PREVIEW_VGA_60FPS_VOFFSET               (16)
#define PREVIEW_VGA_60FPS_HRESHAPE_FALL         MCLK2MCLK(0x500)
#define PREVIEW_VGA_60FPS_HRESHAPE_RISE         MCLK2MCLK(0x600)
#define PREVIEW_VGA_60FPS_VRESHAPE_COUNTER_SEL  (BY_HD)
#define PREVIEW_VGA_60FPS_VRESHAPE_FALL         (PREVIEW_VGA_60FPS_VOFFSET-3)
#define PREVIEW_VGA_60FPS_VRESHAPE_RISE         (PREVIEW_VGA_60FPS_VOFFSET-1)

#define PREVIEW_1080P_LINETOTAL                 MCLK2MCLK(0xEA0)
#define PREVIEW_1080P_LINEBLANK                 (6)
#define PREVIEW_1080P_FRAMETOTAL                (1296)
#define PREVIEW_1080P_FRAMEBLANK                (1296)
#define PREVIEW_1080P_HOFFSET                   MCLK2MCLK(0x500)
#define PREVIEW_1080P_HCROP_OFFSET              (0)/*6330 BUG workaround 4n-1*/
#define PREVIEW_1080P_VOFFSET                   (16)
#define PREVIEW_1080P_HRESHAPE_FALL             MCLK2MCLK(0x440)
#define PREVIEW_1080P_HRESHAPE_RISE             MCLK2MCLK(0x450)
#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL      (BY_HD)
#define PREVIEW_1080P_VRESHAPE_FALL             (PREVIEW_1080P_VOFFSET-3)
#define PREVIEW_1080P_VRESHAPE_RISE             (PREVIEW_1080P_VOFFSET-1)

/* Capture default definition here */
#define CAPTURE_LINETOTAL                       MCLK2MCLK(0x1200)
#define CAPTURE_LINEBLANK                       (6)
#define CAPTURE_FRAMETOTAL                      (3288)
#define CAPTURE_FRAMEBLANK                      (3288)
#define CAPTURE_HOFFSET                         MCLK2MCLK(0x48)
#define CAPTURE_HCROP_OFFSET                    (0)/*6330 BUG workaround 4n-1*/
#define CAPTURE_VOFFSET                         (16)
#define CAPTURE_HRESHAPE_FALL                   MCLK2MCLK(0x40)
#define CAPTURE_HRESHAPE_RISE                   MCLK2MCLK(0x50)
#define CAPTURE_VRESHAPE_COUNTER_SEL            (BY_HD)
#define CAPTURE_VRESHAPE_FALL		            (CAPTURE_VOFFSET-3)
#define CAPTURE_VRESHAPE_RISE		            (CAPTURE_VOFFSET-1)

#define CAPTURE_LINETOTAL_CALIB                 MCLK2MCLK(0x2200)
#define CAPTURE_LINEBLANK_CALIB                 (6)
#define CAPTURE_HOFFSET_CALIB                   MCLK2MCLK(0x48)
#define CAPTURE_HRESHAPE_FALL_CALIB             MCLK2MCLK(0x40)
#define CAPTURE_HRESHAPE_RISE_CALIB             MCLK2MCLK(0x50)
/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV     1        /* mclk drivng, '1' : 8mA, '0' : 4mA */
#define MCLKMODE	1	     /*mclk mode,'0'from I/O Pad,'1'from internal*/
#define PCLKMODE	2	     /*pclk mode,'0'from I/O Pad,'1'from internal*/
#define TGPLLEN   	1	     /*enable TGPLL,'0'disable,'1'enable*/
#define TGPLLCLKSET	8	     /*TGPLL clk 0/192MHz,1/144MHz,  2/135MHz, 3/126MHz,4/120MHz,
					                     5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz, 9/90MHz,  10/72MHz*/
#define MCLKDIV		4	     /*MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV		2	     /*pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE	0     	 /*master clk phase*/
#define PIXELPHASE	3	     /*pixel clk phase*/
#define ADCKPHASE	0	     /*adck clk phase*/

#define	PCLKNXSEL_INT	0x00
#define	PCLKNXSEL_LVDS	0x01
#define	PCLKNXSEL_EXT	0x02

#define	PCLK1XSEL_NX	0x00
#define	PCLK1XSEL_NX_2	0x04
#define	PCLK1XSEL_INT	0x08
/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	          /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                        TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_SLAVE    /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						                        TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_VGA_CFAPATTERN	  BAYER_GRBG  /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_1080P_CFAPATTERN  BAYER_GRBG
#define	SNAP_CFAPATTERN		      BAYER_GRBG    
#define	SNAP_DUMMYVD_NUM	          0       /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	          1	      /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	          1	      /*for typical CCD==n_dummy + n_readout + 1*/
#define	SNAP_EARLY_NUM		          0       /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM	      0       /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	EXP_FRAME_NUM       	      2	      /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/

#define	FLDSEL_START_NUM	          0		  /*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		          0	 	  /*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN        0		  /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	      /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	          0		  /*sony FALSE;panasonic/ITG TRUE*/

#define MODE_SYNC_EARLY                (50)   /*define precmd position (cmdport -MODE_SYNC_EARLY) */

#define	CAPTURE_HDNUM_FRAME_START     (110)	  /*the front interrupt line number of the snap new frame */
#define	CAPTURE_HDNUM_FRAME_END	        (2)         /*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	      (CAPTURE_VOFFSET+CAPTURE_VSIZE-200)	      /*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_HDNUM_FRAME_START     (110)	  /*the front interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT	  (600)	  /*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_CMDPORT	  (600)	  /*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END	    (2)	  /*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_FRAME_END	(2)	  /*the rear interrupt line number of the preview new frame*/
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
#define	TG_SPI_I2C_MODE  0x20     	       	/*I2C slave addr for write*/
#if EVB
    #define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF | SENSOR_SERIAL_FORMAT2_SSC_SIF2_LMI/*don't change*/
#else
    #if 0
		#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF | SENSOR_SERIAL_FORMAT_SSC_SIF2
	#else
	    #define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF
	#endif
#endif
#define	TG_FORMAT3  	 0		      		/*don't change*/
#define	TG_SPEED         FRONT_SSC_CLK_192K
 											/*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
											  FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#define	TG_NBIT_REGDATA  16					/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16					/*ssc/spi register address numbers */
#define MI_REG(a,b)     {(a>>8),(a&0xFF),(b>>8),(b&0xFF)}
#define COUNT_OF(array)	(sizeof(array)/sizeof(array[0]))

extern UINT32 sensor_event_wait(UINT32 event);
extern UINT32 sensorModeGet();
extern UINT32 tmrTimeClockGet();
extern UINT32 appStillCapFlashStatusGet(void);

void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewVGA30fpsBegin();
static void sensorDoFuncPreviewVGAZoom30fpsBegin();
static void sensorDoFuncPreviewVGA60fpsBegin();
static void sensorDoFuncPreview1080pBegin();
static void sensorDoFuncPreviewFullSizeBegin();
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
static void sensorFrontRestart();
static UINT32 sensorResumeExecuteLoad();
static UINT32 sensorSuspendExecuteLoad();


void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param);
#if STREAMOF_ENABLE
void sensorDoFuncStreamOff(UINT32 abort);
void sensorDoFuncStreamOn(UINT32 abort);
#endif

static const UINT8 cmd_denoise_on[][4]={
    MI_REG(0x30EE,0x0020),
};

static const UINT8 cmd_denoise_off[][4]={
    MI_REG(0x30EE,0x0000),
};

static const UINT8 cmd_OB_output_on[][4]={
    MI_REG(0x3044,0x0F80),
};

static const UINT8 cmd_OB_output_off[][4]={
    MI_REG(0x3044,0x0580),
};

static const UINT8 cmd_LSC_on[][4]={
    MI_REG(0x3780,0x8000),
};

static const UINT8 cmd_LSC_off[][4]={
    MI_REG(0x3780,0x0000),
};

static const UINT8 cmd_stream_off[][4]={
    MI_REG(0x0100,0x0000),
};

static const UINT8 cmd_stream_on[][4]={
    MI_REG(0x0100,0x0100),
};

static const UINT8 cmd_group_on[][4]={
    MI_REG(0x0104,0x0100),
};

static const UINT8 cmd_group_off[][4]={
    MI_REG(0x0104,0x0000),
};

static const UINT8 cmd_frex_on[][4]={
    MI_REG(0x315E,0x0001),
};

static const UINT8 cmd_frex_off[][4]={
    MI_REG(0x315E,0x0000),
};

static const UINT8 cmd_pll_110[][4]={
    MI_REG(0x0306, 110),   /*0x6E*/
};

static const UINT8 cmd_pll_55[][4]={
    MI_REG(0x0306,  55),   /*0x37*/
};

static UINT16 cmd_init_1[][2] =
{/*HW RESET, OTPM write workaround*/
    {0x3140, 0x2214},
    {0x304A, 0x0010},
    {END_CMD,END_CMD}
};

static UINT16 cmd_init_2[][2] =
{/*[SW Reset]*/
    {0x0103, 0x01  },   /* SOFTWARE_RESET              */
    {DELAY_US,300  },   /* Delay 300 us                */
#if 0
    {0x0100, 0x0000},   /* [Streaming Off]             */
    {DELAY_US,500  },
    {0x0100, 0x0100},   /* [Streaming On]              */
    {DELAY_US,500  },
#endif
 /*[Hispi_init]*/
	{0x301A, 0x0010},	/* RESET_REGISTER			   */
	{END_CMD,END_CMD}
};

static UINT16 cmd_init_3[][2] SENSOR_SIF1DMA_BUF=
{
/*[default register changes] */
	{0x3064, 0x0845},
    {0x30C0, 0x0020},
    {0x3174, 0x8000},
    {0x3ED8, 0x65A5},
    {0x3EDA, 0x2525},
    {0x3EFA, 0x7444},
/*[Sequencer SVN9671] */
	{0x3D00, 0x0436},
    {0x3D02, 0x445E},
    {0x3D04, 0x6799},
    {0x3D06, 0xFFFF},
    {0x3D08, 0x7783},
    {0x3D0A, 0x101B},
    {0x3D0C, 0x732C},
    {0x3D0E, 0x4230},
    {0x3D10, 0x5881},
    {0x3D12, 0x5C3A},
    {0x3D14, 0x0140},
    {0x3D16, 0x2300},
    {0x3D18, 0x815F},
    {0x3D1A, 0x6789},
    {0x3D1C, 0x5920},
    {0x3D1E, 0x0C20},
    {0x3D20, 0x21C0},
    {0x3D22, 0x4684},
    {0x3D24, 0x4892},
    {0x3D26, 0x1A00},
    {0x3D28, 0xBA4C},
    {0x3D2A, 0x8D48},
    {0x3D2C, 0x4641},
    {0x3D2E, 0x408C},
    {0x3D30, 0x4784},
    {0x3D32, 0x4A87},
    {0x3D34, 0x561A},
    {0x3D36, 0x00A5},
    {0x3D38, 0x1A00},
    {0x3D3A, 0x5693},
    {0x3D3C, 0x4D8D},
    {0x3D3E, 0x4A47},
    {0x3D40, 0x4041},
    {0x3D42, 0x8200},
    {0x3D44, 0x2484},
    {0x3D46, 0x7BAF},
    {0x3D48, 0x0024},
    {0x3D4A, 0x8D4F},
    {0x3D4C, 0x831A},
    {0x3D4E, 0x00B4},
    {0x3D50, 0x4684},
    {0x3D52, 0x49CE},
    {0x3D54, 0x4946},
    {0x3D56, 0x4140},
    {0x3D58, 0x9247},
    {0x3D5A, 0x844B},
    {0x3D5C, 0xCE4B},
    {0x3D5E, 0x4741},
    {0x3D60, 0x502F},
    {0x3D62, 0xBD3A},
    {0x3D64, 0x5181},
    {0x3D66, 0x5E73},
    {0x3D68, 0x7C0A},
    {0x3D6A, 0x7770},
    {0x3D6C, 0x8085},
    {0x3D6E, 0x6A82},
    {0x3D70, 0x6742},
    {0x3D72, 0x8244},
    {0x3D74, 0x831A},
    {0x3D76, 0x0099},
    {0x3D78, 0x44DF},
    {0x3D7A, 0x1A00},
    {0x3D7C, 0x8542},
    {0x3D7E, 0x8567},
    {0x3D80, 0x826A},
    {0x3D82, 0x857C},
    {0x3D84, 0x6D80},
    {0x3D86, 0x7000},
    {0x3D88, 0xB831},
    {0x3D8A, 0x40BE},
    {0x3D8C, 0x6700},
    {0x3D8E, 0x0CBD},
    {0x3D90, 0x4482},
    {0x3D92, 0x7898},
    {0x3D94, 0x7480},
    {0x3D96, 0x5680},
    {0x3D98, 0x9755},
    {0x3D9A, 0x8057},
    {0x3D9C, 0x8056},
    {0x3D9E, 0x9256},
    {0x3DA0, 0x8057},
    {0x3DA2, 0x8055},
    {0x3DA4, 0x817C},
    {0x3DA6, 0x989B},
    {0x3DA8, 0x56A6},
    {0x3DAA, 0x44BE},
    {0x3DAC, 0x000C},
    {0x3DAE, 0x867A},
    {0x3DB0, 0x9474},
    {0x3DB2, 0x8A79},
    {0x3DB4, 0x9367},
    {0x3DB6, 0xBF6A},
    {0x3DB8, 0x816C},
    {0x3DBA, 0x8570},
    {0x3DBC, 0x836C},
    {0x3DBE, 0x826A},
    {0x3DC0, 0x8245},
    {0x3DC2, 0xFFFF},
    {0x3DC4, 0xFFD6},
    {0x3DC6, 0x4582},
    {0x3DC8, 0x6A82},
    {0x3DCA, 0x6C83},
    {0x3DCC, 0x7000},
    {0x3DCE, 0x8024},
    {0x3DD0, 0xB181},
    {0x3DD2, 0x6859},
    {0x3DD4, 0x732B},
    {0x3DD6, 0x4030},
    {0x3DD8, 0x4982},
    {0x3DDA, 0x101B},
    {0x3DDC, 0x4083},
    {0x3DDE, 0x6785},
    {0x3DE0, 0x3A00},
    {0x3DE2, 0x8820},
    {0x3DE4, 0x0C59},
    {0x3DE6, 0x8546},
    {0x3DE8, 0x8348},
    {0x3DEA, 0xD04C},
    {0x3DEC, 0x8B48},
    {0x3DEE, 0x4641},
    {0x3DF0, 0x4083},
    {0x3DF2, 0x1A00},
    {0x3DF4, 0x8347},
    {0x3DF6, 0x824A},
    {0x3DF8, 0x9A56},
    {0x3DFA, 0x1A00},
    {0x3DFC, 0x951A},
    {0x3DFE, 0x0056},
    {0x3E00, 0x914D},
    {0x3E02, 0x8B4A},
    {0x3E04, 0x4700},
    {0x3E06, 0x0300},
    {0x3E08, 0x2492},
    {0x3E0A, 0x0024},
    {0x3E0C, 0x8A1A},
    {0x3E0E, 0x004F},
    {0x3E10, 0xB446},
    {0x3E12, 0x8349},
    {0x3E14, 0xB249},
    {0x3E16, 0x4641},
    {0x3E18, 0x408B},
    {0x3E1A, 0x4783},
    {0x3E1C, 0x4BDB},
    {0x3E1E, 0x4B47},
    {0x3E20, 0x4180},
    {0x3E22, 0x502B},
    {0x3E24, 0x4C3A},
    {0x3E26, 0x4180},
    {0x3E28, 0x737C},
    {0x3E2A, 0xD324},
    {0x3E2C, 0x9068},
    {0x3E2E, 0x8A20},
    {0x3E30, 0x2170},
    {0x3E32, 0x8081},
    {0x3E34, 0x6A67},
    {0x3E36, 0x4257},
    {0x3E38, 0x5544},
    {0x3E3A, 0x8644},
    {0x3E3C, 0x9755},
    {0x3E3E, 0x5742},
    {0x3E40, 0x676A},
    {0x3E42, 0x807D},
    {0x3E44, 0x3380},
    {0x3E46, 0x7000},
    {0x3E48, 0x0000},
    {0x3E4A, 0x0000},
    {0x3E4C, 0x0000},
    {0x3E4E, 0x0000},
    {0x3E50, 0x0000},
    {0x3E52, 0x0000},
    {0x3E54, 0x0000},
    {0x3E56, 0x0000},
    {0x3E58, 0x0000},
    {0x3E5A, 0x0000},
    {0x3E5C, 0x0000},
    {0x3E5E, 0x0000},
    {0x3E60, 0x0000},
    {0x3E62, 0x0000},
    {0x3E64, 0x0000},
    {0x3E66, 0x0000},
    {0x3E68, 0x0000},
    {0x3E6A, 0x0000},
    {0x3E6C, 0x0000},
    {0x3E6E, 0x0000},
    {0x3E70, 0x0000},
    {0x3E72, 0x0000},
    {0x3E74, 0x0000},
    {0x3E76, 0x0000},
    {0x3E78, 0x0000},
    {0x3E7A, 0x0000},
    {0x3E7C, 0x0000},
    {0x3E7E, 0x0000},
    {0x3E80, 0x0000},
    {0x3E82, 0x0000},
    {0x3E84, 0x0000},
    {0x3E86, 0x0000},
    {0x3E88, 0x0000},
    {0x3E8A, 0x0000},
    {0x3E8C, 0x0000},
    {0x3E8E, 0x0000},
    {0x3E90, 0x0000},
    {0x3E92, 0x0000},
    {0x3E94, 0x0000},
    {0x3E96, 0x0000},
    {0x3E98, 0x0000},
    {0x3E9A, 0x0000},
    {0x3E9C, 0x0000},
    {0x3E9E, 0x0000},
    {0x3EA0, 0x0000},
    {0x3EA2, 0x0000},
    {0x3EA4, 0x0000},
    {0x3EA6, 0x0000},
    {0x3EA8, 0x0000},
    {0x3EAA, 0x0000},
    {0x3EAC, 0x0000},
    {0x3EAE, 0x0000},
    {0x3EB0, 0x0000},
    {0x3EB2, 0x0000},
    {0x3EB4, 0x0000},
    {0x3EB6, 0x0000},
    {0x3EB8, 0x0000},
    {0x3EBA, 0x0000},
    {0x3EBC, 0x0000},
    {0x3EBE, 0x0000},
    {0x3EC0, 0x0000},
    {0x3EC2, 0x0000},
    {0x3EC4, 0x0000},
    {0x3EC6, 0x0000},
    {0x3EC8, 0x0000},
    {0x3ECA, 0x0000},
};

static UINT16 cmd_init_4[][2] SENSOR_SIF1DMA_BUF=
{
/*[Register Context Definition] */
	{0x30E8, 0x0000},
	{0x30EA, 0xF873},
    {0x30EA, 0x08AA},
    {0x30EA, 0x3219},
    {0x30EA, 0x3219},
    {0x30EA, 0x3219},
    {0x30EA, 0x3200},
    {0x30EA, 0x3200},
    {0x30EA, 0x3200},
    {0x30EA, 0x3200},
    {0x30EA, 0x3200},
    {0x30EA, 0x1769},
    {0x30EA, 0xA6F3},
    {0x30EA, 0xA6F3},
    {0x30EA, 0xA6F3},
    {0x30EA, 0xA6F3},
    {0x30EA, 0xA6F3},
    {0x30EA, 0xA6F3},
    {0x30EA, 0xA6F3},
    {0x30EA, 0xAFF3},
    {0x30EA, 0xFA64},
    {0x30EA, 0xFA64},
    {0x30EA, 0xFA64},
    {0x30EA, 0xF164},
    {0x30EA, 0xFA64},
    {0x30EA, 0xFA64},
    {0x30EA, 0xFA64},
    {0x30EA, 0xF164},
    {0x30EA, 0x276E},
    {0x30EA, 0x18CF},
    {0x30EA, 0x18CF},
    {0x30EA, 0x18CF},
    {0x30EA, 0x28CF},
    {0x30EA, 0x18CF},
    {0x30EA, 0x18CF},
    {0x30EA, 0x18CF},
    {0x30EA, 0x18CF},
    {0x30EA, 0x2363},
    {0x30EA, 0x2363},
    {0x30EA, 0x2352},
    {0x30EA, 0x2363},
    {0x30EA, 0x2363},
    {0x30EA, 0x2363},
    {0x30EA, 0x2352},
    {0x30EA, 0x2352},
    {0x30EA, 0xA394},
    {0x30EA, 0xA394},
    {0x30EA, 0x8F8F},
    {0x30EA, 0xA3D4},
    {0x30EA, 0xA394},
    {0x30EA, 0xA394},
    {0x30EA, 0x8F8F},
    {0x30EA, 0x8FCF},
    {0x30EA, 0xDC23},
    {0x30EA, 0xDC63},
    {0x30EA, 0xDC63},
    {0x30EA, 0xDC23},
    {0x30EA, 0xDC23},
    {0x30EA, 0xDC63},
    {0x30EA, 0xDC63},
    {0x30EA, 0xDC23},
    {0x30EA, 0x0F73},
    {0x30EA, 0x85C0},
    {0x30EA, 0x85C0},
    {0x30EA, 0x85C0},
    {0x30EA, 0x85C0},
    {0x30EA, 0x85C0},
    {0x30EA, 0x85C0},
    {0x30EA, 0x85C0},
    {0x30EA, 0x85C4},
    {0x30EA, 0x0000},
/* Load = LSC setting... */
    /* [Lens Correction 80% 06/13/12 17:47:05] */
	{0x3600, 0x0BD0},	/* P_GR_P0Q0       */
    {0x3602, 0x844D},	/* P_GR_P0Q1       */
    {0x3604, 0x49EF},	/* P_GR_P0Q2       */
    {0x3606, 0x156D},	/* P_GR_P0Q3       */
    {0x3608, 0xD211},	/* P_GR_P0Q4       */
    {0x360A, 0x0A90},	/* P_RD_P0Q0       */
    {0x360C, 0xBC08},	/* P_RD_P0Q1       */
    {0x360E, 0x38AF},	/* P_RD_P0Q2       */
    {0x3610, 0x6A8C},	/* P_RD_P0Q3       */
    {0x3612, 0xADB1},	/* P_RD_P0Q4       */
    {0x3614, 0x0E70},	/* P_BL_P0Q0       */
    {0x3616, 0xA4CD},	/* P_BL_P0Q1       */
    {0x3618, 0x2890},	/* P_BL_P0Q2       */
    {0x361A, 0x268D},	/* P_BL_P0Q3       */
    {0x361C, 0x9512},	/* P_BL_P0Q4       */
    {0x361E, 0x0F70},	/* P_GB_P0Q0       */
    {0x3620, 0x87AA},	/* P_GB_P0Q1       */
    {0x3622, 0x774F},	/* P_GB_P0Q2       */
    {0x3624, 0x140D},	/* P_GB_P0Q3       */
    {0x3626, 0xE251},	/* P_GB_P0Q4       */
    {0x3640, 0x234C},	/* P_GR_P1Q0       */
    {0x3642, 0xB34C},	/* P_GR_P1Q1       */
    {0x3644, 0x1AD0},	/* P_GR_P1Q2       */
    {0x3646, 0x2D8E},	/* P_GR_P1Q3       */
    {0x3648, 0xA192},	/* P_GR_P1Q4       */
    {0x364A, 0x17CC},	/* P_RD_P1Q0       */
    {0x364C, 0x1C8A},	/* P_RD_P1Q1       */
    {0x364E, 0x05D0},	/* P_RD_P1Q2       */
    {0x3650, 0x2C6A},	/* P_RD_P1Q3       */
    {0x3652, 0x8972},	/* P_RD_P1Q4       */
    {0x3654, 0xA68B},	/* P_BL_P1Q0       */
    {0x3656, 0x894C},	/* P_BL_P1Q1       */
    {0x3658, 0x56B0},	/* P_BL_P1Q2       */
    {0x365A, 0x4BEE},	/* P_BL_P1Q3       */
    {0x365C, 0xD9F2},	/* P_BL_P1Q4       */
    {0x365E, 0xB9CB},	/* P_GB_P1Q0       */
    {0x3660, 0xFD2A},	/* P_GB_P1Q1       */
    {0x3662, 0x42B0},	/* P_GB_P1Q2       */
    {0x3664, 0xCD4A},	/* P_GB_P1Q3       */
    {0x3666, 0xC5D2},	/* P_GB_P1Q4       */
    {0x3680, 0x3910},	/* P_GR_P2Q0       */
    {0x3682, 0x4D8F},	/* P_GR_P2Q1       */
    {0x3684, 0xBC14},	/* P_GR_P2Q2       */
    {0x3686, 0x97F2},	/* P_GR_P2Q3       */
    {0x3688, 0x2E76},	/* P_GR_P2Q4       */
    {0x368A, 0x1710},	/* P_RD_P2Q0       */
    {0x368C, 0x678F},	/* P_RD_P2Q1       */
    {0x368E, 0x9AF4},	/* P_RD_P2Q2       */
    {0x3690, 0xA0B2},	/* P_RD_P2Q3       */
    {0x3692, 0x0DD6},	/* P_RD_P2Q4       */
    {0x3694, 0x0C11},	/* P_BL_P2Q0       */
    {0x3696, 0x28EF},	/* P_BL_P2Q1       */
    {0x3698, 0x8915},	/* P_BL_P2Q2       */
    {0x369A, 0xE2D1},	/* P_BL_P2Q3       */
    {0x369C, 0x7C36},	/* P_BL_P2Q4       */
    {0x369E, 0x4810},	/* P_GB_P2Q0       */
    {0x36A0, 0x250F},	/* P_GB_P2Q1       */
    {0x36A2, 0xD294},	/* P_GB_P2Q2       */
    {0x36A4, 0xF9D1},	/* P_GB_P2Q3       */
    {0x36A6, 0x4156},	/* P_GB_P2Q4       */
    {0x36C0, 0x5FAE},	/* P_GR_P3Q0       */
    {0x36C2, 0x6B2E},	/* P_GR_P3Q1       */
    {0x36C4, 0xAD53},	/* P_GR_P3Q2       */
    {0x36C6, 0x90F0},	/* P_GR_P3Q3       */
    {0x36C8, 0x2875},	/* P_GR_P3Q4       */
    {0x36CA, 0x500E},	/* P_RD_P3Q0       */
    {0x36CC, 0x540C},	/* P_RD_P3Q1       */
    {0x36CE, 0x9893},	/* P_RD_P3Q2       */
    {0x36D0, 0x71CF},	/* P_RD_P3Q3       */
    {0x36D2, 0x13D5},	/* P_RD_P3Q4       */
    {0x36D4, 0x578F},	/* P_BL_P3Q0       */
    {0x36D6, 0x780E},	/* P_BL_P3Q1       */
    {0x36D8, 0x8894},	/* P_BL_P3Q2       */
    {0x36DA, 0xF8CF},	/* P_BL_P3Q3       */
    {0x36DC, 0x0316},	/* P_BL_P3Q4       */
    {0x36DE, 0x21CF},	/* P_GB_P3Q0       */
    {0x36E0, 0x65AA},	/* P_GB_P3Q1       */
    {0x36E2, 0xE013},	/* P_GB_P3Q2       */
    {0x36E4, 0x7330},	/* P_GB_P3Q3       */
    {0x36E6, 0x5B75},	/* P_GB_P3Q4       */
    {0x3700, 0x8D73},	/* P_GR_P4Q0       */
    {0x3702, 0xB9D1},	/* P_GR_P4Q1       */
    {0x3704, 0x7C36},	/* P_GR_P4Q2       */
    {0x3706, 0x35F4},	/* P_GR_P4Q3       */
    {0x3708, 0xE8F8},	/* P_GR_P4Q4       */
    {0x370A, 0xD812},	/* P_RD_P4Q0       */
    {0x370C, 0xC811},	/* P_RD_P4Q1       */
    {0x370E, 0x4D76},	/* P_RD_P4Q2       */
    {0x3710, 0x2814},	/* P_RD_P4Q3       */
    {0x3712, 0xBC78},	/* P_RD_P4Q4       */
    {0x3714, 0xC7F3},	/* P_BL_P4Q0       */
    {0x3716, 0x9031},	/* P_BL_P4Q1       */
    {0x3718, 0x3BB7},	/* P_BL_P4Q2       */
    {0x371A, 0x0AF4},	/* P_BL_P4Q3       */
    {0x371C, 0xAD59},	/* P_BL_P4Q4       */
    {0x371E, 0x9AF3},	/* P_GB_P4Q0       */
    {0x3720, 0xF9D0},	/* P_GB_P4Q1       */
    {0x3722, 0x1017},	/* P_GB_P4Q2       */
    {0x3724, 0x7B93},	/* P_GB_P4Q3       */
    {0x3726, 0x8459},	/* P_GB_P4Q4       */
    {0x3782, 0x0924},	/* POLY_ORIGIN_C   */
    {0x3784, 0x062C},	/* POLY_ORIGIN_R   */
    {0x37C0, 0x0000},	/* P_GR_Q5         */
    {0x37C2, 0x0000},	/* P_RD_Q5         */
    {0x37C4, 0x0000},	/* P_BL_Q5         */
    {0x37C6, 0x0000},	/* P_GB_Q5         */
    {0x3780, 0x8000},   /* turn on shading correction         */
/* End-Load LSC setting... */
    {0x301A, 0x0010},   /* RESET_REGISTER              */
    {0x305E, 0x1430},   /* GLOBAL_GAIN                 */
    {0x31AE, 0x0304},   /* SERIAL_FORMAT               */
    {0x3064, 0x0845},   /* RESERVED_MFR_3064           */
#if 0
    {0x31C6, 0x048A},   /*Config HiSPi Interface to Streaming  Packed 12bit */
#else
    {0x31C6, 0x0402},
#endif

    {0x301E, 0x00A8},   /* DATA_PEDESTAL_              */
    {0x3EDA, 0xE525},   /* @@add             */

#if 1
    {0x0300, 4     },   /* VT_PIX_CLK_DIV			*/
    {0x0302, 1     },   /* VT_SYS_CLK_DIV			*/
    {0x0304, 6     },   /* PRE_PLL_CLK_DIV			*/
    {0x0306, 110   },   /* PLL_MULTIPLIER			*/
    {0x0308, 12    },   /* OP_PIX_CLK_DIV			*/
    {0x030A, 1     },   /* OP_SYS_CLK_DIV			*/

    {0x3064, 0x0845},
    {0x0112, 0x0C0C},   /* ccp_data_format          */

    #if CAP_FRAME_MODE_BY_GPIO
        /*config GPI2 as FREX*/
        {0x301A, 0x0118},         /* bit8:gpi_en */
        {0x3026, 0xFD7F},         /* Associate the trigger function with an active-high input pin: GPI2*/
        {0x315E, 0x0002},         /* global bulb enabled */
    #else
        /*default: 0x301A = 0x0014 */
        {0x301A, 0x0018},/*sync EOF*/
    #endif
#endif
    {END_CMD,END_CMD}
};

static UINT8 cmd_vga_30fps_code[][4] SENSOR_SIF1DMA_BUF =
{
    /*  A14041_Preset_for_iCatch_With_OTPM_check_20120508.ini*/
    /* [Preview-1096x822@60fps] drop to 30fps by PLL_Multiple = 110/2 */
    /*  MI_REG(0x301A, 0x0018),*/
    MI_REG(0x0340, 901*2 ),    /* FRAME_LENGTH_LINES:901	    */
    MI_REG(0x0342, 2034  ),    /* LINE_LENGTH_PCK           */
    MI_REG(0x034C, 1096  ),    /* X_OUTPUT_SIZE		        */
    MI_REG(0x034E, 822   ),    /* Y_OUTPUT_SIZE		        */
    MI_REG(0x0400, 0x0000),
    MI_REG(0x3002, 32    ),
    MI_REG(0x3004, 144   ),
    MI_REG(0x3006, 3313  ),
    MI_REG(0x3008, 4521  ),
    MI_REG(0x3010, 98    ),    /* FINE_CORRECTION 			*/
    MI_REG(0x3014, 570   ),    /* FINE_INTEGRATION_TIME_	*/
    /*MI_REG(0x3012, 901 ),*/  /* COARSE_INTEGRATION_TIME_	*/
    MI_REG(0x3016, 0x0112),    /* ROW_SPEED				    */
    MI_REG(0x3040, 0x09C7),    /* READ_MODE				    */
    MI_REG(0x306E, 0x9080),
    MI_REG(0x30D4, 0x9080),
    MI_REG(0x3176, 0x8000),
    MI_REG(0x3178, 0x0A00),
    MI_REG(0x317C, 0xA00A),
    MI_REG(0x30E8, 0x8005),
    MI_REG(0x3EE4, 0x6309),    /* Larry-06.14.2012 12bit ADC*/
    MI_REG(0x3EE8, 0x0647),
    MI_REG(0x3EEA, 0x15F0),
    /*MI_REG(0x301A, 0x0008),*/
};
static UINT8 cmd_vga_zoom_30fps_code[][4] SENSOR_SIF1DMA_BUF =
{
    /*A14041_Preset_for_iCatch_With_OTPM_check_20120718.txt*/
    /* [Preview-1096x822@30fps] crop for zoom */
    /*MI_REG(0x301A, 0x0018),*/
    MI_REG(0x0340, 901  ),    /* FRAME_LENGTH_LINES:901	    */
    MI_REG(0x0342, 4062  ),    /* LINE_LENGTH_PCK           */
    MI_REG(0x034C, 1096  ),    /* X_OUTPUT_SIZE		        */
    MI_REG(0x034E, 822   ),    /* Y_OUTPUT_SIZE		        */
    MI_REG(0x0400, 0x0000),
    MI_REG(0x3002, 852   ),
    MI_REG(0x3004, 1238  ),
    MI_REG(0x3006, 2493  ),
    MI_REG(0x3008, 3427  ),
    MI_REG(0x3010, 252	 ),    /* FINE_CORRECTION 			*/
    MI_REG(0x3014, 1466  ),    /* FINE_INTEGRATION_TIME_	*/
    /*MI_REG(0x3012, 901 ),*/  /* COARSE_INTEGRATION_TIME_	*/
    MI_REG(0x3016, 0x0112),    /* ROW_SPEED				    */
    MI_REG(0x3040, 0x04C3),    /* READ_MODE				    */
    MI_REG(0x306E, 0x9080),
    MI_REG(0x30D4, 0x9100),
    MI_REG(0x3176, 0x8000),
    MI_REG(0x3178, 0x0000),
    MI_REG(0x317C, 0xA00A),
    MI_REG(0x30E8, 0x8005),
    MI_REG(0x3EE4, 0x6309),    /* Larry-06.14.2012 12bit ADC*/
    MI_REG(0x3EE8, 0x0047),
    MI_REG(0x3EEA, 0x15F0),
    /*MI_REG(0x301A, 0x0008),*/
};



static UINT8 cmd_vga_60fps_code[][4] SENSOR_SIF1DMA_BUF =
{
    /*A14041_Preset_for_iCatch_With_OTPM_check_20120508.ini*/
    /* [Preview-1096x822@60fps] */
    /*MI_REG(0x301A, 0x0018),*/
    MI_REG(0x0340, 901  ),    /* FRAME_LENGTH_LINES:901	    */
    MI_REG(0x0342, 2034  ),    /* LINE_LENGTH_PCK           */
    MI_REG(0x034C, 1096  ),    /* X_OUTPUT_SIZE		        */
    MI_REG(0x034E, 822   ),    /* Y_OUTPUT_SIZE		        */
    MI_REG(0x0400, 0x0000),
    MI_REG(0x3002, 32    ),
    MI_REG(0x3004, 144   ),
    MI_REG(0x3006, 3313  ),
    MI_REG(0x3008, 4521  ),
    MI_REG(0x3010, 98    ),    /* FINE_CORRECTION 			*/
    MI_REG(0x3014, 570   ),    /* FINE_INTEGRATION_TIME_	*/
    /*MI_REG(0x3012, 901 ),*/  /* COARSE_INTEGRATION_TIME_	*/
    MI_REG(0x3016, 0x0112),    /* ROW_SPEED				    */
    MI_REG(0x3040, 0x09C7),    /* READ_MODE				    */
    MI_REG(0x306E, 0x9080),
    MI_REG(0x30D4, 0x9080),
    MI_REG(0x3176, 0x8000),
    MI_REG(0x3178, 0x0A00),
    MI_REG(0x317C, 0xA00A),
    MI_REG(0x30E8, 0x8007),
    MI_REG(0x3EE4, 0x6309),    /* Larry-06.14.2012 12bit ADC*/
    MI_REG(0x3EE8, 0x0647),
    MI_REG(0x3EEA, 0x15F0),
    /*MI_REG(0x301A, 0x0008),*/
};

static UINT8 cmd_1080p_code[][4] SENSOR_SIF1DMA_BUF =
{
	/*A14041_Preset_for_iCatch_With_OTPM_check_20120508.ini*/
    /* [FHD:2304x1296@30fps] */
    /*MI_REG(0x301A, 0x0018),*/
    MI_REG(0x0340, 1375  ),    /* FRAME_LENGTH_LINES   	    */
    MI_REG(0x0342, 5334  ),    /* LINE_LENGTH_PCK           */
    MI_REG(0x034C, 2304  ),    /* X_OUTPUT_SIZE		        */
    MI_REG(0x034E, 1296  ),    /* Y_OUTPUT_SIZE		        */
    MI_REG(0x0400, 0x0000),
    MI_REG(0x3002, 380   ),
    MI_REG(0x3004, 32    ),
    MI_REG(0x3006, 2969  ),
    MI_REG(0x3008, 4637  ),
    MI_REG(0x3010, 516   ),    /* FINE_CORRECTION 			*/
    MI_REG(0x3014, 2442  ),    /* FINE_INTEGRATION_TIME_	*/
    /*MI_REG(0x3012, 1375),*/  /* COARSE_INTEGRATION_TIME_	*/
    MI_REG(0x3016, 0x0111),    /* ROW_SPEED				    */
    MI_REG(0x3040, 0x04C3),    /* READ_MODE				    */
    MI_REG(0x306E, 0x9080),
    MI_REG(0x30D4, 0x9100),
    MI_REG(0x3176, 0x8000),
    MI_REG(0x3178, 0x0000),
    MI_REG(0x317C, 0x800A),
	MI_REG(0x30E8, 0x8005),
    MI_REG(0x3EE4, 0x6309),
    MI_REG(0x3EE8, 0x0047),
    MI_REG(0x3EEA, 0x15F0),
    /*MI_REG(0x301A, 0x0008),*/
};

static UINT8 cmd_still_code[][4] SENSOR_SIF1DMA_BUF =
{
	/*A14041_Preset_for_iCatch_With_OTPM_check_20120508.ini*/
	/* [Altek-14M STILL @ 9.5fps] */
    /*MI_REG(0x301A, 0x0018),*/
    MI_REG(0x0340, 3431  ),   /* FRAME_LENGTH_LINES   	   */
    MI_REG(0x0342, 6720  ),   /* LINE_LENGTH_PCK           */
    MI_REG(0x034C, 4384  ),   /* X_OUTPUT_SIZE		       */
    MI_REG(0x034E, 3288  ),   /* Y_OUTPUT_SIZE		       */
    MI_REG(0x0400, 0x0000),
    MI_REG(0x3002, 32    ),
    MI_REG(0x3004, 144   ),
    MI_REG(0x3006, 3319  ),
    MI_REG(0x3008, 4527  ),
    MI_REG(0x3010, 234   ),   /* FINE_CORRECTION - Larry 06.14.2012 */
    MI_REG(0x3014, 2658  ),   /* FINE_INTEGRATION_TIME_	   */
    /*MI_REG(0x3012, 1000),*/ /* COARSE_INTEGRATION_TIME_  */
    MI_REG(0x3016, 0x0111),   /* ROW_SPEED				   */
    MI_REG(0x3040, 0x0041),   /* READ_MODE				   */
    MI_REG(0x306E, 0x9080),
    MI_REG(0x30D4, 0xB080),
#if 0
    /*for Rolling mode*/
    MI_REG(0x3176, 0x8000),
#else
    /*Aptina-Larry 20120606 for Global Reset*/
    MI_REG(0x3176, 0x9000),   /* ANALOG_CONTROL4           */
#endif
    MI_REG(0x3178, 0x0000),
    MI_REG(0x317C, 0xA00A),
    MI_REG(0x30E8, 0x8001),
    MI_REG(0x3EE4, 0x6309),
    MI_REG(0x3EE8, 0x0047),
    MI_REG(0x3EEA, 0x1580),
    /*MI_REG(0x301A, 0x0008),*/
};

static UINT8 cmd_still_code_calib[][4] SENSOR_SIF1DMA_BUF =
{
	/*A14041_Preset_for_iCatch_With_OTPM_check_20120508.ini*/
	/* [Entire Array STILL @ 9.0fps] */
    /*MI_REG(0x301A, 0x0018),*/
    MI_REG(0x0340, 3431  ),   /* FRAME_LENGTH_LINES   	   */
    MI_REG(0x0342, 7056  ),   /* LINE_LENGTH_PCK           */
    MI_REG(0x034C, 4608  ),   /* X_OUTPUT_SIZE		       */
    MI_REG(0x034E, 3288  ),   /* Y_OUTPUT_SIZE		       */
    MI_REG(0x0400, 0x0000),
    MI_REG(0x3002, 32    ),
    MI_REG(0x3004, 32   ),
    MI_REG(0x3006, 3319  ),
    MI_REG(0x3008, 4639  ),
    MI_REG(0x3010, 208   ),   /* FINE_CORRECTION 		   */
    MI_REG(0x3014, 1314  ),   /* FINE_INTEGRATION_TIME_	   */
    /*MI_REG(0x3012, 1000),*/ /* COARSE_INTEGRATION_TIME_  */
    MI_REG(0x3016, 0x0111),   /* ROW_SPEED				   */
    MI_REG(0x3040, 0x0041),   /* READ_MODE				   */
    MI_REG(0x306E, 0x9080),
    MI_REG(0x30D4, 0xB080),
    MI_REG(0x3176, 0x8000),
    MI_REG(0x3178, 0x0000),
    MI_REG(0x317C, 0xA00A),
    MI_REG(0x30E8, 0x8001),
    MI_REG(0x3EE4, 0x6309),
    MI_REG(0x3EE8, 0x0047),
    MI_REG(0x3EEA, 0x1580),
    /*MI_REG(0x301A, 0x0008),*/
};


/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorHiSpiHDCfg =
{
	.wordcntsel 	 =1 ,		/*Hispi hsize from 0x90a4*/
	.Lvdsmode		 =1 ,	    /*12bit mode*/
	.Lvdslanenum	 =0 ,	    /*4 lane mode*/
	.Lvdslanemode	 =0 ,	    /*enable lane3 in 4 lanes mode*/
	.Lvdslaneswap	 =0 ,	    /*MSB first */
	.Lvdsraw		 =0 ,	    /*raw 10 */
	.Lvdseofsel 	 =1 ,	    /*detect eof sync code*/
	.Lvdseolsel 	 =0 ,	    /*detect eol sync code*/
	.lvdsvdsrc		 =1 ,	    /*1: decode emedded data header sof as vd source*/
	.lvds_dphy_sel	 =1 ,	    /*MIPI D-PHY*/
	.Ctrlword_sol	 =0x800,
	.Ctrlword_sof	 =0xc00 ,   /* Hd  mode = 0xc00 full resoultion mode =0xc80*/
	.Ctrlword_eol	 =0xa00 ,
	.Ctrlword_eof	 =0xe00 ,
	.syncwrod0		 =0 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	/*HW signal spec */
	.vmode			 =0 ,
	.S2pphase		 =0 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	/*Extention Part */
	.Exthd_num		 =14,
	.Exthd_pol		 =0,
	.Exthd_mode 	 =0,
        .cropenable 	 =1 ,
};

static const sensorLVDSCfg_t sensorHiSpiFullCfg =
{
	.wordcntsel 	 =1 ,		/*Hispi hsize from 0x90a4*/
	.Lvdsmode		 =1 ,	    /*12bit mode*/
	.Lvdslanenum	 =0 ,	    /*4 lane mode*/
	.Lvdslanemode	 =0 ,	    /*enable lane3 in 4 lanes mode*/
	.Lvdslaneswap	 =0 ,	    /*MSB first */
	.Lvdsraw		 =0 ,	    /*raw 10 */
	.Lvdseofsel 	 =1 ,	    /*detect eof sync code*/
	.Lvdseolsel 	 =0 ,	    /*detect eol sync code*/
	.lvdsvdsrc		 =1 ,	    /*1: decode emedded data header sof as vd source*/
	.lvds_dphy_sel	 =1 ,	    /*MIPI D-PHY*/
	.Ctrlword_sol	 =0x800,
    .Ctrlword_sof    =0xc00,    /* Hd  mode = 0xc00 full resoultion mode =0xc80*/
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
	.Exthd_num		 =14,
	.Exthd_pol		 =0,
	.Exthd_mode 	 =0,
        .cropenable 	 =1,
};

/*sensor cfg*/
static const sensorTgPreviewCfg_t sensorTgPreviewVGA30fpsCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGA30fpsBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &(const sensorTgCrop_t) {
        .xOffset = 0,
	    .yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE_30FPS,
	    .ySize = PREVIEW_VGA_VSIZE_30FPS,
    },
 	.hsize = PREVIEW_VGA_HSIZE_30FPS + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_VGA_VSIZE_30FPS,
	.sensor_model = SENSORMODEL,
   	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
    .hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 30000000,
	.sensor_pclk = 120000000,
	.sensor_htotal = 4144,
	.pix_100khz =1485, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGA60fpsCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGA60fpsBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &(const sensorTgCrop_t) {
        .xOffset = 0,
	    .yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE_60FPS,
	    .ySize = PREVIEW_VGA_VSIZE_60FPS,
    },
	.hsize = PREVIEW_VGA_HSIZE_60FPS + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_VGA_VSIZE_60FPS,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
    .hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 60000000,
	.sensor_pclk = 120000000,
	.sensor_htotal = 4144,
	.pix_100khz =1485, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGAZoom30fpsCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGAZoom30fpsBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &(const sensorTgCrop_t) {
        .xOffset = 0,
	    .yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE_60FPS,
	    .ySize = PREVIEW_VGA_VSIZE_60FPS,
    },
	.hsize = PREVIEW_VGA_HSIZE_60FPS + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_VGA_VSIZE_60FPS,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
    .hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 30000000,
	.sensor_pclk = 120000000,
	.sensor_htotal = 4144,
	.pix_100khz =1485, 
};


static const sensorTgPreviewCfg_t sensorTgPreview1080pCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080pBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &(const sensorTgCrop_t) {
        .xOffset = 0,
	    .yOffset = 0,
		.xSize = PREVIEW_1080P_HSIZE,
	    .ySize = PREVIEW_1080P_VSIZE,
    },
	.hsize = PREVIEW_1080P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio = 3200,/*16:9 sensor*/
	.vratio = 1800,
#endif
	.fps_mhz = 30000000,
	.sensor_pclk = 120000000,
	.sensor_htotal = 4144,
	.pix_100khz =1485, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewFullSizeCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewFullSizeBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &(const sensorTgCrop_t) {
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
	.fps_mhz = 9500000,
	.sensor_pclk = 120000000,
	.sensor_htotal = 4144,
	.pix_100khz =1485, 
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	&sensorTgPreviewVGA30fpsCfg,    /*  1096x822   @30fps*/
	&sensorTgPreviewVGA60fpsCfg,    /*  1096x822   @60fps  */
    &sensorTgPreview1080pCfg,       /*  2304x1296  @30fps  */
	&sensorTgPreviewVGAZoom30fpsCfg,/*  1096x822   @30fps*/
    &sensorTgPreviewFullSizeCfg,    /*  4384x3288  @9.5fps */
};

static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
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
		.n_snap_early = SNAP_EARLY_NUM,
		.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
		.fldsel_start = FLDSEL_START_NUM,
		.fldsel_end = FLDSEL_END_NUM,
		.pix_100khz =1485, 
};

static const sensorTgSnapCfg_t sensorTgSnapRollCalibCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
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
        .hsize = CAPTURE_HSIZE_CALIB,
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
		.pix_100khz =1485, 
};


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
	.pix_100khz =1485, 
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg,
    &sensorTgSnapFrameCfg,
    &sensorTgSnapRollCalibCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVGA30fpsCfg = {
	.linetotal       = PREVIEW_VGA_30FPS_LINETOTAL,
	.lineblank       = PREVIEW_VGA_30FPS_LINEBLANK,
	.frametotal      = PREVIEW_VGA_30FPS_FRAMETOTAL,
	.frameblank      = PREVIEW_VGA_30FPS_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_VGA_30FPS_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_VGA_30FPS_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_VGA_30FPS_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_VGA_30FPS_VRESHAPE_RISE,
	.hoffset         = PREVIEW_VGA_30FPS_HOFFSET,
	.voffset         = PREVIEW_VGA_30FPS_VOFFSET,
	.hsize           = PREVIEW_VGA_HSIZE_30FPS + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = PREVIEW_VGA_VSIZE_30FPS,
	.lastlinepix     = PREVIEW_VGA_30FPS_LINETOTAL + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_30FPS_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end   = PREVIEW_VGA_HDNUM_FRAME_END,
	.hdnum_cmdport     = PREVIEW_VGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVGA60fpsCfg = {
	.linetotal       = PREVIEW_VGA_60FPS_LINETOTAL,
	.lineblank       = PREVIEW_VGA_60FPS_LINEBLANK,
	.frametotal      = PREVIEW_VGA_60FPS_FRAMETOTAL,
	.frameblank      = PREVIEW_VGA_60FPS_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_VGA_60FPS_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_VGA_60FPS_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_VGA_60FPS_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_VGA_60FPS_VRESHAPE_RISE,
	.hoffset         = PREVIEW_VGA_60FPS_HOFFSET,
	.voffset         = PREVIEW_VGA_60FPS_VOFFSET,
	.hsize           = PREVIEW_VGA_HSIZE_60FPS + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = PREVIEW_VGA_VSIZE_60FPS,
	.lastlinepix     = PREVIEW_VGA_60FPS_LINETOTAL + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_60FPS_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end   = PREVIEW_VGA_HDNUM_FRAME_END,
	.hdnum_cmdport     = PREVIEW_VGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080pCfg = {
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
	.lastlinepix     = PREVIEW_1080P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
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
	.hdnum_frame_end   = PREVIEW_1080P_HDNUM_FRAME_END,
	.hdnum_cmdport     = PREVIEW_1080P_HDNUM_CMDPORT,
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
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end   = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport     = CAPTURE_HDNUM_CMDPORT,
};

static  sensorSyncSigCfg_t sensorSyncSigSnapCalibCfg = {
	.linetotal       = CAPTURE_LINETOTAL_CALIB,
	.lineblank       = CAPTURE_LINEBLANK_CALIB,
	.frametotal      = CAPTURE_FRAMETOTAL,
	.frameblank      = CAPTURE_FRAMEBLANK,
	.hreshape_fall   = CAPTURE_HRESHAPE_FALL_CALIB,
	.hreshape_rise   = CAPTURE_HRESHAPE_RISE_CALIB,
	.vreshape_fall   = CAPTURE_VRESHAPE_FALL,
	.vreshape_rise   = CAPTURE_VRESHAPE_RISE,
	.hoffset         = CAPTURE_HOFFSET_CALIB,
	.voffset         = CAPTURE_VOFFSET,
	.hsize           = CAPTURE_HSIZE_CALIB,
	.vsize           = CAPTURE_VSIZE,
	.lastlinepix     = CAPTURE_HSIZE_CALIB,
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
	.hdnum_frame_end   = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport     = CAPTURE_HDNUM_CMDPORT,
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
		.stop = sensorDoFuncStop,
		.pwrsave = NULL,
		.expwrite = sensor_cmdport_exp_write,
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
		.crop = NULL,
#if STREAMOF_ENABLE
		.streamoff = sensorDoFuncStreamOff,
		.streamon = sensorDoFuncStreamOn,
#endif
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreviewVGA30fpsCfg,
	.verStr = MODEL_VERSION,
};

void
sensorMT_SingleWrite(UINT16 addr,UINT16 data)
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

void
sensorMT_BatchWrite(UINT16 *pdata)
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
			tmrUsWait(pdata[i*2+1]);
		else
		{
			regdata[0]=pdata[i*2]>>8;
			regdata[1]=pdata[i*2]&0xFF;
			regdata[2]=pdata[i*2+1]>>8;
			regdata[3]=pdata[i*2+1]&0xFF;
            #if 0
				SENSOR_PROF_LOG_PRINT(1,"[%x %x %x %x]",regdata[0],regdata[1],regdata[2],regdata[3]);
            #endif
         	sensorSerialBulkWrite(cmdportid,1,regdata,0);
		}
	}
}

UINT16
sensorMT_Read(UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[4];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	sensorSerialBlockRead(cmdportid,1,regread,0);
	return ((regread[2]<<8)|regread[3]);
}

void
sensorMT_Write(UINT16 addr,UINT16 data)
{
	UINT32 cmdportid;
	UINT8 regread[4];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	regread[2]=data>>8;
	regread[3]=data&0xFF;
	sensorSerialBulkWrite(cmdportid,1,regread,0);
}


void
sensorMT_HRST(void)
{
    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0);            /* RESET Low*/
	tmrUsWait(10000); /*10ms*/
    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
}

void
sensorMT_SRST(void)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;

    sensorMT_BatchWrite(cmd_init_1[0]);   /*HW RESET, OTPM write workaround*/
	tmrUsWait(10000);
    sensorMT_BatchWrite(cmd_init_2[0]);   /*[SW Reset][Hispi_init]*/
    OTPM = (sensorMT_Read(0x3ECC)>>8)&0xF0;
    SENSOR_PROF_LOG_PRINT(1, "OTPM ver:%x", OTPM);
    if(OTPM!=0x30){
        sensorMT_BatchWrite(cmd_init_3[0]);
        SENSOR_PROF_LOG_PRINT(1, "Default Register Changes");
        SENSOR_PROF_LOG_PRINT(1, "the Sequencer SVN9671 preset");
    }
    sensorMT_BatchWrite(cmd_init_4[0]);
    sensorSerialBulkWrite(cmdportid,COUNT_OF(cmd_vga_30fps_code), cmd_vga_30fps_code[0],0);
    sensorSerialBulkWrite(cmdportid,COUNT_OF(cmd_stream_on), cmd_stream_on[0],0);
}

void
sensorTg_mt9F002_Install()
{
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
    profLogAdd(0, MODEL_VERSION);
}

static void
sensorDoFuncInit()
{
	UINT32 cmdportid;

    SENSOR_PROF_LOG_PRINT(1, "________Sensor driver build time <<%s>> \n", BUILD_TIME);
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncInit() 9F002 +++");

    /* TGGPIO configuration here */
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

    /*I2C S2Key*/
    WRITE16(0xb0009030, READ16(0xb0009030) |=    0x000C);
    WRITE16(0xb0009032, READ16(0xb0009032) &=   ~0x000C);

    /*LMIpullen*/
    WRITE16(0xb0000100, 0xFCFF);/*LMI9, LMI8*/

    /*TG PLL=MCLK out=24MHz*/
	sensorTgPllClkExtSet(24000000);/*24MHz*/

    /* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
    /*****TODO*****front reset after these???*/

    #if EVB
	/*halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121000);*/ /* 121 MHz  CDSP 243MHz*/
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,150000);     /* 150 MHz  CDSP 300MHz*/
    #else
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,148500);     /* 148.5 MHz  CDSP 297MHz*/
    #endif

#if EVB
    /*EVB*/
	sp5kGpioCfgSet(SENOR_GPIO_GRP,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);
#else
    /*Real-Kit*/
    sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWR,SENSOR_PWR);
    sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);
#endif
#if CAP_FRAME_MODE_BY_GPIO
    sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_FREX,SENSOR_FREX);
    FREX(0);
#endif
    /*Micron Power up sequence Here*/
#if EVB
    /*EVB*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,0); /* PWD LOW*/
	tmrUsWait(50000); /*50ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(25000); /*25ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
#elif Boqy_5330
    /*Real-Kit*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWR,SENSOR_PWR); /* PWD High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,0); /* PWD LOW*/
	tmrUsWait(50000); /*50ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(25000); /*25ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(50000); /*50ms*/
#elif Boqy_6330
    WRITE8(0xb0009048, (READ8(0xb0009048) & (1<<4)));
    WRITE8(0xb0009049, (READ8(0xb0009049) & (1<<7)));
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWR);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);
/*Micron Power up sequence Here*/
    /*Real-Kit*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWR,SENSOR_PWR);
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(25000); /*25ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
#endif
   	cmdportid = sensorTgCfg.cmdportid_xtg;
   	tmrUsWait(2000);

	/*I2C Init code Here */
	SENSOR_PROF_LOG_PRINT(1, "sensor DoFunInit model-ID =%x",sensorMT_Read(0x3000));
    SENSOR_PROF_LOG_ADD(1,"cmd_init");

    sensorMT_SRST();

	sensorLVDSCfgSet(&sensorHiSpiHDCfg);

	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);
	tmrUsWait(100000);	/* delay 100 ms*/

    sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
   	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,240000); /* 240MHz */
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/


#if STREAMOF_ENABLE
	sensorStsCfg(SENSOR_SYS_CFG_FLOW_USE_ABORT, STREAMOF_ENABLE);
#endif

	/*sensorStsCfg(SENSOR_SYS_CFG_FLOW_EARLY_SWITCH_PREVIEW, 1);*/

#if MODE_SYNC_EARLY
    sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);/*config precmd position (cmdport -MODE_SYNC_EARLY) */
#endif

#if (_SENSOR_DBG_)
    /*debug by TGGPIO2*/
    WRITE8(0xb0009030, (READ8(0xb0009030)|(0x02)));
    DBG_GPIO(0);
#endif

    sensorResumeInstall(sensorResumeExecuteLoad);
	sensorSuspendInstall(sensorSuspendExecuteLoad);

    sensor_bshut_vcount_offset_set(7400);/*unit:us*/
#if 0
#if EVB
	/*halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121000);*/ /* 121 MHz  CDSP 243MHz*/
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,150000);     /* 150 MHz  CDSP 300MHz*/
#else
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,148500);     /* 148.5 MHz  CDSP 297MHz*/
#endif
#endif
	printf("Sensor_mt9F002 init finish \n");
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncInit() 9F002 ---");
	tmrUsWait(10000);

#if 0
sp5kSensorModeCfgSet(0x13, 0x41);
#endif
}

static void
sensorDoFuncStop()
{
}

static void
sensorSnapModeAsyncCb(void)
{
	SENSOR_PROF_LOG_ADD(1,"AsyncSet");
	sensorSyncSigInit(p_size_pcg);
    sensorDsenExtSet();
}

static void
sensorSnapModeAsyncSet(const sensorSyncSigCfg_t *pspcg ,UINT32 sync)
{
    fCap2Pv = 0;
	p_size_pcg=pspcg;

	if(sync)
		sensor_frame_cb_reg(SENSOR_IRQ_SVC_ORG_VD,sensorSnapModeAsyncCb,0);
	else
		sensorSnapModeAsyncCb();
}

static void
sensorDoPreviewBegin(UINT32 nreg, const UINT8 *pdata,const sensorSyncSigCfg_t *pspcg)
{
    UINT32 cmdportid;
	UINT32 r3012,r0342,exp_line;
    cmdportid = sensorTgCfg.cmdportid_xtg;

     if(fCap2Pv){
          sensor_event_wait(SENSOR_EVENT_VVALID_END);
     }

    sensorSerialBulkWrite(cmdportid, COUNT_OF(cmd_stream_off), cmd_stream_off[0],0);
	/*
		for exp line smooth
		readback prev mode R0x0342(line total) & R0x3012(exp linel)
		re-calculate Rx3012 for current mode
	*/
	r0342=sensorMT_Read(0x0342);
	r3012=sensorMT_Read(0x3012);
    /*sensor_event_wait(SENSOR_EVENT_VVALID_END); */
        sensorSerialBulkWrite(cmdportid, nreg,pdata,0);
	exp_line=r0342*r3012;
	r0342=sensorMT_Read(0x0342);
	exp_line=exp_line/r0342;
	sensorMT_Write(0x3012,exp_line);
    if(is_aborted == 0){
		sensorSerialBulkWrite(cmdportid,COUNT_OF(cmd_stream_on), cmd_stream_on[0],0);
    }
    sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,0,DONTCHANE,DONTCHANE,DONTCHANE);
    sensorSnapModeAsyncSet(pspcg,0);

}

static void
sensorDoFuncPreviewVGA30fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(cmd_vga_30fps_code), cmd_vga_30fps_code[0],&sensorSyncSigPreviewVGA30fpsCfg);
	SENSOR_PROF_LOG_ADD(1, "mipi VGA 30fps cmd done");
    fpvsusp = 1;
}

static void
sensorDoFuncPreviewVGAZoom30fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(cmd_vga_zoom_30fps_code), cmd_vga_zoom_30fps_code[0],&sensorSyncSigPreviewVGA30fpsCfg);
	SENSOR_PROF_LOG_ADD(1, "mipi VGA 30fps cmd zoom done");
    fpvsusp = 1;
}

static void
sensorDoFuncPreviewVGA60fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(cmd_vga_60fps_code), cmd_vga_60fps_code[0],&sensorSyncSigPreviewVGA60fpsCfg);
	SENSOR_PROF_LOG_ADD(1, "mipi VGA 60fps cmd done");
    fpvsusp = 1;
}

static void
sensorDoFuncPreview1080pBegin()
{
    sensorDoPreviewBegin(COUNT_OF(cmd_1080p_code), cmd_1080p_code[0],&sensorSyncSigPreview1080pCfg);
	SENSOR_PROF_LOG_ADD(1, "mipi 1080p 30fps cmd done");
    fpvsusp = 1;
}

static void
sensorDoFuncPreviewFullSizeBegin()
{
    sensorDoPreviewBegin(COUNT_OF(cmd_still_code), cmd_still_code[0],&sensorSyncSigSnapCfg);
	SENSOR_PROF_LOG_ADD(1, "mipi full size cmd done");
    fpvsusp = 1;
}

static void
sensorDoFuncPreviewShortExp()
{
}

static void
sensorDoFuncPreviewExpEnd()
{
}

/*static void
sensorSnapFrameDummySet()
{
}*/

 void 
FlashDirectTrig()
{
    WRITE8(0xb0009035, READ8(0xb0009035)&0xFE);            /*Config TGGPIO bit8: Flash*/
    halFrontFlashCfgSet(2, 0xffff);     /*FRONT_FLASH_DURATION*/
    /*halFrontFlashCfgSet(1, 0x4321);*/ /*FRONT_FLASH_DELAY*/
    halFrontFlashCfgSet(0, 0);          /*FRONT_FLASH_MODE*/
    halFrontFlashTrig();
}


static void sensorFrontRestart(void)
{
    WRITE8(0xb0009010,1);
    WRITE8(0xb0009010,0);
}

static void sensorSpotLine(void)
{
    WRITE16(0xb0009064,1);
    SENSOR_PROF_LOG_PRINT(1,"line count= (%d)", READ16(0xb0009064));
}

void
sensorDoFuncSnapReadoutStart()
{
    TIME_LOG(1);
    SENSOR_PROF_LOG_PRINT(1, "EOF_2_SOF:            %d.%d ms", TIME_DIFF(1,0)/1000,TIME_DIFF(1,0)%1000);
}
void report_sensor_reg(void)
{
	SENSOR_PROF_LOG_PRINT(1,"[0x2270]=%x",*(volatile UINT8 *)0xb0002270);
    SENSOR_PROF_LOG_PRINT(1,"[0x2271]=%x",*(volatile UINT8 *)0xb0002271);
    SENSOR_PROF_LOG_PRINT(1,"[0x2272]=%d",*(volatile UINT16 *)0xb0002272);
    SENSOR_PROF_LOG_PRINT(1,"[0x2274]=%d",*(volatile UINT16 *)0xb0002274);
    SENSOR_PROF_LOG_PRINT(1,"[0x2276]=%d",*(volatile UINT16 *)0xb0002276);
    SENSOR_PROF_LOG_PRINT(1,"[0x2278]=%d",*(volatile UINT16 *)0xb0002278);
    SENSOR_PROF_LOG_PRINT(1,"[0x227A]=%x",*(volatile UINT16 *)0xb000227A);
    SENSOR_PROF_LOG_PRINT(1,"[0x227F]=%x",*(volatile UINT8 *)0xb000227F);
    SENSOR_PROF_LOG_PRINT(1,"[0x2004]=%x",*(volatile UINT8 *)0xb0002004);

	SENSOR_PROF_LOG_PRINT(1,"mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	SENSOR_PROF_LOG_PRINT(1,"front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	SENSOR_PROF_LOG_PRINT(1,"imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	SENSOR_PROF_LOG_PRINT(1,"rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	SENSOR_PROF_LOG_PRINT(1,"rawmux output size    = (%d,%d)",READ16(0xb0002090),READ16(0xb0002092));
	WRITE16(0xb0009064,1);
	SENSOR_PROF_LOG_PRINT(1,"line count            = (%d)",READ16(0xb0009064));
	SENSOR_PROF_LOG_PRINT(1,"front2cdsp gating     = (%x)",READ8(0xb0009080));
	SENSOR_PROF_LOG_PRINT(1,"snap trig             = (%x)",READ8(0xb0009068));
}

static void
sensorDoFuncSnapExpBeginEOFcb()
{
    UINT32 cmdportid, smode;
    cmdportid = sensorTgCfg.cmdportid_xtg;
    smode  = sensorModeGet();
    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapExpBeginEOFcb");
DBG_GPIO(1);
if(smode!=0x42){
    sensorSerialAsyncWrite(cmdportid, COUNT_OF(cmd_still_code), cmd_still_code[0]);
}else{
    sensorSerialAsyncWrite(cmdportid, COUNT_OF(cmd_still_code_calib), cmd_still_code_calib[0]);
}
    sensorSerialFlush(cmdportid);
    sensorSerialAsyncWrite(cmdportid, COUNT_OF(cmd_stream_on), cmd_stream_on[0]);
    sensorSerialFlush(cmdportid);

if(smode==0x41){
    SENSOR_PROF_LOG_ADD(1, "Frame mode------");
    #if CAP_FRAME_MODE_BY_GPIO
        FREX(1);
    #else
        sensorSerialAsyncWrite(cmdportid, COUNT_OF(cmd_frex_on), cmd_frex_on[0]);
        sensorSerialFlush(cmdportid);
    #endif

    sensor_frame_cb_reg(SENSOR_IRQ_SVC_NEG_VD, sensorDoFuncSnapReadoutStart, 0);
    TIME_LOG(0);
}else{
    SENSOR_PROF_LOG_ADD(1, "Rolling mode------");
    if(appStillCapFlashStatusGet()==1){
        SENSOR_PROF_LOG_ADD(1, "flash trig at ReadoutField0 Frame End------");
        sensor_frame_cb_reg(SENSOR_IRQ_SVC_FRAME_END, FlashDirectTrig, 0);
    }
}

    sensorFrontRestart();

if(smode!=0x42){
    sensorSyncSigInit(&sensorSyncSigSnapCfg);
}else{
    sensorSyncSigInit(&sensorSyncSigSnapCalibCfg);
}
    sensorDsenExtSet();
}

static void
sensorDoFuncSnapFrexOff()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
DBG_GPIO(0);

    #if CAP_FRAME_MODE_BY_GPIO
        FREX(0);
        SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapFrexOff");
    #else
        /*
          GRR Enter: by register trigger (L->H  rising edge)
                     H->L "will not" exit GRR.

          GRR Exit: Automatic at end of frame readout.

        */
        sensorSerialAsyncWrite(cmdportid, COUNT_OF(cmd_frex_off), cmd_frex_off[0]);
        sensorSerialFlush(cmdportid);
    #endif

    sensorFrontRestart();
    halFrontSnapTrig(1);
    sensorSyncSigInit(&sensorSyncSigSnapCfg);
}

static void
sensorDoFuncSnapBshutterBegin()
{
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapBshutterBegin()");
}

static void
sensorDoFuncSnapBshutterFollow()
{
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapBshutterFollow()");
}

static void
sensorDoFuncSnapBshutterEnd()
{
    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapBshutterEnd()");
    sensorDoFuncSnapFrexOff();
}

static void
sensorDoFuncSnapEarly0()
{
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapEarly0");
}

static void
sensorDoFuncSnapExpBegin()
{
    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapExpBegin()");
    UINT32 cmdportid, smode;
    cmdportid = sensorTgCfg.cmdportid_xtg;
    fCap2Pv = 1;
    smode  = sensorModeGet();
    sensorSerialBulkWrite(cmdportid,COUNT_OF(cmd_stream_off), cmd_stream_off[0],0);
    sensor_frame_cb_reg(SENSOR_IRQ_SVC_FRAME_END, sensorDoFuncSnapExpBeginEOFcb, 0);
}

static void
sensorDoFuncSnapShortExp()
{
}

static void
sensorDoFuncSnapExpEnd()
{
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapExpEnd");
}

static void
sensorDoFuncSnapReadoutField0()
{
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutField0()");
    SENSOR_PROF_LOG_PRINT(1,"front2cdsp gating     = (%x)",READ8(0xb0009080));
	SENSOR_PROF_LOG_PRINT(1,"snap trig             = (%x)",READ8(0xb0009068));
}

static void
sensorDoFuncSnapFieldEnd()
{
    sensorSpotLine();
	SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapFieldEnd()");
    SENSOR_PROF_LOG_PRINT(1,"front2cdsp gating     = (%x)",READ8(0xb0009080));
	SENSOR_PROF_LOG_PRINT(1,"snap trig             = (%x)",READ8(0xb0009068));
}

static void
sensorDoFuncSnapReadoutEnd()
{
    if(appStillCapFlashStatusGet()==2){  /*Fake Flash*/
        SENSOR_PROF_LOG_ADD(1, "Fake Flash trig at ReadoutEnd------");
		FlashDirectTrig();
	}

    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutEnd()");
    SENSOR_PROF_LOG_PRINT(1,"front2cdsp gating     = (%x)", READ8(0xb0009080));
    SENSOR_PROF_LOG_PRINT(1,"snap trig             = (%x)", READ8(0xb0009068));
#if 0
	SENSOR_PROF_LOG_PRINT(1, "[%03d]Early2Frmend:            %d.%d ms",g_snap_flow_count,TIME_DIFF(1,0)/1000,TIME_DIFF(1,0)%1000);
	SENSOR_PROF_LOG_PRINT(1, "[%03d]Frmend2BshutterEnd:      %d.%d ms",g_snap_flow_count,TIME_DIFF(2,1)/1000,TIME_DIFF(2,1)%1000);
	SENSOR_PROF_LOG_PRINT(1, "[%03d]BshutterEnd2Readoutend:  %d.%d ms",g_snap_flow_count,TIME_DIFF(3,2)/1000,TIME_DIFF(3,2)%1000);
	SENSOR_PROF_LOG_PRINT(1, "[%03d]Early2Readoutend:        %d.%d ms %d.%d FPS ",g_snap_flow_count,TIME_DIFF(3,0)/1000,TIME_DIFF(3,0)%1000,1000000/TIME_DIFF(3,0),((1000000*10)/TIME_DIFF(3,0))%10);
#endif
}

static void
sensorDoFuncSnapNext(UINT32 next_mode)
{
	if((next_mode&0xf0)==SENSOR_MODE_SNAP)
	{
		SENSOR_PROF_LOG_ADD(1,"@@@@ get sensor next mode is snap");
	}else{
		SENSOR_PROF_LOG_ADD(1,"@@@@ get sensor next mode is preview");
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
sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
    #if(1)
		if((sensorModeGet()&0xF0)==0x40){ /*don't set exp time& dummy in snap mode*/
            sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
            SENSOR_PROF_LOG_PRINT(1,"snap [nreg=%d][%x %x %x %x][%x %x %x %x][%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7],pdata[8],pdata[9],pdata[10],pdata[11]);
	    }else{
	    	/*SENSOR_PROF_LOG_PRINT(1,"prev [nreg=%d][%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);*/
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
		}
	#endif
}

static void
sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
}

UINT16
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

#if SP5K_SENSOR_MT9F002
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regdata[][4] SENSOR_SIF1DMA_VARBUF = {
		MI_REG(0x3012,0x0000),
	};
	*(UINT16 *)&regdata[0][2] = ((expLine>>8)&0xff) | ((expLine&0xff)<<8);
	PROF_LOG_PRINT(1,"expLine=%d",expLine);
	sensorSerialDirectBufSnoop(regdata[0], sizeof(regdata));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regdata[0]);
}
#endif

#if STREAMOF_ENABLE
static void
sensorDoFuncStreamOff(UINT32 abort)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==0){
        SENSOR_PROF_LOG_PRINT(1, "sensorModeGet= %x", sensorModeGet());
		SENSOR_PROF_LOG_ADD(1, "stream off s");
		sensorSerialFlush(cmdportid);
        sensorSerialBulkWrite(cmdportid, COUNT_OF(cmd_stream_off), cmd_stream_off[0],0);
		SENSOR_PROF_LOG_ADD(1, "stream off e");
		is_aborted = 1;
	}

}

static void
sensorDoFuncStreamOn(UINT32 abort)
{
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==1 ){
	    SENSOR_PROF_LOG_ADD(1, "stream on s");
		sensorSerialFlush(cmdportid);
        sensorSerialBulkWrite(cmdportid, COUNT_OF(cmd_stream_on), cmd_stream_on[0],0);
		SENSOR_PROF_LOG_ADD(1, "stream on e");
		is_aborted = 0;
	}
}
#endif

static UINT32 sensorSuspended;
static UINT32 sensorResumeExecuteLoad()
{
	#if SUSPEND_ENABLE
		if(sensorSuspended ==1){
	    	/* sensorResumeExecute() */
	    	SENSOR_PROF_LOG_ADD(1, "senResu s");/*Edward 20111126*/
            UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	    	sensorSerialFlush(cmdportid);
            sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0])), cmd_stream_on[0],0);
 	        tmrUsWait(5000);/*5ms*/
 	    	SENSOR_PROF_LOG_ADD(1, "senResu e");/*Edward 20111126*/
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
	    	SENSOR_PROF_LOG_ADD(1, "senSusp s");/*Edward 20111126*/
	    	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	    	sensorSerialFlush(cmdportid);
            sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_off)/sizeof(cmd_stream_off[0])), cmd_stream_off[0],0);
 	        tmrUsWait(5000);/*5ms*/
	    	SENSOR_PROF_LOG_ADD(1, "senSusp e");/*Edward 20111126*/
	    	sensorSuspended = 1;
	    }
	#endif
	return SUCCESS;
}


void
sensorMT_OB_Output(UINT8 on)
{
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;

    if(on){
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_OB_output_on)/sizeof(cmd_OB_output_on[0])), cmd_OB_output_on[0],0);
        printf("Aptina 14M sensor OB output Enable \n");
    }else{
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_OB_output_off)/sizeof(cmd_OB_output_off[0])), cmd_OB_output_off[0],0);
        printf("Aptina 14M sensor OB output Disable \n");
    }
}

void
sensorMT_DeSensor(UINT8 on)
{
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;

    if(on){
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_denoise_on)/sizeof(cmd_denoise_on[0])), cmd_denoise_on[0],0);
        printf("Aptina 14M sensor De-Noise Enable \n");
    }else{
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_denoise_off)/sizeof(cmd_denoise_off[0])), cmd_denoise_off[0],0);
        printf("Aptina 14M sensor De-Noise Disable \n");
    }
}

void
sensorMT_LSC(UINT8 on)
{
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;

    if(on){
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_LSC_on)/sizeof(cmd_LSC_on[0])), cmd_LSC_on[0],0);
        printf("Aptina 14M sensor LSC Enable \n");
    }else{
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_LSC_off)/sizeof(cmd_LSC_off[0])), cmd_LSC_off[0],0);
        printf("Aptina 14M sensor LSC Disable \n");
    }
}


void
sensorMT_DumpReg(void)
{
	UINT32 i;
	UINT16 reg_to_dump_init[] = {
		0x0000,0x0002,0x0003,0x0004,0x0005,0x0006,0x0008,0x0040,0x0041,0x0042,0x0044,0x0046,0x0048,0x004A,0x004C,0x004E,0x0050,0x0052,0x0054,0x0056,
		0x0058,0x005A,0x005C,0x005E,0x0080,0x0084,0x0086,0x0088,0x008A,0x008C,0x008E,0x0090,0x0092,0x00C0,0x00C1,0x00C2,0x00C4,0x00C6,0x00C8,0x00CA,
		0x00CC,0x00CE,0x0100,0x0101,0x0103,0x0104,0x0105,0x0110,0x0111,0x0112,0x0120,0x0200,0x0202,0x0204,0x0206,0x0208,0x020A,0x020C,0x020E,0x0210,
		0x0212,0x0214,0x0300,0x0302,0x0304,0x0306,0x0308,0x030A,0x0340,0x0342,0x0344,0x0346,0x0348,0x034A,0x034C,0x034E,0x0380,0x0382,0x0384,0x0386,
		0x0400,0x0402,0x0404,0x0406,0x0500,0x0600,0x0602,0x0604,0x0606,0x0608,0x060A,0x060C,0x060E,0x0610,0x1000,0x1004,0x1006,0x1008,0x100A,0x1080,
		0x1084,0x1086,0x1088,0x1100,0x1104,0x1108,0x110A,0x110C,0x1110,0x1114,0x1116,0x1118,0x111C,0x1120,0x1122,0x1124,0x1128,0x112C,0x1130,0x1134,
		0x1136,0x1140,0x1142,0x1144,0x1146,0x1148,0x114A,0x1160,0x1162,0x1164,0x1168,0x116C,0x116E,0x1170,0x1174,0x1180,0x1182,0x1184,0x1186,0x11C0,
		0x11C2,0x11C4,0x11C6,0x1200,0x1204,0x1206,0x1208,0x120A,0x1300,0x1400,0x1402,0x1404,0x1406,0x1408,0x140A,0x140C,0x140E,0x1410,0x3000,0x3002,
		0x3004,0x3006,0x3008,0x300A,0x300C,0x3010,0x3012,0x3014,0x3016,0x3018,0x301A,0x301C,0x301D,0x301E,0x3021,0x3022,0x3023,0x3024,0x3026,0x3028,
		0x302A,0x302C,0x302E,0x3030,0x3032,0x3034,0x3036,0x3038,0x303A,0x303B,0x303C,0x3040,0x3044,0x3046,0x3048,0x304A,0x304C,0x304E,0x3050,0x3052,
		0x3054,0x3056,0x3058,0x305A,0x305C,0x305E,0x3062,0x3064,0x3066,0x306A,0x306C,0x306E,0x3070,0x3072,0x3074,0x3076,0x3078,0x307A,0x3082,0x3084,
		0x30A0,0x30A2,0x30A4,0x30A6,0x30A8,0x30AA,0x30AC,0x30AE,0x30B0,0x30B2,0x30B4,0x30BC,0x30C0,0x30C2,0x30C4,0x30C6,0x30C8,0x30CA,0x30CC,0x30CE,
		0x30D0,0x30D2,0x30D4,0x30D6,0x30D8,0x30DA,0x30E0,0x30E2,0x30E4,0x30E8,0x30EA,0x30EC,0x30EE,0x3130,0x3132,0x3134,0x3136,0x3138,0x313A,0x313C,
		0x313E,0x3140,0x3154,0x3156,0x3158,0x315A,0x315C,0x315E,0x3160,0x3162,0x3164,0x3166,0x3168,0x3170,0x3172,0x3174,0x3176,0x3178,0x317A,0x317C,
		0x318A,0x318C,0x318E,0x3190,0x31A0,0x31A2,0x31A4,0x31A6,0x31A8,0x31AA,0x31AC,0x31AE,0x31B0,0x31B2,0x31B4,0x31B6,0x31B8,0x31BA,0x31BC,0x31BE,
		0x31C0,0x31C2,0x31C4,0x31C6,0x31DA,0x31DC,0x31DE,0x31E0,0x31E2,0x31E4,0x31E6,0x31E8,0x31EA,0x31EC,0x31EE,0x31F2,0x31F4,0x31F6,0x31F8,0x31FA,
		0x31FC,0x31FE,0x3600,0x3602,0x3604,0x3606,0x3608,0x360A,0x360C,0x360E,0x3610,0x3612,0x3614,0x3616,0x3618,0x361A,0x361C,0x361E,0x3620,0x3622,
		0x3624,0x3626,0x3640,0x3642,0x3644,0x3646,0x3648,0x364A,0x364C,0x364E,0x3650,0x3652,0x3654,0x3656,0x3658,0x365A,0x365C,0x365E,0x3660,0x3662,
		0x3664,0x3666,0x3680,0x3682,0x3684,0x3686,0x3688,0x368A,0x368C,0x368E,0x3690,0x3692,0x3694,0x3696,0x3698,0x369A,0x369C,0x369E,0x36A0,0x36A2,
		0x36A4,0x36A6,0x36C0,0x36C2,0x36C4,0x36C6,0x36C8,0x36CA,0x36CC,0x36CE,0x36D0,0x36D2,0x36D4,0x36D6,0x36D8,0x36DA,0x36DC,0x36DE,0x36E0,0x36E2,
		0x36E4,0x36E6,0x3700,0x3702,0x3704,0x3706,0x3708,0x370A,0x370C,0x370E,0x3710,0x3712,0x3714,0x3716,0x3718,0x371A,0x371C,0x371E,0x3720,0x3722,
		0x3724,0x3726,0x3780,0x3782,0x3784,0x37C0,0x37C2,0x37C4,0x37C6,0x3800,0x3802,0x3804,0x3806,0x3808,0x380A,0x380C,0x380E,0x3810,0x3812,0x3814,
		0x3816,0x3818,0x381A,0x381C,0x381E,0x3820,0x3822,0x3824,0x3826,0x3828,0x382A,0x382C,0x382E,0x3830,0x3832,0x3834,0x3836,0x3838,0x383A,0x383C,
		0x383E,0x3840,0x3842,0x3844,0x3846,0x3848,0x384A,0x384C,0x384E,0x3850,0x3852,0x3854,0x3856,0x3858,0x385A,0x385C,0x385E,0x3860,0x3862,0x3864,
		0x3866,0x3868,0x386A,0x386C,0x386E,0x3870,0x3872,0x3874,0x3876,0x3878,0x387A,0x387C,0x387E,0x3880,0x3882,0x3884,0x3886,0x3888,0x388A,0x388C,
		0x388E,0x3890,0x3892,0x3894,0x3896,0x3898,0x389A,0x389C,0x389E,0x38A0,0x38A2,0x38A4,0x38A6,0x38A8,0x38AA,0x38AC,0x38AE,0x38B0,0x38B2,0x38B4,
		0x38B6,0x38B8,0x38BA,0x38BC,0x38BE,0x38C0,0x38C2,0x38C4,0x38C6,0x38C8,0x38CA,0x38CC,0x38CE,0x38D0,0x38D2,0x38D4,0x38D6,0x38D8,0x38DA,0x38DC,
		0x38DE,0x38E0,0x38E2,0x38E4,0x38E6,0x38E8,0x38EA,0x38EC,0x38EE,0x38F0,0x38F2,0x38F4,0x38F6,0x38F8,0x38FA,0x38FC,0x38FE,0x3900,0x3902,0x3904,
		0x3906,0x3908,0x390A,0x390C,0x390E,0x3910,0x3912,0x3914,0x3916,0x3918,0x391A,0x391C,0x391E,0x3920,0x3922,0x3924,0x3926,0x3928,0x392A,0x392C,
		0x392E,0x3930,0x3932,0x3934,0x3936,0x3938,0x393A,0x393C,0x393E,0x3940,0x3942,0x3944,0x3946,0x3948,0x394A,0x394C,0x394E,0x3950,0x3952,0x3954,
		0x3956,0x3958,0x395A,0x395C,0x395E,0x3960,0x3962,0x3964,0x3966,0x3968,0x396A,0x396C,0x396E,0x3970,0x3972,0x3974,0x3976,0x3978,0x397A,0x397C,
		0x397E,0x3980,0x3982,0x3984,0x3986,0x3988,0x398A,0x398C,0x398E,0x3990,0x3992,0x3994,0x3996,0x3998,0x399A,0x399C,0x399E,0x39A0,0x39A2,0x39A4,
		0x39A6,0x39A8,0x39AA,0x39AC,0x39AE,0x39B0,0x39B2,0x39B4,0x39B6,0x39B8,0x39BA,0x39BC,0x39BE,0x39C0,0x39C2,0x39C4,0x39C6,0x39C8,0x39CA,0x39CC,
		0x39CE,0x39D0,0x39D2,0x39D4,0x39D6,0x39D8,0x39DA,0x39DC,0x39DE,0x39E0,0x39E2,0x39E4,0x39E6,0x39E8,0x39EA,0x39EC,0x39EE,0x39F0,0x39F2,0x39F4,
		0x39F6,0x39F8,0x39FA,0x39FC,0x39FE,0x3D00,0x3D02,0x3D04,0x3D06,0x3D08,0x3D0A,0x3D0C,0x3D0E,0x3D10,0x3D12,0x3D14,0x3D16,0x3D18,0x3D1A,0x3D1C,
		0x3D1E,0x3D20,0x3D22,0x3D24,0x3D26,0x3D28,0x3D2A,0x3D2C,0x3D2E,0x3D30,0x3D32,0x3D34,0x3D36,0x3D38,0x3D3A,0x3D3C,0x3D3E,0x3D40,0x3D42,0x3D44,
		0x3D46,0x3D48,0x3D4A,0x3D4C,0x3D4E,0x3D50,0x3D52,0x3D54,0x3D56,0x3D58,0x3D5A,0x3D5C,0x3D5E,0x3D60,0x3D62,0x3D64,0x3D66,0x3D68,0x3D6A,0x3D6C,
		0x3D6E,0x3D70,0x3D72,0x3D74,0x3D76,0x3D78,0x3D7A,0x3D7C,0x3D7E,0x3D80,0x3D82,0x3D84,0x3D86,0x3D88,0x3D8A,0x3D8C,0x3D8E,0x3D90,0x3D92,0x3D94,
		0x3D96,0x3D98,0x3D9A,0x3D9C,0x3D9E,0x3DA0,0x3DA2,0x3DA4,0x3DA6,0x3DA8,0x3DAA,0x3DAC,0x3DAE,0x3DB0,0x3DB2,0x3DB4,0x3DB6,0x3DB8,0x3DBA,0x3DBC,
		0x3DBE,0x3DC0,0x3DC2,0x3DC4,0x3DC6,0x3DC8,0x3DCA,0x3DCC,0x3DCE,0x3DD0,0x3DD2,0x3DD4,0x3DD6,0x3DD8,0x3DDA,0x3DDC,0x3DDE,0x3DE0,0x3DE2,0x3DE4,
		0x3DE6,0x3DE8,0x3DEA,0x3DEC,0x3DEE,0x3DF0,0x3DF2,0x3DF4,0x3DF6,0x3DF8,0x3DFA,0x3DFC,0x3DFE,0x3E00,0x3E02,0x3E04,0x3E06,0x3E08,0x3E0A,0x3E0C,
		0x3E0E,0x3E10,0x3E12,0x3E14,0x3E16,0x3E18,0x3E1A,0x3E1C,0x3E1E,0x3E20,0x3E22,0x3E24,0x3E26,0x3E28,0x3E2A,0x3E2C,0x3E2E,0x3E30,0x3E32,0x3E34,
		0x3E36,0x3E38,0x3E3A,0x3E3C,0x3E3E,0x3E40,0x3E42,0x3E44,0x3E46,0x3E48,0x3E4A,0x3E4C,0x3E4E,0x3E50,0x3E52,0x3E54,0x3E56,0x3E58,0x3E5A,0x3E5C,
		0x3E5E,0x3E60,0x3E62,0x3E64,0x3E66,0x3E68,0x3E6A,0x3E6C,0x3E6E,0x3E70,0x3E72,0x3E74,0x3E76,0x3E78,0x3E7A,0x3E7C,0x3E7E,0x3E80,0x3E82,0x3E84,
		0x3E86,0x3E88,0x3E8A,0x3E8C,0x3E8E,0x3E90,0x3E92,0x3E94,0x3E96,0x3E98,0x3E9A,0x3E9C,0x3E9E,0x3EA0,0x3EA2,0x3EA4,0x3EA6,0x3EA8,0x3EAA,0x3EAC,
		0x3EAE,0x3EB0,0x3EB2,0x3EB4,0x3EB6,0x3EB8,0x3EBA,0x3EBC,0x3EBE,0x3EC0,0x3EC2,0x3EC4,0x3EC6,0x3EC8,0x3ECA,0x3ECC,0x3ECE,0x3ED0,0x3ED2,0x3ED4,
		0x3ED6,0x3ED8,0x3EDA,0x3EDC,0x3EDE,0x3EE0,0x3EE2,0x3EE4,0x3EE6,0x3EE8,0x3EEA,0x3EEC,0x3EEE,0x3EF0,0x3EF2,0x3EF4,0x3EF6,0x3EFA,0x3EFC,0x3EFE,
		0x3F02,0x3F04,0x3F06,0x3F08,0x3F0A,0x3FE0,0x3FE2,0x3FE4,0x3FE6,0x3FE8,0x3FEA,0x3FEC
	};

	for(i=0; i<sizeof(reg_to_dump_init)/sizeof(UINT16); i++) {
		UINT16 reg_addr = reg_to_dump_init[i];
		printf("REG= 0x%04X, 0x%04X \n", reg_addr, sensorMT_Read(reg_addr));
	}
}

