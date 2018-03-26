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
 *  Author:                                                     *
 *                                                                        *
 *  Version: 0.01, 2012.5.3:
 *  Add initial settings and preview modes. *
 *  *
 *
 *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_panamn34120-0.00.00"
#define BUILD_TIME              __DATE__  "-"  __TIME__


#include "common.h"
#include "../flow/sysregs.h"
#include "../flow/hal_front.h"
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
/*#include "../sensor_util.h"*/
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_dbg_api.h"

#include <stdio.h>
#include "middleware/proflog.h"
/*#include "../../../../include/hal/hal_glob.h"*/
#include "../flow/sensor_model_snap.h"
#include "../include/customization/solution_def.h"
/*#include "../customization/hostfw/app/app_Gpio.h"*/

#define  SENSOR_PROF_LOG_EN   (0)
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

#define MODE_SYNC_EARLY  (50) /*define precmd position (cmdport -MODE_SYNC_EARLY) */

static BOOL   fCap2Pv;
static UINT32 is_aborted 	= 0; 
static UINT32 exp_aborted 	= 0; 
static UINT32 g_snap_flow_count	= 0; 

static UINT32 snap_time_log[5];

#if SENSOR_PROF_LOG_EN 
static UINT8  afe_snap_Exp[8];/*Backup snap */
#endif
static const sensorSyncSigCfg_t        *p_size_pcg;
static sensorSyncSigGenSizeCfg_t *p_gen_pcg;
static UINT32                     g_hoffset;
static UINT32 g_senSvcIdx; 


#define LVDS_VGA2CH_EN                 (0)  
#define STREAMOF_ENABLE                (0)    
#define SUSPEND_ENABLE                 (0)
#define BURST_CAP_EN                   (1)
#define SENSOR_IRQ_SVC_FRAME_END       (1)
#define SENSOR_IRQ_SVC_NEG_VD          (10)
#define SENSOR_IRQ_SVC_ORG_VD          (12)
#define SENSOR_EVENT_VVALID_END        (1UL<<13)
#define SENSOR_EVENT_SNAP_DONE         (1UL<<15)


#define COUNT_OF(array)	(sizeof(array)/sizeof(array[0]))
#define PANACMOS_WREG(a,b) {(a&0xFF),(a>>8),(b&0xFF),(b>>8)}

#define TIME_TAG(id) (snap_time_log[id])
#define TIME_LOG(id)  snap_time_log[id]=tmrTimeClockGet()
#define TIME_DIFF(a,b) (snap_time_log[a]-snap_time_log[b])

/*default preview mode ...*/
#define DEF_INIT_AFE_CODE     afe_vga_code 
#define DEF_SYNC_SIG_CFG      sensorSyncSigPreviewVGACfg
#define DEF_SYNC_SIG_GEN_CFG  sensorSyncSigGenPreviewVGA30fpsCfg

/* Sensor image size definition here */
#define NO             0
#define BY_PIXEL       1
#define BY_HD          0
#define NOUSE          0
#define DISABLE        0xFFFF
#define DONTCHANE      0x7FFFFFFF

#if defined(HW_EVB_SPE7001)
    #define SENSOR_PWRDN    (1)     /*TGGPIO 0*/
    #define SENSOR_RESET    (1<<23) /*FMGPIO 23*/
#endif

#define SENSOR_MCLK      54000000
#if(CPU_CLK==6)       /*CPU_CLK = 300MHz PCLK 1X = 300MHz*/
    #define MCLK2MCLK(a)     (a)
    #define FACTOR_MCLK(a)   (a*555/100)    /* 300MHz/54MHz = 5.55 */
#elif (CPU_CLK==8)    /*CPU_CLK = 360MHz MCLK = 240MHz*/
    #define MCLK2MCLK(a)     (a*240/300)    /* 300MHz/240MHz for reshape timing */
    #define FACTOR_MCLK(a)   (a*444/100)    /* 240MHz/54MHz = 4.44 */
#else
    #warning not support lower clock
    #define MCLK2MCLK(a)     (a)
    #define FACTOR_MCLK(a)   (a)
#endif

/*****TODO***multiple preview mode for VGA*/

/* For  preview size 640*480    */
#define PREVIEW_VGA_HSIZE  (1543)  /* preview horiz. size*/
#define PREVIEW_VGA_VSIZE  (496)   /* preview vertical size*/
#define PREVIEW_VGA_VTOTAL (568)    /* preview vertical size*/

/* For  preview size 1280*720  */
#define PREVIEW_720P_HSIZE  (1543)  /* preview horiz. size*/
#define PREVIEW_720P_VSIZE  (872)  /* preview vertical size*/
#define PREVIEW_720P_VTOTAL (924)   /* preview vertical size*/

/*For preview size, 1920*1080  */
#define PREVIEW_1080P_HSIZE  (2314)    /* preview horiz. size*/
#define PREVIEW_1080P_VSIZE  (1740)    /* preview vertical size*/
#define PREVIEW_1080P_VTOTAL (1804)    /* preview vertical size*/


/* For 14M output. */
#define CAPTURE_HSIZE  (4632)		/* capture horiz. size*/
#define CAPTURE_VSIZE  (3488)		/* capture vertical size*/
#define CAPTURE_VTOTAL (3590)   


/* Preview default definition here */
/* For  preview size 640*480, 30fps */
#define PREVIEW_VGA_LINETOTAL   FACTOR_MCLK(650) 
#define PREVIEW_VGA_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_VGA_FRAMETOTAL  PREVIEW_VGA_VTOTAL	    /* preview Vd length*/
#define PREVIEW_VGA_FRAMEBLANK  PREVIEW_VGA_VTOTAL		/* preview Vd sync length*/
#define PREVIEW_VGA_HOFFSET     (PREVIEW_VGA_LINETOTAL-PREVIEW_VGA_HSIZE-16) 
#define PREVIEW_VGA_HCROP_OFFSET (16-1)/*6330 BUG workaround 4n-1*/
#define PREVIEW_VGA_VOFFSET     (63)    /* preview Vd valid line data offset position*/
/* Preview resh_VGA_pe sync signal definition here */
#define PREVIEW_VGA_HRESHAPE_FALL         MCLK2MCLK(1200) 	/* preview Hd reshap fall*/
#define PREVIEW_VGA_HRESHAPE_RISE         MCLK2MCLK(1300)	/* preview Hd reshap rise*/
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_VGA_VRESHAPE_FALL        (PREVIEW_VGA_VOFFSET-2) 	/* preview Vd reshap fall*/
#define PREVIEW_VGA_VRESHAPE_RISE        (PREVIEW_VGA_VOFFSET-1)	/* preview Vd reshap rise*/

/*For preview 1080P size, 1920*1080, 30fps*/
#define PREVIEW_1080P_LINETOTAL   FACTOR_MCLK(950)	/* preview Hd length*/
#define PREVIEW_1080P_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_1080P_FRAMETOTAL  PREVIEW_1080P_VTOTAL	/* preview Vd length*/
#define PREVIEW_1080P_FRAMEBLANK  PREVIEW_1080P_VTOTAL	/* preview Vd sync length*/
#define PREVIEW_1080P_HOFFSET     (PREVIEW_1080P_LINETOTAL-PREVIEW_1080P_HSIZE-16)   	/* preview Hd valid pixel data offset position*/
#define PREVIEW_1080P_HCROP_OFFSET (16-1)/*6330 BUG workaround 4n-1*/
#define PREVIEW_1080P_VOFFSET     47		/* preview Vd valid line data offset position*/
/* Preview resh_1080P_pe sync signal definition here */
#define PREVIEW_1080P_HRESHAPE_FALL         MCLK2MCLK(1200) 	/* preview Hd reshap fall*/
#define PREVIEW_1080P_HRESHAPE_RISE         MCLK2MCLK(1300)	/* preview Hd reshap rise*/
#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_1080P_VRESHAPE_FALL         (PREVIEW_1080P_VOFFSET-2) 	/* preview Vd reshap fall*/
#define PREVIEW_1080P_VRESHAPE_RISE         (PREVIEW_1080P_VOFFSET-1)	/* preview Vd reshap rise*/

/*For preview size, 1280*720, 60fps, cropping*/
#define PREVIEW_720P_LINETOTAL   FACTOR_MCLK(634)	/* preview Hd length*/
#define PREVIEW_720P_LINEBLANK   6		/* preview Hd sync length*/
#define PREVIEW_720P_FRAMETOTAL  PREVIEW_720P_VTOTAL	/* preview Vd length*/
#define PREVIEW_720P_FRAMEBLANK  PREVIEW_720P_VTOTAL	/* preview Vd sync length*/
#define PREVIEW_720P_HOFFSET     (PREVIEW_720P_LINETOTAL-PREVIEW_720P_HSIZE-16)   	/* preview Hd valid pixel data offset position*/
#define PREVIEW_720P_HCROP_OFFSET  (16-1)/*6330 BUG workaround 4n-1*/
#define PREVIEW_720P_VOFFSET     55		/* preview Vd valid line data offset position*/
/* Preview resh_720P_pe sync signal definition here */
#define PREVIEW_720P_HRESHAPE_FALL         MCLK2MCLK(1000) 	/* preview Hd reshap fall*/
#define PREVIEW_720P_HRESHAPE_RISE         MCLK2MCLK(1100)	    /* preview Hd reshap rise*/
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL  BY_HD	/* preview Vd reshap control*/
#define PREVIEW_720P_VRESHAPE_FALL         (PREVIEW_720P_VOFFSET-2) /* preview Vd reshap fall*/
#define PREVIEW_720P_VRESHAPE_RISE         (PREVIEW_720P_VOFFSET-1)	/* preview Vd reshap rise*/

/* Capture default definition here */
#define CAPTURE_LINETOTAL   FACTOR_MCLK(1964)	/* capture Hd length*/
#define CAPTURE_LINEBLANK   6			/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  CAPTURE_VTOTAL		/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  CAPTURE_VTOTAL		/* mipi use > vsize is more safe*/

#define CAPTURE_HOFFSET     (CAPTURE_LINETOTAL-CAPTURE_HSIZE-16)    /*Capturee Hd valid pixel data offset position*/
#define CAPTURE_HCROP_OFFSET  (48-1)    /*6330 BUG workaround 4n-1*/
#define CAPTURE_VOFFSET     (78)    		/* capture Vd valid line data offset position*/

/* Capture reshape sync signal definition here */
#define CAPTURE_HRESHAPE_FALL         MCLK2MCLK(0x0400)	/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE         MCLK2MCLK(0x0500)	/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  BY_HD /* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL		  (CAPTURE_VOFFSET-2)	/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_VRESHAPE_RISE		  (CAPTURE_VOFFSET-1)   /* capture Vd reshap rise , count by pixel*/ 



/*Special for early frame (delay frame) length control*/
/* Use Frame end  length */
/* early0 = 20ms - snap2snap optime(0.6~0.9ms)    */
#define CAPTURE_EARLY20ms_VSIZE  	     (678)	    /* capture vertical size become 20ms delay*/
#define CAPTURE_EARLY20ms_VOFFSET        (50)    	/* capture Vd valid line data offset position*/
#define	CAPTURE_EARLY20ms_HDNUM_CMDPORT	 (500)      /* the command port interrupt line number of the snap new frame*/
#define	CAPTURE_EARLY20ms_HDNUM_FRAME_END (CAPTURE_EARLY20ms_VSIZE+CAPTURE_EARLY20ms_VOFFSET+1)    /*the command port interrupt line number of the snap new frame*/
/* Capture reshape sync signal definition here */
#define CAPTURE_EARLY20ms_VRESHAPE_FALL	 (28)	/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_EARLY20ms_VRESHAPE_RISE	 (30)   /* capture Vd reshap rise , count by pixel*/ 

/* early0 = 3ms = FullSizeUcode(1.7ms) + 0.4~ 0.5ms(waitNewFrame)*/
#define CAPTURE_EARLY3ms_VSIZE  	     (30)	 /* capture vertical size become 1.4 ms delay*/
#define CAPTURE_EARLY3ms_VOFFSET         (5)     /* capture Vd valid line data offset position*/
#define	CAPTURE_EARLY3ms_HDNUM_CMDPORT	 (16)    /* the command port interrupt line number of the snap new frame*/
#define	CAPTURE_EARLY3ms_HDNUM_FRAME_END (CAPTURE_EARLY3ms_VSIZE+CAPTURE_EARLY3ms_VOFFSET+1)    /*the command port interrupt line number of the snap new frame*/
/* Capture reshape sync signal definition here */
#define CAPTURE_EARLY3ms_VRESHAPE_FALL	 (2)	/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_EARLY3ms_VRESHAPE_RISE	 (3) /* capture Vd reshap rise , count by pixel*/ 


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

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_MASTER  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_1080P_CFAPATTERN	BAYER_GRBG          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_VGA_CFAPATTERN      BAYER_GRBG
#define PREVIEW_720P_CFAPATTERN     BAYER_GRBG
#define PREVIEW_QVGA_CFAPATTERN     BAYER_GRBG

#define	SNAP_CFAPATTERN		BAYER_BGGR       /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	            /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	1	            /*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		        1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM	  1	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN  0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/

#define	CAPTURE_HDNUM_FRAME_START  110	/*the front interrupt line number of the snap new frame */
#define	PREVIEW_HDNUM_FRAME_START  110		/*the front interrupt line number of the preview new frame*/
#define	CAPTURE_HDNUM_FRAME_END	 	(CAPTURE_VOFFSET+CAPTURE_VSIZE+1)/*(2)*/ 	/*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT		(CAPTURE_VOFFSET+CAPTURE_VSIZE+1)  /*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_VGA_HDNUM_FRAME_END		2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_CMDPORT		300		/*3281*//*976*/		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/

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
#define	TG_SPEED    	 8
#define	TG_NBIT_REGDATA  16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */

/*prototypes of sensor model methods*/
extern void sensorSyncSigGenInit(const sensorSyncSigGenSizeCfg_t *pcfg	);
extern UINT32 tmrTimeClockGet();
extern UINT32 sensorModeGet(void);
extern UINT32 sensor_event_wait(UINT32 event);

static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewVGA120fpsBegin();
static void sensorDoFuncPreviewVGA60fpsBegin();
static void sensorDoFuncPreviewVGA30fpsBegin();
static void sensorDoFuncPreview720P60fpsBegin();
static void sensorDoFuncPreview720P30fpsBegin();
static void sensorDoFuncPreview1080PBegin();
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
static UINT32 sensorResumeExecuteLoad();
static UINT32 sensorSuspendExecuteLoad();
void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);
UINT32 sensor_frame_repeat_cb_reg(UINT32 svcid,void *f,UINT32 param);
void sensor_frame_cb_list_unreg(UINT32 svcid, UINT32 cbidx);

static void sensorPv2SnapModeSet(void);
static void sensorSnap2SnapModeSet(void);
static void sensorDoFuncSnapNext(UINT32 next_mode);
void (*fp_SnapEarlyDo)(void); /*function point for capture early*/


#if STREAMOF_ENABLE
    static void sensorDoFuncStreamOff(UINT32 abort);
    static void sensorDoFuncStreamOn(UINT32 abort);
#endif


static const UINT8 afe_FrameMode_Blank_code[][4]={ /*Disable output dummy frame*/
	PANACMOS_WREG(0x0066, 0x0001), 
};


static const UINT8 afe_ShortExpcode[][4]={  /*Under 1 VD exp , still mode off */
	PANACMOS_WREG(0x0064, 0x0000), 
	PANACMOS_WREG(0x0066, 0x1000),/*??? */
};


static const UINT8 afe_streamon_code[][4]={ /*stream on , power save => resume */
	/* power save 1 */
	/* TG reset 	  */
};
static const UINT8 afe_streamoff_code[][4]={ /*stream off , resume => power save */
	/* power save 1 */
	/* TG reset 	  */
};

static const UINT8 afe_sensorpsav[][4]={/* Power save 3-2   */ 
	/* power save 1 */
	/* TG reset 	  */
};


