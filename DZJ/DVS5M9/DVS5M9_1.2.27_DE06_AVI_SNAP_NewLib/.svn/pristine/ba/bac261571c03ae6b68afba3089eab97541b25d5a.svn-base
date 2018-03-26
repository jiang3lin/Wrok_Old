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
 *  Author: Allen Liu                                                 *
 *
 *  01.01.01, 20130712 Allen.Liu
 *  Parallel I/F 12-bit at spca6330
 *  FHD(0x30):2288X1286 30fps, Pclk 98MHz
 *  WVGA(0x31):1152X646 60fps, Pclk 98MHz
 *  FHD(0x40):2304X1296 30fps, Pclk 98MHz
 *
 *  01.01.02, 20130827 Allen.Liu
 *  CDSP crop FHD to 2112X1188
 *  Solve pink noise at light area for OTPM V5
 *  FHD(0x30):2112X1188 30fps, Pclk 98MHz
 *  WVGA(0x31):1152X646 60fps, Pclk 98MHz
 *  FHD(0x40):2304X1296 30fps, Pclk 98MHz
 **************************************************************************/
#define MODEL_VERSION "$unp_tg_ar0330cs-01.01.02"
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

#define MASK_EXP_CMD	0
#define BURST_CAP_EN   	0

/*****GPIO Config*****/
#if defined(HW_EVB_SPE7001) || defined(HW_RDK_6330)
#define SENSOR_RESET 	(1<<4)
#else
#define SENSOR_RESET    	(1<<0)
#endif

#define NO              	0          
#define BY_PIXEL   	1        
#define BY_HD         	0        
#define NOUSE         	0
#define DISABLE        	0xFFFF

#define MODE_SYNC_EARLY	(50)

/* For preview size 1152*648, 60fps */
#define PREVIEW_WVGA_HSIZE  (1152)  	/* preview horiz. size*/
#define PREVIEW_WVGA_VSIZE  (648-2) 	/* preview vertical size*/
/* Preview default definition here */
#define PREVIEW_WVGA_EXTEND_LINES	(16)
#define PREVIEW_WVGA_LINETOTAL   		(1104)					/* preview Hd length*/
#define PREVIEW_WVGA_LINEBLANK   		(NOUSE)					/* preview Hd sync length*/
#define PREVIEW_WVGA_FRAMETOTAL  	(648)					/* preview Vd length*/
#define PREVIEW_WVGA_FRAMEBLANK  	(NOUSE)					/* preview Vd sync length*/
#define PREVIEW_WVGA_HOFFSET     		(0x36a)  					/* preview Hd valid pixel data offset position*/
#define PREVIEW_WVGA_VOFFSET     		(PREVIEW_WVGA_EXTEND_LINES+2)   		/* preview Vd valid line data offset position*/
#define PREVIEW_WVGA_HRESHAPE_FALL	(DISABLE)				/* preview Hd reshap fall*/
#define PREVIEW_WVGA_HRESHAPE_RISE   	(DISABLE)				/* preview Hd reshap rise*/
#define PREVIEW_WVGA_VRESHAPE_COUNTER_SEL  NO				/* preview Vd reshap control*/
#define PREVIEW_WVGA_VRESHAPE_FALL   	(DISABLE) 				/* preview Vd reshap fall*/
#define PREVIEW_WVGA_VRESHAPE_RISE   	(DISABLE) 				/* preview Vd reshap rise*/

/* For preview size 2304*1296, 30fps */
#define PREVIEW_FHD_HSIZE  (2304-16)  	/* preview horiz. size*/
#define PREVIEW_FHD_VSIZE  (1296-10)  	/* preview vertical size*/
/* Preview default definition here */
#define PREVIEW_FHD_EXTEND_LINES		(4)
#define PREVIEW_FHD_LINETOTAL   		(2512)				/* preview Hd length*/
#define PREVIEW_FHD_LINEBLANK   		(NOUSE)				/* preview Hd sync length*/
#define PREVIEW_FHD_FRAMETOTAL  		(1296)				/* preview Vd length*/
#define PREVIEW_FHD_FRAMEBLANK  		(NOUSE)				/* preview Vd sync length*/
#define PREVIEW_FHD_HOFFSET     		(0xbc)  				/* preview Hd valid pixel data offset position*/
#define PREVIEW_FHD_VOFFSET     		(PREVIEW_FHD_EXTEND_LINES+2)   	/* preview Vd valid line data offset position*/
#define PREVIEW_FHD_HRESHAPE_FALL	(DISABLE)			/* preview Hd reshap fall*/
#define PREVIEW_FHD_HRESHAPE_RISE    	(DISABLE)			/* preview Hd reshap rise*/
#define PREVIEW_FHD_VRESHAPE_COUNTER_SEL  NO			/* preview Vd reshap control*/
#define PREVIEW_FHD_VRESHAPE_FALL     	(DISABLE) 			/* preview Vd reshap fall*/
#define PREVIEW_FHD_VRESHAPE_RISE     	(DISABLE) 			/* preview Vd reshap rise*/

/* For capture size 2304*1296, 30fps */
#define CAPTURE_HSIZE  (2304)	/* capture horiz. size*/
#define CAPTURE_VSIZE  (1296)	/* capture vertical size*/
/* Capture default definition here */
#define CAPTURE_LINETOTAL			(PREVIEW_FHD_LINETOTAL)		/* capture Hd length*/
#define CAPTURE_LINEBLANK   		(NOUSE)						/* capture Hd sync length*/
#define CAPTURE_FRAMETOTAL  		(CAPTURE_VSIZE)				/* capture Vd length*/
#define CAPTURE_FRAMEBLANK  		(NOUSE)						/* mipi use > vsize is more safe*/
#define CAPTURE_HOFFSET     			(PREVIEW_FHD_HOFFSET)   		/*Capturee Hd valid pixel data offset position*/
#define CAPTURE_VOFFSET    			(PREVIEW_FHD_VOFFSET)     	/* capture Vd valid line data offset position*/
#define CAPTURE_HRESHAPE_FALL        (DISABLE)					/* capture Hd reshap fall*/
#define CAPTURE_HRESHAPE_RISE        (DISABLE)					/* capture Hd reshap rise*/
#define CAPTURE_VRESHAPE_COUNTER_SEL  NO 						/* capture Vd reshap control*/
#define CAPTURE_VRESHAPE_FALL		(DISABLE)					/* capture Vd reshap fall , count by pixel*/
#define CAPTURE_VRESHAPE_RISE		(DISABLE)					/* capture Vd reshap rise , count by pixel*/ 

