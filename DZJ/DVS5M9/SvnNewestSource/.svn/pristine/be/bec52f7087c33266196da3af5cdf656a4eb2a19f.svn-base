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
 *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_ar0331-0.00.07"
#define BUILD_TIME        __DATE__  "-"  __TIME__

/*HISTORY
* 0.00.01 lester,
*   Initial version.
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

#define BURST_CAP_EN          	1
/*#define PROF_LOG_EN             1*/
#define HAS_STREAM_OFF          0
#define MODE_SYNC_EARLY        50
#define _SENSOR_DBG_            0

UINT32 smode;

#if 1
enum{   
    SENSOR_LINEAR = 0,
    SENSOR_HDR_4X, 
    SENSOR_HDR_8X, 
    SENSOR_HDR_16X, 
    SENSOR_HDR_32X, 
};
enum{   
    SENSOR_2D = 0,
    SENSOR_DLO,    
};
#endif

#if BURST_CAP_EN
	#define S2S_TIME_LOG 		0
    #define DDR_CLOCK_297MHz    1
#else
    #define S2S_TIME_LOG 		0
    #define DDR_CLOCK_297MHz    0
#endif

#define SP5K_SET_LVDS_HCROP(a)    sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,a,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF)


/*#if PROF_LOG_EN
    #define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
    #define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)
    static  UINT32 g_cap_count=0;
#else
    #define PROF_LOG_ADD(id,msg)
    #define PROF_LOG_PRINT(id,str,args...)
#endif*/

#if S2S_TIME_LOG
    static UINT32 g_time_log[10];
#endif

#if(_SENSOR_DBG_)
    /*debug by TGGPIO2*/ 
    #define DBG_GPIO(x) if(x){WRITE8(0xb0009032, (READ8(0xb0009032)|(0x02)));}else{WRITE8(0xb0009032, (READ8(0xb0009032)&(~0x02)));}
#else
    #define DBG_GPIO(x)
#endif

/* Sensor image size definition here */
#define NO              0                    
#define BY_PIXEL        1                   
#define BY_HD           0                   
#define NOUSE           0                   
#define DISABLE         0xFFFF              
#define CVR_RDK        (1)

#if CVR_RDK
#define SENSOR_FREX             (1<<4)        
#define SENSOR_PWRDN            (1<<10)       
#define SENSOR_RESET            (1<<0)        
/*#define SENSOR_TEST             (1<<3)        */
#define OUT_SENSOR_GPIO_MASK     SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET/*|SENSOR_TEST*/
#define CFG_SENSOR_GPIO_DIR      SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET/*|SENSOR_TEST*/
#define SENOR_GPIO_GRP           SP5K_GPIO_GRP_TGL
#endif

/*******************************************************/
#define PREVIEW_1080P_HSIZE  	  			 1920 								/* preview horiz. size*/
#define PREVIEW_1080P_LINETOTAL   		  (1920*2)							    /* preview Hd length 0x9088-htotal*/
#define PREVIEW_1080P_LINEBLANK   		  	    6								/* preview Hd sync length 0x908c hblank*/
#define PREVIEW_1080P_HOFFSET     			0x700								/* preview Hd valid pixel data offset position 0x90a6 hoffset*/
#define PREVIEW_1080P_HCROP_OFFSET	           (0)   	                        /* lvds crop offset (4N-1 BUG!) */              
#define PREVIEW_1080P_HRESHAPE_FALL         0x1E0								/* preview Hd reshap fall 0x90b2*/
#define PREVIEW_1080P_HRESHAPE_RISE         0x200   		                    /* preview Hd reshap rise 0x90b0*/

#define PREVIEW_1080P_VSIZE   				 1080 								/* preview vertical size 0x90a8 vsize */
#define PREVIEW_1080P_FRAMETOTAL  			 1081								/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_1080P_FRAMEBLANK  			 1081								/* preview Vd sync length 0x9090 vblank->vtotal*/

#define PREVIEW_1080P_EXTEND_LINES 			   15
#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL  BY_HD								/* preview Vd reshap control*/
#define PREVIEW_1080P_VRESHAPE_FALL         PREVIEW_1080P_EXTEND_LINES			/* preview Vd reshap fall 0x90b6*/
#define PREVIEW_1080P_VRESHAPE_RISE         PREVIEW_1080P_EXTEND_LINES+1		/* preview Vd reshap rise 0x90b4*/
#define PREVIEW_1080P_VOFFSET     			PREVIEW_1080P_EXTEND_LINES+3		/* preview Vd valid line data offset position 0x90aa voffset*/
/*******************************************************/

#define PREVIEW_1080_FPS30_HSIZE  	  		  1920 								/* preview horiz. size*/
#define PREVIEW_1080_FPS30_LINETOTAL   	   (1920*4) 							/* preview Hd length 0x9088-htotal*/
#define PREVIEW_1080_FPS30_LINEBLANK   		     6								/* preview Hd sync length 0x908c hblank*/
#define PREVIEW_1080_FPS30_HOFFSET     		0x1500							    /* preview Hd valid pixel data offset position 0x90a6 hoffset*/
#define PREVIEW_1080_FPS30_HCROP_OFFSET	        (0)   	                        /* lvds crop offset (4N-1 BUG!) */              
#define PREVIEW_1080_FPS30_HRESHAPE_FALL    0x1100							    /* preview Hd reshap fall 0x90b2*/
#define PREVIEW_1080_FPS30_HRESHAPE_RISE    0x1200								/* preview Hd reshap rise 0x90b0*/

#define PREVIEW_1080_FPS30_VSIZE   			  1080   							/* preview vertical size 0x90a8 vsize */
#define PREVIEW_1080_FPS30_FRAMETOTAL  		  1081   							/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_1080_FPS30_FRAMEBLANK  		  1081								/* preview Vd sync length 0x9090 vblank->vtotal*/

#define PREVIEW_1080_FPS30_EXTEND_LINES 	    15
#define PREVIEW_1080_FPS30_VRESHAPE_COUNTER_SEL  BY_HD							/* preview Vd reshap control*/
#define PREVIEW_1080_FPS30_VRESHAPE_FALL    PREVIEW_1080_FPS30_EXTEND_LINES 	/* preview Vd reshap fall 0x90b6*/
#define PREVIEW_1080_FPS30_VRESHAPE_RISE 	PREVIEW_1080_FPS30_EXTEND_LINES+1	/* preview Vd reshap rise 0x90b4*/
#define PREVIEW_1080_FPS30_VOFFSET     		PREVIEW_1080_FPS30_EXTEND_LINES+3 	/* preview Vd valid line data offset position 0x90aa voffset*/
/*******************************************************/

#define PREVIEW_VGA_FPS120_HSIZE			   640								/* preview horiz. size*/
#define PREVIEW_VGA_FPS120_LINETOTAL   		(640*6)								/* preview Hd length 0x9088-htotal*/
#define PREVIEW_VGA_FPS120_LINEBLANK   		     6								/* preview Hd sync length 0x908c hblank*/
#define PREVIEW_VGA_FPS120_HOFFSET     		 0xD00								/* preview Hd valid pixel data offset position 0x90a6 hoffset*/
#define PREVIEW_VGA_FPS120_HCROP_OFFSET	        (0)   	                        /* lvds crop offset (4N-1 BUG!) */              
#define PREVIEW_VGA_FPS120_HRESHAPE_FALL     0xB00 								/* preview Hd reshap fall  0x90b2*/
#define PREVIEW_VGA_FPS120_HRESHAPE_RISE     0xC00								/* preview Hd reshap rise 0x90b0*/

#define PREVIEW_VGA_FPS120_VSIZE			   480								/* preview vertical size 0x90a8 vsize*/
#define PREVIEW_VGA_FPS120_FRAMETOTAL  		   481						 		/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_VGA_FPS120_FRAMEBLANK  		   481								/* preview Vd sync length 0x9090 vblank->vtotal*/

#define PREVIEW_VGA_FPS120_EXTEND_LINES 	    15
#define PREVIEW_VGA_FPS120_VRESHAPE_COUNTER_SEL  BY_HD							/* preview Vd reshap control*/
#define PREVIEW_VGA_FPS120_VRESHAPE_FALL    PREVIEW_VGA_FPS120_EXTEND_LINES 	/* preview Vd reshap fall 0x90b6*/
#define PREVIEW_VGA_FPS120_VRESHAPE_RISE    PREVIEW_VGA_FPS120_EXTEND_LINES+1 	/* preview Vd reshap rise 0x90b4*/
#define PREVIEW_VGA_FPS120_VOFFSET     		PREVIEW_VGA_FPS120_EXTEND_LINES+3   /* preview Vd valid line data offset position 0x90aa voffset*/
/*******************************************************/

#define PREVIEW_QVGA_FPS240_HSIZE			    320								/* preview horiz. size*/                                                             
#define PREVIEW_QVGA_FPS240_LINETOTAL   	(320*14) 							/* preview Hd length 0x9088-htotal*/                                           
#define PREVIEW_QVGA_FPS240_LINEBLANK   	      6								/* preview Hd sync length 0x908c hblank*/                                        
#define PREVIEW_QVGA_FPS240_HOFFSET     	  0xF00								/* preview Hd valid pixel data offset position 0x90a6 hoffset*/                
#define PREVIEW_QVGA_FPS240_HCROP_OFFSET	     (0)   	                        /* lvds crop offset (4N-1 BUG!) */              
#define PREVIEW_QVGA_FPS240_HRESHAPE_FALL     0xD00								/* preview Hd reshap fall  0x90b2*/                                          
#define PREVIEW_QVGA_FPS240_HRESHAPE_RISE     0xE00								/* preview Hd reshap rise 0x90b0*/                                             
                                                                                                                   
#define PREVIEW_QVGA_FPS240_VSIZE			    240								/* preview vertical size 0x90a8 vsize*/                                              
#define PREVIEW_QVGA_FPS240_FRAMETOTAL  	    241						 		/* preview Vd length 0x908e vtotal no use*/                                           
#define PREVIEW_QVGA_FPS240_FRAMEBLANK  	    241								/* preview Vd sync length 0x9090 vblank->vtotal*/                                      
                                                                                                                  
#define PREVIEW_QVGA_FPS240_EXTEND_LINES 	     15                                                                                                   
#define PREVIEW_QVGA_FPS240_VRESHAPE_COUNTER_SEL  BY_HD							/* preview Vd reshap control*/                                             
#define PREVIEW_QVGA_FPS240_VRESHAPE_FALL   PREVIEW_QVGA_FPS240_EXTEND_LINES 	/* preview Vd reshap fall 0x90b6*/                             
#define PREVIEW_QVGA_FPS240_VRESHAPE_RISE   PREVIEW_QVGA_FPS240_EXTEND_LINES+1 	/* preview Vd reshap rise 0x90b4*/                           
#define PREVIEW_QVGA_FPS240_VOFFSET     	PREVIEW_QVGA_FPS240_EXTEND_LINES+3  /* preview Vd valid line data offset position 0x90aa voffset*/
/*******************************************************/
#define CAPTURE_PREVIEW_MODE				     0
#define CAPTURE_CONTEXT_TEST				     0

