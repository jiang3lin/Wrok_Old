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

#define MODEL_VERSION "$unp_tg_ar0330-0.00.05"
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

#define BURST_CAP_EN     	1

#if BURST_CAP_EN
#define S2S_TIME_LOG 		0
#define DDR_CLOCK_297MHz    1
#else
#define S2S_TIME_LOG 		0
#define DDR_CLOCK_297MHz    0
#endif

#define HAS_STREAM_OFF    1
#define MODE_SYNC_EARLY   50

/*#define PROF_LOG_EN    1

#if PROF_LOG_EN
#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)
#else
#define PROF_LOG_ADD(id,msg)
#define PROF_LOG_PRINT(id,str,args...)
#endif*/

#if 1
#define SENSOR_FREX     (1<<4)           /* for EVB */
#define SENSOR_PWRDN    (1<<14)
#define SENSOR_RESET    (1<<4)
#define SENSOR_TEST     (1<<3)

#define OUT_SENSOR_GPIO_MASK 	SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define CFG_SENSOR_GPIO_DIR 	SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define SENOR_GPIO_GRP SP5K_GPIO_GRP_TGL
#else
/*#define SENSOR_FREX     BIT6*/       /* for A900 */
#define SENSOR_FREX     (1<<15)           /* for EVB */
#define SENSOR_PWRDN    (1<<14)
#define SENSOR_RESET    (1<<12)
#define SENSOR_TEST     (1<<3)

#define OUT_SENSOR_GPIO_MASK 	SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define CFG_SENSOR_GPIO_DIR 	SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET|SENSOR_TEST
#define SENOR_GPIO_GRP SP5K_GPIO_GRP_LMI
#endif

void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);

/* Sensor image size definition here */
#define NO             0
#define BY_PIXEL       1
#define BY_HD          0
#define NOUSE          0
#define DISABLE        0xFFFF


#define  _SENSOR_DBG_  (1)
#if(_SENSOR_DBG_)
    /*debug by TGGPIO2*/ 
    #define DBG_GPIO(x) if(x){WRITE8(0xb0009032, (READ8(0xb0009032)|(0x02)));}else{WRITE8(0xb0009032, (READ8(0xb0009032)&(~0x02)));}
#else
    #define DBG_GPIO(x)
#endif

#define lum_2048x1536 1
#define lum_2048x1152 0
#define lum_2048x1280 0
/*******************************************************/

#define PREVIEW_WVGA_FPS60_HSIZE			1152								/* preview horiz. size*/
#define PREVIEW_WVGA_FPS60_LINETOTAL   		1242 								/* preview Hd length 0x9088-htotal*/
#define PREVIEW_WVGA_FPS60_LINEBLANK   		6									/* preview Hd sync length 0x908c hblank*/
#define PREVIEW_WVGA_FPS60_HOFFSET     		80									/* preview Hd valid pixel data offset position 0x90a6 hoffset*/
#define PREVIEW_WVGA_FPS60_HRESHAPE_FALL    1400 								/* preview Hd reshap fall  0x90b2*/
#define PREVIEW_WVGA_FPS60_HRESHAPE_RISE    1500								/* preview Hd reshap rise 0x90b0*/

#define PREVIEW_WVGA_FPS60_VSIZE			638									/* preview vertical size 0x90a8 vsize*/
#define PREVIEW_WVGA_FPS60_FRAMETOTAL  		642+1						 		/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_WVGA_FPS60_FRAMEBLANK  		642+1								/* preview Vd sync length 0x9090 vblank->vtotal*/

#define PREVIEW_WVGA_FPS60_EXTEND_LINES 	12
#define PREVIEW_WVGA_FPS60_VRESHAPE_COUNTER_SEL  BY_HD							/* preview Vd reshap control*/
#define PREVIEW_WVGA_FPS60_VRESHAPE_FALL    1 									/* preview Vd reshap fall 0x90b6*/
#define PREVIEW_WVGA_FPS60_VRESHAPE_RISE    2 									/* preview Vd reshap rise 0x90b4*/
#define PREVIEW_WVGA_FPS60_VOFFSET     	 	3  									/*  preview Vd valid line data offset position 0x90aa voffset*/
/*******************************************************/

#define PREVIEW_1080_FPS30_HSIZE  	  		1920 								/* preview horiz. size*/
#define PREVIEW_1080_FPS30_LINETOTAL   		6000 								/* preview Hd length 0x9088-htotal*/
#define PREVIEW_1080_FPS30_LINEBLANK   		6									/* preview Hd sync length 0x908c hblank*/
#define PREVIEW_1080_FPS30_HOFFSET     		4000								/* preview Hd valid pixel data offset position 0x90a6 hoffset*/
#define PREVIEW_1080_FPS30_HRESHAPE_FALL    700 								/* preview Hd reshap fall 0x90b2*/
#define PREVIEW_1080_FPS30_HRESHAPE_RISE    800									/* preview Hd reshap rise 0x90b0*/

#define PREVIEW_1080_FPS30_VSIZE   			1080   								/* preview vertical size 0x90a8 vsize */
#define PREVIEW_1080_FPS30_FRAMETOTAL  		1081   								/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_1080_FPS30_FRAMEBLANK  		1081								/* preview Vd sync length 0x9090 vblank->vtotal*/

#define PREVIEW_1080_FPS30_EXTEND_LINES 	12
#define PREVIEW_1080_FPS30_VRESHAPE_COUNTER_SEL  BY_HD							/* preview Vd reshap control*/
#define PREVIEW_1080_FPS30_VRESHAPE_FALL    PREVIEW_1080_FPS30_EXTEND_LINES+1	/* preview Vd reshap fall 0x90b6*/
#define PREVIEW_1080_FPS30_VRESHAPE_RISE 	PREVIEW_1080_FPS30_EXTEND_LINES+2	/* preview Vd reshap rise 0x90b4*/
#define PREVIEW_1080_FPS30_VOFFSET     		PREVIEW_1080_FPS30_EXTEND_LINES+3 	/* preview Vd valid line data offset position 0x90aa voffset*/
/*******************************************************/

#define PREVIEW_2304_FPS30_HSIZE 			2304								/* preview horiz. size*/ 
#define PREVIEW_2304_FPS30_LINEBLANK   		6									/* preview Hd sync length  0x908c hblank*/

#define PREVIEW_2304_FPS30_LINETOTAL   		(PREVIEW_2304_FPS30_HSIZE*3)		/* preview Hd length  0x9088-htotal*/
#define PREVIEW_2304_FPS30_HRESHAPE_FALL    400									/* preview Hd reshap fall  0x90b2*/
#define PREVIEW_2304_FPS30_HRESHAPE_RISE    (PREVIEW_2304_FPS30_HRESHAPE_FALL+0x100)/* preview Hd reshap rise 0x90b0*/
#define PREVIEW_2304_FPS30_HOFFSET     		(PREVIEW_2304_FPS30_HRESHAPE_RISE+0x100)/* preview Hd valid pixel data offset position  0x90a6 hoffset*/

#define PREVIEW_2304_FPS30_VSIZE 			1296								/* preview vertical size  0x90a8 vsize*/
#define PREVIEW_2304_FPS30_FRAMETOTAL   	1297								/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_2304_FPS30_FRAMEBLANK   	1297								/* mipi use > vsize is more safe*/

#define PREVIEW_2304_FPS30_EXTEND_LINES 	12
#define PREVIEW_2304_FPS30_VRESHAPE_COUNTER_SEL	BY_HD 							/* preview Vd reshap control*/
#define PREVIEW_2304_FPS30_VRESHAPE_FALL	PREVIEW_2304_FPS30_EXTEND_LINES+0	/* preview Vd reshap fall  0x90b6*/
#define PREVIEW_2304_FPS30_VRESHAPE_RISE	PREVIEW_2304_FPS30_EXTEND_LINES+1	/* preview Vd reshap rise  0x90b4*/
#define PREVIEW_2304_FPS30_VOFFSET     		PREVIEW_2304_FPS30_EXTEND_LINES+2	/* preview Vd valid line data offset position  0x90aa voffset*/
/*******************************************************/

#define CAPTURE_2304_HSIZE 					2304								/* capture horiz. size*/                                                                                           
#define CAPTURE_2304_LINEBLANK   			6									/* capture Hd sync length  0x908c hblank*/                                     

#define CAPTURE_2304_LINETOTAL   			(CAPTURE_2304_HSIZE*3)				/* capture Hd length  0x9088-htotal*/    
#define CAPTURE_2304_HRESHAPE_FALL          400									/* capture Hd reshap fall  0x90b2*/                                    
#define CAPTURE_2304_HRESHAPE_RISE          (CAPTURE_2304_HRESHAPE_FALL+0x100)  /* capture Hd reshap rise 0x90b0*/                                     
#define CAPTURE_2304_HOFFSET     			(CAPTURE_2304_HRESHAPE_RISE+0x100)	/* capturee Hd valid pixel data offset position  0x90a6 hoffset*/          

#define CAPTURE_2304_VSIZE 					1296								/* capture vertical size  0x90a8 vsize*/                                       
#define CAPTURE_2304_FRAMETOTAL   			1297								/* capture Vd length 0x908e vtotal no use*/                                
#define CAPTURE_2304_FRAMEBLANK   			1297								/* mipi use > vsize is more safe*/                                         
                                                                                                                 