/*pana sensor mode 9*/
static const UINT8 afe_vga_code[][4]={
/* BATCH MODE9
# VCYCLE:715 HCYCLE:630 (MCLK 54M)
#_DSC_11_V2_7mix_H3mix_12bit_Ver28b_120307
## PLL_REG ## */
	PANACMOS_WREG(0x0008 ,0x42B9),
	PANACMOS_WREG(0x000F ,0x7FFF),
	PANACMOS_WREG(0x0001 ,0x0030),
	PANACMOS_WREG(0x0002 ,0x0003),
	PANACMOS_WREG(0x0003 ,0x07E9),
	PANACMOS_WREG(0x0000 ,0x0003),
	PANACMOS_WREG(0x0004 ,0x0083),
	PANACMOS_WREG(0x0006 ,0x11DC),
	PANACMOS_WREG(0x0009 ,0x02C0),
	PANACMOS_WREG(0x0005 ,0x0001),
	PANACMOS_WREG(0x0007 ,0x7FFF),
	PANACMOS_WREG(0x000A ,0x0000),
	PANACMOS_WREG(0x000B ,0x0100),
	PANACMOS_WREG(0x000C ,0x0000),
	PANACMOS_WREG(0x000D ,0x2090),
	PANACMOS_WREG(0x000E ,0x0103),
	PANACMOS_WREG(0x0010 ,0x0000),
	PANACMOS_WREG(0x0011 ,0x0001),
	PANACMOS_WREG(0x0000 ,0x0053),
	PANACMOS_WREG(0x0008 ,0x02B8),
	/* ## SYS_REG ## */
	PANACMOS_WREG(0x0020 ,0x6103),
	PANACMOS_WREG(0x0021 ,0x1012),
	PANACMOS_WREG(0x0022 ,0x2555),
	PANACMOS_WREG(0x0023 ,0x0222),
	PANACMOS_WREG(0x0024 ,0x0222),
	PANACMOS_WREG(0x0025 ,0x1375),
	PANACMOS_WREG(0x0026 ,0x0333),
	PANACMOS_WREG(0x0027 ,0x0444),
	PANACMOS_WREG(0x0028 ,0x8390),
	PANACMOS_WREG(0x0029 ,0x0000),
	PANACMOS_WREG(0x002A ,0x0000),
	PANACMOS_WREG(0x002B ,0x50F0),
	PANACMOS_WREG(0x002C ,0x0014),
	PANACMOS_WREG(0x002D ,0x0007),
	PANACMOS_WREG(0x002E ,0x0000),
	PANACMOS_WREG(0x0030 ,0x4700),
	PANACMOS_WREG(0x0031 ,0x0015),
	PANACMOS_WREG(0x0032 ,0x8386),
	PANACMOS_WREG(0x0033 ,0x6662),
	PANACMOS_WREG(0x0034 ,0x0066),
	PANACMOS_WREG(0x0035 ,0x0000),
	PANACMOS_WREG(0x0036 ,0x0000),
	PANACMOS_WREG(0x0037 ,0x0000),
	PANACMOS_WREG(0x0038 ,0x0000),
	PANACMOS_WREG(0x0039 ,0x0000),
	PANACMOS_WREG(0x003A ,0x01FF),
	PANACMOS_WREG(0x003B ,0x30FF),
	PANACMOS_WREG(0x003C ,0x0FFE),
	PANACMOS_WREG(0x003D ,0x0001),
	PANACMOS_WREG(0x003E ,0x001C),
	PANACMOS_WREG(0x003F ,0x0031),
	PANACMOS_WREG(0x0040 ,0x0001),
	PANACMOS_WREG(0x0041 ,0x0300),
	PANACMOS_WREG(0x0042 ,0x0808),
	PANACMOS_WREG(0x0043 ,0x0A0A),
	PANACMOS_WREG(0x0044 ,0x0000),
	PANACMOS_WREG(0x0045 ,0x0000),
	PANACMOS_WREG(0x0046 ,0x0000),
	PANACMOS_WREG(0x0047 ,0x0800),
	PANACMOS_WREG(0x0048 ,0x4310),
	PANACMOS_WREG(0x0049 ,0x679A),
	PANACMOS_WREG(0x004A ,0x2010),
	PANACMOS_WREG(0x004B ,0x6703),
	PANACMOS_WREG(0x004C ,0x0450),
	PANACMOS_WREG(0x004D ,0x0000),
	PANACMOS_WREG(0x004E ,0x0000),
	PANACMOS_WREG(0x004F ,0x0002),
	PANACMOS_WREG(0x0050 ,0x0000),
	PANACMOS_WREG(0x0051 ,0x0001),
	PANACMOS_WREG(0x0052 ,0x0003),
	PANACMOS_WREG(0x0053 ,0x0033),
	PANACMOS_WREG(0x0054 ,0x0000),
	PANACMOS_WREG(0x0055 ,0x0330),
	PANACMOS_WREG(0x0060 ,0x804A),
	PANACMOS_WREG(0x0061 ,0x6100),
	PANACMOS_WREG(0x0062 ,0x0140),
	PANACMOS_WREG(0x0063 ,0x0000),
	PANACMOS_WREG(0x0064 ,0x0004),
	PANACMOS_WREG(0x0065 ,0x0003),
	PANACMOS_WREG(0x0066 ,0x0000),
	PANACMOS_WREG(0x0067 ,0x0174),
	PANACMOS_WREG(0x0068 ,0x0001),
	PANACMOS_WREG(0x0069 ,0x1400),
	PANACMOS_WREG(0x006A ,0x0001),
	PANACMOS_WREG(0x006B ,0x000C),
	PANACMOS_WREG(0x006C ,0x3400),
	PANACMOS_WREG(0x006D ,0x2AA2),
	PANACMOS_WREG(0x006E ,0x5000),
	PANACMOS_WREG(0x006F ,0xAA2A),
	PANACMOS_WREG(0x0070 ,0xA800),
	PANACMOS_WREG(0x0071 ,0x2000),
	PANACMOS_WREG(0x0072 ,0x000D),
	PANACMOS_WREG(0x0073 ,0x5854),
	PANACMOS_WREG(0x0074 ,0x0101),
	PANACMOS_WREG(0x0075 ,0xFC30),
	PANACMOS_WREG(0x0076 ,0x3FED),
	PANACMOS_WREG(0x0077 ,0x0034),
	PANACMOS_WREG(0x0078 ,0x2380),
	PANACMOS_WREG(0x0079 ,0x0000),
	PANACMOS_WREG(0x007A ,0x0000),
	PANACMOS_WREG(0x007B ,0x00B2),
	PANACMOS_WREG(0x007C ,0x01F5),
	PANACMOS_WREG(0x007D ,0x0000),
	PANACMOS_WREG(0x007E ,0x0005),
	PANACMOS_WREG(0x007F ,0x00BA),
	PANACMOS_WREG(0x0080 ,0x0000),
	PANACMOS_WREG(0x0081 ,0x0032),
	PANACMOS_WREG(0x0082 ,0x0064),
	PANACMOS_WREG(0x0083 ,0x0096),
	PANACMOS_WREG(0x0084 ,0x0000),
	/*## TG_REG ##*/
	PANACMOS_WREG(0x0100 ,0x034F),
	PANACMOS_WREG(0x0101 ,0x034F),
	PANACMOS_WREG(0x0102 ,0x0002),
	PANACMOS_WREG(0x0103 ,0x0002),
	PANACMOS_WREG(0x0104 ,0x0005),
	PANACMOS_WREG(0x0105 ,0x0005),
	PANACMOS_WREG(0x0106 ,0x0005),
	PANACMOS_WREG(0x0107 ,0x0005),
	PANACMOS_WREG(0x0108 ,0x0004),
	PANACMOS_WREG(0x0109 ,0x02CA),
	PANACMOS_WREG(0x010A ,0x0C00),
	PANACMOS_WREG(0x010B ,0x3020),
	PANACMOS_WREG(0x010C ,0x000C),
	PANACMOS_WREG(0x010D ,0x0020),
	PANACMOS_WREG(0x010E ,0x003C),
	PANACMOS_WREG(0x010F ,0x0238),
	PANACMOS_WREG(0x0110 ,0x0000),
	PANACMOS_WREG(0x0111 ,0x0238),
	PANACMOS_WREG(0x0112 ,0x0000),
	PANACMOS_WREG(0x0113 ,0x000A),
	PANACMOS_WREG(0x0114 ,0x0000),
	PANACMOS_WREG(0x0115 ,0x0000),
	PANACMOS_WREG(0x0116 ,0x0000),
	PANACMOS_WREG(0x0117 ,0x0239),
	PANACMOS_WREG(0x0118 ,0x0343),
	PANACMOS_WREG(0x0119 ,0x036C),
	PANACMOS_WREG(0x011A ,0x012A),
	PANACMOS_WREG(0x011B ,0x0139),
	PANACMOS_WREG(0x011C ,0x0112),
	PANACMOS_WREG(0x011D ,0x011F),
	PANACMOS_WREG(0x011E ,0x0800),
	PANACMOS_WREG(0x011F ,0x0000),
	PANACMOS_WREG(0x0120 ,0x0000),
	PANACMOS_WREG(0x0121 ,0x0C07),
	PANACMOS_WREG(0x0122 ,0x0405),
	PANACMOS_WREG(0x0123 ,0x0000),
	PANACMOS_WREG(0x0124 ,0x0004),
	PANACMOS_WREG(0x0125 ,0x03A0),
	PANACMOS_WREG(0x0126 ,0x2165),
	PANACMOS_WREG(0x0127 ,0xC2E2),
	PANACMOS_WREG(0x0128 ,0x0000),
	PANACMOS_WREG(0x0129 ,0x000F),
	PANACMOS_WREG(0x012A ,0x0000),
	PANACMOS_WREG(0x012B ,0x000F),
	PANACMOS_WREG(0x012C ,0x000F),
	PANACMOS_WREG(0x012D ,0x0056),
	PANACMOS_WREG(0x012E ,0x0000),
	PANACMOS_WREG(0x012F ,0x0800),
	PANACMOS_WREG(0x0130 ,0x0000),
	PANACMOS_WREG(0x0131 ,0x0000),
	PANACMOS_WREG(0x0132 ,0x0004),
	PANACMOS_WREG(0x0133 ,0x00B4),
	PANACMOS_WREG(0x0134 ,0x033C),
	PANACMOS_WREG(0x0135 ,0x033C),
	PANACMOS_WREG(0x0136 ,0x0339),
	PANACMOS_WREG(0x0137 ,0x0339),
	PANACMOS_WREG(0x0138 ,0x0369),
	PANACMOS_WREG(0x0139 ,0x0369),
	PANACMOS_WREG(0x013A ,0x0000),
	PANACMOS_WREG(0x013B ,0x0020),
	PANACMOS_WREG(0x013C ,0x0181),
	PANACMOS_WREG(0x013D ,0x003C),
	PANACMOS_WREG(0x013E ,0x3400),
	PANACMOS_WREG(0x013F ,0x0055),
	PANACMOS_WREG(0x0140 ,0x0053),
	PANACMOS_WREG(0x0141 ,0x00AE),
	PANACMOS_WREG(0x0142 ,0x0000),
	PANACMOS_WREG(0x0143 ,0x004E),
	PANACMOS_WREG(0x0144 ,0x00BE),
	PANACMOS_WREG(0x0145 ,0x01F8),
	PANACMOS_WREG(0x0146 ,0x033C),
	PANACMOS_WREG(0x0147 ,0x033C),
	PANACMOS_WREG(0x0148 ,0x0004),
	PANACMOS_WREG(0x0149 ,0x00BE),
	PANACMOS_WREG(0x014A ,0x000F),
	PANACMOS_WREG(0x014B ,0x001D),
	PANACMOS_WREG(0x014C ,0x01F8),
	PANACMOS_WREG(0x014D ,0x0010),
	PANACMOS_WREG(0x014E ,0x0C00),
	PANACMOS_WREG(0x014F ,0x001B),
	PANACMOS_WREG(0x0150 ,0x1107),
	PANACMOS_WREG(0x0151 ,0x0030),
	PANACMOS_WREG(0x0152 ,0x1B0C),
	PANACMOS_WREG(0x0153 ,0x1106),
	PANACMOS_WREG(0x0154 ,0x006A),
	PANACMOS_WREG(0x0155 ,0x00DA),
	PANACMOS_WREG(0x0156 ,0x006A),
	PANACMOS_WREG(0x0157 ,0x00DA),
	PANACMOS_WREG(0x0158 ,0x0005),
	PANACMOS_WREG(0x0159 ,0x3032),
	PANACMOS_WREG(0x015A ,0x0030),
	PANACMOS_WREG(0x015B ,0x0000),
	PANACMOS_WREG(0x015C ,0x3002),
	PANACMOS_WREG(0x015D ,0x1003),
	PANACMOS_WREG(0x015E ,0x1F70),
	PANACMOS_WREG(0x015F ,0x0032),
	PANACMOS_WREG(0x0160 ,0x3007),
	PANACMOS_WREG(0x0161 ,0x1212),
	PANACMOS_WREG(0x0162 ,0x0000),
	PANACMOS_WREG(0x0163 ,0x003D),
	PANACMOS_WREG(0x0164 ,0x00E6),
	PANACMOS_WREG(0x0165 ,0x0300),
	PANACMOS_WREG(0x0166 ,0x000F),
	PANACMOS_WREG(0x0167 ,0x000F),
	PANACMOS_WREG(0x0168 ,0x000F),
	PANACMOS_WREG(0x0169 ,0x000F),
	PANACMOS_WREG(0x016A ,0x000F),
	PANACMOS_WREG(0x016B ,0x000F),
	PANACMOS_WREG(0x016C ,0x000F),
	PANACMOS_WREG(0x016D ,0x000F),
	PANACMOS_WREG(0x016E ,0x0000),
	PANACMOS_WREG(0x016F ,0x001E),
	PANACMOS_WREG(0x0170 ,0x0100),
	PANACMOS_WREG(0x0171 ,0x005F),
	PANACMOS_WREG(0x0172 ,0x01ED),
	PANACMOS_WREG(0x0173 ,0x005F),
	PANACMOS_WREG(0x0174 ,0x01ED),
	PANACMOS_WREG(0x0175 ,0x0700),
	PANACMOS_WREG(0x0176 ,0x11F9),
	PANACMOS_WREG(0x0177 ,0x3233),
	PANACMOS_WREG(0x0178 ,0x0405),
	PANACMOS_WREG(0x0179 ,0x0011),
	PANACMOS_WREG(0x017A ,0x0202),
	PANACMOS_WREG(0x017B ,0x0707),
	PANACMOS_WREG(0x017C ,0x0F05),
	PANACMOS_WREG(0x017D ,0x0007),
	PANACMOS_WREG(0x017E ,0x000E),
	PANACMOS_WREG(0x017F ,0x000C),
	PANACMOS_WREG(0x0180 ,0x000C),
	PANACMOS_WREG(0x0181 ,0x0006),
	PANACMOS_WREG(0x0182 ,0x0000),
	PANACMOS_WREG(0x0183 ,0x0000),
	PANACMOS_WREG(0x0184 ,0x0000),
	PANACMOS_WREG(0x0185 ,0x0100),
	PANACMOS_WREG(0x0186 ,0x0801),
	PANACMOS_WREG(0x0187 ,0x0300),
	PANACMOS_WREG(0x0188 ,0x1803),
	PANACMOS_WREG(0x0189 ,0x0202),
	PANACMOS_WREG(0x018A ,0x6000),
	PANACMOS_WREG(0x018B ,0x3003),
	PANACMOS_WREG(0x018C ,0x0000),
	PANACMOS_WREG(0x018D ,0x032A),
	PANACMOS_WREG(0x018E ,0x032A),
	PANACMOS_WREG(0x018F ,0x0007),
	PANACMOS_WREG(0x0190 ,0x0019),
	PANACMOS_WREG(0x0191 ,0x0007),
	PANACMOS_WREG(0x0192 ,0x000D),
	PANACMOS_WREG(0x0193 ,0x0000),
	PANACMOS_WREG(0x0195 ,0x0000),
	PANACMOS_WREG(0x0196 ,0x0000),
	PANACMOS_WREG(0x0197 ,0x0002),
	PANACMOS_WREG(0x0198 ,0x000E),
	PANACMOS_WREG(0x0199 ,0x0008),
	PANACMOS_WREG(0x019A ,0x0002),
	PANACMOS_WREG(0x019B ,0x0004),
	PANACMOS_WREG(0x019C ,0x0000),
	PANACMOS_WREG(0x019D ,0x0A06),
	PANACMOS_WREG(0x019E ,0x0F1A),
	PANACMOS_WREG(0x019F ,0x0000),
	PANACMOS_WREG(0x01A0 ,0x0003),
	PANACMOS_WREG(0x01A1 ,0x0000),
	PANACMOS_WREG(0x01A2 ,0x0000),
	PANACMOS_WREG(0x01A3 ,0x0003),
	PANACMOS_WREG(0x01A4 ,0x0000),
	PANACMOS_WREG(0x01A5 ,0x0000),
	PANACMOS_WREG(0x01A6 ,0x0000),
	PANACMOS_WREG(0x01A7 ,0x0000),
	PANACMOS_WREG(0x01A8 ,0x0036),
	PANACMOS_WREG(0x01A9 ,0x0000),
	PANACMOS_WREG(0x01AA ,0x0036),
	PANACMOS_WREG(0x01AB ,0x0000),
	PANACMOS_WREG(0x01AC ,0x0000),
	PANACMOS_WREG(0x01AD ,0x0000),
	PANACMOS_WREG(0x01AE ,0x0056),
	PANACMOS_WREG(0x01AF ,0x0059),
	PANACMOS_WREG(0x01B0 ,0x00F0),
	PANACMOS_WREG(0x01B1 ,0x00FF),
	PANACMOS_WREG(0x01B2 ,0x001B),
	PANACMOS_WREG(0x01B3 ,0x0014),
	PANACMOS_WREG(0x01B4 ,0x6F07),
	PANACMOS_WREG(0x01B5 ,0x7707),
	PANACMOS_WREG(0x01B6 ,0x007A),
	PANACMOS_WREG(0x01B7 ,0x0188),
	PANACMOS_WREG(0x01B8 ,0x7179),
	PANACMOS_WREG(0x01B9 ,0x00F7),
	PANACMOS_WREG(0x01BA ,0x0177),
	PANACMOS_WREG(0x01BB ,0x1DFC),
	PANACMOS_WREG(0x01BC ,0x3D2C),
	PANACMOS_WREG(0x01BD ,0x4444),
	PANACMOS_WREG(0x01BE ,0x4B4B),
	PANACMOS_WREG(0x01BF ,0x0058),
	PANACMOS_WREG(0x01C0 ,0x0069),
	PANACMOS_WREG(0x01C1 ,0x0070),
	PANACMOS_WREG(0x01C2 ,0x0DF9),
	PANACMOS_WREG(0x01C3 ,0x0E00),
	PANACMOS_WREG(0x01C4 ,0x0E09),
	PANACMOS_WREG(0x01C5 ,0x0052),
	PANACMOS_WREG(0x01C6 ,0x0057),
	PANACMOS_WREG(0x01C7 ,0x0E18),
	PANACMOS_WREG(0x01C8 ,0x0E1B),
	PANACMOS_WREG(0x01C9 ,0x3F03),
	PANACMOS_WREG(0x01CA ,0x3F00),
	PANACMOS_WREG(0x01CB ,0x0007),
	PANACMOS_WREG(0x01CC ,0x0000),
	PANACMOS_WREG(0x01CD ,0x0000),
	PANACMOS_WREG(0x01CE ,0x0707),
	PANACMOS_WREG(0x01CF ,0x0700),
	PANACMOS_WREG(0x01D0 ,0x0007),
	PANACMOS_WREG(0x01D1 ,0x0108),
	PANACMOS_WREG(0x01D2 ,0x0129),
	PANACMOS_WREG(0x01D3 ,0x314A),
	PANACMOS_WREG(0x01D4 ,0x35AC),
	PANACMOS_WREG(0x01D5 ,0x0000),
	PANACMOS_WREG(0x01D6 ,0x0000),
	PANACMOS_WREG(0x01D7 ,0x0000),
	PANACMOS_WREG(0x01D8 ,0x0000),
	PANACMOS_WREG(0x01D9 ,0x0000),
	PANACMOS_WREG(0x01DA ,0x0000),
	PANACMOS_WREG(0x01DB ,0x0000),
	PANACMOS_WREG(0x01DC ,0x0000),
	PANACMOS_WREG(0x01DD ,0x0000),
	PANACMOS_WREG(0x01DE ,0x0000),
	PANACMOS_WREG(0x01DF ,0x0108),
	PANACMOS_WREG(0x01E0 ,0x0108),
	PANACMOS_WREG(0x01E1 ,0x0000),
	PANACMOS_WREG(0x01E2 ,0x3180),
	PANACMOS_WREG(0x01E3 ,0x018C),
	PANACMOS_WREG(0x01E4 ,0x0000),
	PANACMOS_WREG(0x01E5 ,0x0000),
	PANACMOS_WREG(0x01E6 ,0x2100),
	PANACMOS_WREG(0x01E7 ,0x2100),
	PANACMOS_WREG(0x01E8 ,0x0000),
	PANACMOS_WREG(0x01E9 ,0x3000),
	PANACMOS_WREG(0x01EA ,0x318C),
	PANACMOS_WREG(0x01EB ,0x0000),
	PANACMOS_WREG(0x01EC ,0x0000),
	PANACMOS_WREG(0x01ED ,0x0000),
	PANACMOS_WREG(0x01EE ,0x0000),
	PANACMOS_WREG(0x01EF ,0x041C),
	PANACMOS_WREG(0x01F0 ,0x0055),
	PANACMOS_WREG(0x01F1 ,0x0000),
	PANACMOS_WREG(0x01F2 ,0x0000),
	PANACMOS_WREG(0x01F3 ,0x0000),
	PANACMOS_WREG(0x01F4 ,0x0000),
	PANACMOS_WREG(0x0300 ,0x02CB),
	PANACMOS_WREG(0x0301 ,0x0000),
	PANACMOS_WREG(0x0302 ,0x0000),
	PANACMOS_WREG(0x0303 ,0x0000),
	PANACMOS_WREG(0x0304 ,0x0000),
	PANACMOS_WREG(0x0305 ,0x0238),
	PANACMOS_WREG(0x0306 ,0x0000),
	PANACMOS_WREG(0x0307 ,0x000A),
	PANACMOS_WREG(0x0308 ,0x0000),
	PANACMOS_WREG(0x0309 ,0x0000),
	PANACMOS_WREG(0x030A ,0x0000),
	PANACMOS_WREG(0x030B ,0x0239),
	/*## TG_RST ##*/
	PANACMOS_WREG(0x0000 ,0x0173),
	PANACMOS_WREG(0x0000 ,0x0173),
	PANACMOS_WREG(0x0000 ,0x0173),                       
};

