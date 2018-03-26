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

#define MODEL_VERSION "$unp_tg_ov16820-0.00.00"

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

/*#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)*/

/* Sensor image size definition here */
#define NO                      0  
#define BY_PIXEL                1  
#define BY_HD                   0
#define NOUSE                   0
#define DISABLE            0xFFFF
#define LANE_2_EN               0 
#define MASK_EXP_CMD            0 
#define CAP_FRAME_MODE          0        /* Frame mode or Rolling mode */  

#if 0 
    /*Reak kit*/
    /*#define SENSOR_FREX     BIT6*/          /* for A900 */
	#define SENSOR_FREX     (1<<15)           /* for EVB */                  
    #define SENSOR_PWRDN    (1<<14)                                          
    #define SENSOR_RESET    (1<<12)                                          
    #define OUT_SENSOR_GPIO_MASK SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET       
    #define CFG_SENSOR_GPIO_DIR SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET        
    #define SENOR_GPIO_GRP SP5K_GPIO_GRP_LMI                                 
#else
    /*6330 EVB*/
#endif

/*****TODO***multiple preview mode for VGA*/
/* For  preview size 640*480, 120fps */
#define PREVIEW_VGA_HSIZE  		 640  			
#define PREVIEW_VGA_VSIZE  		 480  			

/*For preview size, 1920*1080, 30fps*/
#define PREVIEW_1080P_HSIZE  	1920    		
#define PREVIEW_1080P_VSIZE  	1080           

/* For  preview size 1280*720, 60fps, cropping */
#define PREVIEW_720P_HSIZE  	1280           
#define PREVIEW_720P_VSIZE  	 720           

/* For  preview size 320*240, 240fps */
#define PREVIEW_QVGA_HSIZE  	 320  			
#define PREVIEW_QVGA_VSIZE  	 240  			

/* For  preview size 2304*1728, 30fps */
#define PREVIEW_4M_HSIZE  		2304  			
#define PREVIEW_4M_VSIZE  		1728  			

/* For 16M output. */
#define CAPTURE_HSIZE  			4608				
#define CAPTURE_VSIZE  			3456				

/* Preview default definition here */
/* For  preview size 640*480, 30fps */
#define PREVIEW_VGA_LINETOTAL                        (640*2)	                      
#define PREVIEW_VGA_LINEBLANK                        (6)		                      
#define PREVIEW_VGA_FRAMETOTAL                       (PREVIEW_VGA_VSIZE)	                     
#define PREVIEW_VGA_FRAMEBLANK                       (PREVIEW_VGA_VSIZE+1)		                 
#define PREVIEW_VGA_HOFFSET                          (0x60)		                  
#define PREVIEW_VGA_VOFFSET                          (11)                         
#define PREVIEW_VGA_HRESHAPE_FALL                    (0x40) 	                      
#define PREVIEW_VGA_HRESHAPE_RISE                    (0x80)	                      
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL             (BY_HD)	                      
#define PREVIEW_VGA_VRESHAPE_FALL                    (PREVIEW_VGA_VOFFSET-2) 	                  
#define PREVIEW_VGA_VRESHAPE_RISE                    (PREVIEW_VGA_VOFFSET-1)                     

/*For preview 1080P size, 1920*1080, 30fps*/
#define PREVIEW_1080P_LINETOTAL                      (1920*4)                      
#define PREVIEW_1080P_LINEBLANK                      (6)		                
#define PREVIEW_1080P_FRAMETOTAL                     (PREVIEW_1080P_VSIZE)	                    
#define PREVIEW_1080P_FRAMEBLANK                     (PREVIEW_1080P_VSIZE+1)	                    
#define PREVIEW_1080P_HOFFSET                        (0x60)    	                
#define PREVIEW_1080P_VOFFSET                        (11)	                    
#define PREVIEW_1080P_HRESHAPE_FALL                  (0x40) 	                    
#define PREVIEW_1080P_HRESHAPE_RISE                  (0x80)	                    
#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL           (BY_HD)	                    
#define PREVIEW_1080P_VRESHAPE_FALL                  (PREVIEW_1080P_VOFFSET-2)                       
#define PREVIEW_1080P_VRESHAPE_RISE                  (PREVIEW_1080P_VOFFSET-1)                       

/*For preview size, 1280*720, 60fps, cropping*/
#define PREVIEW_720P_LINETOTAL                       (1280*4)                       
#define PREVIEW_720P_LINEBLANK                       (6)		                         
#define PREVIEW_720P_FRAMETOTAL                      (PREVIEW_720P_VSIZE)	         
#define PREVIEW_720P_FRAMEBLANK                      (PREVIEW_720P_VSIZE+1)	         
#define PREVIEW_720P_HOFFSET                         (0x60)                           
#define PREVIEW_720P_VOFFSET                         (11)		                     
#define PREVIEW_720P_HRESHAPE_FALL                   (0x40)                     
#define PREVIEW_720P_HRESHAPE_RISE                   (0x80)	                   
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL            (BY_HD)	                   
#define PREVIEW_720P_VRESHAPE_FALL                   (PREVIEW_720P_VOFFSET-2)  
#define PREVIEW_720P_VRESHAPE_RISE                   (PREVIEW_720P_VOFFSET-1)   
                                
/* For  preview size 320*240, 30fps */
#define PREVIEW_QVGA_LINETOTAL                       (320*4)                         
#define PREVIEW_QVGA_LINEBLANK                       (6) 		                         
#define PREVIEW_QVGA_FRAMETOTAL                      (PREVIEW_QVGA_VSIZE) 	             
#define PREVIEW_QVGA_FRAMEBLANK                      (PREVIEW_QVGA_VSIZE+1) 	         
#define PREVIEW_QVGA_HOFFSET                         (0x60)                             
#define PREVIEW_QVGA_VOFFSET                         (11)                            
#define PREVIEW_QVGA_HRESHAPE_FALL                   (0x40)                       
#define PREVIEW_QVGA_HRESHAPE_RISE                   (0x80) 	                     
#define PREVIEW_QVGA_VRESHAPE_COUNTER_SEL            (BY_HD) 	                 
#define PREVIEW_QVGA_VRESHAPE_FALL                   (PREVIEW_QVGA_VOFFSET-2)     
#define PREVIEW_QVGA_VRESHAPE_RISE                   (PREVIEW_QVGA_VOFFSET-1)      

/*For preview 4M size, 2304*1728, 30fps*/
#define PREVIEW_4M_LINETOTAL                         (2560)	                                         
#define PREVIEW_4M_LINEBLANK                         (6)		                                     
#define PREVIEW_4M_FRAMETOTAL                        (PREVIEW_4M_VSIZE)	                             
#define PREVIEW_4M_FRAMEBLANK                        (PREVIEW_4M_VSIZE+1)	                         
#define PREVIEW_4M_HOFFSET                           (0x60)   	                                     
#define PREVIEW_4M_VOFFSET                           (11)		                                     
#define PREVIEW_4M_HRESHAPE_FALL                     (0x40) 	                                     
#define PREVIEW_4M_HRESHAPE_RISE                     (0x80)	                                         
#define PREVIEW_4M_VRESHAPE_COUNTER_SEL              (BY_HD)	                                     
#define PREVIEW_4M_VRESHAPE_FALL                     (PREVIEW_4M_VOFFSET-2)                          
#define PREVIEW_4M_VRESHAPE_RISE                     (PREVIEW_4M_VOFFSET-1)                          
                                                                                                     
/* Capture default definition here */                                                                
#define CAPTURE_LINETOTAL                            (CAPTURE_HSIZE*2)                               
#define CAPTURE_LINEBLANK                            (6)				                             
#define CAPTURE_FRAMETOTAL                           (CAPTURE_VSIZE)		                         
#define CAPTURE_FRAMEBLANK                           (CAPTURE_VSIZE+1)                               
#define CAPTURE_HOFFSET                              (0x60)                                          
#define CAPTURE_VOFFSET                              (11)                                            
#define CAPTURE_HRESHAPE_FALL                        (0x0300)                                        
#define CAPTURE_HRESHAPE_RISE                        (0x0400)                                        
#define CAPTURE_VRESHAPE_COUNTER_SEL                 (BY_HD)                                         
#define CAPTURE_VRESHAPE_FALL		                 (CAPTURE_VOFFSET-2)	                         
#define CAPTURE_VRESHAPE_RISE		                 (CAPTURE_VOFFSET-1)                             

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

#define	PCLKNXSEL_INT	      0x00          
#define	PCLKNXSEL_LVDS	      0x01         
#define	PCLKNXSEL_EXT	      0x02         
                                           
#define	PCLK1XSEL_NX	      0x00         
#define	PCLK1XSEL_NX_2	      0x04         
#define	PCLK1XSEL_INT	      0x08         

#define PREVIEW_VGA_PCLK     48000
#define PREVIEW_1080P_PCLK   54000
#define CAPTURE_PCLK         24000

#define RGBADJ		             0	      /*RGB input data sample phase adj "1"negedge "0"posedge*/

#if PCLKMODE
	#define PCLK_SRC             0        /* '0': from master clock, '1' from LVDS, '2' from external , '3' from external divided by 2 */
#else
	#define PCLK_SRC             2
#endif
/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	                /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                              TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			TG_CMOS_SP5K_SLAVE          /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,         
						                              TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/                                   
                                                    /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/                     
#define PREVIEW_QVGA_CFAPATTERN     BAYER_GBRG
#define PREVIEW_VGA_CFAPATTERN      BAYER_GBRG
#define PREVIEW_720P_CFAPATTERN     BAYER_GRBG
#define	PREVIEW_1080P_CFAPATTERN	BAYER_GBRG  
#define	PREVIEW_4M_CFAPATTERN	    BAYER_BGGR 
#define	SNAP_CFAPATTERN		        BAYER_BGGR      /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/           
#define	SNAP_DUMMYVD_NUM	               0        /*snap dummy Vd need numbers*/                              
#define	SNAP_READOUTVD_NUM		           1        /*snap readout Vd need numbers*/                                
#define	SNAP_CAPTUREVD_NUM		           1        /*for typical CCD==n_dummy + n_readout + 1*/                    

#define	SNAP_EARLY_NUM		               1	    /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM	           1	    /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	               0		/*It arrange the start field data to the start position of the sdram,0,1,2 */              
#define	FLDSEL_END_NUM		               0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */                  
#define	SKIP_SNAP_READOUTVD_EN             0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN       2	    /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	               0		/*sony FALSE;panasonic/ITG TRUE*/
#define	EXP_FRAME_NUM       	           2	    /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/ 

#define	CAPTURE_HDNUM_FRAME_START        110	    /*the front interrupt line number of the snap new frame */
#define	PREVIEW_HDNUM_FRAME_START        110		/*the front interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_FRAME_END		   2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_VGA_HDNUM_CMDPORT		 400		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_FRAME_END	   2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080P_HDNUM_CMDPORT		 300		/*3281*//*976*/		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_FRAME_END	   2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_720P_HDNUM_CMDPORT		 400		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_FRAME_END	   2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_QVGA_HDNUM_CMDPORT		 200		/*the command port interrupt line number of the preview new frame*/
#define	PREVIEW_4M_HDNUM_FRAME_END	       2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_4M_HDNUM_CMDPORT		 500		/*the command port interrupt line number of the preview new frame*/
#define	CAPTURE_HDNUM_FRAME_END	 	       2 	  	/*the rear interrupt line number of the snap new frame*/               
#define	CAPTURE_HDNUM_CMDPORT		    2000  	    /*the command port interrupt line number of the snap new frame*/       
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
#define	TG_SPI_I2C_MODE  0x6c     	                                         /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	    0		                                             /*don't change*/
#if 0/*SPCA5210*/
	#define	TG_SPEED         FRONT_SSC_CLK_192K
#else
	#define	TG_SPEED    	 FRONT_SSC_CLK_384K    /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						                             FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA     8		               /*ssc/spi register data numbers */           
#define	TG_NBIT_REGADDR    16		               /*ssc/spi register address numbers */        
/*prototypes of sensor model methods*/
extern UINT32 sensorModeGet(void);
extern UINT32 sensor_event_wait(UINT32 event);


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
static void sensorDoFuncPreview4MBegin();
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
static UINT32 sensorResumeLoad();
static UINT32 sensorSuspendLoad();

static const UINT8 afe_init_code_SWReset[][3]={
{0x01,0x03,0x01}
};