/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */                                                                                                                                                                                        
#define MCLKMODE		   1	     /* mclk mode,'0'from I/O Pad,'1'from internal*/                                                                                                                                                                                      
#define PCLKMODE		   0	     /* pclk mode,'0'from I/O Pad,'1'from internal*/                                                                                                                                                                                      
#define TGPLLEN   		   1	     /* enable TGPLL,'0'disable,'1'enable*/                                                                                                                                                                                               
#define TGPLLCLKSET		   8	     /* TGPLL clk 0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,                                                                                                                                                                           
						                          5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/                                                                                                                                                                
#define MCLKDIV			   4	     /* MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/                                                                                                                                            
#define PCLKDIV			   2	     /* pixel clock = (master clock) / pclkdiv*/                                                                                                                                                                                          
#define MASTERPHASE	   0       /* master clk phase*/
#define PIXELPHASE		   0	     /* pixel clk phase*/
#define ADCKPHASE		   0	     /* adck clk phase*/                                                                                                                                                                                                                  

#define RGBADJ		          0	      /*RGB input data sample phase adj "1"negedge "0"posedge*/

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		TYPE_CMOS	    /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      					TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define TGTYPE				TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						     							 TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/

#define PREVIEW_FHD_CFAPATTERN		BAYER_BGGR	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_WVGA_CFAPATTERN		BAYER_BGGR	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define SNAP_CFAPATTERN				BAYER_BGGR /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define SNAP_DUMMYVD_NUM	     			0             	/*snap dummy Vd need numbers*/
#define SNAP_READOUTVD_NUM			1             	/*snap readout Vd need numbers*/             
#define SNAP_EARLY_NUM		          		0	            	/*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define SNAP_FRAME_EARLY_NUM		  	2            	 	/*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define FLDSEL_START_NUM	          		0		       /*It arrange the start field data to the start position of the sdram,0,1,2 */             
#define FLDSEL_END_NUM			 	  	0             	/*It arrange the end field data to the end position of the sdram,0,1,2 */                 
#define SKIP_SNAP_READOUTVD_EN        	0            		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define VSHUTTER_RESEND 	          		0		       /*sony FALSE;panasonic/ITG TRUE*/
#define EXP_FRAME_NUM       	      			1	            	/*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/
#define SKIP_SNAP_FRAME_READOUTVD_EN  2	            	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/

#define PREVIEW_HDNUM_FRAME_START         	(110)                                                
#define PREVIEW_WVGA_HDNUM_FRAME_END	(2)             
#define PREVIEW_WVGA_HDNUM_CMDPORT		(300)                           
#define PREVIEW_FHD_HDNUM_FRAME_END 	(2)                                             
#define PREVIEW_FHD_HDNUM_CMDPORT		(600)                                               
#define CAPTURE_HDNUM_FRAME_START        	(110)                                          
#define CAPTURE_HDNUM_FRAME_END	     		(2)                                       
#define CAPTURE_HDNUM_CMDPORT	             	(600) 
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
#define	TG_SPI_I2C_MODE  	0x20     	           /*I2C slave addr for write*/
#define	TG_FORMAT2  	 	FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	    	0		           /*don't change*/
#define	TG_SPEED    	 FRONT_SSC_CLK_192K    /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						                             	FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#define	TG_NBIT_REGDATA    16		           /*ssc/spi register data numbers */           
#define	TG_NBIT_REGADDR    16		           /*ssc/spi register address numbers */  

/**************global variable *******************/
static UINT32 cmdportid_tg;
/*static UINT32 g_IsSnap2Pv=0;*/

/**************prototypes of sensor model methods *******************/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewFHDBegin();
static void sensorDoFuncPreviewWVGABegin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
static void sensorDoFuncSnapNext(UINT32 next_mode);

/******************** AR0330 uCode ****************************/
static const UINT8 cmd_SWReset[][4]={
{0x30,0x1A,0x00,0xD9}  /*sw reset*/
};

static const UINT8 cmd_init_code[][4]={
{0x31,0xAE,0x03,0x01},  
{0x30,0x1A,0x10,0xD8}, /*stream off*/ 
{0x30,0x64,0x18,0x02}, /*Disable Embedded Data */  
{0x30,0x78,0x00,0x01}, /*Marker to say that 'Defaults' have been run */
{0x31,0xE0,0x07,0x03},
{0x30,0x6E,0xFC,0x10}
};

static const UINT8 cmd_OTPM_V1[][4]={
{0x30,0xBA,0x00,0x2C},
{0x30,0xFE,0x00,0x80},
{0x31,0xE0,0x00,0x00},
{0x3E,0xCE,0x00,0xFF},
{0x3E,0xD0,0xE4,0xF6},
{0x3E,0xD2,0x01,0x46},
{0x3E,0xD4,0x8F,0x6C},
{0x3E,0xD6,0x66,0xCC},
{0x3E,0xD8,0x8C,0x42},
{0x3E,0xDA,0x88,0x22},
{0x3E,0xDC,0x22,0x22},
{0x3E,0xDE,0x22,0xC0},
{0x3E,0xE0,0x15,0x00},
{0x3E,0xE6,0x00,0x80},
{0x3E,0xE8,0x20,0x27},
{0x3E,0xEA,0x00,0x1D},
{0x3F,0x06,0x04,0x6A}
};

