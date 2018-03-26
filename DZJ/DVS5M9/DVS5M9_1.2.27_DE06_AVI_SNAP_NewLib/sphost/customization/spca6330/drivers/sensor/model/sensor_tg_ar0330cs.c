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
 *  Author: Vincent Tsao                                                  *
 *
 *  1.01.08, 20121224 Ryan.Kuo 
 *           fix sensor isp use flip and mirror function 
 *           will get color bar on image@VGA mode  
 *
 *  2.01.09, 20130123 Ryan.Kuo 
 *           issue: VGA mode will get pink image when image sensor face
 *                  high light and apply short exposure time and small gain.
 *
 *           fix: "clock per line" must be more than "1135" to avoid get 
 *                pink image and fine tune "line per frame" to keep 
 *                frame rate.
 *
 * 2.01.10, 20130124 Ryan.Kuo 
 *          mark: Mode_9_PV_BP (6 pixels) 
 *
 * 3.01.11, 20130124 Ryan.Kuo 
 *          1. add 720p@60fps crop from 2256x1504, FOV become 56%.
 *          2. add PCLK1x_102MHZ for save power consumption.
 *
 * 3.01.12, 20130218 Ryan.Kuo 
 *          1. replace FACTOR_PCLK1X define from PCLK1X_288MHZ with PCLK1x_102MHZ.
 *          2. add dump sensor register function. 
 *
 * 3.01.13, 20130219 Ryan.Kuo 
 *          1. 1152x648 use binning mode to get better jagged than skip mode.
 *          2. fine tune R.0x300C(LINE_LENGTH_PCK), R.0x30BA(Digital_Ctrl) to avoid
 *             linearity issue, pink image issue and horizontal bar issue.             
 *
 * 3.01.14, 20130219 Ryan.Kuo 
 *          2 lane MIPI i/f                                            PCLK1x   CDSPclk
 *          [MIPI Timing - 12bit 588Mbps, PCLK1x=588/12*2= 98MHz]      102MHz   204MHz
 *          [MIPI Timing - 10bit 490Mbps, PCLK1x=490/10*2= 98MHz]      102MHz   204MHz
 *
 *          Preview mode:
 *              1152 x  645 @60fps
 *              2304 x 1293 @30fps
 *              1280 x  720 @60fps
 *
 *          Snap mode:
 *              2304 x 1293 @30fps
 *
 * 4.01.15, 20130308 Ryan.Kuo 
 *          1. 1152x648@60fps
 *             skew/jag line when strong light and use short exposure time.
 *             use "AR0330CS_2lane_MIPI.ini" replace with "1152x648 60fps 12bit - MIPI - 98MPixels - 2lane MIPI-20120828-Joyce.txt"
 *
 * 4.01.16, 20130313 Ryan.Kuo
 *          1. Capture -> VGA Preview will halt on
 *             remove cmd_init_sequencer.
 *  
 *
 * 5.01.17, 20130322 Ryan.Kuo
 *          1. 1152 x  645 @60fps, use "AR0330CS_2lane_MIPI.ini"
 *             when strong light and short exposure time, exist 2 issue.
 *             a. must do the flow: sream off -> uCode -> stream on
 *                otherwise pink image. 
 *
 *             b. R.0x3040 = 0x2000, digital col_binning and row skipping 
 *                fix yellowish and jagged. 
 *
 * 6.01.18, 20130403 Ryan.Kuo 
 *          1. 1152 x  645 @60fps, use cmd_vga_init "$unp_tg_AR0330CS-4.01.16"
 *             bcs 5.01.17 has vertical bar in dark region.
 *             a. must strea on /off in mode change, so sync EOF and follow the sequence:
 *                stream off-> uCode -> Stream on.
 *             
 *          2. Optimize I2C flush time
 *             2.1  Speed up FRONT_SSC_CLK_768K from FRONT_SSC_CLK_192K.
 *             2.2  remove cmd_init_sequencer in sensor mode change, just do in initial phase.
 *                  Note: 
 *                     cmd_init_sequencer cost 40ms (FRONT_SSC_CLK_192K)
 *                                             28ms (FRONT_SSC_CLK_768K)
 * 
 * 
 * 7.01.19, 20130412 Ryan.Kuo 
 *          1. 1152 x  645 @60fps, use cmd_vga_init "$unp_tg_AR0330CS-5.01.17"
 *             but 5.01.17 has vertical bar in dark region. 
 *             -> use "AR0330CS_iCatch_Spreadsheet.xls" to decide OTPM revsion"
 *                need to more testing this issue and maybe try to increase LineLengthPclk. 
 *              
 *          2. FHD_2112x1188, 1080p@30fps  w/o Hscale
 *
 * 8.01.20, 20130417 Ryan.Kuo 
 *          1. 1152 x  645 @60fps has fixed noise patterm in dark region.
 *             a. increase LineLengthPclk.
 *             b. don't set seq_short.
 *
 *             Note: 
 *                 OTPM_Ver: V4
 *
 * 8.01.21, 20130725 Allen.Liu
 *          1. VGA and FHD has vertical bar in dark region.
 *            -> add delay time after sensor's software reset
 *
 *             Note: 
 *                 OTPM_Ver: V4
 *
 * 8.01.22, 20130827 Allen.Liu
 *          1. Solve pink noise at light area for OTPM V5
 *
 ******************************************************************************/
                 
#define MODEL_VERSION "$unp_tg_AR0330CS-8.01.22"
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

#if SP5K_ICAT_EIS
#define HAS_GSEN4EIS      1
#endif

#define  SENSOR_PROF_LOG_EN   (1)
#define  FHD_2112x1188        (1)  