#define CAPTURE_2304_EXTEND_LINES 			12                                                                                             
#define CAPTURE_2304_VRESHAPE_COUNTER_SEL  	BY_HD 								/* capture Vd reshap control*/                                       
#define CAPTURE_2304_VRESHAPE_FALL		  	CAPTURE_2304_EXTEND_LINES+0			/* capture Vd reshap fall  0x90b6*/                        
#define CAPTURE_2304_VRESHAPE_RISE		  	CAPTURE_2304_EXTEND_LINES+1			/* capture Vd reshap rise  0x90b4*/                        
#define CAPTURE_2304_VOFFSET     			CAPTURE_2304_EXTEND_LINES+2			/* capture Vd valid line data offset position  0x90aa voffset*/
/*******************************************************/

#define PREVIEW_2064_FPS30_HSIZE 			2064								/* preview horiz. size*/ 
#define PREVIEW_2064_FPS30_LINEBLANK   		6									/* preview Hd sync length  0x908c hblank*/

#define PREVIEW_2064_FPS30_LINETOTAL   		(PREVIEW_2064_FPS30_HSIZE*3)		/* preview Hd length  0x9088-htotal*/
#define PREVIEW_2064_FPS30_HRESHAPE_FALL   	0x2bc								/* preview Hd reshap fall  0x90b2*/
#define PREVIEW_2064_FPS30_HRESHAPE_RISE    (PREVIEW_2064_FPS30_HRESHAPE_FALL+0x100)/* preview Hd reshap rise 0x90b0*/
#define PREVIEW_2064_FPS30_HOFFSET     		(PREVIEW_2064_FPS30_HRESHAPE_RISE+0x100)

#define PREVIEW_2064_FPS30_VSIZE 			1290								/* preview vertical size  0x90a8 vsize*/
#define PREVIEW_2064_FPS30_FRAMETOTAL   	1291								/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_2064_FPS30_FRAMEBLANK   	1291								/* mipi use > vsize is more safe*/
#define PREVIEW_2064_FPS30_EXTEND_LINES 	12
#define PREVIEW_2064_FPS30_VRESHAPE_COUNTER_SEL	BY_HD 							/* preview Vd reshap control*/
#define PREVIEW_2064_FPS30_VRESHAPE_FALL	PREVIEW_2064_FPS30_EXTEND_LINES+0	/* preview Vd reshap fall  0x90b6*/
#define PREVIEW_2064_FPS30_VRESHAPE_RISE	PREVIEW_2064_FPS30_EXTEND_LINES+1	/* preview Vd reshap rise  0x90b4*/
#define PREVIEW_2064_FPS30_VOFFSET     		PREVIEW_2064_FPS30_EXTEND_LINES+1	/* preview Vd valid line data offset position  0x90aa voffset*/
/*******************************************************/

#define CAPTURE_2064_HSIZE 					2064								/* capture horiz. size*/                                                                                             
#define CAPTURE_2064_LINEBLANK   			6									/* capture Hd sync length  0x908c hblank*/                                     

#define CAPTURE_2064_LINETOTAL   			(CAPTURE_2064_HSIZE*3)				/* capture Hd length  0x9088-htotal*/  
#define CAPTURE_2064_HRESHAPE_FALL          0x2bc								/* capture Hd reshap fall  0x90b2*/                                    
#define CAPTURE_2064_HRESHAPE_RISE          (CAPTURE_2064_HRESHAPE_FALL+0x100)	/* capture Hd reshap rise 0x90b0*/ 
#define CAPTURE_2064_HOFFSET     			(CAPTURE_2064_HRESHAPE_RISE+0x100)  /* capturee Hd valid pixel data offset position  0x90a6 hoffset*/          
                                                                                                                                  
#define CAPTURE_2064_VSIZE 					1290								/* capture vertical size  0x90a8 vsize*/                                       
#define CAPTURE_2064_FRAMETOTAL   			1291								/* capture Vd length 0x908e vtotal no use*/                                
#define CAPTURE_2064_FRAMEBLANK   			1291								/* mipi use > vsize is more safe*/                                                                                                                                     
#define CAPTURE_2064_EXTEND_LINES 			12                                                                                             
#define CAPTURE_2064_VRESHAPE_COUNTER_SEL  	BY_HD 								/* capture Vd reshap control*/                                       
#define CAPTURE_2064_VRESHAPE_FALL		  	CAPTURE_2064_EXTEND_LINES+0			/* capture Vd reshap fall  0x90b6*/                        
#define CAPTURE_2064_VRESHAPE_RISE		  	CAPTURE_2064_EXTEND_LINES+1			/* capture Vd reshap rise  0x90b4*/                        
#define CAPTURE_2064_VOFFSET     			CAPTURE_2064_EXTEND_LINES+1			/* capture Vd valid line data offset position  0x90aa voffset*/
/*******************************************************/

#define PREVIEW_2048_FPS30_HSIZE 				2048								/* preview horiz. size*/ 
#define PREVIEW_2048_FPS30_LINEBLANK   		    6									/* preview Hd sync length  0x908c hblank*/
#if lum_2048x1536
#define PREVIEW_2048_FPS30_LINETOTAL   		    (PREVIEW_2048_FPS30_HSIZE*3)	
#define PREVIEW_2048_FPS30_HRESHAPE_FALL   	    0x384								/* preview Hd reshap fall  0x90b2*/
#define PREVIEW_2048_FPS30_HRESHAPE_RISE    	(PREVIEW_2048_FPS30_HRESHAPE_FALL+0x100)/* preview Hd reshap rise 0x90b0*/
#define PREVIEW_2048_FPS30_HOFFSET     		    (PREVIEW_2048_FPS30_HRESHAPE_RISE+0x100)
#define PREVIEW_2048_FPS30_VSIZE 				1536								/* preview vertical size  0x90a8 vsize*/
#elif lum_2048x1152
#define PREVIEW_2048_FPS30_LINETOTAL   		    (PREVIEW_2048_FPS30_HSIZE*3)	
#define PREVIEW_2048_FPS30_HRESHAPE_FALL   	    0x800								/* preview Hd reshap fall  0x90b2*/
#define PREVIEW_2048_FPS30_HRESHAPE_RISE    	(PREVIEW_2048_FPS30_HRESHAPE_FALL+0x100)/* preview Hd reshap rise 0x90b0*/
#define PREVIEW_2048_FPS30_HOFFSET     		    (PREVIEW_2048_FPS30_HRESHAPE_RISE+0x100)
#define PREVIEW_2048_FPS30_VSIZE 				1152								/* preview vertical size  0x90a8 vsize*/
#elif lum_2048x1280
#define PREVIEW_2048_FPS30_LINETOTAL   		    (PREVIEW_2048_FPS30_HSIZE*3)	
#define PREVIEW_2048_FPS30_HRESHAPE_FALL   	    0x3E8								/* preview Hd reshap fall  0x90b2*/
#define PREVIEW_2048_FPS30_HRESHAPE_RISE    	(PREVIEW_2048_FPS30_HRESHAPE_FALL+0x100)/* preview Hd reshap rise 0x90b0*/
#define PREVIEW_2048_FPS30_HOFFSET     		    (PREVIEW_2048_FPS30_HRESHAPE_RISE+0x100)
#define PREVIEW_2048_FPS30_VSIZE 				1280								/* preview vertical size  0x90a8 vsize*/
#endif

#define PREVIEW_2048_FPS30_FRAMETOTAL   		(PREVIEW_2048_FPS30_VSIZE+1)		/* preview Vd length 0x908e vtotal no use*/
#define PREVIEW_2048_FPS30_FRAMEBLANK   		(PREVIEW_2048_FPS30_VSIZE+1)		/* mipi use > vsize is more safe*/

#define PREVIEW_2048_FPS30_EXTEND_LINES 		12
#define PREVIEW_2048_FPS30_VRESHAPE_COUNTER_SEL	BY_HD 							/* preview Vd reshap control*/
#define PREVIEW_2048_FPS30_VRESHAPE_FALL	PREVIEW_2048_FPS30_EXTEND_LINES+0	/* preview Vd reshap fall  0x90b6*/
#define PREVIEW_2048_FPS30_VRESHAPE_RISE	PREVIEW_2048_FPS30_EXTEND_LINES+1	/* preview Vd reshap rise  0x90b4*/
#define PREVIEW_2048_FPS30_VOFFSET     	    PREVIEW_2048_FPS30_EXTEND_LINES+2	/* preview Vd valid line data offset position  0x90aa voffset*/
/*******************************************************/

#define CAPTURE_2048_HSIZE 					2048								/* capture horiz. size*/                                                                                             
#define CAPTURE_2048_LINEBLANK   			6									/* capture Hd sync length  0x908c hblank*/                                                                              