#define CAPTURE_HSIZE 						  2048								/* capture horiz. size*/ 
#define CAPTURE_LINETOTAL   			   (2048*5)								/* capture Hd length  0x9088-htotal*/
#define CAPTURE_LINEBLANK   				     6								/* capture Hd sync length  0x908c hblank*/
#define CAPTURE_HOFFSET     				0x2000   							/* capturee Hd valid pixel data offset position  0x90a6 hoffset*/
#define CAPTURE_HCROP_OFFSET	                (0)   	                        /* lvds crop offset (4N-1 BUG!) */              
#define CAPTURE_HRESHAPE_FALL          		0x1E00								/* capture Hd reshap fall  0x90b2*/
#define CAPTURE_HRESHAPE_RISE          		0x1F00								/* capture Hd reshap rise 0x90b0*/

#define CAPTURE_VSIZE 						  1536								/* capture vertical size  0x90a8 vsize*/
#define CAPTURE_FRAMETOTAL   				  1537								/* capture Vd length 0x908e vtotal no use*/
#define CAPTURE_FRAMEBLANK   				  1537								/* mipi use > vsize is more safe*/

#define CAPTURE_EXTEND_LINES 				    15
#define CAPTURE_VRESHAPE_COUNTER_SEL  		 BY_HD 								/* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL  		         CAPTURE_EXTEND_LINES				/* capture Vd reshap fall  0x90b6*/
#define CAPTURE_VRESHAPE_RISE		  		 CAPTURE_EXTEND_LINES+1				/* capture Vd reshap rise  0x90b4*/
#define CAPTURE_VOFFSET     				 CAPTURE_EXTEND_LINES+3				/* capture Vd valid line data offset position  0x90aa voffset*/
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
#define SENSORMODEL		TYPE_CMOS	   
#define	TGTYPE			TG_CMOS_SP5K_SLAVE 

#define PREVIEW_1080P_CFAPATTERN  		BAYER_GRBG
#define PREVIEW_1080_FPS30_CFAPATTERN 	BAYER_GRBG
#define PREVIEW_VGA_FPS120_CFAPATTERN 	BAYER_GRBG
#define PREVIEW_QVGA_FPS240_CFAPATTERN 	BAYER_GRBG
#define	SNAP_CFAPATTERN					BAYER_GRBG        /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	          0		/*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	          1	    /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	          2	    /*for typical CCD==n_dummy + n_readout + 1*/
#define	SNAP_EARLY_NUM		          0	    /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		  2	    /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	          0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		          0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN        0 	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	    /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	          0		/*sony FALSE;panasonic/ITG TRUE*/
#define	EXP_FRAME_NUM              	  1	    /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/

#define	PREVIEW_HDNUM_FRAME_START            110	/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_FRAME_END 		   2	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_CMDPORT	 		   PREVIEW_1080P_VOFFSET+PREVIEW_1080P_VSIZE-42 	/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_1080_FPS30_HDNUM_FRAME_END     2	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080_FPS30_HDNUM_CMDPORT	   PREVIEW_1080P_VOFFSET+PREVIEW_1080P_VSIZE-37 	/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_FPS120_HDNUM_FRAME_END     2	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_FPS120_HDNUM_CMDPORT	 320 	/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_FPS240_HDNUM_FRAME_END    2	/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_FPS240_HDNUM_CMDPORT	 160 	/*the command port interrupt line number of the preview new frame*/
#define	CAPTURE_HDNUM_FRAME_START  	          30	/*the front interrupt line number of the snap new frame */
#define	CAPTURE_HDNUM_FRAME_END	 			   2    /*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	 			 700	/*the command port interrupt line number of the snap new frame*/
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
#define	TG_SPI_I2C_MODE  0x20     	                                         /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	 0		                                             /*don't change*/
#define	TG_SPEED         FRONT_SSC_CLK_192K
 						/*FRONT_SSC_CLK_24K,  FRONT_SSC_CLK_48K,    FRONT_SSC_CLK_96K,  FRONT_SSC_CLK_192K,
						  FRONT_SSC_CLK_384K, FRONT_SSC_CLK_768K, FRONT_SSC_CLK_1500K,  FRONT_SSC_CLK_3M    */
#define	TG_NBIT_REGDATA  16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		/*ssc/spi register address numbers */
#define MI_REG(a,b)      { (a)>>8, (a)&0xFF, (b)>>8, (b)&0xFF, }

extern UINT32 sensor_event_wait(UINT32 event);

static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreview1080PBegin();
static void sensorDoFuncPreview1080Fps30Begin();
static void sensorDoFuncPreviewVgaFps120Begin();
static void sensorDoFuncPreviewQvgaFps240Begin();

static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();

/*static void sensorDoFuncSnapEarly0();*/
static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
/*static void sensorDoFuncSnapBshutterEnd();*/
static void sensorDoFuncSnap_dark_BshutterEnd();

static void sensorDoFuncSnapNext(UINT32 next_mode);
extern UINT32 tmrTimeClockGet();
#if HAS_STREAM_OFF
static void sensorDoFuncStreamOff(UINT32 abort);
static void sensorDoFuncStreamOn (UINT32 abort);
#endif
UINT32 sensorAEMeanGet();

/* Aptina AR0331 setting define */
static const UINT8 cmd_stream_on[][4] SENSOR_SIF1DMA_BUF = {
#if 0
	MI_REG(0x301A, 0x005C),
#else	
    MI_REG(0x301A, 0x005E),        /* restart */
#endif    
};

static const UINT8 cmd_stream_off[][4] SENSOR_SIF1DMA_BUF = {
	MI_REG(0x301A, 0x0058),
};

static const UINT8 cmd_ALTM_Enable[][4] SENSOR_SIF1DMA_BUF = {
    /*MI_REG(0x301A, 0x005E),*/    /* restart */ 
	MI_REG(0x2400, 0x0002),
};

static const UINT8 cmd_ALTM_Bypass[][4] SENSOR_SIF1DMA_BUF = {
	MI_REG(0x2400, 0x0003),
};

#if CAPTURE_CONTEXT_TEST
static const UINT8 cmd_context_a_on[][4] SENSOR_SIF1DMA_BUF = {
	MI_REG(0x30B0, 0x0000),
};
#endif

static const UINT8 cmd_sw_reset[][4] SENSOR_SIF1DMA_BUF = {
	/* [Reset] */
#if 0    
	MI_REG(0x301A,0x0001),    /* Reset This bit always reads as 0.  
	                             Setting this bit initiates a reset sequence:
	                             the frame being generated will be truncated  */
	MI_REG(0x301A,0x10D8),    /* bit7: parallel i/f enable */
#else
    MI_REG(0x301A,0x0059),
    MI_REG(0x301A,0x0058),    
#endif
};

static const UINT8 cmd_DLO_on[][4] SENSOR_SIF1DMA_BUF = {
    /* LOAD= 2D motion compensation OFF */
	MI_REG(0x3190, 0xE000),     /*FIELD_WR= HDR_MC_CTRL4, 0xE000 			// REG= 0x3190, 0xE000 */
};

static const UINT8 cmd_DLO_off[][4] SENSOR_SIF1DMA_BUF = {
	MI_REG(0x3190, 0x0000),     /*FIELD_WR= HDR_MC_CTRL4, 0x0000 			// REG= 0x3190, 0x0000 */
};

static const UINT8 cmd_2Dmotion_comp_on[][4] SENSOR_SIF1DMA_BUF = {
    MI_REG(0x318C, 0xC001),     /*FIELD_WR= HDR_MC_CTRL2, 0xC001            // REG= 0x318C, 0xC001 */
    MI_REG(0x3198, 0x061E),     /*FIELD_WR= HDR_MC_CTRL8, 0x061E            // REG= 0x3198, 0x061E */
#if 1 
/*cause purple bar when switch DLO and 2D
  tune command port to frame end
*/
    MI_REG(0x30FE, 0x0000),     /*FIELD_WR= NOISE_PEDESTAL, 0x0000          // REG= 0x30FE, 0x0000 */
#endif    
    MI_REG(0x320A, 0x0000),     /*FIELD_WR= ADACD_PEDESTAL, 0x0000          // REG= 0x320A, 0x0000 */
};

static const UINT8 cmd_2Dmotion_comp_off[][4] SENSOR_SIF1DMA_BUF = {
    MI_REG(0x318C, 0x0000),     /*FIELD_WR= HDR_MC_CTRL2, 0x0000 			// REG= 0x318C, 0x0000 */
    MI_REG(0x3198, 0x0000),     /*FIELD_WR= HDR_MC_CTRL4, 0x0000			// REG= 0x3190, 0x0000 */
#if 1  
    /*cause purple bar when switch DLO and 2D
      tune command port to frame end
    */
    MI_REG(0x30FE, 0x0080),     /*FIELD_WR= NOISE_PEDESTAL, 0x0080		    // REG= 0x30FE, 0x0080 */
#endif    
    MI_REG(0x320A, 0x0080),     /*FIELD_WR= ADACD_PEDESTAL, 0x0080		    // REG= 0x320A, 0x0080 */
};

static const UINT8 cmd_pll_dataformat[][4] SENSOR_SIF1DMA_BUF = {
    #if 0
        MI_REG(0x301A, 0x0058),	/*FIELD_WR= RESET_REGISTER,  0x0058	*/			  
    #endif        
	MI_REG(0x30B0, 0x0000), 	/*FIELD_WR= DIGITAL_TEST,    0x0000 */		
	#if 0
        /*mark it in here bcs different setting in Linear or HDR mode. */
	    MI_REG(0x30BA, 0x06EC), /*FIELD_WR= DIGITAL_CTRL,    0x06EC 	*/			  
	    MI_REG(0x31AC, 0x0C0C), /*FIELD_WR= DATA_FORMAT_BITS,0x0C0C 	*/
	#endif    
	/* [Hidden: PLL_settings - 4 Lane 12-bit HiSPi] */
	MI_REG(0x302A, 0x0006), 	/*FIELD_WR= VT_PIX_CLK_DIV,  0x0006		*/																			  
	MI_REG(0x302C, 0x0001), 	/*FIELD_WR= VT_SYS_CLK_DIV,  0x0001 	*/																		  
	MI_REG(0x302E, 0x000B), 	/*FIELD_WR= PRE_PLL_CLK_DIV, 0x0004		*/
	MI_REG(0x3030, 0x00CC), 	/*FIELD_WR= PLL_MULTIPLIER,  0x0042		*/																			  
	MI_REG(0x3036, 0x000C), 	/*FIELD_WR= OP_PIX_CLK_DIV,  0x000C		*/
	MI_REG(0x3038, 0x0001), 	/*FIELD_WR= OP_SYS_CLK_DIV,  0x0001		*/
};