static const UINT8 cmd_OTPM_V2[][4]={
{0x30,0xBA,0x00,0x2C},
{0x30,0xFE,0x00,0x80},
{0x31,0xE0,0x00,0x00},
{0x3E,0xCE,0x00,0xFF},
{0x3E,0xD0,0xE4,0xF6},
{0x3E,0xD2,0x01,0x46},
{0x3E,0xD4,0x8F,0x6C},
{0x3E,0xD6,0x66,0xCC},
{0x3E,0xD8,0x8C,0x42},
{0x3E,0xDA,0x88,0x9B},
{0x3E,0xDC,0x88,0x63},
{0x3E,0xDE,0xAA,0x04},
{0x3E,0xE0,0x15,0xF0},
{0x3E,0xE6,0x00,0x8C},
{0x3E,0xE8,0x20,0x24},
{0x3E,0xEA,0xFF,0x1F},
{0x3F,0x06,0x04,0x6A}
};

static const UINT8 cmd_OTPM_V3[][4]={
{0x3E,0xD2,0x01,0x46},
{0x3E,0xD4,0x8F,0x6C},
{0x3E,0xD6,0x66,0xCC},
{0x3E,0xD8,0x8C,0x42}
};

static const UINT8 cmd_OTPM_V4[][4]={
{0x3E,0xD2,0x01,0x46},
{0x3E,0xD6,0x66,0xCC},
{0x3E,0xD8,0x8C,0x42}
};

static const UINT8 cmd_OTPM_V5[][4]={
{0x3E,0xD2,0x01,0x46}
};

static const UINT8 cmd_sequencer_A[][4]={
{0x30,0x88,0x80,0x00},
{0x30,0x86,0x45,0x40},
{0x30,0x86,0x61,0x34},
{0x30,0x86,0x4A,0x31},
{0x30,0x86,0x43,0x42},
{0x30,0x86,0x45,0x60},
{0x30,0x86,0x27,0x14},
{0x30,0x86,0x3D,0xFF},
{0x30,0x86,0x3D,0xFF},
{0x30,0x86,0x3D,0xEA},
{0x30,0x86,0x27,0x04},
{0x30,0x86,0x3D,0x10},
{0x30,0x86,0x27,0x05},
{0x30,0x86,0x3D,0x10},
{0x30,0x86,0x27,0x15},
{0x30,0x86,0x35,0x27},
{0x30,0x86,0x05,0x3D},
{0x30,0x86,0x10,0x45},
{0x30,0x86,0x40,0x27},
{0x30,0x86,0x04,0x27},
{0x30,0x86,0x14,0x3D},
{0x30,0x86,0xFF,0x3D},
{0x30,0x86,0xFF,0x3D},
{0x30,0x86,0xEA,0x62},
{0x30,0x86,0x27,0x28},
{0x30,0x86,0x36,0x27},
{0x30,0x86,0x08,0x3D},
{0x30,0x86,0x64,0x44},
{0x30,0x86,0x2C,0x2C},
{0x30,0x86,0x2C,0x2C},
{0x30,0x86,0x4B,0x01},
{0x30,0x86,0x43,0x2D},
{0x30,0x86,0x46,0x43},
{0x30,0x86,0x16,0x47},
{0x30,0x86,0x43,0x5F},
{0x30,0x86,0x4F,0x50},
{0x30,0x86,0x26,0x04},
{0x30,0x86,0x26,0x84},
{0x30,0x86,0x20,0x27},
{0x30,0x86,0xFC,0x53},
{0x30,0x86,0x0D,0x5C},
{0x30,0x86,0x0D,0x60},
{0x30,0x86,0x57,0x54},
{0x30,0x86,0x17,0x09},
{0x30,0x86,0x55,0x56},
{0x30,0x86,0x49,0x17},
{0x30,0x86,0x14,0x5C},
{0x30,0x86,0x09,0x45},
{0x30,0x86,0x00,0x45},
{0x30,0x86,0x80,0x26},
{0x30,0x86,0xA6,0x27},
{0x30,0x86,0xF8,0x17},
{0x30,0x86,0x02,0x27},
{0x30,0x86,0xFA,0x5C},
{0x30,0x86,0x0B,0x5F},
{0x30,0x86,0x53,0x07},
{0x30,0x86,0x53,0x02},
{0x30,0x86,0x4D,0x28},
{0x30,0x86,0x6C,0x4C},
{0x30,0x86,0x09,0x28},
{0x30,0x86,0x2C,0x28},
{0x30,0x86,0x29,0x4E},
{0x30,0x86,0x17,0x18},
{0x30,0x86,0x26,0xA2},
{0x30,0x86,0x5C,0x03},
{0x30,0x86,0x17,0x44},
{0x30,0x86,0x28,0x09},
{0x30,0x86,0x27,0xF2},
{0x30,0x86,0x17,0x14},
{0x30,0x86,0x28,0x08},
{0x30,0x86,0x16,0x4D},
{0x30,0x86,0x1A,0x26},
{0x30,0x86,0x83,0x17},
{0x30,0x86,0x01,0x45},
{0x30,0x86,0xA0,0x17},
{0x30,0x86,0x07,0x27},
{0x30,0x86,0xF3,0x17},
{0x30,0x86,0x29,0x45},
{0x30,0x86,0x80,0x17},
{0x30,0x86,0x08,0x27},
{0x30,0x86,0xF2,0x17},
{0x30,0x86,0x28,0x5D},
{0x30,0x86,0x27,0xFA},
{0x30,0x86,0x17,0x0E},
{0x30,0x86,0x26,0x81},
{0x30,0x86,0x53,0x00},
{0x30,0x86,0x17,0xE6},
{0x30,0x86,0x53,0x02},
{0x30,0x86,0x17,0x10},
{0x30,0x86,0x26,0x83},
{0x30,0x86,0x26,0x82},
{0x30,0x86,0x48,0x27},
{0x30,0x86,0xF2,0x4D},
{0x30,0x86,0x4E,0x28},
{0x30,0x86,0x09,0x4C},
{0x30,0x86,0x0B,0x17},
{0x30,0x86,0x6D,0x28},
{0x30,0x86,0x08,0x17},
{0x30,0x86,0x01,0x4D},
{0x30,0x86,0x1A,0x17},
{0x30,0x86,0x01,0x26},
{0x30,0x86,0x03,0x5C},
{0x30,0x86,0x00,0x45},
{0x30,0x86,0x40,0x27},
{0x30,0x86,0x90,0x17},
{0x30,0x86,0x2A,0x4A},
{0x30,0x86,0x0A,0x43},
{0x30,0x86,0x16,0x0B},
{0x30,0x86,0x43,0x27},
{0x30,0x86,0x94,0x45},
{0x30,0x86,0x60,0x17},
{0x30,0x86,0x07,0x27},
{0x30,0x86,0x95,0x17},
{0x30,0x86,0x25,0x45},
{0x30,0x86,0x40,0x17},
{0x30,0x86,0x08,0x27},
{0x30,0x86,0x90,0x5D},
{0x30,0x86,0x28,0x08},
{0x30,0x86,0x53,0x0D},
{0x30,0x86,0x26,0x45},
{0x30,0x86,0x5C,0x01},
{0x30,0x86,0x27,0x98},
{0x30,0x86,0x4B,0x12},
{0x30,0x86,0x44,0x52},
{0x30,0x86,0x51,0x17},
{0x30,0x86,0x02,0x60},
{0x30,0x86,0x18,0x4A},
{0x30,0x86,0x03,0x43},
{0x30,0x86,0x16,0x04},
{0x30,0x86,0x43,0x16},
{0x30,0x86,0x58,0x43},
{0x30,0x86,0x16,0x59},
{0x30,0x86,0x43,0x16},
{0x30,0x86,0x5A,0x43},
{0x30,0x86,0x16,0x5B},
{0x30,0x86,0x43,0x27},
{0x30,0x86,0x9C,0x45},
{0x30,0x86,0x60,0x17},
{0x30,0x86,0x07,0x27},
{0x30,0x86,0x9D,0x17},
{0x30,0x86,0x25,0x45},
{0x30,0x86,0x40,0x17},
{0x30,0x86,0x10,0x27},
{0x30,0x86,0x98,0x17},
{0x30,0x86,0x20,0x22},
{0x30,0x86,0x4B,0x12},
{0x30,0x86,0x44,0x2C},
{0x30,0x86,0x2C,0x2C},
{0x30,0x86,0x2C,0x00}
};