#if SENSOR_PROF_LOG_EN
    #define PROF_ID           (0)
    #define SENSOR_PROF_LOG_ADD(str)			profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)	profLogPrintf(PROF_ID, fmt, ##arg)
    
    #define WRITELINE (*((volatile UINT16 *)(0xb0009064)) = (1))
    #define READLINE (*((volatile UINT16 *)(0xb0009064)))
    #define PROF_LOG_LINE_COUNT(str,args...)\
    do{\
        WRITELINE;\
        profLogPrintf(0,"%s_%d @ %d "str,__func__ ,__LINE__,READLINE,##args);\
        profLogPrintf(1,"%s_%d @ %d "str,__func__ ,__LINE__,READLINE,##args);\
    }while(0)
#else
    #define SENSOR_PROF_LOG_ADD(str)
    #define SENSOR_PROF_LOG_PRINT(fmt,arg...)
    #define PROF_LOG_LINE_COUNT(str,args...) 
#endif

#define DDR_CLOCK_297MHz           (1)  
#define PCLK1X_102MHZ              (1) 
#define SUSPEND_ENABLE             (1)
#define STREAMOF_ENABLE            (0)      
#define MODE_SYNC_EARLY            (50) 

#if defined(HW_EVB_SPE7001)
#define MIPI_RAW12                 (0)	/*In sensor_afe_ar0330cs.c, nbit_rgb = FRONT_IMG_SENSOR_BIT_10 or FRONT_IMG_SENSOR_BIT_12*/
#else
#define MIPI_RAW12                 (1)	/*In sensor_afe_ar0330cs.c, nbit_rgb = FRONT_IMG_SENSOR_BIT_10 or FRONT_IMG_SENSOR_BIT_12*/
#endif

#define SP5K_SET_LVDS_HCROP(a)    sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,a,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF)

static UINT32 g_IsSnap2Pv;   
static UINT32 sensorSuspended;
static UINT32 snap_time_log[5];
static BOOL   fpvsusp;
#if STREAMOF_ENABLE
static UINT8  is_aborted;
#endif
static UINT16 OTPM1, OTPM2, OTPM3;


#define TIME_TAG(id)   (snap_time_log[id])
#define TIME_LOG(id)    snap_time_log[id]=tmrTimeClockGet()
#define TIME_DIFF(a,b) (snap_time_log[a]-snap_time_log[b])

#define SENSOR_IRQ_SVC_FRAME_END       (1)
#define SENSOR_IRQ_SVC_NEG_VD          (10)
#define SENSOR_IRQ_SVC_ORG_VD          (12)
#define SENSOR_EVENT_NEW_FRAME		   (1UL<<2)
#define SENSOR_EVENT_VVALID_END        (1UL<<13)
#define SENSOR_EVENT_CDSP_EOF          (1UL<<16)
                                         
#define NO                        0          
#define BY_PIXEL                  1        
#define BY_HD                     0        
#define NOUSE                     0        
#define DISABLE              0xFFFF        
#define DELAY_US 		     0xFF00
#define END_CMD  		     0xFFFF
   
#define SENSOR_FREX          (1<<4)        
#define SENSOR_PWRDN         (1<<14)       
#define SENSOR_TEST          (1<<3)      

#if defined(HW_EVB_SPE7001) || defined(HW_RDK_6330)
#define SENSOR_RESET         (1<<4)
#else
#define SENSOR_RESET         (1<<0)  
#endif
  

#define OUT_SENSOR_GPIO_MASK     SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define CFG_SENSOR_GPIO_DIR      SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define SENOR_GPIO_GRP           SP5K_GPIO_GRP_TGL

#if PCLK1X_102MHZ
#define FACTOR_PCLK1X(a)   (a*102/288)   
#else
#define FACTOR_PCLK1X(a)   (a*288/288)  
#endif
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_HOUTPUT     (1152)
#define PREVIEW_VGA_VOUTPUT     (648)   
#define PREVIEW_1080P_HOUTPUT   (2304)
#define PREVIEW_1080P_VOUTPUT   (1296)
#define PREVIEW_720P_HOUTPUT    (1280)
#define PREVIEW_720P_VOUTPUT    ( 723)
#define PREVIEW_EXTEND_LINES    (12)

#define PREVIEW_VGA_HSIZE       (PREVIEW_VGA_HOUTPUT)         /*-SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP)*/
#define PREVIEW_VGA_VSIZE       (PREVIEW_VGA_VOUTPUT-3-1)     /* -1 for even */
#define PREVIEW_1080P_HSIZE     (PREVIEW_1080P_HOUTPUT)       /*-SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP)*/
#define PREVIEW_1080P_VSIZE     (PREVIEW_1080P_VOUTPUT-3-1)   /* -1 for even */   
#define PREVIEW_720P_HSIZE      (PREVIEW_720P_HOUTPUT)        /*-SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP)*/
#define PREVIEW_720P_VSIZE      (PREVIEW_720P_VOUTPUT-3)      
#define CAPTURE_HSIZE           (PREVIEW_1080P_HSIZE)	
#define CAPTURE_VSIZE           (PREVIEW_1080P_VSIZE) 
/* Preview default definition here */
/*****TODO***multiple preview mode for VGA*/
#define PREVIEW_VGA_LINETOTAL                FACTOR_PCLK1X(0x1800)                        
#define PREVIEW_VGA_LINEBLANK                (6	    )                        
#define PREVIEW_VGA_FRAMETOTAL               (PREVIEW_VGA_VSIZE    )                       
#define PREVIEW_VGA_FRAMEBLANK               (PREVIEW_VGA_VOUTPUT-1)      /* 1st is bad line.*/                       
#define PREVIEW_VGA_HOFFSET                  FACTOR_PCLK1X(0xc00 )                                                               
#define PREVIEW_VGA_HCROP_OFFSET		 	 (0     ) 	                  /* lvds crop offset (4N-1 BUG!) */              
#define PREVIEW_VGA_VOFFSET                  (PREVIEW_EXTEND_LINES+2)                    
#define PREVIEW_VGA_HRESHAPE_FALL            FACTOR_PCLK1X(0xa00 )                       
#define PREVIEW_VGA_HRESHAPE_RISE            FACTOR_PCLK1X(0xb00 )                       
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL     (BY_HD )                       
#define PREVIEW_VGA_VRESHAPE_FALL            (PREVIEW_VGA_VOFFSET-2)                        
#define PREVIEW_VGA_VRESHAPE_RISE            (PREVIEW_VGA_VOFFSET-1)        
                                                                            
#define PREVIEW_1080P_LINETOTAL              FACTOR_PCLK1X(0x1a00 )                       
#define PREVIEW_1080P_LINEBLANK              (6	    )                      
#define PREVIEW_1080P_FRAMETOTAL             (PREVIEW_1080P_VSIZE    )                       
#define PREVIEW_1080P_FRAMEBLANK             (PREVIEW_1080P_VOUTPUT-1)                       
#define PREVIEW_1080P_HOFFSET                FACTOR_PCLK1X(0x2c0 )   
#define PREVIEW_1080P_HCROP_OFFSET		 	 (0     )                     /* lvds crop offset (4N-1 BUG!) */                    
#define PREVIEW_1080P_VOFFSET                (PREVIEW_EXTEND_LINES+2)                       
#define PREVIEW_1080P_HRESHAPE_FALL          FACTOR_PCLK1X(0x1a0 )                       
#define PREVIEW_1080P_HRESHAPE_RISE          FACTOR_PCLK1X(0x1b0 )                       
#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL   (BY_HD )                       
#define PREVIEW_1080P_VRESHAPE_FALL          (PREVIEW_1080P_VOFFSET-2)                       
#define PREVIEW_1080P_VRESHAPE_RISE          (PREVIEW_1080P_VOFFSET-1)  

#define PREVIEW_720P_LINETOTAL               FACTOR_PCLK1X(0x1a00 )                        
#define PREVIEW_720P_LINEBLANK               (6	    )                      
#define PREVIEW_720P_FRAMETOTAL              (PREVIEW_720P_VSIZE    )                       
#define PREVIEW_720P_FRAMEBLANK              (PREVIEW_720P_VOUTPUT-1)                       
#define PREVIEW_720P_HOFFSET                 FACTOR_PCLK1X(0x2c0 )   
#define PREVIEW_720P_HCROP_OFFSET		 	 (0     )                     /* lvds crop offset (4N-1 BUG!) */                   
#define PREVIEW_720P_VOFFSET                 (PREVIEW_EXTEND_LINES+2)                       
#define PREVIEW_720P_HRESHAPE_FALL           FACTOR_PCLK1X(0x81E )                       
#define PREVIEW_720P_HRESHAPE_RISE           FACTOR_PCLK1X(0x878 )                       
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL    (BY_HD )                       
#define PREVIEW_720P_VRESHAPE_FALL           (PREVIEW_720P_VOFFSET-2)                       
#define PREVIEW_720P_VRESHAPE_RISE           (PREVIEW_720P_VOFFSET-1)                                            

#define CAPTURE_LINETOTAL                    (PREVIEW_1080P_LINETOTAL)             
#define CAPTURE_LINEBLANK                    (PREVIEW_1080P_LINEBLANK)           
#define CAPTURE_FRAMETOTAL                   (PREVIEW_1080P_FRAMETOTAL)           
#define CAPTURE_FRAMEBLANK                   (PREVIEW_1080P_FRAMEBLANK)           
#define CAPTURE_HOFFSET                      (PREVIEW_1080P_HOFFSET   )    
#define CAPTURE_HCROP_OFFSET                 (PREVIEW_1080P_HCROP_OFFSET) 
#define CAPTURE_VOFFSET                      (PREVIEW_1080P_VOFFSET   )                
#define CAPTURE_HRESHAPE_FALL                (PREVIEW_1080P_HRESHAPE_FALL)    
#define CAPTURE_HRESHAPE_RISE                (PREVIEW_1080P_HRESHAPE_RISE)    
#define CAPTURE_VRESHAPE_COUNTER_SEL         (BY_HD)    
#define CAPTURE_VRESHAPE_FALL		         (PREVIEW_1080P_VRESHAPE_FALL)   
#define CAPTURE_VRESHAPE_RISE		         (PREVIEW_1080P_VRESHAPE_RISE)    

#define	PREVIEW_HDNUM_FRAME_START            (110)                                                
#define	PREVIEW_VGA_HDNUM_FRAME_END	         (2  )             
#define	PREVIEW_VGA_HDNUM_CMDPORT	         (300)                           
#define	PREVIEW_1080P_HDNUM_FRAME_END        (2  )                                             
#define	PREVIEW_1080P_HDNUM_CMDPORT	         (300)   
#define	PREVIEW_720P_HDNUM_FRAME_END         (2  )                                             
#define	PREVIEW_720P_HDNUM_CMDPORT	         (300)                                                

#define	CAPTURE_HDNUM_FRAME_START            (110)                                          
#define	CAPTURE_HDNUM_FRAME_END	             (2  )                                       
#define	CAPTURE_HDNUM_CMDPORT	             (300)                                      
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
#define SENSORMODEL		              TYPE_CMOS	          /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                                    TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			              TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						                                    TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define PREVIEW_VGA_CFAPATTERN        BAYER_BGGR						                      
#define	PREVIEW_1080P_CFAPATTERN	  BAYER_BGGR          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	PREVIEW_720P_CFAPATTERN	      BAYER_BGGR

#define	SNAP_CFAPATTERN		          BAYER_BGGR          
#define	SNAP_DUMMYVD_NUM	          0             /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM		      1             /*snap readout Vd need numbers*/             
#define	SNAP_CAPTUREVD_NUM		      1             /*for typical CCD==n_dummy + n_readout + 1*/ 
#define	SNAP_EARLY_NUM		          0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		  2             /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	          0		        /*It arrange the start field data to the start position of the sdram,0,1,2 */             
#define	FLDSEL_END_NUM			 	  0             /*It arrange the end field data to the end position of the sdram,0,1,2 */                 
#define	SKIP_SNAP_READOUTVD_EN        0            	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	            /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	          0		        /*sony FALSE;panasonic/ITG TRUE*/
#define	EXP_FRAME_NUM       	      1	            /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/
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
 					                              /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						                            FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#define	TG_NBIT_REGDATA  16		                  /*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  16		                  /*ssc/spi register address numbers */
                                                          
#define MI_REG(a,b) { (a)>>8, (a)&0xFF, (b)>>8, (b)&0xFF, }

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
#if STREAMOF_ENABLE
static void sensorDoFuncStreamOff(UINT32 abort);
static void sensorDoFuncStreamOn (UINT32 abort);
#endif
#if HAS_GSEN4EIS
static void sensorEisTrig(void);
#endif
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreview1080pBegin();
static void sensorDoFuncPreview720pBegin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();
static void sensorDoFuncSnapNext(UINT32 next_mode);
static UINT32 sensorResumeExecuteLoad();
static UINT32 sensorSuspendExecuteLoad();

extern UINT32 tmrTimeClockGet();
extern UINT32 sensorModeGet();
/*-----------------------------------------------------------------------------------------------*/
static UINT16 cmd_stream_on[][2] SENSOR_SIF1DMA_BUF = {
    {0x301A, 0x005C},
    {END_CMD,END_CMD}    
};

static UINT16 cmd_stream_off[][2] SENSOR_SIF1DMA_BUF = {
	{0x301A, 0x0058},
    {END_CMD,END_CMD}    
};

static UINT16 cmd_init_1[][2] SENSOR_SIF1DMA_BUF=
{
/* AR0330CS_iCatch_Spreadsheet.xls */
	{0x301A, 0x0059},   /* [SW Reset]     */
#if 1
    {DELAY_US,10000  },    
#endif
    {0x31AE, 0x0202},	/* MIPI Output */   
    {0x301A, 0x0058},   /* Disable Streaming */   
#if 0    
    {DELAY_US,34000},
#endif    
    {0x3064, 0x1802},   /* Disable Embedded Data */  
    {0x3078, 0x0001},   /* Marker to say that 'Defaults' have been run */
    {0x31E0, 0x0003},   
    {0x3046, 0x4038},   /* for debug */
    {0x3048, 0x8480},   /* for debug */
    {END_CMD,END_CMD}
};

static UINT16 cmd_init_2_V1[][2] SENSOR_SIF1DMA_BUF=
{
    {0x30BA, 0x002C},
	{0x30FE, 0x0080}, 
    {0x31E0, 0x0000}, 
    {0x3ECE, 0x00FF},
    {0x3ED0, 0xE4F6}, 
    {0x3ED2, 0x0146}, 
    {0x3ED4, 0x8F6C}, 
    {0x3ED6, 0x66CC}, 
    {0x3ED8, 0x8C42}, 
    {0x3EDA, 0x8822}, 
    {0x3EDC, 0x2222}, 
    {0x3EDE, 0x22C0}, 
    {0x3EE0, 0x1500}, 
    {0x3EE6, 0x0080}, 
    {0x3EE8, 0x2027}, 
    {0x3EEA, 0x001D}, 
    {0x3F06, 0x046A},
    {END_CMD,END_CMD},       
};

static UINT16 cmd_init_2_V2[][2] SENSOR_SIF1DMA_BUF=
{
    {0x30BA, 0x002C},
	{0x30FE, 0x0080},          
    {0x31E0, 0x0000},          
    {0x3ECE, 0x00FF},         
    {0x3ED0, 0xE4F6},          
    {0x3ED2, 0x0146},          
    {0x3ED4, 0x8F6C},          
    {0x3ED6, 0x66CC},          
    {0x3ED8, 0x8C42},          
    {0x3EDA, 0x889B},          
    {0x3EDC, 0x8863},          
    {0x3EDE, 0xAA04},          
    {0x3EE0, 0x15F0},          
    {0x3EE6, 0x008C},          
    {0x3EE8, 0x2024},          
    {0x3EEA, 0xFF1F},          
    {0x3F06, 0x046A},
    {END_CMD,END_CMD},       
};

static UINT16 cmd_init_2_V3[][2] SENSOR_SIF1DMA_BUF=
{
    {0x3ED2, 0x0146}, 
    {0x3ED4, 0x8F6C}, 
    {0x3ED6, 0x66CC},  
    {0x3ED8, 0x8C42},
    {END_CMD,END_CMD}
};

static UINT16 cmd_init_2_V4[][2] SENSOR_SIF1DMA_BUF=
{
    {0x3ED2, 0x0146}, 
    {0x3ED6, 0x66CC},  
    {0x3ED8, 0x8C42},
    {END_CMD,END_CMD}
};

static UINT16 cmd_init_2_V5[][2] SENSOR_SIF1DMA_BUF=
{
    {0x3ED2, 0x0146}, 
    {END_CMD,END_CMD}
};

static UINT16 cmd_init_sequencerA[][2] SENSOR_SIF1DMA_BUF=
{
    {0x3088, 0x8000},
    {0x3086, 0x4540},
    {0x3086, 0x6134},
    {0x3086, 0x4A31},
    {0x3086, 0x4342},
    {0x3086, 0x4560},
    {0x3086, 0x2714},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DEA},
    {0x3086, 0x2704},
    {0x3086, 0x3D10},
    {0x3086, 0x2705},
    {0x3086, 0x3D10},
    {0x3086, 0x2715},
    {0x3086, 0x3527},
    {0x3086, 0x053D},
    {0x3086, 0x1045},
    {0x3086, 0x4027},
    {0x3086, 0x0427},
    {0x3086, 0x143D},
    {0x3086, 0xFF3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xEA62},
    {0x3086, 0x2728},
    {0x3086, 0x3627},
    {0x3086, 0x083D},
    {0x3086, 0x6444},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C2C},
    {0x3086, 0x4B01},
    {0x3086, 0x432D},
    {0x3086, 0x4643},
    {0x3086, 0x1647},
    {0x3086, 0x435F},
    {0x3086, 0x4F50},
    {0x3086, 0x2604},
    {0x3086, 0x2684},
    {0x3086, 0x2027},
    {0x3086, 0xFC53},
    {0x3086, 0x0D5C},
    {0x3086, 0x0D60},
    {0x3086, 0x5754},
    {0x3086, 0x1709},
    {0x3086, 0x5556},
    {0x3086, 0x4917},
    {0x3086, 0x145C},
    {0x3086, 0x0945},
    {0x3086, 0x0045},
    {0x3086, 0x8026},
    {0x3086, 0xA627},
    {0x3086, 0xF817},
    {0x3086, 0x0227},
    {0x3086, 0xFA5C},
    {0x3086, 0x0B5F},
    {0x3086, 0x5307},
    {0x3086, 0x5302},
    {0x3086, 0x4D28},
    {0x3086, 0x6C4C},
    {0x3086, 0x0928},
    {0x3086, 0x2C28},
    {0x3086, 0x294E},
    {0x3086, 0x1718},
    {0x3086, 0x26A2},
    {0x3086, 0x5C03},
    {0x3086, 0x1744},
    {0x3086, 0x2809},
    {0x3086, 0x27F2},
    {0x3086, 0x1714},
    {0x3086, 0x2808},
    {0x3086, 0x164D},
    {0x3086, 0x1A26},
    {0x3086, 0x8317},
    {0x3086, 0x0145},
    {0x3086, 0xA017},
    {0x3086, 0x0727},
    {0x3086, 0xF317},
    {0x3086, 0x2945},
    {0x3086, 0x8017},
    {0x3086, 0x0827},
    {0x3086, 0xF217},
    {0x3086, 0x285D},
    {0x3086, 0x27FA},
    {0x3086, 0x170E},
    {0x3086, 0x2681},
    {0x3086, 0x5300},
    {0x3086, 0x17E6},
    {0x3086, 0x5302},
    {0x3086, 0x1710},
    {0x3086, 0x2683},
    {0x3086, 0x2682},
    {0x3086, 0x4827},
    {0x3086, 0xF24D},
    {0x3086, 0x4E28},
    {0x3086, 0x094C},
    {0x3086, 0x0B17},
    {0x3086, 0x6D28},
    {0x3086, 0x0817},
    {0x3086, 0x014D},
    {0x3086, 0x1A17},
    {0x3086, 0x0126},
    {0x3086, 0x035C},
    {0x3086, 0x0045},
    {0x3086, 0x4027},
    {0x3086, 0x9017},
    {0x3086, 0x2A4A},
    {0x3086, 0x0A43},
    {0x3086, 0x160B},
    {0x3086, 0x4327},
    {0x3086, 0x9445},
    {0x3086, 0x6017},
    {0x3086, 0x0727},
    {0x3086, 0x9517},
    {0x3086, 0x2545},
    {0x3086, 0x4017},
    {0x3086, 0x0827},
    {0x3086, 0x905D},
    {0x3086, 0x2808},
    {0x3086, 0x530D},
    {0x3086, 0x2645},
    {0x3086, 0x5C01},
    {0x3086, 0x2798},
    {0x3086, 0x4B12},
    {0x3086, 0x4452},
    {0x3086, 0x5117},
    {0x3086, 0x0260},
    {0x3086, 0x184A},
    {0x3086, 0x0343},
    {0x3086, 0x1604},
    {0x3086, 0x4316},
    {0x3086, 0x5843},
    {0x3086, 0x1659},
    {0x3086, 0x4316},
    {0x3086, 0x5A43},
    {0x3086, 0x165B},
    {0x3086, 0x4327},
    {0x3086, 0x9C45},
    {0x3086, 0x6017},
    {0x3086, 0x0727},
    {0x3086, 0x9D17},
    {0x3086, 0x2545},
    {0x3086, 0x4017},
    {0x3086, 0x1027},
    {0x3086, 0x9817},
    {0x3086, 0x2022},
    {0x3086, 0x4B12},
    {0x3086, 0x442C},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C00},
};