static const UINT8 afe_vga2_code[][4]={
/* BATCH MODE6
# VCYCLE:1226 HCYCLE:1638 (MCLK 54M)
#_DSC_08_V3_3mix_H3mix_12bit_v_Ver28b_120313
## PLL_REG ## */
	PANACMOS_WREG( 0x0008 ,0x42B9),
	PANACMOS_WREG( 0x000F ,0x7FFF),
	PANACMOS_WREG( 0x0001 ,0x0030),
	PANACMOS_WREG( 0x0002 ,0x0003),
	PANACMOS_WREG( 0x0003 ,0x07E9),
	PANACMOS_WREG( 0x0000 ,0x0003),
	PANACMOS_WREG( 0x0004 ,0x0083),
	PANACMOS_WREG( 0x0006 ,0x11DC),
	PANACMOS_WREG( 0x0009 ,0x02C0),
	PANACMOS_WREG( 0x0005 ,0x0001),
	PANACMOS_WREG( 0x0007 ,0x7FFF),
	PANACMOS_WREG( 0x000A ,0x0000),
	PANACMOS_WREG( 0x000B ,0x0100),
	PANACMOS_WREG( 0x000C ,0x0000),
	PANACMOS_WREG( 0x000D ,0x2090),
	PANACMOS_WREG( 0x000E ,0x0103),
	PANACMOS_WREG( 0x0010 ,0x0000),
	PANACMOS_WREG( 0x0011 ,0x0001),
	PANACMOS_WREG( 0x0000 ,0x0053),
	PANACMOS_WREG( 0x0008 ,0x02B8),
	/*## SYS_REG ##*/
	PANACMOS_WREG( 0x0020 ,0x6105),
	PANACMOS_WREG( 0x0021 ,0x1012),
	PANACMOS_WREG( 0x0022 ,0x2555),
	PANACMOS_WREG( 0x0023 ,0x0222),
	PANACMOS_WREG( 0x0024 ,0x0222),
	PANACMOS_WREG( 0x0025 ,0x1375),
	PANACMOS_WREG( 0x0026 ,0x0333),
	PANACMOS_WREG( 0x0027 ,0x0444),
	PANACMOS_WREG( 0x0028 ,0x8390),
	PANACMOS_WREG( 0x0029 ,0x0000),
	PANACMOS_WREG( 0x002A ,0x0000),
	PANACMOS_WREG( 0x002B ,0x50F0),
	PANACMOS_WREG( 0x002C ,0x0014),
	PANACMOS_WREG( 0x002D ,0x0007),
	PANACMOS_WREG( 0x002E ,0x0000),
	PANACMOS_WREG( 0x0030 ,0x4700),
	PANACMOS_WREG( 0x0031 ,0x0055),
	PANACMOS_WREG( 0x0032 ,0x8386),
	PANACMOS_WREG( 0x0033 ,0x6662),
	PANACMOS_WREG( 0x0034 ,0x0066),
	PANACMOS_WREG( 0x0035 ,0x0000),
	PANACMOS_WREG( 0x0036 ,0x0000),
	PANACMOS_WREG( 0x0037 ,0x0000),
	PANACMOS_WREG( 0x0038 ,0x0000),
	PANACMOS_WREG( 0x0039 ,0x0000),
	PANACMOS_WREG( 0x003A ,0x01FF),
	PANACMOS_WREG( 0x003B ,0x30FF),
	PANACMOS_WREG( 0x003C ,0x0FFE),
	PANACMOS_WREG( 0x003D ,0x0001),
	PANACMOS_WREG( 0x003E ,0x001C),
	PANACMOS_WREG( 0x003F ,0x0031),
	PANACMOS_WREG( 0x0040 ,0x0001),
	PANACMOS_WREG( 0x0041 ,0x0300),
	PANACMOS_WREG( 0x0042 ,0x0808),
	PANACMOS_WREG( 0x0043 ,0x0A0A),
	PANACMOS_WREG( 0x0044 ,0x0000),
	PANACMOS_WREG( 0x0045 ,0x0000),
	PANACMOS_WREG( 0x0046 ,0x0000),
	PANACMOS_WREG( 0x0047 ,0x0800),
	PANACMOS_WREG( 0x0048 ,0x4310),
	PANACMOS_WREG( 0x0049 ,0x679A),
	PANACMOS_WREG( 0x004A ,0x2010),
	PANACMOS_WREG( 0x004B ,0x6703),
	PANACMOS_WREG( 0x004C ,0x0450),
	PANACMOS_WREG( 0x004D ,0x0000),
	PANACMOS_WREG( 0x004E ,0x0000),
	PANACMOS_WREG( 0x004F ,0x0002),
	PANACMOS_WREG( 0x0050 ,0x0000),
	PANACMOS_WREG( 0x0051 ,0x0001),
	PANACMOS_WREG( 0x0052 ,0x0003),
	PANACMOS_WREG( 0x0053 ,0x0012),
	PANACMOS_WREG( 0x0054 ,0x0000),
	PANACMOS_WREG( 0x0055 ,0x0330),
	PANACMOS_WREG( 0x0060 ,0x8012),
	PANACMOS_WREG( 0x0061 ,0x6100),
	PANACMOS_WREG( 0x0062 ,0x0140),
	PANACMOS_WREG( 0x0063 ,0x0000),
	PANACMOS_WREG( 0x0064 ,0x0004),
	PANACMOS_WREG( 0x0065 ,0x0003),
	PANACMOS_WREG( 0x0066 ,0x0000),
	PANACMOS_WREG( 0x0067 ,0x0174),
	PANACMOS_WREG( 0x0068 ,0x0003),
	PANACMOS_WREG( 0x0069 ,0x1400),
	PANACMOS_WREG( 0x006A ,0x0001),
	PANACMOS_WREG( 0x006B ,0x000C),
	PANACMOS_WREG( 0x006C ,0x3400),
	PANACMOS_WREG( 0x006D ,0x2AA2),
	PANACMOS_WREG( 0x006E ,0x5000),
	PANACMOS_WREG( 0x006F ,0xAA2A),
	PANACMOS_WREG( 0x0070 ,0xA800),
	PANACMOS_WREG( 0x0071 ,0x2000),
	PANACMOS_WREG( 0x0072 ,0x000D),
	PANACMOS_WREG( 0x0073 ,0x5854),
	PANACMOS_WREG( 0x0074 ,0x0101),
	PANACMOS_WREG( 0x0075 ,0xFC30),
	PANACMOS_WREG( 0x0076 ,0x3FED),
	PANACMOS_WREG( 0x0077 ,0x0034),
	PANACMOS_WREG( 0x0078 ,0x2380),
	PANACMOS_WREG( 0x0079 ,0x0000),
	PANACMOS_WREG( 0x007A ,0x0000),
	PANACMOS_WREG( 0x007B ,0x00B2),
	PANACMOS_WREG( 0x007C ,0x008E),
	PANACMOS_WREG( 0x007D ,0x0000),
	PANACMOS_WREG( 0x007E ,0x0005),
	PANACMOS_WREG( 0x007F ,0x00BA),
	PANACMOS_WREG( 0x0080 ,0x0000),
	PANACMOS_WREG( 0x0081 ,0x0032),
	PANACMOS_WREG( 0x0082 ,0x0064),
	PANACMOS_WREG( 0x0083 ,0x0096),
	PANACMOS_WREG( 0x0084 ,0x0000),
	/*## TG_REG ##*/
	PANACMOS_WREG( 0x0100 ,0x0557),
	PANACMOS_WREG( 0x0101 ,0x0331),
	PANACMOS_WREG( 0x0102 ,0x0002),
	PANACMOS_WREG( 0x0103 ,0x0002),
	PANACMOS_WREG( 0x0104 ,0x0005),
	PANACMOS_WREG( 0x0105 ,0x0005),
	PANACMOS_WREG( 0x0106 ,0x0005),
	PANACMOS_WREG( 0x0107 ,0x0005),
	PANACMOS_WREG( 0x0108 ,0x0004),
	PANACMOS_WREG( 0x0109 ,0x04C9),
	PANACMOS_WREG( 0x010A ,0x0800),
	PANACMOS_WREG( 0x010B ,0x1C14),
	PANACMOS_WREG( 0x010C ,0x0008),
	PANACMOS_WREG( 0x010D ,0x0014),
	PANACMOS_WREG( 0x010E ,0x0028),
	PANACMOS_WREG( 0x010F ,0x04B8),
	PANACMOS_WREG( 0x0110 ,0x0000),
	PANACMOS_WREG( 0x0111 ,0x04B8),
	PANACMOS_WREG( 0x0112 ,0x0000),
	PANACMOS_WREG( 0x0113 ,0x000A),
	PANACMOS_WREG( 0x0114 ,0x0000),
	PANACMOS_WREG( 0x0115 ,0x0000),
	PANACMOS_WREG( 0x0116 ,0x0000),
	PANACMOS_WREG( 0x0117 ,0x04B9),
	PANACMOS_WREG( 0x0118 ,0x031C),
	PANACMOS_WREG( 0x0119 ,0x0117),
	PANACMOS_WREG( 0x011A ,0x012A),
	PANACMOS_WREG( 0x011B ,0x0139),
	PANACMOS_WREG( 0x011C ,0x0117),
	PANACMOS_WREG( 0x011D ,0x011F),
	PANACMOS_WREG( 0x011E ,0x0800),
	PANACMOS_WREG( 0x011F ,0x0000),
	PANACMOS_WREG( 0x0120 ,0x0000),
	PANACMOS_WREG( 0x0121 ,0x0C07),
	PANACMOS_WREG( 0x0122 ,0x0405),
	PANACMOS_WREG( 0x0123 ,0x0000),
	PANACMOS_WREG( 0x0124 ,0x0004),
	PANACMOS_WREG( 0x0125 ,0x03A0),
	PANACMOS_WREG( 0x0126 ,0x2165),
	PANACMOS_WREG( 0x0127 ,0x0C07),
	PANACMOS_WREG( 0x0128 ,0x0000),
	PANACMOS_WREG( 0x0129 ,0x000F),
	PANACMOS_WREG( 0x012A ,0x0000),
	PANACMOS_WREG( 0x012B ,0x000F),
	PANACMOS_WREG( 0x012C ,0x000F),
	PANACMOS_WREG( 0x012D ,0x0056),
	PANACMOS_WREG( 0x012E ,0x0000),
	PANACMOS_WREG( 0x012F ,0x0800),
	PANACMOS_WREG( 0x0130 ,0x0000),
	PANACMOS_WREG( 0x0131 ,0x0000),
	PANACMOS_WREG( 0x0132 ,0x0004),
	PANACMOS_WREG( 0x0133 ,0x00B4),
	PANACMOS_WREG( 0x0134 ,0x0122),
	PANACMOS_WREG( 0x0135 ,0x0122),
	PANACMOS_WREG( 0x0136 ,0x012D),
	PANACMOS_WREG( 0x0137 ,0x012D),
	PANACMOS_WREG( 0x0138 ,0x011D),
	PANACMOS_WREG( 0x0139 ,0x011D),
	PANACMOS_WREG( 0x013A ,0x0000),
	PANACMOS_WREG( 0x013B ,0x0020),
	PANACMOS_WREG( 0x013C ,0x0181),
	PANACMOS_WREG( 0x013D ,0x003C),
	PANACMOS_WREG( 0x013E ,0x3400),
	PANACMOS_WREG( 0x013F ,0x0055),
	PANACMOS_WREG( 0x0140 ,0x0053),
	PANACMOS_WREG( 0x0141 ,0x00AE),
	PANACMOS_WREG( 0x0142 ,0x0000),
	PANACMOS_WREG( 0x0143 ,0x006F),
	PANACMOS_WREG( 0x0144 ,0x0182),
	PANACMOS_WREG( 0x0145 ,0x009E),
	PANACMOS_WREG( 0x0146 ,0x0122),
	PANACMOS_WREG( 0x0147 ,0x0122),
	PANACMOS_WREG( 0x0148 ,0x0004),
	PANACMOS_WREG( 0x0149 ,0x00BE),
	PANACMOS_WREG( 0x014A ,0x000F),
	PANACMOS_WREG( 0x014B ,0x001D),
	PANACMOS_WREG( 0x014C ,0x009E),
	PANACMOS_WREG( 0x014D ,0x0010),
	PANACMOS_WREG( 0x014E ,0x0C00),
	PANACMOS_WREG( 0x014F ,0x001B),
	PANACMOS_WREG( 0x0150 ,0x1107),
	PANACMOS_WREG( 0x0151 ,0x0030),
	PANACMOS_WREG( 0x0152 ,0x1B0C),
	PANACMOS_WREG( 0x0153 ,0x1106),
	PANACMOS_WREG( 0x0154 ,0x006A),
	PANACMOS_WREG( 0x0155 ,0x0051),
	PANACMOS_WREG( 0x0156 ,0x006A),
	PANACMOS_WREG( 0x0157 ,0x0051),
	PANACMOS_WREG( 0x0158 ,0x0005),
	PANACMOS_WREG( 0x0159 ,0x3032),
	PANACMOS_WREG( 0x015A ,0x0030),
	PANACMOS_WREG( 0x015B ,0x0000),
	PANACMOS_WREG( 0x015C ,0x3002),
	PANACMOS_WREG( 0x015D ,0x1003),
	PANACMOS_WREG( 0x015E ,0x1F70),
	PANACMOS_WREG( 0x015F ,0x0032),
	PANACMOS_WREG( 0x0160 ,0x3007),
	PANACMOS_WREG( 0x0161 ,0x1212),
	PANACMOS_WREG( 0x0162 ,0x0000),
	PANACMOS_WREG( 0x0163 ,0x003D),
	PANACMOS_WREG( 0x0164 ,0x00E6),
	PANACMOS_WREG( 0x0165 ,0x0300),
	PANACMOS_WREG( 0x0166 ,0x000F),
	PANACMOS_WREG( 0x0167 ,0x000F),
	PANACMOS_WREG( 0x0168 ,0x000F),
	PANACMOS_WREG( 0x0169 ,0x000F),
	PANACMOS_WREG( 0x016A ,0x000F),
	PANACMOS_WREG( 0x016B ,0x000F),
	PANACMOS_WREG( 0x016C ,0x000F),
	PANACMOS_WREG( 0x016D ,0x000F),
	PANACMOS_WREG( 0x016E ,0x0000),
	PANACMOS_WREG( 0x016F ,0x001E),
	PANACMOS_WREG( 0x0170 ,0x0100),
	PANACMOS_WREG( 0x0171 ,0x005F),
	PANACMOS_WREG( 0x0172 ,0x0190),
	PANACMOS_WREG( 0x0173 ,0x005F),
	PANACMOS_WREG( 0x0174 ,0x0190),
	PANACMOS_WREG( 0x0175 ,0x0700),
	PANACMOS_WREG( 0x0176 ,0x11F9),
	PANACMOS_WREG( 0x0177 ,0x3233),
	PANACMOS_WREG( 0x0178 ,0x0405),
	PANACMOS_WREG( 0x0179 ,0x0011),
	PANACMOS_WREG( 0x017A ,0x0202),
	PANACMOS_WREG( 0x017B ,0x0707),
	PANACMOS_WREG( 0x017C ,0x0F05),
	PANACMOS_WREG( 0x017D ,0x0007),
	PANACMOS_WREG( 0x017E ,0x000E),
	PANACMOS_WREG( 0x017F ,0x000C),
	PANACMOS_WREG( 0x0180 ,0x000C),
	PANACMOS_WREG( 0x0181 ,0x0006),
	PANACMOS_WREG( 0x0182 ,0x0000),
	PANACMOS_WREG( 0x0183 ,0x0000),
	PANACMOS_WREG( 0x0184 ,0x0000),
	PANACMOS_WREG( 0x0185 ,0x0100),
	PANACMOS_WREG( 0x0186 ,0x0800),
	PANACMOS_WREG( 0x0187 ,0x0300),
	PANACMOS_WREG( 0x0188 ,0x1800),
	PANACMOS_WREG( 0x0189 ,0x0202),
	PANACMOS_WREG( 0x018A ,0x6000),
	PANACMOS_WREG( 0x018B ,0x3003),
	PANACMOS_WREG( 0x018C ,0x0000),
	PANACMOS_WREG( 0x018D ,0x0129),
	PANACMOS_WREG( 0x018E ,0x0129),
	PANACMOS_WREG( 0x018F ,0x0007),
	PANACMOS_WREG( 0x0190 ,0x0019),
	PANACMOS_WREG( 0x0191 ,0x0007),
	PANACMOS_WREG( 0x0192 ,0x000D),
	PANACMOS_WREG( 0x0193 ,0x0000),
	PANACMOS_WREG( 0x0195 ,0x0000),
	PANACMOS_WREG( 0x0196 ,0x0000),
	PANACMOS_WREG( 0x0197 ,0x0002),
	PANACMOS_WREG( 0x0198 ,0x000E),
	PANACMOS_WREG( 0x0199 ,0x0008),
	PANACMOS_WREG( 0x019A ,0x0002),
	PANACMOS_WREG( 0x019B ,0x0004),
	PANACMOS_WREG( 0x019C ,0x0000),
	PANACMOS_WREG( 0x019D ,0x0A06),
	PANACMOS_WREG( 0x019E ,0x0F1A),
	PANACMOS_WREG( 0x019F ,0x0000),
	PANACMOS_WREG( 0x01A0 ,0x0003),
	PANACMOS_WREG( 0x01A1 ,0x0000),
	PANACMOS_WREG( 0x01A2 ,0x0000),
	PANACMOS_WREG( 0x01A3 ,0x0003),
	PANACMOS_WREG( 0x01A4 ,0x0000),
	PANACMOS_WREG( 0x01A5 ,0x0000),
	PANACMOS_WREG( 0x01A6 ,0x0000),
	PANACMOS_WREG( 0x01A7 ,0x0000),
	PANACMOS_WREG( 0x01A8 ,0x0036),
	PANACMOS_WREG( 0x01A9 ,0x0000),
	PANACMOS_WREG( 0x01AA ,0x0036),
	PANACMOS_WREG( 0x01AB ,0x0000),
	PANACMOS_WREG( 0x01AC ,0x0000),
	PANACMOS_WREG( 0x01AD ,0x0000),
	PANACMOS_WREG( 0x01AE ,0x0056),
	PANACMOS_WREG( 0x01AF ,0x0059),
	PANACMOS_WREG( 0x01B0 ,0x00F0),
	PANACMOS_WREG( 0x01B1 ,0x00FF),
	PANACMOS_WREG( 0x01B2 ,0x001B),
	PANACMOS_WREG( 0x01B3 ,0x0014),
	PANACMOS_WREG( 0x01B4 ,0x5B07),
	PANACMOS_WREG( 0x01B5 ,0xC707),
	PANACMOS_WREG( 0x01B6 ,0x004B),
	PANACMOS_WREG( 0x01B7 ,0x014A),
	PANACMOS_WREG( 0x01B8 ,0xCD4C),
	PANACMOS_WREG( 0x01B9 ,0x00F7),
	PANACMOS_WREG( 0x01BA ,0x0177),
	PANACMOS_WREG( 0x01BB ,0x2BFA),
	PANACMOS_WREG( 0x01BC ,0x3D2A),
	PANACMOS_WREG( 0x01BD ,0x433C),
	PANACMOS_WREG( 0x01BE ,0x4F48),
	PANACMOS_WREG( 0x01BF ,0x0054),
	PANACMOS_WREG( 0x01C0 ,0x006D),
	PANACMOS_WREG( 0x01C1 ,0x006C),
	PANACMOS_WREG( 0x01C2 ,0x0E05),
	PANACMOS_WREG( 0x01C3 ,0x0E04),
	PANACMOS_WREG( 0x01C4 ,0x0E17),
	PANACMOS_WREG( 0x01C5 ,0x0E16),
	PANACMOS_WREG( 0x01C6 ,0x0E1B),
	PANACMOS_WREG( 0x01C7 ,0x0FFF),
	PANACMOS_WREG( 0x01C8 ,0x0000),
	PANACMOS_WREG( 0x01C9 ,0x0202),
	PANACMOS_WREG( 0x01CA ,0x023F),
	PANACMOS_WREG( 0x01CB ,0x3F02),
	PANACMOS_WREG( 0x01CC ,0x0000),
	PANACMOS_WREG( 0x01CD ,0x0000),
	PANACMOS_WREG( 0x01CE ,0x0202),
	PANACMOS_WREG( 0x01CF ,0x023F),
	PANACMOS_WREG( 0x01D0 ,0x3F02),
	PANACMOS_WREG( 0x01D1 ,0x0000),
	PANACMOS_WREG( 0x01D2 ,0x0000),
	PANACMOS_WREG( 0x01D3 ,0x0000),
	PANACMOS_WREG( 0x01D4 ,0x0000),
	PANACMOS_WREG( 0x01D5 ,0x0000),
	PANACMOS_WREG( 0x01D6 ,0x0000),
	PANACMOS_WREG( 0x01D7 ,0x0000),
	PANACMOS_WREG( 0x01D8 ,0x0000),
	PANACMOS_WREG( 0x01D9 ,0x0000),
	PANACMOS_WREG( 0x01DA ,0x0000),
	PANACMOS_WREG( 0x01DB ,0x0000),
	PANACMOS_WREG( 0x01DC ,0x0000),
	PANACMOS_WREG( 0x01DD ,0x0000),
	PANACMOS_WREG( 0x01DE ,0x0000),
	PANACMOS_WREG( 0x01DF ,0x0000),
	PANACMOS_WREG( 0x01E0 ,0x0000),
	PANACMOS_WREG( 0x01E1 ,0x0000),
	PANACMOS_WREG( 0x01E2 ,0x0000),
	PANACMOS_WREG( 0x01E3 ,0x0000),
	PANACMOS_WREG( 0x01E4 ,0x0000),
	PANACMOS_WREG( 0x01E5 ,0x0000),
	PANACMOS_WREG( 0x01E6 ,0x0000),
	PANACMOS_WREG( 0x01E7 ,0x0000),
	PANACMOS_WREG( 0x01E8 ,0x0000),
	PANACMOS_WREG( 0x01E9 ,0x0000),
	PANACMOS_WREG( 0x01EA ,0x0000),
	PANACMOS_WREG( 0x01EB ,0x0000),
	PANACMOS_WREG( 0x01EC ,0x0000),
	PANACMOS_WREG( 0x01ED ,0x0000),
	PANACMOS_WREG( 0x01EE ,0x0000),
	PANACMOS_WREG( 0x01EF ,0x0010),
	PANACMOS_WREG( 0x01F0 ,0x0000),
	PANACMOS_WREG( 0x01F1 ,0x0000),
	PANACMOS_WREG( 0x01F2 ,0x0000),
	PANACMOS_WREG( 0x01F3 ,0x0000),
	PANACMOS_WREG( 0x01F4 ,0x0000),
	PANACMOS_WREG( 0x0300 ,0x04CA),
	PANACMOS_WREG( 0x0301 ,0x0000),
	PANACMOS_WREG( 0x0302 ,0x0000),
	PANACMOS_WREG( 0x0303 ,0x0000),
	PANACMOS_WREG( 0x0304 ,0x0000),
	PANACMOS_WREG( 0x0305 ,0x04B8),
	PANACMOS_WREG( 0x0306 ,0x0000),
	PANACMOS_WREG( 0x0307 ,0x000A),
	PANACMOS_WREG( 0x0308 ,0x0000),
	PANACMOS_WREG( 0x0309 ,0x0000),
	PANACMOS_WREG( 0x030A ,0x0000),
	PANACMOS_WREG( 0x030B ,0x04B9),
	/*## TG_RST ##*/
	PANACMOS_WREG( 0x0000 ,0x0173),
	PANACMOS_WREG( 0x0000 ,0x0173),
	PANACMOS_WREG( 0x0000 ,0x0173),                         
};