static const UINT8 afe_init_code[][3]={
#if 0
	/*preview size 640*480*/                 
    {0x03,0x00,0x02},                        
    /*{0x03,0x02,0x64},*/                    
    {0x03,0x02,0x5a},                        
    {0x03,0x05,0x04},                        
    {0x03,0x06,0x00},                        
    {0x03,0x0b,0x02},                        
    {0x03,0x0c,0x14},                        
    {0x03,0x0e,0x01},                        
    {0x03,0x13,0x02},                        
    {0x03,0x14,0x14},                        
    {0x03,0x1f,0x00},                        
    {0x30,0x18,0x72},                        
    {0x30,0x22,0x01},                        
    {0x30,0x31,0x0a},                        
    {0x30,0x32,0x80},                        
    {0x31,0x06,0x01},                        
    {0x35,0x00,0x00},                        
    {0x35,0x01,0x1f},                        
    {0x35,0x02,0x40},                        
    {0x35,0x08,0x0c},                        
    {0x35,0x09,0xff},                        
    {0x36,0x01,0x88},                        
    {0x36,0x02,0x00},                        
    {0x36,0x03,0x04},                        
    {0x36,0x04,0x02},                        
    {0x36,0x05,0x50},                        
    {0x36,0x06,0x00},                        
    {0x36,0x07,0x2b},                        
    {0x36,0x08,0x12},                        
    {0x36,0x09,0x00},                        
    {0x36,0x0a,0x02},                        
    {0x36,0x0b,0x02},                        
    {0x36,0x0c,0x10},                        
    {0x36,0x0d,0x01},                        
    {0x36,0x12,0x20},                        
    {0x36,0x13,0x88},                        
    {0x36,0x14,0x77},                        
    {0x36,0x15,0xf4},                        
    {0x36,0x16,0x34},                        
    {0x36,0x17,0x00},                        
    {0x36,0x18,0x20},                        
    {0x36,0x19,0x00},                        
    {0x36,0x1a,0x10},                        
    {0x36,0x1d,0x00},                        
    {0x36,0x1e,0x00},                        
    {0x36,0x31,0x60},                        
    {0x36,0x4a,0x06},                        
    {0x37,0x00,0x40},                        
    {0x37,0x01,0x08},                        
    {0x37,0x02,0x11},                        
    {0x37,0x03,0x20},                        
    {0x37,0x04,0x08},                        
    {0x37,0x05,0x00},                        
    {0x37,0x06,0x84},                        
    {0x37,0x07,0x08},                        
    {0x37,0x08,0x20},                        
    {0x37,0x09,0x3c},                        
    {0x37,0x0a,0x01},                        
    {0x37,0x0b,0x5d},                        
    {0x37,0x0c,0x03},                        
    {0x37,0x0e,0x20},                        
    {0x37,0x0f,0x05},                        
    {0x37,0x10,0x20},                        
    {0x37,0x11,0x0e},                        
    {0x37,0x14,0x31},                        
    {0x37,0x18,0x75},                        
    {0x37,0x19,0x13},                        
    {0x37,0x1a,0x55},                        
    {0x37,0x1b,0x03},                        
    {0x37,0x1c,0x55},                        
    {0x37,0x1d,0x03},                        
    {0x37,0x1e,0x09},                        
    {0x37,0x1f,0x16},                        
    {0x37,0x20,0x0b},                        
    {0x37,0x21,0x18},                        
    {0x37,0x22,0x0b},                        
    {0x37,0x23,0x18},                        
    {0x37,0x24,0x04},                        
    {0x37,0x25,0x04},                        
    {0x37,0x26,0x02},                        
    {0x37,0x27,0x02},                        
    {0x37,0x28,0x02},                        
    {0x37,0x29,0x02},                        
    {0x37,0x2a,0x05},                        
    {0x37,0x2b,0x65},                        
    {0x37,0x2c,0x55},                        
    {0x37,0x2d,0x65},                        
    {0x37,0x2e,0x53},                        
    {0x37,0x2f,0x33},                        
    {0x37,0x30,0x33},                        
    {0x37,0x31,0x33},                        
    {0x37,0x32,0x03},                        
    {0x37,0x33,0x80},                        
    {0x37,0x34,0x10},                        
    {0x37,0x39,0x03},                        
    {0x37,0x3a,0x20},                        
    {0x37,0x3b,0x0c},                        
    {0x37,0x3c,0x1c},                        
    {0x37,0x3e,0x18},                        
    {0x37,0x60,0x05},                        
    {0x37,0x61,0x00},                        
    {0x37,0x62,0x05},                        
    {0x37,0x63,0x00},                        
    {0x37,0x64,0x05},                        
    {0x37,0x65,0x00},                        
    {0x3f,0x08,0x20},                        
    {0x38,0x00,0x01},                        
    {0x38,0x01,0x80},                        
    {0x38,0x02,0x01},                        
    {0x38,0x03,0x24},                        
    {0x38,0x04,0x11},                        
    {0x38,0x05,0x3f},                        
    {0x38,0x06,0x0c},                        
    {0x38,0x07,0x7b},                        
    {0x38,0x08,0x02},                        
    {0x38,0x09,0x80},                        
    {0x38,0x0a,0x01},                        
    {0x38,0x0b,0xe0},                        
    {0x38,0x0c,0x05},                        
    {0x38,0x0d,0x2a},                        
    {0x38,0x0e,0x01},                        
    {0x38,0x0f,0xf8},                        
    {0x38,0x11,0x08},                        
    {0x38,0x13,0x00},                        
    {0x38,0x14,0x09},                        
    {0x38,0x15,0x03},                        
    {0x38,0x20,0x00},                        
    {0x38,0x21,0x01},                        
    {0x38,0x23,0x08},                        
    {0x38,0x28,0x00},                        
    {0x38,0x29,0x00},                        
    {0x38,0x2a,0x0b},                        
    {0x38,0x2b,0x01},                        
    {0x38,0x30,0x06},                        
    {0x38,0x32,0x00},                        
    {0x38,0x33,0x00},                        
    {0x38,0x34,0x00},                        
    {0x3f,0x00,0x02},                        
    {0x40,0x01,0x83},                        
    {0x40,0x02,0x04},                        
    {0x40,0x03,0x02},                        
    {0x40,0x0e,0x00},                        
    {0x40,0x11,0x00},                        
    {0x40,0x12,0x00},                        
    {0x42,0x00,0x08},                        
    {0x43,0x02,0x7f},                        
    {0x43,0x03,0xff},                        
    {0x43,0x04,0x00},                        
    {0x43,0x05,0x00},                        
    {0x45,0x01,0x30},                        
    {0x46,0x03,0x10},                        
    {0x48,0x37,0x14},                        
    {0x4b,0x00,0x22},                        
    {0x49,0x03,0x00},                        
    {0x50,0x00,0x53},                        
    {0x50,0x01,0x01},                        
    {0x50,0x04,0x00},                        
    {0x50,0x13,0x20},                        
    {0x50,0x51,0x00},                        
    {0x59,0x80,0x00},                        
    {0x59,0x81,0x00},                        
    {0x59,0x82,0x00},                        
    {0x59,0x83,0x00},                        
    {0x59,0x84,0x00},                        
    {0x59,0x85,0x00},                        
    {0x59,0x86,0x00},                        
    {0x59,0x87,0x00},                        
    {0x59,0x88,0x00},                        
    {0x01,0x00,0x01},                        
#else
    
    /*preview size 2304x1728 */
    {0x03,0x00,0x02},                                                      
    {0x03,0x02,0x5a},                                                      
    {0x03,0x05,0x04},                                                      
    {0x03,0x06,0x00},                                                      
    {0x03,0x0b,0x02},                                                      
    {0x03,0x0c,0x14},                                                      
    {0x03,0x0e,0x01},                                                      
    {0x03,0x13,0x02},                                                      
    {0x03,0x14,0x14},                                                      
    {0x03,0x1f,0x00},                                                      
    #if LANE_2_EN                                                          
    	{0x30,0x18,0x32}, /*2 lane mode*/                          
    #else                                                                  
        {0x30,0x18,0x72}, /*4 lane mode*/                          
    #endif                                                                 
    {0x30,0x22,0x01},                                                      
    {0x30,0x31,0x0a},                                                      
    {0x30,0x32,0x80},                                                      
    {0x31,0x06,0x01},                                                      
    {0x35,0x00,0x00},                                                      
    {0x35,0x01,0x1f},                                                      
    {0x35,0x02,0x00},                                                      
    {0x35,0x08,0x0c},                                                      
    {0x35,0x09,0xff},                                                      
    {0x36,0x01,0x88},                                                      
    {0x36,0x02,0x00},                                                      
    {0x36,0x03,0x05},                                                      
    {0x36,0x04,0x02},                                                      
    {0x36,0x05,0x50},                                                      
    {0x36,0x06,0x00},                                                      
    {0x36,0x07,0x2b},                                                      
    {0x36,0x08,0x12},                                                      
    {0x36,0x09,0x00},                                                      
    {0x36,0x0a,0x02},                                                      
    {0x36,0x0b,0x02},                                                      
    {0x36,0x0c,0x10},                                                      
    {0x36,0x0d,0x05},                                                      
    {0x36,0x12,0x20},                                                      
    {0x36,0x13,0x88},                                                      
    {0x36,0x14,0x77},                                                      
    {0x36,0x15,0xf4},                                                      
    {0x36,0x16,0x31},                                                      
    {0x36,0x17,0x00},                                                      
    {0x36,0x18,0x20},                                                      
    {0x36,0x19,0x00},                                                      
    {0x36,0x1a,0x10},                                                      
    {0x36,0x1d,0x00},                                                      
    {0x36,0x1e,0x00},                                                      
    {0x36,0x31,0x60},                                                      
    {0x36,0x4a,0x06},                                                      
    {0x37,0x00,0x30},                                                      
    {0x37,0x01,0x08},                                                      
    {0x37,0x02,0x11},                                                      
    {0x37,0x03,0x20},                                                      
    {0x37,0x04,0x08},                                                      
    {0x37,0x05,0x00},                                                      
    {0x37,0x06,0x84},                                                      
    {0x37,0x07,0x08},                                                      
    {0x37,0x08,0x20},                                                      
    {0x37,0x09,0x3c},                                                      
    {0x37,0x0a,0x01},                                                      
    {0x37,0x0b,0x5d},                                                      
    {0x37,0x0c,0x03},                                                      
    {0x37,0x0e,0x20},                                                      
    {0x37,0x0f,0x05},                                                      
    {0x37,0x10,0x20},                                                      
    {0x37,0x11,0x20},                                                      
    {0x37,0x14,0x31},                                                      
    {0x37,0x18,0x75},                                                      
    {0x37,0x19,0x13},                                                      
    {0x37,0x1a,0x55},                                                      
    {0x37,0x1b,0x03},                                                      
    {0x37,0x1c,0x55},                                                      
    {0x37,0x1d,0x03},                                                      
    {0x37,0x1e,0x09},                                                      
    {0x37,0x1f,0x16},                                                      
    {0x37,0x20,0x0b},                                                      
    {0x37,0x21,0x18},                                                      
    {0x37,0x22,0x0b},                                                      
    {0x37,0x23,0x18},                                                      
    {0x37,0x24,0x04},                                                      
    {0x37,0x25,0x04},                                                      
    {0x37,0x26,0x02},                                                      
    {0x37,0x27,0x02},                                                      
    {0x37,0x28,0x02},                                                      
    {0x37,0x29,0x02},                                                      
    {0x37,0x2a,0x05},                                                      
    {0x37,0x2b,0x65},                                                      
    {0x37,0x2c,0x55},                                                      
    {0x37,0x2d,0x65},                                                      
    {0x37,0x2e,0x53},                                                      
    {0x37,0x2f,0x33},                                                      
    {0x37,0x30,0x33},                                                      
    {0x37,0x31,0x33},                                                      
    {0x37,0x32,0x03},                                                      
    {0x37,0x33,0x80},                                                      
    {0x37,0x34,0x10},                                                      
    {0x37,0x39,0x03},                                                      
    {0x37,0x3a,0x20},                                                      
    {0x37,0x3b,0x0c},                                                      
    {0x37,0x3c,0x1c},                                                      
    {0x37,0x3e,0x18},                                                      
    {0x37,0x60,0x05},                                                      
    {0x37,0x61,0x00},                                                      
    {0x37,0x62,0x05},                                                      
    {0x37,0x63,0x00},                                                      
    {0x37,0x64,0x05},                                                      
    {0x37,0x65,0x00},                                                      
    {0x3f,0x08,0x20},                                                      
    {0x38,0x00,0x00},                                                      
    {0x38,0x01,0x00},                                                      
    {0x38,0x02,0x00},                                                      
    {0x38,0x03,0x0C},                                                      
    {0x38,0x04,0x12},                                                      
    {0x38,0x05,0x3f},                                                      
    {0x38,0x06,0x0d},                                                      
    {0x38,0x07,0x97},                                                      
    {0x38,0x08,0x09},                                                      
    {0x38,0x09,0x00},                                                      
    {0x38,0x0a,0x06},                                                      
    {0x38,0x0b,0xc0},                                                      
    {0x38,0x0c,0x05},                                                      
    {0x38,0x0d,0xf0}, 
    
    {0x38,0x0e,0x06},                                                      
    {0x38,0x0f,0xda}, 
    
    {0x38,0x11,0x17},                                                      
    {0x38,0x13,0x02},                                                      
    {0x38,0x14,0x03},                                                      
    {0x38,0x15,0x01},                                                      
    {0x38,0x20,0x00},                                                      
    {0x38,0x21,0x06},                                                      
    #if 0                                                                  
    	{0x38,0x23,0x08},                                                      
    	{0x38,0x28,0x00},                                                      
    #endif                                                                 
    {0x38,0x29,0x00},                                                      
    {0x38,0x2a,0x03},                                                      
    {0x38,0x2b,0x01},                                                      
    {0x38,0x30,0x08},                                                      
    {0x38,0x32,0x00},                                                      
    {0x38,0x33,0x00},                                                      
    {0x38,0x34,0x00},                                                      
    {0x3f,0x00,0x02},                                                      
    {0x40,0x01,0x83},                                                      
    {0x40,0x02,0x02},                                                      
    {0x40,0x03,0x04},                                                      
    {0x40,0x0e,0x00},                                                      
    {0x40,0x11,0x00},                                                      
    {0x40,0x12,0x00},                                                      
    {0x42,0x00,0x08},                                                      
    {0x43,0x02,0x7f},                                                      
    {0x43,0x03,0xff},                                                      
    {0x43,0x04,0x00},                                                      
    {0x43,0x05,0x00},                                                      
    {0x45,0x01,0x30},                                                      
    {0x46,0x03,0x10},                                                      
    {0x48,0x37,0x28},                                                      
    {0x4b,0x00,0x22},                                                      
    {0x49,0x03,0x00},                                                      
    {0x50,0x00,0x53},                                                      
    {0x50,0x01,0x01},                                                      
    /*{0x50,0x04,0x00},*/                                                  
    {0x50,0x13,0x20},                                                      
    {0x50,0x51,0x00},                                                      
    {0x59,0x80,0x00},                                                      
    {0x59,0x81,0x00},                                                      
    {0x59,0x82,0x00},                                                      
    {0x59,0x83,0x00},                                                      
    {0x59,0x84,0x00},                                                      
    {0x59,0x85,0x00},                                                      
    {0x59,0x86,0x00},                                                      
    {0x59,0x87,0x00},                                                      
    {0x59,0x88,0x00},                                                      
    {0x01,0x00,0x01},                                                      
#endif

/*Digital Gain Enable*/
	{0x50,0x04,0x80},

#if 1
	/*frex setting*/
	{0x30,0x02,0x20},       /*frex input */
	{0x3B,0x9F,0x22},       /*frex_pad_in_enable */
                       		/*
                            Bit[7]:      frex_sccb_req (self-clearing)
							Bit[6]:      Debug mode
							Bit[5]:      frex_strobe_out_sel                                   
                                 0:      Strobe for rolling mode                                     
                                 1:      Strobe for frame mode                                       
                            Bit[4]:      frex_nopchg     
                            -----------------------------
                            Bit[3]:      frex_strobe polarity                                  
                            	 0:      Active high                                                 
                                 1:      Active low                                                  
                            Bit[2]:      frex_shutter polarity                                 
                            Bit[1]:      frex_pad_in_enable                                    
                                 0:      Frame mode is triggered by register                                     
                                 1:      Frame mode is triggered by FREX pad                                                        
                            Bit[0]:      no_latch at SOF for frex_sccb_req                                   
                                 0:      Trigger frame mode in SOF                                                                
                                 1:      Trigger frame mode immediately 
                            */
#endif
};

