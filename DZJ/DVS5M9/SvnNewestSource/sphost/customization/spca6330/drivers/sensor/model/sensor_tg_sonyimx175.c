/*************************************************************************
 *                                                                        
 *       Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.          
 *                                                                        
 *  This software is copyrighted by and is the property of Sunplus        
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    
 *  Co., Ltd. This software may only be used in accordance with the       
 *  corresponding license agreement. Any unauthorized use, duplication,   
 *  distribution, or disclosure of this software is expressly forbidden.  
 *                                                                        
 *  This Copyright notice MUST not be removed or modified without prior   
 *  written consent of Sunplus Technology Co., Ltd.                       
 *                                                                        
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        
 *  software without notice.                                              
 *                                                                        
 *  Sunplus Technology Co., Ltd.                                          
 *  19, Innovation First Road, Science-Based Industrial Park,             
 *  Hsin-Chu, Taiwan                                                      
 *                                                                        
 *  Author: Ryan Kuo                                                  
 *                                                                        
 *  Version: 0.01, 2011.9.22:
 *  Version: 0.02, 2012.3.9: sam wang
 *  for 6330 RDK EVB  
 *  1080P 30FPS is not avaliable (Bandwidth issue)
 *  0x30 :VGA 30FPS
 *  0x31 :QVGA 120FPS (initial code only. use slow clock under check)
 *  0x32 :720P 30FPS 
 *  0x33 :720P 60FPS 
 *  0x40 :8M mode (15fps)
 *  
 *  0.00.10, 1.00.11
 *      Preview mode                                          Sensor mode  Data Rate    FPS(before)    FPS(now)
 *           1632 x 1224 @ 30fps  bin2-VGA30FPS                  0x30      (320Mbps)       29.86        30.0068
 *            816 x  612 @120fps  bin4-QVGA120FPS                0x31      (660Mbps)      119.91       120.097 
 *           1626 x  912 @ 30fps  bin2-720@60fps+dummy=30fps     0x32      (500Mbps)       29.994       30.0069 
 *           1626 x  912 @ 60fps  bin2-720@60fps                 0x33      (500Mbps)       59.98        60.0448 
 *           2112 x 1188 @ 30fps  crop full size, FOV is bad     0x34      (500Mbps)       29.99        30.0068 
 *            816 x  612 @ 30fps  bin4-QVGA120FPS+dummy=30fps    0x35      (660Mbps)       29.97        30.0009 
 *           1632 x 1224 @ 60fps  bin2-VGA60FPS                  0x36      (640Mbps)       59.72        60.0149 
 *           1632 x 1224 @ 30fps  VGA@60fps+dummy=30fps          0x37      (640Mbps)       29.86        30.0074 
 *		                                                                    
 *      Snap mode                                                           
 *           3264 x 2448 @ 15fps                                 0x40      (640Mbps) 
 *           1632 x 1224 @ 30fps  bin2-VGA30FPS                  0x41      (320Mbps)       29.86        30.0068
 * 
 * 1.00.11  
 *      1. fine tune every frame rate of sensor mode.
 *      2. extension line: 6->3, bcs 0x36-VGA@60fps V-Blanking is very short.
 *      3. sensor mode switch to QVGA(0x31, 0x35) must flush uCode in Vvalid End
 *         bcs data rate is maximum and truncate the stream cause cause disaster.
 *      4. Exposure table include Frame_length
 *         exposure time over 1 VD, Frame_length = exp_line + 4
 *
 * 2.00.12 20130617
 *      1. add Snap2 (1632 x 1224) for KX1371.
 *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_sony-imx175-2.00.12"
#define BUILD_TIME        __DATE__  "-"  __TIME__

#include "common.h"
#include "../flow/sysregs.h"
#include "../flow/hal_front.h"
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_dbg_api.h"
#include <stdio.h>
#include "middleware/proflog.h"
#include "../flow/sensor_model_snap.h"
#include "../include/customization/solution_def.h"

#define MASK_EXP_CMD                    0
#define SUSPEND_ENABLE                 (1)
#define BURST_CAP_EN                   (1)
#define SENSOR_IRQ_SVC_FRAME_END       (1)
#define SENSOR_IRQ_SVC_NEG_VD          (10)
#define SENSOR_IRQ_SVC_ORG_VD          (12)
#define SENSOR_EVENT_NEW_FRAME		   (1UL<<2)
#define SENSOR_EVENT_VVALID_END        (1UL<<13)
#define SENSOR_EVENT_CDSP_EOF          (1UL<<16)

#define SP5K_SET_LVDS_HCROP(a)    sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,a,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF)
#define MODE_SYNC_EARLY                (50)
#define SENSOR_PROF_LOG_EN             (1)

#if SENSOR_PROF_LOG_EN
    #define PROF_ID                                 (3)
    #define SENSOR_PROF_LOG_ADD(str)                profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)      profLogPrintf(PROF_ID, fmt, ##arg)
    
    #define WRITELINE (*((volatile UINT16 *)(0xb0009064))  = (1))
    #define READLINE  (*((volatile UINT16 *)(0xb0009064)))
    
    #define PROF_LOG_LINE_COUNT(str,args...)\
    do{\
        WRITELINE;\
        profLogPrintf(0,"%s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(1,"%s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(2,"%s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(3,"%s_"str" @ %d",__func__, ##args, READLINE);\
    }while(0)
#else
    #define SENSOR_PROF_LOG_ADD(str)
    #define SENSOR_PROF_LOG_PRINT(fmt,arg...)
    #define PROF_LOG_LINE_COUNT(str,args...) 
#endif

/* Sensor image size definition here */
#define NO             0
#define BY_PIXEL       1
#define BY_HD          0
#define NOUSE          0
#define DISABLE        0xFFFF

#if defined(HW_EVB_SPE7001)
/*5330 settings*/
/*
	#define SENSOR_FREX     (1<<15)          
	#define SENSOR_PWRDN    (1<<14)
	#define SENSOR_RESET    (1<<12)
	#define OUT_SENSOR_GPIO_MASK SENSOR_FREX| SENSOR_PWRDN| SENSOR_RESET
	#define CFG_SENSOR_GPIO_DIR SENSOR_FREX | SENSOR_PWRDN| SENSOR_RESET
	#define SENOR_GPIO_GRP SP5K_GPIO_GRP_LMI
*/
/*6330 settings*/
#define SENSOR_PWRDN	(1<<0)
#define SENSOR_RESET	(1<<23)

#define SENSOR_PWRDN_GPIO_MASK SENSOR_PWRDN
#define SENSOR_PWRDN_GPIO_CFG  SENSOR_PWRDN
#define SENSOR_PWRDN_GRP       SP5K_GPIO_GRP_TGL
	
#define SENSOR_RESET_GPIO_MASK SENSOR_RESET
#define SENSOR_RESET_GPIO_CFG  SENSOR_RESET
#define SENSOR_RESET_GRP       SP5K_GPIO_GRP_FML
#endif

#if(CPU_CLK==6)/*CPU_CLK = 300MHz PCLK 1X = 300MHz*/
#define MCLK2MCLK(a)     a
#elif (CPU_CLK==8)/*CPU_CLK = 360MHz MCLK = 288MHz*/
#define MCLK2MCLK(a)     (a*288/300) /*300MHz/288MHz for reshape timing*/
#else
#warning not support lower clock
#define MCLK2MCLK(a)     (a)
#endif

#define PREVIEW_EXTEND_LINES                        (3)
	                                                           
/*horiz. and vertical size*/                                    
	#define PREVIEW_VGA_HSIZE                		(1632)      /* 3264/2 = 1632  */
	#define PREVIEW_VGA_VSIZE                		(1224)	    /* 2448/2 = 1224  */
	#define PREVIEW_VGA_VOFFSET 					(PREVIEW_EXTEND_LINES+4)	
	#define PREVIEW_VGA_HCROP_OFFSET		 		(0) 	    /* lvds crop offset (4N BUG!) */

    #define PREVIEW_QVGA_HSIZE						(816)       /* 3264/4 = 816 */ 
    #define PREVIEW_QVGA_VSIZE						(612)       /* 2448/4 = 612 */
    #define PREVIEW_QVGA_VOFFSET 					(PREVIEW_EXTEND_LINES+4) 
	#define PREVIEW_QVGA_HCROP_OFFSET				(0) 	    /* lvds crop offset (4N BUG!) */
 
 	#define PREVIEW_720P_HSIZE               		(1626)		                   
	#define PREVIEW_720P_VSIZE               		(912)    
	#define PREVIEW_720P_VOFFSET					(PREVIEW_EXTEND_LINES+4)
	#define PREVIEW_720P_HCROP_OFFSET		 		(0) 	    /* lvds crop offset (4N BUG!) */
	/* For preview size, 3280*1848, 30fps */
	/*200Mpixel/s is too fast for 6330*/
#if 1/*crop to 2112 to avoid hscale*/
	#define PREVIEW_1080P_HSIZE 					(2112)      /*(3280)	*/	
	#define PREVIEW_1080P_VSIZE 					(1188)      /*(1824)	*/	
	#define PREVIEW_1080P_VOFFSET					(PREVIEW_EXTEND_LINES+4+318) 
	#define PREVIEW_1080P_HCROP_OFFSET				(584-1) 	/* lvds crop offset (4N BUG!) */
#else/*original output */
	#define PREVIEW_1080P_HSIZE              		(3280)		
	#define PREVIEW_1080P_VSIZE  	         		(1824)	 	
	#define PREVIEW_1080P_VOFFSET					(PREVIEW_EXTEND_LINES+4)	 
	#define PREVIEW_1080P_HCROP_OFFSET		 		(0) 	    /* lvds crop offset (4N BUG!) */
#endif
    /* For Capture 3280x2464 */
	#define CAPTURE_HSIZE                    		(3264)					
	#define CAPTURE_VSIZE                    		(2448)					
	#define CAPTURE_VOFFSET 						(PREVIEW_EXTEND_LINES+4) 	 
	#define CAPTURE_HCROP_OFFSET		 			(0)	        /* lvds crop offset (4N BUG!) */
    /* For Capture2 1632x1224 */
	#define CAPTURE2_HSIZE                    		(PREVIEW_VGA_HSIZE)					
	#define CAPTURE2_VSIZE                    		(PREVIEW_VGA_VSIZE)					
	#define CAPTURE2_VOFFSET 						(PREVIEW_VGA_VOFFSET) 	 
	#define CAPTURE2_HCROP_OFFSET		 			(PREVIEW_VGA_HCROP_OFFSET)	