static const UINT8 cmd_LinearMode[][4] SENSOR_SIF1DMA_BUF = {
    /* [Hidden: Linear Mode] */
        MI_REG(0x3082,0x0009),		/*BITFIELD= 0x3082, 0x0003, 0x0001 		*/
    
    /* [2D motion compensation OFF] */
        MI_REG(0x318C, 0x0000),     /*FIELD_WR= HDR_MC_CTRL2,   0x0000 		*/		
	    MI_REG(0x3190, 0x0000),     /*FIELD_WR= HDR_MC_CTRL4,   0x0000		*/		
	    MI_REG(0x30FE, 0x0080),     /*FIELD_WR= NOISE_PEDESTAL, 0x0080		*/		
	    MI_REG(0x320A, 0x0080),     /*FIELD_WR= ADACD_PEDESTAL, 0x0080		*/
    
    /* [ALTM Bypassed] */
        #if 0
	        MI_REG(0x301A, 0x0058),		/*BITFIELD= 0x301A, 0x0004, 0	   // Stop Streaming  */
        #endif	    
	    MI_REG(0x2400, 0x0003),		/*FIELD_WR= ALTM_CONTROL, 0x0003 		*/
	    MI_REG(0x2450, 0x0000),  	/*FIELD_WR= ALTM_OUT_PEDESTAL, 0x0000	*/
	    MI_REG(0x301E, 0x00A8),  	/*FIELD_WR= DATA_PEDESTAL, 0x00A8		*/
	    /* [ALTM Disabled Devware Color Setup] */
	        #if 0  
                ???????????????????
                ???????????????????
                ???????????????????  
            #endif   
        #if 0
	        MI_REG(0x301A, 0x005C), 	/*BITFIELD= 0x301A, 0x0004, 1	   // Enable Streaming */
        #endif
        
	/* [ADACD Disabled] */
	    MI_REG(0x3200, 0x0000), 	/*FIELD_WR= ADACD_CONTROL, 0x0000 		*/

	/* [Companding Disabled] */
	    MI_REG(0x31D0, 0x0000), 	/*FIELD_WR= COMPANDING, 0x0000	   // REG= 0x31D0, 0x0000  */
    
        MI_REG(0x31E0, 0x0200), 	/*FIELD_WR= PIX_DEF_ID,  0x0200    // REG= 0x31E0, 0x0200*/

        /*MI_REG(0x3060, 0x0006),*/ /*FIELD_WR= ANALOG_GAIN, 0x0006	   // REG= 0x3060, 0x0006*/

	/* [Enable Embedded Data and Stats]	*/																						 
	    MI_REG(0x3064, 0x1982), 	/*FIELD_WR=SMIA_TEST, EMBEDDED_STATS_EN, 0x0001	// REG= 0x3064, 0x1982*/
	                              	/*FIELD_WR=SMIA_TEST, EMBEDDED_DATA,     0x0001	// REG= 0x3064, 0x1982*/
};

static const UINT8 cmd_HDRMode_SetUp[][4] SENSOR_SIF1DMA_BUF = {
    #if 0 
    /*make it to singal command*/
    /* [2D motion compensation ON]	*/	
        /* [DLO disabled] */
            MI_REG(0x3190, 0x0000),  
            
        MI_REG(0x318C, 0xC001),     /*FIELD_WR= HDR_MC_CTRL2, 0xC001 			// REG= 0x318C, 0xC001 */
        MI_REG(0x3198, 0x061E),     /*FIELD_WR= HDR_MC_CTRL8, 0x061E 			// REG= 0x3198, 0x061E */
        MI_REG(0x30FE, 0x0000),     /*FIELD_WR= NOISE_PEDESTAL, 0x0000		    // REG= 0x30FE, 0x0000 */
        MI_REG(0x320A, 0x0000),     /*FIELD_WR= ADACD_PEDESTAL, 0x0000		    // REG= 0x320A, 0x0000 */
    #endif        
    /* [ALTM Enabled] */
    #if 0
        MI_REG(0x301A, 0x0058),	   /* BITFIELD= 0x301A, 0x0004, 0		   	        // Stop Streaming*/
    #endif        
        MI_REG(0x2410, 0x0010),    /* FIELD_WR= ALTM_POWER_GAIN, 0x0010             // REG= 0x2410, 0x0010 */
        MI_REG(0x2412, 0x0010),    /* FIELD_WR= ALTM_POWER_OFFSET, 0x0010		    // REG= 0x2412, 0x0010 */
        MI_REG(0x2442, 0x0080),    /* FIELD_WR= ALTM_CONTROL_KEY_K0, 0x0080		    // REG= 0x2442, 0x0080 */   
        MI_REG(0x2444, 0x0000),    /* FIELD_WR= ALTM_CONTROL_KEY_K01_LO, 0x0000	    // REG= 0x2444, 0x0000 */
        MI_REG(0x2446, 0x0004),    /* FIELD_WR= ALTM_CONTROL_KEY_K01_HI, 0x0004	    // REG= 0x2446, 0x0004 */
        MI_REG(0x2440, 0x0002),    /* FIELD_WR= ALTM_CONTROL_DAMPER, 0x0002		    // REG= 0x2440, 0x0002 */
        MI_REG(0x2450, 0x0000),    /* FIELD_WR= ALTM_OUT_PEDESTAL, 0x0000		    // REG= 0x2450, 0x0000 */
        MI_REG(0x2438, 0x0010),    /* FIELD_WR= ALTM_CONTROL_MIN_FACTOR, 0x0010	    // REG= 0x2438, 0x0010 */
        MI_REG(0x243A, 0x0020),    /* FIELD_WR= ALTM_CONTROL_MAX_FACTOR, 0x0020	    // REG= 0x243A, 0x0020 */
        MI_REG(0x243C, 0x0000),    /* FIELD_WR= ALTM_CONTROL_DARK_FLOOR, 0x0000	    // REG= 0x243C, 0x0000 */
        MI_REG(0x243E, 0x0200),    /* FIELD_WR= ALTM_CONTROL_BRIGHT_FLOOR, 0x0200	// REG= 0x243E, 0x0200 */
        MI_REG(0x31D0, 0x0000),    /* FIELD_WR= COMPANDING, 0x0000 			        // REG= 0x31D0, 0x0000 */
    #if 0      
        /* AR0331-REV2-20111201-ver3197.ini */
        MI_REG(0x301E, 0x0000),    /* FIELD_WR= DATA_PEDESTAL, 0x0000			    // REG= 0x301E, 0x0000 */
    #else
        /* AR0331-REV2-20120210-ver3514.ini */
        MI_REG(0x301E, 0x0010),    /* FIELD_WR= DATA_PEDESTAL, 0x0010			    // REG= 0x301E, 0x0000 */
    #endif
        
        /* [ALTM Enabled Devware Color Setup] */
            #if 0  
                ???????????????????
                ???????????????????
                ???????????????????  
            #endif  
    #if 0  
        /* ALTM enable in here will halt on when system reset 
           move it after Enable Streaming.       


               note: system boot on is well                            */
        MI_REG(0x2400, 0x0002),    /* FIELD_WR= ALTM_CONTROL, 0x0002              	// REG= 0x2400, 0x0002 */
    #endif
    #if 0
        MI_REG(0x301A, 0x005C),	   /* BITFIELD= 0x301A, 0x0004, 1		   	        // Enable Streaming*/
    #endif   

    /* [ADACD Enabled] */
        MI_REG(0x3202, 0x00CF),    /* REG= 0x3202, 0x00CF				// REG= 0x3202, 0x00CF */
        MI_REG(0x3206, 0x0A06),    /* REG= 0x3206, 0x0A06				// REG= 0x3206, 0x0A06 */
        MI_REG(0x3208, 0x1A12),    /* REG= 0x3208, 0x1A12				// REG= 0x3208, 0x1A12 */
        MI_REG(0x3200, 0x0002),    /* FIELD_WR= ADACD_CONTROL, 0x0002 	// REG= 0x3200, 0x0002 */

    MI_REG(0x31E0, 0x0200), 	   /*FIELD_WR= PIX_DEF_ID,  0x0200     // REG= 0x31E0, 0x0200*/
	/*MI_REG(0x3060, 0x0006),*/ 	   /*FIELD_WR= ANALOG_GAIN, 0x0006	   // REG= 0x3060, 0x0006*/
    MI_REG(0x318A, 0x0E10),        /*FIELD_WR= HDR_MC_CTRL1, 0x0E10	   // REG= 0x318A, 0x0E10*/

    /* [Enable Embedded Data and Stats]	*/																						 
	    MI_REG(0x3064, 0x1982),    /*FIELD_WR=SMIA_TEST, EMBEDDED_STATS_EN, 0x0001	// REG= 0x3064, 0x1982*/
	                               /*FIELD_WR=SMIA_TEST, EMBEDDED_DATA,     0x0001	// REG= 0x3064, 0x1982*/
    #if 0
    MI_REG(0x301A, 0x005E),        /* restart,  BITFIELD= 0x301A, 0x0002, 1 */
    #endif
};

static const UINT8 cmd_init[][4] SENSOR_SIF1DMA_BUF= {
    /* AR0331-REV2-20120210-ver3514.ini 
 	   [=== HiSPi Linear Mode - 4 Lane SLVS Streaming, 12 bits/pixel ===] */ 														 

    MI_REG(0x31AE, 0x0304),	  	/*FIELD_WR= SERIAL_FORMAT, 0x0304: 4 lanes HiSpi */	
    MI_REG(0x31C6, 0x0402),     /*hispi_control_status: MSB  */
    MI_REG(0x306E, 0x9010),	  	/*FIELD_WR= DATAPATH_SELECT, 0x9010		*/		  
 
 	/*[Analog Settings]*/																													 
	MI_REG(0x3180, 0x8089),		/*FIELD_WR= DELTA_DK_CONTROL, 0x8089 	// REG= 0x3180, 0x8089*/ 													 
	MI_REG(0x30F4, 0x4000),		/* REG= 0x30F4, 0x4000 */ 													 
	MI_REG(0x3ED4, 0x8F6C),		/* REG= 0x3ED4, 0x8F6C */ 													 
	MI_REG(0x3ED6, 0x6666),		/* REG= 0x3ED6, 0x6666 */ 	
	MI_REG(0x3ED8, 0x8C42),		/* REG= 0x3ED8, 0x8C42 */ 	
	MI_REG(0x3EDA, 0x8899),		/* REG= 0x3EDA, 0x8899 */ 													 
	MI_REG(0x3EE6, 0x00F0),		/* REG= 0x3EE6, 0x00F0 */ 													 																																																					 																										 
		
    #if 0
    /* AR0331-REV2-20111201-ver3197.ini */
        MI_REG(0x3ED2, 0x9F46),		/*FIELD_WR= DAC_LD_6_7, 0x9F46			*/	  
    #else
    /* AR0331-REV2-20120210-ver3514.ini */
        MI_REG(0x3ED2, 0x1F46),		/*FIELD_WR= DAC_LD_6_7, 0x1F46			*/	
    #endif    
    #if 0    
        MI_REG(0x301A, 0x005C),		/*FIELD_WR= RESET_REGISTER, 0x005C		*/	  
    #endif    
};