static const UINT8 afe_vga_code[][3]={
	/*preview size 640*480*/                                      
    {0x03,0x00,0x02},                                             
    {0x03,0x02,0x5a},                                             
    {0x03,0x05,0x04},                                             
    {0x03,0x06,0x00},                                             
    {0x03,0x0b,0x02},                                             
    {0x03,0x0c,0x14},                                             
    {0x03,0x0e,0x01},                                             
    {0x03,0x13,0x02},                                             
    {0x03,0x14,0x14},                                             
    {0x03,0x1f,0x00},                                             
    #if LANE_2_EN                                                 
    	{0x30,0x18,0x32}, /*2 lane mode*/                         
    #else                                                         
    	{0x30,0x18,0x72}, /*4 lane mode*/                         
    #endif                                                        
    {0x30,0x22,0x01},                                             
    {0x30,0x31,0x0a},                                             
    {0x30,0x32,0x80},                                             
    {0x31,0x06,0x01},                                             
    {0x35,0x00,0x00},                                             
    {0x35,0x01,0x1f},                                             
    {0x35,0x02,0x40},                                             
    {0x35,0x08,0x0c},                                             
    {0x35,0x09,0xff},                                             
    {0x36,0x01,0x88},                                             
    {0x36,0x02,0x00},                                             
    {0x36,0x03,0x04},                                             
    {0x36,0x04,0x02},                                             
    {0x36,0x05,0x50},                                             
    {0x36,0x06,0x00},                                             
    {0x36,0x07,0x2b},                                             
    {0x36,0x08,0x12},                                             
    {0x36,0x09,0x00},                                             
    {0x36,0x0a,0x02},                                             
    {0x36,0x0b,0x02},                                             
    {0x36,0x0c,0x10},                                             
    {0x36,0x0d,0x01},                                             
    {0x36,0x12,0x20},                                             
    {0x36,0x13,0x88},                                             
    {0x36,0x14,0x77},                                             
    {0x36,0x15,0xf4},                                             
    {0x36,0x16,0x34},                                             
    {0x36,0x17,0x00},                                             
    {0x36,0x18,0x20},                                             
    {0x36,0x19,0x00},                                             
    {0x36,0x1a,0x10},                                             
    {0x36,0x1d,0x00},                                             
    {0x36,0x1e,0x00},                                             
    {0x36,0x31,0x60},                                             
    {0x36,0x4a,0x06},                                             
    {0x37,0x00,0x40},                                             
    {0x37,0x01,0x08},                                             
    {0x37,0x02,0x11},                                             
    {0x37,0x03,0x20},                                             
    {0x37,0x04,0x08},                                             
    {0x37,0x05,0x00},                                             
    {0x37,0x06,0x84},                                             
    {0x37,0x07,0x08},                                             
    {0x37,0x08,0x20},                                             
    {0x37,0x09,0x3c},                                             
    {0x37,0x0a,0x01},                                             
    {0x37,0x0b,0x5d},                                             
    {0x37,0x0c,0x03},                                             
    {0x37,0x0e,0x20},                                             
    {0x37,0x0f,0x05},                                             
    {0x37,0x10,0x20},                                             
    {0x37,0x11,0x0e},                                             
    {0x37,0x14,0x31},                                             
    {0x37,0x18,0x75},                                             
    {0x37,0x19,0x13},                                             
    {0x37,0x1a,0x55},                                             
    {0x37,0x1b,0x03},                                             
    {0x37,0x1c,0x55},                                             
    {0x37,0x1d,0x03},                                             
    {0x37,0x1e,0x09},                                             
    {0x37,0x1f,0x16},                                             
    {0x37,0x20,0x0b},                                             
    {0x37,0x21,0x18},                                             
    {0x37,0x22,0x0b},                                             
    {0x37,0x23,0x18},                                             
    {0x37,0x24,0x04},                                             
    {0x37,0x25,0x04},                                             
    {0x37,0x26,0x02},                                             
    {0x37,0x27,0x02},                                             
    {0x37,0x28,0x02},                                             
    {0x37,0x29,0x02},                                             
    {0x37,0x2a,0x05},                                             
    {0x37,0x2b,0x65},                                             
    {0x37,0x2c,0x55},                                             
    {0x37,0x2d,0x65},                                             
    {0x37,0x2e,0x53},                                             
    {0x37,0x2f,0x33},                                             
    {0x37,0x30,0x33},                                             
    {0x37,0x31,0x33},                                             
    {0x37,0x32,0x03},                                             
    {0x37,0x33,0x80},                                             
    {0x37,0x34,0x10},                                             
    {0x37,0x39,0x03},                                             
    {0x37,0x3a,0x20},                                             
    {0x37,0x3b,0x0c},                                             
    {0x37,0x3c,0x1c},                                             
    {0x37,0x3e,0x18},                                             
    {0x37,0x60,0x05},                                             
    {0x37,0x61,0x00},                                             
    {0x37,0x62,0x05},                                             
    {0x37,0x63,0x00},                                             
    {0x37,0x64,0x05},                                             
    {0x37,0x65,0x00},                                             
    {0x3f,0x08,0x20},                                             
    {0x38,0x00,0x01},                                             
    {0x38,0x01,0x80},                                             
    {0x38,0x02,0x01},                                             
    {0x38,0x03,0x24},                                             
    {0x38,0x04,0x11},                                             
    {0x38,0x05,0x3f},                                             
    {0x38,0x06,0x0c},                                             
    {0x38,0x07,0x7b},                                             
    {0x38,0x08,0x02},                                             
    {0x38,0x09,0x80},                                             
    {0x38,0x0a,0x01},                                             
    {0x38,0x0b,0xe0},                                             
    {0x38,0x0c,0x05},                                             
    {0x38,0x0d,0x2a},                                             
    {0x38,0x0e,0x01},                                             
    {0x38,0x0f,0xf8},                                             
    {0x38,0x11,0x08},                                             
    {0x38,0x13,0x00},                                             
    {0x38,0x14,0x09},                                             
    {0x38,0x15,0x03},                                             
    {0x38,0x20,0x00},                                             
    {0x38,0x21,0x07},                                             
    {0x38,0x23,0x08},                                             
    {0x38,0x28,0x00},                                             
    {0x38,0x29,0x00},                                             
    {0x38,0x2a,0x0b},                                             
    {0x38,0x2b,0x01},                                             
    {0x38,0x30,0x06},                                             
    {0x38,0x32,0x00},                                             
    {0x38,0x33,0x00},                                             
    {0x38,0x34,0x00},                                             
    {0x3f,0x00,0x02},                                             
    {0x40,0x01,0x83},                                             
    {0x40,0x02,0x04},                                             
    {0x40,0x03,0x02},                                             
    {0x40,0x0e,0x00},                                             
    {0x40,0x11,0x00},                                             
    {0x40,0x12,0x00},                                             
    {0x42,0x00,0x08},                                             
    {0x43,0x02,0x7f},                                             
    {0x43,0x03,0xff},                                             
    {0x43,0x04,0x00},                                             
    {0x43,0x05,0x00},                                             
    {0x45,0x01,0x30},                                             
    {0x46,0x03,0x10},                                             
    {0x48,0x37,0x14},                                             
    {0x4b,0x00,0x22},                                             
    {0x49,0x03,0x00},                                             
    {0x50,0x00,0x53},                                             
    {0x50,0x01,0x01},                                             
    /*{0x50,0x04,0x00},*/                                         
    {0x50,0x13,0x20},                                             
    {0x50,0x51,0x00},                                             
    {0x59,0x80,0x00},                                             
    {0x59,0x81,0x00},                                             
    {0x59,0x82,0x00},                                             
    {0x59,0x83,0x00},                                             
    {0x59,0x84,0x00},                                             
    {0x59,0x85,0x00},                                             
    {0x59,0x86,0x00},                                             
    {0x59,0x87,0x00},                                             
    {0x59,0x88,0x00},                                             
    {0x01,0x00,0x01},                                             
};