/*****************************************************************************************************/
	#define PREVIEW_VGA_LINETOTAL					MCLK2MCLK(8000)	                            
	#define PREVIEW_VGA_LINEBLANK                   (6)		
	#define PREVIEW_VGA_FRAMETOTAL                	(1231) 
	#define PREVIEW_VGA_FRAMEBLANK                	(1231)	
	#define PREVIEW_VGA_HOFFSET                   	(6000)  
    /*640Mbps*/                                                                                       
	#define PREVIEW_VGA_60FPS_HRESHAPE_FALL         MCLK2MCLK(2000) 	   
	#define PREVIEW_VGA_60FPS_HRESHAPE_RISE         MCLK2MCLK(2200)		   
    /*320Mbps*/                                                                        
	#define PREVIEW_VGA_30FPS_HRESHAPE_FALL			MCLK2MCLK(4200)		
	#define PREVIEW_VGA_30FPS_HRESHAPE_RISE			MCLK2MCLK(4500)		
	#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL		BY_HD		           
	#define PREVIEW_VGA_VRESHAPE_FALL             	(PREVIEW_VGA_VOFFSET-2) 	 
	#define PREVIEW_VGA_VRESHAPE_RISE             	(PREVIEW_VGA_VOFFSET-1)      
    /**********************************************************************/
    #define PREVIEW_QVGA_LINETOTAL					0x1400	
    #define PREVIEW_QVGA_LINEBLANK					(6) 	
    #define PREVIEW_QVGA_FRAMETOTAL					(617)	
    #define PREVIEW_QVGA_FRAMEBLANK					(617)	
    #define PREVIEW_QVGA_HOFFSET 					0x0f00
	#define PREVIEW_QVGA_HRESHAPE_FALL				0x0b10
	#define PREVIEW_QVGA_HRESHAPE_RISE				0x0b20/*MCLK2MCLK(3000)*/	  
	#define PREVIEW_QVGA_VRESHAPE_COUNTER_SEL		BY_HD	
	#define PREVIEW_QVGA_VRESHAPE_FALL				(PREVIEW_QVGA_VOFFSET-2) 		  
	#define PREVIEW_QVGA_VRESHAPE_RISE				(PREVIEW_QVGA_VOFFSET-1) 	
    /**********************************************************************/
    /* For preview size 1640*924,    60fps */
    #define PREVIEW_720P_LINETOTAL                  (3448)
	#define PREVIEW_720P_LINEBLANK     			    (6)
	#define PREVIEW_720P_FRAMETOTAL                 (925)
	#define PREVIEW_720P_FRAMEBLANK    		      	(925)
	#define PREVIEW_720P_HOFFSET                    (1600) 
	#define PREVIEW_720P_HRESHAPE_FALL              MCLK2MCLK(1500)
	#define PREVIEW_720P_HRESHAPE_RISE             	MCLK2MCLK(1600) 
	#define PREVIEW_720P_VRESHAPE_COUNTER_SEL 		BY_HD
	#define PREVIEW_720P_VRESHAPE_FALL              (PREVIEW_720P_VOFFSET-2)
	#define PREVIEW_720P_VRESHAPE_RISE             	(PREVIEW_720P_VOFFSET-1)
    /* For preview size, 3280*1848, 30fps */
	#define PREVIEW_1080P_LINETOTAL		  	     	(3448)		               	
	#define PREVIEW_1080P_LINEBLANK   		 	    (6)		                    
	#define PREVIEW_1080P_FRAMETOTAL  		 	 	(1841+4)		         
	#define PREVIEW_1080P_FRAMEBLANK  		 	 	(1841+4)		             
	#define PREVIEW_1080P_HOFFSET     		       	160  	                 
	#define PREVIEW_1080P_HRESHAPE_FALL             MCLK2MCLK(150) 	         
	#define PREVIEW_1080P_HRESHAPE_RISE            	MCLK2MCLK(250)	         
	#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL  	BY_HD	                 
	#define PREVIEW_1080P_VRESHAPE_FALL             (PREVIEW_1080P_VOFFSET-2)
	#define PREVIEW_1080P_VRESHAPE_RISE            	(PREVIEW_1080P_VOFFSET-1)
    /* Capture default definition here */
	#define CAPTURE_LINETOTAL                    	(3440)
	#define CAPTURE_LINEBLANK						(6)		
	#define CAPTURE_FRAMETOTAL  				 	(2465)
	#define CAPTURE_FRAMEBLANK  				 	(2465)
	#define CAPTURE_HOFFSET                       	(160) 
	#define CAPTURE_HRESHAPE_FALL                   MCLK2MCLK(3800)		
	#define CAPTURE_HRESHAPE_RISE                  	MCLK2MCLK(4000)		
	#define CAPTURE_VRESHAPE_COUNTER_SEL      		BY_HD  	            
	#define CAPTURE_VRESHAPE_FALL		            (CAPTURE_VOFFSET-2) 
	#define CAPTURE_VRESHAPE_RISE		           	(CAPTURE_VOFFSET-1) 
    /* Capture default definition here */
    #define CAPTURE2_LINETOTAL                    	(PREVIEW_VGA_LINETOTAL)
	#define CAPTURE2_LINEBLANK						(PREVIEW_VGA_LINEBLANK)		
	#define CAPTURE2_FRAMETOTAL  				 	(PREVIEW_VGA_FRAMETOTAL)
	#define CAPTURE2_FRAMEBLANK  				 	(PREVIEW_VGA_FRAMEBLANK)
	#define CAPTURE2_HOFFSET                       	(PREVIEW_VGA_HOFFSET) 
	#define CAPTURE2_HRESHAPE_FALL                  (PREVIEW_VGA_30FPS_HRESHAPE_FALL)		
	#define CAPTURE2_HRESHAPE_RISE                 	(PREVIEW_VGA_30FPS_HRESHAPE_RISE)
	#define CAPTURE2_VRESHAPE_COUNTER_SEL      		(PREVIEW_VGA_VRESHAPE_COUNTER_SEL)
	#define CAPTURE2_VRESHAPE_FALL		            (PREVIEW_VGA_VRESHAPE_FALL) 
	#define CAPTURE2_VRESHAPE_RISE		           	(PREVIEW_VGA_VRESHAPE_RISE)
/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV            1         /* mclk drivng, '1' : 8mA, '0' : 4mA */
#define MCLKMODE		   1	     /* mclk mode,'0'from I/O Pad,'1'from internal*/
#define PCLKMODE		   2	     /* pclk mode,'0'from I/O Pad,'1'from internal*/
#define TGPLLEN   		   1	     /* enable TGPLL,'0'disable,'1'enable*/
#define TGPLLCLKSET		   8         /* TGPLL clk  0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,    5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/
           
#define MCLKDIV			   4	     /* MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV			   2	     /* pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE		   0     	 /* master clk phase*/
#define PIXELPHASE		   3	     /* pixel clk phase*/
#define ADCKPHASE		   0	     /* adck clk phase*/

#define	PCLKNXSEL_INT	   0x00
#define	PCLKNXSEL_LVDS	   0x01
#define	PCLKNXSEL_EXT	   0x02
#define	PCLK1XSEL_NX	   0x00
#define	PCLK1XSEL_NX_2	   0x04
#define	PCLK1XSEL_INT	   0x08
/**************sensorCfgInit table define ****************************/
#define SENSORMODEL						  TYPE_CMOS		/* TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                                                        TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			         TG_CMOS_SP5K_SLAVE		/* TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
														     TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_VGA_CFAPATTERN	         BAYER_RGGB     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	PREVIEW_QVGA_CFAPATTERN	         BAYER_RGGB     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_720P_CFAPATTERN          BAYER_RGGB
#define PREVIEW_1080P_CFAPATTERN         BAYER_RGGB

#define	SNAP_CFAPATTERN					 BAYER_GBRG     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP2_CFAPATTERN				 BAYER_RGGB     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM						  0     /* snap dummy Vd need numbers*/            
#define	SNAP_READOUTVD_NUM		        		  1     /* snap readout Vd need numbers*/             
#define	SNAP_CAPTUREVD_NUM		        		  1     /* for typical CCD==n_dummy + n_readout + 1*/ 
#define	SNAP_EARLY_NUM		                      0	    /* for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	                      0		/* It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM							  0	 	/* It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN                    0		/* a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN              2	    /* a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	                      0		/* sony FALSE;panasonic/ITG TRUE*/
#define	EXP_FRAME_NUM       	                  2

#define	PREVIEW_HDNUM_FRAME_START               (20)		
#define	PREVIEW_VGA_HDNUM_FRAME_END	            (PREVIEW_VGA_VOFFSET+PREVIEW_VGA_VSIZE)	
#define	PREVIEW_VGA_HDNUM_CMDPORT		        (400)	  	
#define	PREVIEW_QVGA_HDNUM_FRAME_END	        (PREVIEW_QVGA_VOFFSET+PREVIEW_QVGA_VSIZE)	
#define	PREVIEW_QVGA_HDNUM_CMDPORT		        (400)		
#define	PREVIEW_720P_HDNUM_FRAME_END	        (PREVIEW_720P_VOFFSET+PREVIEW_720P_VSIZE)	
#define	PREVIEW_720P_HDNUM_CMDPORT		        (400)		
#define	PREVIEW_1080P_HDNUM_FRAME_END           (PREVIEW_1080P_VOFFSET+PREVIEW_1080P_VSIZE)
#define	PREVIEW_1080P_HDNUM_CMDPORT	            (400)	

#define	CAPTURE_HDNUM_FRAME_START               (20)	   
#define	CAPTURE_HDNUM_FRAME_END	                (CAPTURE_VOFFSET+CAPTURE_VSIZE)  
#define	CAPTURE_HDNUM_CMDPORT	                (2000)  
#define	CAPTURE2_HDNUM_FRAME_END	            (PREVIEW_VGA_HDNUM_FRAME_END)  
#define	CAPTURE2_HDNUM_CMDPORT	                (PREVIEW_VGA_HDNUM_CMDPORT)  
/**************create the command port condition  define ****************************/
/* UINT8 port_id is dynamic allocated by sensor_serial.c::sensorSerialCreate()      
 *  stored in sensor{Tg/Afe}Cfg.cmdportid_{xtg,exp,afe,agc,dcoffset}                 
 *                                                                                   
 * structure of physical definitions                                                  	
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
    #define	TG_SPI_I2C_MODE     0x20                /* I2C slave addr for write */
    #define	TG_FORMAT2          FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF
    #define	TG_FORMAT3            0
    #define	TG_SPEED            FRONT_SSC_CLK_384K  /* FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
                                                       FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
    #define	TG_NBIT_REGADDR     16                  /* ssc/spi register address numbers */
    #define	TG_NBIT_REGDATA     8                   /* ssc/spi register data numbers */