static const UINT8 cmd_LinearSeqLoad[][4] SENSOR_SIF1DMA_BUF = {
    /* [AR0331 Linear sequencer load - 1.0] */
    /*cost about 35ms*/
    MI_REG(0x3088, 0x8000),
	MI_REG(0x3086, 0x4A03),
	MI_REG(0x3086, 0x4316),
	MI_REG(0x3086, 0x0443),
	MI_REG(0x3086, 0x1645),
	MI_REG(0x3086, 0x4045),
	MI_REG(0x3086, 0x6017),
	MI_REG(0x3086, 0x5045),
	MI_REG(0x3086, 0x404B),
	MI_REG(0x3086, 0x1244),
	MI_REG(0x3086, 0x6134),
	MI_REG(0x3086, 0x4A31),
	MI_REG(0x3086, 0x4342),
	MI_REG(0x3086, 0x4560),
	MI_REG(0x3086, 0x2714),
	MI_REG(0x3086, 0x3DFF),
	MI_REG(0x3086, 0x3DFF),
	MI_REG(0x3086, 0x3DEA),
	MI_REG(0x3086, 0x2704),
	MI_REG(0x3086, 0x3D10),
	MI_REG(0x3086, 0x2705),
	MI_REG(0x3086, 0x3D10),
	MI_REG(0x3086, 0x2715),
	MI_REG(0x3086, 0x3527),
	MI_REG(0x3086, 0x053D),
	MI_REG(0x3086, 0x1045),
	MI_REG(0x3086, 0x4027),
	MI_REG(0x3086, 0x0427),
	MI_REG(0x3086, 0x143D),
	MI_REG(0x3086, 0xFF3D),
	MI_REG(0x3086, 0xFF3D),
	MI_REG(0x3086, 0xEA62),
	MI_REG(0x3086, 0x2728),
	MI_REG(0x3086, 0x3627),
	MI_REG(0x3086, 0x083D),
	MI_REG(0x3086, 0x6444),
	MI_REG(0x3086, 0x2C2C),
	MI_REG(0x3086, 0x2C2C),
	MI_REG(0x3086, 0x4B01),
	MI_REG(0x3086, 0x432D),
	MI_REG(0x3086, 0x4643),
	MI_REG(0x3086, 0x1647),
	MI_REG(0x3086, 0x435F),
	MI_REG(0x3086, 0x4F50),
	MI_REG(0x3086, 0x2604),
	MI_REG(0x3086, 0x2694),
	MI_REG(0x3086, 0x2027),
	MI_REG(0x3086, 0xFC53),
	MI_REG(0x3086, 0xD5C ),
	MI_REG(0x3086, 0xD57 ),
	MI_REG(0x3086, 0x5417),
	MI_REG(0x3086, 0x955 ),
	MI_REG(0x3086, 0x5649),
	MI_REG(0x3086, 0x5307),
	MI_REG(0x3086, 0x5303),
	MI_REG(0x3086, 0x4D28),
	MI_REG(0x3086, 0x6C4C),
	MI_REG(0x3086, 0x928 ),
	MI_REG(0x3086, 0x2C28),
	MI_REG(0x3086, 0x294E),
	MI_REG(0x3086, 0x5C09),
	MI_REG(0x3086, 0x4500),
	MI_REG(0x3086, 0x4580),
	MI_REG(0x3086, 0x26B6),
	MI_REG(0x3086, 0x27F8),
	MI_REG(0x3086, 0x1702),
	MI_REG(0x3086, 0x27FA),
	MI_REG(0x3086, 0x5C0B),
	MI_REG(0x3086, 0x1718),
	MI_REG(0x3086, 0x26B2),
	MI_REG(0x3086, 0x5C03),
	MI_REG(0x3086, 0x1744),
	MI_REG(0x3086, 0x27F2),
	MI_REG(0x3086, 0x1702),
	MI_REG(0x3086, 0x2809),
	MI_REG(0x3086, 0x1710),
	MI_REG(0x3086, 0x1628),
	MI_REG(0x3086, 0x84D ),
	MI_REG(0x3086, 0x1A26),
	MI_REG(0x3086, 0x9316),
	MI_REG(0x3086, 0x1627),
	MI_REG(0x3086, 0xFA45),
	MI_REG(0x3086, 0xA017),
	MI_REG(0x3086, 0x727 ),
	MI_REG(0x3086, 0xFB17),
	MI_REG(0x3086, 0x2945),
	MI_REG(0x3086, 0x8017),
	MI_REG(0x3086, 0x827 ),
	MI_REG(0x3086, 0xFA17),
	MI_REG(0x3086, 0x285D),
	MI_REG(0x3086, 0x170E),
	MI_REG(0x3086, 0x2691),
	MI_REG(0x3086, 0x5301),
	MI_REG(0x3086, 0x1740),
	MI_REG(0x3086, 0x5302),
	MI_REG(0x3086, 0x1710),
	MI_REG(0x3086, 0x2693),
	MI_REG(0x3086, 0x2692),
	MI_REG(0x3086, 0x484D),
	MI_REG(0x3086, 0x4E28),
	MI_REG(0x3086, 0x94C ),
	MI_REG(0x3086, 0xB17 ),
	MI_REG(0x3086, 0x5F27),
	MI_REG(0x3086, 0xF217),
	MI_REG(0x3086, 0x1428),
	MI_REG(0x3086, 0x816 ),
	MI_REG(0x3086, 0x4D1A),
	MI_REG(0x3086, 0x1616),
	MI_REG(0x3086, 0x27FA),
	MI_REG(0x3086, 0x2603),
	MI_REG(0x3086, 0x5C01),
	MI_REG(0x3086, 0x4540),
	MI_REG(0x3086, 0x2798),
	MI_REG(0x3086, 0x172A),
	MI_REG(0x3086, 0x4A0A),
	MI_REG(0x3086, 0x4316),
	MI_REG(0x3086, 0xB43 ),
	MI_REG(0x3086, 0x279C),
	MI_REG(0x3086, 0x4560),
	MI_REG(0x3086, 0x1707),
	MI_REG(0x3086, 0x279D),
	MI_REG(0x3086, 0x1725),
	MI_REG(0x3086, 0x4540),
	MI_REG(0x3086, 0x1708),
	MI_REG(0x3086, 0x2798),
	MI_REG(0x3086, 0x5D53),
	MI_REG(0x3086, 0xD26 ),
	MI_REG(0x3086, 0x455C),
	MI_REG(0x3086, 0x14B ),
	MI_REG(0x3086, 0x1244),
	MI_REG(0x3086, 0x5251),
	MI_REG(0x3086, 0x1702),
	MI_REG(0x3086, 0x6018),
	MI_REG(0x3086, 0x4A03),
	MI_REG(0x3086, 0x4316),
	MI_REG(0x3086, 0x443 ),
	MI_REG(0x3086, 0x1658),
	MI_REG(0x3086, 0x4316),
	MI_REG(0x3086, 0x5943),
	MI_REG(0x3086, 0x165A),
	MI_REG(0x3086, 0x4316),
	MI_REG(0x3086, 0x5B43),
	MI_REG(0x3086, 0x4540),
	MI_REG(0x3086, 0x279C),
	MI_REG(0x3086, 0x4560),
	MI_REG(0x3086, 0x1707),
	MI_REG(0x3086, 0x279D),
	MI_REG(0x3086, 0x1725),
	MI_REG(0x3086, 0x4540),
	MI_REG(0x3086, 0x1710),
	MI_REG(0x3086, 0x2798),
	MI_REG(0x3086, 0x1720),
	MI_REG(0x3086, 0x224B),
	MI_REG(0x3086, 0x1244),
	MI_REG(0x3086, 0x2C2C),
	MI_REG(0x3086, 0x2C2C),

    MI_REG(0x30BA, 0x06EC), 	/*FIELD_WR= DIGITAL_CTRL,    0x06EC 	*/			  
	MI_REG(0x31AC, 0x0C0C), 	/*FIELD_WR= DATA_FORMAT_BITS,0x0C0C 	
                                                DataIn:  12 bits
	                                            DataOut: 12 bits        */
};