static UINT16 cmd_init_sequencerB[][2] SENSOR_SIF1DMA_BUF=
{
    {0x3088, 0x8000},
    {0x3086, 0x4A03},
    {0x3086, 0x4316},
    {0x3086, 0x0443},
    {0x3086, 0x1645},
    {0x3086, 0x4045},
    {0x3086, 0x6017},
    {0x3086, 0x2045},
    {0x3086, 0x404B},
    {0x3086, 0x1244},
    {0x3086, 0x6134},
    {0x3086, 0x4A31},
    {0x3086, 0x4342},
    {0x3086, 0x4560},
    {0x3086, 0x2714},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DEA},
    {0x3086, 0x2704},
    {0x3086, 0x3D10},
    {0x3086, 0x2705},
    {0x3086, 0x3D10},
    {0x3086, 0x2715},
    {0x3086, 0x3527},
    {0x3086, 0x053D},
    {0x3086, 0x1045},
    {0x3086, 0x4027},
    {0x3086, 0x0427},
    {0x3086, 0x143D},
    {0x3086, 0xFF3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xEA62},
    {0x3086, 0x2728},
    {0x3086, 0x3627},
    {0x3086, 0x083D},
    {0x3086, 0x6444},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C2C},
    {0x3086, 0x4B01},
    {0x3086, 0x432D},
    {0x3086, 0x4643},
    {0x3086, 0x1647},
    {0x3086, 0x435F},
    {0x3086, 0x4F50},
    {0x3086, 0x2604},
    {0x3086, 0x2684},
    {0x3086, 0x2027},
    {0x3086, 0xFC53},
    {0x3086, 0x0D5C},
    {0x3086, 0x0D57},
    {0x3086, 0x5417},
    {0x3086, 0x0955},
    {0x3086, 0x5649},
    {0x3086, 0x5307},
    {0x3086, 0x5302},
    {0x3086, 0x4D28},
    {0x3086, 0x6C4C},
    {0x3086, 0x0928},
    {0x3086, 0x2C28},
    {0x3086, 0x294E},
    {0x3086, 0x5C09},
    {0x3086, 0x6045},
    {0x3086, 0x0045},
    {0x3086, 0x8026},
    {0x3086, 0xA627},
    {0x3086, 0xF817},
    {0x3086, 0x0227},
    {0x3086, 0xFA5C},
    {0x3086, 0x0B17},
    {0x3086, 0x1826},
    {0x3086, 0xA25C},
    {0x3086, 0x0317},
    {0x3086, 0x4427},
    {0x3086, 0xF25F},
    {0x3086, 0x2809},
    {0x3086, 0x1714},
    {0x3086, 0x2808},
    {0x3086, 0x1701},
    {0x3086, 0x4D1A},
    {0x3086, 0x2683},
    {0x3086, 0x1701},
    {0x3086, 0x27FA},
    {0x3086, 0x45A0},
    {0x3086, 0x1707},
    {0x3086, 0x27FB},
    {0x3086, 0x1729},
    {0x3086, 0x4580},
    {0x3086, 0x1708},
    {0x3086, 0x27FA},
    {0x3086, 0x1728},
    {0x3086, 0x5D17},
    {0x3086, 0x0E26},
    {0x3086, 0x8153},
    {0x3086, 0x0117},
    {0x3086, 0xE653},
    {0x3086, 0x0217},
    {0x3086, 0x1026},
    {0x3086, 0x8326},
    {0x3086, 0x8248},
    {0x3086, 0x4D4E},
    {0x3086, 0x2809},
    {0x3086, 0x4C0B},
    {0x3086, 0x6017},
    {0x3086, 0x2027},
    {0x3086, 0xF217},
    {0x3086, 0x535F},
    {0x3086, 0x2808},
    {0x3086, 0x164D},
    {0x3086, 0x1A17},
    {0x3086, 0x0127},
    {0x3086, 0xFA26},
    {0x3086, 0x035C},
    {0x3086, 0x0145},
    {0x3086, 0x4027},
    {0x3086, 0x9817},
    {0x3086, 0x2A4A},
    {0x3086, 0x0A43},
    {0x3086, 0x160B},
    {0x3086, 0x4327},
    {0x3086, 0x9C45},
    {0x3086, 0x6017},
    {0x3086, 0x0727},
    {0x3086, 0x9D17},
    {0x3086, 0x2545},
    {0x3086, 0x4017},
    {0x3086, 0x0827},
    {0x3086, 0x985D},
    {0x3086, 0x2645},
    {0x3086, 0x4B17},
    {0x3086, 0x0A28},
    {0x3086, 0x0853},
    {0x3086, 0x0D52},
    {0x3086, 0x5112},
    {0x3086, 0x4460},
    {0x3086, 0x184A},
    {0x3086, 0x0343},
    {0x3086, 0x1604},
    {0x3086, 0x4316},
    {0x3086, 0x5843},
    {0x3086, 0x1659},
    {0x3086, 0x4316},
    {0x3086, 0x5A43},
    {0x3086, 0x165B},
    {0x3086, 0x4327},
    {0x3086, 0x9C45},
    {0x3086, 0x6017},
    {0x3086, 0x0727},
    {0x3086, 0x9D17},
    {0x3086, 0x2545},
    {0x3086, 0x4017},
    {0x3086, 0x1027},
    {0x3086, 0x9817},
    {0x3086, 0x2022},
    {0x3086, 0x4B12},
    {0x3086, 0x442C},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C00},
    {0x3086, 0x0000},
};