extern UINT32 sensor_event_wait(UINT32 event);
extern UINT32 sensorModeGet();

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreviewQVGABegin();
static void sensorDoFuncPreview720PBegin();
static void sensorDoFuncPreview1080PBegin();
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
static void sensorDoFuncSnapBshutterEnd();
static void sensorDoFuncSnapNext(UINT32 next_mode);
static UINT32 sensorResumeExecuteLoad();
static UINT32 sensorSuspendExecuteLoad();

static UINT32 g_last_exp_line;
static UINT32 g_last_smode;
static UINT32 g_IsSnap2Pv;
static UINT32 sensorSuspended;
static BOOL   fpvsusp;

/******************** uCode ****************************/
static const UINT8 cmd_init_code_SWRstOn[][3]={
	{0x01,0x03, 0x01}
};
static const UINT8 cmd_init_code_SWRstOff[][3]={
	{0x01,0x03, 0x00}
};
static const UINT8 cmd_init_code_StreamOn[][3]={
	{0x01,0x00, 0x01}
};
static const UINT8 cmd_init_code_StreamOff[][3]={
	{0x01,0x00, 0x00}
};

static const UINT8 cmd_vga_30fps_code[][3]={
	/* Format: Addr, Data				 */
										 
	/* PLL settings 					 */
	/* INCK = 24MHz 					 */
	/* MIPI 4lanes 640MHz				 */
	/* line_total 3440 pixel_clock/line  */
	/* frame_total 1246 line/frame		 */
	/* h_size 1640						 */
	/* v_size 1232						 */
	
	{0x01,0x00, 0x00},	         /* Standby 		          */
						                                      
						         /* 24M*320/12 = 640M 60FPS   */
						         /* 24M*160/12 = 320M 30FPS   */
	{0x03,0x01, 0x05},	         /* vt_pix_clk_div	          */
	{0x03,0x03, 0x01},	         /* vt_sys_clk_div	          */
	{0x03,0x05, 0x0C},	         /* pre_pll_clk_di	          */
	{0x03,0x09, 0x05},	         /* OPPXCK_DIV		          */
	{0x03,0x0B, 0x01},	         /* OPSYCK_DIV		          */
                                                              
    /*30FPS*/                                                        
	{0x03,0x0C, 0x00},	         /* PLL_OP_MPY		          */     /*160 = 0x0A0*/ 
	{0x03,0x0D, 0xA0},	         /* PLL_OP_MPY		          */
	{0x03,0x0E, 0x01},	         /* PLL_SOLO_DRIVE	          */
#if 0					         	                          
	{0x02,0x02, 0x09},	         /* shutter 				  */
	{0x02,0x03, 0xAA},	         /* shutter 				  */
#endif
	{0x03,0x40, (1240>>8  )},	 /* frame_length_lines [15:8] */
	{0x03,0x41, (1240&0xFF)},	 /* frame_length_lines [7:0]  */    /*(0x30) 30fps:  1246-->1240*/
	{0x03,0x42,0x0D},            /* line_length_pck [15:8]    */           
	{0x03,0x43,0x70},            /* line_length_pck [7:0]     */           
	{0x03,0x44,0x00},           
	{0x03,0x45,0x00},           
	{0x03,0x46,0x00},           
	{0x03,0x47,0x00},           
	{0x03,0x48,0x0C},           
	{0x03,0x49,0xCF},           
	{0x03,0x4A,0x09},           
	{0x03,0x4B,0x9F},           
	{0x03,0x4C,0x06},           
	{0x03,0x4D,0x60},           
	{0x03,0x4E,0x04},           
	{0x03,0x4F,0xCE},           
	{0x03,0x90,0x01},           
	{0x30,0x20,0x10},           
	{0x30,0x2D,0x02},           
	{0x30,0x2F,0x80},           
	{0x30,0x32,0xA3},           
	{0x30,0x33,0x20},           
	{0x30,0x34,0x24},           
	{0x30,0x41,0x15},           
	{0x30,0x42,0x87},           
	{0x30,0x50,0x35},           
	{0x30,0x56,0x57},           
	{0x30,0x5D,0x41},           
	{0x30,0x97,0x69},           
	{0x31,0x09,0x41},           
	{0x31,0x48,0x3F},           
	{0x33,0x0F,0x07},           
	{0x33,0x44,0x5F},           
	{0x33,0x45,0x1F},           
	{0x33,0x68,0x18},           
	{0x33,0x69,0x00},           
	{0x33,0x70,0x6F},           
	{0x33,0x71,0x27},           
	{0x33,0x72,0x47},           
	{0x33,0x73,0x27},           
	{0x33,0x74,0x27},           
	{0x33,0x75,0x27},           
	{0x33,0x76,0x8F},           
	{0x33,0x77,0x37},           
	{0x33,0xD4,0x06},           
	{0x33,0xD5,0x60},           
	{0x33,0xD6,0x04},           
	{0x33,0xD7,0xCE},           
	{0x41,0x00,0x0E},           
	{0x41,0x04,0x32},           
	{0x41,0x05,0x32},           
	{0x41,0x08,0x01},           
	{0x41,0x09,0x7C},           
	{0x41,0x0A,0x00},           
	{0x41,0x0B,0x00},           
	{0x01,0x00, 0x01}, /* Streaming   */
};

static const UINT8 cmd_vga_60fps_code[][3]={
	/* Format: Addr, Data				 */
										 
	/* PLL settings 					 */
	/* INCK = 24MHz 					 */
	/* MIPI 4lanes 640MHz				 */
	/* line_total 3440 pixel_clock/line  */
	/* frame_total 1246 line/frame		 */
	/* h_size 1640						 */
	/* v_size 1232						 */
	
	{0x01,0x00, 0x00},	         /* Standby 		          */
						          
						         /* 24M*320/12 = 640M 60FPS   */
						         /* 24M*160/12 = 320M 30FPS   */
	{0x03,0x01, 0x05},	         /* vt_pix_clk_div	          */
	{0x03,0x03, 0x01},	         /* vt_sys_clk_div	          */
	{0x03,0x05, 0x0C},	         /* pre_pll_clk_di	          */
	{0x03,0x09, 0x05},	         /* OPPXCK_DIV		          */
	{0x03,0x0B, 0x01},	         /* OPSYCK_DIV		          */
                                                              
    /*60FPS*/                                                   
	{0x03,0x0C, 0x01},	         /* PLL_OP_MPY		          */     /*320 = 0x140*/ 
	{0x03,0x0D, 0x40},	         /* PLL_OP_MPY		          */                     
	{0x03,0x0E, 0x01},	         /* PLL_SOLO_DRIVE	          */
#if 0					         	 
	{0x02,0x02, 0x09},	         /* shutter 				  */
	{0x02,0x03, 0xAA},	         /* shutter 				  */
#endif
	{0x03,0x40, (1240>>8  )},	 /* frame_length_lines [15:8] */    /*(0x36) 60fps: 1246--> 1240*/
	{0x03,0x41, (1240&0xFF)},	 /* frame_length_lines [7:0]  */    /*(0x37) 30fps: 2492--> 2480*/
	{0x03,0x42,0x0D},            /* line_length_pck [15:8]    */           
	{0x03,0x43,0x70},            /* line_length_pck [7:0]     */           
	{0x03,0x44,0x00},           
	{0x03,0x45,0x00},           
	{0x03,0x46,0x00},           
	{0x03,0x47,0x00},           
	{0x03,0x48,0x0C},           
	{0x03,0x49,0xCF},           
	{0x03,0x4A,0x09},           
	{0x03,0x4B,0x9F},           
	{0x03,0x4C,0x06},           
	{0x03,0x4D,0x60},           
	{0x03,0x4E,0x04},           
	{0x03,0x4F,0xCE},           
	{0x03,0x90,0x01},           
	{0x30,0x20,0x10},           
	{0x30,0x2D,0x02},           
	{0x30,0x2F,0x80},           
	{0x30,0x32,0xA3},           
	{0x30,0x33,0x20},           
	{0x30,0x34,0x24},           
	{0x30,0x41,0x15},           
	{0x30,0x42,0x87},           
	{0x30,0x50,0x35},           
	{0x30,0x56,0x57},           
	{0x30,0x5D,0x41},           
	{0x30,0x97,0x69},           
	{0x31,0x09,0x41},           
	{0x31,0x48,0x3F},           
	{0x33,0x0F,0x07},           
	{0x33,0x44,0x5F},           
	{0x33,0x45,0x1F},           
	{0x33,0x68,0x18},           
	{0x33,0x69,0x00},           
	{0x33,0x70,0x6F},           
	{0x33,0x71,0x27},           
	{0x33,0x72,0x47},           
	{0x33,0x73,0x27},           
	{0x33,0x74,0x27},           
	{0x33,0x75,0x27},           
	{0x33,0x76,0x8F},           
	{0x33,0x77,0x37},           
	{0x33,0xD4,0x06},           
	{0x33,0xD5,0x60},           
	{0x33,0xD6,0x04},           
	{0x33,0xD7,0xCE},           
	{0x41,0x00,0x0E},           
	{0x41,0x04,0x32},           
	{0x41,0x05,0x32},           
	{0x41,0x08,0x01},           
	{0x41,0x09,0x7C},           
	{0x41,0x0A,0x00},           
	{0x41,0x0B,0x00},           
	{0x01,0x00, 0x01}, /* Streaming   */
};