static const UINT8 cmd_sequencer_B[][4]={
{0x30,0x88,0x80,0x00},
{0x30,0x86,0x4A,0x03},
{0x30,0x86,0x43,0x16},
{0x30,0x86,0x04,0x43},
{0x30,0x86,0x16,0x45},
{0x30,0x86,0x40,0x45},
{0x30,0x86,0x60,0x17},
{0x30,0x86,0x20,0x45},
{0x30,0x86,0x40,0x4B},
{0x30,0x86,0x12,0x44},
{0x30,0x86,0x61,0x34},
{0x30,0x86,0x4A,0x31},
{0x30,0x86,0x43,0x42},
{0x30,0x86,0x45,0x60},
{0x30,0x86,0x27,0x14},
{0x30,0x86,0x3D,0xFF},
{0x30,0x86,0x3D,0xFF},
{0x30,0x86,0x3D,0xEA},
{0x30,0x86,0x27,0x04},
{0x30,0x86,0x3D,0x10},
{0x30,0x86,0x27,0x05},
{0x30,0x86,0x3D,0x10},
{0x30,0x86,0x27,0x15},
{0x30,0x86,0x35,0x27},
{0x30,0x86,0x05,0x3D},
{0x30,0x86,0x10,0x45},
{0x30,0x86,0x40,0x27},
{0x30,0x86,0x04,0x27},
{0x30,0x86,0x14,0x3D},
{0x30,0x86,0xFF,0x3D},
{0x30,0x86,0xFF,0x3D},
{0x30,0x86,0xEA,0x62},
{0x30,0x86,0x27,0x28},
{0x30,0x86,0x36,0x27},
{0x30,0x86,0x08,0x3D},
{0x30,0x86,0x64,0x44},
{0x30,0x86,0x2C,0x2C},
{0x30,0x86,0x2C,0x2C},
{0x30,0x86,0x4B,0x01},
{0x30,0x86,0x43,0x2D},
{0x30,0x86,0x46,0x43},
{0x30,0x86,0x16,0x47},
{0x30,0x86,0x43,0x5F},
{0x30,0x86,0x4F,0x50},
{0x30,0x86,0x26,0x04},
{0x30,0x86,0x26,0x84},
{0x30,0x86,0x20,0x27},
{0x30,0x86,0xFC,0x53},
{0x30,0x86,0x0D,0x5C},
{0x30,0x86,0x0D,0x57},
{0x30,0x86,0x54,0x17},
{0x30,0x86,0x09,0x55},
{0x30,0x86,0x56,0x49},
{0x30,0x86,0x53,0x07},
{0x30,0x86,0x53,0x02},
{0x30,0x86,0x4D,0x28},
{0x30,0x86,0x6C,0x4C},
{0x30,0x86,0x09,0x28},
{0x30,0x86,0x2C,0x28},
{0x30,0x86,0x29,0x4E},
{0x30,0x86,0x5C,0x09},
{0x30,0x86,0x60,0x45},
{0x30,0x86,0x00,0x45},
{0x30,0x86,0x80,0x26},
{0x30,0x86,0xA6,0x27},
{0x30,0x86,0xF8,0x17},
{0x30,0x86,0x02,0x27},
{0x30,0x86,0xFA,0x5C},
{0x30,0x86,0x0B,0x17},
{0x30,0x86,0x18,0x26},
{0x30,0x86,0xA2,0x5C},
{0x30,0x86,0x03,0x17},
{0x30,0x86,0x44,0x27},
{0x30,0x86,0xF2,0x5F},
{0x30,0x86,0x28,0x09},
{0x30,0x86,0x17,0x14},
{0x30,0x86,0x28,0x08},
{0x30,0x86,0x17,0x01},
{0x30,0x86,0x4D,0x1A},
{0x30,0x86,0x26,0x83},
{0x30,0x86,0x17,0x01},
{0x30,0x86,0x27,0xFA},
{0x30,0x86,0x45,0xA0},
{0x30,0x86,0x17,0x07},
{0x30,0x86,0x27,0xFB},
{0x30,0x86,0x17,0x29},
{0x30,0x86,0x45,0x80},
{0x30,0x86,0x17,0x08},
{0x30,0x86,0x27,0xFA},
{0x30,0x86,0x17,0x28},
{0x30,0x86,0x5D,0x17},
{0x30,0x86,0x0E,0x26},
{0x30,0x86,0x81,0x53},
{0x30,0x86,0x01,0x17},
{0x30,0x86,0xE6,0x53},
{0x30,0x86,0x02,0x17},
{0x30,0x86,0x10,0x26},
{0x30,0x86,0x83,0x26},
{0x30,0x86,0x82,0x48},
{0x30,0x86,0x4D,0x4E},
{0x30,0x86,0x28,0x09},
{0x30,0x86,0x4C,0x0B},
{0x30,0x86,0x60,0x17},
{0x30,0x86,0x20,0x27},
{0x30,0x86,0xF2,0x17},
{0x30,0x86,0x53,0x5F},
{0x30,0x86,0x28,0x08},
{0x30,0x86,0x16,0x4D},
{0x30,0x86,0x1A,0x17},
{0x30,0x86,0x01,0x27},
{0x30,0x86,0xFA,0x26},
{0x30,0x86,0x03,0x5C},
{0x30,0x86,0x01,0x45},
{0x30,0x86,0x40,0x27},
{0x30,0x86,0x98,0x17},
{0x30,0x86,0x2A,0x4A},
{0x30,0x86,0x0A,0x43},
{0x30,0x86,0x16,0x0B},
{0x30,0x86,0x43,0x27},
{0x30,0x86,0x9C,0x45},
{0x30,0x86,0x60,0x17},
{0x30,0x86,0x07,0x27},
{0x30,0x86,0x9D,0x17},
{0x30,0x86,0x25,0x45},
{0x30,0x86,0x40,0x17},
{0x30,0x86,0x08,0x27},
{0x30,0x86,0x98,0x5D},
{0x30,0x86,0x26,0x45},
{0x30,0x86,0x4B,0x17},
{0x30,0x86,0x0A,0x28},
{0x30,0x86,0x08,0x53},
{0x30,0x86,0x0D,0x52},
{0x30,0x86,0x51,0x12},
{0x30,0x86,0x44,0x60},
{0x30,0x86,0x18,0x4A},
{0x30,0x86,0x03,0x43},
{0x30,0x86,0x16,0x04},
{0x30,0x86,0x43,0x16},
{0x30,0x86,0x58,0x43},
{0x30,0x86,0x16,0x59},
{0x30,0x86,0x43,0x16},
{0x30,0x86,0x5A,0x43},
{0x30,0x86,0x16,0x5B},
{0x30,0x86,0x43,0x27},
{0x30,0x86,0x9C,0x45},
{0x30,0x86,0x60,0x17},
{0x30,0x86,0x07,0x27},
{0x30,0x86,0x9D,0x17},
{0x30,0x86,0x25,0x45},
{0x30,0x86,0x40,0x17},
{0x30,0x86,0x10,0x27},
{0x30,0x86,0x98,0x17},
{0x30,0x86,0x20,0x22},
{0x30,0x86,0x4B,0x12},
{0x30,0x86,0x44,0x2C},
{0x30,0x86,0x2C,0x2C},
{0x30,0x86,0x2C,0x00},
{0x30,0x86,0x00,0x00}
};