static const UINT8 afe_720P_code[][3]={
#if 0
	/*old*/                                                     
    /*preview size 1280*720*/                                     
    {0x03,0x00,0x02},                                             
    {0x03,0x02,0x5a},                                             
    {0x03,0x05,0x04},                                             
    {0x03,0x06,0x00},                                             
    {0x03,0x0b,0x02},                                             
    {0x03,0x0c,0x14},                                             
    {0x03,0x0e,0x01},                                             
    {0x03,0x13,0x02},                                             
    {0x03,0x14,0x14},                                             
    {0x03,0x1f,0x00},                                             
    {0x30,0x18,0x72},                                             
    {0x30,0x22,0x01},                                             
    {0x30,0x31,0x0a},                                             
    {0x30,0x32,0x80},                                             
    {0x31,0x06,0x01},                                             
    {0x35,0x00,0x00},                                             
    {0x35,0x01,0x1f},                                             
    {0x35,0x02,0x40},                                             
    {0x35,0x08,0x0c},                                             
    {0x35,0x09,0xff},                                             
    {0x36,0x01,0x88},                                             
    {0x36,0x02,0x00},                                             
    {0x36,0x03,0x04},                                             
    {0x36,0x04,0x02},                                             
    {0x36,0x05,0x50},                                             
    {0x36,0x06,0x00},                                             
    {0x36,0x07,0x2b},                                             
    {0x36,0x08,0x12},                                             
    {0x36,0x09,0x00},                                             
    {0x36,0x0a,0x00},                                             
    {0x36,0x0b,0x02},                                             
    {0x36,0x0c,0x10},                                             
    {0x36,0x0d,0x01},                                             
    {0x36,0x12,0x20},                                             
    {0x36,0x13,0x88},                                             
    {0x36,0x14,0x77},                                             
    {0x36,0x15,0xf4},                                             
    {0x36,0x16,0x30},                                             
    {0x36,0x17,0x00},                                             
    {0x36,0x18,0x20},                                             
    {0x36,0x19,0x00},                                             
    {0x36,0x1a,0x10},                                             
    {0x36,0x1d,0x00},                                             
    {0x36,0x1e,0x00},                                             
    {0x36,0x31,0x60},                                             
    {0x36,0x4a,0x06},                                             
    {0x37,0x00,0x40},                                             
    {0x37,0x01,0x08},                                             
    {0x37,0x02,0x11},                                             
    {0x37,0x03,0x20},                                             
    {0x37,0x04,0x08},                                             
    {0x37,0x05,0x00},                                             
    {0x37,0x06,0x84},                                             
    {0x37,0x07,0x08},                                             
    {0x37,0x08,0x20},                                             
    {0x37,0x09,0x3c},                                             
    {0x37,0x0a,0x01},                                             
    {0x37,0x0b,0x5d},                                             
    {0x37,0x0c,0x03},                                             
    {0x37,0x0e,0x20},                                             
    {0x37,0x0f,0x05},                                             
    {0x37,0x10,0x20},                                             
    {0x37,0x11,0x0e},                                             
    {0x37,0x14,0x31},                                             
    {0x37,0x18,0x75},                                             
    {0x37,0x19,0x13},                                             
    {0x37,0x1a,0x55},                                             
    {0x37,0x1b,0x03},                                             
    {0x37,0x1c,0x55},                                             
    {0x37,0x1d,0x03},                                             
    {0x37,0x1e,0x09},                                             
    {0x37,0x1f,0x16},                                             
    {0x37,0x20,0x0b},                                             
    {0x37,0x21,0x18},                                             
    {0x37,0x22,0x0b},                                             
    {0x37,0x23,0x18},                                             
    {0x37,0x24,0x04},                                             
    {0x37,0x25,0x04},                                             
    {0x37,0x26,0x02},                                             
    {0x37,0x27,0x02},                                             
    {0x37,0x28,0x02},                                             
    {0x37,0x29,0x02},                                             
    {0x37,0x2a,0x05},                                             
    {0x37,0x2b,0x65},                                             
    {0x37,0x2c,0x55},                                             
    {0x37,0x2d,0x65},                                             
    {0x37,0x2e,0x53},                                             
    {0x37,0x2f,0x33},                                             
    {0x37,0x30,0x33},                                             
    {0x37,0x31,0x33},                                             
    {0x37,0x32,0x03},                                             
    {0x37,0x33,0x80},                                             
    {0x37,0x34,0x10},                                             
    {0x37,0x39,0x03},                                             
    {0x37,0x3a,0x20},                                             
    {0x37,0x3b,0x0c},                                             
    {0x37,0x3c,0x1c},                                             
    {0x37,0x3e,0x18},                                             
    {0x37,0x60,0x05},                                             
    {0x37,0x61,0x00},                                             
    {0x37,0x62,0x05},                                             
    {0x37,0x63,0x00},                                             
    {0x37,0x64,0x05},                                             
    {0x37,0x65,0x00},                                             
    {0x3f,0x08,0x20},                                             
    {0x38,0x00,0x01},                                             
    {0x38,0x01,0x80},                                             
    {0x38,0x02,0x02},                                             
    {0x38,0x03,0x92},                                             
    {0x38,0x04,0x10},                                             
    {0x38,0x05,0xdf},                                             
    {0x38,0x06,0x0b},                                             
    {0x38,0x07,0x0d},                                             
    {0x38,0x08,0x05},                                             
    {0x38,0x09,0x00},                                             
    {0x38,0x0a,0x02},                                             
    {0x38,0x0b,0xd0},                                             
    {0x38,0x0c,0x07},                                             
    {0x38,0x0d,0x00},                                             
    {0x38,0x0e,0x02},                                             
    {0x38,0x0f,0xe8},                                             
    {0x38,0x11,0x10},                                             
    {0x38,0x13,0x00},                                             
    {0x38,0x14,0x03},                                             
    {0x38,0x15,0x03},                                             
    {0x38,0x20,0x01},                                             
    {0x38,0x21,0x00},                                             
    {0x38,0x23,0x08},                                             
    {0x38,0x28,0x00},                                             
    {0x38,0x29,0x03},                                             
    {0x38,0x2a,0x05},                                             
    {0x38,0x2b,0x01},                                             
    {0x38,0x30,0x06},                                             
    {0x38,0x32,0x00},                                             
    {0x38,0x33,0x00},                                             
    {0x38,0x34,0x00},                                             
    {0x3f,0x00,0x02},                                             
    {0x40,0x01,0x83},                                             
    {0x40,0x02,0x01},                                             
    {0x40,0x03,0x02},                                             
    {0x40,0x0e,0x00},                                             
    {0x40,0x11,0x00},                                             
    {0x40,0x12,0x00},                                             
    {0x42,0x00,0x08},                                             
    {0x43,0x02,0x7f},                                             
    {0x43,0x03,0xff},                                             
    {0x43,0x04,0x00},                                             
    {0x43,0x05,0x00},                                             
    {0x45,0x01,0x30},                                             
    {0x46,0x03,0x10},                                             
    {0x48,0x37,0x14},                                             
    {0x4b,0x00,0x22},                                             
    {0x49,0x03,0x00},                                             
    {0x50,0x00,0x53},                                             
    {0x50,0x01,0x01},                                             
    /*{0x50,0x04,0x00},*/                                         
    {0x50,0x13,0x20},                                             
    {0x50,0x51,0x00},                                             
    {0x59,0x80,0x00},                                             
    {0x59,0x81,0x00},                                             
    {0x59,0x82,0x00},                                             
    {0x59,0x83,0x00},                                             
    {0x59,0x84,0x00},                                             
    {0x59,0x85,0x00},                                             
    {0x59,0x86,0x00},                                             
    {0x59,0x87,0x00},                                             
    {0x59,0x88,0x00},                                             
    {0x01,0x00,0x01},                                             
#else                                                         
    /*1214. fix VTS frame rate issue*/                        
    {0x03,0x00,0x02},                                             
    {0x03,0x02,0x5a},                                             
    {0x03,0x05,0x04},                                             
    {0x03,0x06,0x00},                                             
    {0x03,0x0b,0x02},                                             
    {0x03,0x0c,0x14},                                             
    {0x03,0x0e,0x01},                                             
    {0x03,0x13,0x02},                                             
    {0x03,0x14,0x14},                                             
    {0x03,0x1f,0x00},                                             
    {0x30,0x18,0x72},                                             
    {0x30,0x22,0x01},                                             
    {0x30,0x31,0x0a},                                             
    {0x30,0x32,0x80},                                             
    {0x31,0x06,0x01},                                             
    {0x35,0x00,0x00},                                             
    {0x35,0x01,0x2e},                                             
    {0x35,0x02,0x20},                                             
    {0x35,0x08,0x0c},                                             
    {0x35,0x09,0xff},                                             
    {0x36,0x01,0x88},                                             
    {0x36,0x02,0x00},                                             
    {0x36,0x03,0x06},                                             
    {0x36,0x04,0x06},                                             
    {0x36,0x05,0x50},                                             
    {0x36,0x06,0x00},                                             
    {0x36,0x07,0x2b},                                             
    {0x36,0x08,0x16},                                             
    {0x36,0x09,0x00},                                             
    {0x36,0x0a,0x04},                                             
    {0x36,0x0b,0x02},                                             
    {0x36,0x0c,0x10},                                             
    {0x36,0x0d,0x05},                                             
    {0x36,0x0f,0x64},                                             
    {0x36,0x10,0x58},                                             
    {0x36,0x11,0x48},                                             
    {0x36,0x12,0x40},                                             
    {0x36,0x13,0x88},                                             
    {0x36,0x14,0x77},                                             
    {0x36,0x15,0x44},                                             
    {0x36,0x16,0x30},                                             
    {0x36,0x17,0x00},                                             
    {0x36,0x18,0x20},                                             
    {0x36,0x19,0x00},                                             
    {0x36,0x1a,0x10},                                             
    {0x36,0x1c,0x30},                                             
    {0x36,0x1d,0x00},                                             
    {0x36,0x1e,0x00},                                             
    {0x36,0x31,0x60},                                             
    {0x36,0x4a,0x06},                                             
    {0x37,0x00,0x30},                                             
    {0x37,0x01,0x08},                                             
    {0x37,0x02,0x11},                                             
    {0x37,0x03,0x20},                                             
    {0x37,0x04,0x08},                                             
    {0x37,0x05,0x00},                                             
    {0x37,0x06,0x84},                                             
    {0x37,0x07,0x08},                                             
    {0x37,0x08,0x20},                                             
    {0x37,0x09,0x3c},                                             
    {0x37,0x0a,0x01},                                             
    {0x37,0x0b,0x5d},                                             
    {0x37,0x0c,0x03},                                             
    {0x37,0x0e,0x20},                                             
    {0x37,0x0f,0x05},                                             
    {0x37,0x10,0x20},                                             
    {0x37,0x11,0x20},                                             
    {0x37,0x14,0x31},                                             
    {0x37,0x18,0x75},                                             
    {0x37,0x19,0x13},                                             
    {0x37,0x1a,0x55},                                             
    {0x37,0x1b,0x03},                                             
    {0x37,0x1c,0x55},                                             
    {0x37,0x1d,0x03},                                             
    {0x37,0x1e,0x09},                                             
    {0x37,0x1f,0x16},                                             
    {0x37,0x20,0x0b},                                             
    {0x37,0x21,0x18},                                             
    {0x37,0x22,0x0b},                                             
    {0x37,0x23,0x18},                                             
    {0x37,0x24,0x04},                                             
    {0x37,0x25,0x04},                                             
    {0x37,0x26,0x02},                                             
    {0x37,0x27,0x02},                                             
    {0x37,0x28,0x02},                                             
    {0x37,0x29,0x02},                                             
    {0x37,0x2a,0x05},                                             
    {0x37,0x2b,0x65},                                             
    {0x37,0x2c,0x55},                                             
    {0x37,0x2d,0x65},                                             
    {0x37,0x2e,0x53},                                             
    {0x37,0x2f,0x33},                                             
    {0x37,0x30,0x33},                                             
    {0x37,0x31,0x33},                                             
    {0x37,0x32,0x03},                                             
    {0x37,0x33,0x80},                                             
    {0x37,0x34,0x10},                                             
    {0x37,0x39,0x03},                                             
    {0x37,0x3a,0x20},                                             
    {0x37,0x3b,0x0c},                                             
    {0x37,0x3c,0x1c},                                             
    {0x37,0x3e,0x18},                                             
    {0x37,0x60,0x00},                                             
    {0x37,0x61,0x30},                                             
    {0x37,0x62,0x00},                                             
    {0x37,0x63,0xc0},                                             
    {0x37,0x64,0x03},                                             
    {0x37,0x65,0x00},                                             
    {0x3f,0x08,0x20},                                             
    {0x38,0x00,0x01},                                             
    {0x38,0x01,0x80},                                             
    {0x38,0x02,0x02},                                             
    {0x38,0x03,0x92},                                             
    {0x38,0x04,0x10},                                             
    {0x38,0x05,0xe1},                                             
    {0x38,0x06,0x0b},                                             
    {0x38,0x07,0x13},                                             
    {0x38,0x08,0x05},                                             
    {0x38,0x09,0x00},                                             
    {0x38,0x0a,0x02},                                             
    {0x38,0x0b,0xd0},                                             
    {0x38,0x0c,0x07},                                             
    {0x38,0x0d,0x04},                                             
    {0x38,0x0e,0x02},                                             
    {0x38,0x0f,0xe6},                                             
    {0x38,0x11,0x0f},                                             
    {0x38,0x13,0x02},                                             
    {0x38,0x14,0x03},                                             
    {0x38,0x15,0x03},                                             
    {0x38,0x20,0x01},                                             
    {0x38,0x21,0x0e},                                             
    {0x38,0x23,0x08},                                             
    {0x38,0x27,0x02},                                             
    {0x38,0x28,0x00},                                             
    {0x38,0x29,0x03},                                             
    {0x38,0x2a,0x05},                                             
    {0x38,0x2b,0x01},                                             
    {0x38,0x30,0x06},                                             
    {0x38,0x32,0x00},                                             
    {0x38,0x33,0x00},                                             
    {0x38,0x34,0x00},                                             
    {0x3f,0x00,0x02},                                             
    {0x40,0x01,0x83},                                             
    {0x40,0x02,0x01},                                             
    {0x40,0x03,0x02},                                             
    {0x40,0x0e,0x00},                                             
    {0x40,0x11,0x00},                                             
    {0x40,0x12,0x00},                                             
    {0x42,0x00,0x08},                                             
    {0x43,0x02,0x7f},                                             
    {0x43,0x03,0xff},                                             
    {0x43,0x04,0x00},                                             
    {0x43,0x05,0x00},                                             
    {0x45,0x01,0x30},                                             
    {0x46,0x03,0x10},                                             
    {0x48,0x37,0x2c},                                             
    {0x4b,0x00,0x22},                                             
    {0x49,0x03,0x00},                                             
    {0x50,0x00,0x5f},                                             
    {0x50,0x01,0x01},                                             
    /*{0x50,0x04,0x00},*/                                         
    {0x50,0x13,0x20},                                             
    {0x50,0x51,0x00},                                             
    {0x59,0x80,0x00},                                             
    {0x59,0x81,0x21},                                             
    {0x59,0x82,0x00},                                             
    {0x59,0x83,0x00},                                             
    {0x59,0x84,0x00},                                             
    {0x59,0x85,0x00},                                             
    {0x59,0x86,0x00},                                             
    {0x59,0x87,0x00},                                             
    {0x59,0x88,0x00},                                             
    {0x01,0x00,0x01},                                             
#endif                                                        
};
static const UINT8 afe_1080P_code[][3]={
	/*preview size 1920*1080*/                                
    {0x03,0x00,0x02},                                         
    {0x03,0x02,0x5a},                                         
    {0x03,0x05,0x04},                                         
    {0x03,0x06,0x00},                                         
    {0x03,0x0b,0x02},                                         
    {0x03,0x0c,0x14},                                         
    {0x03,0x0e,0x01},                                         
    {0x03,0x13,0x02},                                         
    {0x03,0x14,0x14},                                         
    {0x03,0x1f,0x00},                                         
    #if LANE_2_EN                                             
    	{0x30,0x18,0x32}, /*2 lane mode*/             
    #else                                                     
    	{0x30,0x18,0x72}, /*4 lane mode*/             
    #endif                                                    
    {0x30,0x22,0x01},                                         
    {0x30,0x31,0x0a},                                         
    {0x30,0x32,0x80},                                         
    {0x31,0x06,0x01},                                         
    {0x35,0x00,0x00},                                         
    {0x35,0x01,0x1f},                                         
    {0x35,0x02,0x00},                                         
    {0x35,0x08,0x0c},                                         
    {0x35,0x09,0xff},                                         
    {0x36,0x01,0x88},                                         
    {0x36,0x02,0x00},                                         
    {0x36,0x03,0x05},                                         
    {0x36,0x04,0x02},                                         
    {0x36,0x05,0x50},                                         
    {0x36,0x06,0x00},                                         
    {0x36,0x07,0x2b},                                         
    {0x36,0x08,0x12},                                         
    {0x36,0x09,0x00},                                         
    {0x36,0x0a,0x00},                                         
    {0x36,0x0b,0x02},                                         
    {0x36,0x0c,0x10},                                         
    {0x36,0x0d,0x05},                                         
    {0x36,0x12,0x20},                                         
    {0x36,0x13,0x88},                                         
    {0x36,0x14,0x77},                                         
    {0x36,0x15,0xf4},                                         
    {0x36,0x16,0x30},                                         
    {0x36,0x17,0x00},                                         
    {0x36,0x18,0x20},                                         
    {0x36,0x19,0x00},                                         
    {0x36,0x1a,0x10},                                         
    {0x36,0x1d,0x00},                                         
    {0x36,0x1e,0x00},                                         
    {0x36,0x31,0x60},                                         
    {0x36,0x4a,0x06},                                         
    {0x37,0x00,0x40},                                         
    {0x37,0x01,0x08},                                         
    {0x37,0x02,0x11},                                         
    {0x37,0x03,0x20},                                         
    {0x37,0x04,0x08},                                         
    {0x37,0x05,0x00},                                         
    {0x37,0x06,0x84},                                         
    {0x37,0x07,0x08},                                         
    {0x37,0x08,0x20},                                         
    {0x37,0x09,0x3c},                                         
    {0x37,0x0a,0x01},                                         
    {0x37,0x0b,0x5d},                                         
    {0x37,0x0c,0x03},                                         
    {0x37,0x0e,0x20},                                         
    {0x37,0x0f,0x05},                                         
    {0x37,0x10,0x20},                                         
    {0x37,0x11,0x0e},                                         
    {0x37,0x14,0x31},                                         
    {0x37,0x18,0x75},                                         
    {0x37,0x19,0x13},                                         
    {0x37,0x1a,0x55},                                         
    {0x37,0x1b,0x03},                                         
    {0x37,0x1c,0x55},                                         
    {0x37,0x1d,0x03},                                         
    {0x37,0x1e,0x09},                                         
    {0x37,0x1f,0x16},                                         
    {0x37,0x20,0x0b},                                         
    {0x37,0x21,0x18},                                         
    {0x37,0x22,0x0b},                                         
    {0x37,0x23,0x18},                                         
    {0x37,0x24,0x04},                                         
    {0x37,0x25,0x04},                                         
    {0x37,0x26,0x02},                                         
    {0x37,0x27,0x02},                                         
    {0x37,0x28,0x02},                                         
    {0x37,0x29,0x02},                                         
    {0x37,0x2a,0x05},                                         
    {0x37,0x2b,0x65},                                         
    {0x37,0x2c,0x55},                                         
    {0x37,0x2d,0x65},                                         
    {0x37,0x2e,0x53},                                         
    {0x37,0x2f,0x33},                                         
    {0x37,0x30,0x33},                                         
    {0x37,0x31,0x33},                                         
    {0x37,0x32,0x03},                                         
    {0x37,0x33,0x80},                                         
    {0x37,0x34,0x10},                                         
    {0x37,0x39,0x03},                                         
    {0x37,0x3a,0x20},                                         
    {0x37,0x3b,0x0c},                                         
    {0x37,0x3c,0x1c},                                         
    {0x37,0x3e,0x18},                                         
    {0x37,0x60,0x05},                                         
    {0x37,0x61,0x00},                                         
    {0x37,0x62,0x05},                                         
    {0x37,0x63,0x00},                                         
    {0x37,0x64,0x05},                                         
    {0x37,0x65,0x00},                                         
    {0x3f,0x08,0x20},                                         
    {0x38,0x00,0x01},                                         
    {0x38,0x01,0x80},                                         
    {0x38,0x02,0x02},                                         
    {0x38,0x03,0x94},                                         
    {0x38,0x04,0x10},                                         
    {0x38,0x05,0xbf},                                         
    {0x38,0x06,0x0b},                                         
    {0x38,0x07,0x0b},                                         
    {0x38,0x08,0x07},                                         
    {0x38,0x09,0x80},                                         
    {0x38,0x0a,0x04},                                         
    {0x38,0x0b,0x38},                                         
    {0x38,0x0c,0x09},                                         
    {0x38,0x0d,0x66},                                         
    {0x38,0x0e,0x04},                                         
    {0x38,0x0f,0x54},                                         
    {0x38,0x11,0x18},                                         
    {0x38,0x13,0x00},                                         
    {0x38,0x14,0x03},                                         
    {0x38,0x15,0x01},                                         
    {0x38,0x20,0x01},                                         
    {0x38,0x21,0x06},                                         
    {0x38,0x23,0x08},                                         
    {0x38,0x28,0x00},                                         
    {0x38,0x29,0x02},                                         
    {0x38,0x2a,0x03},                                         
    {0x38,0x2b,0x01},                                         
    {0x38,0x30,0x08},                                         
    {0x38,0x32,0x00},                                         
    {0x38,0x33,0x00},                                         
    {0x38,0x34,0x00},                                         
    {0x3f,0x00,0x02},                                         
    {0x40,0x01,0x83},                                         
    {0x40,0x02,0x02},                                         
    {0x40,0x03,0x04},                                         
    {0x40,0x0e,0x00},                                         
    {0x40,0x11,0x00},                                         
    {0x40,0x12,0x00},                                         
    {0x42,0x00,0x08},                                         
    {0x43,0x02,0x7f},                                         
    {0x43,0x03,0xff},                                         
    {0x43,0x04,0x00},                                         
    {0x43,0x05,0x00},                                         
    {0x45,0x01,0x30},                                         
    {0x46,0x03,0x10},                                         
    {0x48,0x37,0x14},                                         
    {0x4b,0x00,0x22},                                         
    {0x49,0x03,0x00},                                         
    {0x50,0x00,0x53},                                         
    {0x50,0x01,0x01},                                         
    /*{0x50,0x04,0x00},*/                                     
    {0x50,0x13,0x20},                                         
    {0x50,0x51,0x00},                                         
    {0x59,0x80,0x00},                                         
    {0x59,0x81,0x00},                                         
    {0x59,0x82,0x00},                                         
    {0x59,0x83,0x00},                                         
    {0x59,0x84,0x00},                                         
    {0x59,0x85,0x00},                                         
    {0x59,0x86,0x00},                                         
    {0x59,0x87,0x00},                                         
    {0x59,0x88,0x00},                                         
    {0x01,0x00,0x01},                                         
};