static UINT16 cmd_init_sequencerP1[][2] SENSOR_SIF1DMA_BUF=
{
    {0x3088, 0x800C},
    {0x3086, 0x2045},
};

static UINT16 cmd_init_pll[][2] SENSOR_SIF1DMA_BUF=
{
#if MIPI_RAW12	
   /* [Serial 12-bit PLL 98MHz - 2lane MIPI] */
	    {0x302A, 6	   },	/* vt_pix_clk_d iv        */                      
        {0x302C, 2	   },   /* vt_sys_clk_d iv        */                                    
        {0x302E, 4	   },   /* pre_pll_clk_ div       */                                    
        {0x3030, 98   },    /* pll_multipli er        */                                    
        {0x3036, 12	   },   /* op_pix_clk_d iv        */                                    
        {0x3038, 1	   },   /* op_sys_clk_d iv        */
   /* [MIPI Timing - 12bit 588Mbps, PCLK1x=588/12*2= 98MHz]*/     
        {0x31AC, 0x0C0C},   /* data_format bit number */                                    
        {0x31B0, 36    },   /* FRAME PREAMB LE        */                                    
        {0x31B2, 12	   },   /* LINE PREAMBL E         */                                    
        {0x31B4, 0x2643},   /* MIPI TIMING  0         */                                    
        {0x31B6, 0x114E},   /* MIPI TIMING  1         */                                    
        {0x31B8, 0x2048},   /* MIPI TIMING 2          */                                    
        {0x31BA, 0x0186},   /* MIPI TIMING 3          */                                    
        {0x31BC, 0x8005},   /* MIPI TIMING 4          */                                    
        {0x31BE, 0x2003},   /* MIPI CONFIG STATUS     */	
#else
   /* [Serial 10-bit PLL 98MHz - 2lane MIPI] */
	    {0x302A, 5	   },	/* vt_pix_clk_d iv        */                      
        {0x302C, 2	   },   /* vt_sys_clk_d iv        */                                    
        {0x302E, 4	   },   /* pre_pll_clk_ div       */                                    
        {0x3030, 82   },    /* pll_multipli er        */                                    
        {0x3036, 10	   },   /* op_pix_clk_d iv        */                                    
        {0x3038, 1	   },   /* op_sys_clk_d iv        */
   /* [MIPI Timing - 10bit 490Mbps, PCLK1x=490/10*2= 98MHz]*/      
        {0x31AC, 0x0A0A},   /* data_format bit number */                                    
        {0x31B0, 40    },   /* FRAME PREAMB LE        */                                    
        {0x31B2, 14	   },   /* LINE PREAMBL E         */                                    
        {0x31B4, 0x2743},   /* MIPI TIMING  0         */                                    
        {0x31B6, 0x114E},   /* MIPI TIMING  1         */                                    
        {0x31B8, 0x2049},   /* MIPI TIMING 2          */                                    
        {0x31BA, 0x0186},   /* MIPI TIMING 3          */                                    
        {0x31BC, 0x8005},   /* MIPI TIMING 4          */                                    
        {0x31BE, 0x2003},   /* MIPI CONFIG STATUS     */
#endif           
#if 1
    {0x31E0, 0x0703},   /* Improve Thermal Noise     */
#endif    
	{END_CMD,END_CMD},                           
};