static const UINT8 cmd_sequencer_patch1[][4]={
{0x30,0x88,0x80,0x0C},
{0x30,0x86,0x20,0x45}
};

static const UINT8 cmd_fhd_code[][4]={
{0x30,0x1A,0x10,0xD8}, /*stream off*/
{0x30,0x2A,0x00,0x06}, /*vt_pix_clk_div*/
{0x30,0x2C,0x00,0x01}, /*vt_sys_clk_div*/
{0x30,0x2E,0x00,0x04}, /*pre_pll_clk_div*/
{0x30,0x30,0x00,0x62}, /*pll_multiplier*/
{0x30,0x36,0x00,0x06}, /*op_pix_clk_div*/
{0x30,0x38,0x00,0x01}, /*op_sys_clk_div*/
{0x31,0xAC,0x0C,0x0C}, /*data_format*/
{0x30,0x04,0x00,0x06}, /*X_ADDR_START*/
{0x30,0x08,0x09,0x05}, /*X_ADDR_END 2309*/  
{0x30,0x02,0x00,0x78}, /*Y_ADDR_START 120*/  
{0x30,0x06,0x05,0x87}, /*Y_ADDR_END 1415*/  
{0x30,0xA2,0x00,0x01}, /*X_ODD_INCREMENT*/
{0x30,0xA6,0x00,0x01}, /*Y_ODD_INCREMENT*/
{0x30,0x40,0x00,0x00}, /*Row Bin*/
{0x30,0x0C,0x04,0xE0}, /*LINE_LENGTH_PCK 1248*/ 
{0x30,0x0A,0x05,0x1E}, /*FRAME_LENGTH_LINES 1308+2*/ 
{0x30,0x14,0x00,0x00}, /*FINE_INTEGRATION_TIME(fixed value)*/
/*{0x30,0x12,0x05,0x1B},*/ /*Coarse_Integration_Time 1307*/  
{0x30,0x42,0x00,0x00}, /*EXTRA_DELAY*/
{0x30,0xBA,0x00,0x2C}, /*Digital_Ctrl_Adc_High_Speed*/
{0x30,0x88,0x80,0xBA},
{0x30,0x86,0xE6,0x53},
{0x30,0x1A,0x10,0xDC} /*stream on*/
};