static const UINT8 afe_qvga_code[][3]={
	/*preview size 320*240*/                              
    {0x03,0x00,0x02},                                     
    {0x03,0x02,0x5a},                                     
    {0x03,0x05,0x04},                                     
    {0x03,0x06,0x00},                                     
    {0x03,0x0b,0x02},                                     
    {0x03,0x0c,0x14},                                     
    {0x03,0x0e,0x01},                                     
    {0x03,0x13,0x02},                                     
    {0x03,0x14,0x14},                                     
    {0x03,0x1f,0x00},                                     
    #if LANE_2_EN                                         
    	{0x30,0x18,0x32}, /*2 lane mode*/             
    #else                                                 
    	{0x30,0x18,0x72}, /*4 lane mode*/             
    #endif                                                
    {0x30,0x22,0x01},                                     
    {0x30,0x31,0x0a},                                     
    {0x30,0x32,0x80},                                     
    {0x31,0x06,0x01},                                     
    {0x35,0x00,0x00},                                     
    {0x35,0x01,0x1f},                                     
    {0x35,0x02,0x40},                                     
    {0x35,0x08,0x0c},                                     
    {0x35,0x09,0xff},                                     
    {0x36,0x01,0x88},                                     
    {0x36,0x02,0x00},                                     
    {0x36,0x03,0x04},                                     
    {0x36,0x04,0x02},                                     
    {0x36,0x05,0x50},                                     
    {0x36,0x06,0x00},                                     
    {0x36,0x07,0x2b},                                     
    {0x36,0x08,0x12},                                     
    {0x36,0x09,0x00},                                     
    {0x36,0x0a,0x02},                                     
    {0x36,0x0b,0x02},                                     
    {0x36,0x0c,0x10},                                     
    {0x36,0x0d,0x01},                                     
    {0x36,0x12,0x20},                                     
    {0x36,0x13,0x88},                                     
    {0x36,0x14,0x77},                                     
    {0x36,0x15,0xf4},                                     
    {0x36,0x16,0x34},                                     
    {0x36,0x17,0x00},                                     
    {0x36,0x18,0x20},                                     
    {0x36,0x19,0x00},                                     
    {0x36,0x1a,0x10},                                     
    {0x36,0x1d,0x00},                                     
    {0x36,0x1e,0x00},                                     
    {0x36,0x31,0x60},                                     
    {0x36,0x4a,0x06},                                     
    {0x37,0x00,0x40},                                     
    {0x37,0x01,0x08},                                     
    {0x37,0x02,0x11},                                     
    {0x37,0x03,0x20},                                     
    {0x37,0x04,0x08},                                     
    {0x37,0x05,0x00},                                     
    {0x37,0x06,0x84},                                     
    {0x37,0x07,0x08},                                     
    {0x37,0x08,0x20},                                     
    {0x37,0x09,0x3c},                                     
    {0x37,0x0a,0x01},                                     
    {0x37,0x0b,0x5d},                                     
    {0x37,0x0c,0x03},                                     
    {0x37,0x0e,0x20},                                     
    {0x37,0x0f,0x05},                                     
    {0x37,0x10,0x20},                                     
    {0x37,0x11,0x0e},                                     
    {0x37,0x14,0x31},                                     
    {0x37,0x18,0x75},                                     
    {0x37,0x19,0x13},                                     
    {0x37,0x1a,0x55},                                     
    {0x37,0x1b,0x03},                                     
    {0x37,0x1c,0x55},                                     
    {0x37,0x1d,0x03},                                     
    {0x37,0x1e,0x09},                                     
    {0x37,0x1f,0x16},                                     
    {0x37,0x20,0x0b},                                     
    {0x37,0x21,0x18},                                     
    {0x37,0x22,0x0b},                                     
    {0x37,0x23,0x18},                                     
    {0x37,0x24,0x04},                                     
    {0x37,0x25,0x04},                                     
    {0x37,0x26,0x02},                                     
    {0x37,0x27,0x02},                                     
    {0x37,0x28,0x02},                                     
    {0x37,0x29,0x02},                                     
    {0x37,0x2a,0x05},                                     
    {0x37,0x2b,0x65},                                     
    {0x37,0x2c,0x55},                                     
    {0x37,0x2d,0x65},                                     
    {0x37,0x2e,0x53},                                     
    {0x37,0x2f,0x33},                                     
    {0x37,0x30,0x33},                                     
    {0x37,0x31,0x33},                                     
    {0x37,0x32,0x03},                                     
    {0x37,0x33,0x80},                                     
    {0x37,0x34,0x10},                                     
    {0x37,0x39,0x03},                                     
    {0x37,0x3a,0x20},                                     
    {0x37,0x3b,0x0c},                                     
    {0x37,0x3c,0x1c},                                     
    {0x37,0x3e,0x18},                                     
    {0x37,0x60,0x05},                                     
    {0x37,0x61,0x00},                                     
    {0x37,0x62,0x05},                                     
    {0x37,0x63,0x00},                                     
    {0x37,0x64,0x05},                                     
    {0x37,0x65,0x00},                                     
    {0x3f,0x08,0x20},                                     
    {0x38,0x00,0x01},                                     
    {0x38,0x01,0x80},                                     
    {0x38,0x02,0x01},                                     
    {0x38,0x03,0x24},                                     
    {0x38,0x04,0x11},                                     
    {0x38,0x05,0x3f},                                     
    {0x38,0x06,0x0c},                                     
    {0x38,0x07,0x7b},                                     
    {0x38,0x08,0x01},                                     
    {0x38,0x09,0x40},                                     
    {0x38,0x0a,0x00},                                     
    {0x38,0x0b,0xf0},                                     
    {0x38,0x0c,0x02},                                     
    {0x38,0x0d,0x94},                                     
    {0x38,0x0e,0x01},                                     
    {0x38,0x0f,0xf8},                                     
    {0x38,0x11,0x10},                                     
    {0x38,0x13,0x00},                                     
    {0x38,0x14,0x09},                                     
    {0x38,0x15,0x03},                                     
    {0x38,0x20,0x00},                                     
    {0x38,0x21,0x07},                                     
    {0x38,0x23,0x08},                                     
    {0x38,0x28,0x00},                                     
    {0x38,0x29,0x00},                                     
    {0x38,0x2a,0x0b},                                     
    {0x38,0x2b,0x01},                                     
    {0x38,0x30,0x06},                                     
    {0x38,0x32,0x00},                                     
    {0x38,0x33,0x00},                                     
    {0x38,0x34,0x00},                                     
    {0x3f,0x00,0x02},                                     
    {0x40,0x01,0x83},                                     
    {0x40,0x02,0x04},                                     
    {0x40,0x03,0x02},                                     
    {0x40,0x0e,0x00},                                     
    {0x40,0x11,0x00},                                     
    {0x40,0x12,0x00},                                     
    {0x42,0x00,0x08},                                     
    {0x43,0x02,0x7f},                                     
    {0x43,0x03,0xff},                                     
    {0x43,0x04,0x00},                                     
    {0x43,0x05,0x00},                                     
    {0x45,0x01,0x30},                                     
    {0x46,0x03,0x10},                                     
    {0x48,0x37,0x14},                                     
    {0x4b,0x00,0x22},                                     
    {0x49,0x03,0x00},                                     
    {0x50,0x00,0xd3},                                     
    {0x50,0x01,0x01},                                     
    /*{0x50,0x04,0x00},*/                                 
    {0x50,0x13,0x20},                                     
    {0x50,0x51,0x30},                                     
    {0x59,0x80,0x00},                                     
    {0x59,0x81,0x08},                                     
    {0x59,0x82,0x00},                                     
    {0x59,0x83,0x02},                                     
    {0x59,0x84,0x02},                                     
    {0x59,0x85,0x80},                                     
    {0x59,0x86,0x01},                                     
    {0x59,0x87,0xe0},                                     
    {0x59,0x88,0x01},                                     
    {0x01,0x00,0x01},                                     
};