static const UINT8 cmd_HDRSeqLoad[][4] SENSOR_SIF1DMA_BUF = {
    /* [AR0331 HDR sequencer load - 3.0] */
    /*cost about 52ms*/
    MI_REG(0x3088, 0x8000),
    MI_REG(0x3086, 0x4540),
    MI_REG(0x3086, 0x6134),
    MI_REG(0x3086, 0x4A31),
    MI_REG(0x3086, 0x4342),
    MI_REG(0x3086, 0x4560),
    MI_REG(0x3086, 0x2714),
    MI_REG(0x3086, 0x3DFF),
    MI_REG(0x3086, 0x3DFF),
    MI_REG(0x3086, 0x3DEA),
    MI_REG(0x3086, 0x2704),
    MI_REG(0x3086, 0x3D10),
    MI_REG(0x3086, 0x2705),
    MI_REG(0x3086, 0x3D10),
    MI_REG(0x3086, 0x2715),
    MI_REG(0x3086, 0x3527),
    MI_REG(0x3086, 0x53D ),
    MI_REG(0x3086, 0x1045),
    MI_REG(0x3086, 0x4027),
    MI_REG(0x3086, 0x427 ),
    MI_REG(0x3086, 0x143D),
    MI_REG(0x3086, 0xFF3D),
    MI_REG(0x3086, 0xFF3D),
    MI_REG(0x3086, 0xEA62),
    MI_REG(0x3086, 0x2728),
    MI_REG(0x3086, 0x3627),
    MI_REG(0x3086, 0x83D ),
    MI_REG(0x3086, 0x6444),
    MI_REG(0x3086, 0x2C2C),
    MI_REG(0x3086, 0x2C2C),
    MI_REG(0x3086, 0x4B00),
    MI_REG(0x3086, 0x432D),
    MI_REG(0x3086, 0x6343),
    MI_REG(0x3086, 0x1664),
    MI_REG(0x3086, 0x435F),
    MI_REG(0x3086, 0x4F50),
    MI_REG(0x3086, 0x2604),
    MI_REG(0x3086, 0x2694),
    MI_REG(0x3086, 0x27FC),
    MI_REG(0x3086, 0x530D),
    MI_REG(0x3086, 0x5C0D),
    MI_REG(0x3086, 0x5754),
    MI_REG(0x3086, 0x1709),
    MI_REG(0x3086, 0x5556),
    MI_REG(0x3086, 0x4953),
    MI_REG(0x3086, 0x753 ),
    MI_REG(0x3086, 0x34D ),
    MI_REG(0x3086, 0x286C),
    MI_REG(0x3086, 0x4C09),
    MI_REG(0x3086, 0x282C),
    MI_REG(0x3086, 0x2828),
    MI_REG(0x3086, 0x261C),
    MI_REG(0x3086, 0x4E5C),
    MI_REG(0x3086, 0x960 ),
    MI_REG(0x3086, 0x4500),
    MI_REG(0x3086, 0x4580),
    MI_REG(0x3086, 0x26BE),
    MI_REG(0x3086, 0x27F8),
    MI_REG(0x3086, 0x1702),
    MI_REG(0x3086, 0x27FA),
    MI_REG(0x3086, 0x5C0B),
    MI_REG(0x3086, 0x1712),
    MI_REG(0x3086, 0x26BA),
    MI_REG(0x3086, 0x5C03),
    MI_REG(0x3086, 0x1713),
    MI_REG(0x3086, 0x27F2),
    MI_REG(0x3086, 0x171C),
    MI_REG(0x3086, 0x5F28),
    MI_REG(0x3086, 0x817 ),
    MI_REG(0x3086, 0x360 ),
    MI_REG(0x3086, 0x173C),
    MI_REG(0x3086, 0x26B2),
    MI_REG(0x3086, 0x1616),
    MI_REG(0x3086, 0x5F4D),
    MI_REG(0x3086, 0x1926),
    MI_REG(0x3086, 0x9316),
    MI_REG(0x3086, 0x1627),
    MI_REG(0x3086, 0xFA45),
    MI_REG(0x3086, 0xA017),
    MI_REG(0x3086, 0x527 ),
    MI_REG(0x3086, 0xFB17),
    MI_REG(0x3086, 0x1F45),
    MI_REG(0x3086, 0x801F),
    MI_REG(0x3086, 0x1705),
    MI_REG(0x3086, 0x27FA),
    MI_REG(0x3086, 0x171E),
    MI_REG(0x3086, 0x5D17),
    MI_REG(0x3086, 0xC26 ),
    MI_REG(0x3086, 0x9248),
    MI_REG(0x3086, 0x4D4E),
    MI_REG(0x3086, 0x269A),
    MI_REG(0x3086, 0x2808),
    MI_REG(0x3086, 0x4C0B),
    MI_REG(0x3086, 0x6017),
    MI_REG(0x3086, 0x327 ),
    MI_REG(0x3086, 0xF217),
    MI_REG(0x3086, 0x2626),
    MI_REG(0x3086, 0x9216),
    MI_REG(0x3086, 0x165F),
    MI_REG(0x3086, 0x4D19),
    MI_REG(0x3086, 0x2693),
    MI_REG(0x3086, 0x1616),
    MI_REG(0x3086, 0x27FA),
    MI_REG(0x3086, 0x2643),
    MI_REG(0x3086, 0x5C01),
    MI_REG(0x3086, 0x4540),
    MI_REG(0x3086, 0x2798),
    MI_REG(0x3086, 0x1720),
    MI_REG(0x3086, 0x4A65),
    MI_REG(0x3086, 0x4316),
    MI_REG(0x3086, 0x6643),
    MI_REG(0x3086, 0x5A43),
    MI_REG(0x3086, 0x165B),
    MI_REG(0x3086, 0x4327),
    MI_REG(0x3086, 0x9C45),
    MI_REG(0x3086, 0x6017),
    MI_REG(0x3086, 0x627 ),
    MI_REG(0x3086, 0x9D17),
    MI_REG(0x3086, 0x1C45),
    MI_REG(0x3086, 0x4023),
    MI_REG(0x3086, 0x1705),
    MI_REG(0x3086, 0x2798),
    MI_REG(0x3086, 0x5D26),
    MI_REG(0x3086, 0x4417),
    MI_REG(0x3086, 0xE28 ),
    MI_REG(0x3086, 0x53  ),
    MI_REG(0x3086, 0x14B ),
    MI_REG(0x3086, 0x5251),
    MI_REG(0x3086, 0x1244),
    MI_REG(0x3086, 0x4B01),
    MI_REG(0x3086, 0x432D),
    MI_REG(0x3086, 0x4643),
    MI_REG(0x3086, 0x1647),
    MI_REG(0x3086, 0x434F),
    MI_REG(0x3086, 0x5026),
    MI_REG(0x3086, 0x426 ),
    MI_REG(0x3086, 0x8427),
    MI_REG(0x3086, 0xFC53),
    MI_REG(0x3086, 0xD5C ),
    MI_REG(0x3086, 0xD57 ),
    MI_REG(0x3086, 0x5417),
    MI_REG(0x3086, 0x955 ),
    MI_REG(0x3086, 0x5649),
    MI_REG(0x3086, 0x5307),
    MI_REG(0x3086, 0x5303),
    MI_REG(0x3086, 0x4D28),
    MI_REG(0x3086, 0x6C4C),
    MI_REG(0x3086, 0x928 ),
    MI_REG(0x3086, 0x2C28),
    MI_REG(0x3086, 0x2826),
    MI_REG(0x3086, 0xC4E ),
    MI_REG(0x3086, 0x5C09),
    MI_REG(0x3086, 0x6045),
    MI_REG(0x3086, 0x45  ),
    MI_REG(0x3086, 0x8026),
    MI_REG(0x3086, 0xAE27),
    MI_REG(0x3086, 0xF817),
    MI_REG(0x3086, 0x227 ),
    MI_REG(0x3086, 0xFA5C),
    MI_REG(0x3086, 0xB17 ),
    MI_REG(0x3086, 0x1226),
    MI_REG(0x3086, 0xAA5C),
    MI_REG(0x3086, 0x317 ),
    MI_REG(0x3086, 0xB27 ),
    MI_REG(0x3086, 0xF217),
    MI_REG(0x3086, 0x265F),
    MI_REG(0x3086, 0x2808),
    MI_REG(0x3086, 0x1703),
    MI_REG(0x3086, 0x6017),
    MI_REG(0x3086, 0x326 ),
    MI_REG(0x3086, 0xA216),
    MI_REG(0x3086, 0x165F),
    MI_REG(0x3086, 0x4D1A),
    MI_REG(0x3086, 0x2683),
    MI_REG(0x3086, 0x1616),
    MI_REG(0x3086, 0x27FA),
    MI_REG(0x3086, 0x45A0),
    MI_REG(0x3086, 0x1705),
    MI_REG(0x3086, 0x27FB),
    MI_REG(0x3086, 0x171F),
    MI_REG(0x3086, 0x4580),
    MI_REG(0x3086, 0x2017),
    MI_REG(0x3086, 0x527 ),
    MI_REG(0x3086, 0xFA17),
    MI_REG(0x3086, 0x1E5D),
    MI_REG(0x3086, 0x170C),
    MI_REG(0x3086, 0x2682),
    MI_REG(0x3086, 0x484D),
    MI_REG(0x3086, 0x4E26),
    MI_REG(0x3086, 0x8A28),
    MI_REG(0x3086, 0x84C ),
    MI_REG(0x3086, 0xB60 ),
    MI_REG(0x3086, 0x1707),
    MI_REG(0x3086, 0x27F2),
    MI_REG(0x3086, 0x1738),
    MI_REG(0x3086, 0x2682),
    MI_REG(0x3086, 0x1616),
    MI_REG(0x3086, 0x5F4D),
    MI_REG(0x3086, 0x1A26),
    MI_REG(0x3086, 0x8316),
    MI_REG(0x3086, 0x1627),
    MI_REG(0x3086, 0xFA26),
    MI_REG(0x3086, 0x435C),
    MI_REG(0x3086, 0x145 ),
    MI_REG(0x3086, 0x4027),
    MI_REG(0x3086, 0x9817),
    MI_REG(0x3086, 0x1F4A),
    MI_REG(0x3086, 0x1244),
    MI_REG(0x3086, 0x343 ),
    MI_REG(0x3086, 0x1604),
    MI_REG(0x3086, 0x4316),
    MI_REG(0x3086, 0x5843),
    MI_REG(0x3086, 0x1659),
    MI_REG(0x3086, 0x4316),
    MI_REG(0x3086, 0x279C),
    MI_REG(0x3086, 0x4560),
    MI_REG(0x3086, 0x1705),
    MI_REG(0x3086, 0x279D),
    MI_REG(0x3086, 0x171D),
    MI_REG(0x3086, 0x4540),
    MI_REG(0x3086, 0x2217),
    MI_REG(0x3086, 0x527 ),
    MI_REG(0x3086, 0x985D),
    MI_REG(0x3086, 0x2645),
    MI_REG(0x3086, 0x170E),
    MI_REG(0x3086, 0x2800),
    MI_REG(0x3086, 0x5301),
    MI_REG(0x3086, 0x4B52),
    MI_REG(0x3086, 0x5112),
    MI_REG(0x3086, 0x4460),
    MI_REG(0x3086, 0x2C2C),
    MI_REG(0x3086, 0x2C2C),

    MI_REG(0x30BA, 0x07EC), 	/*FIELD_WR= DIGITAL_CTRL,    0x07EC 	*/		
	MI_REG(0x31AC, 0x100C), 	/*FIELD_WR= DATA_FORMAT_BITS,0x100C 	
	                                            DataIn:  16 bits
	                                            DataOut: 12 bits        */
};

static const UINT8 cmd_1080_fps60_init[][4] SENSOR_SIF1DMA_BUF = {
	/* 1080p size FPS60*/
   	MI_REG(0x3002, 0x00E8),	  	/*FIELD_WR= Y_ADDR_START		*/			  
	MI_REG(0x3004, 0x0046),	  	/*FIELD_WR= X_ADDR_START		*/			  
	MI_REG(0x3006, 0x051F),	  	/*FIELD_WR= Y_ADDR_END			*/	  
	MI_REG(0x3008, 0x07c5),	  	/*FIELD_WR= X_ADDR_END			*/	
	MI_REG(0x300A, 0x045D),	  	/*FIELD_WR= FRAME_LENGTH_LINES	*/	  	
	MI_REG(0x300C, 0x0453),	  	/*FIELD_WR= LINE_LENGTH_PCK		*/	  
	/*MI_REG(0x3012, 0x030D),	  FIELD_WR= COARSE_INTEGRATION_TIME*/	
	MI_REG(0x30A2, 0x0001),	  	/*FIELD_WR= X_ODD_INC			*/	  
	MI_REG(0x30A6, 0x0001),	  	/*FIELD_WR= Y_ODD_INC 			*/	  
	MI_REG(0x3040, 0x0000),	  	/*FIELD_WR= READ_MODE 			*/	
};

static const UINT8 cmd_1080_fps30_init[][4] SENSOR_SIF1DMA_BUF = {
	/* 1080p size FPS30*/
   	MI_REG(0x3002, 0x00E8),	  	/*FIELD_WR= Y_ADDR_START		*/			  
	MI_REG(0x3004, 0x0046),	  	/*FIELD_WR= X_ADDR_START		*/			  
	MI_REG(0x3006, 0x051F),	  	/*FIELD_WR= Y_ADDR_END			*/	  
	MI_REG(0x3008, 0x07c5),	  	/*FIELD_WR= X_ADDR_END			*/	
	MI_REG(0x300A, 0x045D),	  	/*FIELD_WR= FRAME_LENGTH_LINES	*/	
	MI_REG(0x300C, 0x08a5),	  	/*FIELD_WR= LINE_LENGTH_PCK		*/	  
	/*MI_REG(0x3012, 0x0187),	  	FIELD_WR= COARSE_INTEGRATION_TIME*/	
	MI_REG(0x30A2, 0x0001),	  	/*FIELD_WR= X_ODD_INC			*/	  
	MI_REG(0x30A6, 0x0001),	  	/*FIELD_WR= Y_ODD_INC 			*/	  
	MI_REG(0x3040, 0x0000),	  	/*FIELD_WR= READ_MODE 			*/	
};

static const UINT8 cmd_vga_fps120_init[][4] SENSOR_SIF1DMA_BUF = {
	/* vga size FPS120*/
	MI_REG(0x3002, 0x0034), 	/*FIELD_WR= Y_ADDR_START		*/			  
	MI_REG(0x3004, 0x0046), 	/*FIELD_WR= X_ADDR_START		*/			  
	MI_REG(0x3006, 0x05d3), 	/*FIELD_WR= Y_ADDR_END			*/	  
	MI_REG(0x3008, 0x07c5), 	/*FIELD_WR= X_ADDR_END			*/	
	MI_REG(0x300A, 0x0205), 	/*FIELD_WR= FRAME_LENGTH_LINES	*/	  
	MI_REG(0x300C, 0x04ab), 	/*FIELD_WR= LINE_LENGTH_PCK		*/	  
	/*MI_REG(0x3012, 0x0067), 	FIELD_WR= COARSE_INTEGRATION_TIME*/
	MI_REG(0x30A2, 0x0005), 	/*FIELD_WR= X_ODD_INC			*/
	MI_REG(0x30A6, 0x0005), 	/*FIELD_WR= Y_ODD_INC			*/
	MI_REG(0x3040, 0x3000), 	/*FIELD_WR= READ_MODE, 0x3000 	*/
};