/*static UINT16 cmd_init_seq_short[][2] SENSOR_SIF1DMA_BUF=
{
	{0x3088, 0x80BA},
	{0x3086, 0x0253},
	{END_CMD,END_CMD},                           
};*/

static UINT16 cmd_vga_init[][2] SENSOR_SIF1DMA_BUF = 
{
#if 0
/* AR0330CS_2lane_MIPI.ini
    issue 1: skew/jag line when strong light and use short exposure time.
        fine tune 2 registers to fix it.
            R.0x3014 = 90       // default: 0
            R.0x30BA = 0x47     // default: 0x40 
*/
    #if  0  
        {0x3004, 0     },       /* X_ADDR_START: 2303-0+1=2304 */  /* 1152x2=2304 */           
        {0x3008, 2303  },       /* X_ADDR_END   */            
        {0x3002, 124   },       /* Y_ADDR_START: 1419-124+1=1296 */            
        {0x3006, 1419  },       /* Y_ADDR_END   */     
    #else
    /* Same as 1080p FOV */
        {0x3004, 6     },       /* X_ADDR_START: 2308-6+1=2303 */  
        {0x3008, 2309  },      
        {0x3002, 120   },       /* Y_ADDR_START: 1415-120+1=1296 */            
        {0x3006, 1415  },       /* Y_ADDR_END   */ 
    #endif    
    /* Sub-sampling */
    	{0x30A2,3      }, 		/* X_ODD_INCREMENT */  
    	{0x30A6,3	   }, 		/* Y_ODD_INCREMENT */  
        {0x3040, 0x3000},       /* READ_MODE       */  
    /* Frame-Timing */
        {0x300C, 1242  },       /* LINE_LENGTH_PCK             */             
        {0x300A, 661   },       /* FRAME_LENGTH_LINES          */       
        {0x3014, 90    },       /* FINE_INTEGRATION_TIME       */               
        {0x3042, 0	   },       /* EXTRA_DELAY                 */               
        {0x30BA, 0x47  }, 
#else
/*  1152x648 60fps 12bit - MIPI - 98MPixels - 2lane MIPI-20120828-Joyce.txt
      issue 1: pink image with short exposure and small gain when strong light.
      issue 2: non-linearity under 1 exposure frame. 
      --> fixed issue 1, 2 by must using stream on/off before change uCode.
      
      issue 3: yellowish and jagged when strong light and short exposure time
      --> workround: using digital col_binning and row skipping.
      
      issue 4: veritcal bar in dark regison.
      --> use "AR0330CS_iCatch_Spreadsheet.xls" to decide OTPM revsion"
          need to increase LineLengthPclk and must don't set "seq_short".
*/
   	{0x3004, 6	   },		/* X_ADDR_START: 2308-6+1=2303 */  
    {0x3008, 2309  },      
    {0x3002, 120   }, 	    /* Y_ADDR_START: 1415-120+1=1296 */            
    {0x3006, 1415  }, 	    /* Y_ADDR_END   */            
/* Sub-sampling */
	{0x30A2, 3     }, 		/* X_ODD_INCREMENT */  
	{0x30A6, 3     }, 		/* Y_ODD_INCREMENT */  
	{0x3040, 0x2000},  		/* READ_MODE        
                               0x3000: digital col_binning and digital row_binning  
                               0x2000, digital col_binning and row skipping
                                                                                    */ 
/* Frame-Timing */
    {0x300C, 1104+100 },    /* LINE_LENGTH_PCK             */             
    {0x300A, 740-62   },    /* FRAME_LENGTH_LINES          */   
    {0x3014, 16	   },       /* FINE_INTEGRATION_TIME       */               
    {0x3042, 968   },       /* EXTRA_DELAY                 */  
    {0x30BA, 0x002C},
    {END_CMD,END_CMD}, 
#endif    
};

static UINT16 cmd_1080p_init[][2] SENSOR_SIF1DMA_BUF = 
{
    /* [2304x1296 30fps 12bit - MIPI - 98MPixels - 2lane MIPI] */	
    /* ARRAY READOUT SETTINGS */
	{0x3004, 6	   },		/* X_ADDR_START */            
    {0x3008, 2309  }, 	    /* X_ADDR_END   */            
    {0x3002, 120   }, 	    /* Y_ADDR_START */            
    {0x3006, 1415  }, 	    /* Y_ADDR_END   */            

/* Sub-sampling */
	{0x30A2, 1     }, 		/* X_ODD_INCREMENT */  
	{0x30A6, 1     }, 		/* Y_ODD_INCREMENT */  
	{0x3040, 0x0000},  		/* READ_MODE       */  

/* Frame-Timing */
	{0x300C, 1248  },       /* LINE_LENGTH_PCK             */             
    {0x300A, 1308  },       /* FRAME_LENGTH_LINES          */               
    {0x3014, 16	   },       /* FINE_INTEGRATION_TIME       */               
    {0x3042, 0	   },       /* EXTRA_DELAY                 */               
    {0x30BA, 0x002C},       /* Digital_Ctrl_Adc_High_Speed */                  
	{END_CMD,END_CMD},    
};