static const UINT8 cmd_qvga_code[][3]={
	/*
	 Format: Addr, Data
	 PLL settings
	 INCK = 24MHz
	 MIPI 4lanes 660MHz
	 line_total 3440 pixel_clock/line
	 frame_total 630 line/frame 
	 h_size 820
	 v_size 616 									  

     660M/10bit * 4lane = 264M (pclk1x 288MMhz now-preview mode 9 )
	*/
	{0x01,0x00, 0x00},           /* Standby                   */
	                             /* 24M*330/12 = 660M         */
	{0x03,0x01, 0x05},           /* vt_pix_clk_div            */
	{0x03,0x03, 0x01},           /* vt_sys_clk_div            */
	{0x03,0x05, 0x0C},           /* pre_pll_clk_di            */
	{0x03,0x09, 0x05},           /* OPPXCK_DIV                */
	{0x03,0x0B, 0x01},           /* OPSYCK_DIV                */
#if 1                                                        
	{0x03,0x0C, 0x01},           /* PLL_OP_MPY                */   /*330 = 0x14A   120fps*/   
	{0x03,0x0D, 0x4A},           /* PLL_OP_MPY                */
#else                                                        
    #if 0                                                    
	{0x03,0x0C, 0x00},           /* PLL_OP_MPY                */   /*165 = 0x0A5    60fps*/
	{0x03,0x0D, 0x5A},           /* PLL_OP_MPY                */
    #else                                                    
    {0x03,0x0C, 0x00},           /* PLL_OP_MPY                */   /*82.5 = 0x052   30fps*/
	{0x03,0x0D, 0x52},           /* PLL_OP_MPY                */
    #endif                                                   
#endif                                                       
	{0x03,0x0E, 0x01},           /* PLL_SOLO_DRIVE            */
	                             /* 0x0103, 0x01  SW_Reset    */
#if 0                            
	{0x02,0x02, 0x09},           /* shutter                   */
	{0x02,0x03, 0xAA},           /* shutter                   */
	                             
	{0x03,0x40, 0x09},           /* frame_length_lines [15:8] */
	{0x03,0x41, 0xAE},           /* frame_length_lines [7:0]  */
#endif
	{0x03,0x40, (639>>8  )},	 /* frame_length_lines [15:8] */    /*(0x31)120fps:    640-->639*/
	{0x03,0x41, (639&0xFF)},	 /* frame_length_lines [7:0]  */    /*(0x35) 30fps:   2560-->2558*/
	{0x03,0x42, 0x0D},           /* line_length_pck [15:8]    */
	{0x03,0x43, 0x70},           /* line_length_pck [7:0]     */
	{0x03,0x44, 0x00},  
	{0x03,0x45, 0x00},  
	{0x03,0x46, 0x00},  
	{0x03,0x47, 0x00},  
	{0x03,0x48, 0x0C},  
	{0x03,0x49, 0xCF},  
	{0x03,0x4A, 0x09},  
	{0x03,0x4B, 0x9F},  
	{0x03,0x4C, 0x03},           /* X_OUT_SIZE 0x334= 820      */  
	{0x03,0x4D, 0x34+12},        /* +8 still has 3 dummy pixels*/  
	{0x03,0x4E, 0x02},           /* Y_OUT_SIZE 0x268= 616      */  
	{0x03,0x4F, 0x68},  
	{0x03,0x90, 0x02},  
	{0x30,0x20, 0x10},  
	{0x30,0x2D, 0x03},  
	{0x30,0x2F, 0x80},  
	{0x30,0x32, 0xA3},  
	{0x30,0x33, 0x20},  
	{0x30,0x34, 0x24},  
	{0x30,0x41, 0x15},  
	{0x30,0x42, 0x87},  
	{0x30,0x50, 0x35},  
	{0x30,0x56, 0x57},  
	{0x30,0x5D, 0x41},  
	{0x30,0x97, 0x69},  
	{0x31,0x09, 0x41},  
	{0x31,0x48, 0x3F},  
	{0x33,0x0F, 0x07},  
	{0x33,0x44, 0x6F},  
	{0x33,0x45, 0x1F},  
	{0x33,0x64, 0x00},  
	{0x33,0x68, 0x24},  
	{0x33,0x69, 0x00},  
	{0x33,0x70, 0x6F},  
	{0x33,0x71, 0x27},  
	{0x33,0x72, 0x47},  
	{0x33,0x73, 0x27},  
	{0x33,0x74, 0x27},  
	{0x33,0x75, 0x27},  
	{0x33,0x76, 0x8F},  
	{0x33,0x77, 0x37},  
	{0x33,0xC8, 0x00},  
	{0x33,0xD4, 0x03},  
	{0x33,0xD5, 0x34},  
	{0x33,0xD6, 0x02},  
	{0x33,0xD7, 0x68},  
	{0x41,0x00, 0x0E},  
	{0x41,0x04, 0x32},  
	{0x41,0x05, 0x32},  
	{0x41,0x08, 0x01},  
	{0x41,0x09, 0x7C},  
	{0x41,0x0A, 0x00},  
	{0x41,0x0B, 0x00},
	{0x01,0x00, 0x01}, /* Streaming   */
};

static const UINT8 cmd_720P_code[][3]={
	/* Format: Addr, Data                  */
	/*                                     */
	/* PLL settings                        */
	/* INCK = 24MHz                        */
	/* MIPI 4lanes 500MHz                  */
	/* line_total 3448 pixel_clock/line    */
	/* frame_total 964 line/frame          */
	/* h_size 1640                         */
	/* v_size 924                          */

	{0x01,0x00,0x00 },          /* Standby                      */
                                
	{0x03,0x01,0x05 },          /* vt_pix_clk_div               */
	{0x03,0x03,0x01 },          /* vt_sys_clk_div               */
	{0x03,0x05,0x0C },          /* pre_pll_clk_di               */
	{0x03,0x09,0x05 },          /* OPPXCK_DIV                   */
	{0x03,0x0B,0x01 },          /* OPSYCK_DIV                   */
	{0x03,0x0C,0x00 },          /* PLL_OP_MPY                   */
	{0x03,0x0D,0xFA },          /* PLL_OP_MPY                   */
	{0x03,0x0E,0x01 },          /* PLL_SOLO_DRIVE               */
                                
#if 0                           
	{0x02,0x02,0x03 },          /* shutter                      */
	{0x02,0x03,0xC0 },          /* shutter                      */
#endif
    {0x03,0x40,(966>>8  ) },    /* frame_length_lines [15:8]    */  /*(0x32)30fps: 1934-->1933*/
	{0x03,0x41,(966&0xFF) },    /* frame_length_lines [7:0]     */  /*(0x33)60fps:  967--> 966*/
	{0x03,0x42,0x0D },          /* line_length_pck [15:8]       */
	{0x03,0x43,0x78 },          /* line_length_pck [7:0]        */
	{0x03,0x44,0x00 },
	{0x03,0x45,0x00 },
	{0x03,0x46,0x01 },
	{0x03,0x47,0x34 },
	{0x03,0x48,0x0C },
	{0x03,0x49,0xCF },
	{0x03,0x4A,0x08 },
	{0x03,0x4B,0x6B },
	{0x03,0x4C,0x06 },
#if 0    
	{0x03,0x4D,0x68 },
#else
    {0x03,0x4D,0x60 },          /* bin2 math FOV with VGA mode*/
#endif  
	{0x03,0x4E,0x03 },
	{0x03,0x4F,0x9C },
	{0x03,0x90,0x01 },
	{0x30,0x20,0x10 },
	{0x30,0x2D,0x02 },
	{0x30,0x2F,0x80 },
	{0x30,0x32,0xA3 },
	{0x30,0x33,0x20 },
	{0x30,0x34,0x24 },
	{0x30,0x41,0x15 },
	{0x30,0x42,0x87 },
	{0x30,0x50,0x35 },
	{0x30,0x56,0x57 },
	{0x30,0x5D,0x41 },
	{0x30,0x97,0x69 },
	{0x31,0x09,0x41 },
	{0x31,0x48,0x3F },
	{0x33,0x0F,0x07 },
	{0x33,0x44,0x5F },
	{0x33,0x45,0x1F },
	{0x33,0x68,0x09 },
	{0x33,0x69,0x00 },
	{0x33,0x70,0x6F },
	{0x33,0x71,0x27 },
	{0x33,0x72,0x47 },
	{0x33,0x73,0x27 },
	{0x33,0x74,0x27 },
	{0x33,0x75,0x27 },
	{0x33,0x76,0x8F },
	{0x33,0x77,0x37 },
	{0x33,0xD4,0x06 },
	{0x33,0xD5,0x68 },
	{0x33,0xD6,0x03 },
	{0x33,0xD7,0x9C },
	{0x41,0x00,0x0E },
	{0x41,0x04,0x32 },
	{0x41,0x05,0x32 },
	{0x41,0x08,0x01 },
	{0x41,0x09,0x7C },
	{0x41,0x0A,0x00 },
	{0x41,0x0B,0x00 },

	{0x01,0x00, 0x01},	
};

static const UINT8 cmd_1080P_code[][3]={
/* Format: Addr, Data                                      */
/* PLL settings                                            */
/* INCK = 24MHz                                            */
/* MIPI 4lanes 500MHz                                      */
/* line_total 3448 pixel_clock/line                        */
/* frame_total 1928 line/frame                             */
/* h_size 3280                                             */
/* v_size 1848                                             */
/* 200Mpixl/s                                              */

	{0x01,0x00,0x00},         /* Standby                   */  
	                          /* 24M*250/12 = 500M         */
	{0x03,0x01,0x05},         /* vt_pix_clk_div            */
	{0x03,0x03,0x01},         /* vt_sys_clk_div            */
	{0x03,0x05,0x0C},         /* pre_pll_clk_di            */
	{0x03,0x09,0x05},         /* OPPXCK_DIV                */
	{0x03,0x0B,0x01},         /* OPSYCK_DIV                */
	{0x03,0x0C,0x00},         /* PLL_OP_MPY                */
                              
#if 0 /*250MHz*/              
	{0x03,0x0D,0x7D},         /* PLL_OP_MPY			       */
#else /*500MHz*/              
	{0x03,0x0D,0xFA},         /* PLL_OP_MPY                */
#endif                        
                              
	{0x03,0x0E,0x01},         /* PLL_SOLO_DRIVE            */
	                          /* 0x0103, 0x01 //SW_Reset   */
#if 0	                         
	{0x02,0x02,0x07},         /* shutter                   */
	{0x02,0x03,0x84},         /* shutter                   */
#endif
    {0x03,0x40,(1933>>8  )},  /* frame_length_lines [15:8] */    /*(0x34) 30fps: 1934--> 1933*/
	{0x03,0x41,(1933&0xFF)},  /* frame_length_lines [7:0]  */     
	{0x03,0x42,0x0D},         /* line_length_pck [15:8]    */    
	{0x03,0x43,0x78},         /* line_length_pck [7:0]     */    
	{0x03,0x44,0x00},
	{0x03,0x45,0x00},
	{0x03,0x46,0x01},
	{0x03,0x47,0x34},
	{0x03,0x48,0x0C},
	{0x03,0x49,0xCF},
	{0x03,0x4A,0x08},
	{0x03,0x4B,0x6B},
	{0x03,0x4C,0x0C},
	{0x03,0x4D,0xD0},
	{0x03,0x4E,0x07},
	{0x03,0x4F,0x38},
	{0x03,0x90,0x00},
	{0x30,0x20,0x10},
	{0x30,0x2D,0x02},
	{0x30,0x2F,0x80},
	{0x30,0x32,0xA3},
	{0x30,0x33,0x20},
	{0x30,0x34,0x24},
	{0x30,0x41,0x15},
	{0x30,0x42,0x87},
	{0x30,0x50,0x35},
	{0x30,0x56,0x57},
	{0x30,0x5D,0x41},
	{0x30,0x97,0x69},
	{0x31,0x09,0x41},
	{0x31,0x48,0x3F},
	{0x33,0x0F,0x07},
	{0x33,0x44,0x5F},
	{0x33,0x45,0x1F},
	{0x33,0x68,0x09},
	{0x33,0x69,0x00},
	{0x33,0x70,0x6F},
	{0x33,0x71,0x27},
	{0x33,0x72,0x47},
	{0x33,0x73,0x27},
	{0x33,0x74,0x27},
	{0x33,0x75,0x27},
	{0x33,0x76,0x8F},
	{0x33,0x77,0x37},
	{0x33,0xD4,0x0C},
	{0x33,0xD5,0xD0},
	{0x33,0xD6,0x07},
	{0x33,0xD7,0x38},
	{0x41,0x00,0x0E},
	{0x41,0x04,0x32},
	{0x41,0x05,0x32},
	{0x41,0x08,0x01},
	{0x41,0x09,0x7C},
	{0x41,0x0A,0x00},
	{0x41,0x0B,0x00},

	{0x01,0x00, 0x1},  /* Streaming on */	
};