/*pana sensor mode 6*/
static const UINT8 afe_720P_code[][4]={
/* BATCH
# VCYCLE:1226 HCYCLE:1638 (MCLK 54M)
#_DSC_08_V3_3mix_H3mix_12bit_v_Ver28b_120313
## PLL_REG ## */
	PANACMOS_WREG( 0x0008 ,0x42B9),
	PANACMOS_WREG( 0x000F ,0x7FFF),
	PANACMOS_WREG( 0x0001 ,0x0030),
	PANACMOS_WREG( 0x0002 ,0x0003),
	PANACMOS_WREG( 0x0003 ,0x07E9),
	PANACMOS_WREG( 0x0000 ,0x0003),
	PANACMOS_WREG( 0x0004 ,0x0083),
	PANACMOS_WREG( 0x0006 ,0x11DC),
	PANACMOS_WREG( 0x0009 ,0x02C0),
	PANACMOS_WREG( 0x0005 ,0x0001),
	PANACMOS_WREG( 0x0007 ,0x7FFF),
	PANACMOS_WREG( 0x000A ,0x0000),
	PANACMOS_WREG( 0x000B ,0x0100),
	PANACMOS_WREG( 0x000C ,0x0000),
	PANACMOS_WREG( 0x000D ,0x2090),
	PANACMOS_WREG( 0x000E ,0x0103),
	PANACMOS_WREG( 0x0010 ,0x0000),
	PANACMOS_WREG( 0x0011 ,0x0001),
	PANACMOS_WREG( 0x0000 ,0x0053),
	PANACMOS_WREG( 0x0008 ,0x02B8),
	/*## SYS_REG ##*/
	PANACMOS_WREG( 0x0020 ,0x6105),
	PANACMOS_WREG( 0x0021 ,0x1012),
	PANACMOS_WREG( 0x0022 ,0x2555),
	PANACMOS_WREG( 0x0023 ,0x0222),
	PANACMOS_WREG( 0x0024 ,0x0222),
	PANACMOS_WREG( 0x0025 ,0x1375),
	PANACMOS_WREG( 0x0026 ,0x0333),
	PANACMOS_WREG( 0x0027 ,0x0444),
	PANACMOS_WREG( 0x0028 ,0x8390),
	PANACMOS_WREG( 0x0029 ,0x0000),
	PANACMOS_WREG( 0x002A ,0x0000),
	PANACMOS_WREG( 0x002B ,0x50F0),
	PANACMOS_WREG( 0x002C ,0x0014),
	PANACMOS_WREG( 0x002D ,0x0007),
	PANACMOS_WREG( 0x002E ,0x0000),
	PANACMOS_WREG( 0x0030 ,0x4700),
	PANACMOS_WREG( 0x0031 ,0x0055),
	PANACMOS_WREG( 0x0032 ,0x8386),
	PANACMOS_WREG( 0x0033 ,0x6662),
	PANACMOS_WREG( 0x0034 ,0x0066),
	PANACMOS_WREG( 0x0035 ,0x0000),
	PANACMOS_WREG( 0x0036 ,0x0000),
	PANACMOS_WREG( 0x0037 ,0x0000),
	PANACMOS_WREG( 0x0038 ,0x0000),
	PANACMOS_WREG( 0x0039 ,0x0000),
	PANACMOS_WREG( 0x003A ,0x01FF),
	PANACMOS_WREG( 0x003B ,0x30FF),
	PANACMOS_WREG( 0x003C ,0x0FFE),
	PANACMOS_WREG( 0x003D ,0x0001),
	PANACMOS_WREG( 0x003E ,0x001C),
	PANACMOS_WREG( 0x003F ,0x0031),
	PANACMOS_WREG( 0x0040 ,0x0001),
	PANACMOS_WREG( 0x0041 ,0x0300),
	PANACMOS_WREG( 0x0042 ,0x0808),
	PANACMOS_WREG( 0x0043 ,0x0A0A),
	PANACMOS_WREG( 0x0044 ,0x0000),
	PANACMOS_WREG( 0x0045 ,0x0000),
	PANACMOS_WREG( 0x0046 ,0x0000),
	PANACMOS_WREG( 0x0047 ,0x0800),
	PANACMOS_WREG( 0x0048 ,0x4310),
	PANACMOS_WREG( 0x0049 ,0x679A),
	PANACMOS_WREG( 0x004A ,0x2010),
	PANACMOS_WREG( 0x004B ,0x6703),
	PANACMOS_WREG( 0x004C ,0x0450),
	PANACMOS_WREG( 0x004D ,0x0000),
	PANACMOS_WREG( 0x004E ,0x0000),
	PANACMOS_WREG( 0x004F ,0x0002),
	PANACMOS_WREG( 0x0050 ,0x0000),
	PANACMOS_WREG( 0x0051 ,0x0001),
	PANACMOS_WREG( 0x0052 ,0x0003),
	PANACMOS_WREG( 0x0053 ,0x0012),
	PANACMOS_WREG( 0x0054 ,0x0000),
	PANACMOS_WREG( 0x0055 ,0x0330),
	PANACMOS_WREG( 0x0060 ,0x8012),
	PANACMOS_WREG( 0x0061 ,0x6100),
	PANACMOS_WREG( 0x0062 ,0x0140),
	PANACMOS_WREG( 0x0063 ,0x0000),
	PANACMOS_WREG( 0x0064 ,0x0004),
	PANACMOS_WREG( 0x0065 ,0x0003),
	PANACMOS_WREG( 0x0066 ,0x0000),
	PANACMOS_WREG( 0x0067 ,0x0174),
	PANACMOS_WREG( 0x0068 ,0x0003),
	PANACMOS_WREG( 0x0069 ,0x1400),
	PANACMOS_WREG( 0x006A ,0x0001),
	PANACMOS_WREG( 0x006B ,0x000C),
	PANACMOS_WREG( 0x006C ,0x3400),
	PANACMOS_WREG( 0x006D ,0x2AA2),
	PANACMOS_WREG( 0x006E ,0x5000),
	PANACMOS_WREG( 0x006F ,0xAA2A),
	PANACMOS_WREG( 0x0070 ,0xA800),
	PANACMOS_WREG( 0x0071 ,0x2000),
	PANACMOS_WREG( 0x0072 ,0x000D),
	PANACMOS_WREG( 0x0073 ,0x5854),
	PANACMOS_WREG( 0x0074 ,0x0101),
	PANACMOS_WREG( 0x0075 ,0xFC30),
	PANACMOS_WREG( 0x0076 ,0x3FED),
	PANACMOS_WREG( 0x0077 ,0x0034),
	PANACMOS_WREG( 0x0078 ,0x2380),
	PANACMOS_WREG( 0x0079 ,0x0000),
	PANACMOS_WREG( 0x007A ,0x0000),
	PANACMOS_WREG( 0x007B ,0x00B2),
	PANACMOS_WREG( 0x007C ,0x008E),
	PANACMOS_WREG( 0x007D ,0x0000),
	PANACMOS_WREG( 0x007E ,0x0005),
	PANACMOS_WREG( 0x007F ,0x00BA),
	PANACMOS_WREG( 0x0080 ,0x0000),
	PANACMOS_WREG( 0x0081 ,0x0032),
	PANACMOS_WREG( 0x0082 ,0x0064),
	PANACMOS_WREG( 0x0083 ,0x0096),
	PANACMOS_WREG( 0x0084 ,0x0000),
	/*## TG_REG ##*/
	PANACMOS_WREG( 0x0100 ,0x0557),
	PANACMOS_WREG( 0x0101 ,0x0331),
	PANACMOS_WREG( 0x0102 ,0x0002),
	PANACMOS_WREG( 0x0103 ,0x0002),
	PANACMOS_WREG( 0x0104 ,0x0005),
	PANACMOS_WREG( 0x0105 ,0x0005),
	PANACMOS_WREG( 0x0106 ,0x0005),
	PANACMOS_WREG( 0x0107 ,0x0005),
	PANACMOS_WREG( 0x0108 ,0x0004),
	PANACMOS_WREG( 0x0109 ,0x04C9),
	PANACMOS_WREG( 0x010A ,0x0800),
	PANACMOS_WREG( 0x010B ,0x1C14),
	PANACMOS_WREG( 0x010C ,0x0008),
	PANACMOS_WREG( 0x010D ,0x0014),
	PANACMOS_WREG( 0x010E ,0x0028),
	PANACMOS_WREG( 0x010F ,0x04B8),
	PANACMOS_WREG( 0x0110 ,0x0000),
	PANACMOS_WREG( 0x0111 ,0x04B8),
	PANACMOS_WREG( 0x0112 ,0x0000),
	PANACMOS_WREG( 0x0113 ,0x000A),
	PANACMOS_WREG( 0x0114 ,0x0000),
	PANACMOS_WREG( 0x0115 ,0x0000),
	PANACMOS_WREG( 0x0116 ,0x0000),
	PANACMOS_WREG( 0x0117 ,0x04B9),
	PANACMOS_WREG( 0x0118 ,0x031C),
	PANACMOS_WREG( 0x0119 ,0x0117),
	PANACMOS_WREG( 0x011A ,0x012A),
	PANACMOS_WREG( 0x011B ,0x0139),
	PANACMOS_WREG( 0x011C ,0x0117),
	PANACMOS_WREG( 0x011D ,0x011F),
	PANACMOS_WREG( 0x011E ,0x0800),
	PANACMOS_WREG( 0x011F ,0x0000),
	PANACMOS_WREG( 0x0120 ,0x0000),
	PANACMOS_WREG( 0x0121 ,0x0C07),
	PANACMOS_WREG( 0x0122 ,0x0405),
	PANACMOS_WREG( 0x0123 ,0x0000),
	PANACMOS_WREG( 0x0124 ,0x0004),
	PANACMOS_WREG( 0x0125 ,0x03A0),
	PANACMOS_WREG( 0x0126 ,0x2165),
	PANACMOS_WREG( 0x0127 ,0x0C07),
	PANACMOS_WREG( 0x0128 ,0x0000),
	PANACMOS_WREG( 0x0129 ,0x000F),
	PANACMOS_WREG( 0x012A ,0x0000),
	PANACMOS_WREG( 0x012B ,0x000F),
	PANACMOS_WREG( 0x012C ,0x000F),
	PANACMOS_WREG( 0x012D ,0x0056),
	PANACMOS_WREG( 0x012E ,0x0000),
	PANACMOS_WREG( 0x012F ,0x0800),
	PANACMOS_WREG( 0x0130 ,0x0000),
	PANACMOS_WREG( 0x0131 ,0x0000),
	PANACMOS_WREG( 0x0132 ,0x0004),
	PANACMOS_WREG( 0x0133 ,0x00B4),
	PANACMOS_WREG( 0x0134 ,0x0122),
	PANACMOS_WREG( 0x0135 ,0x0122),
	PANACMOS_WREG( 0x0136 ,0x012D),
	PANACMOS_WREG( 0x0137 ,0x012D),
	PANACMOS_WREG( 0x0138 ,0x011D),
	PANACMOS_WREG( 0x0139 ,0x011D),
	PANACMOS_WREG( 0x013A ,0x0000),
	PANACMOS_WREG( 0x013B ,0x0020),
	PANACMOS_WREG( 0x013C ,0x0181),
	PANACMOS_WREG( 0x013D ,0x003C),
	PANACMOS_WREG( 0x013E ,0x3400),
	PANACMOS_WREG( 0x013F ,0x0055),
	PANACMOS_WREG( 0x0140 ,0x0053),
	PANACMOS_WREG( 0x0141 ,0x00AE),
	PANACMOS_WREG( 0x0142 ,0x0000),
	PANACMOS_WREG( 0x0143 ,0x006F),
	PANACMOS_WREG( 0x0144 ,0x0182),
	PANACMOS_WREG( 0x0145 ,0x009E),
	PANACMOS_WREG( 0x0146 ,0x0122),
	PANACMOS_WREG( 0x0147 ,0x0122),
	PANACMOS_WREG( 0x0148 ,0x0004),
	PANACMOS_WREG( 0x0149 ,0x00BE),
	PANACMOS_WREG( 0x014A ,0x000F),
	PANACMOS_WREG( 0x014B ,0x001D),
	PANACMOS_WREG( 0x014C ,0x009E),
	PANACMOS_WREG( 0x014D ,0x0010),
	PANACMOS_WREG( 0x014E ,0x0C00),
	PANACMOS_WREG( 0x014F ,0x001B),
	PANACMOS_WREG( 0x0150 ,0x1107),
	PANACMOS_WREG( 0x0151 ,0x0030),
	PANACMOS_WREG( 0x0152 ,0x1B0C),
	PANACMOS_WREG( 0x0153 ,0x1106),
	PANACMOS_WREG( 0x0154 ,0x006A),
	PANACMOS_WREG( 0x0155 ,0x0051),
	PANACMOS_WREG( 0x0156 ,0x006A),
	PANACMOS_WREG( 0x0157 ,0x0051),
	PANACMOS_WREG( 0x0158 ,0x0005),
	PANACMOS_WREG( 0x0159 ,0x3032),
	PANACMOS_WREG( 0x015A ,0x0030),
	PANACMOS_WREG( 0x015B ,0x0000),
	PANACMOS_WREG( 0x015C ,0x3002),
	PANACMOS_WREG( 0x015D ,0x1003),
	PANACMOS_WREG( 0x015E ,0x1F70),
	PANACMOS_WREG( 0x015F ,0x0032),
	PANACMOS_WREG( 0x0160 ,0x3007),
	PANACMOS_WREG( 0x0161 ,0x1212),
	PANACMOS_WREG( 0x0162 ,0x0000),
	PANACMOS_WREG( 0x0163 ,0x003D),
	PANACMOS_WREG( 0x0164 ,0x00E6),
	PANACMOS_WREG( 0x0165 ,0x0300),
	PANACMOS_WREG( 0x0166 ,0x000F),
	PANACMOS_WREG( 0x0167 ,0x000F),
	PANACMOS_WREG( 0x0168 ,0x000F),
	PANACMOS_WREG( 0x0169 ,0x000F),
	PANACMOS_WREG( 0x016A ,0x000F),
	PANACMOS_WREG( 0x016B ,0x000F),
	PANACMOS_WREG( 0x016C ,0x000F),
	PANACMOS_WREG( 0x016D ,0x000F),
	PANACMOS_WREG( 0x016E ,0x0000),
	PANACMOS_WREG( 0x016F ,0x001E),
	PANACMOS_WREG( 0x0170 ,0x0100),
	PANACMOS_WREG( 0x0171 ,0x005F),
	PANACMOS_WREG( 0x0172 ,0x0190),
	PANACMOS_WREG( 0x0173 ,0x005F),
	PANACMOS_WREG( 0x0174 ,0x0190),
	PANACMOS_WREG( 0x0175 ,0x0700),
	PANACMOS_WREG( 0x0176 ,0x11F9),
	PANACMOS_WREG( 0x0177 ,0x3233),
	PANACMOS_WREG( 0x0178 ,0x0405),
	PANACMOS_WREG( 0x0179 ,0x0011),
	PANACMOS_WREG( 0x017A ,0x0202),
	PANACMOS_WREG( 0x017B ,0x0707),
	PANACMOS_WREG( 0x017C ,0x0F05),
	PANACMOS_WREG( 0x017D ,0x0007),
	PANACMOS_WREG( 0x017E ,0x000E),
	PANACMOS_WREG( 0x017F ,0x000C),
	PANACMOS_WREG( 0x0180 ,0x000C),
	PANACMOS_WREG( 0x0181 ,0x0006),
	PANACMOS_WREG( 0x0182 ,0x0000),
	PANACMOS_WREG( 0x0183 ,0x0000),
	PANACMOS_WREG( 0x0184 ,0x0000),
	PANACMOS_WREG( 0x0185 ,0x0100),
	PANACMOS_WREG( 0x0186 ,0x0800),
	PANACMOS_WREG( 0x0187 ,0x0300),
	PANACMOS_WREG( 0x0188 ,0x1800),
	PANACMOS_WREG( 0x0189 ,0x0202),
	PANACMOS_WREG( 0x018A ,0x6000),
	PANACMOS_WREG( 0x018B ,0x3003),
	PANACMOS_WREG( 0x018C ,0x0000),
	PANACMOS_WREG( 0x018D ,0x0129),
	PANACMOS_WREG( 0x018E ,0x0129),
	PANACMOS_WREG( 0x018F ,0x0007),
	PANACMOS_WREG( 0x0190 ,0x0019),
	PANACMOS_WREG( 0x0191 ,0x0007),
	PANACMOS_WREG( 0x0192 ,0x000D),
	PANACMOS_WREG( 0x0193 ,0x0000),
	PANACMOS_WREG( 0x0195 ,0x0000),
	PANACMOS_WREG( 0x0196 ,0x0000),
	PANACMOS_WREG( 0x0197 ,0x0002),
	PANACMOS_WREG( 0x0198 ,0x000E),
	PANACMOS_WREG( 0x0199 ,0x0008),
	PANACMOS_WREG( 0x019A ,0x0002),
	PANACMOS_WREG( 0x019B ,0x0004),
	PANACMOS_WREG( 0x019C ,0x0000),
	PANACMOS_WREG( 0x019D ,0x0A06),
	PANACMOS_WREG( 0x019E ,0x0F1A),
	PANACMOS_WREG( 0x019F ,0x0000),
	PANACMOS_WREG( 0x01A0 ,0x0003),
	PANACMOS_WREG( 0x01A1 ,0x0000),
	PANACMOS_WREG( 0x01A2 ,0x0000),
	PANACMOS_WREG( 0x01A3 ,0x0003),
	PANACMOS_WREG( 0x01A4 ,0x0000),
	PANACMOS_WREG( 0x01A5 ,0x0000),
	PANACMOS_WREG( 0x01A6 ,0x0000),
	PANACMOS_WREG( 0x01A7 ,0x0000),
	PANACMOS_WREG( 0x01A8 ,0x0036),
	PANACMOS_WREG( 0x01A9 ,0x0000),
	PANACMOS_WREG( 0x01AA ,0x0036),
	PANACMOS_WREG( 0x01AB ,0x0000),
	PANACMOS_WREG( 0x01AC ,0x0000),
	PANACMOS_WREG( 0x01AD ,0x0000),
	PANACMOS_WREG( 0x01AE ,0x0056),
	PANACMOS_WREG( 0x01AF ,0x0059),
	PANACMOS_WREG( 0x01B0 ,0x00F0),
	PANACMOS_WREG( 0x01B1 ,0x00FF),
	PANACMOS_WREG( 0x01B2 ,0x001B),
	PANACMOS_WREG( 0x01B3 ,0x0014),
	PANACMOS_WREG( 0x01B4 ,0x5B07),
	PANACMOS_WREG( 0x01B5 ,0xC707),
	PANACMOS_WREG( 0x01B6 ,0x004B),
	PANACMOS_WREG( 0x01B7 ,0x014A),
	PANACMOS_WREG( 0x01B8 ,0xCD4C),
	PANACMOS_WREG( 0x01B9 ,0x00F7),
	PANACMOS_WREG( 0x01BA ,0x0177),
	PANACMOS_WREG( 0x01BB ,0x2BFA),
	PANACMOS_WREG( 0x01BC ,0x3D2A),
	PANACMOS_WREG( 0x01BD ,0x433C),
	PANACMOS_WREG( 0x01BE ,0x4F48),
	PANACMOS_WREG( 0x01BF ,0x0054),
	PANACMOS_WREG( 0x01C0 ,0x006D),
	PANACMOS_WREG( 0x01C1 ,0x006C),
	PANACMOS_WREG( 0x01C2 ,0x0E05),
	PANACMOS_WREG( 0x01C3 ,0x0E04),
	PANACMOS_WREG( 0x01C4 ,0x0E17),
	PANACMOS_WREG( 0x01C5 ,0x0E16),
	PANACMOS_WREG( 0x01C6 ,0x0E1B),
	PANACMOS_WREG( 0x01C7 ,0x0FFF),
	PANACMOS_WREG( 0x01C8 ,0x0000),
	PANACMOS_WREG( 0x01C9 ,0x0202),
	PANACMOS_WREG( 0x01CA ,0x023F),
	PANACMOS_WREG( 0x01CB ,0x3F02),
	PANACMOS_WREG( 0x01CC ,0x0000),
	PANACMOS_WREG( 0x01CD ,0x0000),
	PANACMOS_WREG( 0x01CE ,0x0202),
	PANACMOS_WREG( 0x01CF ,0x023F),
	PANACMOS_WREG( 0x01D0 ,0x3F02),
	PANACMOS_WREG( 0x01D1 ,0x0000),
	PANACMOS_WREG( 0x01D2 ,0x0000),
	PANACMOS_WREG( 0x01D3 ,0x0000),
	PANACMOS_WREG( 0x01D4 ,0x0000),
	PANACMOS_WREG( 0x01D5 ,0x0000),
	PANACMOS_WREG( 0x01D6 ,0x0000),
	PANACMOS_WREG( 0x01D7 ,0x0000),
	PANACMOS_WREG( 0x01D8 ,0x0000),
	PANACMOS_WREG( 0x01D9 ,0x0000),
	PANACMOS_WREG( 0x01DA ,0x0000),
	PANACMOS_WREG( 0x01DB ,0x0000),
	PANACMOS_WREG( 0x01DC ,0x0000),
	PANACMOS_WREG( 0x01DD ,0x0000),
	PANACMOS_WREG( 0x01DE ,0x0000),
	PANACMOS_WREG( 0x01DF ,0x0000),
	PANACMOS_WREG( 0x01E0 ,0x0000),
	PANACMOS_WREG( 0x01E1 ,0x0000),
	PANACMOS_WREG( 0x01E2 ,0x0000),
	PANACMOS_WREG( 0x01E3 ,0x0000),
	PANACMOS_WREG( 0x01E4 ,0x0000),
	PANACMOS_WREG( 0x01E5 ,0x0000),
	PANACMOS_WREG( 0x01E6 ,0x0000),
	PANACMOS_WREG( 0x01E7 ,0x0000),
	PANACMOS_WREG( 0x01E8 ,0x0000),
	PANACMOS_WREG( 0x01E9 ,0x0000),
	PANACMOS_WREG( 0x01EA ,0x0000),
	PANACMOS_WREG( 0x01EB ,0x0000),
	PANACMOS_WREG( 0x01EC ,0x0000),
	PANACMOS_WREG( 0x01ED ,0x0000),
	PANACMOS_WREG( 0x01EE ,0x0000),
	PANACMOS_WREG( 0x01EF ,0x0010),
	PANACMOS_WREG( 0x01F0 ,0x0000),
	PANACMOS_WREG( 0x01F1 ,0x0000),
	PANACMOS_WREG( 0x01F2 ,0x0000),
	PANACMOS_WREG( 0x01F3 ,0x0000),
	PANACMOS_WREG( 0x01F4 ,0x0000),
	PANACMOS_WREG( 0x0300 ,0x04CA),
	PANACMOS_WREG( 0x0301 ,0x0000),
	PANACMOS_WREG( 0x0302 ,0x0000),
	PANACMOS_WREG( 0x0303 ,0x0000),
	PANACMOS_WREG( 0x0304 ,0x0000),
	PANACMOS_WREG( 0x0305 ,0x04B8),
	PANACMOS_WREG( 0x0306 ,0x0000),
	PANACMOS_WREG( 0x0307 ,0x000A),
	PANACMOS_WREG( 0x0308 ,0x0000),
	PANACMOS_WREG( 0x0309 ,0x0000),
	PANACMOS_WREG( 0x030A ,0x0000),
	PANACMOS_WREG( 0x030B ,0x04B9),
	/*## TG_RST ##*/
	PANACMOS_WREG( 0x0000 ,0x0173),
	PANACMOS_WREG( 0x0000 ,0x0173),
	PANACMOS_WREG( 0x0000 ,0x0173),                         
};