static UINT16 cmd_720p_init[][2] SENSOR_SIF1DMA_BUF = 
{   /* AR0330CS_iCatch_Spreadsheet.xls                         */ 
    /* [1280x720 60fps 12bit - MIPI - 98MPixels - 2lane MIPI] */	
    /* ARRAY READOUT SETTINGS */
	{0x3004, 512   },		/* X_ADDR_START */            
    {0x3008, 1791  }, 	    /* X_ADDR_END   */            
    {0x3002, 412   }, 	    /* Y_ADDR_START */            
    {0x3006, 1131+3}, 	    /* Y_ADDR_END   */            

/* Sub-sampling */
	{0x30A2, 1     }, 		/* X_ODD_INCREMENT */  
	{0x30A6, 1     }, 		/* Y_ODD_INCREMENT */  
	{0x3040, 0x0000},  		/* READ_MODE       */  

/* Frame-Timing */
	{0x300C, 1014  },       /* LINE_LENGTH_PCK             */             
    {0x300A, 788+17},       /* FRAME_LENGTH_LINES drop frame to 59.95 from 61.25 fps */               
    {0x3014, 0	   },       /* FINE_INTEGRATION_TIME       */               
    {0x3042, 968	   },   /* EXTRA_DELAY                 */               
    {0x30BA, 0x006C},       /* Digital_Ctrl_Adc_High_Speed */                  
	{END_CMD,END_CMD},    
};

/*-----------------------------------------------------------------------------------------------*/
static const sensorLVDSCfg_t sensorMIPICfg =
{
    .mipi_nockrsten  =0 , 
	.wordcntsel 	 =0 ,	/* 0: hsize from From LVDS embeded data (CSI2)      */ 
	.Lvdsmode		 =0 ,	/* 0: 8bit ,analog marco bit mode.                  */ 
	.Lvdslanenum	 =1 ,	/* 1: 2 lane                                        */ 
	.Lvdslanemode	 =0 ,	/* 0: enable lane3 in 4 lanes mode                  */
	.Lvdslaneswap	 =1 ,	/* 1: LSB first                                     */   /*must LSB*/
#if MIPI_RAW12	 
	.Lvdsraw		 =1 ,	/* 1: raw 12                                        */
	.Ctrlword_sol	 =0x2C, 
#else
    .Lvdsraw		 =0 ,   /* 1: raw 10                                        */
    .Ctrlword_sol	 =0x2B,  
#endif	  

	.Lvdseofsel 	 =0 ,	/* 2: detect eof sync code, 1: count eol sync code, 0: count sol sync code  */  
	.Lvdseolsel 	 =0 ,	/* 0: count pixel number                            */  /* N.G: 1: detect eol sync code */
	.lvdsvdsrc		 =1 ,	/* 1: decode emedded data header sof as vd source   */
	.lvds_dphy_sel	 =1 ,	/* 1: MIPI D-PHY                                    */ 
	.Ctrlword_sof	 =0x00 ,
	.syncwrod0		 =0xB8 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	/*HW signal spec */
	.vmode			 =0 ,
	.S2pphase		 =0 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	/*Extention Part */
	.Exthd_num		 =PREVIEW_EXTEND_LINES-1,
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
	.hsize = PREVIEW_VGA_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
	.vsize = PREVIEW_VGA_VSIZE,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_MIPI_RAW, 
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio = 3200,
	.vratio = 1800,
#endif	
	.fps_mhz = 59980000,
	.sensor_pclk = 49000000,
	.sensor_htotal = 1104,
	.pix_100khz =1028, 
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
#if FHD_2112x1188    
	.crop = &( const sensorTgCrop_t ) {
	#if HAS_GSEN4EIS
        .xOffset = (PREVIEW_1080P_HOUTPUT-1920)/2,  /**/
        .yOffset = (PREVIEW_1080P_VOUTPUT-1080)/2,                  /**/
        .xSize = 1920,
        .ySize = 1080,
	#else
		.xOffset = (PREVIEW_1080P_HSIZE-2112)/2,
		.yOffset = (PREVIEW_1080P_VSIZE-1-1188)/2,
		.xSize = 2112,
		.ySize = 1188,
	#endif	
	},
#else
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_1080P_HSIZE,
		.ySize = PREVIEW_1080P_VSIZE,
	},
#endif    
	.hsize = PREVIEW_1080P_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW, 
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio = 3200,
	.vratio = 1800,
#endif	
	.fps_mhz = 30020000,
	.sensor_pclk = 49000000,
	.sensor_htotal = 1248,
	.pix_100khz =1028, 
};

static const sensorTgPreviewCfg_t sensorTgPreview720pCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720pBegin,
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
	.hsize = PREVIEW_720P_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
	.vsize = PREVIEW_720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW, 
	.cfa_pattern = PREVIEW_720P_CFAPATTERN,
#if 1 /*Edward 20100223 set logical sensor h/v size to fit aspect ratio*/
	.hratio = 3200,
	.vratio = 1800,
#endif	
	.fps_mhz = 59900000,
	.sensor_pclk = 49000000,
	.sensor_htotal = 1014,
	.pix_100khz =1028, 
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &sensorTgPreviewVGACfg,
    &sensorTgPreview1080pCfg, 
	&sensorTgPreview720pCfg, 
};


static sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = NULL,
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
		.pix_100khz =1028,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	 &sensorTgSnapRollCfg
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
	.hsize           = PREVIEW_VGA_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
	.vsize           = PREVIEW_VGA_VSIZE+1,
	.lastlinepix     = PREVIEW_VGA_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
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
	.hsize           = PREVIEW_1080P_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
	.vsize           = PREVIEW_1080P_VSIZE+1,
	.lastlinepix     = PREVIEW_1080P_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
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

static const sensorSyncSigCfg_t sensorSyncSigPreview720pCfg = {
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
	.hsize           = PREVIEW_720P_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
	.vsize           = PREVIEW_720P_VSIZE+1,
	.lastlinepix     = PREVIEW_720P_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
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
	.hsize           = CAPTURE_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
	.vsize           = CAPTURE_VSIZE,
	.lastlinepix     = CAPTURE_HSIZE,/*+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP),*/
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
#if STREAMOF_ENABLE
	.conf_has_abort = STREAMOF_ENABLE,
#endif
#if HAS_GSEN4EIS
    .conf_has_Gsen4EIS = HAS_GSEN4EIS,
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
		.streamon  = sensorDoFuncStreamOn,
#endif
#if HAS_GSEN4EIS
        .EIStrig = sensorEisTrig,
#endif

	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreviewVGACfg,
	.verStr = MODEL_VERSION,
};

void
sensorTg_ar0330cs_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
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
sensorMT_BatchWrite(UINT16 *pdata)
{
	UINT32 cmdportid;
	UINT8 regdata[4];
	UINT16 i;
	cmdportid = sensorTgCfg.cmdportid_xtg;
    sensorSerialFlush(cmdportid);
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
				SENSOR_PROF_LOG_PRINT("[%x %x %x %x]",regdata[0],regdata[1],regdata[2],regdata[3]);
            #endif
         	sensorSerialBulkWrite(cmdportid,1,regdata,0);
		}
	}
}