static const UINT8 cmd_still_code[][3]={

	/* Format: Addr, Data                       */
	/* PLL settings                             */
	/* INCK = 24MHz                             */
	/* MIPI 4lanes 640MHz                       */
	/* line_total 3440*2=6880 pixel_clock/line  */
	/* frame_total 2478 line/frame              */
	/* h_size 3280                              */
	/* v_size 2464                              */

	{0x01,0x00, 0x00},              /* Standby                   */
	                                /* 24M*160/12 = 640M         */
	{0x03,0x01, 0x05},              /* vt_pix_clk_div            */
	{0x03,0x03, 0x01},              /* vt_sys_clk_div            */
	{0x03,0x05, 0x0C},              /* pre_pll_clk_di            */
	{0x03,0x09, 0x05},              /* OPPXCK_DIV                */
	{0x03,0x0B, 0x01},              /* OPSYCK_DIV                */
                                                                
	{0x03,0x0C, 0x01},              /* PLL_OP_MPY                */
	{0x03,0x0D, 0x40},              /* PLL_OP_MPY                */
                                                                
	{0x03,0x0E, 0x01},              /* PLL_SOLO_DRIVE            */
	                                /* 0x0103, 0x01  SW_Reset    */
#if 0                               
	{0x02,0x02, 0x09},              /* shutter                   */
	{0x02,0x03, 0xAA},              /* shutter                   */
#endif                              
    {0x03,0x40, 0x09},              /* frame_length_lines [15:8] */
	{0x03,0x41, 0xAE},              /* frame_length_lines [7:0]  */
	{0x03,0x42, ((3440*2)>>8)},     /* line_length_pck [15:8]    */
	{0x03,0x43, ((3440*2)&0xFF)},   /* line_length_pck [7:0]     */
	{0x03,0x44, 0x00},  
	{0x03,0x45, 0x00},  
	{0x03,0x46, 0x00},  
	{0x03,0x47, 0x00},  
	{0x03,0x48, 0x0C},  
	{0x03,0x49, 0xCF},  
	{0x03,0x4A, 0x09},  
	{0x03,0x4B, 0x9F},  
	{0x03,0x4C, 0x0C},  
	{0x03,0x4D, 0xC0},  
	{0x03,0x4E, 0x09},  
	{0x03,0x4F, 0xA0},  
	{0x03,0x90, 0x00},  
	{0x30,0x20, 0x10},  
	{0x30,0x2D, 0x02},  
	{0x30,0x2F, 0x80},  
	{0x30,0x32, 0xA3},  
	{0x30,0x33, 0x20},  
	{0x30,0x34, 0x24},  
	{0x30,0x41, 0x15},  
	{0x30,0x42, 0x87},  
	{0x30,0x50, 0x35},  
	{0x30,0x56, 0x57},  
	{0x30,0x5D, 0x41},  
	{0x30,0x97, 0x69},  
	{0x31,0x09, 0x41},  
	{0x31,0x48, 0x3F},  
	{0x33,0x0F, 0x07},  
	{0x33,0x44, 0x57},  
	{0x33,0x45, 0x1F},  
	{0x33,0x64, 0x00},  
	{0x33,0x68, 0x18},  
	{0x33,0x69, 0x00},  
	{0x33,0x70, 0x77},  
	{0x33,0x71, 0x2F},  
	{0x33,0x72, 0x4F},  
	{0x33,0x73, 0x2F},  
	{0x33,0x74, 0x2F},  
	{0x33,0x75, 0x37},  
	{0x33,0x76, 0x9F},  
	{0x33,0x77, 0x37},  
	{0x33,0xD4, 0x0C},  
	{0x33,0xD5, 0xC0},  
	{0x33,0xD6, 0x09},  
	{0x33,0xD7, 0xA0},  
	{0x41,0x00, 0x0E},  
	{0x41,0x04, 0x32},  
	{0x41,0x05, 0x32},  
	{0x41,0x08, 0x01},  
	{0x41,0x09, 0x7C},  
	{0x41,0x0A, 0x00},  
	{0x41,0x0B, 0x00},  
	 
	{0x01,0x00, 0x01}, /* Streaming   */
};

/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg =     
{                                                
	.mipi_nockrsten  =0 ,                    
	.wordcntsel 	 =0 ,   /* 0: From LVDS embeded data (CSI2)  1: From [90A4: HSIZE BYTE COUNT] 5330 mipi 4 Lane bug*/                         
	.Lvdsmode		 =0 ,   /* 8 bit mode (CSI2 )*/                        
	.Lvdslanenum	 =0 ,   /* Option: 2(1) or 4(0) lanes mode, refer to R0x3364 default: 4 lanes, desired: 2 lanes*/         
                            /* 720p, 1080p don't set the register, so default: 4 lanes*/
                            /* full is setted 4 lanes mode. */
	.Lvdslanemode	 =0 ,                         
	.Lvdslaneswap	 =1 ,   /* LSB*/                       
	.Lvdsraw		 =0 ,   /* raw10*/ 
	.Lvdsoutsel 	 =0 ,   /* debug, output s2pdata lane to Tggpio&Lmigpio */
	.Lvdseofsel 	 =1 ,   /* count eol number equal as Vblank [0x9090] (MIPI/LVDS) */                       
	.Lvdseolsel 	 =0 ,   /* 0: count pixel equal as the register horizorntal size 0x90A4  Hsize 1: detect sync code*/
	.lvdsvdsrc		 =1 ,   /* decoding embedded data header sof as internal vd source*/                     
	.lvds_dphy_sel	 =1 ,   /* MIPI D-PHY*/
	.Ctrlword_sol	 =0x2B,                      
	.Ctrlword_sof	 =0x00,                     
	.syncwrod0		 =0xB8,                     
	.syncwrod1		 =0 ,                        
	.syncwrod2		 =0 , 
	/*HW signal spec */
	.vmode			 =0 ,
	.S2pphase		 =4 ,                        
	.tsettle		 =3 ,                        
	.teot			 =7 ,   
	/*Extention Part */
	.Exthd_num		 =PREVIEW_EXTEND_LINES-1,                        
	.Exthd_pol		 =0 ,                        
	.Exthd_mode 	 =0 ,  
	.cropenable 	 =1 ,
};                                               

static const sensorTgPreviewCfg_t sensorTgPreviewVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewVGABegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
	.hsize = 					PREVIEW_VGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize =                    PREVIEW_VGA_VSIZE,
	.sensor_model =             SENSORMODEL,
	.cfa_pattern =              PREVIEW_VGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 30007000,
	.sensor_pclk = 128000000,
	.sensor_htotal = 3440,			
    .pix_100khz =1440,
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGA60FPSCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewVGABegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
	.hsize = 					PREVIEW_VGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize =                    PREVIEW_VGA_VSIZE,
	.sensor_model =             SENSORMODEL,
	.cfa_pattern =              PREVIEW_VGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 60015000,
	.sensor_pclk = 128000000,
	.sensor_htotal = 1720,			
	.pix_100khz =1440,
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGA30FPSCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewVGABegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
	.hsize = 					PREVIEW_VGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize =                    PREVIEW_VGA_VSIZE,
	.sensor_model =             SENSORMODEL,
	.cfa_pattern =              PREVIEW_VGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 30007000,
	.sensor_pclk = 128000000,
	.sensor_htotal = 1720,			
	.pix_100khz =1440,
};

static const sensorTgPreviewCfg_t sensorTgPreviewQVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewQVGABegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_QVGA_HSIZE,
		.ySize = PREVIEW_QVGA_VSIZE,
	},
	.hsize = 					PREVIEW_QVGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize =                    PREVIEW_QVGA_VSIZE,
	.sensor_model =             SENSORMODEL,
	.cfa_pattern =              PREVIEW_QVGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 120100000,
	.sensor_pclk = 264000000,
	.sensor_htotal = 3440,			
	.pix_100khz =1440,
};

static const sensorTgPreviewCfg_t sensorTgPreviewQVGA30FPSCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewQVGABegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_QVGA_HSIZE,
		.ySize = PREVIEW_QVGA_VSIZE,
	},
	.hsize = 					PREVIEW_QVGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize =                    PREVIEW_QVGA_VSIZE,
	.sensor_model =             SENSORMODEL,
	.cfa_pattern =              PREVIEW_QVGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 30001000,
	.sensor_pclk = 264000000,
	.sensor_htotal = 3440,			
	.pix_100khz =1440,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreview1080PBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_1080P_HSIZE,
		.ySize = PREVIEW_1080P_VSIZE,
	},
	.hsize =					PREVIEW_1080P_HSIZE,
	.vsize =					PREVIEW_1080P_VSIZE,
	.sensor_model =				SENSORMODEL,
	.cfa_pattern =				PREVIEW_1080P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 30007000,
	.sensor_pclk = 200000000,
	.sensor_htotal = 3448, 		
	.pix_100khz =1440,
};

static const sensorTgPreviewCfg_t sensorTgPreview720PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreview720PBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_720P_HSIZE,
		.ySize = PREVIEW_720P_VSIZE,
	},
	.hsize =					PREVIEW_720P_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize = 					PREVIEW_720P_VSIZE,  
	.sensor_model =             SENSORMODEL,
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 30007000,
	.sensor_pclk = 200000000,
	.sensor_htotal = 3448, 		
	.pix_100khz =1440,
};