#if lum_2048x1536
#define CAPTURE_2048_LINETOTAL   		    (CAPTURE_2048_HSIZE*3)	
#define CAPTURE_2048_HRESHAPE_FALL   	    0x384								/* preview Hd reshap fall  0x90b2*/
#define CAPTURE_2048_HRESHAPE_RISE    	    (CAPTURE_2048_HRESHAPE_FALL+0x100)  /* preview Hd reshap rise 0x90b0*/
#define CAPTURE_2048_HOFFSET     		    (CAPTURE_2048_HRESHAPE_RISE+0x100)
#define CAPTURE_2048_VSIZE 				    1536								/* preview vertical size  0x90a8 vsize*/
#elif lum_2048x1152
#define CAPTURE_2048_LINETOTAL   		    (CAPTURE_2048_HSIZE*3)	
#define CAPTURE_2048_HRESHAPE_FALL   	    0x800								/* preview Hd reshap fall  0x90b2*/
#define CAPTURE_2048_HRESHAPE_RISE    	    (CAPTURE_2048_HRESHAPE_FALL+0x100)  /* preview Hd reshap rise 0x90b0*/
#define CAPTURE_2048_HOFFSET     		    (CAPTURE_2048_HRESHAPE_RISE+0x100)
#define CAPTURE_2048_VSIZE 				    1152								/* preview vertical size  0x90a8 vsize*/
#elif lum_2048x1280
#define CAPTURE_2048_LINETOTAL   		    (CAPTURE_2048_HSIZE*3)	
#define CAPTURE_2048_HRESHAPE_FALL   	    0x3E8								/* preview Hd reshap fall  0x90b2*/
#define CAPTURE_2048_HRESHAPE_RISE    	    (CAPTURE_2048_HRESHAPE_FALL+0x100)  /* preview Hd reshap rise 0x90b0*/
#define CAPTURE_2048_HOFFSET     		    (CAPTURE_2048_HRESHAPE_RISE+0x100)
#define CAPTURE_2048_VSIZE 				    1280								/* preview vertical size  0x90a8 vsize*/
#endif

#define CAPTURE_2048_FRAMETOTAL   		    (CAPTURE_2048_VSIZE+1)				/* preview Vd length 0x908e vtotal no use*/
#define CAPTURE_2048_FRAMEBLANK   		    (CAPTURE_2048_VSIZE+1)				/* mipi use > vsize is more safe*/

#define CAPTURE_2048_EXTEND_LINES 			12                                                                                             
#define CAPTURE_2048_VRESHAPE_COUNTER_SEL  	BY_HD 								/* capture Vd reshap control*/                                       
#define CAPTURE_2048_VRESHAPE_FALL		  	CAPTURE_2048_EXTEND_LINES+0			/* capture Vd reshap fall  0x90b6*/                        
#define CAPTURE_2048_VRESHAPE_RISE		  	CAPTURE_2048_EXTEND_LINES+1			/* capture Vd reshap rise  0x90b4*/                        
#define CAPTURE_2048_VOFFSET     			CAPTURE_2048_EXTEND_LINES+2			/* capture Vd valid line data offset position  0x90aa voffset*/
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
#define SENSORMODEL		TYPE_CMOS	   
#define	TGTYPE			TG_CMOS_SP5K_SLAVE 

#define PREVIEW_WVGA_FPS60_CFAPATTERN 	BAYER_GRBG
#define PREVIEW_1080_FPS30_CFAPATTERN 	BAYER_GRBG

#define PREVIEW_2048_FPS30_CFAPATTERN	BAYER_GRBG
#define PREVIEW_2304_FPS30_CFAPATTERN	BAYER_GRBG
#define PREVIEW_2064_FPS30_CFAPATTERN	BAYER_GRBG

#define	SNAP_CFAPATTERN					BAYER_GRBG        /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM	0			/*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM	1	        /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM	2	        /*for typical CCD==n_dummy + n_readout + 1*/

#define	SNAP_EARLY_NUM		        0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		2	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/

#define	FLDSEL_START_NUM	0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM		0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */

#define	SKIP_SNAP_READOUTVD_EN   0 /*0*/		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/

#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	0		/*sony FALSE;panasonic/ITG TRUE*/


#define	PREVIEW_HDNUM_FRAME_START    110			/*the front interrupt line number of the preview new frame*/

#define	PREVIEW_WVGA_FPS60_HDNUM_FRAME_END   (PREVIEW_WVGA_FPS60_VOFFSET+PREVIEW_WVGA_FPS60_VSIZE-1)			/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_WVGA_FPS60_HDNUM_CMDPORT	 PREVIEW_WVGA_FPS60_HDNUM_FRAME_END 	/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_1080_FPS30_HDNUM_FRAME_END   2		/*the rear interrupt line number of the preview new frame*/
#define	PREVIEW_1080_FPS30_HDNUM_CMDPORT	 500 	/*the command port interrupt line number of the preview new frame*/

#define	PREVIEW_2048_FPS30_HDNUM_FRAME_START  	110	/*the front interrupt line number of the snap new frame */
#define	PREVIEW_2048_FPS30_HDNUM_FRAME_END	 	2   /*the rear interrupt line number of the snap new frame*/
#define	PREVIEW_2048_FPS30_HDNUM_CMDPORT	 	300	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_2304_FPS30_HDNUM_FRAME_START  	110	/*the front interrupt line number of the snap new frame */
#define	PREVIEW_2304_FPS30_HDNUM_FRAME_END	 	2   /*the rear interrupt line number of the snap new frame*/
#define	PREVIEW_2304_FPS30_HDNUM_CMDPORT	 	300	/*the command port interrupt line number of the snap new frame*/

#define	PREVIEW_2064_FPS30_HDNUM_FRAME_START  	110	/*the front interrupt line number of the snap new frame */
#define	PREVIEW_2064_FPS30_HDNUM_FRAME_END	 	2   /*the rear interrupt line number of the snap new frame*/
#define	PREVIEW_2064_FPS30_HDNUM_CMDPORT	 	300	/*the command port interrupt line number of the snap new frame*/

#define	CAPTURE_HDNUM_FRAME_START  				110				/*the front interrupt line number of the snap new frame */
#define	CAPTURE_HDNUM_FRAME_END	 				2      /*the rear interrupt line number of the snap new frame*/
#define	CAPTURE_HDNUM_CMDPORT	 			 	300	/*the command port interrupt line number of the snap new frame*/

#define	EXP_FRAME_NUM       	1	    /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/

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


#if PROF_LOG_EN
static UINT32 g_cap_count=0;
#endif

#if S2S_TIME_LOG
static UINT32 g_time_log[10];
#endif

#define MI_REG(a,b) { (a)>>8, (a)&0xFF, (b)>>8, (b)&0xFF, }

extern UINT32 tmrTimeClockGet();
extern UINT32 sensor_event_wait(UINT32 event);

/*prototypes of sensor model methods*/

static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();

#if HAS_STREAM_OFF
static void sensorDoFuncStreamOff(UINT32 abort);
static void sensorDoFuncStreamOn (UINT32 abort);
#endif

static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);

static void sensorDoFuncPreview1080PFps30Begin();
static void sensorDoFuncPreviewWVgaFps60Begin();
static void sensorDoFuncPreview2048Fps30Begin();
static void sensorDoFuncPreview2304Fps30Begin();
static void sensorDoFuncPreview2064Fps30Begin();

static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();

static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapExpBegin1();
static void sensorDoFuncSnapExpBegin2();

static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();

static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();

/*static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapEarly1();*/

static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
/*static void sensorDoFuncSnapBshutterEnd();*/

static void sensorDoFuncSnap_dark_BshutterEnd();

static void sensorDoFuncSnapNext(UINT32 next_mode);
static void sensorDoFuncSnapNext1(UINT32 next_mode);