static const UINT8 afe_4M_code[][3]={
	/*preview size 2304x1728 */                               
    {0x03,0x00,0x02},                                         
    {0x03,0x02,0x5a},                                         
    {0x03,0x05,0x04},                                         
    {0x03,0x06,0x00},                                         
    {0x03,0x0b,0x02},                                         
    {0x03,0x0c,0x14},                                         
    {0x03,0x0e,0x01},                                         
    {0x03,0x13,0x02},                                         
    {0x03,0x14,0x14},                                         
    {0x03,0x1f,0x00},                                         
    #if LANE_2_EN                                             
    	{0x30,0x18,0x32}, /*2 lane mode*/             
    #else                                                     
    	{0x30,0x18,0x72}, /*4 lane mode*/             
    #endif                                                    
    {0x30,0x22,0x01},                                         
    {0x30,0x31,0x0a},                                         
    {0x30,0x32,0x80},                                         
    {0x31,0x06,0x01},                                         
    {0x35,0x00,0x00},                                         
    {0x35,0x01,0x1f},                                         
    {0x35,0x02,0x00},                                         
    {0x35,0x08,0x0c},                                         
    {0x35,0x09,0xff},                                         
    {0x36,0x01,0x88},                                         
    {0x36,0x02,0x00},                                         
    {0x36,0x03,0x05},                                         
    {0x36,0x04,0x02},                                         
    {0x36,0x05,0x50},                                         
    {0x36,0x06,0x00},                                         
    {0x36,0x07,0x2b},                                         
    {0x36,0x08,0x12},                                         
    {0x36,0x09,0x00},                                         
    {0x36,0x0a,0x02},                                         
    {0x36,0x0b,0x02},                                         
    {0x36,0x0c,0x10},                                         
    {0x36,0x0d,0x05},                                         
    {0x36,0x12,0x20},                                         
    {0x36,0x13,0x88},                                         
    {0x36,0x14,0x77},                                         
    {0x36,0x15,0xf4},                                         
    {0x36,0x16,0x31},                                         
    {0x36,0x17,0x00},                                         
    {0x36,0x18,0x20},                                         
    {0x36,0x19,0x00},                                         
    {0x36,0x1a,0x10},                                         
    {0x36,0x1d,0x00},                                         
    {0x36,0x1e,0x00},                                         
    {0x36,0x31,0x60},                                         
    {0x36,0x4a,0x06},                                         
    {0x37,0x00,0x30},                                         
    {0x37,0x01,0x08},                                         
    {0x37,0x02,0x11},                                         
    {0x37,0x03,0x20},                                         
    {0x37,0x04,0x08},                                         
    {0x37,0x05,0x00},                                         
    {0x37,0x06,0x84},                                         
    {0x37,0x07,0x08},                                         
    {0x37,0x08,0x20},                                         
    {0x37,0x09,0x3c},                                         
    {0x37,0x0a,0x01},                                         
    {0x37,0x0b,0x5d},                                         
    {0x37,0x0c,0x03},                                         
    {0x37,0x0e,0x20},                                         
    {0x37,0x0f,0x05},                                         
    {0x37,0x10,0x20},                                         
    {0x37,0x11,0x20},                                         
    {0x37,0x14,0x31},                                         
    {0x37,0x18,0x75},                                         
    {0x37,0x19,0x13},                                         
    {0x37,0x1a,0x55},                                         
    {0x37,0x1b,0x03},                                         
    {0x37,0x1c,0x55},                                         
    {0x37,0x1d,0x03},                                         
    {0x37,0x1e,0x09},                                         
    {0x37,0x1f,0x16},                                         
    {0x37,0x20,0x0b},                                         
    {0x37,0x21,0x18},                                         
    {0x37,0x22,0x0b},                                         
    {0x37,0x23,0x18},                                         
    {0x37,0x24,0x04},                                         
    {0x37,0x25,0x04},                                         
    {0x37,0x26,0x02},                                         
    {0x37,0x27,0x02},                                         
    {0x37,0x28,0x02},                                         
    {0x37,0x29,0x02},                                         
    {0x37,0x2a,0x05},                                         
    {0x37,0x2b,0x65},                                         
    {0x37,0x2c,0x55},                                         
    {0x37,0x2d,0x65},                                         
    {0x37,0x2e,0x53},                                         
    {0x37,0x2f,0x33},                                         
    {0x37,0x30,0x33},                                         
    {0x37,0x31,0x33},                                         
    {0x37,0x32,0x03},                                         
    {0x37,0x33,0x80},                                         
    {0x37,0x34,0x10},                                         
    {0x37,0x39,0x03},                                         
    {0x37,0x3a,0x20},                                         
    {0x37,0x3b,0x0c},                                         
    {0x37,0x3c,0x1c},                                         
    {0x37,0x3e,0x18},                                         
    {0x37,0x60,0x05},                                         
    {0x37,0x61,0x00},                                         
    {0x37,0x62,0x05},                                         
    {0x37,0x63,0x00},                                         
    {0x37,0x64,0x05},                                         
    {0x37,0x65,0x00},                                         
    {0x3f,0x08,0x20},                                         
    {0x38,0x00,0x00},                                         
    {0x38,0x01,0x00},                                         
    {0x38,0x02,0x00},                                         
    {0x38,0x03,0x0C},                                         
    {0x38,0x04,0x12},                                         
    {0x38,0x05,0x3f},                                         
    {0x38,0x06,0x0d},                                         
    {0x38,0x07,0x97},                                         
    {0x38,0x08,0x09},                                         
    {0x38,0x09,0x00},                                         
    {0x38,0x0a,0x06},                                         
    {0x38,0x0b,0xc0},                                         
    {0x38,0x0c,0x05},                                         
    {0x38,0x0d,0xf0},                                         
    {0x38,0x0e,0x06},                                         
    {0x38,0x0f,0xda},                                         
    {0x38,0x11,0x17},                                         
    {0x38,0x13,0x02},                                         
    {0x38,0x14,0x03},                                         
    {0x38,0x15,0x01},                                         
    {0x38,0x20,0x00},                                         
    {0x38,0x21,0x06},                                         
    #if 0                                                     
    	{0x38,0x23,0x08},                                         
    	{0x38,0x28,0x00},                                         
    #endif                                                    
    {0x38,0x29,0x00},                                         
    {0x38,0x2a,0x03},                                         
    {0x38,0x2b,0x01},                                         
    {0x38,0x30,0x08},                                         
    {0x38,0x32,0x00},                                         
    {0x38,0x33,0x00},                                         
    {0x38,0x34,0x00},                                         
    {0x3f,0x00,0x02},                                         
    {0x40,0x01,0x83},                                         
    {0x40,0x02,0x02},                                         
    {0x40,0x03,0x04},                                         
    {0x40,0x0e,0x00},                                         
    {0x40,0x11,0x00},                                         
    {0x40,0x12,0x00},                                         
    {0x42,0x00,0x08},                                         
    {0x43,0x02,0x7f},                                         
    {0x43,0x03,0xff},                                         
    {0x43,0x04,0x00},                                         
    {0x43,0x05,0x00},                                         
    {0x45,0x01,0x30},                                         
    {0x46,0x03,0x10},                                         
    {0x48,0x37,0x28},                                         
    {0x4b,0x00,0x22},                                         
    {0x49,0x03,0x00},                                         
    {0x50,0x00,0x53},                                         
    {0x50,0x01,0x01},                                         
    /*{0x50,0x04,0x00},*/                                     
    {0x50,0x13,0x20},                                         
    {0x50,0x51,0x00},                                         
    {0x59,0x80,0x00},                                         
    {0x59,0x81,0x00},                                         
    {0x59,0x82,0x00},                                         
    {0x59,0x83,0x00},                                         
    {0x59,0x84,0x00},                                         
    {0x59,0x85,0x00},                                         
    {0x59,0x86,0x00},                                         
    {0x59,0x87,0x00},                                         
    {0x59,0x88,0x00},                                         
    {0x01,0x00,0x01},                                         
};