static const sensorTgPreviewCfg_t sensorTgPreview720P60FPSCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreview720PBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_720P_HSIZE,
		.ySize = PREVIEW_720P_VSIZE,
	},
	.hsize =					PREVIEW_720P_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize = 					PREVIEW_720P_VSIZE,  
	.sensor_model =             SENSORMODEL,
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 60046000,
	.sensor_pclk = 200000000,
	.sensor_htotal = 3448, 		
	.pix_100khz =1440,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &sensorTgPreviewVGACfg,            /* bin2-VGA30FPS(320Mbps)           0x30 */          
    &sensorTgPreviewQVGACfg,           /* bin4-QVGA120FPS                  0x31 */          
    &sensorTgPreview720PCfg,           /* bin2-720@60fps+dummy=30fps       0x32 */          
    &sensorTgPreview720P60FPSCfg,      /* bin2-720@60fps                   0x33 */          
    &sensorTgPreview1080PCfg,          /* crop full size, FOV is bad       0x34 */           
    &sensorTgPreviewQVGA30FPSCfg,      /* bin4-QVGA120FPS+dummy=30fps      0x35 */          
    &sensorTgPreviewVGA60FPSCfg,       /* bin2-VGA60FPS(640Mbps)           0x36 */          
    &sensorTgPreviewVGA30FPSCfg,       /* VGA@60fps+dummy=30fps            0x37 */          
};  

static  sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
				/*sensorDoFuncSnapEarly0, 	*/
			},
			.snapExpBegin =        sensorDoFuncSnapExpBegin,
			.snapBshutterBegin =   sensorDoFuncSnapBshutterBegin,
			.snapBshutterFollow =  sensorDoFuncSnapBshutterFollow,
			.snapBshutterEnd =     sensorDoFuncSnapBshutterEnd,
			.snapLongExpBegin =    NULL,
			.snapLongExpFollow =   NULL,
			.snapLongExpEnd =      NULL,
			.snapShortExp =        sensorDoFuncSnapShortExp,
			.snapExpEnd =          sensorDoFuncSnapExpEnd,
			.psnapDummy =          NULL,
			.psnapReadoutField = (sensorDoFunc_t []) {
				sensorDoFuncSnapReadoutField0,
			},
			.snapFieldEnd =        sensorDoFuncSnapFieldEnd,
			.snapReadoutEnd =      sensorDoFuncSnapReadoutEnd,
			.snapNext = 		   sensorDoFuncSnapNext,			
		},
        .crop = NULL, 
 		.hsize =                   CAPTURE_HSIZE,
		.vsize =				   CAPTURE_VSIZE,
		.sensor_model =			   SENSORMODEL,/*TYPE_HSCMOS,*/
		.cfa_pattern =			   SNAP_CFAPATTERN,
		.n_dummy =				   SNAP_DUMMYVD_NUM,        	
		.n_readout =			   SNAP_READOUTVD_NUM,      
		.n_capvd =				   SNAP_CAPTUREVD_NUM,      
		.n_snap_early =			   SNAP_EARLY_NUM,    
		.skip_snap_readout_vd =	   SKIP_SNAP_READOUTVD_EN,  
		.fldsel_start =			   FLDSEL_START_NUM,        
		.fldsel_end =			   FLDSEL_END_NUM,          
		.hratio = 4000, /*4:3 sensor*/
		.vratio = 3000,
		.pix_100khz =1440,
};

static  sensorTgSnapCfg_t sensorTgSnap2RollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
				/*sensorDoFuncSnapEarly0, 	*/
			},
			.snapExpBegin =        sensorDoFuncSnapExpBegin,
			.snapBshutterBegin =   sensorDoFuncSnapBshutterBegin,
			.snapBshutterFollow =  sensorDoFuncSnapBshutterFollow,
			.snapBshutterEnd =     sensorDoFuncSnapBshutterEnd,
			.snapLongExpBegin =    NULL,
			.snapLongExpFollow =   NULL,
			.snapLongExpEnd =      NULL,
			.snapShortExp =        sensorDoFuncSnapShortExp,
			.snapExpEnd =          sensorDoFuncSnapExpEnd,
			.psnapDummy =          NULL,
			.psnapReadoutField = (sensorDoFunc_t []) {
				sensorDoFuncSnapReadoutField0,
			},
			.snapFieldEnd =        sensorDoFuncSnapFieldEnd,
			.snapReadoutEnd =      sensorDoFuncSnapReadoutEnd,
			.snapNext = 		   sensorDoFuncSnapNext,			
		},
        .crop = NULL, 
		.hsize =                   CAPTURE2_HSIZE,
		.vsize =				   CAPTURE2_VSIZE,
		.sensor_model =			   SENSORMODEL,/*TYPE_HSCMOS,*/
		.cfa_pattern =			   SNAP2_CFAPATTERN,
		.n_dummy =				   SNAP_DUMMYVD_NUM,        	
		.n_readout =			   SNAP_READOUTVD_NUM,      
		.n_capvd =				   SNAP_CAPTUREVD_NUM,      
		.n_snap_early =			   SNAP_EARLY_NUM,    
		.skip_snap_readout_vd =	   SKIP_SNAP_READOUTVD_EN,  
		.fldsel_start =			   FLDSEL_START_NUM,        
		.fldsel_end =			   FLDSEL_END_NUM,          
		.hratio = 4000, /*4:3 sensor*/
		.vratio = 3000,
		.pix_100khz =1440,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg, 
    &sensorTgSnap2RollCfg, 
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVGACfg = {
	.linetotal       = PREVIEW_VGA_LINETOTAL,
	.lineblank       = PREVIEW_VGA_LINEBLANK,
	.frametotal      = PREVIEW_VGA_FRAMETOTAL,
	.frameblank      = PREVIEW_VGA_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_VGA_30FPS_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_VGA_30FPS_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_VGA_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_VGA_VRESHAPE_RISE,
	.hoffset         = PREVIEW_VGA_HOFFSET,
	.voffset         = PREVIEW_VGA_VOFFSET,
	.hsize           = PREVIEW_VGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize           = PREVIEW_VGA_VSIZE,
	.lastlinepix     = PREVIEW_VGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_VGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_VGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_VGA_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVGA60fpsCfg = {
	.linetotal       = PREVIEW_VGA_LINETOTAL,
	.lineblank       = PREVIEW_VGA_LINEBLANK,
	.frametotal      = PREVIEW_VGA_FRAMETOTAL,
	.frameblank      = PREVIEW_VGA_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_VGA_60FPS_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_VGA_60FPS_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_VGA_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_VGA_VRESHAPE_RISE,
	.hoffset         = PREVIEW_VGA_HOFFSET,
	.voffset         = PREVIEW_VGA_VOFFSET,
	.hsize           = PREVIEW_VGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize           = PREVIEW_VGA_VSIZE,
	.lastlinepix     = PREVIEW_VGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
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
	.hsize           = PREVIEW_QVGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize           = PREVIEW_QVGA_VSIZE,
	.lastlinepix     = PREVIEW_QVGA_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,  
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_QVGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_QVGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_QVGA_HDNUM_CMDPORT,
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
	.hsize           = PREVIEW_1080P_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize           = PREVIEW_1080P_VSIZE,
	.lastlinepix     = PREVIEW_1080P_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
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
	.hsize           = PREVIEW_720P_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.vsize           = PREVIEW_720P_VSIZE,
	.lastlinepix     = PREVIEW_720P_HSIZE,/* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,*/
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_720P_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
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

static  sensorSyncSigCfg_t sensorSyncSigSnap2Cfg = {
	.linetotal         = CAPTURE2_LINETOTAL,
	.lineblank         = CAPTURE2_LINEBLANK,
	.frametotal        = CAPTURE2_FRAMETOTAL,
	.frameblank        = CAPTURE2_FRAMEBLANK,
	.hreshape_fall     = CAPTURE2_HRESHAPE_FALL,
	.hreshape_rise     = CAPTURE2_HRESHAPE_RISE,
	.vreshape_fall     = CAPTURE2_VRESHAPE_FALL,
	.vreshape_rise     = CAPTURE2_VRESHAPE_RISE,
	.hoffset           = CAPTURE2_HOFFSET,
	.voffset           = CAPTURE2_VOFFSET,
	.hsize             = CAPTURE2_HSIZE,
	.vsize             = CAPTURE2_VSIZE,
	.lastlinepix       = CAPTURE2_HSIZE,
	.hdvdopos          = 0,
	.conf_totalsync    = 0,
	.conf_hdpol        = 0,
	.conf_vdpol        = 0,
	.conf_hdpclkedge   = 0,
	.conf_hvldbypass   = 0,
	.conf_vvldbypass   = 0,
	.frameblank_base   = 0,
	.vreshape_base     = CAPTURE2_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	   = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	   = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end   = CAPTURE2_HDNUM_FRAME_END,
	.hdnum_cmdport     = CAPTURE2_HDNUM_CMDPORT,
};

static sensorTgCfg_t sensorTgCfg = {
	.tg_type =					TGTYPE,
	.n_preview_submode =		sizeof(sensorTgPreviewCfgTbl) /
								sizeof(sensorTgPreviewCfgTbl[0]),
	.n_snap_submode =			sizeof(sensorTgSnapCfgTbl) /
								sizeof(sensorTgSnapCfgTbl[0]),
	.vshutter_need_resend =		VSHUTTER_RESEND,
	.exp_effect_frame_count =	EXP_FRAME_NUM,

};

static const sensorTgDev_t sensorTgDev = {
	.tgCfg = &sensorTgCfg,
	.tgSvc = &(const sensorTgSvc_t) {
		.resload =		sensorCfgInit,
		.resstop =		sensorCfgStop,
		.init =			sensorDoFuncInit,
		.stop =			sensorDoFuncStop, /****TODO***/
		.pwrsave =		NULL, /****TODO***/
		.expwrite =		sensor_cmdport_exp_write,
		.expvtbldo =	sensor_cmdport_exp_vtbl_do,
		.crop =			NULL, /****TODO***/
	},
	.previewCfg =		sensorTgPreviewCfgTbl,
	.snapCfg =			sensorTgSnapCfgTbl,
	.defSyncSigCfg  =	&sensorSyncSigPreviewVGACfg,
	.verStr =			MODEL_VERSION,
};

static void 
sensorReportsize()
{
	SENSOR_PROF_LOG_PRINT("cdsp gating           = (%x)",READ8(0xb0009080));
	SENSOR_PROF_LOG_PRINT("mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	SENSOR_PROF_LOG_PRINT("front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	SENSOR_PROF_LOG_PRINT("imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	SENSOR_PROF_LOG_PRINT("rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	WRITE16(0xb0009064,1);
	SENSOR_PROF_LOG_PRINT("line count            = (%d)",READ16(0xb0009064));
}

UINT32 
SONYsensorRead(UINT16 addr)
{
	UINT32 cmdportid;
	static UINT8 regread[3];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	
	SENSOR_PROF_LOG_ADD("SONYsensorRead++");
	sensorSerialBlockRead(cmdportid,1,regread,0);
	while((READ8(0xb0009105)&0x01)==0x01);/*sif1  CPU  trigger busy*/
    printf("\nREAD [0x%x] = [0x%x]\n",addr,regread[2]);
	SENSOR_PROF_LOG_PRINT("READ [%x] = [%x]",addr,regread[2]);
	SENSOR_PROF_LOG_ADD("SONYsensorRead--");
	return regread[2];
}

void
sensorTg_sonyimx175_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}
	
static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
	
	SENSOR_PROF_LOG_ADD("SONY IMX175 sensorDoFuncInit++ 6330 ");
	SENSOR_PROF_LOG_PRINT("________Sensor driver ver <<%s>>", MODEL_VERSION);
	SENSOR_PROF_LOG_PRINT("________Sensor driver build time <<%s>>", BUILD_TIME);
    SENSOR_PROF_LOG_ADD("sensorDoFuncInit()_sonyimx175_enter_....");
    /* TGGPIO configuration here */    
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;
	/*Debug only */
	/*Pixel Clock = 27MHz /27*48/2 = 24 MHz, XGA= 15fps VGA=30fps*/

	sensorTgPllClkExtSet(24000000);/*24MHz*/                                                
	
	halPllFrontClkSet(0x00,94500); /* 94.5 MHz */   
    /*halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121000);*/ /* 126 MHz  CDSP 243MHz*/
	/* Master clock and Pixel clock configuration here */                                       
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);                                
	/*****TODO*****front reset after these???*/                                                 
	#if defined(HW_EVB_SPE7001) /*EVB case  */

		#if 0
		/*5330 */
		sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN);                            
		sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);                            
		sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0);				
		sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);	
		#endif
		
		/*6330 */
		sp5kGpioCfgSet(SENSOR_PWRDN_GRP,SENSOR_PWRDN,SENSOR_PWRDN);   
		sp5kGpioWrite(SENSOR_PWRDN_GRP,SENSOR_PWRDN,0);				
		tmrUsWait(100000); /*10ms*/
		sp5kGpioWrite(SENSOR_PWRDN_GRP,SENSOR_PWRDN,SENSOR_PWRDN);				
		
		tmrUsWait(100000); /*10ms*/
	#else
        sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_ON);
	#endif
	
   	cmdportid = sensorTgCfg.cmdportid_xtg;
   	tmrUsWait(10000);
    printf("sensor DoFunInit model-ID =\n ");
	UINT16 id[2];
	UINT16 i;
	id[0]=SONYsensorRead(0x0000); /*sensor ID-MSB */
    id[1]=SONYsensorRead(0x0001); /*sensor ID-LSB  */
	printf("sensor id =%x %x\n",id[0],id[1]);
	for(i=0;i<3;i++)
	{
		if((id[0]==0x01)&&(id[1]==0x75))
			continue;
		else		
		{
			id[0]=SONYsensorRead(0x0000); /*sensor ID-MSB */
			id[1]=SONYsensorRead(0x0001); /*sensor ID-LSB  */
			printf("retry read sensor ID\n");
			printf("sensor id =%x %x\n",id[0],id[1]);
		}
	}