static const UINT8 afe_1080P_code[][4]={
/*BATCH MODE 3
# VCYCLE:1814 HCYCLE:792 (MCLK 54M)
#_DSC_05_V2_2mix_H2mix_12bit_v_Ver28b_120313
## PLL_REG ##*/
	PANACMOS_WREG( 0x0008 ,0x42B9),
	PANACMOS_WREG( 0x000F ,0x7FFF),
	PANACMOS_WREG( 0x0001 ,0x0030),
	PANACMOS_WREG( 0x0002 ,0x0003),
	PANACMOS_WREG( 0x0003 ,0x07E9),
	PANACMOS_WREG( 0x0000 ,0x0003),
	PANACMOS_WREG( 0x0004 ,0x0083),
	PANACMOS_WREG( 0x0006 ,0x11DC),
	PANACMOS_WREG( 0x0009 ,0x02C0),
	PANACMOS_WREG( 0x0005 ,0x0001),
	PANACMOS_WREG( 0x0007 ,0x7FFF),
	PANACMOS_WREG( 0x000A ,0x0000),
	PANACMOS_WREG( 0x000B ,0x0100),
	PANACMOS_WREG( 0x000C ,0x0000),
	PANACMOS_WREG( 0x000D ,0x2090),
	PANACMOS_WREG( 0x000E ,0x0103),
	PANACMOS_WREG( 0x0010 ,0x0000),
	PANACMOS_WREG( 0x0011 ,0x0001),
	PANACMOS_WREG( 0x0000 ,0x0053),
	PANACMOS_WREG( 0x0008 ,0x02B8),
	/*## SYS_REG ##*/
	PANACMOS_WREG( 0x0020 ,0x6103),
	PANACMOS_WREG( 0x0021 ,0x1012),
	PANACMOS_WREG( 0x0022 ,0x2555),
	PANACMOS_WREG( 0x0023 ,0x0222),
	PANACMOS_WREG( 0x0024 ,0x0222),
	PANACMOS_WREG( 0x0025 ,0x1375),
	PANACMOS_WREG( 0x0026 ,0x0333),
	PANACMOS_WREG( 0x0027 ,0x0444),
	PANACMOS_WREG( 0x0028 ,0x8390),
	PANACMOS_WREG( 0x0029 ,0x0000),
	PANACMOS_WREG( 0x002A ,0x0000),
	PANACMOS_WREG( 0x002B ,0x00F0),
	PANACMOS_WREG( 0x002C ,0x0014),
	PANACMOS_WREG( 0x002D ,0x0007),
	PANACMOS_WREG( 0x002E ,0x0000),
	PANACMOS_WREG( 0x0030 ,0x4700),
	PANACMOS_WREG( 0x0031 ,0x0055),
	PANACMOS_WREG( 0x0032 ,0x8386),
	PANACMOS_WREG( 0x0033 ,0x6662),
	PANACMOS_WREG( 0x0034 ,0x0066),
	PANACMOS_WREG( 0x0035 ,0x0000),
	PANACMOS_WREG( 0x0036 ,0x0000),
	PANACMOS_WREG( 0x0037 ,0x0000),
	PANACMOS_WREG( 0x0038 ,0x0000),
	PANACMOS_WREG( 0x0039 ,0x0000),
	PANACMOS_WREG( 0x003A ,0x01FF),
	PANACMOS_WREG( 0x003B ,0x30FF),
	PANACMOS_WREG( 0x003C ,0x0FFE),
	PANACMOS_WREG( 0x003D ,0x0001),
	PANACMOS_WREG( 0x003E ,0x001C),
	PANACMOS_WREG( 0x003F ,0x0030),
	PANACMOS_WREG( 0x0040 ,0x0001),
	PANACMOS_WREG( 0x0041 ,0x0300),
	PANACMOS_WREG( 0x0042 ,0x0808),
	PANACMOS_WREG( 0x0043 ,0x070A),
	PANACMOS_WREG( 0x0044 ,0x0000),
	PANACMOS_WREG( 0x0045 ,0x0000),
	PANACMOS_WREG( 0x0046 ,0x0000),
	PANACMOS_WREG( 0x0047 ,0x0800),
	PANACMOS_WREG( 0x0048 ,0x4310),
	PANACMOS_WREG( 0x0049 ,0x679A),
	PANACMOS_WREG( 0x004A ,0x2010),
	PANACMOS_WREG( 0x004B ,0x6703),
	PANACMOS_WREG( 0x004C ,0x0450),
	PANACMOS_WREG( 0x004D ,0x0000),
	PANACMOS_WREG( 0x004E ,0x0000),
	PANACMOS_WREG( 0x004F ,0x0002),
	PANACMOS_WREG( 0x0050 ,0x0000),
	PANACMOS_WREG( 0x0051 ,0x0001),
	PANACMOS_WREG( 0x0052 ,0x0003),
	PANACMOS_WREG( 0x0053 ,0x0012),
	PANACMOS_WREG( 0x0054 ,0x0000),
	PANACMOS_WREG( 0x0055 ,0x0330),
	PANACMOS_WREG( 0x0060 ,0x8049),
	PANACMOS_WREG( 0x0061 ,0x6100),
	PANACMOS_WREG( 0x0062 ,0x0140),
	PANACMOS_WREG( 0x0063 ,0x0000),
	PANACMOS_WREG( 0x0064 ,0x0004),
	PANACMOS_WREG( 0x0065 ,0x0003),
	PANACMOS_WREG( 0x0066 ,0x0000),
	PANACMOS_WREG( 0x0067 ,0x0174),
	PANACMOS_WREG( 0x0068 ,0x0003),
	PANACMOS_WREG( 0x0069 ,0x1400),
	PANACMOS_WREG( 0x006A ,0x0001),
	PANACMOS_WREG( 0x006B ,0x000C),
	PANACMOS_WREG( 0x006C ,0x3400),
	PANACMOS_WREG( 0x006D ,0x2AA2),
	PANACMOS_WREG( 0x006E ,0x5000),
	PANACMOS_WREG( 0x006F ,0xAA2A),
	PANACMOS_WREG( 0x0070 ,0xA800),
	PANACMOS_WREG( 0x0071 ,0x2000),
	PANACMOS_WREG( 0x0072 ,0x000D),
	PANACMOS_WREG( 0x0073 ,0x5854),
	PANACMOS_WREG( 0x0074 ,0x0101),
	PANACMOS_WREG( 0x0075 ,0xFC30),
	PANACMOS_WREG( 0x0076 ,0x3FED),
	PANACMOS_WREG( 0x0077 ,0x0034),
	PANACMOS_WREG( 0x0078 ,0x2380),
	PANACMOS_WREG( 0x0079 ,0x0000),
	PANACMOS_WREG( 0x007A ,0x0000),
	PANACMOS_WREG( 0x007B ,0x00B2),
	PANACMOS_WREG( 0x007C ,0x015B),
	PANACMOS_WREG( 0x007D ,0x0000),
	PANACMOS_WREG( 0x007E ,0x0005),
	PANACMOS_WREG( 0x007F ,0x00BA),
	PANACMOS_WREG( 0x0080 ,0x0000),
	PANACMOS_WREG( 0x0081 ,0x0032),
	PANACMOS_WREG( 0x0082 ,0x0064),
	PANACMOS_WREG( 0x0083 ,0x0096),
	PANACMOS_WREG( 0x0084 ,0x0000),
	/*## TG_REG ##*/
	PANACMOS_WREG( 0x0100 ,0x029C),
	PANACMOS_WREG( 0x0101 ,0x029C),
	PANACMOS_WREG( 0x0102 ,0x0002),
	PANACMOS_WREG( 0x0103 ,0x0002),
	PANACMOS_WREG( 0x0104 ,0x0005),
	PANACMOS_WREG( 0x0105 ,0x0005),
	PANACMOS_WREG( 0x0106 ,0x0005),
	PANACMOS_WREG( 0x0107 ,0x0005),
	PANACMOS_WREG( 0x0108 ,0x0004),
	PANACMOS_WREG( 0x0109 ,0x0715),
	PANACMOS_WREG( 0x010A ,0x0C00),
	PANACMOS_WREG( 0x010B ,0x281C),
	PANACMOS_WREG( 0x010C ,0x000C),
	PANACMOS_WREG( 0x010D ,0x001C),
	PANACMOS_WREG( 0x010E ,0x0034),
	PANACMOS_WREG( 0x010F ,0x070C),
	PANACMOS_WREG( 0x0110 ,0x0000),
	PANACMOS_WREG( 0x0111 ,0x070C),
	PANACMOS_WREG( 0x0112 ,0x0000),
	PANACMOS_WREG( 0x0113 ,0x000A),
	PANACMOS_WREG( 0x0114 ,0x0000),
	PANACMOS_WREG( 0x0115 ,0x0000),
	PANACMOS_WREG( 0x0116 ,0x0000),
	PANACMOS_WREG( 0x0117 ,0x070D),
	PANACMOS_WREG( 0x0118 ,0x03E2),
	PANACMOS_WREG( 0x0119 ,0x03CF),
	PANACMOS_WREG( 0x011A ,0x03FC),
	PANACMOS_WREG( 0x011B ,0x03EC),
	PANACMOS_WREG( 0x011C ,0x03A5),
	PANACMOS_WREG( 0x011D ,0x03B8),
	PANACMOS_WREG( 0x011E ,0x0800),
	PANACMOS_WREG( 0x011F ,0x0000),
	PANACMOS_WREG( 0x0120 ,0x0000),
	PANACMOS_WREG( 0x0121 ,0x0C07),
	PANACMOS_WREG( 0x0122 ,0x0405),
	PANACMOS_WREG( 0x0123 ,0x0000),
	PANACMOS_WREG( 0x0124 ,0x000C),
	PANACMOS_WREG( 0x0125 ,0x03A0),
	PANACMOS_WREG( 0x0126 ,0x216C),
	PANACMOS_WREG( 0x0127 ,0x0C07),
	PANACMOS_WREG( 0x0128 ,0x0000),
	PANACMOS_WREG( 0x0129 ,0x000F),
	PANACMOS_WREG( 0x012A ,0x0000),
	PANACMOS_WREG( 0x012B ,0x000F),
	PANACMOS_WREG( 0x012C ,0x000F),
	PANACMOS_WREG( 0x012D ,0x0056),
	PANACMOS_WREG( 0x012E ,0x0000),
	PANACMOS_WREG( 0x012F ,0x0800),
	PANACMOS_WREG( 0x0130 ,0x0000),
	PANACMOS_WREG( 0x0131 ,0x0000),
	PANACMOS_WREG( 0x0132 ,0x0004),
	PANACMOS_WREG( 0x0133 ,0x00B4),
	PANACMOS_WREG( 0x0134 ,0x0351),
	PANACMOS_WREG( 0x0135 ,0x0351),
	PANACMOS_WREG( 0x0136 ,0x0354),
	PANACMOS_WREG( 0x0137 ,0x0354),
	PANACMOS_WREG( 0x0138 ,0x03C4),
	PANACMOS_WREG( 0x0139 ,0x03C4),
	PANACMOS_WREG( 0x013A ,0x0000),
	PANACMOS_WREG( 0x013B ,0x0020),
	PANACMOS_WREG( 0x013C ,0x0181),
	PANACMOS_WREG( 0x013D ,0x003C),
	PANACMOS_WREG( 0x013E ,0x2900),
	PANACMOS_WREG( 0x013F ,0x0055),
	PANACMOS_WREG( 0x0140 ,0x0053),
	PANACMOS_WREG( 0x0141 ,0x00AE),
	PANACMOS_WREG( 0x0142 ,0x0000),
	PANACMOS_WREG( 0x0143 ,0x004E),
	PANACMOS_WREG( 0x0144 ,0x0182),
	PANACMOS_WREG( 0x0145 ,0x014D),
	PANACMOS_WREG( 0x0146 ,0x0351),
	PANACMOS_WREG( 0x0147 ,0x0351),
	PANACMOS_WREG( 0x0148 ,0x0004),
	PANACMOS_WREG( 0x0149 ,0x00BE),
	PANACMOS_WREG( 0x014A ,0x000F),
	PANACMOS_WREG( 0x014B ,0x001D),
	PANACMOS_WREG( 0x014C ,0x014D),
	PANACMOS_WREG( 0x014D ,0x0010),
	PANACMOS_WREG( 0x014E ,0x0C00),
	PANACMOS_WREG( 0x014F ,0x001B),
	PANACMOS_WREG( 0x0150 ,0x1107),
	PANACMOS_WREG( 0x0151 ,0x0030),
	PANACMOS_WREG( 0x0152 ,0x1B0C),
	PANACMOS_WREG( 0x0153 ,0x1106),
	PANACMOS_WREG( 0x0154 ,0x006A),
	PANACMOS_WREG( 0x0155 ,0x00E8),
	PANACMOS_WREG( 0x0156 ,0x006A),
	PANACMOS_WREG( 0x0157 ,0x00E8),
	PANACMOS_WREG( 0x0158 ,0x0005),
	PANACMOS_WREG( 0x0159 ,0x3032),
	PANACMOS_WREG( 0x015A ,0x0030),
	PANACMOS_WREG( 0x015B ,0x0000),
	PANACMOS_WREG( 0x015C ,0x3002),
	PANACMOS_WREG( 0x015D ,0x1003),
	PANACMOS_WREG( 0x015E ,0x1F70),
	PANACMOS_WREG( 0x015F ,0x0032),
	PANACMOS_WREG( 0x0160 ,0x3007),
	PANACMOS_WREG( 0x0161 ,0x1212),
	PANACMOS_WREG( 0x0162 ,0x0000),
	PANACMOS_WREG( 0x0163 ,0x003D),
	PANACMOS_WREG( 0x0164 ,0x00E6),
	PANACMOS_WREG( 0x0165 ,0x0300),
	PANACMOS_WREG( 0x0166 ,0x000F),
	PANACMOS_WREG( 0x0167 ,0x000F),
	PANACMOS_WREG( 0x0168 ,0x000F),
	PANACMOS_WREG( 0x0169 ,0x000F),
	PANACMOS_WREG( 0x016A ,0x000F),
	PANACMOS_WREG( 0x016B ,0x000F),
	PANACMOS_WREG( 0x016C ,0x000F),
	PANACMOS_WREG( 0x016D ,0x000F),
	PANACMOS_WREG( 0x016E ,0x0000),
	PANACMOS_WREG( 0x016F ,0x001E),
	PANACMOS_WREG( 0x0170 ,0x0100),
	PANACMOS_WREG( 0x0171 ,0x005F),
	PANACMOS_WREG( 0x0172 ,0x01ED),
	PANACMOS_WREG( 0x0173 ,0x005F),
	PANACMOS_WREG( 0x0174 ,0x01ED),
	PANACMOS_WREG( 0x0175 ,0x0700),
	PANACMOS_WREG( 0x0176 ,0x11F9),
	PANACMOS_WREG( 0x0177 ,0x3233),
	PANACMOS_WREG( 0x0178 ,0x0405),
	PANACMOS_WREG( 0x0179 ,0x0011),
	PANACMOS_WREG( 0x017A ,0x0202),
	PANACMOS_WREG( 0x017B ,0x0707),
	PANACMOS_WREG( 0x017C ,0x0F05),
	PANACMOS_WREG( 0x017D ,0x0007),
	PANACMOS_WREG( 0x017E ,0x000E),
	PANACMOS_WREG( 0x017F ,0x000C),
	PANACMOS_WREG( 0x0180 ,0x000C),
	PANACMOS_WREG( 0x0181 ,0x0006),
	PANACMOS_WREG( 0x0182 ,0x0000),
	PANACMOS_WREG( 0x0183 ,0x0000),
	PANACMOS_WREG( 0x0184 ,0x0000),
	PANACMOS_WREG( 0x0185 ,0x0100),
	PANACMOS_WREG( 0x0186 ,0x0800),
	PANACMOS_WREG( 0x0187 ,0x0300),
	PANACMOS_WREG( 0x0188 ,0x1800),
	PANACMOS_WREG( 0x0189 ,0x0202),
	PANACMOS_WREG( 0x018A ,0x6000),
	PANACMOS_WREG( 0x018B ,0x3003),
	PANACMOS_WREG( 0x018C ,0x0000),
	PANACMOS_WREG( 0x018D ,0x035A),
	PANACMOS_WREG( 0x018E ,0x035A),
	PANACMOS_WREG( 0x018F ,0x0007),
	PANACMOS_WREG( 0x0190 ,0x0019),
	PANACMOS_WREG( 0x0191 ,0x0007),
	PANACMOS_WREG( 0x0192 ,0x000D),
	PANACMOS_WREG( 0x0193 ,0x0000),
	PANACMOS_WREG( 0x0195 ,0x0000),
	PANACMOS_WREG( 0x0196 ,0x0000),
	PANACMOS_WREG( 0x0197 ,0x0002),
	PANACMOS_WREG( 0x0198 ,0x000E),
	PANACMOS_WREG( 0x0199 ,0x0008),
	PANACMOS_WREG( 0x019A ,0x0002),
	PANACMOS_WREG( 0x019B ,0x0004),
	PANACMOS_WREG( 0x019C ,0x0000),
	PANACMOS_WREG( 0x019D ,0x0A06),
	PANACMOS_WREG( 0x019E ,0x0F1A),
	PANACMOS_WREG( 0x019F ,0x0000),
	PANACMOS_WREG( 0x01A0 ,0x0003),
	PANACMOS_WREG( 0x01A1 ,0x0000),
	PANACMOS_WREG( 0x01A2 ,0x0000),
	PANACMOS_WREG( 0x01A3 ,0x0003),
	PANACMOS_WREG( 0x01A4 ,0x0000),
	PANACMOS_WREG( 0x01A5 ,0x0000),
	PANACMOS_WREG( 0x01A6 ,0x0000),
	PANACMOS_WREG( 0x01A7 ,0x0000),
	PANACMOS_WREG( 0x01A8 ,0x0036),
	PANACMOS_WREG( 0x01A9 ,0x0000),
	PANACMOS_WREG( 0x01AA ,0x0036),
	PANACMOS_WREG( 0x01AB ,0x0000),
	PANACMOS_WREG( 0x01AC ,0x0000),
	PANACMOS_WREG( 0x01AD ,0x0000),
	PANACMOS_WREG( 0x01AE ,0x0056),
	PANACMOS_WREG( 0x01AF ,0x0059),
	PANACMOS_WREG( 0x01B0 ,0x00F0),
	PANACMOS_WREG( 0x01B1 ,0x00FF),
	PANACMOS_WREG( 0x01B2 ,0x001B),
	PANACMOS_WREG( 0x01B3 ,0x0014),
	PANACMOS_WREG( 0x01B4 ,0x6F07),
	PANACMOS_WREG( 0x01B5 ,0x7707),
	PANACMOS_WREG( 0x01B6 ,0x007A),
	PANACMOS_WREG( 0x01B7 ,0x0188),
	PANACMOS_WREG( 0x01B8 ,0x7179),
	PANACMOS_WREG( 0x01B9 ,0x00F7),
	PANACMOS_WREG( 0x01BA ,0x0177),
	PANACMOS_WREG( 0x01BB ,0x27FE),
	PANACMOS_WREG( 0x01BC ,0x3F2A),
	PANACMOS_WREG( 0x01BD ,0x4342),
	PANACMOS_WREG( 0x01BE ,0x4B4A),
	PANACMOS_WREG( 0x01BF ,0x005E),
	PANACMOS_WREG( 0x01C0 ,0x0067),
	PANACMOS_WREG( 0x01C1 ,0x006A),
	PANACMOS_WREG( 0x01C2 ,0x0E03),
	PANACMOS_WREG( 0x01C3 ,0x0E06),
	PANACMOS_WREG( 0x01C4 ,0x0E13),
	PANACMOS_WREG( 0x01C5 ,0x0052),
	PANACMOS_WREG( 0x01C6 ,0x005D),
	PANACMOS_WREG( 0x01C7 ,0x0E16),
	PANACMOS_WREG( 0x01C8 ,0x0E1B),
	PANACMOS_WREG( 0x01C9 ,0x0301),
	PANACMOS_WREG( 0x01CA ,0x0100),
	PANACMOS_WREG( 0x01CB ,0x0003),
	PANACMOS_WREG( 0x01CC ,0x0000),
	PANACMOS_WREG( 0x01CD ,0x0000),
	PANACMOS_WREG( 0x01CE ,0x0301),
	PANACMOS_WREG( 0x01CF ,0x0100),
	PANACMOS_WREG( 0x01D0 ,0x0003),
	PANACMOS_WREG( 0x01D1 ,0x0000),
	PANACMOS_WREG( 0x01D2 ,0x0000),
	PANACMOS_WREG( 0x01D3 ,0x0000),
	PANACMOS_WREG( 0x01D4 ,0x0000),
	PANACMOS_WREG( 0x01D5 ,0x0000),
	PANACMOS_WREG( 0x01D6 ,0x0000),
	PANACMOS_WREG( 0x01D7 ,0x0000),
	PANACMOS_WREG( 0x01D8 ,0x0000),
	PANACMOS_WREG( 0x01D9 ,0x0000),
	PANACMOS_WREG( 0x01DA ,0x0000),
	PANACMOS_WREG( 0x01DB ,0x0000),
	PANACMOS_WREG( 0x01DC ,0x0000),
	PANACMOS_WREG( 0x01DD ,0x0000),
	PANACMOS_WREG( 0x01DE ,0x0000),
	PANACMOS_WREG( 0x01DF ,0x0000),
	PANACMOS_WREG( 0x01E0 ,0x0000),
	PANACMOS_WREG( 0x01E1 ,0x0000),
	PANACMOS_WREG( 0x01E2 ,0x0000),
	PANACMOS_WREG( 0x01E3 ,0x0000),
	PANACMOS_WREG( 0x01E4 ,0x0000),
	PANACMOS_WREG( 0x01E5 ,0x0000),
	PANACMOS_WREG( 0x01E6 ,0x0000),
	PANACMOS_WREG( 0x01E7 ,0x0000),
	PANACMOS_WREG( 0x01E8 ,0x0000),
	PANACMOS_WREG( 0x01E9 ,0x0000),
	PANACMOS_WREG( 0x01EA ,0x0000),
	PANACMOS_WREG( 0x01EB ,0x0000),
	PANACMOS_WREG( 0x01EC ,0x0000),
	PANACMOS_WREG( 0x01ED ,0x0000),
	PANACMOS_WREG( 0x01EE ,0x0000),
	PANACMOS_WREG( 0x01EF ,0x0018),
	PANACMOS_WREG( 0x01F0 ,0x0000),
	PANACMOS_WREG( 0x01F1 ,0x0000),
	PANACMOS_WREG( 0x01F2 ,0x0000),
	PANACMOS_WREG( 0x01F3 ,0x0000),
	PANACMOS_WREG( 0x01F4 ,0x0000),
	PANACMOS_WREG( 0x0300 ,0x0716),
	PANACMOS_WREG( 0x0301 ,0x0000),
	PANACMOS_WREG( 0x0302 ,0x0000),
	PANACMOS_WREG( 0x0303 ,0x0000),
	PANACMOS_WREG( 0x0304 ,0x0000),
	PANACMOS_WREG( 0x0305 ,0x070C),
	PANACMOS_WREG( 0x0306 ,0x0000),
	PANACMOS_WREG( 0x0307 ,0x000A),
	PANACMOS_WREG( 0x0308 ,0x0000),
	PANACMOS_WREG( 0x0309 ,0x0000),
	PANACMOS_WREG( 0x030A ,0x0000),
	PANACMOS_WREG( 0x030B ,0x070D),
	/*## TG_RST ##*/
	PANACMOS_WREG( 0x0000 ,0x0173),
	PANACMOS_WREG( 0x0000 ,0x0173),
	PANACMOS_WREG( 0x0000 ,0x0173),
                        
};