/* Aptina AR0330 setting define */
static const UINT8 cmd_init[][4] SENSOR_SIF1DMA_BUF= {
	MI_REG( 0x301A, 0x005D	),	/*//Reset Sensor																		*/
	MI_REG( 0x31AE,0x304	),	/*//Output Interface Configured to Parallel 		*/
	MI_REG( 0x301A, 0x004D	),	/*//BITFIELD=0x301A,0x0004,0	//Disable Streaming   */
	MI_REG( 0x301A, 0x0058	),	/*//BITFIELD=0x301A,0x0040,1	//Drive Pins		  */
	MI_REG( 0x3064, 0x1802	),	/*//Disable Embedded Data							*/
														/*/LOAD = default registers */						 
	MI_REG(0x3078,0x0001	),	/*//Marker to say that 'Defaults' have been run 	  */
	
	/*LOAD = Enable DevWare Colorpipe CCM and AWB settings*/
	/*LOAD = 2304x1296 60fps 12bit - HiSPi*/
	/*[WVGA 1152x648 60fps 12bit - HiSPi]  */	
	/*//IF_REG = 0x3078, 0xFFF, == 0, LOAD=Demo Initialization - 4 lane HiSPi	  */
	MI_REG(0x301A, 0x0058 	),	/* //BITFIELD=0x301A,0x0004,0	// Disable Streaming */
	MI_REG(0x31AE, 0x304	), 	/* // HiSPi Output	*/
	
	/*LOAD=Serial 12-bit PLL 196MHz*/
	/*[Serial 12-bit PLL 196MHz]	*/
	/*// Assuming Input Clock of 24MHz.  Output Clock will be 96MHz 	*/
	/*STATE= Master Clock, 196000000 	LOAD=MIPI Timing - 12bit 588Mbps*/
	MI_REG(0x302A, 6	),		/* // vt_pix_clk_div		*/
	MI_REG(0x302C, 1	),		/* // vt_sys_clk_div		*/
	MI_REG(0x302E, 4	),		/* // pre_pll_clk_div		*/
	MI_REG(0x3030, 49), 		/* // pll_multiplier			*/
	MI_REG(0x3036, 12), 		/* // op_pix_clk_div		*/
	MI_REG(0x3038, 1	),		/* // op_sys_clk_div		*/
	MI_REG(0x31AC, 0xC0C	),	/* // data_format is 12-bit 	*/
	/*LOAD=MIPI Timing - 12bit 588Mbps*/
	/*								  */
	/*[MIPI Timing - 12bit 588Mbps]   */
	MI_REG(0x31AC, 0xC0C),		/* // DATA FORMAT BITS	*/
	MI_REG(0x31B0, 36	),		/* // FRAME PREAMBLE		*/
	MI_REG(0x31B2, 12	),		/* // LINE PREAMBLE 		*/
	MI_REG(0x31B4, 0x2643),		/* // MIPI TIMING 0 		*/
	MI_REG(0x31B6, 0x114E),		/* // MIPI TIMING 1 		*/
	MI_REG(0x31B8, 0x2048), 	/* // MIPI TIMING 2 		*/
	MI_REG(0x31BA, 0x0186),		/* // MIPI TIMING 3 		*/
	MI_REG(0x31BC, 0x8005), 	/* // MIPI TIMING 4 		*/
	MI_REG(0x31BE, 0x2003),		/* // MIPI CONFIG STATUS	*/
	
	/*//ARRAY READOUT SETTINGS*/
	MI_REG(0x3004, 0	 ),	  	/* // X_ADDR_START		*/
	MI_REG(0x3008, 2303  ),	  	/* // X_ADDR_END		*/
	MI_REG(0x3002, 120	 ),	  	/* // Y_ADDR_START		*/
	MI_REG(0x3006, 1415  ),	  	/* // Y_ADDR_END		*/

	/*//Sub-sampling	  */
	MI_REG(0x30A2, 1 	), 	/*// X_ODD_INCREMENT*/
	MI_REG(0x30A6, 1 	), 	/*// Y_ODD_INCREMENT*/
	MI_REG(0x3040, 0x0000), /*// READ_MODE 	 	*/

	
	 /*//Frame-Timing	 */  
	MI_REG(0x300C, 1242 	),	/* // LINE_LENGTH_PCK	*/
	MI_REG(0x300A, 1315 	),	/* // FRAME_LENGTH_LINES	*/
	MI_REG(0x3014, 0		),	/* // FINE_INTEGRATION_TIME 	*/
	MI_REG(0x3012, 1307 	),	/* // Coarse_Integration_Time	*/
	MI_REG(0x3042, 949		),	/* // EXTRA_DELAY		*/
	MI_REG(0x30BA, 0x002C	),	/* //BITFIELD=0x30BA,0x0040,0	// Digital_Ctrl_Adc_High_Speed */										 

};

  static const UINT8 cmd_init1[][4] SENSOR_SIF1DMA_BUF= {

		MI_REG(0x301A, 0x0048),	/*BITFIELD=0x301A,0x0004,0  //Disable Streaming*/
		/*DELAY=100*/
		MI_REG(0x3088, 0x8000),
		MI_REG(0x3086, 0x4A03),
		MI_REG(0x3086, 0x4316),
		MI_REG(0x3086, 0x0443),
		MI_REG(0x3086, 0x1645),
		MI_REG(0x3086, 0x4045),
		MI_REG(0x3086, 0x6017),
		MI_REG(0x3086, 0x2045),  
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
		MI_REG(0x3086, 0x2684),
		MI_REG(0x3086, 0x2027),
		MI_REG(0x3086, 0xFC53),
		MI_REG(0x3086, 0x0D5C),
		MI_REG(0x3086, 0x0D57),
		MI_REG(0x3086, 0x5417),
		MI_REG(0x3086, 0x0955),
		MI_REG(0x3086, 0x5649),
		MI_REG(0x3086, 0x5307),
		MI_REG(0x3086, 0x5302),
		MI_REG(0x3086, 0x4D28),
		MI_REG(0x3086, 0x6C4C),
		MI_REG(0x3086, 0x0928),
		MI_REG(0x3086, 0x2C28),
		MI_REG(0x3086, 0x294E),
		MI_REG(0x3086, 0x5C09),
		MI_REG(0x3086, 0x6045),
		MI_REG(0x3086, 0x0045),
		MI_REG(0x3086, 0x8026),
		MI_REG(0x3086, 0xA627),
		MI_REG(0x3086, 0xF817),
		MI_REG(0x3086, 0x0227),
		MI_REG(0x3086, 0xFA5C),
		MI_REG(0x3086, 0x0B17),
		MI_REG(0x3086, 0x1826),
		MI_REG(0x3086, 0xA25C),
		MI_REG(0x3086, 0x0317),
		MI_REG(0x3086, 0x4427),
		MI_REG(0x3086, 0xF25F),
		MI_REG(0x3086, 0x2809),
		MI_REG(0x3086, 0x1714),
		MI_REG(0x3086, 0x2808),
		MI_REG(0x3086, 0x1701),
		MI_REG(0x3086, 0x4D1A),
		MI_REG(0x3086, 0x2683),
		MI_REG(0x3086, 0x1701),
		MI_REG(0x3086, 0x27FA),
		MI_REG(0x3086, 0x45A0),
		MI_REG(0x3086, 0x1707),
		MI_REG(0x3086, 0x27FB),
		MI_REG(0x3086, 0x1729),
		MI_REG(0x3086, 0x4580),
		MI_REG(0x3086, 0x1708),
		MI_REG(0x3086, 0x27FA),
		MI_REG(0x3086, 0x1728),
		MI_REG(0x3086, 0x5D17),
		MI_REG(0x3086, 0x0E26),
		MI_REG(0x3086, 0x8153),
		MI_REG(0x3086, 0x0117),
		MI_REG(0x3086, 0xE653),
		MI_REG(0x3086, 0x0217),
		MI_REG(0x3086, 0x1026),
		MI_REG(0x3086, 0x8326),
		MI_REG(0x3086, 0x8248),
		MI_REG(0x3086, 0x4D4E),
		MI_REG(0x3086, 0x2809),
		MI_REG(0x3086, 0x4C0B),
		MI_REG(0x3086, 0x6017),
		MI_REG(0x3086, 0x2027),
		MI_REG(0x3086, 0xF217),
		MI_REG(0x3086, 0x535F),
		MI_REG(0x3086, 0x2808),
		MI_REG(0x3086, 0x164D),
		MI_REG(0x3086, 0x1A17),
		MI_REG(0x3086, 0x0127),
		MI_REG(0x3086, 0xFA26),
		MI_REG(0x3086, 0x035C),
		MI_REG(0x3086, 0x0145),
		MI_REG(0x3086, 0x4027),
		MI_REG(0x3086, 0x9817),
		MI_REG(0x3086, 0x2A4A),
		MI_REG(0x3086, 0x0A43),
		MI_REG(0x3086, 0x160B),
		MI_REG(0x3086, 0x4327),
		MI_REG(0x3086, 0x9C45),
		MI_REG(0x3086, 0x6017),
		MI_REG(0x3086, 0x0727),
		MI_REG(0x3086, 0x9D17),
		MI_REG(0x3086, 0x2545),
		MI_REG(0x3086, 0x4017),
		MI_REG(0x3086, 0x0827),
		MI_REG(0x3086, 0x985D),
		MI_REG(0x3086, 0x2645),
		MI_REG(0x3086, 0x4B17),
		MI_REG(0x3086, 0x0A28),
		MI_REG(0x3086, 0x0853),
		MI_REG(0x3086, 0x0D52),
		MI_REG(0x3086, 0x5112),
		MI_REG(0x3086, 0x4460),
		MI_REG(0x3086, 0x184A),
		MI_REG(0x3086, 0x0343),
		MI_REG(0x3086, 0x1604),
		MI_REG(0x3086, 0x4316),
		MI_REG(0x3086, 0x5843),
		MI_REG(0x3086, 0x1659),
		MI_REG(0x3086, 0x4316),
		MI_REG(0x3086, 0x5A43),
		MI_REG(0x3086, 0x165B),
		MI_REG(0x3086, 0x4327),
		MI_REG(0x3086, 0x9C45),
		MI_REG(0x3086, 0x6017),
		MI_REG(0x3086, 0x0727),
		MI_REG(0x3086, 0x9D17),
		MI_REG(0x3086, 0x2545),
		MI_REG(0x3086, 0x4017),
		MI_REG(0x3086, 0x1027),
		MI_REG(0x3086, 0x9817),
		MI_REG(0x3086, 0x2022),
		MI_REG(0x3086, 0x4B12),
		MI_REG(0x3086, 0x442C),
		MI_REG(0x3086, 0x2C2C),
		MI_REG(0x3086, 0x2C00),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),
		MI_REG(0x3086, 0x0000),

		MI_REG( 0x301A, 0x0058 ),    /*  BITFIELD=0x301A,0x0004,1 //Enable Streaming           						*/
		MI_REG(0x31C6,0x402	 ),		/* //HiSPi Control Status - Streaming S                               */
		MI_REG(0x3012,1308	 ),		/* //Set coarse integration time to default frame_length_lines        */
		MI_REG( 0x301A, 0x005C ),		/* //Enable Streaming       */   

		/*Default use the VGA resolution settings*/
};