static const UINT8 cmd_wvga_code[][4]={
{0x30,0x1A,0x10,0xD8}, /*stream off*/
#if 0 /*fpclk=96MHz*/
{0x30,0x2A,0x00,0x08}, /*vt_pix_clk_div*/
{0x30,0x2C,0x00,0x01}, /*vt_sys_clk_div*/
{0x30,0x2E,0x00,0x01}, /*pre_pll_clk_div*/
{0x30,0x30,0x00,0x20}, /*pll_multiplier*/
{0x30,0x36,0x00,0x08}, /*op_pix_clk_div*/
{0x30,0x38,0x00,0x01}, /*op_sys_clk_div*/
{0x31,0xAC,0x0C,0x0C}, /*data_format*/
#else /*fpclk=98MHz*/
{0x30,0x2A,0x00,0x06}, /*vt_pix_clk_div*/
{0x30,0x2C,0x00,0x01}, /*vt_sys_clk_div*/
{0x30,0x2E,0x00,0x04}, /*pre_pll_clk_div*/
{0x30,0x30,0x00,0x62}, /*pll_multiplier*/
{0x30,0x36,0x00,0x06}, /*op_pix_clk_div*/
{0x30,0x38,0x00,0x01}, /*op_sys_clk_div*/
{0x31,0xAC,0x0C,0x0C}, /*data_format*/
#endif
{0x30,0x04,0x00,0x06}, /*X_ADDR_START*/
{0x30,0x08,0x09,0x05}, /*X_ADDR_END 2309*/  
{0x30,0x02,0x00,0x78}, /*Y_ADDR_START 120*/  
{0x30,0x06,0x05,0x87}, /*Y_ADDR_END 1415*/  
{0x30,0xA2,0x00,0x03}, /*X_ODD_INCREMENT*/
{0x30,0xA6,0x00,0x03}, /*Y_ODD_INCREMENT*/
{0x30,0x40,0x12,0x00}, /*Row Bin*/
{0x30,0x0C,0x03,0xF6}, /*LINE_LENGTH_PCK 1014*/ 
{0x30,0x0A,0x03,0x26}, /*FRAME_LENGTH_LINES 805+1*/ 
{0x30,0x14,0x00,0x00}, /*FINE_INTEGRATION_TIME(fixed value)*/
/*{0x30,0x12,0x03,0x24},*//*Coarse_Integration_Time 804*/  
{0x30,0x42,0x01,0x8D}, /*EXTRA_DELAY 397*/
{0x30,0xBA,0x00,0x2C}, /*Digital_Ctrl_Adc_High_Speed*/
{0x30,0x88,0x80,0xBA},
{0x30,0x86,0x02,0x53}, 
{0x30,0x1A,0x10,0xDC} /*stream on*/
};

static const UINT8 cmd_still_code[][4]={
#if 0 /*2048*1536, 25fps*/
{0x30,0x1A,0x10,0xD8}, /*stream off*/
{0x30,0x2A,0x00,0x06}, /*vt_pix_clk_div*/
{0x30,0x2C,0x00,0x01}, /*vt_sys_clk_div*/
{0x30,0x2E,0x00,0x04}, /*pre_pll_clk_div*/
{0x30,0x30,0x00,0x62}, /*pll_multiplier*/
{0x30,0x36,0x00,0x06}, /*op_pix_clk_div*/
{0x30,0x38,0x00,0x01}, /*op_sys_clk_div*/
{0x31,0xAC,0x0C,0x0C}, /*data_format*/
{0x30,0x04,0x00,0x80}, /*X_ADDR_START 128*/
{0x30,0x08,0x08,0x7F}, /*X_ADDR_END 2175*/  
{0x30,0x02,0x00,0x06}, /*Y_ADDR_START 6*/  
{0x30,0x06,0x06,0x05}, /*Y_ADDR_END 1541*/  
{0x30,0xA2,0x00,0x01}, /*X_ODD_INCREMENT*/
{0x30,0xA6,0x00,0x01}, /*Y_ODD_INCREMENT*/
{0x30,0x40,0x00,0x00}, /*Row Bin*/
{0x30,0x0C,0x04,0xDA}, /*LINE_LENGTH_PCK 1242*/ 
{0x30,0x0A,0x06,0x0C}, /*FRAME_LENGTH_LINES 1548*/ 
{0x30,0x14,0x00,0x00}, /*FINE_INTEGRATION_TIME(fixed value)*/
{0x30,0x42,0x00,0x00}, /*EXTRA_DELAY*/
{0x30,0xBA,0x00,0x2C}, /*Digital_Ctrl_Adc_High_Speed*/ 
{0x30,0x88,0x80,0xBA},
{0x30,0x86,0xE6,0x53},
{0x30,0x1A,0x10,0xDC} /*stream on*/
#else /*2304*1296, 30fps*/
{0x30,0x1A,0x10,0xD8}, /*stream off*/
{0x30,0x2A,0x00,0x06}, /*vt_pix_clk_div*/
{0x30,0x2C,0x00,0x01}, /*vt_sys_clk_div*/
{0x30,0x2E,0x00,0x04}, /*pre_pll_clk_div*/
{0x30,0x30,0x00,0x62}, /*pll_multiplier*/
{0x30,0x36,0x00,0x06}, /*op_pix_clk_div*/
{0x30,0x38,0x00,0x01}, /*op_sys_clk_div*/
{0x31,0xAC,0x0C,0x0C}, /*data_format*/
{0x30,0x04,0x00,0x06}, /*X_ADDR_START*/
{0x30,0x08,0x09,0x05}, /*X_ADDR_END 2309*/  
{0x30,0x02,0x00,0x78}, /*Y_ADDR_START 120*/  
{0x30,0x06,0x05,0x87}, /*Y_ADDR_END 1415*/  
{0x30,0xA2,0x00,0x01}, /*X_ODD_INCREMENT*/
{0x30,0xA6,0x00,0x01}, /*Y_ODD_INCREMENT*/
{0x30,0x40,0x00,0x00}, /*Row Bin*/
{0x30,0x0C,0x04,0xE0}, /*LINE_LENGTH_PCK 1248*/ 
{0x30,0x0A,0x05,0x1E}, /*FRAME_LENGTH_LINES 1308+2*/ 
{0x30,0x14,0x00,0x00}, /*FINE_INTEGRATION_TIME(fixed value)*/
/*{0x30,0x12,0x05,0x1B}, *//*Coarse_Integration_Time 1307*/  
{0x30,0x42,0x00,0x00}, /*EXTRA_DELAY*/
{0x30,0xBA,0x00,0x2C}, /*Digital_Ctrl_Adc_High_Speed*/
{0x30,0x88,0x80,0xBA},
{0x30,0x86,0xE6,0x53},
{0x30,0x1A,0x10,0xDC} /*stream on*/
#endif
};