static const UINT8 afe_still_code[][4]={
};	

static const sensorLVDSCfg_t sensorLVDS_Prev4laneCfg = 
{
	.wordcntsel 	 =0 ,
	.Lvdsmode		 =1 ,
	.Lvdslanenum	 =0 ,/*4 lane mode*/
	.Lvdslanemode	 =0 ,
	.Lvdslaneswap	 =0 , /*LSB*/
	.Lvdsraw		 =1 ,
	.Lvdsoutsel 	 =0 ,
	.Lvdseofsel 	 =0 ,
	.Lvdseolsel 	 =1 , /*1: detect eol sync code   , 0: count pixel number equal as the register horizontal  */
	.vmode			 =1 , /*0: LVDS, 1:subLVDS */
	.lvdsvdsrc		 =1 ,
	.lvds_dphy_sel	 =0 ,/*d phy*/
	.S2pphase		 =4 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	.Ctrlword_sol	 =0x00,
	.Ctrlword_sof	 =0x02,
	.Ctrlword_eol    =0x01,
	.Ctrlword_eof    =0x03,
	.syncwrod0		 =0xB8 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	.Exthd_num		 =8 ,
	.Exthd_pol		 =0 ,
	.Exthd_mode 	 =0 ,
	.cropenable 	 =1 ,
};
static const sensorLVDSCfg_t sensorLVDS_Full4laneCfg = 
{
	.wordcntsel 	 =0 ,
	.Lvdsmode		 =1 ,
	.Lvdslanenum	 =0 ,/*4 lane mode*/
	.Lvdslanemode	 =0 ,
	.Lvdslaneswap	 =0 , /*LSB*/
	.Lvdsraw		 =1 ,
	.Lvdsoutsel 	 =0 ,
	.Lvdseofsel 	 =0 ,
	.Lvdseolsel 	 =1 , /*1: detect eol sync code   , 0: count pixel number equal as the register horizontal  */
	.vmode			 =1 , /*0: LVDS, 1:subLVDS */
	.lvdsvdsrc		 =1 ,
	.lvds_dphy_sel	 =0 ,/*d phy*/
	.S2pphase		 =4 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	.Ctrlword_sol	 =0x00,
	.Ctrlword_sof	 =0x02,
	.Ctrlword_eol    =0x01,
	.Ctrlword_eof    =0x03,
	.syncwrod0		 =0xB8 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	.Exthd_num		 =0 ,
	.Exthd_pol		 =0 ,
	.Exthd_mode 	 =0 ,
	.cropenable 	 =1 ,
};