static const UINT8 cmd_1080_fps30_init[][4] SENSOR_SIF1DMA_BUF = {

	/*[Serial 12-bit PLL 196MHz]	*/
	/*// Assuming Input Clock of 24MHz.  Output Clock will be 96MHz 	*/
	/*STATE= Master Clock, 196000000	LOAD=MIPI Timing - 12bit 588Mbps*/
	MI_REG(0x301A,0x058	),
	MI_REG(0x302A, 6	),	/* // vt_pix_clk_div	*/
	MI_REG(0x302C, 1	),	/* // vt_sys_clk_div	*/
	MI_REG(0x302E, 4	),	/* // pre_pll_clk_div	*/
	MI_REG(0x3030, 49	), 	/* // pll_multiplier		*/
	MI_REG(0x3036, 12	), 	/* // op_pix_clk_div	*/
	MI_REG(0x3038, 1	),	/* // op_sys_clk_div	*/
	MI_REG(0x31AC, 0xC0C),	/* // data_format is 12-bit 	*/

	MI_REG(0x3004, 0	),	/*// X_ADDR_START	*/
	MI_REG(0x3008, 2303 ),	/*// X_ADDR_END  	*/
	MI_REG(0x3002, 120	),	/*// Y_ADDR_START	*/
	MI_REG(0x3006, 1415 ),	/*// Y_ADDR_END  	*/

	/*Sub-sampling*/  
	MI_REG(0x30A2, 1 	), 	/*// X_ODD_INCREMENT*/
	MI_REG(0x30A6, 1 	), 	/*// Y_ODD_INCREMENT*/
	MI_REG(0x3040, 0x0000), /*// READ_MODE 	 	*/


	/*Frame-Timing*/
	MI_REG(0x300C, 1248 ), 	/*// LINE_LENGTH_PCK 	*/					 
	MI_REG(0x300A, 1308 ), 	/*// FRAME_LENGTH_LINES	*/

	MI_REG(0x3014, 0	), 	/*// FINE_INTEGRATION_TIME	*/
	MI_REG(0x3012, 1307 ), 	/*// Coarse_Integration_Time 	*/
	MI_REG(0x3042, 949	), 	/*// EXTRA_DELAY 			*/
	MI_REG(0x30BA, 0x002C), /*//BITFIELD=0x30BA,0x0040,0	 */   
	MI_REG(0x301A, 0x004),

};

static const UINT8 cmd_wvga_fps60_init[][4] SENSOR_SIF1DMA_BUF = {

	/* WVGA 1152x648 60fps 12bit - HiSPi*/
	
	/*New code*/
	MI_REG(0x301A,0x058),
	MI_REG(0x31AE,0x304),
	/*[Serial 12-bit PLL 196MHz]	*/
	/*// Assuming Input Clock of 24MHz.  Output Clock will be 96MHz 	*/
	/*STATE= Master Clock, 196000000	LOAD=MIPI Timing - 12bit 588Mbps*/
	MI_REG(0x302A, 6	),		/* // vt_pix_clk_div	*/
	MI_REG(0x302C, 1	),		/* // vt_sys_clk_div	*/
	MI_REG(0x302E, 4	),		/* // pre_pll_clk_div	*/
	MI_REG(0x3030, 98	), 		/* // pll_multiplier		*/
	MI_REG(0x3036, 12	), 		/* // op_pix_clk_div	*/
	MI_REG(0x3038, 1	),		/* // op_sys_clk_div	*/
	MI_REG(0x31AC, 0xC0C),		/* // data_format is 12-bit 	*/


	/*//ARRAY READOUT SETTINGS*/
	MI_REG(0x3004, 0	),	  	/* // X_ADDR_START	*/
	MI_REG(0x3008, 2303 ),	  	/* // X_ADDR_END	*/
	MI_REG(0x3002, 120	),	  	/* // Y_ADDR_START	*/
	MI_REG(0x3006, 1415 ),	  	/* // Y_ADDR_END	*/

	/*//Sub-sampling	  */
	MI_REG(0x30A2,3 	),		/*// X_ODD_INCREMENT*/
	MI_REG(0x30A6,3 	),		/*// Y_ODD_INCREMENT*/
	MI_REG(0x3040, 0x0000),		/*// READ_MODE	*/

	/*//Frame-Timing	 */  
	MI_REG(0x300C, 1242 ),		/* // LINE_LENGTH_PCK*/
	MI_REG(0x300A, 1315 ),		/* // FRAME_LENGTH_LINES		*/
	MI_REG(0x3014, 0	),		/* // FINE_INTEGRATION_TIME 	*/
	MI_REG(0x3012, 1314 ),		/* // Coarse_Integration_Time	*/
	MI_REG(0x3042, 103	),		/* // EXTRA_DELAY*/
	MI_REG(0x30BA, 0x002C),		/* //BITFIELD=0x30BA,0x0040,0	// Digital_Ctrl_Adc_High_Speed */

	MI_REG(0x301A,0x004),

};

static const UINT8 cmd_2304_fps30_init[][4] SENSOR_SIF1DMA_BUF = {

	/*2304x1296 30 fps*/
	MI_REG(0x301A,0x058	),
	MI_REG(0x302A, 6	),	/* // vt_pix_clk_div	*/
	MI_REG(0x302C, 1	),	/* // vt_sys_clk_div	*/
	MI_REG(0x302E, 4	),	/* // pre_pll_clk_div	*/
	MI_REG(0x3030, 49	), 	/* // pll_multiplier		*/
	MI_REG(0x3036, 12	), 	/* // op_pix_clk_div	*/
	MI_REG(0x3038, 1	),	/* // op_sys_clk_div	*/
	MI_REG(0x31AC, 0xC0C),	/* // data_format is 12-bit 	*/

	MI_REG(0x3004, 0	),	/*// X_ADDR_START	*/
	MI_REG(0x3008, 2303 ),	/*// X_ADDR_END  	*/
	MI_REG(0x3002, 120	),	/*// Y_ADDR_START	*/
	MI_REG(0x3006, 1415 ),	/*// Y_ADDR_END  	*/

	/*Sub-sampling*/  
	MI_REG(0x30A2, 1 	), 	/*// X_ODD_INCREMENT*/
	MI_REG(0x30A6, 1 	), 	/*// Y_ODD_INCREMENT*/
	MI_REG(0x3040, 0x0000), /*// READ_MODE 	 	*/

	/*Frame-Timing*/
	MI_REG(0x300C, 1248 ), 	/*// LINE_LENGTH_PCK 	*/					 
	MI_REG(0x300A, 1308 ), 	/*// FRAME_LENGTH_LINES	*/

	MI_REG(0x3014, 0	), 	/*// FINE_INTEGRATION_TIME	*/
	MI_REG(0x3012, 1307 ), 	/*// Coarse_Integration_Time 	*/
	MI_REG(0x3042, 949	), 	/*// EXTRA_DELAY 			*/
	MI_REG(0x30BA, 0x002C), /*//BITFIELD=0x30BA,0x0040,0	 */   
	MI_REG(0x301A, 0x004),

};

static const UINT8 cmd_2048_fps30_init[][4] SENSOR_SIF1DMA_BUF = {

	MI_REG(0x301A,0x058 ),
	MI_REG(0x302A, 6	),	/* // vt_pix_clk_div	*/
	MI_REG(0x302C, 1	),	/* // vt_sys_clk_div	*/
	MI_REG(0x302E, 4	),	/* // pre_pll_clk_div	*/
#if lum_2048x1536
	MI_REG(0x3030, 58	),	/* // pll_multiplier	*/
#elif lum_2048x1152
    MI_REG(0x3030, 50	),	/* // pll_multiplier	*/
#elif lum_2048x1280
    MI_REG(0x3030, 50	),	/* // pll_multiplier	*/
#endif
	MI_REG(0x3036, 12	),	/* // op_pix_clk_div	*/
	MI_REG(0x3038, 1	),	/* // op_sys_clk_div	*/
	MI_REG(0x31AC, 0xC0C),	/* // data_format is 12-bit 	*/

	MI_REG(0x3004, 134	),	/*// X_ADDR_START	*/
	MI_REG(0x3008, 2181 ),	/*// X_ADDR_END 	*/

#if lum_2048x1536
	MI_REG(0x3002, 6	),	/*// Y_ADDR_START	*/
	MI_REG(0x3006, 1541 ),	/*// Y_ADDR_END 	*/
#elif lum_2048x1152
	MI_REG(0x3002, 198	),	/*// Y_ADDR_START	*/
	MI_REG(0x3006, 1349 ),	/*// Y_ADDR_END 	*/
#elif lum_2048x1280
	MI_REG(0x3002, 134	),	/*// Y_ADDR_START	*/
	MI_REG(0x3006, 1413 ),	/*// Y_ADDR_END 	*/
#endif
	
	/*Sub-sampling*/  
	MI_REG(0x30A2, 1	),	/*// X_ODD_INCREMENT*/
	MI_REG(0x30A6, 1	),	/*// Y_ODD_INCREMENT*/
	MI_REG(0x3040, 0x0000), /*// READ_MODE		*/

	/*Frame-Timing*/
#if lum_2048x1536
	MI_REG(0x300C, 1248 ),	/*// LINE_LENGTH_PCK	*/					 
	MI_REG(0x300A, 1548 ),	/*// FRAME_LENGTH_LINES */
#elif lum_2048x1152
	MI_REG(0x300C, 1431 ),	/*// LINE_LENGTH_PCK	*/					 
	MI_REG(0x300A, 1164 ),	/*// FRAME_LENGTH_LINES */
#elif lum_2048x1280
	MI_REG(0x300C, 1289 ),	/*// LINE_LENGTH_PCK	*/					 
	MI_REG(0x300A, 1292 ),	/*// FRAME_LENGTH_LINES */
#endif
	
	MI_REG(0x3014, 0	),	/*// FINE_INTEGRATION_TIME	*/
	MI_REG(0x3012, 1547 ),	/*// Coarse_Integration_Time	*/

#if lum_2048x1536
    MI_REG(0x3042, 1429	),	/*// EXTRA_DELAY			*/
#elif lum_2048x1152
    MI_REG(0x3042, 982	),	/*// EXTRA_DELAY			*/
#elif lum_2048x1280
    MI_REG(0x3042, 1278	),	/*// EXTRA_DELAY			*/
#endif	
	
	MI_REG(0x30BA, 0x002C), /*//BITFIELD=0x30BA,0x0040,0	 */   
	MI_REG(0x301A, 0x004),

};