/***************************sensor tg cfg******************************/
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
	        .xOffset = (PREVIEW_FHD_HSIZE-2112)/2,
	        .yOffset = (PREVIEW_FHD_VSIZE-1188)/2,
	        .xSize = 2112,
	        .ySize = 1188,	
	},
	.hsize = PREVIEW_FHD_HSIZE,
	.vsize = PREVIEW_FHD_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_FHD_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 29400000,
	.sensor_pclk = 98000000,
	.sensor_htotal = 1248*2,
	.pix_100khz =1480, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewWVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewWVGABegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_WVGA_HSIZE,
		.ySize = PREVIEW_WVGA_VSIZE,
	},
	.hsize = PREVIEW_WVGA_HSIZE,
	.vsize = PREVIEW_WVGA_VSIZE,
	.sensor_model = SENSORMODEL,
    	.cfa_pattern = PREVIEW_WVGA_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 59980000,
	.sensor_pclk = 98000000,
	.sensor_htotal = 1014*2,
	.pix_100khz =1480, 
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	&sensorTgPreviewFHDCfg,
    	&sensorTgPreviewWVGACfg,
};

static sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = NULL,
		.snapExpBegin = sensorDoFuncSnapExpBegin,
		.snapBshutterBegin = NULL,
		.snapBshutterFollow = NULL,
		.snapBshutterEnd = NULL,
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
	.n_snap_early = SNAP_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
	.pix_100khz =1480,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg,
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
	.hsize           = PREVIEW_FHD_HSIZE,
	.vsize           = PREVIEW_FHD_VSIZE,
	.lastlinepix     = PREVIEW_FHD_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0, /*register 0x9082: parallel->0, mipi/lvds->3*/
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_FHD_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_FHD_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_FHD_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewWVGACfg = {
	.linetotal       = PREVIEW_WVGA_LINETOTAL,
	.lineblank       = PREVIEW_WVGA_LINEBLANK,
	.frametotal      = PREVIEW_WVGA_FRAMETOTAL,
	.frameblank      = PREVIEW_WVGA_FRAMEBLANK,
	.hreshape_fall   = PREVIEW_WVGA_HRESHAPE_FALL,
	.hreshape_rise   = PREVIEW_WVGA_HRESHAPE_RISE,
	.vreshape_fall   = PREVIEW_WVGA_VRESHAPE_FALL,
	.vreshape_rise   = PREVIEW_WVGA_VRESHAPE_RISE,
	.hoffset         = PREVIEW_WVGA_HOFFSET,
	.voffset         = PREVIEW_WVGA_VOFFSET,
	.hsize           = PREVIEW_WVGA_HSIZE,
	.vsize           = PREVIEW_WVGA_VSIZE,
	.lastlinepix     = PREVIEW_WVGA_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_WVGA_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = PREVIEW_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_WVGA_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_WVGA_HDNUM_CMDPORT,
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
	.defSyncSigCfg  = &sensorSyncSigPreviewFHDCfg,
	.verStr = MODEL_VERSION,
};

void sensorTg_ar0330cs_plcc_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

static void sensorReportsize()
{
	profLogPrintf(3,"mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	profLogPrintf(3,"front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	profLogPrintf(3,"imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	profLogPrintf(3,"rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	WRITE16(0xb0009064,1);
	profLogPrintf(3,"line count            = (%d)",READ16(0xb0009064));
}

static UINT16 sensorRead(UINT16 addr)
{
	UINT8 regread[4];
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	sensorSerialBlockRead(cmdportid_tg,1,regread,0);
	return (regread[3]|(regread[2]<<8));
}

static void sensorInit(void)
{
	UINT16 OTPM1,OTPM2,OTPM3;

	sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_SWReset)/sizeof(cmd_SWReset[0]), cmd_SWReset[0],0); /*sw reset*/
	tmrUsWait(34000); /*34ms*/
	sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_init_code)/sizeof(cmd_init_code[0]), cmd_init_code[0],0);

	OTPM1=sensorRead(0x300E);
	OTPM2=sensorRead(0x30F0);
	OTPM3=sensorRead(0x3072);
	profLogPrintf(3,"OTPM 0x300E=0x%x 0x30F0=0x%x 0x3072=0x%x",OTPM1,OTPM2,OTPM3);

	if(OTPM2 == 0x1200){
	       profLogAdd(3,"OTPM revsion-V1");
	       sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_OTPM_V1)/sizeof(cmd_OTPM_V1[0]), cmd_OTPM_V1[0],0);
	       sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_sequencer_A)/sizeof(cmd_sequencer_A[0]), cmd_sequencer_A[0],0);
	}else if(OTPM3 == 0x0000){
	        profLogAdd(3,"OTPM revsion-V2");
	       sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_OTPM_V2)/sizeof(cmd_OTPM_V2[0]), cmd_OTPM_V2[0],0);
	       sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_sequencer_B)/sizeof(cmd_sequencer_B[0]), cmd_sequencer_B[0],0);
	}else if(OTPM3 == 0x0006){
	        profLogAdd(3,"OTPM revsion-V3");
	       sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_OTPM_V3)/sizeof(cmd_OTPM_V3[0]), cmd_OTPM_V3[0],0);
	       sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_sequencer_patch1)/sizeof(cmd_sequencer_patch1[0]), cmd_sequencer_patch1[0],0);
	}else if(OTPM3 == 0x0007){
	        profLogAdd(3,"OTPM revsion-V4");
	        sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_OTPM_V4)/sizeof(cmd_OTPM_V4[0]), cmd_OTPM_V4[0],0);
	}else if(OTPM3 == 0x0008){
	        profLogAdd(3,"OTPM revsion-V5");
	        sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_OTPM_V5)/sizeof(cmd_OTPM_V5[0]), cmd_OTPM_V5[0],0);
	}else{
	        profLogAdd(3,"OTPM revsion-V???");
	}

	sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_fhd_code)/sizeof(cmd_fhd_code[0]), cmd_fhd_code[0],0); /*set pv size and stream on*/