static const UINT8 afe_still_code[][3]={
#if 1    
	/*sf ok  Data rate:360MHz, System Clk:40MHz */                
    {0x03,0x00,0x02},                        
    {0x03,0x02,0x5a},                        
    {0x03,0x03,0x00},                        
    {0x03,0x04,0x01},                        
    {0x03,0x05,0x04},                        
    {0x03,0x06,0x00},                        
    {0x03,0x07,0x01},                        
    {0x03,0x0b,0x02},                        
    {0x03,0x0c,0x14},                        
    {0x03,0x0d,0x00},                        
    {0x03,0x0e,0x01},                        
    {0x03,0x0f,0x00},                        
    {0x03,0x13,0x02},                        
    {0x03,0x14,0x14},                        
    {0x03,0x15,0x00},                        
    {0x03,0x16,0x00},                        
    {0x03,0x17,0x00},                        
    {0x03,0x1f,0x00},                        
    {0x30,0x18,0x72},                        
    {0x30,0x22,0x01},                        
    {0x30,0x31,0x0a},                        
    {0x30,0x32,0x80},                        
    {0x31,0x06,0x01},                        
    {0x36,0x01,0x88},                        
    {0x36,0x02,0x00},                        
    {0x36,0x05,0x50},                        
    {0x36,0x06,0x00},                        
    {0x36,0x07,0x2b},                        
    {0x36,0x08,0x12},                        
    {0x36,0x09,0x00},                        
    {0x36,0x0b,0x02},                        
    {0x36,0x0c,0x10},                        
    {0x36,0x12,0x20},                        
    {0x36,0x13,0x88},                        
    {0x36,0x14,0x77},                        
    {0x36,0x15,0xf4},                        
    {0x36,0x17,0x00},                        
    {0x36,0x18,0x20},                        
    {0x36,0x19,0x00},                        
    {0x36,0x1a,0x10},                        
    {0x36,0x1d,0x00},                        
    {0x36,0x1e,0x00},                        
    {0x36,0x31,0x60},                        
    {0x36,0x4a,0x06},                        
    {0x37,0x07,0x08},                        
    {0x37,0x18,0x75},                        
    {0x37,0x1a,0x55},                        
    {0x37,0x1c,0x55},                        
    {0x37,0x33,0x80},                        
    {0x37,0x34,0x10},                        
    {0x37,0x60,0x01},                        
    {0x37,0x61,0x00},                        
    {0x37,0x62,0x01},                        
    {0x37,0x63,0x00},                        
    {0x37,0x64,0x01},                        
    {0x37,0x65,0x00},                        
    {0x38,0x23,0x08},                        
    {0x38,0x28,0x00},                        
    {0x38,0x32,0x00},                        
    {0x38,0x33,0x00},                        
    {0x38,0x34,0x00},                        
    {0x3f,0x00,0x02},                        
    {0x40,0x01,0x83},                        
    {0x40,0x0e,0x00},                        
    {0x40,0x11,0x00},                        
    {0x40,0x12,0x00},                        
    {0x42,0x00,0x08},                        
    {0x43,0x02,0x7f},                        
    {0x43,0x03,0xff},                        
    {0x43,0x04,0x00},                        
    {0x43,0x05,0x00},                        
    {0x45,0x01,0x30},                        
    {0x46,0x03,0x10},                        
    {0x4b,0x00,0x22},                        
    {0x49,0x03,0x00},                        
    {0x50,0x00,0x5f},                        
    {0x50,0x01,0x01},                        
    {0x50,0x13,0x20},                        
    {0x50,0x51,0x00},                        
    /*{0x01,0x00,0x01},*//*don't care*/                           
    {0x36, 0x03, 0x00},                      
    {0x36, 0x04, 0x00},                      
                                             
    {0x36, 0x0a, 0x00},                      
    {0x36, 0x0b, 0x02},                      
    {0x36, 0x0d, 0x01},                      
    {0x36, 0x16, 0x30},                      
    {0x37, 0x00, 0x80},                      
    {0x37, 0x01, 0x10},                      
    {0x37, 0x02, 0x22},                      
    {0x37, 0x03, 0x40},                      
    {0x37, 0x04, 0x10},                      
    {0x37, 0x05, 0x01},                      
    {0x37, 0x06, 0x04},                      
    {0x37, 0x08, 0x40},                      
    {0x37, 0x09, 0x78},                      
    {0x37, 0x0a, 0x02},                      
    {0x37, 0x0b, 0xb2},                      
    {0x37, 0x0c, 0x06},                      
    {0x37, 0x0e, 0x40},                      
    {0x37, 0x0f, 0x0a},                      
    {0x37, 0x10, 0x30},                      
    {0x37, 0x11, 0x1c},                      
    {0x37, 0x14, 0x31},                      
    {0x37, 0x19, 0x25},                      
    {0x37, 0x1b, 0x05},                      
    {0x37, 0x1d, 0x05},                      
    {0x37, 0x1e, 0x11},                      
    {0x37, 0x1f, 0x2c},                      
    {0x37, 0x20, 0x15},                      
    {0x37, 0x21, 0x30},                      
    {0x37, 0x22, 0x15},                      
    {0x37, 0x23, 0x30},                      
    {0x37, 0x24, 0x08},                      
    {0x37, 0x25, 0x08},                      
    {0x37, 0x26, 0x04},                      
    {0x37, 0x27, 0x04},                      
    {0x37, 0x28, 0x04},                      
    {0x37, 0x29, 0x04},                      
    {0x37, 0x2a, 0x09},                      
    {0x37, 0x2b, 0xc9},                      
    {0x37, 0x2c, 0xa9},                      
    {0x37, 0x2d, 0xb9},                      
    {0x37, 0x2e, 0x95},                      
    {0x37, 0x2f, 0x55},                      
    {0x37, 0x30, 0x55},                      
    {0x37, 0x31, 0x55},                      
    {0x37, 0x32, 0x05},                      
    {0x37, 0x39, 0x05},                      
    {0x37, 0x3a, 0x40},                      
    {0x37, 0x3b, 0x18},                      
    {0x37, 0x3c, 0x38},                      
    {0x37, 0x3e, 0x30},                      
    {0x38, 0x00, 0x00},                      
    {0x38, 0x01, 0x20},                      
    {0x38, 0x02, 0x00},                      
    {0x38, 0x03, 0x0e},                      
    {0x38, 0x04, 0x12},                      
    {0x38, 0x05, 0x3f},                      
    {0x38, 0x06, 0x0d},                      
    {0x38, 0x07, 0x91},                      
    {0x38, 0x08, 0x12},                      
    {0x38, 0x09, 0x00},                      
    {0x38, 0x0a, 0x0d},                      
    {0x38, 0x0b, 0x80},                      
    {0x38, 0x0c, 0x05},                      
    {0x38, 0x0d, 0xf8},                      
    {0x38, 0x0e, 0x0d},                      
    {0x38, 0x0f, 0xa4},                      
    {0x38, 0x11, 0x0f},                      
    {0x38, 0x13, 0x00},                      
    {0x38, 0x14, 0x01},                      
    {0x38, 0x15, 0x01},                      
    {0x38, 0x20, 0x00},                      
    {0x38, 0x21, 0x06},                      
    {0x38, 0x29, 0x00},                      
    {0x38, 0x2a, 0x01},                      
    {0x38, 0x2b, 0x01},                      
    {0x38, 0x30, 0x08},                      
    {0x3f, 0x08, 0x40},                      
    {0x40, 0x02, 0x04},                      
    {0x40, 0x03, 0x08},                      
    {0x48, 0x37, 0x13},                      
    {0x35, 0x01, 0xda},                      
    {0x35, 0x02, 0x00},                      
    {0x35, 0x08, 0x08},                      
    {0x35, 0x09, 0xff},                      
    {0x36, 0x38, 0x00},                      
    {0x30, 0x18, 0x72},                      
    {0x31, 0x06, 0x21},   

    {0x59, 0x88, 0x00}, /* Manual window Disable*/
	#if 1                                    
    	/*OV FAE(chad): 360MHz(Data rate), 80MHz(System clk)*/      
 	    {0x03,0x00,0x01},                    
 	    {0x03,0x02,0x3c},                    
	    {0x03,0x05,0x04},                    
	    {0x31,0x06,0x01},                    
                                             
    	{0x38,0x0c,0x0B}, /*HTSx2*/          
    	{0x38,0x0d,0xF0},                    
	#endif     
    {0x01, 0x00, 0x01}, 
#else
    /*Data rate:400MHz, System Clk:80MHz */     
    {0x03,0x00,0x02},                             
    {0x03,0x02,0x64},                             
    {0x03,0x05,0x04},                             
    {0x03,0x06,0x00},                             
    {0x03,0x0b,0x02},                             
    {0x03,0x0c,0x14},                             
    {0x03,0x0e,0x00},                             
    {0x03,0x13,0x02},                             
    {0x03,0x14,0x14},                             
    {0x03,0x1f,0x00},                             
                                                  
    {0x30,0x18,0xf2},                             
    {0x30,0x22,0x01},                             
    {0x30,0x31,0x0a},                             
    {0x30,0x32,0x80},                             
    {0x31,0x06,0x01},                             
    {0x36,0x01,0x88},                             
    {0x36,0x02,0x00},                             
    {0x36,0x05,0x50},                             
    {0x36,0x06,0x00},                             
    {0x36,0x07,0x2b},                             
    {0x36,0x08,0x12},                             
    {0x36,0x09,0x00},                             
    {0x36,0x0c,0x10},                             
    {0x36,0x12,0x20},                             
    {0x36,0x13,0x88},                             
    {0x36,0x14,0x77},                             
    {0x36,0x15,0xf4},                             
    {0x36,0x17,0x00},                             
    {0x36,0x18,0x20},                             
    {0x36,0x19,0x00},                             
    {0x36,0x1a,0x10},                             
    {0x36,0x1d,0x00},                             
    {0x36,0x1e,0x00},                             
    {0x36,0x31,0x60},                             
    {0x36,0x4a,0x06},                             
    {0x37,0x07,0x08},                             
    {0x37,0x18,0x75},                             
    {0x37,0x1a,0x55},                             
    {0x37,0x1c,0x55},                             
    {0x37,0x33,0x80},                             
    {0x37,0x34,0x10},                             
    {0x37,0x60,0x01},                             
    {0x37,0x61,0x00},                             
    {0x37,0x62,0x01},                             
    {0x37,0x63,0x00},                             
    {0x37,0x64,0x01},                             
    {0x37,0x65,0x00},                             
                                                  
    {0x38,0x23,0x08},                             
    {0x38,0x28,0x00},                             
    {0x38,0x32,0x00},                             
    {0x38,0x33,0x00},                             
    {0x38,0x34,0x00},                             
    {0x3f,0x00,0x02},                             
                                                  
    {0x40,0x01,0x83},                             
    {0x40,0x0e,0x00},                             
    {0x40,0x11,0x00},                             
    {0x40,0x12,0x00},                             
    {0x42,0x00,0x08},                             
    {0x43,0x02,0x7f},                             
    {0x43,0x03,0xff},                             
    {0x43,0x04,0x00},                             
    {0x43,0x05,0x00},                             
    {0x45,0x01,0x30},                             
    {0x46,0x03,0x10},                             
    {0x4b,0x00,0x22},                             
    {0x49,0x03,0x00},                             
    {0x50,0x00,0x5f},                             
    {0x50,0x01,0x01},                             
    {0x50,0x13,0x20},                             
    {0x50,0x51,0x00},                             
                      
    {0x03,0x0e,0x00},                             
                                                  
    {0x36,0x03,0x00},                             
    {0x36,0x04,0x00},                             
    {0x36,0x0a,0x00},                             
    {0x36,0x0b,0x02},                             
    {0x36,0x0d,0x01},                             
    {0x36,0x16,0x30},                             
    {0x37,0x00,0x80},                             
    {0x37,0x01,0x10},                             
    {0x37,0x02,0x22},                             
    {0x37,0x03,0x40},                             
    {0x37,0x04,0x10},                             
    {0x37,0x05,0x01},                             
    {0x37,0x06,0x04},                             
    {0x37,0x08,0x40},                             
    {0x37,0x09,0x78},                             
    {0x37,0x0a,0x02},                             
    {0x37,0x0b,0xb2},                             
    {0x37,0x0c,0x06},                             
    {0x37,0x0e,0x40},                             
    {0x37,0x0f,0x0a},                             
    {0x37,0x10,0x30},                             
    {0x37,0x11,0x1c},                             
    {0x37,0x14,0x31},                             
    {0x37,0x19,0x25},                             
    {0x37,0x1b,0x05},                             
    {0x37,0x1d,0x05},                             
    {0x37,0x1e,0x11},                             
    {0x37,0x1f,0x2c},                             
    {0x37,0x20,0x15},                             
    {0x37,0x21,0x30},                             
    {0x37,0x22,0x15},                             
    {0x37,0x23,0x30},                             
    {0x37,0x24,0x08},                             
    {0x37,0x25,0x08},                             
    {0x37,0x26,0x04},                             
    {0x37,0x27,0x04},                             
    {0x37,0x28,0x04},                             
    {0x37,0x29,0x04},                             
    {0x37,0x2a,0x09},                             
    {0x37,0x2b,0xc9},                             
    {0x37,0x2c,0xa9},                             
    {0x37,0x2d,0xb9},                             
    {0x37,0x2e,0x95},                             
    {0x37,0x2f,0x55},                             
    {0x37,0x30,0x55},                             
    {0x37,0x31,0x55},                             
    {0x37,0x32,0x05},                             
    {0x37,0x39,0x05},                             
    {0x37,0x3a,0x40},                             
    {0x37,0x3b,0x18},                             
    {0x37,0x3c,0x38},                             
    {0x37,0x3e,0x30},                             
                                                  
    {0x38,0x00,0x00},                             
    {0x38,0x01,0x20},                             
    {0x38,0x02,0x00},                             
    {0x38,0x03,0x0e},                             
    {0x38,0x04,0x12},                             
    {0x38,0x05,0x3f},                             
    {0x38,0x06,0x0d},                             
    {0x38,0x07,0x91},                             
    {0x38,0x08,0x12},                             
    {0x38,0x09,0x00},                             
    {0x38,0x0a,0x0d},                             
    {0x38,0x0b,0x80},                             
    {0x38,0x0c,0x05},                             
    {0x38,0x0d,0xf8},                             
    {0x38,0x0e,0x0d},                             
    {0x38,0x0f,0xa4},                             
    {0x38,0x11,0x0f},                             
    {0x38,0x13,0x00},                             
    {0x38,0x14,0x01},                             
    {0x38,0x15,0x01},                             
    {0x38,0x20,0x00},                             
    {0x38,0x21,0x06},                             
    {0x38,0x29,0x00},                             
    {0x38,0x2a,0x01},                             
    {0x38,0x2b,0x01},                             
    {0x38,0x30,0x08},                             
                                                  
    {0x3f,0x08,0x40},                             
    {0x40,0x02,0x04},                             
    {0x40,0x03,0x08},                             
                                                  
    {0x35,0x01,0xda},                             
    {0x35,0x02,0x00},                             
    {0x35,0x08,0x08},                             
    {0x35,0x09,0xff},                             
                                                  
    {0x36,0x38,0x00},    
                                                  
    {0x30,0x18,0x72},                             
    {0x31,0x06,0x21}, 

    {0x38,0x0c,0x0b}, 
    {0x38,0x0d,0xf0}, 
                                                  
    {0x01,0x00,0x01},    
#endif
};	

/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg = 
{
	.wordcntsel 	 =0 ,
	.Lvdsmode		 =0 ,
#if LANE_2_EN	
	.Lvdslanenum	 =1 ,/*2 lane mode*/
#else
	.Lvdslanenum	 =0 ,/*4 lane mode*/
#endif
	.Lvdslanemode	 =0 ,
	.Lvdslaneswap	 =1 ,
	.Lvdsraw		 =0 ,
	.Lvdsoutsel 	 =0 ,
	.Lvdseofsel 	 =1 ,
	.Lvdseolsel 	 =0 ,
	.vmode			 =0 ,
	.lvdsvdsrc		 =1 ,
	.lvds_dphy_sel	 =1 ,
	.S2pphase		 =0 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	.Ctrlword_sol	 =0x2B,
	.Ctrlword_sof	 =0x00 ,
	.syncwrod0		 =0xB8 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	.Exthd_num		 =8 ,
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
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
	.hratio = PREVIEW_VGA_HSIZE,
	.vratio = PREVIEW_VGA_VSIZE,
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
	.crop = NULL,
	.hsize = PREVIEW_1080P_HSIZE,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
	.hratio = PREVIEW_1080P_HSIZE,
	.vratio = PREVIEW_1080P_VSIZE,
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
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_720P_CFAPATTERN,
	.hratio = PREVIEW_720P_HSIZE,
	.vratio = PREVIEW_720P_VSIZE,
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
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_QVGA_CFAPATTERN,
	.hratio = PREVIEW_QVGA_HSIZE,
	.vratio = PREVIEW_QVGA_VSIZE,
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
	.crop = NULL,
	.hsize = CAPTURE_HSIZE,
	.vsize = CAPTURE_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = SNAP_CFAPATTERN,
	.hratio = CAPTURE_HSIZE,
	.vratio = CAPTURE_VSIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview4MCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview4MBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_4M_HSIZE,
	.vsize = PREVIEW_4M_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_4M_CFAPATTERN,
	.hratio = PREVIEW_4M_HSIZE,
	.vratio = PREVIEW_4M_VSIZE,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &sensorTgPreview4MCfg,       /*  30  fps */
    &sensorTgPreviewQVGACfg,     /* 240  fps */
    &sensorTgPreview720PCfg,     /*  30  fps */
    &sensorTgPreview1080PCfg,    /*  30  fps */
	&sensorTgPreviewVGACfg,      /* 120  fps */
	&sensorTgPreview720PCfg,     /*  60  fps */ 
    &sensorTgPreviewFULLCfg,     /*  7.5 fps*/    
};

static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
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

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
    &sensorTgSnapRollCfg,    
    &sensorTgSnapFrameCfg,    
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
	.hsize           = PREVIEW_1080P_HSIZE,
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

static const sensorSyncSigCfg_t sensorSyncSigPreview4MCfg = {
	.linetotal       = PREVIEW_4M_LINETOTAL,
	.lineblank       = PREVIEW_4M_LINEBLANK,
	.frametotal      = PREVIEW_4M_FRAMETOTAL,
	.frameblank      = PREVIEW_4M_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_4M_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_4M_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_4M_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_4M_VRESHAPE_RISE,
	.hoffset         = PREVIEW_4M_HOFFSET,
	.voffset         = PREVIEW_4M_VOFFSET,
	.hsize           = PREVIEW_4M_HSIZE,
	.vsize           = PREVIEW_4M_VSIZE,
	.lastlinepix     = PREVIEW_4M_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_4M_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_4M_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_4M_HDNUM_CMDPORT,
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

static const sensorSyncSigCfg_t sensorSyncSigPreviewFULLCfg = {
    .linetotal       = CAPTURE_LINETOTAL,                                                
	.lineblank       = CAPTURE_LINEBLANK,    
	.hreshape_fall   = CAPTURE_HRESHAPE_FALL,                                          
	.hreshape_rise   = CAPTURE_HRESHAPE_RISE,
	.hoffset         = CAPTURE_HOFFSET,      
	
	.frametotal      = CAPTURE_FRAMETOTAL,                                                
	.frameblank      = CAPTURE_VSIZE+1,
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
      .frameblank      = CAPTURE_VSIZE+1,
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
	.defSyncSigCfg  = &sensorSyncSigPreview4MCfg,
	.verStr = MODEL_VERSION,
};

UINT8 
OVsensorRead(UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[3];
	/*UINT8 readval=0;*/
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	sensorSerialBlockRead(cmdportid,1,regread,0);
	profLogPrintf(1,"\nREAD [%4x] = [%4x]",addr,regread[2]);
	return regread[2];
}

void OVsensorExpLine(UINT16 line)
{
    static UINT8 regdata[9];
    regdata[0]=0x35;
    regdata[1]=0x00;
    regdata[2]=((line>>8)&0xF0)>>4;
    regdata[3]=0x35;
    regdata[4]=0x01;
    regdata[5]=(line>>4)&0xFF;
    regdata[6]=0x35;
    regdata[7]=0x02;
    regdata[8]=(line&0x0F)<<4;
    sensorSerialBulkWrite(0,3,regdata,0);
}

void OVsensorDummyLine(UINT16 line)
{
    static UINT8 regdata[6];
    regdata[0]=0x38;
    regdata[1]=0x0E;
    regdata[2]=line>>8;
    regdata[3]=0x38;
    regdata[4]=0x0F;
    regdata[5]=line&0xFF;
    sensorSerialBulkWrite(0,2,regdata,0);
}

void OVsensorInit()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
    sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code_SWReset)/sizeof(afe_init_code_SWReset[0])), afe_init_code_SWReset[0],0);	
	tmrUsWait(20000);
	sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code)/sizeof(afe_init_code[0])), afe_init_code[0],0);
}
void OVsensorPLLRead()
{
    PROF_LOG_ADD(1,"\n---- HTS ---");
    OVsensorRead(0x380C);
    OVsensorRead(0x380D);
    PROF_LOG_ADD(1,"\n---- VTS ---");
    OVsensorRead(0x380E);
    OVsensorRead(0x380F);
    PROF_LOG_ADD(1,"\n---- HS ---");
    OVsensorRead(0x3808);
    OVsensorRead(0x3809);
    PROF_LOG_ADD(1,"\n---- VS ---");
    OVsensorRead(0x380A);
    OVsensorRead(0x380B);
    PROF_LOG_ADD(1,"\n---- OV Sensor PLL ---\n");
    OVsensorRead(0x0300);
    OVsensorRead(0x0302);
    OVsensorRead(0x0303);
    OVsensorRead(0x0304);
    OVsensorRead(0x0305);
    OVsensorRead(0x0306);
    OVsensorRead(0x0307);
    OVsensorRead(0x030b);
    OVsensorRead(0x030c);
    OVsensorRead(0x030d);
    OVsensorRead(0x030e);
    OVsensorRead(0x030f);
    OVsensorRead(0x0313);
    OVsensorRead(0x0314);
    OVsensorRead(0x0315);
    OVsensorRead(0x0316);
    OVsensorRead(0x0317);
    OVsensorRead(0x3018);
    OVsensorRead(0x3022);
    OVsensorRead(0x3106);
    OVsensorRead(0x3607);
    OVsensorRead(0x360b);
    PROF_LOG_ADD(1,"\n----------------------\n");
}