static const UINT8 cmd_stream_on[][4] SENSOR_SIF1DMA_BUF = {
		MI_REG(0x301A, 0x005C),
};

static const UINT8 cmd_stream_off[][4] SENSOR_SIF1DMA_BUF = {
		MI_REG(0x301A, 0x0058),
};

/*sensor HiSPi cfg*/
static const sensorLVDSCfg_t sensorHiSpiCfg =
{
	.wordcntsel 	 =1 ,	/*Hispi hsize from 0x90a4*/
	.Lvdsmode		 =1 ,	/*12bit mode*/
	.Lvdslanenum	 =0 ,	/*4 lane mode*/
	.Lvdslanemode	 =0 ,	/*enable lane3 in 4 lanes mode*/
	.Lvdslaneswap	 =0 ,	/*MSB first */
	.Lvdsraw		 =1 ,	/*raw 12 */
	.Lvdseofsel 	 =2 ,	/*0 detect eof sync code DIFF2*/
	.Lvdseolsel 	 =0 ,	/*detect eol sync code */
	.lvdsvdsrc		 =1 ,	/*1: decode emedded data header sof as vd source*/
	.lvds_dphy_sel	 =1 ,	/*MIPI D-PHY*/
	.Ctrlword_sol	 =0x800,
	.Ctrlword_sof	 =0xc00,/* Hd  mode = 0xc00 full resoultion mode =0xc80*/ /* full mode  0xc00*/
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
	.Exthd_num		 =CAPTURE_2304_EXTEND_LINES,
	.Exthd_pol		 =0 ,	
	.Exthd_mode 	 =0
};

/*sensor cfg*/
static const sensorTgPreviewCfg_t sensorTgPreviewVgaFps60Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewWVgaFps60Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_WVGA_FPS60_HSIZE,
	.vsize = PREVIEW_WVGA_FPS60_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_WVGA_FPS60_CFAPATTERN,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080Fps30PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview1080PFps30Begin,
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
};

static const sensorTgPreviewCfg_t sensorTgPreview2048Fps30PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview2048Fps30Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_2048_FPS30_HSIZE,
	.vsize = PREVIEW_2048_FPS30_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_2048_FPS30_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 29970000,
	.sensor_pclk = 54000000,
	.sensor_htotal = 6400, 	
};

static const sensorTgPreviewCfg_t sensorTgPreview2304Fps30PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview2304Fps30Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_2304_FPS30_HSIZE,
	.vsize = PREVIEW_2304_FPS30_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_2304_FPS30_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 29970000,
	.sensor_pclk = 49000000,
	.sensor_htotal = 6400, 
};

static const sensorTgPreviewCfg_t sensorTgPreview2064Fps30PCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview2064Fps30Begin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = NULL,
	.hsize = PREVIEW_2064_FPS30_HSIZE,
	.vsize = PREVIEW_2064_FPS30_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_2064_FPS30_CFAPATTERN,
	.hratio = 3200,
	.vratio = 2000,
	.fps_mhz = 29970000,
	.sensor_pclk = 49000000,
	.sensor_htotal = 6400, 
};

/*mode*/
static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {

	/*&sensorTgPreview1080Fps30PCfg,*/
	/*&sensorTgPreview1080Fps30PCfg, &sensorTgPreviewVgaFps60Cfg, */
	&sensorTgPreview2048Fps30PCfg, &sensorTgPreview2304Fps30PCfg, &sensorTgPreview2064Fps30PCfg,
} ;
/*#endif*/

static sensorTgSnapCfg_t sensorTgSnapRollCfg_2304 = {
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
			.snapNext = sensorDoFuncSnapNext1,
		},
		.crop = NULL, /****TODO***/
		.hsize = CAPTURE_2304_HSIZE,
		.vsize = CAPTURE_2304_VSIZE,
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

static sensorTgSnapCfg_t sensorTgSnapRollCfg_2064 = {
	.fpdo = &(const sensorSnapDo_t) {
			#if 0
			.psnapEarly = (sensorDoFunc_t []){
				sensorDoFuncSnapEarly0,
			},
			#else
			.psnapEarly = NULL,
			#endif
			
			.snapExpBegin = sensorDoFuncSnapExpBegin2,
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
			.snapNext = sensorDoFuncSnapNext1,
		},
		.crop = NULL, /****TODO***/
		.hsize = CAPTURE_2064_HSIZE,
		.vsize = CAPTURE_2064_VSIZE,
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

static sensorTgSnapCfg_t sensorTgSnapRollCfg_2048 = {
	.fpdo = &(const sensorSnapDo_t) {
			#if 0
			.psnapEarly = (sensorDoFunc_t []){
				sensorDoFuncSnapEarly0,
			},
			#else
			.psnapEarly = NULL,
			#endif
			
			.snapExpBegin = sensorDoFuncSnapExpBegin1,
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
		.hsize = CAPTURE_2048_HSIZE,
		.vsize = CAPTURE_2048_VSIZE,
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
	/*0x40: 2048x1536 / 0x41: 2304x1296*/
	  &sensorTgSnapRollCfg_2048, &sensorTgSnapRollCfg_2304, &sensorTgSnapRollCfg_2064,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewWVgaFps60Cfg = {
	.linetotal       = PREVIEW_WVGA_FPS60_LINETOTAL,
	.lineblank       = PREVIEW_WVGA_FPS60_LINEBLANK,
	.frametotal      = PREVIEW_WVGA_FPS60_FRAMETOTAL,
	.frameblank      = PREVIEW_WVGA_FPS60_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_WVGA_FPS60_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_WVGA_FPS60_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_WVGA_FPS60_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_WVGA_FPS60_VRESHAPE_RISE,
	.hoffset         = PREVIEW_WVGA_FPS60_HOFFSET,
	.voffset         = PREVIEW_WVGA_FPS60_VOFFSET,
	.hsize           = PREVIEW_WVGA_FPS60_HSIZE,
	.vsize           = PREVIEW_WVGA_FPS60_VSIZE,
	.lastlinepix     = PREVIEW_WVGA_FPS60_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_WVGA_FPS60_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_WVGA_FPS60_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_WVGA_FPS60_HDNUM_CMDPORT,
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

static const sensorSyncSigCfg_t sensorSyncSigPreview2048Fps25Cfg = {
	.linetotal       = PREVIEW_2048_FPS30_LINETOTAL,
	.lineblank       = PREVIEW_2048_FPS30_LINEBLANK,
	.frametotal      = PREVIEW_2048_FPS30_FRAMETOTAL,
	.frameblank      = PREVIEW_2048_FPS30_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_2048_FPS30_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_2048_FPS30_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_2048_FPS30_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_2048_FPS30_VRESHAPE_RISE,
	.hoffset         = PREVIEW_2048_FPS30_HOFFSET,
	.voffset         = PREVIEW_2048_FPS30_VOFFSET,
	.hsize           = PREVIEW_2048_FPS30_HSIZE,
	.vsize           = PREVIEW_2048_FPS30_VSIZE,
	.lastlinepix     = PREVIEW_2048_FPS30_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_2048_FPS30_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_2048_FPS30_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_2048_FPS30_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_2048_FPS30_HDNUM_CMDPORT,

};

static const sensorSyncSigCfg_t sensorSyncSigPreview2304Fps30Cfg = {
	.linetotal       = PREVIEW_2304_FPS30_LINETOTAL,
	.lineblank       = PREVIEW_2304_FPS30_LINEBLANK,
	.frametotal      = PREVIEW_2304_FPS30_FRAMETOTAL,
	.frameblank      = PREVIEW_2304_FPS30_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_2304_FPS30_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_2304_FPS30_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_2304_FPS30_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_2304_FPS30_VRESHAPE_RISE,
	.hoffset         = PREVIEW_2304_FPS30_HOFFSET,
	.voffset         = PREVIEW_2304_FPS30_VOFFSET,
	.hsize           = PREVIEW_2304_FPS30_HSIZE,
	.vsize           = PREVIEW_2304_FPS30_VSIZE,
	.lastlinepix     = PREVIEW_2304_FPS30_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_2304_FPS30_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_2304_FPS30_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_2304_FPS30_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_2304_FPS30_HDNUM_CMDPORT,

};

static const sensorSyncSigCfg_t sensorSyncSigPreview2064Fps30Cfg = {
	.linetotal       = PREVIEW_2064_FPS30_LINETOTAL,
	.lineblank       = PREVIEW_2064_FPS30_LINEBLANK,
	.frametotal      = PREVIEW_2064_FPS30_FRAMETOTAL,
	.frameblank      = PREVIEW_2064_FPS30_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_2064_FPS30_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_2064_FPS30_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_2064_FPS30_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_2064_FPS30_VRESHAPE_RISE,
	.hoffset         = PREVIEW_2064_FPS30_HOFFSET,
	.voffset         = PREVIEW_2064_FPS30_VOFFSET,
	.hsize           = PREVIEW_2064_FPS30_HSIZE,
	.vsize           = PREVIEW_2064_FPS30_VSIZE,
	.lastlinepix     = PREVIEW_2064_FPS30_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_2064_FPS30_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_2064_FPS30_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_2064_FPS30_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_2064_FPS30_HDNUM_CMDPORT,

};

static sensorSyncSigCfg_t sensorSyncSigSnapCfg_2304 = {
	.linetotal       = CAPTURE_2304_LINETOTAL,
	.lineblank       = CAPTURE_2304_LINEBLANK,
	.frametotal      = CAPTURE_2304_FRAMETOTAL,
	.frameblank      = CAPTURE_2304_FRAMEBLANK,
	.hreshape_fall   = CAPTURE_2304_HRESHAPE_FALL,
	.hreshape_rise   = CAPTURE_2304_HRESHAPE_RISE,
	.vreshape_fall   = CAPTURE_2304_VRESHAPE_FALL,
	.vreshape_rise   = CAPTURE_2304_VRESHAPE_RISE,
	.hoffset         = CAPTURE_2304_HOFFSET,
	.voffset         = CAPTURE_2304_VOFFSET,
	.hsize           = CAPTURE_2304_HSIZE,
	.vsize           = CAPTURE_2304_VSIZE,
	.lastlinepix     = CAPTURE_2304_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = CAPTURE_2304_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
};

static sensorSyncSigCfg_t sensorSyncSigSnapCfg_2064 = {
	.linetotal       = CAPTURE_2064_LINETOTAL,
	.lineblank       = CAPTURE_2064_LINEBLANK,
	.frametotal      = CAPTURE_2064_FRAMETOTAL,
	.frameblank      = CAPTURE_2064_FRAMEBLANK,
	.hreshape_fall   = CAPTURE_2064_HRESHAPE_FALL,
	.hreshape_rise   = CAPTURE_2064_HRESHAPE_RISE,
	.vreshape_fall   = CAPTURE_2064_VRESHAPE_FALL,
	.vreshape_rise   = CAPTURE_2064_VRESHAPE_RISE,
	.hoffset         = CAPTURE_2064_HOFFSET,
	.voffset         = CAPTURE_2064_VOFFSET,
	.hsize           = CAPTURE_2064_HSIZE,
	.vsize           = CAPTURE_2064_VSIZE,
	.lastlinepix     = CAPTURE_2064_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = CAPTURE_2064_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
};

static sensorSyncSigCfg_t sensorSyncSigSnapCfg_2048 = {
	.linetotal       = CAPTURE_2048_LINETOTAL,
	.lineblank       = CAPTURE_2048_LINEBLANK,
	.frametotal      = CAPTURE_2048_FRAMETOTAL,
	.frameblank      = CAPTURE_2048_FRAMEBLANK,
	.hreshape_fall   = CAPTURE_2048_HRESHAPE_FALL,
	.hreshape_rise   = CAPTURE_2048_HRESHAPE_RISE,
	.vreshape_fall   = CAPTURE_2048_VRESHAPE_FALL,
	.vreshape_rise   = CAPTURE_2048_VRESHAPE_RISE,
	.hoffset         = CAPTURE_2048_HOFFSET,
	.voffset         = CAPTURE_2048_VOFFSET,
	.hsize           = CAPTURE_2048_HSIZE,
	.vsize           = CAPTURE_2048_VSIZE,
	.lastlinepix     = CAPTURE_2048_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 3,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = CAPTURE_2048_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport	 = CAPTURE_HDNUM_CMDPORT,

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
	.defSyncSigCfg  = &sensorSyncSigPreview2048Fps25Cfg,
	.verStr = MODEL_VERSION,
};

void
sensorTg_ar0330_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}


/*------method definitions-----*/
static UINT8 is_firstrun;

#if HAS_STREAM_OFF
static UINT8 is_aborted;

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


static void
sensorDoFuncStreamOff(
		UINT32 abort
		)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	is_aborted = 1;
	
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_off) / sizeof(cmd_stream_off[0]), cmd_stream_off[0], 0);
    tmrUsWait(40000);                         /*avoid to LBUS error (reset cdsp reg) when "pvraw" */  
}