void	
sensorMT_SRST(void)
{
	UINT32 cmdportid, cnt;
	cmdportid = sensorTgCfg.cmdportid_xtg;
    
	OTPM1 = sensorMT_Read(0x300E);
    OTPM2 = sensorMT_Read(0x30F0);
    OTPM3 = sensorMT_Read(0x3072);
	SENSOR_PROF_LOG_PRINT("AR0330CS model-ID %x",sensorMT_Read(0x3000));
    SENSOR_PROF_LOG_PRINT("   OTPM 0x300E:%x", OTPM1);    /*0x2000*/
    SENSOR_PROF_LOG_PRINT("   OTPM 0x30F0:%x", OTPM2);    /*0x1208*/
    SENSOR_PROF_LOG_PRINT("   OTPM 0x3072:%x", OTPM3);    /*0x0007*/

    sensorMT_BatchWrite(cmd_init_1[0]); 
    
    if(OTPM2 == 0x1200){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V1");
        sensorMT_BatchWrite(cmd_init_2_V1[0]);  
        sensorMT_BatchWrite(cmd_init_sequencerA[0]);  
    }else if(OTPM3 == 0x0000){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V2");
        sensorMT_BatchWrite(cmd_init_2_V2[0]);  
        sensorMT_BatchWrite(cmd_init_sequencerB[0]);  
    }else if(OTPM3 == 0x0006){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V3");
        sensorMT_BatchWrite(cmd_init_2_V3[0]);  
        sensorMT_BatchWrite(cmd_init_sequencerP1[0]);  
    }else if(OTPM3 == 0x0007){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V4");
        sensorMT_BatchWrite(cmd_init_2_V4[0]);  
    }else if(OTPM3 == 0x0008){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V5");
        sensorMT_BatchWrite(cmd_init_2_V5[0]);
    }else{
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V???");
    }
    
    #if 0
        /*
        issue 4: veritcal bar in dark regison.
        --> use "AR0330CS_iCatch_Spreadsheet.xls" to decide OTPM revsion"
             need to increase LineLengthPclk and must don't set "seq_short".
        */
        sensorMT_BatchWrite(cmd_init_seq_short[0]); 
    #endif    
    sensorMT_BatchWrite(cmd_init_pll[0]);
    sensorMT_BatchWrite(cmd_vga_init[0]);
    tmrUsWait(1000);
    sensorMT_BatchWrite(cmd_stream_on[0]); 
    
    for(cnt=0; cnt<10; cnt++){
        if(sensorMT_Read(0x301A) == 0x58){
            SENSOR_PROF_LOG_PRINT("cnt: %d", cnt);
            SENSOR_PROF_LOG_PRINT("Stream s: %x",sensorMT_Read(0x301A)); 
            tmrUsWait(3000); 
            sensorMT_BatchWrite(cmd_stream_on[0]); 
            SENSOR_PROF_LOG_PRINT("Stream e: %x",sensorMT_Read(0x301A)); 
        }else{
            break;
        }
    }
}

#if HAS_GSEN4EIS
static void
sensorEisTrig(
    void
)
{      
    EIStrigCB();
}
#endif

static void 
sensorReportsize()
{
    SENSOR_PROF_LOG_PRINT("----- sensorReportsize -----");
	SENSOR_PROF_LOG_PRINT("mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	SENSOR_PROF_LOG_PRINT("front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	SENSOR_PROF_LOG_PRINT("imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	SENSOR_PROF_LOG_PRINT("rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	WRITE16(0xb0009064,1);
	SENSOR_PROF_LOG_PRINT("line count            = (%d)",READ16(0xb0009064));
    SENSOR_PROF_LOG_PRINT("----------------------------");
}

static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
	SENSOR_PROF_LOG_PRINT(__FUNCTION__);
	SENSOR_PROF_LOG_PRINT("________Sensor driver ver <<%s>>", MODEL_VERSION);
	SENSOR_PROF_LOG_PRINT("________Sensor driver build time <<%s>>", BUILD_TIME);

	/* TGGPIO configuration here */
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

    /*TG PLL=MCLK out=24MHz*/
	sensorTgPllClkExtSet(24000000);

	/* Master clock and Pixel clock configuration here */
#if (DDR_CLOCK_297MHz)
    #if PCLK1X_102MHZ
			halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,102000); 
	#else
			halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,288000); 
	#endif
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); 
#else
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121000); 
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,0); 
#endif


	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	
	/*****TODO*****front reset after these???*/
#if (SENSOR_RESET==1) /*TGGPIO 0 3.3V , use enable and external pull high to control high low*/
	sp5kGpioPullSet(SP5K_GPIO_GRP_TGL,SENSOR_RESET, 0);/*pull disable*/
	tmrUsWait(35000); /*35ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); 
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /*external pull high => output low*/
	tmrUsWait(5000); /*5ms*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); /*input => let external pull high*/
#else/*TGGPIO4 1.8V*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(35000); /*35ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(5000); /*5ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
#endif

	cmdportid = sensorTgCfg.cmdportid_xtg;
	tmrUsWait(10000); /* at least 150000xMCLK = 6.25ms on AR0330CS-DS p.12 */

	/*I2C Init code Here */
	sensorMT_SRST();

	sensorLVDSCfgSet(&sensorMIPICfg);
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif

#if STREAMOF_ENABLE
	sensorStsCfg(SENSOR_SYS_CFG_FLOW_USE_ABORT, STREAMOF_ENABLE);
#endif

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);


	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

    sensorResumeInstall(sensorResumeExecuteLoad);
	sensorSuspendInstall(sensorSuspendExecuteLoad);
	SENSOR_PROF_LOG_PRINT("sensorDoFuncInit done");
}

static void
sensorDoFuncStop()
{
}

static void
sensorDoFuncPreviewVGABegin()
{
	SENSOR_PROF_LOG_PRINT("sensorDoFuncPreviewVGABegin");
	sensorEventVvalidWait(1);
	g_IsSnap2Pv=0;
    fpvsusp=1;
    PROF_LOG_LINE_COUNT("cmd_stream_off  s");
    sensorMT_BatchWrite(cmd_stream_off[0]);    
    PROF_LOG_LINE_COUNT("cmd_stream_off  e");
	sensorMT_BatchWrite(cmd_vga_init[0]); 
    SENSOR_PROF_LOG_PRINT("cmd_vga_init");
    
    if(OTPM2 == 0x1200){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V1");
        sensorMT_BatchWrite(cmd_init_sequencerA[0]);  
    }else if(OTPM3 == 0x0000){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V2");
        sensorMT_BatchWrite(cmd_init_sequencerB[0]);  
    }else if(OTPM3 == 0x0006){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V3");
        sensorMT_BatchWrite(cmd_init_sequencerP1[0]);  
    }else if(OTPM3 == 0x0007){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V4");
    }else if(OTPM3 == 0x0008){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V5");
    }else{
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V???");
    }
#if 0
	sensorMT_BatchWrite(cmd_init_seq_short[0]);
    SENSOR_PROF_LOG_PRINT("cmd_init_seq_short");
#endif    
	sensorMT_BatchWrite(cmd_stream_on[0]); 
    SENSOR_PROF_LOG_PRINT("cmd_stream_on");
#if PCLK1X_102MHZ
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,102000); 
#else
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,288000); 
#endif	
    sensorSyncSigInit(&sensorSyncSigPreviewVGACfg);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    SP5K_SET_LVDS_HCROP(PREVIEW_VGA_HCROP_OFFSET);
    SENSOR_PROF_LOG_PRINT("MIPI VGA_60fps done");
}

static void
sensorDoFuncPreview1080pBegin()
{
    SENSOR_PROF_LOG_PRINT("sensorDoFuncPreview1080pBegin");
	sensorEventVvalidWait(1);
	g_IsSnap2Pv=0;
    fpvsusp=1;
    PROF_LOG_LINE_COUNT("cmd_stream_off  s"); 
    sensorMT_BatchWrite(cmd_stream_off[0]);    
    PROF_LOG_LINE_COUNT("cmd_stream_off  e");    
	sensorMT_BatchWrite(cmd_1080p_init[0]); 
    SENSOR_PROF_LOG_PRINT("cmd_1080p_init");
    
    if(OTPM2 == 0x1200){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V1");
        sensorMT_BatchWrite(cmd_init_sequencerA[0]);  
    }else if(OTPM3 == 0x0000){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V2");
        sensorMT_BatchWrite(cmd_init_sequencerB[0]);  
    }else if(OTPM3 == 0x0006){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V3");
        sensorMT_BatchWrite(cmd_init_sequencerP1[0]);  
    }else if(OTPM3 == 0x0007){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V4");
    }else if(OTPM3 == 0x0008){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V5");
    }else{
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V???");
    }

#if 0
	sensorMT_BatchWrite(cmd_init_seq_short[0]);
    SENSOR_PROF_LOG_PRINT("cmd_init_seq_short");
#endif
    sensorMT_BatchWrite(cmd_stream_on[0]); 
    SENSOR_PROF_LOG_PRINT("cmd_stream_on");

#if PCLK1X_102MHZ
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,102000); 
#else
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,288000); 
#endif	   
    sensorSyncSigInit(&sensorSyncSigPreview1080pCfg);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    SP5K_SET_LVDS_HCROP(PREVIEW_1080P_HCROP_OFFSET);
    SENSOR_PROF_LOG_PRINT("MIPI 1080p_30fps done");
}