/*orgin VGA size 30fps*/
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
	.fps_mhz = 29970000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 1540, 	
};
/*VGA size 60fps*/
static const sensorTgPreviewCfg_t sensorTgPreviewVGA60fpsCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGA60fpsBegin,
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
	.fps_mhz = 59940000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 936, 	
};
/*VGA size 120fps*/
static const sensorTgPreviewCfg_t sensorTgPreviewVGA120fpsCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGA120fpsBegin,
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
	.fps_mhz = 119880000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 936, 	
};
/*720P size 60fps*/
static const sensorTgPreviewCfg_t sensorTgPreview720P60fpsCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720P60fpsBegin,
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
	.fps_mhz = 59940000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 693, 	
};
/*720P size 60fps*/
static const sensorTgPreviewCfg_t sensorTgPreview720P30fpsCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720P30fpsBegin,
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
	.fps_mhz = 29970000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 693, 	
};
/*1080P size 30fps*/
static const sensorTgPreviewCfg_t sensorTgPreview1080PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
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
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 29970000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 936, 	
};

/* VGA dist mode@30fps (1080p30) */
static const sensorTgPreviewCfg_t sensorTgPreviewVGA30fpsDistCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = (PREVIEW_1080P_HSIZE-2112)/2,
		.yOffset = (PREVIEW_1080P_VSIZE-1728)/2,
		.xSize = /*PREVIEW_1080P_HSIZE*/2112,
		.ySize = /*PREVIEW_1080P_VSIZE*/1728,
		.yuvW = 704,
		.yuvH = 576,
	},
	.hsize = PREVIEW_1080P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 29970000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 936, 	
};

/* 1080P dist@30fps (1080p30) */
static const sensorTgPreviewCfg_t sensorTgPreview1080PDistCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = (PREVIEW_1080P_HSIZE-2112)/2,
		.yOffset = (PREVIEW_1080P_VSIZE-1276)/2,
		.xSize = /*PREVIEW_1080P_HSIZE*/2112,
		.ySize = /*PREVIEW_1080P_VSIZE*/1276,
		.yuvW = 1968,
		.yuvH = 1188,
	},
	.hsize = PREVIEW_1080P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 29970000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 936, 	
};

/* 720P dist@60fps (720p60) */
static const sensorTgPreviewCfg_t sensorTgPreview720P60fpsDistCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720P60fpsBegin,
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
		.yuvW = PREVIEW_720P_HSIZE,
		.yuvH = PREVIEW_720P_VSIZE,
	},
	.hsize = PREVIEW_720P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_720P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 59940000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 693, 	
};

/* 720P dist@30fps (1080p@30fps) */
static const sensorTgPreviewCfg_t sensorTgPreview720P30fpsDistCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = (PREVIEW_1080P_HSIZE-2112)/2,
		.yOffset = (PREVIEW_1080P_VSIZE-1296)/2,
		.xSize = /*PREVIEW_1080P_HSIZE*/2112,
		.ySize = /*PREVIEW_1080P_VSIZE*/1296,
		.yuvW = 1408,
		.yuvH = 864,
	},
	.hsize = PREVIEW_1080P_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 29970000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 936, 	
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
};


static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &sensorTgPreviewVGA30fpsCfg,		/*No.9      630x(715*3)	*/
	&sensorTgPreviewVGA120fpsCfg,  		/*No.9      630x715		*/
    &sensorTgPreview720P60fpsCfg,   	/*No.8      630x1430		*/
    &sensorTgPreview1080PCfg,  			/*No.3      792*2275		*/
	&sensorTgPreview720P30fpsCfg,   	/*No.8      630x(1430*2)	*/
};



static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewVGA120fpsCfg = {
 	  .hblank =64,
	  .htotal =630,
	  .vblank =2,
	  .vtotal =715,
};

static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewVGA60fpsCfg = {
 	  .hblank =64,
	  .htotal =630,
	  .vblank =2,
	  .vtotal =(715*2),
};

static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreviewVGA30fpsCfg = {
 	  .hblank =64,
	  .htotal =630,
	  .vblank =2,
	  .vtotal =(715*4),
};

static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreview720P60fpsCfg = {
 	  .hblank =64,
	  .htotal =630,
	  .vblank =2,
	  .vtotal =1430,
};

static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreview720P30fpsCfg = {
	  .hblank =64,
	  .htotal =630,
	  .vblank =2,
	  .vtotal =(1430*2),
};

static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenPreview1080PCfg = {
 	  .hblank =64,
	  .htotal =792,
	  .vblank =2,
	  .vtotal =2275,
};

static  sensorSyncSigGenSizeCfg_t sensorSyncSigGenCaptureFullCfg = {
 	  .hblank =64,
	  .htotal =918,
	  .vblank =2,
	  .vtotal =3620,
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

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
&sensorTgSnapFrameCfg,    
&sensorTgSnapRollCfg,    
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


static const sensorSyncSigCfg_t sensorSyncSigPreviewFULLCfg = {
    .linetotal       = CAPTURE_LINETOTAL,                                                
	.lineblank       = CAPTURE_LINEBLANK,    
	.hreshape_fall   = CAPTURE_HRESHAPE_FALL,                                          
	.hreshape_rise   = CAPTURE_HRESHAPE_RISE,
	.hoffset         = CAPTURE_HOFFSET,      
	
	.frametotal      = CAPTURE_FRAMETOTAL,                                                
	.frameblank      = CAPTURE_FRAMEBLANK,/*CAPTURE_VSIZE+1,*/
	.vreshape_fall   = CAPTURE_VRESHAPE_FALL,                                             
	.vreshape_rise   = CAPTURE_VRESHAPE_RISE,                                               
	.voffset         = CAPTURE_VOFFSET,                                                          
	.hsize           = CAPTURE_HSIZE + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
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

static  sensorSyncSigCfg_t sensorSyncSigSnapEarly3msCfg = {
      .linetotal       = CAPTURE_LINETOTAL,                                            
      .lineblank       = CAPTURE_LINEBLANK,     
      .hreshape_fall   = CAPTURE_HRESHAPE_FALL,                                       
      .hreshape_rise   = CAPTURE_HRESHAPE_RISE,
      .hoffset         = CAPTURE_HOFFSET,   
      
      .frametotal      = CAPTURE_FRAMETOTAL,                                                  
      .frameblank      = CAPTURE_FRAMEBLANK,/*CAPTURE_VSIZE+1,*/
      .vreshape_fall   = CAPTURE_EARLY3ms_VRESHAPE_FALL,                                            
      .vreshape_rise   = CAPTURE_EARLY3ms_VRESHAPE_RISE,                                                
      .voffset         = CAPTURE_EARLY3ms_VOFFSET,                                                      
      .hsize           = CAPTURE_HSIZE,                                                        
      .vsize           = CAPTURE_EARLY3ms_VSIZE,                                                      
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
      .hdnum_frame_end   = CAPTURE_EARLY3ms_HDNUM_FRAME_END,                                             
      .hdnum_cmdport     = CAPTURE_EARLY3ms_HDNUM_CMDPORT,              
};

static  sensorSyncSigCfg_t sensorSyncSigSnapEarly20msCfg = {
      .linetotal       = CAPTURE_LINETOTAL,                                            
      .lineblank       = CAPTURE_LINEBLANK,     
      .hreshape_fall   = CAPTURE_HRESHAPE_FALL,                                       
      .hreshape_rise   = CAPTURE_HRESHAPE_RISE,
      .hoffset         = CAPTURE_HOFFSET,   
      
      .frametotal      = CAPTURE_FRAMETOTAL,                                                  
      .frameblank      = CAPTURE_FRAMEBLANK,/*CAPTURE_VSIZE+1,*/
      .vreshape_fall   = CAPTURE_EARLY20ms_VRESHAPE_FALL,                                            
      .vreshape_rise   = CAPTURE_EARLY20ms_VRESHAPE_RISE,                                                
      .voffset         = CAPTURE_EARLY20ms_VOFFSET,                                                      
      .hsize           = CAPTURE_HSIZE,                                                        
      .vsize           = CAPTURE_EARLY20ms_VSIZE,                                                      
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
      .hdnum_frame_end   = CAPTURE_EARLY20ms_HDNUM_FRAME_END,                                             
      .hdnum_cmdport     = CAPTURE_EARLY20ms_HDNUM_CMDPORT,              
};


static  sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
      .linetotal       = CAPTURE_LINETOTAL,                                            
      .lineblank       = CAPTURE_LINEBLANK,     
      .hreshape_fall   = CAPTURE_HRESHAPE_FALL,                                       
      .hreshape_rise   = CAPTURE_HRESHAPE_RISE,
      .hoffset         = CAPTURE_HOFFSET,   
      
      .frametotal      = CAPTURE_FRAMETOTAL,                                                  
      .frameblank      = CAPTURE_FRAMEBLANK,/*CAPTURE_VSIZE+1,*/
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
		.stop = sensorDoFuncStop, /****TODO***/
		.pwrsave = NULL, /****TODO***/
		.expwrite = sensor_cmdport_exp_write,
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
		.crop = NULL, /****TODO***/
#if STREAMOF_ENABLE
		.streamoff = sensorDoFuncStreamOff,
		.streamon = sensorDoFuncStreamOn,
#endif	
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &DEF_SYNC_SIG_CFG,
	.verStr = MODEL_VERSION,
};

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


void sensor_vd_restart(void)
{
    WRITE8(0xb0009086,0);
    WRITE8(0xb0009010,1);
    WRITE8(0xb0009010,0);
    WRITE8(0xb0009086,1);
}

void sensor_vd_restart_snap_trigger(void)
{
    WRITE8(0xb0009086,0);
    WRITE8(0xb0009010,1);
    WRITE8(0xb0009010,0);
	halFrontSnapTrig(1);	
    WRITE8(0xb0009086,1);
}

UINT32 sensor_linecount_check(UINT32 timeout)
{
	UINT32 linecount1,linecount2;
	WRITE16(0xb0009064,1);
	linecount1=READ16(0xb0009064);
	tmrUsWait(timeout);
 	WRITE16(0xb0009064,1);
	linecount2=READ16(0xb0009064);
	SENSOR_PROF_LOG_PRINT(1,"line count check (1: %d   2: %d)",linecount1,linecount2);    
 	if(linecount2!=linecount1)
		return 1;
	else
		return 0;
}

void 
PanaHtotalFineTune(sensorSyncSigGenSizeCfg_t *pcfg)
{
	UINT32 cdspclk,tgpllclk,mclk;
	tgpllclk=sensorTgPllClkExtGet()/1000; /*kHz*/
	halCdspMasterClockGet(&cdspclk,&mclk);/*kHz*/
	/*SENSOR_PROF_LOG_PRINT(1,"htotal=%d++ mclk=%d tgpll=%d",pcfg->htotal,mclk,tgpllclk);*/
	pcfg->htotal=pcfg->htotal*mclk/tgpllclk; /*Pana MN34120 Htotal*/
	/*SENSOR_PROF_LOG_PRINT(1,"htotal=%d--",pcfg->htotal);*/
}

void
sensorTg_PANAMN34120_Install()
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
    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncInit()___enter");
    SENSOR_PROF_LOG_PRINT(1, "________Sensor driver build time <<%s>> \n", BUILD_TIME);
	SENSOR_PROF_LOG_ADD(0, "sensorDoFuncInit()___enter");

   /* TGGPIO configuration here */    
    #if	defined(HW_VSC560Z)
		    appI2CWrite(0x02, 0xFF, 0x70); 
    #endif

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	halFrontClkCfg(MCLKMODE,0); /*MCLK: internal clock*/
                                /*PCLK: external clock */

#if defined(HW_EVB_SPE7001) /*GPIO control */
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FML,SENSOR_RESET,SENSOR_RESET);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,SENSOR_PWRDN);
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_FML,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
#else
	tmrUsWait(1000);  /*40mS*/
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_ON);
	tmrUsWait(1000);  /*40mS*/
#endif

	/*Debug only */
	sensorTgPllClkExtSet(SENSOR_MCLK);/*54MHz*/
	
	/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	
	/*****TODO*****front reset after these???*/

   	cmdportid = sensorTgCfg.cmdportid_xtg;
  
 	sensorSerialBulkWrite(cmdportid,COUNT_OF(DEF_INIT_AFE_CODE), DEF_INIT_AFE_CODE[0],0);
        tmrUsWait(2000);

	/*TG PLL=MCLK out=24MHz*/
	sensorLVDSCfgSet(&sensorLVDS_Prev4laneCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

	tmrUsWait(10000);	/* delay 100 ms*/

	if((sensorTgPllClkExtGet()*11/3)>140000000){  /* 54Mx11(data rate/lane)  x4/12 = 198M */
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,240000); /* 240MHz */
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9*/
	}
	else{
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,150000); /* 150 MHz */
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,0); /*enable preview mode 9*/
	}
	
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/

	/*fine tune htotal*/
	PanaHtotalFineTune(&sensorSyncSigGenPreviewVGA120fpsCfg);
    PanaHtotalFineTune(&sensorSyncSigGenPreviewVGA60fpsCfg);
	PanaHtotalFineTune(&sensorSyncSigGenPreviewVGA30fpsCfg);
	PanaHtotalFineTune(&sensorSyncSigGenPreview720P60fpsCfg);
	PanaHtotalFineTune(&sensorSyncSigGenPreview720P30fpsCfg);
	PanaHtotalFineTune(&sensorSyncSigGenPreview1080PCfg);
	PanaHtotalFineTune(&sensorSyncSigGenCaptureFullCfg);
	
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
    sensorSyncSigGenInit(&DEF_SYNC_SIG_GEN_CFG);

	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

#if STREAMOF_ENABLE	
	sensorStsCfg(SENSOR_SYS_CFG_FLOW_USE_ABORT, STREAMOF_ENABLE);
#endif

#if (_SENSOR_DBG_)
    /*debug by TGGPIO2*/  
    WRITE8(0xb0009030, (READ8(0xb0009030)|(0x02)));
    DBG_GPIO(0);
#endif
    sensorResumeInstall(sensorResumeExecuteLoad);
	sensorSuspendInstall(sensorSuspendExecuteLoad);
	
    fCap2Pv = 0;
    sensor_bshut_vcount_offset_set(6000);/*unit:us*//*sub line: 200 line x 26.27us=5.25us*/
                                                    /*Rest time:5H*/
	fp_SnapEarlyDo=sensorPv2SnapModeSet;

	halFrontSerialCfgSet(FRONT_SIF1_IDLE_CFG,0);/*speed up 3-wire clk*/
	
	#if MODE_SYNC_EARLY
		sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);/*config precmd position (cmdport -MODE_SYNC_EARLY) */
	#endif
    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncInit()___done");
    SENSOR_PROF_LOG_ADD(0, "sensorDoFuncInit()___done");    
}

static void
sensorDoFuncStop()
{
}

static void
sensorSnapModeAsyncCb(void)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	profLogAdd(1,"AsyncSet");
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,g_hoffset,DONTCHANE,DONTCHANE,DONTCHANE);
	sensorSyncSigInit(p_size_pcg);
	sensorSyncSigGenInit(p_gen_pcg);
    sensorDsenExtSet();
}