static void
sensorDoFuncStreamOn(
		UINT32 abort
		)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	/*DBG_GPIO(1);*/
	if (abort != 2) {
		tmrUsWait(10000); /*This delay is needed before streaming on.*/
	}
	is_aborted = 0;
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_on) / sizeof(cmd_stream_on[0]), cmd_stream_on[0], 0);
/*	DBG_GPIO(0);*/
}
#endif

#if 0
static void
sensorStreamOff(void)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	
	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_stream_off) / sizeof(cmd_stream_off[0]), cmd_stream_off[0]);
	sensorSerialFlush(cmdportid);
}

static void
sensorStreamOnAndFrameRestart(UINT32 sync)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
	if (sync) {
		sensorSerialDoneForceWaitAll(cmdportid);
	}
	tmrUsWait(10000); /*This delay is needed before streaming on.*/
	if(sensorSerialBulkWrite(cmdportid, sizeof(cmd_stream_on) / sizeof(cmd_stream_on[0]), cmd_stream_on[0], 0)==SUCCESS)
		profLogAdd(1,"sensorStreamOnAndFrameRestart success");
	else
		profLogAdd(1,"sensorStreamOnAndFrameRestart fail");

	sensorRead(0x301a);
}

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
#endif

static void
sensorDoFuncInit()
{

	UINT32 cmdportid;
	PROF_LOG_ADD(1, __FUNCTION__);
	profLogPrintf(1, "________Sensor driver build time <<%s>> \n", BUILD_TIME);

	/* TGGPIO configuration here */
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;
	profLogAdd(1,"APTINA AR0330");
	
	/* halFrontSyncSigCfgSet(FRONT_SYNC_HDVD_RESHAPE_POLAR,1,0);add by stanley inverter fronthd*/

	sensorTgPllClkExtSet(24000000);/*Lowest input clock provided from ASIC is 16MHz - EXTCLK*/

	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	
	/*****TODO*****front reset after these???*/
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

/* Master clock and Pixel clock configuration here */
#if (DDR_CLOCK_297MHz)
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,288000); /* 121 MHz	CDSP 243MHz*/
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9 lester*/
#else
		halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121000); /* 121 MHz	CDSP 243MHz*/
		sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,0); /*enable preview mode 9 lester*/
#endif

	cmdportid = sensorTgCfg.cmdportid_xtg;
	tmrUsWait(2000);
	profLogAdd(1,"aptina AR0330  CLCC");

	/*I2C Init code Here */
	profLogPrintf(1,"sensor DoFunInit model-ID %x",sensorRead(0x3000));
	tmrUsWait(25000); /*25ms*/

	/*TG PLL=MCLK out=24MHz*/
	profLogAdd(1,"cmd_init");
	sensorLVDSCfgSet(&sensorHiSpiCfg);

	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

/*#warning *FIXME* abort test, move to host*/
	printf("*FIXME*%s:%u:abort test, move to host\n",__FILE__,__LINE__);
#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif
/*#warning *FIXME* abort test, move to host*/

#if 1
#if HAS_STREAM_OFF
	sensorStsCfg(SENSOR_SYS_CFG_FLOW_USE_ABORT, HAS_STREAM_OFF);
#endif
#endif

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialBulkWrite(cmdportid, 1, cmd_init[0], 0);
	tmrUsWait(300);
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_init) / sizeof(cmd_init[0]) - 1, cmd_init[1], 0);

	tmrUsWait(1000);
	sensorSerialBulkWrite(cmdportid, 1, cmd_init1[0], 0);
	tmrUsWait(300);
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_init1) / sizeof(cmd_init1[0]) - 1, cmd_init1[1], 0);

	/*sensorStreamOnAndFrameRestart(0);*/

	is_firstrun = 1;	
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
sensorDoFuncPreview2048Fps30Begin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;

	/*SEND I2C COMMAND here */
	profLogAdd(1," 2048x1536_25fps init");

	if(snap_flag)
	{
		sensor_event_wait((1UL<<13));
		snap_flag = 0;
	}
		
#if HAS_STREAM_OFF
	if (is_aborted) {
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigSet;
	}

	fpssg(&sensorSyncSigPreview2048Fps25Cfg);
	
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_2048_fps30_init) / sizeof(cmd_2048_fps30_init[0]), cmd_2048_fps30_init[0]);

	sensorSerialFlush(cmdportid);

	PROF_LOG_ADD(1, " HiSPi 2048x1536_25fps done");

	/*sensorStreamOnAndFrameRestart(1);*/

	/*Change TG Mode Here*/
}

static void
sensorDoFuncPreview2304Fps30Begin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;

	/*SEND I2C COMMAND here */
	profLogAdd(1," 2304x1296_30fps init");

	if(snap_flag)
	{
		sensor_event_wait((1UL<<13));
		snap_flag = 0;
	}
		