static void
sensorDoFuncPreview720pBegin()
{
    SENSOR_PROF_LOG_PRINT("sensorDoFuncPreview720pBegin");
 	sensorEventVvalidWait(1);
	g_IsSnap2Pv=0;
    fpvsusp=1;
    PROF_LOG_LINE_COUNT("cmd_stream_off  s");     
    sensorMT_BatchWrite(cmd_stream_off[0]);    
    PROF_LOG_LINE_COUNT("cmd_stream_off  e"); 
	sensorMT_BatchWrite(cmd_720p_init[0]); 
    SENSOR_PROF_LOG_PRINT("cmd_720p_init");
    
    if(OTPM2 == 0x1200){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V1");
        sensorMT_BatchWrite(cmd_init_sequencerA[0]);  
    }else if(OTPM3 == 0x0000){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V2");
        sensorMT_BatchWrite(cmd_init_sequencerB[0]);  
    }else if(OTPM3 == 0x0006){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V3");
        sensorMT_BatchWrite(cmd_init_sequencerP1[0]);  
    }else if(OTPM3 == 0x0007){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V4");
    }else if(OTPM3 == 0x0008){
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V5");
    }else{
        SENSOR_PROF_LOG_PRINT("OTPM revsion-V???");
    }
    
#if 0
	sensorMT_BatchWrite(cmd_init_seq_short[0]);
    SENSOR_PROF_LOG_PRINT("cmd_init_seq_short");
#endif
    sensorMT_BatchWrite(cmd_stream_on[0]); 
    SENSOR_PROF_LOG_PRINT("cmd_stream_on");
    
#if PCLK1X_102MHZ
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,102000); 
#else
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,288000); 
#endif	  
    sensorSyncSigInit(&sensorSyncSigPreview720pCfg);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    SP5K_SET_LVDS_HCROP(PREVIEW_720P_HCROP_OFFSET);
    SENSOR_PROF_LOG_PRINT("MIPI 720p_30fps done");
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

static void
sensorDoFuncSnapExpBegin()
{
	SENSOR_PROF_LOG_PRINT("sensorDoFuncSnapExpBegin");
    TIME_LOG(0);
    if(!g_IsSnap2Pv){
		g_IsSnap2Pv=1;
        sensorMT_BatchWrite(cmd_1080p_init[0]);       
#if 0        
        sensorMT_BatchWrite(cmd_stream_on[0]);  
#endif    
#if PCLK1X_102MHZ
        halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,102000); 
#else
        halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,288000); 
#endif	    
        sensorSyncSigInit(&sensorSyncSigSnapCfg);
        sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
        SP5K_SET_LVDS_HCROP(CAPTURE_HCROP_OFFSET);
    }    
}

static void
sensorDoFuncSnapShortExp()
{
}
static void
sensorDoFuncSnapExpEnd()
{
    SENSOR_PROF_LOG_PRINT("sensorDoFuncSnapExpEnd");
    TIME_LOG(1);
}

static void
sensorDoFuncSnapReadoutField0()
{
    SENSOR_PROF_LOG_PRINT("sensorDoFuncSnapReadoutField0");
    TIME_LOG(2);
}

static void
sensorDoFuncSnapFieldEnd()
{
    SENSOR_PROF_LOG_PRINT("sensorDoFuncSnapFieldEnd");
    TIME_LOG(3);
}


static void
sensorDoFuncSnapReadoutEnd()
{
    SENSOR_PROF_LOG_PRINT("sensorDoFuncSnapReadoutEnd-------");
    TIME_LOG(4);
    SENSOR_PROF_LOG_PRINT("SnapExpS--->ExpEnd:    %d.%d ms",TIME_DIFF(1,0)/1000,TIME_DIFF(1,0)%1000);
    SENSOR_PROF_LOG_PRINT("ExpEnd----->Field0:    %d.%d ms",TIME_DIFF(2,1)/1000,TIME_DIFF(2,1)%1000);
	SENSOR_PROF_LOG_PRINT("Field0----->FieldEnd:  %d.%d ms",TIME_DIFF(3,2)/1000,TIME_DIFF(3,2)%1000);
	SENSOR_PROF_LOG_PRINT("FieEnd----->ReadoutEnd:%d.%d ms",TIME_DIFF(4,3)/1000,TIME_DIFF(4,3)%1000);
    sensorReportsize();
}

static void
sensorDoFuncSnapNext(UINT32 next_mode)
{
    if((next_mode&0xf0)==SENSOR_MODE_SNAP){
        SENSOR_PROF_LOG_ADD("@@@@ get sensor next mode is snap");
    }else{
        SENSOR_PROF_LOG_ADD("@@@@ get sensor next mode is preview");
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
sensor_cmdport_exp_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{
	if((sensorModeGet()&0xF0)==0x40){ /*don't set exp time& dummy in snap mode*/                                                                    
        sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);     
        SENSOR_PROF_LOG_PRINT("snap [nreg=%d][%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]); 
	}else{                                                                                                                                      
		/*SENSOR_PROF_LOG_PRINT("prev [nreg=%d][%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);*/
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
	}
}

static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
}

#if STREAMOF_ENABLE	
static void
sensorDoFuncStreamOff(UINT32 abort)
{
	if(is_aborted==0){
        SENSOR_PROF_LOG_PRINT(sensorModeGet= %x", sensorModeGet());
		SENSOR_PROF_LOG_ADD("stream off s");
		sensorMT_BatchWrite(cmd_stream_off[0]);  
		SENSOR_PROF_LOG_ADD("stream off e");
		is_aborted = 1;
	}
}

static void
sensorDoFuncStreamOn(UINT32 abort)
{
	if(is_aborted==1 ){
	    SENSOR_PROF_LOG_ADD("stream on s");
		sensorMT_BatchWrite(cmd_stream_on[0]);  
		SENSOR_PROF_LOG_ADD("stream on e");
		is_aborted = 0;
	}
}
#endif

static UINT32 sensorResumeExecuteLoad()
{
	#if SUSPEND_ENABLE
		if(sensorSuspended==1){                                                                                                     
            /*sensorResumeExecute()*/
	    	SENSOR_PROF_LOG_ADD("senResu s");/*Edward 20111126*/
            sensorMT_BatchWrite(cmd_stream_on[0]);
 	        tmrUsWait(5000);/*5ms*/ 
 	    	SENSOR_PROF_LOG_ADD("senResu e");/*Edward 20111126*/
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
	    	SENSOR_PROF_LOG_ADD("senSusp s");/*Edward 20111126*/        
            sensorMT_BatchWrite(cmd_stream_off[0]);  	    	                                                             
            tmrUsWait(5000);/*5ms*/                
	    	SENSOR_PROF_LOG_ADD("senSusp e");/*Edward 20111126*/       
	    	sensorSuspended = 1;                                            
	    }
	#endif
	return SUCCESS;	
}

#if SP5K_SENSOR_AR0330CS
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regdata[][4] SENSOR_SIF1DMA_VARBUF = {
		MI_REG(0x3012,0x0000),
	};
	*(UINT16 *)&regdata[0][2] = ((expLine>>8)&0xff) | ((expLine&0xff)<<8);
	printf("<AR0330CS> expLine=%d",expLine);
	sensorSerialDirectBufSnoop(regdata[0], sizeof(regdata));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regdata[0]);
}
#endif

#if SP5K_SENSOR_AR0330CS
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
	printf("sensorMT_DumpReg \n");
	for(i=0; i<sizeof(reg_to_dump_init)/sizeof(UINT16); i++) {
		UINT16 reg_addr = reg_to_dump_init[i];
		printf("REG= 0x%04X, 0x%04X \n", reg_addr, sensorMT_Read(reg_addr));
	}
}
#endif