#if defined(HW_EVB_SPE7001) /*EVB case  */
	sp5kGpioWrite(SENSOR_PWRDN_GRP,SENSOR_PWRDN,0);				/* RESET LOW*/			   
	tmrUsWait(50000); /*50ms*/														   
	sp5kGpioWrite(SENSOR_PWRDN_GRP,SENSOR_PWRDN,SENSOR_PWRDN);				
	tmrUsWait(50000); /*50ms*/														   
#else
	tmrUsWait(50000); /*50ms*/
#endif
	sensorSerialBulkWrite(cmdportid,(sizeof(cmd_vga_30fps_code)/sizeof(cmd_vga_30fps_code[0]))-1, cmd_vga_30fps_code[0],0);
	tmrUsWait(30000);/*30ms*/
	sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code_StreamOn)/sizeof(cmd_init_code_StreamOn[0])), cmd_init_code_StreamOn[0],0);
	tmrUsWait(30000);/*30ms*/

#if 0 /*check VD fail interrupt */
	WRITE8(0xb000901A,0xF5);
	WRITE8(0xb0009002,0x80);
	WRITE8(0xb0009014,0x1);
	tmrUsWait(100000);/*100ms*/
	if(READ8(0xb0009002)==0x80)	
		SENSOR_PROF_LOG_ADD("Init Time out!!!");
#endif 

	/*TG PLL=MCLK out=24MHz*/
	sensorLVDSCfgSet(&sensorLVDSCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

	if(1){/*Always Use preview mode 9 when pclk1x > 150MHz*/
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,300000); /* 300MHz or 288MHz*/
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9*/
	}
	else{
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,150000); /* 150 MHz */
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,0); /*enable preview mode 9*/
	}
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/

#if MODE_SYNC_EARLY /*config pre command IRQ */
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif
    sensorResumeInstall(sensorResumeExecuteLoad);
	sensorSuspendInstall(sensorSuspendExecuteLoad);
    fpvsusp = 0;
	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()___done");
	SENSOR_PROF_LOG_ADD("SONY IMX175 sensorDoFuncInit--");
	printf("sensorDoFuncInit()___done \n"); 	
}

static void 
expline_convert_wr()
{
	UINT32 line_time1,exp_line1,frame_length1=0;
	UINT32 line_time2,exp_line2,frame_length2;
	UINT32 smode;

	static UINT8 regdata[][3] SENSOR_SIF1DMA_VARBUF = {
		{0x02,0x03,0x00},  /* shutter						   */
		{0x02,0x02,0x00},  /* shutter						   */
		{0x03,0x41,0x00},  /* frame_length_lines [7:0]	*/	  
		{0x03,0x40,0x00},  /* frame_length_lines [15:8] */	  
	};
	smode= sensorModeGet();

	if(g_last_smode==0x00)
		return;

	exp_line1=g_last_exp_line;		


	if(g_last_smode==0x30){
		/*VGA 320Mbps 30fps*/
		line_time1 =2688;
	}
	else if(g_last_smode==0x31 || g_last_smode==0x35){
		/*QVGA 120fps, 30fps(dummy)*/
		line_time1 =1303;
	}
	else if(g_last_smode==0x32 || g_last_smode==0x33){
		/*720P 60fps, 30fps(dummy)*/
		line_time1 =1724;
	}
	else if(g_last_smode==0x36 || g_last_smode==0x37){
		/*VGA 640Mbps 60fps, 30fps(dummy)*/
		line_time1 =1724;
	}
 	else /*if(g_last_smode==0x34)*/{
		/*1080P 30fps*/
		line_time1 =1724;
	}	

	if(smode==0x30){
		line_time2 =2688;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=1240;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
		SENSOR_PROF_LOG_PRINT("=> 0x30 (%d)=>(%d) (%d)=>(%d)",exp_line1,exp_line2,frame_length1,frame_length2);
	}
	else if(smode==0x31){
		line_time2 =1303;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=639;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
		SENSOR_PROF_LOG_PRINT("=> 0x31 (%d)=>(%d) (%d)=>(%d)",exp_line1,exp_line2,frame_length1,frame_length2);
	}
	else if(smode==0x32){
		/*720P 30 FPS*/
		line_time2 =1724;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=1933;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
	}
	else if(smode==0x33){
		/*720P 60 FPS*/
		line_time2 =1724;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=966;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
	}
    else if(smode==0x35){
		/*QVGA 30 FPS*/
		line_time2 =1303;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=2558;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
	}
	else if(smode==0x36){
		/*VGA 60fps, 640Mbps*/
		line_time2 =1344;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=1240;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
	}
	else if(smode==0x37){
		/*VGA 30fps, 640Mbps*/
		line_time2 =1344;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=2480;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
	}
	else /*if(smode==0x34)*/{
		/*1080P 30 FPS*/
		line_time2 =1724;
		exp_line2=line_time1*exp_line1/line_time2;
		frame_length2=1933;
		if(exp_line2>frame_length2)
			frame_length2=exp_line2+4;
	}	
	regdata[0][2]=exp_line2&0xFF;
	regdata[1][2]=exp_line2>>8;
	regdata[2][2]=frame_length2&0xFF;
	regdata[3][2]=frame_length2>>8;
	sensorSerialDirectBufSnoop(regdata[0], sizeof(regdata));
	sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg, 4, regdata[0],0);
}

static void
sensorDoFuncStop()
{
    sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_OFF);
}