#if HAS_STREAM_OFF
	if (is_aborted) {
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigSet;
	}

	fpssg(&sensorSyncSigPreview2304Fps30Cfg);
	
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_2304_fps30_init) / sizeof(cmd_2304_fps30_init[0]), cmd_2304_fps30_init[0]);

	sensorSerialFlush(cmdportid);

	PROF_LOG_ADD(1, " HiSPi 2304x1296_30fps done");

	/*sensorStreamOnAndFrameRestart(1);*/

	/*Change TG Mode Here*/
}

static void
sensorDoFuncPreview2064Fps30Begin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;

	/*SEND I2C COMMAND here */
	profLogAdd(1," 2064x1290_30fps init");

	if(snap_flag)
	{
		sensor_event_wait((1UL<<13));
		snap_flag = 0;
	}
		
#if HAS_STREAM_OFF
	if (is_aborted) {
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigSet;
	}

	fpssg(&sensorSyncSigPreview2064Fps30Cfg);
	
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_2304_fps30_init) / sizeof(cmd_2304_fps30_init[0]), cmd_2304_fps30_init[0]);

	sensorSerialFlush(cmdportid);

	PROF_LOG_ADD(1, " HiSPi 2064x1290_30fps done");

	/*sensorStreamOnAndFrameRestart(1);*/

	/*Change TG Mode Here*/
}


static void
sensorDoFuncPreviewWVgaFps60Begin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;

	/*SEND I2C COMMAND here */
	profLogAdd(3," wvga_60fps init");

	if(snap_flag)
	{
		sensor_event_wait((1UL<<13));
		snap_flag = 0;
	}
		
#if HAS_STREAM_OFF
	if (is_aborted) {
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigSet;
	}

	fpssg(&sensorSyncSigPreviewWVgaFps60Cfg);
	
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_wvga_fps60_init)/sizeof(cmd_wvga_fps60_init[0]), cmd_wvga_fps60_init[0]);

	sensorSerialFlush(cmdportid);

	/*sensorStreamOnAndFrameRestart(1);*/

	PROF_LOG_ADD(3, " HiSPi wvga_60fps done");

	/*Change TG Mode Here*/
}

static void
sensorDoFuncPreview1080PFps30Begin()
{
  /*SSG short*/
  /*SSG windows P*/
  /*TG init*/
  /*CDSP preview prepare*/
  /*statistics prepare*/
  /*front_preview_short_exp_frame_set*/
  /*serial port has fw sync mechanism, do it here instead of after vvalid*/

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;

	/*SEND I2C COMMAND here */
	profLogAdd(3," 1080_fps30 init");

	if(snap_flag)
	{
		sensor_event_wait((1UL<<13));
		snap_flag = 0;
	}
		
#if HAS_STREAM_OFF
	if (is_aborted) {
		fpssg = sensorSyncSigInit;
	} else
#endif
	{
		fpssg = sensorSyncSigSet;
	}
	fpssg(&sensorSyncSigPreview1080Fps30Cfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_1080_fps30_init)/sizeof(cmd_1080_fps30_init[0]), cmd_1080_fps30_init[0]);
	sensorSerialFlush(cmdportid);

	/*sensorStreamOnAndFrameRestart(1);*/

	PROF_LOG_ADD(3, " HiSPi vga_120fps done");


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

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	
	snap_flag = 1;

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_2304_fps30_init) / sizeof(cmd_2304_fps30_init[0]), cmd_2304_fps30_init[0]); 
	sensorSerialFlush(cmdportid);

	#if HAS_STREAM_OFF
		if (is_aborted) {
			fpssg = sensorSyncSigInit;
		} else
	#endif
		{
			fpssg = sensorSyncSigSet;
		}
		fpssg(&sensorSyncSigSnapCfg_2304);

		sensorDsenExtSet();
	
#if HAS_STREAM_OFF
		sensorDoFuncStreamOn(0);
#endif
	
	PROF_LOG_ADD(3, __FUNCTION__);
	PROF_LOG_ADD(3, " HiSPi Snap md_2304_fps30_init done");

}

static void
sensorDoFuncSnapExpBegin1()
{
	/*0x41 4:3*/
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

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	
	snap_flag = 1;

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_2048_fps30_init) / sizeof(cmd_2048_fps30_init[0]), cmd_2048_fps30_init[0]); 
	sensorSerialFlush(cmdportid);

	#if HAS_STREAM_OFF
		if (is_aborted) {
			fpssg = sensorSyncSigInit;
		} else
	#endif
		{
			fpssg = sensorSyncSigSet;
		}
		fpssg(&sensorSyncSigSnapCfg_2048);

		sensorDsenExtSet();
	
#if HAS_STREAM_OFF
		sensorDoFuncStreamOn(0);
#endif
	
	PROF_LOG_ADD(3, __FUNCTION__);
	PROF_LOG_ADD(3, " HiSPi Snap cmd_2048_fps30_init done");

}

static void
sensorDoFuncSnapExpBegin2()
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

	void (*fpssg)(const sensorSyncSigCfg_t *pcfg);
	UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;
	
	snap_flag = 1;

	sensorSerialAsyncWrite(cmdportid, sizeof(cmd_2304_fps30_init) / sizeof(cmd_2304_fps30_init[0]), cmd_2304_fps30_init[0]); 
	sensorSerialFlush(cmdportid);

	#if HAS_STREAM_OFF
		if (is_aborted) {
			fpssg = sensorSyncSigInit;
		} else
	#endif
		{
			fpssg = sensorSyncSigSet;
		}
		fpssg(&sensorSyncSigSnapCfg_2064);

		sensorDsenExtSet();
	
#if HAS_STREAM_OFF
		sensorDoFuncStreamOn(0);
#endif
	
	PROF_LOG_ADD(3, __FUNCTION__);
	PROF_LOG_ADD(3, " HiSPi Snap md_2064_fps30_init done");

}


#if 0
/*Dummy line settings before enter frame mode */
static void
sensorSnapFrameDummySet(
)
{
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
	DBG_GPIO(1);
	snap_flag = 1;
}

static void
sensorDoFuncSnapEarly1()
{
}

static void
sensorDoFuncSnapBshutterEnd()
{
}
#endif

/* add this */
void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param);

static void
sensorDoFuncSnapBshutterBegin()
{
}

static void
sensorDoFuncSnapBshutterFollow()
{
}

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

/*DBG_GPIO(1);*/

#if PROF_LOG_EN
	g_cap_count++;
#endif

	PROF_LOG_ADD(1, __FUNCTION__);
	PROF_LOG_PRINT(3, "sensorDoFuncSnapReadField0 count=%d",g_cap_count);

/*sensorReportsize();*/

}

static void
sensorDoFuncSnapFieldEnd()
{

	#if S2S_TIME_LOG
	g_time_log[7]=tmrTimeClockGet();
	#endif
	/*sensorReportsize();	*/
	/*DBG_GPIO(0);*/
}
 
/*at the readout end of the last field*/
static void
sensorDoFuncSnapReadoutEnd()
{

	/*UINT32 t1;*/
	#if S2S_TIME_LOG
	g_time_log[8]=tmrTimeClockGet();
	g_time_log[9]=tmrTimeClockGet();
	#endif
	
	/*sensorReportsize();*/
}

static void
sensorDoFuncSnapNext(UINT32 mode)
{
#if BURST_CAP_EN
	if(mode==0x40) {
		PROF_LOG_ADD(3,"@@@@ get sensor next mode is snap");
		PROF_LOG_PRINT(3,"@@@@ get sensor next mode is snap snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
		sensorTgSnapRollCfg_2304.skip_snap_readout_vd = 3;/*20110830 @ BASEFW change to 3*/
	}
	else {
		PROF_LOG_ADD(3,"@@@@ get sensor next mode is preview");
		PROF_LOG_PRINT(3,"@@@@ get sensor next mode is preview snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
		sensorTgSnapRollCfg_2304.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN;
	}
#endif

}

static void
sensorDoFuncSnapNext1(UINT32 mode)
{
#if BURST_CAP_EN
	if(mode==0x41) {
		PROF_LOG_ADD(3,"@@@@ get sensor next mode is snap");
		PROF_LOG_PRINT(3,"@@@@ get sensor next mode is snap snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
		sensorTgSnapRollCfg_2048.skip_snap_readout_vd = 3;/*20110830 @ BASEFW change to 3*/
	}
	else {
		PROF_LOG_ADD(3,"@@@@ get sensor next mode is preview");
		PROF_LOG_PRINT(3,"@@@@ get sensor next mode is preview snapnum=%x count=%d ",READ8(0xb000906A),g_cap_count);
		sensorTgSnapRollCfg_2048.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN;
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
	/*return 0;*//*test */
	/*exp setting via serial port*/
	profLogPrintf(0,"prev [nreg=%d][%d %d]",nreg,pdata[0],pdata[1]);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
}

static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
}

#if SP5K_SENSOR_AR0330
void sensorCmdPortExpLineSend(UINT16 expLine)
{

	static UINT8 regdata[][4] SENSOR_SIF1DMA_VARBUF = {
		MI_REG(0x3012,0x0000),
	};
	*(UINT16 *)&regdata[0][2] = ((expLine>>8)&0xff) | ((expLine&0xff)<<8);
	PROF_LOG_PRINT(3,"expLine=%d",expLine);
	sensorSerialDirectBufSnoop(regdata[0], sizeof(regdata));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regdata[0]);
}
#endif
/******TODO****/