static const UINT8 cmd_qvga_fps240_init[][4] SENSOR_SIF1DMA_BUF = {
	/* qvga size FPS240*/
	MI_REG(0x3002, 0x019c), 	/*FIELD_WR= Y_ADDR_START		*/			  
	MI_REG(0x3004, 0x0226), 	/*FIELD_WR= X_ADDR_START		*/			  
	MI_REG(0x3006, 0x046b), 	/*FIELD_WR= Y_ADDR_END			*/	  
	MI_REG(0x3008, 0x05e5), 	/*FIELD_WR= X_ADDR_END			*/	
	MI_REG(0x300A, 0x0115), 	/*FIELD_WR= FRAME_LENGTH_LINES	*/	  
	MI_REG(0x300C, 0x045b), 	/*FIELD_WR= LINE_LENGTH_PCK		*/	  
	/*MI_REG(0x3012, 0x006e), 	FIELD_WR= COARSE_INTEGRATION_TIME*/
	MI_REG(0x30A2, 0x0005), 	/*FIELD_WR= X_ODD_INC			*/
	MI_REG(0x30A6, 0x0005), 	/*FIELD_WR= Y_ODD_INC			*/
	MI_REG(0x3040, 0x3000), 	/*FIELD_WR= READ_MODE, 0x3000 	*/
};

static const UINT8 cmd_still_init[][4] SENSOR_SIF1DMA_BUF = {
#if CAPTURE_CONTEXT_TEST
	MI_REG(0x3084, 0x0009),
	MI_REG(0x308C, 0x0004), 	/*FIELD_WR= Y_ADDR_START		 */
	MI_REG(0x308A, 0x0006), 	/*FIELD_WR= X_ADDR_START		 */
	MI_REG(0x3090, 0x0603), 	/*FIELD_WR= Y_ADDR_END			 */
	MI_REG(0x308E, 0x0805), 	/*FIELD_WR= X_ADDR_END			 */
	MI_REG(0x30AA, 0x0625), 	/*FIELD_WR= FRAME_LENGTH_LINES	 */
	MI_REG(0x303E, 0x0C4A), 	/*FIELD_WR= LINE_LENGTH_PCK 	 */
	/*MI_REG(0x3012, 0x1388), 	  FIELD_WR= COARSE_INTEGRATION_TIME*/
	MI_REG(0x30Ae, 0x0001), 	/*FIELD_WR= X_ODD_INC			 */
	MI_REG(0x30A8, 0x0001), 	/*FIELD_WR= Y_ODD_INC	   	  	 */
	MI_REG(0x3040, 0x0000), 	/*FIELD_WR= READ_MODE			 */
	MI_REG(0x30B0, 0x2000),
#else
	MI_REG(0x3002, 0x0004), 	/*FIELD_WR= Y_ADDR_START		 */
	MI_REG(0x3004, 0x0006), 	/*FIELD_WR= X_ADDR_START		 */
	MI_REG(0x3006, 0x0603), 	/*FIELD_WR= Y_ADDR_END			 */
	MI_REG(0x3008, 0x0805), 	/*FIELD_WR= X_ADDR_END			 */
	MI_REG(0x300A, 0x0625), 	/*FIELD_WR= FRAME_LENGTH_LINES	 */
	MI_REG(0x300C, 0x0C4A), 	/*FIELD_WR= LINE_LENGTH_PCK		 */
	/*MI_REG(0x3012, 0x0624), 	FIELD_WR= COARSE_INTEGRATION_TIME*/
	MI_REG(0x30A2, 0x0001), 	/*FIELD_WR= X_ODD_INC			 */
	MI_REG(0x30A6, 0x0001), 	/*FIELD_WR= Y_ODD_INC	   	  	 */
	MI_REG(0x3040, 0x0000), 	/*FIELD_WR= READ_MODE			 */
	MI_REG(0x30B0, 0x0000),
#endif
};

/*sensor HiSPi cfg*/
static const sensorLVDSCfg_t sensorHiSpiCfg =
{
	.wordcntsel 	 =1 ,	    /*Hispi hsize from 0x90a4*/                                                      
	.Lvdsmode		 =1 ,	    /*12bit mode*/                                                                   
	.Lvdslanenum	 =0 ,	    /*4 lane mode*/                                                                  
	.Lvdslanemode	 =0 ,	    /*enable lane3 in 4 lanes mode*/                                                 
	.Lvdslaneswap	 =0 ,	    /*MSB first */                                                                   
	.Lvdsraw		 =1 ,	    /*raw 12 */                                                                      
	.Lvdseofsel 	 =1 ,	    /*count eol number equal as the register Vblank(Reg.0x9090) */                                                        
	.Lvdseolsel 	 =0 ,	    /*count pixel number equal as the register horizontal size(Reg.0x90A4) */                                                        
	.lvdsvdsrc		 =1 ,	    /*1: decode emedded data header sof as vd source*/                               
	.lvds_dphy_sel	 =1 ,	    /*MIPI D-PHY*/                                                                   
	.Ctrlword_sol	 =0x800,                                                                                     
	.Ctrlword_sof	 =0xc80,    /* Hd  mode = 0xc00 full resoultion mode =0xc80*/ /* full mode  0xc00*/          
	.Ctrlword_eol	 =0xa00,
	.Ctrlword_eof	 =0xe00,
	.syncwrod0		 =0 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	/*HW signal spec */
	.vmode			 =0 ,
	.S2pphase		 =0 ,
	.tsettle		 =1 ,
	.teot			 =7 ,
	/*Extention Part */
	.Exthd_num		 =PREVIEW_1080P_EXTEND_LINES,
	.Exthd_pol		 =0 ,	
	.Exthd_mode 	 =0
};

/*sensor cfg*/
static const sensorTgPreviewCfg_t sensorTgPreview1080PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080PBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_1080P_HSIZE,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio = 3200,
	.vratio = 1800,
#endif		
    .fps_mhz = 60460000,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080Fps30Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080Fps30Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_1080_FPS30_HSIZE,
	.vsize = PREVIEW_1080_FPS30_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_1080_FPS30_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio = 3200,
	.vratio = 1800,
#endif		
    .fps_mhz = 30230000,
};

static const sensorTgPreviewCfg_t sensorTgPreviewVgaFps120Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVgaFps120Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_VGA_FPS120_HSIZE,
	.vsize = PREVIEW_VGA_FPS120_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_VGA_FPS120_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio=4000,
	.vratio=3000,
#endif		
    .fps_mhz = 120171000,
};

static const sensorTgPreviewCfg_t sensorTgPreviewQvgaFps240Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewQvgaFps240Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_QVGA_FPS240_HSIZE,
	.vsize = PREVIEW_QVGA_FPS240_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_QVGA_FPS240_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
    .hratio=4000,
    .vratio=3000,
#endif		
    .fps_mhz = 240020000,
};

static const sensorTgPreviewCfg_t sensorTgPreviewSnapCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncSnapExpBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = CAPTURE_HSIZE,
	.vsize = CAPTURE_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = SNAP_CFAPATTERN,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
#if	CAPTURE_PREVIEW_MODE
	&sensorTgPreview1080PCfg,        
	&sensorTgPreview1080Fps30Cfg, 
	&sensorTgPreviewVgaFps120Cfg,
	&sensorTgPreviewQvgaFps240Cfg,
	&sensorTgPreviewSnapCfg
#else
                                     /* Exposure table      */
	&sensorTgPreview1080PCfg,        /* HDR mode-TV:6~13    */
	&sensorTgPreview1080Fps30Cfg,    /* HDR mode-TV:5~12    */
    #if 0 	
	&sensorTgPreviewVgaFps120Cfg,    /* Linear mode-TV:2~13 */
	&sensorTgPreviewQvgaFps240Cfg,   /* Linear mode-TV:2~13 */
	#endif
#endif
};

static sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			#if 0
			.psnapEarly = (sensorDoFunc_t []){
				sensorDoFuncSnapEarly0,
			},
			#else
			.psnapEarly = NULL,
			#endif
			
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
		.n_capvd = SNAP_CAPTUREVD_NUM,	/*??*/
		.n_snap_early = SNAP_EARLY_NUM,
		.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
		.fldsel_start = FLDSEL_START_NUM,
		.fldsel_end = FLDSEL_END_NUM,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
                                 /* Exposure table      */   
	 &sensorTgSnapRollCfg,       /* HDR mode-TV:4~12    */
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
	.hsize           = PREVIEW_1080P_HSIZE,
	.vsize           = PREVIEW_1080P_VSIZE,
	.lastlinepix     = PREVIEW_1080P_LINETOTAL,
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