#if PREVIEW_FHD_EXTEND_LINES
	WRITE8(0xb0009084, 1);
	WRITE8(0xb0009085, PREVIEW_FHD_EXTEND_LINES-1);
#endif
}

static void sensorDoFuncInit()
{
	printf("________Sensor Driver MODEL_VERSION <<%s>>", sensorTgDev.verStr);
	printf("________Sensor Driver Build Time    <<%s>>", BUILD_TIME);
    	profLogAdd(3, "sensorDoFuncInit()___enter");

	/*Internal pixel clock*/
	halFrontClkCfg(MCLKMODE,1);

    	/*TG PLL=MCLK out=24MHz*/
	sensorTgPllClkExtSet(24000000);

	/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
	
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

	tmrUsWait(10000); /* at least 150000xMCLK = 6.25ms on AR0330CS-DS p.12 */

	/*I2C Init code Here */
	sensorInit();

#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif

	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	/*External pixel clock*/
	halFrontClkCfg(MCLKMODE, PCLKMODE);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	profLogAdd(3, "sensorDoFuncInit()___done");
}

static void sensorDoFuncStop()
{
}

static void sensorDoFuncPreviewWVGABegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	sensorEventVvalidWait(1);
	sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_wvga_code)/sizeof(cmd_wvga_code[0]), cmd_wvga_code[0],0);
#if PREVIEW_WVGA_EXTEND_LINES
	WRITE8(0xb0009084, 1);
	WRITE8(0xb0009085, PREVIEW_WVGA_EXTEND_LINES-1);
#endif
	sensorSyncSigInit(&sensorSyncSigPreviewWVGACfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	profLogAdd(3, "Parallel WVGA cmd done");
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
	sensorEventVvalidWait(1);
	sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_fhd_code)/sizeof(cmd_fhd_code[0]), cmd_fhd_code[0],0);
#if PREVIEW_FHD_EXTEND_LINES
	WRITE8(0xb0009084, 1);
	WRITE8(0xb0009085, PREVIEW_FHD_EXTEND_LINES-1);
#endif
	sensorSyncSigInit(&sensorSyncSigPreviewFHDCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	profLogAdd(3, "Parallel FHD cmd done");
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
	sensorSerialBulkWrite(cmdportid_tg, sizeof(cmd_still_code)/sizeof(cmd_still_code[0]), cmd_still_code[0],0);
#if PREVIEW_FHD_EXTEND_LINES
	WRITE8(0xb0009084, 1);
	WRITE8(0xb0009085, PREVIEW_FHD_EXTEND_LINES-1);
#endif
	sensorSyncSigInit(&sensorSyncSigSnapCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	profLogAdd(3,"sensorDoFuncSnapExpBegin");
}

static void sensorDoFuncSnapShortExp()
{
	/*front_snap_short_exp_frame_set()*//*slow shutter, 1 longer frame*/
	/*front_snap_short_exp_tg_set*/
	/*do 1 longer frame exp, historical term "bshutter", if needed*/
}

static void sensorDoFuncSnapExpEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapExpEnd*/
	/*throw user msg??*/
	/*flash rearsync trig*/
	/*m-sh close trig*/
}

static void sensorDoFuncSnapReadoutField0()
{
	/*front_snap_readout_field0_frame_set*/
	/*front_snap_readout_field0_tg_set*/
	/*also integrating fldNum in sensorDoFuncSnapExpBegin(), capCtr???*/
	profLogAdd(3,"sensorDoFuncSnapReadoutField0");
}

static void sensorDoFuncSnapFieldEnd()
{
	/*insert custom service hdnum roll tbl as specific time slot*/
	/*using hdnum2*/
	/*sensorCustomCbSnapEnd()*/
	/*throw user msg??*/
	profLogAdd(3,"sensorDoFuncSnapFieldEnd");
}

static void sensorDoFuncSnapReadoutEnd()
{
	/*panasonic TG TRG reset*/
	/*direct m-sh open*/
	/*direct vsubctl off*/
	/*immediate inform readout all end*/
	sensorReportsize();
	profLogAdd(3,"sensorDoFuncSnapReadoutEnd");
}

static void sensorDoFuncSnapNext(UINT32 mode)
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
	cmdportid_tg=
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

	printf("sensorCfgStop++\n");
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	printf("sensorCfgStop--\n");
}

static void sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
#if !MASK_EXP_CMD
      sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
#endif
}

static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
}

#if SP5K_SENSOR_AR0330CS_PLCC
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regData[][4] ={
		{0x30,0x12,0x00,0x00}
	};
	regData[0][2] = (expLine>>8)&0xff;
	regData[0][3] = (expLine)&0xff;

	profLogPrintf(3,"expLine = %d [%x %x %x %x]",expLine,regData[0][0],regData[0][1],regData[0][2],regData[0][3]);
	sensorEventNewFrameWait(1);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,1, regData[0]);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
}
#endif