static void
sensorSnapModeAsyncSet(const sensorSyncSigCfg_t *pspcg ,sensorSyncSigGenSizeCfg_t *pgpcg,UINT32 hoffset,UINT32 sync)
{
	g_snap_flow_count=0;/*clear snap flow entry count*/
    fCap2Pv = 0;

	p_size_pcg=pspcg;
	p_gen_pcg=pgpcg;
	g_hoffset=hoffset;
	if(sync)
		sensor_frame_cb_reg(SENSOR_IRQ_SVC_ORG_VD,sensorSnapModeAsyncCb,0);
	else
		sensorSnapModeAsyncCb();
	
}
static void
sensorDoPreviewBegin(UINT32 nreg,const UINT8 *pdata,const sensorSyncSigCfg_t *pspcg ,sensorSyncSigGenSizeCfg_t *pgpcg,UINT32 hoffset){
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/

    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
 
    if(fCap2Pv){
        sensor_event_wait(SENSOR_EVENT_VVALID_END);  
    }
	sensorLVDSCfgSet(&sensorLVDS_Prev4laneCfg);
    sensorSerialBulkWrite(cmdportid, nreg,pdata,0);
    if(is_aborted == 0){
        sensorSerialBulkWrite(cmdportid,(sizeof(afe_streamon_code)/sizeof(afe_streamon_code[0])), afe_streamon_code[0],0);
    }     
    sensorSnapModeAsyncSet(pspcg,pgpcg,hoffset,0);


}




static void
sensorDoFuncPreviewVGA30fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(afe_vga_code),afe_vga_code[0],&sensorSyncSigPreviewVGACfg,&sensorSyncSigGenPreviewVGA30fpsCfg,PREVIEW_VGA_HCROP_OFFSET);
	SENSOR_PROF_LOG_ADD(1, "mipi VGA 30fps cmd done");
}

static void
sensorDoFuncPreviewVGA60fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(afe_vga_code),afe_vga_code[0],&sensorSyncSigPreviewVGACfg,&sensorSyncSigGenPreviewVGA60fpsCfg,PREVIEW_VGA_HCROP_OFFSET);
	SENSOR_PROF_LOG_ADD(1, "mipi VGA 60fps cmd done");
}

static void
sensorDoFuncPreviewVGA120fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(afe_vga_code),afe_vga_code[0],&sensorSyncSigPreviewVGACfg,&sensorSyncSigGenPreviewVGA120fpsCfg,PREVIEW_VGA_HCROP_OFFSET);
	SENSOR_PROF_LOG_ADD(1, "mipi VGA 120fps cmd done");
}

static void
sensorDoFuncPreview720P30fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(afe_720P_code),afe_720P_code[0],&sensorSyncSigPreview720PCfg,&sensorSyncSigGenPreview720P30fpsCfg,PREVIEW_720P_HCROP_OFFSET);
	SENSOR_PROF_LOG_ADD(1, "mipi 720P 30fps cmd done");
}                                                                                                                                                                                                                                                                                                                                          

static void
sensorDoFuncPreview720P60fpsBegin()
{
	sensorDoPreviewBegin(COUNT_OF(afe_720P_code),afe_720P_code[0],&sensorSyncSigPreview720PCfg,&sensorSyncSigGenPreview720P60fpsCfg,PREVIEW_720P_HCROP_OFFSET);
	SENSOR_PROF_LOG_ADD(1, "mipi 720P 60fps cmd done");
}                                                                                                                                                                                                                                                                                                                                          

static void
sensorDoFuncPreview1080PBegin()
{
	sensorDoPreviewBegin(COUNT_OF(afe_1080P_code),afe_1080P_code[0],&sensorSyncSigPreview1080PCfg,&sensorSyncSigGenPreview1080PCfg,PREVIEW_1080P_HCROP_OFFSET);
	SENSOR_PROF_LOG_ADD(1, "mipi 1080P 30fps cmd done");
}
static void
sensorDoFuncPreviewFULLBegin()
{
	sensorDoPreviewBegin(COUNT_OF(afe_still_code),afe_still_code[0],&sensorSyncSigPreviewFULLCfg,&sensorSyncSigGenCaptureFullCfg,CAPTURE_HCROP_OFFSET);
	SENSOR_PROF_LOG_ADD(1, "mipi FULL Scan mode cmd  done");
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
sensorSnapExpNegVDCb()
{       
    exp_aborted = 1;	
}

static void sensorPv2SnapModeSet(void)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	g_snap_flow_count++; /*snap flow entry count*/
	TIME_LOG(0);    

	
	sensorSerialBulkWrite(cmdportid, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0],0);
	sensorSerialBulkWrite(cmdportid, sizeof(afe_streamon_code)/sizeof(afe_streamon_code[0]), afe_streamon_code[0],0);
	
	SENSOR_PROF_LOG_PRINT(1, "[%03d]sensorDoFuncSnapEarly0--preview 2 snap do",g_snap_flow_count);
    SENSOR_PROF_LOG_PRINT(0, "[%03d]sensorDoFuncSnapEarly0--preview 2 snap do",g_snap_flow_count);    
	sensorLVDSCfgSet(&sensorLVDS_Full4laneCfg);/*remove extension line*/
	
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,CAPTURE_HCROP_OFFSET,DONTCHANE,DONTCHANE,DONTCHANE); /*set Hoffset2 for hcrop*/																											  

	sensorSyncSigInit(&sensorSyncSigSnapEarly3msCfg);
	sensorSyncSigGenInit(&sensorSyncSigGenCaptureFullCfg); 

	/*sync set vd falling */
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
}

static void sensorSnap2SnapModeSet(void)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensor_event_wait(SENSOR_EVENT_VVALID_END); /*wait raw ready done */
	WRITE8(0xb0009073,0x07);
	g_snap_flow_count++;/*snap flow entry count*/
	TIME_LOG(0);
	SENSOR_PROF_LOG_PRINT(1,"[%03d]sensorDoFuncSnapEarly0--snap 2 snap do",g_snap_flow_count);
    SENSOR_PROF_LOG_PRINT(0,"[%03d]sensorDoFuncSnapEarly0--snap 2 snap do",g_snap_flow_count);    
	SENSOR_PROF_LOG_PRINT(1,"[%03d]front2cdsp gating = (%x)",g_snap_flow_count,READ8(0xb0009080));

	sensor_vd_restart();
	sensorSyncSigInit(&sensorSyncSigSnapEarly20msCfg);
	sensorSyncSigGenInit(&sensorSyncSigGenCaptureFullCfg); 

	if(!sensor_linecount_check(500))/*1ms check if no line count, retry again*/
		sensor_vd_restart();
}

static void
sensorDoFuncSnapEarly0()
{
	if(fp_SnapEarlyDo)
		fp_SnapEarlyDo();
    fCap2Pv = 1;
}

/*Abort current VD to restart a new VD*/

static void sensorSnapExpBeginVdCB(void)
{
	sensorSyncSigGenInit(&sensorSyncSigGenCaptureFullCfg);
    SENSOR_PROF_LOG_ADD(1, "sensorSnapExpBeginVdCB()");
    /*halFrontSnapTrig(1);*//*if bshutter end too late*/
    exp_aborted = 0;
    g_senSvcIdx = sensor_frame_repeat_cb_reg(SENSOR_IRQ_SVC_NEG_VD,sensorSnapExpNegVDCb,0);
	TIME_LOG(1);
    DBG_GPIO(1);
}

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
    SENSOR_PROF_LOG_PRINT(1,"[%03d] SnapExpBegin()",g_snap_flow_count);
	SENSOR_PROF_LOG_PRINT(1,"[%03d]front2cdsp gating = (%x)",g_snap_flow_count,READ8(0xb0009080));

    UINT32 cmdportid;
   	cmdportid = sensorTgCfg.cmdportid_xtg;
    
    if(sensorModeGet()==0x40 ){/*frame mode*/
		sensorSerialAsyncWrite(cmdportid,(sizeof(afe_FrameMode_Blank_code)/sizeof(afe_FrameMode_Blank_code[0])), afe_FrameMode_Blank_code[0]);
		sensorSerialFlush(cmdportid);		 
		sensorSyncSigSet(&sensorSyncSigSnapCfg); /*set real still readout size*/
		sensor_frame_cb_reg(SENSOR_IRQ_SVC_FRAME_END,sensorSnapExpBeginVdCB,0);/*Frame end to Abort current VD */
    }else{
        SENSOR_PROF_LOG_ADD(1, "SnapExpBegin()--Rolling Mode Done");
    }
    
}

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
    SENSOR_PROF_LOG_ADD(0, "sensorDoFuncSnapBshutterEnd()");
    SENSOR_PROF_LOG_PRINT(1,"[%03d] sensorDoFuncSnapBshutterEnd()",g_snap_flow_count);
	SENSOR_PROF_LOG_PRINT(1,"[%03d]front2cdsp gating = (%x)",g_snap_flow_count,READ8(0xb0009080));
    sensor_frame_cb_list_unreg(SENSOR_IRQ_SVC_NEG_VD, g_senSvcIdx);
    
	TIME_LOG(2);
	if(!exp_aborted){
	    if(1){ /*if(afe_snap_Exp[6]<2)*//* speed up >1VD performance */
		    /*short exposure immediately abort current frame*/
		    if((READ8(0xb0009040)&0x02)==0x02){
    			sensor_vd_restart_snap_trigger();
	        }
        }
    }
	else{
		HOST_ASSERT(0);/*If the VDi rising is coming*/
	}
    DBG_GPIO(0); 
}

static void
sensorDoFuncSnapShortExp()
{
}
static void
sensorDoFuncSnapExpEnd()
{
}
static void
sensorDoFuncSnapReadoutField0()
{
}
static void
sensorDoFuncSnapFieldEnd()
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
    
    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapFieldEnd()+++");    
	WRITE16(0xb0009064,1);
	SENSOR_PROF_LOG_PRINT(1,"[%03d]line count            = (%d)",g_snap_flow_count,READ16(0xb0009064));
	SENSOR_PROF_LOG_PRINT(1,"[%03d]front2cdsp gating     = (%x)",g_snap_flow_count,READ8(0xb0009080));
	SENSOR_PROF_LOG_PRINT(1,"[%03d]snap trig             = (%x)",g_snap_flow_count,READ8(0xb0009068)); 	
    SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapFieldEnd()---"); 
	sensorSerialAsyncWrite(cmdportid,(sizeof(afe_ShortExpcode)/sizeof(afe_ShortExpcode[0])), afe_ShortExpcode[0]);
}

/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{  
    #if SENSOR_PROF_LOG_EN
		TIME_LOG(3);                                                                                                                                                                                                              
        SENSOR_PROF_LOG_ADD(0, "sensorDoFuncSnapReadoutEnd()"); 
        SENSOR_PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutEnd()");                                                                                                                                                                   
	    SENSOR_PROF_LOG_PRINT(1, "[%03d]Early2Frmend:            %d.%d ms",g_snap_flow_count,TIME_DIFF(1,0)/1000,TIME_DIFF(1,0)%1000);                                                                                                                    
	    SENSOR_PROF_LOG_PRINT(1, "[%03d]Frmend2BshutterEnd:      %d.%d ms",g_snap_flow_count,TIME_DIFF(2,1)/1000,TIME_DIFF(2,1)%1000);                                                                                                                    
	    SENSOR_PROF_LOG_PRINT(1, "[%03d]BshutterEnd2Readoutend:  %d.%d ms",g_snap_flow_count,TIME_DIFF(3,2)/1000,TIME_DIFF(3,2)%1000);                                                                                                                    
	    SENSOR_PROF_LOG_PRINT(1, "[%03d]Early2Readoutend:        %d.%d ms %d.%d FPS ",g_snap_flow_count,TIME_DIFF(3,0)/1000,TIME_DIFF(3,0)%1000,1000000/TIME_DIFF(3,0),((1000000*10)/TIME_DIFF(3,0))%10);                                                 
	    if(TIME_TAG(4)){                                                                                                                                                                                                           
	    	SENSOR_PROF_LOG_PRINT(1,"[%03d][Burst]Readout2Readout:  %d.%d ms %d.%d FPS",g_snap_flow_count,TIME_DIFF(3,4)/1000,TIME_DIFF(3,4)%1000,1000000/TIME_DIFF(3,4),((1000000*10)/TIME_DIFF(3,4))%10);                                               
                SENSOR_PROF_LOG_PRINT(0,"[%03d][Burst]Readout2Readout:  %d.%d ms %d.%d FPS",g_snap_flow_count,TIME_DIFF(3,4)/1000,TIME_DIFF(3,4)%1000,1000000/TIME_DIFF(3,4),((1000000*10)/TIME_DIFF(3,4))%10);                                                       
            }
	    TIME_LOG(4);                                                                                                                                                                                                              
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
    #if SENSOR_PROF_LOG_EN                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
	    if(sensorModeGet()==0x40){ /*frame mode*/
            UINT8 i;
		    for(i=0;i<(nreg*4);i++){
		        afe_snap_Exp[i]=pdata[i]; /*snap exp write */
            }  
			WRITE16(0xb0009064,1);
	        SENSOR_PROF_LOG_PRINT(1,"[linecount= %d][nreg=%d][%x %x %x %x] [%x %x %x %x]",READ16(0xb0009064),nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7]);                                                                                                                                                                                                                                                                                                                
	    }
    #endif  
    
    sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
}
static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
	if(sensorModeGet()==0x40){/*frame mode*/
		SENSOR_PROF_LOG_PRINT(1,"[vtbl_type=%d] [vcount=%d] ",vtbl_type,vcount);																																																																												 
	}
}

/******TODO****/

#if SP5K_SENSOR_PANAMN34120
void sensorCmdPortExpLineSend(UINT16 expLine)
{
}
#endif

#if SUSPEND_ENABLE
static UINT32 sensorSuspended;
#endif

static UINT32 sensorResumeExecuteLoad()
{
	#if SUSPEND_ENABLE
		if(sensorSuspended ==1){                                                                                                                                                                                                                                 
	    	/* sensorResumeExecute() */                                                                                                                                                                                                                          
	    	SENSOR_PROF_LOG_ADD(1, "senResu s");/*Edward 20111126*/                                                                                                                                                                                              
            /* frome Save mode 3-2 to Normal mode*/
            /*(1)MCLK input*/
                sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN, 1,DONTCHANE,DONTCHANE,DONTCHANE);
                tmrUsWait(1000);
            /*(2)PSV L->H*/    
                SET_CMOS_PWN(1);
                tmrUsWait(1000);
	    	SENSOR_PROF_LOG_ADD(1, "senResu e");/*Edward 20111126*/                                                                                                                                                                                              
	    	sensorSuspended = 0;                                                                                                                                                                                                                                 
	    }                                                                                                                                                                                                                                                        
	#endif
	return SUCCESS;
}
static UINT32 sensorSuspendExecuteLoad()
{
	#if SUSPEND_ENABLE
        if((sensorModeGet()&0x30)!= 0x30) return;
    
		if(sensorSuspended ==0){                                                                                                                                                                                                                                                                                            
	    	/* sensorSuspendExecute() */                                                                                                                                                                                                                                                                                    
	    	SENSOR_PROF_LOG_ADD(1, "senSusp s");/*Edward 20111126*/                                                                                                                                                                                                                                                         
	    	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;                                                                                                                                                                                                                                                                   
	    	/*sensor suspend code body*/                                                                                                                                                                                                                                                                                    
	    	sensorSerialFlush(cmdportid);                                                                                                                                                                                                                                                                                   
            /*Save mode 3-2*/
                sensorSerialBulkWrite(cmdportid,(sizeof(afe_sensorpsav)/sizeof(afe_sensorpsav[0])), afe_sensorpsav[0],0);	                                                                             
 	            tmrUsWait(1000);/*5ms*/                                                                                                                                                                                                                                                                                         
	    	sensorSerialFlush(cmdportid);                                                                                                                                                                                                                                                                                   
                tmrUsWait(1000);   
            /*PSV-Pin: H-->L*/
                SET_CMOS_PWN(0);
                tmrUsWait(1000);
            /*MCLK, HD/VD turn off*/
                sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN, 0,DONTCHANE,DONTCHANE,DONTCHANE);
                halFrontSyncSigCfgSet(FRONT_SYNC_SIGGEN_EN,0,0); 
	    	SENSOR_PROF_LOG_ADD(1, "senSusp e");/*Edward 20111126*/                                                                                                                                                                                                                                                         
	    	sensorSuspended = 1;                                                                                                                                                                                                                                                                                            
	    }                                                                                                                                                                                                                                                                                                                   
	#endif
	return SUCCESS;
}

#if STREAMOF_ENABLE	
static void
sensorDoFuncStreamOff(UINT32 abort)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==0){
		SENSOR_PROF_LOG_ADD(1, "stream off s");/*Edward 20111129*/
		sensorSerialFlush(cmdportid);
		sensorSerialBulkWrite(cmdportid,(sizeof(afe_streamoff_code)/sizeof(afe_streamoff_code[0])), afe_streamoff_code[0],0);	
		/*tmrUsWait(1000);*//*5ms*/
		SENSOR_PROF_LOG_ADD(1, "stream off e");/*Edward 20111129*/
		is_aborted = 1;
	}
}
static void
sensorDoFuncStreamOn(UINT32 abort)
{
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==1 ){
	    SENSOR_PROF_LOG_ADD(1, "stream on s");/*Edward 20111129*/
		sensorSerialFlush(cmdportid);
		sensorSerialBulkWrite(cmdportid,(sizeof(afe_streamon_code)/sizeof(afe_streamon_code[0])), afe_streamon_code[0],0);
        #if 1
            sensor_vd_restart();
        #endif    
		/*tmrUsWait(1000);*//*5ms*/
		SENSOR_PROF_LOG_ADD(1, "stream on e");/*Edward 201111129*/
		is_aborted = 0;
	}
}
#endif

static void
sensorDoFuncSnapNext(UINT32 mode)
{
    #if BURST_CAP_EN
            if((mode&0xF0)==0x40) {
	            SENSOR_PROF_LOG_ADD(1,"@@@@ get sensor next mode is snap");
	            fp_SnapEarlyDo=sensorSnap2SnapModeSet;
	
            }else{
            	SENSOR_PROF_LOG_ADD(1,"@@@@ get sensor next mode is preview");
	            fp_SnapEarlyDo=sensorPv2SnapModeSet;
	            TIME_TAG(4)=0;
            }

    #endif
}