static const sensorSyncSigCfg_t sensorSyncSigPreview1080Fps30Cfg = {
	.linetotal       = PREVIEW_1080_FPS30_LINETOTAL,
	.lineblank       = PREVIEW_1080_FPS30_LINEBLANK,
	.frametotal      = PREVIEW_1080_FPS30_FRAMETOTAL,
	.frameblank      = PREVIEW_1080_FPS30_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_1080_FPS30_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_1080_FPS30_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_1080_FPS30_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_1080_FPS30_VRESHAPE_RISE,
	.hoffset         = PREVIEW_1080_FPS30_HOFFSET,
	.voffset         = PREVIEW_1080_FPS30_VOFFSET,
	.hsize           = PREVIEW_1080_FPS30_HSIZE,
	.vsize           = PREVIEW_1080_FPS30_VSIZE,
	.lastlinepix     = PREVIEW_1080_FPS30_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_1080_FPS30_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_1080_FPS30_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_1080_FPS30_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVgaFps120Cfg = {
	.linetotal       = PREVIEW_VGA_FPS120_LINETOTAL,
	.lineblank       = PREVIEW_VGA_FPS120_LINEBLANK,
	.frametotal      = PREVIEW_VGA_FPS120_FRAMETOTAL,
	.frameblank      = PREVIEW_VGA_FPS120_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_VGA_FPS120_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_VGA_FPS120_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_VGA_FPS120_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_VGA_FPS120_VRESHAPE_RISE,
	.hoffset         = PREVIEW_VGA_FPS120_HOFFSET,
	.voffset         = PREVIEW_VGA_FPS120_VOFFSET,
	.hsize           = PREVIEW_VGA_FPS120_HSIZE,
	.vsize           = PREVIEW_VGA_FPS120_VSIZE,
	.lastlinepix     = PREVIEW_VGA_FPS120_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_FPS120_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_VGA_FPS120_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_VGA_FPS120_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQvgaFps240Cfg = {
	.linetotal       = PREVIEW_QVGA_FPS240_LINETOTAL,       
	.lineblank       = PREVIEW_QVGA_FPS240_LINEBLANK,       
	.frametotal      = PREVIEW_QVGA_FPS240_FRAMETOTAL,      
	.frameblank      = PREVIEW_QVGA_FPS240_FRAMEBLANK,      
	.hreshape_fall   = PREVIEW_QVGA_FPS240_HRESHAPE_FALL,   
	.hreshape_rise   = PREVIEW_QVGA_FPS240_HRESHAPE_RISE,   
	.vreshape_fall   = PREVIEW_QVGA_FPS240_VRESHAPE_FALL,   
	.vreshape_rise   = PREVIEW_QVGA_FPS240_VRESHAPE_RISE,   
	.hoffset         = PREVIEW_QVGA_FPS240_HOFFSET,         
	.voffset         = PREVIEW_QVGA_FPS240_VOFFSET,         
	.hsize           = PREVIEW_QVGA_FPS240_HSIZE,           
	.vsize           = PREVIEW_QVGA_FPS240_VSIZE,           
	.lastlinepix     = PREVIEW_QVGA_FPS240_LINETOTAL,       
	.hdvdopos        = 0,                                   
	.conf_totalsync  = 0,                                   
	.conf_hdpol      = 0,                                   
	.conf_vdpol      = 0,                                   
	.conf_hdpclkedge = 0,                                   
	.conf_hvldbypass = 3,                                   
	.conf_vvldbypass = 0,                                   
	.frameblank_base = 0,                                   
	.vreshape_base   = PREVIEW_QVGA_FPS240_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,         
	.hdnum_frame_end = PREVIEW_QVGA_FPS240_HDNUM_FRAME_END, 
	.hdnum_cmdport   = PREVIEW_QVGA_FPS240_HDNUM_CMDPORT,   
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

static sensorTgCfg_t sensorTgCfg = {
	.tg_type = TGTYPE,
	.n_preview_submode = sizeof(sensorTgPreviewCfgTbl) /
			sizeof(sensorTgPreviewCfgTbl[0]),
	.n_snap_submode = sizeof(sensorTgSnapCfgTbl) /
			sizeof(sensorTgSnapCfgTbl[0]),
	.vshutter_need_resend = VSHUTTER_RESEND,
	.exp_effect_frame_count = EXP_FRAME_NUM,
#if HAS_STREAM_OFF
	.conf_has_abort = HAS_STREAM_OFF,
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
#if HAS_STREAM_OFF
		.streamoff = sensorDoFuncStreamOff,
		.streamon  = sensorDoFuncStreamOn,
#endif
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreview1080PCfg,
	.verStr = MODEL_VERSION,
};

void
sensorTg_ar0331_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
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

#if HAS_STREAM_OFF
static UINT8 is_aborted; 

static void
sensorDoFuncStreamOff(
		UINT32 abort
		)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
    frontReg0_t *pfrontReg = (frontReg0_t *)(LOGI_ADDR_REG);
    if(is_aborted == 0){
		PROF_LOG_ADD(3, "stream off s");
        if(sensorRead(0x301a)==0x5C && (!(pfrontReg->front2cdspgating&0x01)) ){
            PROF_LOG_ADD(3, "wait Vvalid End");       /*avoid sensor will halt on when Linear or HDR sequence load*/
            sensor_event_wait((1UL<<13));             /*SENSOR_EVENT_VVALID_END*/
        } 
        sensorSerialFlush(cmdportid);
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_off) / sizeof(cmd_stream_off[0]), cmd_stream_off[0], 0);  
        tmrUsWait(40000);                             /*avoid to LBUS error (reset cdsp reg) when "pvraw" */       
		PROF_LOG_ADD(3, "stream off e");
		is_aborted = 1;
	}
}

static void
sensorDoFuncStreamOn(
		UINT32 abort
		)
{
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
    if(is_aborted == 1 ){
	    PROF_LOG_ADD(3, "stream on s");
        /*stream on must before ALTM enable otherwise will halt on. 
          maybe not run "ALTM Enabled Devware Color Setup" now before ALTM enable. */
        sensorSerialFlush(cmdportid);  
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_on) / sizeof(cmd_stream_on[0]), cmd_stream_on[0], 0);
        if(smode != SENSOR_LINEAR ){ 
            PROF_LOG_ADD(3, "stream on s -HDR-  cmd_ALTM_Enable "); 
            sensorSerialFlush(cmdportid);
            sensorSerialBulkWrite(cmdportid, sizeof(cmd_ALTM_Enable)/sizeof(cmd_ALTM_Enable[0]),             cmd_ALTM_Enable[0]    , 0);  
        }
 		PROF_LOG_ADD(3, "stream on e");
		is_aborted = 0;
	}
}
#endif

void AptinaStreamOn()
{    
#if 1
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_ADD(3, "AptinaStreamOn s");
    /*stream on must before ALTM enable otherwise will halt on. 
      maybe not run "ALTM Enabled Devware Color Setup" now before ALTM enable. */
    sensorSerialFlush(cmdportid);  
    sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_on) / sizeof(cmd_stream_on[0]), cmd_stream_on[0], 0);
    if(smode != SENSOR_LINEAR ){ 
        PROF_LOG_ADD(3, "AptinaStreamOn -HDR- cmd_ALTM_Enable "); 
        sensorSerialFlush(cmdportid);
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_ALTM_Enable)/sizeof(cmd_ALTM_Enable[0]), cmd_ALTM_Enable[0], 0);  
    }
 	PROF_LOG_ADD(3, "AptinaStreamOn e");    
#endif    
}

void AptinaStreamOff()
{
#if 1
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
    frontReg0_t *pfrontReg = (frontReg0_t *)(LOGI_ADDR_REG);
    PROF_LOG_ADD(3, "AptinaStreamOff s");
    if(sensorRead(0x301a)==0x5C && (!(pfrontReg->front2cdspgating&0x01)) ){
        PROF_LOG_ADD(3, "wait Vvalid End");       /*avoid sensor will halt on when Linear or HDR sequence load*/
        sensor_event_wait((1UL<<13));             /*SENSOR_EVENT_VVALID_END*/
    } 
    sensorSerialFlush(cmdportid);
    sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_off) / sizeof(cmd_stream_off[0]), cmd_stream_off[0], 0);  
    tmrUsWait(40000);                             /*avoid to LBUS error (reset cdsp reg) when "pvraw" */  
	PROF_LOG_ADD(3, "AptinaStreamOff e");
#endif    
}

#if CAPTURE_CONTEXT_TEST
static void
sensorStreamContextOn(UINT32 sync)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	if (sync) {
		sensorSerialDoneForceWaitAll(cmdportid);
	}
	tmrUsWait(10000); /*This delay is needed before streaming on.*/
	if(sensorSerialBulkWrite(cmdportid, sizeof(cmd_context_a_on) / sizeof(cmd_context_a_on[0]), cmd_context_a_on[0], 0)==SUCCESS)
		PROF_LOG_ADD(3,"sensorStreamContextOn success");
	else
		PROF_LOG_ADD(3,"sensorStreamContextOn fail");
}
#endif

static void
sensorReportsize()
{
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
}


/*static*/ void
sensorMode(UINT32 mode, UINT32 func)    
{
    UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
    static UINT8 regdata[][4] SENSOR_SIF1DMA_VARBUF = {
		MI_REG(0x3082, 0x0000),    /* operation_mode_ctrl */
    #if 0        
        MI_REG(0x305E, 0x0000),    /* global_gain */    
    #endif    
	};

    /* T1/T2 Exposure Ratio */
    if(mode==0){
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_LinearSeqLoad)/sizeof(cmd_LinearSeqLoad[0]), cmd_LinearSeqLoad[0]  , 0);
        smode = SENSOR_LINEAR;
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_LinearMode)/sizeof(cmd_LinearMode[0]), cmd_LinearMode[0], 0);
        printf("Linear mode \n");
    }else if(mode==1){
        /* [HDR Mode  4x] */
        *(UINT16 *)&regdata[0][2] = 0x0000;
        #if 0
        *(UINT16 *)&regdata[1][2] = 0x2000;
        #endif
        smode = SENSOR_HDR_4X;
        printf("HDR mode set 4x\n");
    }else if(mode==2){
        /* [HDR Mode  8x] */
        *(UINT16 *)&regdata[0][2] = 0x0400;
        #if 0
        *(UINT16 *)&regdata[1][2] = 0x4000;
        #endif
        smode = SENSOR_HDR_8X;
        printf("HDR mode set 8x\n");
    }else if(mode==3){
        /* [HDR Mode 16x] */
        *(UINT16 *)&regdata[0][2] = 0x0800;
        #if 0
        *(UINT16 *)&regdata[1][2] = 0x8000;
        #endif
        smode = SENSOR_HDR_16X;
        printf("HDR mode set 16x\n");
    }else if(mode==4){
        /* [HDR Mode 32x] */
        *(UINT16 *)&regdata[0][2] = 0x0C00;
        #if 0
        *(UINT16 *)&regdata[1][2] = 0x0001;
        #endif
        smode = SENSOR_HDR_32X;
        printf("HDR mode set 32x\n");
    }  

    if(mode!=0){
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_HDRSeqLoad)/sizeof(cmd_HDRSeqLoad[0]), cmd_HDRSeqLoad[0]  , 0);
        sensorSerialBulkWrite(cmdportid, sizeof(regdata) / sizeof(regdata[0]), regdata[0], 0);
        #if 0     
            tmrUsWait(60000); 

            /*HDR Mode*/
            *(UINT16 *)&regdata[0][2] &= 0xFEFF;
            sensorSerialBulkWrite(cmdportid, 1, regdata[0], 0);
        #endif
        /*HDR Mode Setup*/
        if(func==0){      
            /* [2D motion compensation  ON], [DLO disabled]    */
            /* some noise in dark will be enhanced by motion compensation */
            sensorSerialBulkWrite(cmdportid, sizeof(cmd_DLO_off) / sizeof(cmd_DLO_off[0]), cmd_DLO_off[0], 0);
            sensorSerialBulkWrite(cmdportid, sizeof(cmd_2Dmotion_comp_on) / sizeof(cmd_2Dmotion_comp_on[0]), cmd_2Dmotion_comp_on[0], 0);            
            printf("2D on, DLO off \n");  
        }else if(func==1){
            /* [2D motion compensation OFF], [DLO enabled]     */
            sensorSerialBulkWrite(cmdportid, sizeof(cmd_2Dmotion_comp_off) / sizeof(cmd_2Dmotion_comp_off[0]), cmd_2Dmotion_comp_off[0], 0);
            sensorSerialBulkWrite(cmdportid, sizeof(cmd_DLO_on) / sizeof(cmd_DLO_on[0]), cmd_DLO_on[0], 0);            
            printf("2D off, DLO on \n");              
        }
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_HDRMode_SetUp) / sizeof(cmd_HDRMode_SetUp[0]), cmd_HDRMode_SetUp[0], 0);
    }        
}

static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
	PROF_LOG_ADD(1, __FUNCTION__);
    PROF_LOG_PRINT(1, "________Sensor driver build time <<%s>>", BUILD_TIME); 
    /* TGGPIO configuration here */
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

    /*TG PLL=MCLK out=24MHz*/
	sensorTgPllClkExtSet(24000000);/*Lowest input clock provided from ASIC is 16MHz - EXTCLK*/

    /* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);

#if CVR_RDK
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

	PROF_LOG_ADD(1,"MT AR0331 ");
	PROF_LOG_PRINT(3," sensor DoFunInit model-ID %x",sensorRead(0x3000));
	sensorLVDSCfgSet(&sensorHiSpiCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

	if(1){
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,300000); /* 300MHz */
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /* enable preview mode 9*/
	}
	else{
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,144000); /* 150 MHz */
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,0); /* enable preview mode 9*/
	}
	
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9, 1);    /* enable snap mode 9*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR, 1);
    smode = SENSOR_HDR_16X;/*SENSOR_HDR_16X;  SENSOR_LINEAR*/
      sensorSerialBulkWrite(cmdportid, sizeof(cmd_sw_reset)/sizeof(cmd_sw_reset[0]), cmd_sw_reset[0], 0);