void Dump5Kreg()
{
    profLogPrintf(1,"[0x2270]=%x",*(volatile UINT8 *)0xb0002270);
	profLogPrintf(1,"[0x2271]=%x",*(volatile UINT8 *)0xb0002271);
	profLogPrintf(1,"[0x2272]=%x",*(volatile UINT16 *)0xb0002272);
	profLogPrintf(1,"[0x2274]=%x",*(volatile UINT16 *)0xb0002274);
	profLogPrintf(1,"[0x2276]=%x",*(volatile UINT16 *)0xb0002276);
	profLogPrintf(1,"[0x2278]=%x",*(volatile UINT16 *)0xb0002278);
	profLogPrintf(1,"[0x227A]=%x",*(volatile UINT16 *)0xb000227A);
	profLogPrintf(1,"[0x227F]=%x",*(volatile UINT8 *)0xb000227F);
	profLogPrintf(1,"[0x2004]=%x",*(volatile UINT8 *)0xb0002004);
    profLogPrintf(1,"[0x0082]=%x",*(volatile UINT8 *)0xb0000082);
	profLogPrintf(1,"[0x0083]=%x",*(volatile UINT8 *)0xb0000083);
    profLogPrintf(1,"[0x70F5]=%x",*(volatile UINT8 *)0xb00070F5);
	profLogPrintf(1,"[0xA020]=%x",*(volatile UINT8 *)0xb000A020); 
    WRITE8(0xb0002004,0x10);                        /*Clear desensor error*/ 
}

void
sensorTg_ov16820_Install()
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
    PROF_LOG_ADD(0, "sensorDoFuncInit()___enter");

    /* TGGPIO configuration here */    

	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;

	/*Debug only */

	/*Pixel Clock = 27MHz /27*48/2 = 24 MHz, 1080P= 15fps VGA=30fps*/
#if SPCA6330
	sensorTgPllClkExtSet(24000000);
#else
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_DIV,1,27,(16*2),NOUSE);
	sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_TGPLL_EN,1,SENSOR_TGPLL_DRV_8MA,NOUSE,NOUSE);
#endif

	halPllFrontClkSet(0x00,94500); /* 94.5 MHz */

    /* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);

    /*****TODO*****front reset after these???*/


#if 0 /*GPIO control */
	sp5kGpioCfgSet(SENOR_GPIO_GRP,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);

	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(10000); /*10ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,0); /* PWD LOW*/
	tmrUsWait(50000); /*50ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(25000); /*25ms*/
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
#else
    /*6330 EVB*/
    sp5kGpioCfgSet(GPIO_GRP_FML, (1<<23), (1<<23)); /*RESET*/
    sp5kGpioCfgSet(GPIO_GRP_TGL, (1<<0) , (1<<0) ); /*PWD*/

    sp5kGpioWrite(GPIO_GRP_FML, (1<<23), (0));
    sp5kGpioWrite(GPIO_GRP_TGL, (1<<0),  (1<<0));
    tmrUsWait(50000);
    sp5kGpioWrite(GPIO_GRP_FML, (1<<23), (1<<23));
    tmrUsWait(50000);
    sp5kGpioWrite(GPIO_GRP_TGL, (1<<0),  (0));
    tmrUsWait(50000);
#endif
	
   	cmdportid = sensorTgCfg.cmdportid_xtg;
	WRITE8(0xb000010a,0x18);
   	tmrUsWait(2000);

	OVsensorRead(0x300c); /*sensor ID */
	OVsensorRead(0x300b); /*sensor ID */
	OVsensorRead(0x300a); /*sensor ID */

#if 1

    sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code_SWReset)/sizeof(afe_init_code_SWReset[0])), afe_init_code_SWReset[0],0);	
	tmrUsWait(20000);
	sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code)/sizeof(afe_init_code[0])), afe_init_code[0],0);

	/*TG PLL=MCLK out=24MHz*/
	sensorLVDSCfgSet(&sensorLVDSCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);
	tmrUsWait(100000);	/* delay 100 ms*/
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
#else
	/*TG PLL=MCLK out=24MHz*/
    sensorSerialBulkWrite(cmdportid,(sizeof(afe_init_code_SWReset)/sizeof(afe_init_code_SWReset[0])), afe_init_code_SWReset[0],0);	
	tmrUsWait(100000);	/* delay 100 ms*/
	sensorSerialBulkWrite(cmdportid, sizeof(afe_1080P_code)/sizeof(afe_1080P_code[0]), afe_1080P_code[0],0);
	sensorLVDSCfgSet(&sensorLVDSCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);
	sensorSyncSigInit(&sensorSyncSigPreview1080PCfg);
#endif
	
	/*halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121500);*/ /* 121.5MHz */     
	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,150000);
	PROF_LOG_ADD(0, "sensorDoFuncInit()___done");
    printf("sensorDoFuncInit()___done"); 	
	tmrUsWait(10000);
#if 1
    sensorResumeInstall(sensorResumeLoad);
	sensorSuspendInstall(sensorSuspendLoad);
#endif    
    /*crop*/
    *(volatile UINT8 *) 0xb0009082 = 0x7;
    *(volatile UINT16 *)0xb000909c = 0x0;

    WRITE8(0xb0009010,0x01);
    WRITE8(0xb0009010,0x00);

    /*Frex*/
    #if 1
        sp5kGpioCfgSet(GPIO_GRP_TGL, (1<<4), (1<<4));
        /*sp5kGpioWrite(GPIO_GRP_TGL, (1<<4), (1<<4));*/
        sp5kGpioWrite(GPIO_GRP_TGL, (1<<4), (0));
    #endif

    /*Debug by GPIO*/
    #if 1
        sp5kGpioCfgSet(GPIO_GRP_TGL, (1<<1), (1<<1));
        sp5kGpioWrite(GPIO_GRP_TGL, (1<<1), (0));
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
    sensor_event_wait((1UL<<13));                                                                              
    sensorSerialBulkWrite(cmdportid, sizeof(afe_vga_code)/sizeof(afe_vga_code[0]), afe_vga_code[0],0);         
    sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);                                                             
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);                                                               
    PROF_LOG_ADD(0, "mipi VGA cmd done");                                                                      
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
    sensor_event_wait((1UL<<13));
	sensorSerialBulkWrite(cmdportid, sizeof(afe_1080P_code)/sizeof(afe_1080P_code[0]), afe_1080P_code[0],0);
	sensorSyncSigSet(&sensorSyncSigPreview1080PCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	PROF_LOG_ADD(0, "mipi 1080P done");
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
    sensor_event_wait((1UL<<13));                                                                                   
    sensorSerialBulkWrite(cmdportid, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0],0);        
    sensorSyncSigSet(&sensorSyncSigPreviewFULLCfg);                                                                 
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);                                                                    
    PROF_LOG_ADD(0, "mipi FULL cmd done");                                                                          
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
    sensor_event_wait((1UL<<13));                                                                            
    sensorSerialBulkWrite(cmdportid, sizeof(afe_720P_code)/sizeof(afe_720P_code[0]), afe_720P_code[0],0);    
    sensorSyncSigSet(&sensorSyncSigPreview720PCfg);                                                          
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);                                                             
    PROF_LOG_ADD(0, "mipi 720P cmd done");                                                                   
                                                                                                               
}
static void
sensorDoFuncPreview4MBegin()
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
    sensor_event_wait((1UL<<13));                                                                                     
    sensorSerialBulkWrite(cmdportid, sizeof(afe_4M_code)/sizeof(afe_4M_code[0]), afe_4M_code[0],0);                   
    sensorSyncSigSet(&sensorSyncSigPreview4MCfg);                                                                     
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);                                                                      
    PROF_LOG_ADD(0, "mipi 4M cmd done");                                                                              
}

static void
sensorDoFuncPreviewQVGABegin()
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
    sensor_event_wait((1UL<<13));
	sensorSerialBulkWrite(cmdportid, sizeof(afe_qvga_code)/sizeof(afe_qvga_code[0]), afe_qvga_code[0],0);
	sensorSyncSigSet(&sensorSyncSigPreviewQVGACfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
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
}

static void
sensorDoFuncSnapEarly0()
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	sensorSerialBulkWrite(cmdportid, sizeof(afe_still_code)/sizeof(afe_still_code[0]), afe_still_code[0],0);
	sensorSyncSigSet(&sensorSyncSigSnapCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1); 
    PROF_LOG_ADD(1, "sensorDoFuncSnapEarly0()");
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
    #if 0
	    sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_FREX,SENSOR_FREX);
  	    sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, SENSOR_FREX);
    #else
        sp5kGpioWrite(GPIO_GRP_TGL, (1<<4), (1<<4));
    #endif  
    
	/* add this */ 
    #if 0
	    if (sp5kSnapFlashCfgGet(SP5K_SNAP_FLASH_CFG_MODE))
		    tmrWaitListInstall(2, 53, FlashDirectTrig, 0);/*FlashDirectTrig(0);*/
    #endif
  
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
	PROF_LOG_ADD(1, "sensorDoFuncSnapBshutterEnd()");
    sp5kGpioWrite(GPIO_GRP_TGL, (1<<4), (0));
/*	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_FREX,SENSOR_FREX);
	sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);*/
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
	PROF_LOG_ADD(0, "sensorDoFuncSnapExpEnd()_b");
	/*sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_FREX,SENSOR_FREX);
	sp5kGpioWrite(SENOR_GPIO_GRP, SENSOR_FREX, 0);*/
	PROF_LOG_ADD(0, "sensorDoFuncSnapExpEnd()");
    PROF_LOG_ADD(1, "sensorDoFuncSnapExpEnd()");
}

static void
sensorDoFuncSnapReadoutField0()
{
    PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutField0()");
}

static void
sensorDoFuncSnapFieldEnd()
{
	PROF_LOG_ADD(0, "sensorDoFuncSnapFieldEnd()");
    PROF_LOG_ADD(1, "sensorDoFuncSnapFieldEnd()");    
}

/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{
	PROF_LOG_ADD(1, "sensorDoFuncSnapReadoutEnd()");
	profLogPrintf(1,"[0x2270]=%x",*(volatile UINT8 *)0xb0002270);
	profLogPrintf(1,"[0x2271]=%x",*(volatile UINT8 *)0xb0002271);
	profLogPrintf(1,"[0x2272]=%x",*(volatile UINT16 *)0xb0002272);
	profLogPrintf(1,"[0x2274]=%x",*(volatile UINT16 *)0xb0002274);
	profLogPrintf(1,"[0x2276]=%x",*(volatile UINT16 *)0xb0002276);
	profLogPrintf(1,"[0x2278]=%x",*(volatile UINT16 *)0xb0002278);
	profLogPrintf(1,"[0x227A]=%x",*(volatile UINT16 *)0xb000227A);
	profLogPrintf(1,"[0x227F]=%x",*(volatile UINT8 *)0xb000227F);
	profLogPrintf(1,"[0x2004]=%x",*(volatile UINT8 *)0xb0002004);
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
    #if 0    
	    static UINT8 regdata1[][3] SENSOR_SIF1DMA_VARBUF= { 
	    }; 
	    sensorSerialDirectBufSnoop(regdata1, sizeof(regdata1));

	    if((sensorModeGet()&0xF0) == SENSOR_MODE_SNAP){
		    sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 4, regdata1);
	    }else{	
		    sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, sizeof(regdata1)/sizeof(regdata1[0]), regdata1);
        }   
    #else
        #if 0
        if((sensorModeGet()&0xF0)==0x40){
 		    profLogPrintf(1,"snap [nreg=%d][%x %x %x] [%x %x %x] [%x %x %x] [%x %x %x] [%x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7],pdata[8],pdata[9],pdata[10],pdata[11],pdata[12],pdata[13],pdata[14]);
	    }else{ 
		    profLogPrintf(1,"prev [nreg=%d][%x %x %x %x] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7]);
  	    }
        #endif
        if(sensorModeGet()==0x41){
            
        }else{    
            sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
        }    
    #endif
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

#if SP5K_SENSOR_OV16820
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
static UINT32 sensorSuspended;
static UINT32 sensorResumeLoad()
{
	if(sensorSuspended ==1){
		/* sensorResumeExecute() */
		PROF_LOG_ADD(0, "senResu s");/*Edward 20111126*/
		UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
		/*sensor resume code body*/
		sensorSerialFlush(cmdportid);
		sensorSuspended = 0;
	}
	return SUCCESS;
}
static UINT32 sensorSuspendLoad()
{
	if(sensorSuspended ==0){
		/* sensorSuspendExecute() */
		PROF_LOG_ADD(0, "senSusp s");/*Edward 20111126*/
		UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
		/*sensor suspend code body*/
		sensorSerialFlush(cmdportid);
		PROF_LOG_ADD(0, "senSusp e");/*Edward 20111126*/
		sensorSuspended = 1;
	}
	return SUCCESS;
}