static void
sensorDoFuncPreviewVGABegin()
{
	UINT32 cmdportid, smode;                                                                                                                             
  	SENSOR_PROF_LOG_ADD("mipi VGA cmd done ++");                                                             
	cmdportid = sensorTgCfg.cmdportid_xtg;  
	if(g_IsSnap2Pv){
		sensor_event_wait(SENSOR_EVENT_VVALID_END);
    }
	g_IsSnap2Pv=0;
  	SENSOR_PROF_LOG_ADD("mipi VGA cmd done wait done..");  
  	smode=sensorModeGet();
	sensorSerialFlush(cmdportid);
  	if(smode==0x30){        
  		/*320Mbps*/
  		sensorSerialBulkWrite(cmdportid, sizeof(cmd_vga_30fps_code)/sizeof(cmd_vga_30fps_code[0]), cmd_vga_30fps_code[0],0);  
  		sensorSyncSigInit(&sensorSyncSigPreviewVGACfg);
    }else if(smode==0x36 || smode==0x37){
    	/*640Mbps*/
    	sensorSerialBulkWrite(cmdportid, sizeof(cmd_vga_60fps_code)/sizeof(cmd_vga_60fps_code[0]), cmd_vga_60fps_code[0],0);  
    	sensorSyncSigInit(&sensorSyncSigPreviewVGA60fpsCfg);                                                           
 	}
	expline_convert_wr();
	SP5K_SET_LVDS_HCROP(PREVIEW_VGA_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  	SENSOR_PROF_LOG_ADD("mipi VGA cmd done --");                                                             
    fpvsusp = 1;
}

static void
sensorDoFuncPreviewQVGABegin()
{
    UINT32 cmdportid;
	SENSOR_PROF_LOG_ADD("mipi QVGA cmd done ++");                                                             
	cmdportid = sensorTgCfg.cmdportid_xtg;  

	/*if(g_IsSnap2Pv){*/
		sensor_event_wait(SENSOR_EVENT_VVALID_END);
    /*}*/
    
	g_IsSnap2Pv=0;
  	SENSOR_PROF_LOG_ADD("mipi QVGA cmd done wait done..");                                                             
    sensorSerialFlush(cmdportid);	
 	sensorSerialBulkWrite(cmdportid, sizeof(cmd_qvga_code)/sizeof(cmd_qvga_code[0]), cmd_qvga_code[0],0);
	expline_convert_wr();
  	sensorSyncSigInit(&sensorSyncSigPreviewQVGACfg);                                                    
	SP5K_SET_LVDS_HCROP(PREVIEW_VGA_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
  	SENSOR_PROF_LOG_ADD("mipi QVGA cmd done --");                                                             
    fpvsusp = 1;
}

static void
sensorDoFuncPreview1080PBegin()
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	if(g_IsSnap2Pv){
		sensor_event_wait(SENSOR_EVENT_VVALID_END);
    }
	g_IsSnap2Pv=0;
	sensorSerialFlush(cmdportid);
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_1080P_code)/sizeof(cmd_1080P_code[0]), cmd_1080P_code[0],0);
	expline_convert_wr();
	sensorSyncSigInit(&sensorSyncSigPreview1080PCfg);
	SP5K_SET_LVDS_HCROP(PREVIEW_1080P_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	SENSOR_PROF_LOG_ADD("mipi 1080p done");
    fpvsusp = 1;
}

static void
sensorDoFuncPreview720PBegin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;

	if(g_IsSnap2Pv){
		sensor_event_wait(SENSOR_EVENT_VVALID_END);
    }
	g_IsSnap2Pv=0;
	sensorSerialFlush(cmdportid);
  	sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_code)/sizeof(cmd_720P_code[0]), cmd_720P_code[0],0);
	expline_convert_wr();
	sensorSyncSigInit(&sensorSyncSigPreview720PCfg);     
	SP5K_SET_LVDS_HCROP(PREVIEW_720P_HCROP_OFFSET);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    SENSOR_PROF_LOG_ADD("mipi 720P cmd done");
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

static void
sensorDoFuncSnapExpBegin()
{
    SENSOR_PROF_LOG_ADD("SnapExpBegin s");  
	if(!g_IsSnap2Pv){
		g_IsSnap2Pv=1;
		UINT32 cmdportid, smode;
		cmdportid = sensorTgCfg.cmdportid_xtg;
        smode=sensorModeGet();
        
        if(smode==0x40){
            sensorSyncSigSet(&sensorSyncSigSnapCfg);  
            SP5K_SET_LVDS_HCROP(CAPTURE_HCROP_OFFSET);
            SENSOR_PROF_LOG_ADD("smode: 0x40");  
        }else{
            /* 0x41 */
            sensorSyncSigSet(&sensorSyncSigSnap2Cfg);  
            SP5K_SET_LVDS_HCROP(CAPTURE2_HCROP_OFFSET);
            SENSOR_PROF_LOG_ADD("smode: 0x41");  
        }
		
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
		sensorSerialFlush(cmdportid);
        
        if(smode==0x40){
            sensorSerialBulkWrite(cmdportid,(sizeof(cmd_still_code)/sizeof(cmd_still_code[0])), cmd_still_code[0],0);
        }else{
            /* 0x41 */
            sensorSerialBulkWrite(cmdportid,(sizeof(cmd_vga_30fps_code)/sizeof(cmd_vga_30fps_code[0])), cmd_vga_30fps_code[0],0);
        }
	}
    SENSOR_PROF_LOG_ADD("SnapExpBegin e");  
}

static void
sensorDoFuncSnapBshutterBegin()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapBshutterBegin()");
}

static void
sensorDoFuncSnapBshutterFollow()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapBshutterFollow()");
}

static void
sensorDoFuncSnapBshutterEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapBshutterEnd()");
}

static void
sensorDoFuncSnapShortExp()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapShortExp()");
}

static void
sensorDoFuncSnapExpEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapExpEnd()");
}

static void
sensorDoFuncSnapReadoutField0()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapReadoutField0()");
}

static void
sensorDoFuncSnapFieldEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapFieldEnd()");
	sensorReportsize();
}

static void
sensorDoFuncSnapReadoutEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapReadoutEnd()");
	sensorReportsize();
}

static void
sensorDoFuncSnapNext(UINT32 mode)
{
#if BURST_CAP_EN
	if((mode&0xF0)==0x40) {
		SENSOR_PROF_LOG_ADD("@@@@ get sensor next mode is snap");
		sensorTgSnapRollCfg.skip_snap_readout_vd = 3;/*20110830 @ BASEFW change to 3*/
	}
	else{
		SENSOR_PROF_LOG_ADD("@@@@ get sensor next mode is preview");
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
sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
	UINT32 smode;
	smode=sensorModeGet();
	if((smode&0xF0)==0x30){
		g_last_exp_line   = (pdata[3*1+2]<<8)||(pdata[3*0+2]);
		g_last_exp_line   = pdata[3*1+2];
		g_last_exp_line   = g_last_exp_line<<8;
		g_last_exp_line  |= pdata[3*0+2];
		g_last_smode = smode;
		SENSOR_PROF_LOG_PRINT("[%d %x]",g_last_exp_line,g_last_smode);
	}
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
}

static void
sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
}

static UINT32 vcm_cmdportid=0;
void
VCM_DW9714A_Init(void)
{
	static UINT8 b_init_vcm=0;
	if(b_init_vcm==0)
	{
		static const sensorCmdport_t vcm_cmdport_xtg_cfg = {
			.mode		  = 0x18,
			.format2	  = 1,
			.format3	  = 0,
			.speed		  = 4,
			.nbit_regdata = 8,
			.nbit_regaddr = 8,
		};
		vcm_cmdportid = sensorSerialCreate(&vcm_cmdport_xtg_cfg);
		printf("vcm_cmdportid = %d\n",vcm_cmdportid);
		b_init_vcm=1;
	}
}

void 
VCM_DW9714A_I2CWr(UINT32 v1,UINT32 v2)
{
	static UINT8 regdata[2];
	SENSOR_PROF_LOG_ADD("VCM_DW9714A_I2CWr++");
	VCM_DW9714A_Init();
	regdata[0]=v1;
	regdata[1]=v2;
	sensorSerialBulkWrite(vcm_cmdportid,1,regdata,0);
	printf("VCM_DW9714A_I2CWr WRITE [0x%x 0x%x]\n",v1,v2);
}

void 
VCM_DW9714A_I2CRd(UINT32 v1,UINT32 *v2)
{
	static UINT8 regread[2];
	SENSOR_PROF_LOG_ADD("VCM_DW9714A_I2CRd++");
	VCM_DW9714A_Init();
	regread[0]=v1;
	sensorSerialBlockRead(vcm_cmdportid,1,regread,0);
	while((READ8(0xb0009105)&0x01)==0x01);/*sif1  CPU  trigger busy*/
	*v2=regread[1];
	printf("\nREAD [%x] = [%x]\n",regread[0],regread[1]);
	SENSOR_PROF_LOG_PRINT("VCM_DW9714A_I2CRd READ [%x] = [%x]",v1,*v2);
}

#if SP5K_SENSOR_SONYIMX175
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regData[3][3] ={
		{0x02,0x02,0x00},
		{0x02,0x03,0x00},
		
	};
	regData[0][2] = (expLine>>8)&0xf;
	regData[1][2] =  expLine&0xff;
	
	SENSOR_PROF_LOG_PRINT("expLine(reg0) = %d [%x %x %x]",expLine,regData[0][0],regData[0][1],regData[0][2]);
	SENSOR_PROF_LOG_PRINT("expLine(reg1) = %d [%x %x %x]",expLine,regData[1][0],regData[1][1],regData[1][2]);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,2, regData[0]);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
	sensor_event_wait(0x04);/*wait new frame */
	expLine = expLine;
}
#endif

static UINT32 sensorResumeExecuteLoad()
{
	#if SUSPEND_ENABLE       
		if(sensorSuspended ==1){                                                                                     
	    	/* sensorResumeExecute() */       
	    	printf("senResu s");/*Edward 20111126*/   
            #if 0
                sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code_StreamOn)/sizeof(cmd_init_code_StreamOn[0])), cmd_init_code_StreamOn[0],0);  
                printf("stream on");
            #else
                sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_ON); 
                sensorDoFuncInit();                                                                
                fpvsusp = 1;                                                            
            #endif    
	    	printf("senResu e");/*Edward 20111126*/                                                      
	    	sensorSuspended = 0;                                                                                         
	    }                                                                                    
	#endif
	return SUCCESS;
}

static UINT32 sensorSuspendExecuteLoad()
{
	#if SUSPEND_ENABLE	    
	    printf("fpvsusp = %d \n", fpvsusp);
	    printf("sensorSuspended = %d \n", sensorSuspended);
        if(fpvsusp == 0) return SUCCESS;
		if(sensorSuspended ==0){
            /* sensorSuspendExecute() */  
	    	printf("senSusp s");/*Edward 20111126*/
            #if 0
            sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code_StreamOff)/sizeof(cmd_init_code_StreamOff[0])), cmd_init_code_StreamOff[0],0);    printf("stream off")
            #else
                sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_OFF);
            #endif    
	    	printf("senSusp e");/*Edward 20111126*/
            sensorSuspended = 1;            
	    }                           
	#endif
	return SUCCESS;
}

void
sensor_dump_reg(void)
{
    UINT32 i, addr;
    addr = 0x0168;
    for(i=0; i<4; i++){
        SONYsensorRead(addr+i);	
    }   
    addr = 0x0268;
    for(i=0; i<4; i++){
        SONYsensorRead(addr+i);	
    }  
    addr = 0x0340;
    for(i=0; i<16; i++){
        SONYsensorRead(addr+i);	
    }  
    
    addr = 0x0381;
    for(i=0; i<8; i++){
        SONYsensorRead(addr+i);	
        i++;
    }  
    
    addr = 0x0401;
    for(i=0; i<8; i++){
        SONYsensorRead(addr+i);
        i++;	
    }  

    SONYsensorRead(0x0175);
    SONYsensorRead(0x0275);
    SONYsensorRead(0x0390);
    SONYsensorRead(0x0404);
}