#if 0    
	tmrUsWait(200000);
#else
    tmrUsWait(5000);
#endif
    /*sensorDoFuncStreamOff(0);*/
    if(smode==SENSOR_LINEAR){
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_LinearSeqLoad)/sizeof(cmd_LinearSeqLoad[0]), cmd_LinearSeqLoad[0]  , 0);
        PROF_LOG_ADD(3, "cmd_LinearSeqLoad");        
    }else{
        sensorSerialBulkWrite(cmdportid, sizeof(cmd_HDRSeqLoad)/sizeof(cmd_HDRSeqLoad[0]),       cmd_HDRSeqLoad[0]     , 0);
        PROF_LOG_ADD(3, "cmd_HDRSeqLoad");   
    }
    sensorSerialBulkWrite(cmdportid, sizeof(cmd_pll_dataformat)/sizeof(cmd_pll_dataformat[0]),   cmd_pll_dataformat[0] , 0);
    sensorSerialBulkWrite(cmdportid, sizeof(cmd_1080_fps60_init)/sizeof(cmd_1080_fps60_init[0]), cmd_1080_fps60_init[0], 0);
    sensorMode(SENSOR_HDR_16X/*SENSOR_HDR_16X  SENSOR_LINEAR*/, SENSOR_2D /*SENSOR_DLO*/); 
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_init)/sizeof(cmd_init[0]),                       cmd_init[0]           , 0);

#if 0    
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0]),             cmd_stream_on[0]      , 0);
#else
    AptinaStreamOn();
#endif
    
#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif

#if HAS_STREAM_OFF
	sensorStsCfg(SENSOR_SYS_CFG_FLOW_USE_ABORT, HAS_STREAM_OFF);
#endif
    
#if (_SENSOR_DBG_)
	/*debug by TGGPIO2*/ 
	WRITE8(0xb0009030, (READ8(0xb0009030)|(0x02)));
	DBG_GPIO(0);
#endif 
}

static void
sensorDoFuncStop()
{
}

static UINT8 snap_flag;
static void
sensorDoFuncPreview1080PBegin()
{
	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	PROF_LOG_ADD(3," 1080_60fps: init");
	if(snap_flag){
		sensor_event_wait((1UL<<13));   /*SENSOR_EVENT_VVALID_END*/
		snap_flag = 0;
	}
#if HAS_STREAM_OFF
	if (is_aborted){
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigInit;/*sensorSyncSigSet;*/
	}
	fpssg(&sensorSyncSigPreview1080PCfg);	
    SP5K_SET_LVDS_HCROP(PREVIEW_1080P_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    AptinaStreamOff();
	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_1080_fps60_init)/sizeof(cmd_1080_fps60_init[0]), cmd_1080_fps60_init[0]);
	sensorSerialFlush(cmdportid);
    AptinaStreamOn();
	PROF_LOG_ADD(3,"HiSPi 1080_60fps done");
}


static void
sensorDoFuncPreview1080Fps30Begin()
{
	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	PROF_LOG_ADD(3," 1080_30fps: init");
	if(snap_flag){
		sensor_event_wait((1UL<<13));    /*SENSOR_EVENT_VVALID_END*/
		snap_flag = 0;
	}

#if HAS_STREAM_OFF
	if (is_aborted){
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigInit;/*sensorSyncSigSet;*/
	}
	fpssg(&sensorSyncSigPreview1080Fps30Cfg);
    SP5K_SET_LVDS_HCROP(PREVIEW_1080_FPS30_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    AptinaStreamOff();
	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_1080_fps30_init)/sizeof(cmd_1080_fps30_init[0]), cmd_1080_fps30_init[0]);
	sensorSerialFlush(cmdportid);
    AptinaStreamOn();
	PROF_LOG_ADD(3, " HiSPi 1080_30fps done");
}

static void
sensorDoFuncPreviewVgaFps120Begin()
{
#if 1
    /* No this sensor mode           */
    printf("No this sensor mode\n");
#else
/* VGA@120fps is lnear mode now. */

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	PROF_LOG_ADD(3," vga_120fps init");
	if(snap_flag){
		sensor_event_wait((1UL<<13));    /*SENSOR_EVENT_VVALID_END*/
		snap_flag = 0;
	}
#if HAS_STREAM_OFF
	if (is_aborted){
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigInit;/*sensorSyncSigSet;*/
	}
	fpssg(&sensorSyncSigPreviewVgaFps120Cfg);
    SP5K_SET_LVDS_HCROP(PREVIEW_VGA_FPS120_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    AptinaStreamOff();
	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_vga_fps120_init)/sizeof(cmd_vga_fps120_init[0]), cmd_vga_fps120_init[0]);
	sensorSerialFlush(cmdportid);
    AptinaStreamOn();
	PROF_LOG_ADD(3, " HiSPi vga_120fps done");
#endif    
}

static void
sensorDoFuncPreviewQvgaFps240Begin()
{
#if 1
    /* No this sensor mode           */
    printf("No this sensor mode\n");
#else
    /* QVGA@240fps is lnear mode now and
       need "HAS_STREAM_OFF = 1"  mode switch will normal.
       maybe the uCode is problem bcs other sensor mode are ok.
    */

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	PROF_LOG_ADD(3," qvga_240fps init");
	if(snap_flag){
		sensor_event_wait((1UL<<13));    /*SENSOR_EVENT_VVALID_END*/
		snap_flag = 0;
	}

#if HAS_STREAM_OFF
	if (is_aborted){
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigInit;/*sensorSyncSigSet;*/
	}
	fpssg(&sensorSyncSigPreviewQvgaFps240Cfg);
    SP5K_SET_LVDS_HCROP(PREVIEW_QVGA_FPS240_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    /*AptinaStreamOff();*/
	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_qvga_fps240_init)/sizeof(cmd_qvga_fps240_init[0]),	cmd_qvga_fps240_init[0]);
	sensorSerialFlush(cmdportid);
    /*AptinaStreamOn();*/
	PROF_LOG_ADD(3, " HiSPi qvga_240fps done");
#endif    
}

static void
sensorDoFuncPreviewShortExp()
{
}
static void
sensorDoFuncPreviewExpEnd()
{
}

static void
sensorDoFuncSnapExpBegin()
{
	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	snap_flag = 1;
    AptinaStreamOff();
   	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_still_init) / sizeof(cmd_still_init[0]), cmd_still_init[0]); 
	sensorSerialFlush(cmdportid);
	#if HAS_STREAM_OFF
		if (is_aborted){
			fpssg = sensorSyncSigInit;
		}else
	#endif
		{
			fpssg = sensorSyncSigInit;/*sensorSyncSigSet;*/
		}
		fpssg(&sensorSyncSigSnapCfg);

    SP5K_SET_LVDS_HCROP(CAPTURE_HCROP_OFFSET);
    
	#if !CAPTURE_PREVIEW_MODE
		sensorDsenExtSet();
	#else
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	#endif
	
    #if HAS_STREAM_OFF
	    sensorDoFuncStreamOn(0);
    #else
        AptinaStreamOn();
    #endif

	PROF_LOG_ADD(3, __FUNCTION__);
	PROF_LOG_ADD(3, " HiSPi Snap done");
}

/*static void
sensorDoFuncSnapEarly0()
{
	DBG_GPIO(1);
	snap_flag = 1;
}*/

static void
sensorDoFuncSnapBshutterBegin()
{
}

static void
sensorDoFuncSnapBshutterFollow()
{
}

/*static void
sensorDoFuncSnapBshutterEnd()
{
}*/

static void sensorDoFuncSnap_dark_BshutterEnd()
{
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
	/*void (*fpssg)(const sensorSyncSigCfg_t *pcfg);*/

    #if PROF_LOG_EN
	    g_cap_count++;
    #endif
    PROF_LOG_ADD(3, __FUNCTION__);
	PROF_LOG_ADD(1, __FUNCTION__);
	PROF_LOG_PRINT(1, "sensorDoFuncSnapReadField0 count=%d",g_cap_count);
    PROF_LOG_PRINT(3, "sensorDoFuncSnapReadField0 count=%d",g_cap_count);
}

static void
sensorDoFuncSnapFieldEnd()
{
	#if S2S_TIME_LOG
  	    g_time_log[7]=tmrTimeClockGet();
	#endif
    PROF_LOG_ADD(3, __FUNCTION__);
}
 
static void
sensorDoFuncSnapReadoutEnd()
{
	/*UINT32 t1;*/
	#if S2S_TIME_LOG
	    g_time_log[8]=tmrTimeClockGet();
	    g_time_log[9]=tmrTimeClockGet();
	#endif
    PROF_LOG_ADD(3, __FUNCTION__);
    sensorReportsize();
}

static void
sensorDoFuncSnapNext(UINT32 mode)
{
    PROF_LOG_ADD(3, __FUNCTION__);
#if BURST_CAP_EN
	if(mode==0x40){
		PROF_LOG_ADD(0,"@@@@ get sensor next mode is snap");
		PROF_LOG_PRINT(1,"@@@@ get sensor next mode is snap snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
		sensorTgSnapRollCfg.skip_snap_readout_vd = 3;/*20110830 @ BASEFW change to 3*/
	}
	else{
		PROF_LOG_ADD(0,"@@@@ get sensor next mode is preview");
		PROF_LOG_PRINT(1,"@@@@ get sensor next mode is preview snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
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
#if 0
    if((sensorModeGet()&0xF0)==0x40){
        PROF_LOG_PRINT(0," Capture nreg:%d [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);                              
    }else{
        PROF_LOG_PRINT(0," Preview nreg:%d [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);                              
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
}

#if SP5K_SENSOR_AR0331
void sensorCmdPortExpLineSend(UINT16 expLine)
{
    UINT8 i;
	static UINT8 regdata[][4] SENSOR_SIF1DMA_VARBUF = {
		MI_REG(0x3012,0x0000),
	};
	*(UINT16 *)&regdata[0][2] = ((expLine>>8)&0xff) | ((expLine&0xff)<<8);
	PROF_LOG_PRINT(1,"expLine=%d",expLine);
    printf("expLine = %d\n",  expLine);

#if 0    
	sensorSerialDirectBufSnoop(regdata[0], sizeof(regdata));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regdata[0]);
#else
    sensorSerialBulkWrite(sensorTgCfg.cmdportid_exp, sizeof(regdata)/sizeof(regdata[0]), regdata[0], 0);   
    for(i=0; i<10; i++){
        sensor_event_wait(0x04);/*wait new frame */
    }
    #if 0  
        /* for check effect frame */
        printf("CmdPort-sensorAEMeanGet=%d \n", sensorAEMeanGet());
    #endif
#endif
}

UINT32 sensorAEMeanGet()
{
    UINT32 tmp;
    if(smode==SENSOR_LINEAR){
        tmp = sensorRead(0x3152);
    }else{
        tmp = sensorRead(0x3152)<<4;
    }
    printf("sensorAEMeanGet=%d \n", tmp);
    return tmp;

    /*  0x3150(16 bit) = always 0
        0x3152(16 bit) = 0xffff (max.) include exp and gain effect.
        Linear mode v.s HDR mode
            the AEMean of Linear is multiple of 18.x the HDR mode. 
    */
}
#endif
