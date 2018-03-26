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
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_mi5100p-1.00.17"
#define BUILD_TIME    __DATE__  "-"  __TIME__

#include "common.h"
#include "../flow/hal_front.h"
#if SPCA5110 /****TODO*5110*tggpio temp workaround*/
#include "../flow/sysregs.h"
#else
#include "hal/hal_gpio.h"
#endif
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
#include "../flow/sensor_model_snap.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_ae_api.h"
#include "middleware/proflog.h"

#define SENSOR_DEBUG       0
#if SENSOR_DEBUG
#define SEN_DBG printf
#else
#define SEN_DBG(...) 
#endif

static UINT32 g_IsSnap2Pv; 
/*static BOOL   hLV=0;*/

#define NOUSE                             (0)   
#define BY_PIXEL                          (1)
#define BY_HD                             (0)
#define DISABLE                           (0xFFFF)   
#define DELAY_MS 		                  (0xFE)
#define END_CMD  		                  (0xFF)
#define EXTHD_NUM                         (6)                                                  
#define BIT0                              (1)                    
#define BIT10                             (1<<10)               
#define SENSOR_RESET                      (BIT0)                 
#define SENSOR_PWDN                       (BIT10)  
#define MODE_SYNC_EARLY                   (50)
#define SENSOR_EVENT_VVALID_END           (1UL<<13)

/* HD mode 720P @60fps */
#define PREVIEW_720P_HSIZE   			   1280                  
#define PREVIEW_720P_VSIZE   			   720	
#define PREVIEW_720P_HOFFSET     		   16                      
#define PREVIEW_720P_VOFFSET     		   9		     	              
#define PREVIEW_720P_LINETOTAL   		   (1800)                  
#define PREVIEW_720P_LINEBLANK   		   NOUSE	              
#define PREVIEW_720P_FRAMETOTAL  		   PREVIEW_720P_VSIZE    	              
#define PREVIEW_720P_FRAMEBLANK  		   NOUSE                  
#define PREVIEW_720P_HRESHAPE_FALL         DISABLE 
#define PREVIEW_720P_HRESHAPE_RISE         DISABLE	
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL  BY_HD			
#define PREVIEW_720P_VRESHAPE_FALL         DISABLE 
#define PREVIEW_720P_VRESHAPE_RISE         DISABLE 
#define	PREVIEW_720P_HDNUM_FRAME_START     PREVIEW_720P_VOFFSET+1		
#define	PREVIEW_720P_HDNUM_FRAME_END	   2	
#define	PREVIEW_720P_HDNUM_CMDPORT	       PREVIEW_720P_VSIZE/2	

/*Full HD mode 1080P @30fps */
#define PREVIEW_1080P_HSIZE  			   1920	             
#define PREVIEW_1080P_VSIZE  			   (1080-16)
#define PREVIEW_1080P_HOFFSET     	       16  	                   
#define PREVIEW_1080P_VOFFSET     	       9		             
#define PREVIEW_1080P_LINETOTAL   	       (2000)             
#define PREVIEW_1080P_LINEBLANK   	       NOUSE		            
#define PREVIEW_1080P_FRAMETOTAL  	       PREVIEW_1080P_VSIZE  	            
#define PREVIEW_1080P_FRAMEBLANK  	       NOUSE	    	              
#define PREVIEW_1080P_HRESHAPE_FALL        DISABLE 
#define PREVIEW_1080P_HRESHAPE_RISE        DISABLE	
#define PREVIEW_1080P_VRESHAPE_COUNTER_SEL BY_HD	  
#define PREVIEW_1080P_VRESHAPE_FALL        DISABLE 
#define PREVIEW_1080P_VRESHAPE_RISE        DISABLE 
#define	PREVIEW_1080P_HDNUM_FRAME_START    PREVIEW_1080P_VOFFSET+1		
#define	PREVIEW_1080P_HDNUM_FRAME_END	   2			
#define	PREVIEW_1080P_HDNUM_CMDPORT	       PREVIEW_1080P_VSIZE/2
/* Capture default definition here */
#define CAPTURE_HSIZE  				       (2592)	                                    
#define CAPTURE_VSIZE  		               (1944-3)
#define CAPTURE_HOFFSET     		       0x4E  	                                
#define CAPTURE_VOFFSET     		       9
#define CAPTURE_LINETOTAL   		       CAPTURE_HSIZE+CAPTURE_HOFFSET+20	                                
#define CAPTURE_LINEBLANK   		       NOUSE		                            
#define CAPTURE_FRAMETOTAL  		       CAPTURE_VSIZE                                 
#define CAPTURE_FRAMEBLANK 			       NOUSE		                            
#define CAPTURE_HRESHAPE_FALL              DISABLE                                  
#define CAPTURE_HRESHAPE_RISE              DISABLE	                                
#define CAPTURE_VRESHAPE_COUNTER_SEL       BY_HD	                                    
#define CAPTURE_VRESHAPE_FALL              DISABLE                                  
#define CAPTURE_VRESHAPE_RISE              DISABLE                                  
#define	CAPTURE_HDNUM_FRAME_START	       CAPTURE_VOFFSET+1	                                
#define	CAPTURE_HDNUM_FRAME_END	           CAPTURE_VOFFSET+CAPTURE_VSIZE+1
#define	CAPTURE_HDNUM_CMDPORT	           CAPTURE_VSIZE/2
/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKMODE	    1	         /*mclk mode,'0'from I/O Pad,'1'from internal*/    
#define PCLKMODE	    0	         /*pclk mode,'0'from I/O Pad,'1'from internal*/    
#define MASTERPHASE	    0     	     /*master clk phase*/                              
#define PIXELPHASE	    6	         /*pixel clk phase*/                               
#define ADCKPHASE	    0	         /*adck clk phase*/                                
#ifdef FPGA
	#define RGBADJ		1	         /*RGB input data sample phase adj "1"negedge "0"posedge*/    
#else                                                                                             
	#define RGBADJ		0	         /*RGB input data sample phase adj "1"negedge "0"posedge*/    
#endif
/**************sensorCfgInit table define ****************************/
#define SENSORMODEL		              TYPE_CMOS	          /*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD, TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/                                                                                                                                                                                                                                                                                                                                                                                                                      
#define	TGTYPE			              TG_CMOS_SP5K_SLAVE  /*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE, TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/                                                                                                                                                                                                                                                                                                                                                                                                                                                  
#define	PREVIEW_720P_CFAPATTERN		  BAYER_GBRG          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
#define	PREVIEW_1080P_CFAPATTERN	  BAYER_GBRG          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
#define	SNAP_CFAPATTERN				  BAYER_GBRG          /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
#define	SNAP_DUMMYVD_NUM	          0                   /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM		      1      			  /*snap readout Vd need numbers*/                                                     
#define	SNAP_CAPTUREVD_NUM		      1                   /*for typical CCD==n_dummy + n_readout + 1*/                                         
#define	SNAP_EARLY_NUM			      0      			  /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/        
#define	FLDSEL_START_NUM			  0	  			      /*It arrange the start field data to the start position of the sdram,0,1,2 */        
#define	FLDSEL_END_NUM			 	  0  			      /*It arrange the end field data to the end position of the sdram,0,1,2 */            
#define	SKIP_SNAP_READOUTVD_EN        0  			      /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/ 
#define	VSHUTTER_RESEND 	          0  			      /*sony FALSE;panasonic/ITG TRUE*/                                                    
#define	EXP_FRAME_NUM       	      2	    			  /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/  
#define SNAP_PREVIEW_DELAY_FRAME_NUM  2
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
#define	TG_SPI_I2C_MODE  0xba      	                                        /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF /*don't change*/
#define	TG_FORMAT3  	 0		                                            /*don't change*/
#if SPCA5210
#define	TG_SPEED         2
#else
#define	TG_SPEED    	 FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						                      FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#endif
#define	TG_NBIT_REGDATA  16		/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  8		/*ssc/spi register address numbers */

extern UINT32 sensor_event_wait(UINT32 event);

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreview720pBegin();
static void sensorDoFuncPreview1080pBegin();
static void sensorDoFuncPreviewFullBegin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
#if SP5K_SENSOR_MI5100p
static void sensorAeStsCB(UINT32 mode , sp5kAeCustomSts_t ae_sts);
#endif

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
        .xSize = (PREVIEW_720P_HSIZE-16),
        .ySize = PREVIEW_720P_VSIZE,
    },
	.hsize = PREVIEW_720P_HSIZE,
	.vsize = PREVIEW_720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_720P_CFAPATTERN,
	.hratio=3200,
	.vratio=1800,	
	.fps_mhz = 59910000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 1641, 	
	.pix_100khz =990,
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
    .crop = &( const sensorTgCrop_t ) {
        .xOffset = 0,
        .yOffset = 0,
        .xSize = (PREVIEW_1080P_HSIZE-16),
        .ySize = PREVIEW_1080P_VSIZE,
    },
	.hsize = PREVIEW_1080P_HSIZE,
	.vsize = PREVIEW_1080P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = PREVIEW_1080P_CFAPATTERN,
	.hratio=3200,
	.vratio=1800,	
	.fps_mhz = 29900000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 1972, 
	.pix_100khz =990, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewFullCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewFullBegin,
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
	.hsize = CAPTURE_HSIZE,
	.vsize = CAPTURE_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = SNAP_CFAPATTERN,
	.hratio=4000,
	.vratio=3000,	
	.fps_mhz = 13660000,
	.sensor_pclk = 72000000,
	.sensor_htotal = 2672, 
	.pix_100khz =990, 
};

static const sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0, 	
		},
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
	},
	.crop = NULL, 
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
	.pix_100khz =720, 
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
	.hsize           = PREVIEW_720P_HSIZE,
	.vsize           = PREVIEW_720P_VSIZE+1,
	.lastlinepix     = PREVIEW_720P_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_yuvhdpol	 = 1, /*yuvhd just before hdres 0:active low*/
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_720P_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_720P_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_720P_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
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
	.hsize           = PREVIEW_1080P_HSIZE,
	.vsize           = PREVIEW_1080P_VSIZE+1,
	.lastlinepix     = PREVIEW_1080P_LINETOTAL,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_yuvhdpol	 = 1, /*yuvhd just before hdres 0:active low*/
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = PREVIEW_1080P_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = PREVIEW_1080P_HDNUM_FRAME_START,
	.hdnum_frame_end = PREVIEW_1080P_HDNUM_FRAME_END,
	.hdnum_cmdport   = PREVIEW_1080P_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
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
	.conf_yuvhdpol	 = 1, /*yuvhd just before hdres 0:active low*/
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = CAPTURE_VRESHAPE_COUNTER_SEL,
	.hdnum_frame_start = CAPTURE_HDNUM_FRAME_START,
	.hdnum_frame_end = CAPTURE_HDNUM_FRAME_END,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
                                /* Sensor Output Size                         */ 
	&sensorTgPreview1080pCfg, 	/*   1936x1096               1920x1080 @30fps */
	&sensorTgPreview720pCfg,	/*   2592x1472(b2)1296x736   1280x720  @60fps */
	&sensorTgPreviewFullCfg, 	/*   2592x1944                         @14fps */
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg,
};

static sensorTgCfg_t sensorTgCfg = {
	.tg_type = TGTYPE,
	.n_preview_submode = sizeof(sensorTgPreviewCfgTbl) /
			sizeof(sensorTgPreviewCfgTbl[0]),
	.n_snap_submode = sizeof(sensorTgSnapCfgTbl) /
			sizeof(sensorTgSnapCfgTbl[0]),
	.vshutter_need_resend = VSHUTTER_RESEND,
	.exp_effect_frame_count = EXP_FRAME_NUM,
	.n_snap_pv_delay = SNAP_PREVIEW_DELAY_FRAME_NUM,
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
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
	.defSyncSigCfg  = &sensorSyncSigPreview1080pCfg,
	.verStr = MODEL_VERSION,
};

static const UINT8 cmd_reset_code[][3] SENSOR_SIF1DMA_BUF ={
	/* [Reset] */
    {0x0d,0x00,0x01},       /* RESET_REG */
    {0x0d,0x00,0x00},    
    {END_CMD,END_CMD,END_CMD}         
};

static const UINT8 cmd_pll_code[][3] SENSOR_SIF1DMA_BUF={
	/* [96MHz + Devware Bayer Offset]*/
    /* PIXEL_CLK=96 MHz, HB=1536 --  */
    {0x05,0x00,0x00}, 		/* HORZ_BLANK_REG (HB)=0 */  
    {0x10,0x00,0x51},       /* PLL_CONTROL; POWER UP PLL */               
    {0x11,0x18,0x01},       /* PLL_CONFIG_1: m=24, n=1 */                 
    {0x12,0x00,0x02},       /* PLL_CONFIG_2: p1=2, p2=0 */                
    {DELAY_MS,1,1},         /* Wait 1ms for VCO to lock */
    {0x10,0x00,0x53},       /* PLL_CONTROL; USE PLL     */  
    {DELAY_MS,1,1},         /* <Joyce> Please try 48ms is enough and optimize to 1ms if system is stable.*/
    {0x05,0x00,0x00}, 		/* (1) HORZ_BLANK_REG=0  */    
    {0x07,0x1F,0x8E}, 		/* Enable Parallel Fifo Data */      
    {0x09,0x00,0x10},       /* <Joyce> at least 10 lines */
    {END_CMD,END_CMD,END_CMD}      
};

static const UINT8 cmd_720p_60fps_code[][3] SENSOR_SIF1DMA_BUF={
	/* 720p60_24IN-20120809-Joyce.ini */
	{0x01, 0x00, 0x40},     /*	ROW WINDOW START 64*/			                 
    {0x02, 0x00, 0x18},     /*	COL WINDOW START 24*/	
    {0x03, 0x05, 0xA3},     /*  output 722 lines */  
    {0x04, 0x09, 0xFF},     /*	COL_WINDOW_SIZE_REG 2559, 0xA1F=2591*/    
    {0x05, 0x00, 0x00},     /*	HORZ BLANK  */	                                 
    {0x06, 0x00, 0x00},     /*	VERT BLANK  */
    {0x22, 0x00, 0x11},     /*	ROW_MODE, ROW_SKIP=1, ROW_BIN=1 */		               
    {0x23, 0x00, 0x11},     /*	COL_MODE, COL_SKIP=1, COL_BIN=1 */	                  
    {0x20, 0x00, 0x60},     /*	READ_MODE_2, COL_SUM */                               
    {0x08, 0x00, 0x00},     /*	SHUTTER_WIDTH_HI */	
    {0x0C, 0x00, 0x00},     /*	SHUTTER_DELAY_REG */                    
    {0x70, 0x00, 0x5C},                   
    {0x71, 0x5B, 0x00},                   
    {0x72, 0x59, 0x00},                   
    {0x73, 0x02, 0x00},                   
    {0x74, 0x02, 0x00},                   
    {0x75, 0x28, 0x00},                   
    {0x76, 0x3E, 0x29},                   
    {0x77, 0x3E, 0x29},                   
    {0x78, 0x58, 0x3F},                   
    {0x79, 0x5B, 0x00},                   
    {0x7A, 0x5A, 0x00},                   
    {0x7B, 0x59, 0x00},                   
    {0x7C, 0x59, 0x00},                   
    {0x7E, 0x59, 0x00},                   
    {0x7F, 0x59, 0x00},    
    {0x29, 0x04, 0x81},                   
    {0x3E, 0x00, 0x87},                   
    {0x3F, 0x00, 0x07},                   
    {0x41, 0x00, 0x03},                   
    {0x48, 0x00, 0x18},                   
    {0x5F, 0x1C, 0x16},                   
    {0x1E, 0x00, 0x06},                   
    {0x4F, 0x00, 0x11},                   
    {0x57, 0x00, 0x02}, 
    {0x35, 0x00, 0x0A},                   
    {0x35, 0x00, 0x0A},                   
    {0x3E, 0x00, 0x07},                   
    {0x07, 0x09, 0x0E},                   
    {0x2C, 0x00, 0x11},  
	{END_CMD,END_CMD,END_CMD}  
};

static const UINT8 cmd_1080p_30fps_code[][3] SENSOR_SIF1DMA_BUF={
    /* 1080p30noEIS_24IN-20120813-Joyce.ini */
	{0x07, 0x1F, 0x8E},	/* CONTROL_MODE_REG                */
    {0x01, 0x01, 0xAF},	/* ROW_WINDOW_START_REG            */
    {0x02, 0x01, 0x4D},	/* COL_WINDOW_START_REG must be multiple of 3.             */
                        /*    otherwise will get saw tooth on the high light edge  */
    {0x03, 0x04, 0x3A},	/* ROW_WINDOW_SIZE_REG: 0x43A=1082 */    
    {0x04, 0x07, 0x7F},	/* COL_WINDOW_SIZE_REG             */
    {0x05, 0x00, 0x00},	/* HORZ_BLANK_REG                  */
    {0x06, 0x00, 0x00},	/* VERT_BLANK_REG                  */
    {0x08, 0x00, 0x00},	/* SHUTTER_WIDTH_HI                */
    {0x0C, 0x00, 0x00},	/* SHUTTER_DELAY_REG               */
    {0x22, 0x00, 0x00},	/* ROW_MODE                        */
    {0x23, 0x00, 0x00},	/* COL_MODE                        */
    {0x20, 0x00, 0x40}, /* READ_MODE_2                     */
    {0x06, 0x00, 0x19},	/* VERT_BLANK_REG                  */
    {0x05, 0x00, 0x00},	/* HORZ_BLANK_REG                  */
    {0x1E, 0x40, 0x06},	/* READ_MODE_1                     */
    {0x4F, 0x00, 0x11},	/* VLN_BIAS_CURRENT                */
    {0x57, 0x00, 0x02},	/* ANTI_ECLIPSE_VOLTAGE            */
    {0x3E, 0x00, 0x87},	/* TX_LOW_VOLTAGE                  */
    {0x70, 0x00, 0xBE},	/* RD_SEQ_LEN                      */
    {0x71, 0xBE, 0x00},	/* RD_PTR_TIMING                   */
    {0x71, 0xBE, 0x00},	/* RD_PTR_TIMING                   */
    {0x71, 0xBE, 0x00},	/* RD_PTR_TIMING                   */
    {0x72, 0xBE, 0x00},	/* RD_ROW_EN_TIMING                */
    {0x72, 0xBE, 0x00},	/* RD_ROW_EN_TIMING                */
    {0x72, 0xBE, 0x00},	/* RD_ROW_EN_TIMING                */
    {0x73, 0x03, 0x00},	/* RD_RST_EN_TIMING                */
    {0x73, 0x03, 0x00},	/* RD_RST_EN_TIMING                */
    {0x73, 0x03, 0x00},	/* RD_RST_EN_TIMING                */
    {0x74, 0x03, 0x00},	/* RD_BOOST_RST_TIMING             */
    {0x74, 0x03, 0x00},	/* RD_BOOST_RST_TIMING             */
    {0x74, 0x03, 0x00},	/* RD_BOOST_RST_TIMING             */
    {0x75, 0x3C, 0x00},	/* RD_SHR_TIMING                   */
    {0x75, 0x3C, 0x00},	/* RD_SHR_TIMING                   */
    {0x75, 0x3C, 0x00},	/* RD_SHR_TIMING                   */
    {0x76, 0x8D, 0x3D},	/* RD_TX_EN_TIMING                 */
    {0x76, 0x8D, 0x3D},	/* RD_TX_EN_TIMING                 */
    {0x76, 0x8D, 0x3D},	/* RD_TX_EN_TIMING                 */
    {0x77, 0x4E, 0x44},	/* RD_BOOST_TX_TIMING              */
    {0x78, 0xB6, 0x8E},	/* RD_SHS_TIMING                   */
    {0x78, 0xB6, 0x8E},	/* RD_SHS_TIMING                   */
    {0x78, 0xB6, 0x8E},	/* RD_SHS_TIMING                   */
    {0x79, 0xBE, 0x00},	/* RD_VCL_COL_EN_TIMING            */
    {0x79, 0xBE, 0x00},	/* RD_VCL_COL_EN_TIMING            */
    {0x79, 0xBE, 0x00},	/* RD_VCL_COL_EN_TIMING            */
    {0x7A, 0xBE, 0x00},	/* RD_COLCLAMP_TIMING              */
    {0x7A, 0xBE, 0x00},	/* RD_COLCLAMP_TIMING              */
    {0x7A, 0xBE, 0x00},	/* RD_COLCLAMP_TIMING              */
    {0x7B, 0xBE, 0x00},	/* RD_SHVCL_BAR_TIMING             */
    {0x7B, 0xBE, 0x00},	/* RD_SHVCL_BAR_TIMING             */
    {0x7B, 0xBE, 0x00},	/* RD_SHVCL_BAR_TIMING             */
    {0x7C, 0xBE, 0x00},	/* RD_VLN_EN_TIMING                */
    {0x7C, 0xBE, 0x00},	/* RD_VLN_EN_TIMING                */
    {0x7C, 0xBE, 0x00},	/* RD_VLN_EN_TIMING                */
    {0x7D, 0xFF, 0x00},	/* READ_PULLUP_TIMING              */
    {0x7D, 0xFF, 0x00},	/* READ_PULLUP_TIMING              */
    {0x7D, 0xBE, 0x00},	/* READ_PULLUP_TIMING              */
    {0x7E, 0xBE, 0x00},	/* RD_BOOST_ROW_TIMING             */
    {0x7E, 0xBE, 0x00},	/* RD_BOOST_ROW_TIMING             */
    {0x7E, 0xBE, 0x00},	/* RD_BOOST_ROW_TIMING             */
    {0x7F, 0xBE, 0x00},	/* RD_EN_VLN_SH_TIMING             */
    {0x7F, 0xBE, 0x00},	/* RD_EN_VLN_SH_TIMING             */
    {0x7F, 0xBE, 0x00},	/* RD_EN_VLN_SH_TIMING             */
    {0x80, 0x00, 0x1E},	/* SHUTTER_SEQ_LEN                 */
    {0x81, 0x1F, 0x04},	/* SHUTTER_PTR_TIMING              */
    {0x82, 0x00, 0x00},	/* SHUTTER_ROW_EN_TIMING           */
    {0x83, 0x1B, 0x06},	/* SHUTTER_RST_EN_TIMING           */
    {0x83, 0x1B, 0x06},	/* SHUTTER_RST_EN_TIMING           */
    {0x83, 0x1B, 0x06},	/* SHUTTER_RST_EN_TIMING           */
    {0x84, 0x1D, 0x08},	/* SHUTTER_BOOST_RST_TIMING        */
    {0x84, 0x1D, 0x08},	/* SHUTTER_BOOST_RST_TIMING        */
    {0x84, 0x1D, 0x08},	/* SHUTTER_BOOST_RST_TIMING        */
    {0x86, 0x18, 0x06},	/* SHUTTER_TX_EN_TIMING            */
    {0x87, 0x1A, 0x08},	/* SHUTTER_BOOST_TX_TIMING         */
    {0x1E, 0x00, 0x06},	/* READ_MODE_1                     */
    {0x4F, 0x00, 0x11},	/* VLN_BIAS_CURRENT                */
    {0x57, 0x00, 0x02},	/* ANTI_ECLIPSE_VOLTAGE            */
    {0x35, 0x00, 0x0A},	/* GLOBAL_GAIN_REG                 */
    {0x35, 0x00, 0x0A},	/* GLOBAL_GAIN_REG                 */
    {0x3E, 0x00, 0x07},	/* TX_LOW_VOLTAGE                  */
	{END_CMD,END_CMD,END_CMD}                               
};                                                          
                                                                    
static const UINT8 cmd_still_code[][3] SENSOR_SIF1DMA_BUF={
	/* [Still 2592x1944 14FPS]  SETTINGS FOR STILL (2592 x 1944) Added at 13th/Jul/2012 */
	/* LOAD = Reset                             */
    /* LOAD = 96MHz + Devware Bayer Offset      */
    {0x01, 0x00, 0x36},		/*	ROW WINDOW START 54*/			   
	{0x02, 0x00, 0x10},		/*	COL WINDOW START 16*/			   
	{0x03, 0x07, 0x97},		/*	ROW_WINDOW_SIZE_REG 1943=0x797*/			   
	{0x04, 0x0a, 0x2f},		/*	COL_WINDOW_SIZE_REG 2607=0xa2f*/			   
	{0x05, 0x01, 0xc2},		/*	HORZ BLANK 0x01C2=450 */
    {0x06, 0x00, 0x1e},
	{0x22, 0x00, 0x00},		/*	ROW_MODE */			   
	{0x23, 0x00, 0x00}, 	/*	COL_MODE */	
    {0x20, 0x00, 0x40}, 	/*	READ_MODE_2 */
	{0x08, 0x00, 0x00},		/*	SHUTTER_WIDTH_HI */
	{0x0c, 0x00, 0x00},		/*	SHUTTER_DELAY_REG */
    {0x07,0x1F,0x8E},       /* Enable Parallel Fifo Data */  
	/* LOAD = Full Resolution Optimization - 15FPS @ 96MHz */
    /* LOAD = Demo Initialization                          */
    /* LOAD=Hidden: MIN_ISO(ISO_150)   Updated 01/26/2011  */
    {END_CMD,END_CMD,END_CMD}  
};

static const UINT8 cmd_full_opti_code[][3] SENSOR_SIF1DMA_BUF={
	/* [Full Resolution Optimization - 15FPS @ 96MHz] */
    {0x70, 0x00, 0xBE},		/*	Reserved */			   
	{0x71, 0xBE, 0x00},		/*	Reserved */			   
	{0x72, 0xBE, 0x00},		/*	Reserved */			   
	{0x73, 0x03, 0x00},		/*	Reserved */			   
	{0x74, 0x03, 0x00},		/*	Reserved */			   
	{0x75, 0x3c, 0x00},		/*	Reserved */			   
	{0x76, 0x8d, 0x3d},		/*	Reserved */			   
	{0x77, 0x4e, 0x44},		/*	Reserved */			   
	{0x78, 0xb6, 0x8e},		/*	Reserved */			   
	{0x79, 0xBE, 0x00},		/*	Reserved */			   
	{0x7a, 0xBE, 0x00}, 	/*	Reserved */		 
	{0x7b, 0xBE, 0x00},     /*  Reserved */
	{0x7c, 0xBE, 0x00}, 	/*	Reserved */	
	{0x7d, 0xBE, 0x00}, 	/*	Reserved */		 
	{0x7e, 0xBE, 0x00},     /*  Reserved */
	{0x7f, 0xBE, 0x00},     /*	Reserved */
	{0x29, 0x04, 0x81}, 	/*	Reserved */		 
	{0x3e, 0x00, 0x87},     /*  Reserved */
	{0x3f, 0x00, 0x07}, 	/*	Reserved */		 
	{0x41, 0x00, 0x03}, 	/*	Reserved */		 							  
	{0x48, 0x00, 0x18},     /*  Reserved */
	{0x5f, 0x1c, 0x16},     /*	Reserved */
	{END_CMD,END_CMD,END_CMD}    
};

void
sensorMI_BatchWrite(const UINT8 *pdata)
{
	UINT32 cmdportid;
	UINT8 regdata[3];
	UINT16 i;
	cmdportid = sensorTgCfg.cmdportid_xtg;
    sensorSerialFlush(cmdportid);
	for(i=0;i<1000;i++)
	{
		if(pdata[i*3]==END_CMD)
			break;

		if(pdata[i*3]==DELAY_MS)
			tmrUsWait((pdata[i*3+1])*1000);	
		else
		{
			regdata[0]=pdata[i*3];
			regdata[1]=pdata[i*3+1];
			regdata[2]=pdata[i*3+2];
         	sensorSerialBulkWrite(cmdportid,1,regdata,0);
		}
	}
}

UINT16
MIsensorRead(UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[3];
	cmdportid = sensorTgCfg.cmdportid_xtg;
    regread[0]=addr&0xFF;
    sensorSerialBlockRead(cmdportid,1,regread,0);
	return (regread[2]|(regread[1]<<8));
}

void
sensorTg_mi5100p_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

void	
sensorMI_SRST(void)
{
    SEN_DBG("sensorMI_SRST !!\n"); 
    sensorMI_BatchWrite(cmd_reset_code[0]); 
	tmrUsWait(1000); 
    sensorMI_BatchWrite(cmd_pll_code[0]); 
	sensorMI_BatchWrite(cmd_1080p_30fps_code[0]); 
	SEN_DBG("MI5110 Sensor ID: %x \n", MIsensorRead(0x00));	
}

static void
sensorDoFuncInit()
{
	UINT32 cmdportid;

	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_PWDN|SENSOR_RESET,SENSOR_PWDN|SENSOR_RESET);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_PWDN, 0);   
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET, 0); 
	halFrontClkCfg(MCLKMODE,1);
    sensorTgPllClkExtSet(24000000);
/* Master clock and Pixel clock configuration here */
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
/* Internal clock phase initialization here */
	halFrontImgSrcCfgSet(FRONT_IMG_SRC_IN_PCLK_EDGE, RGBADJ);
	/*****TODO*****front reset after these???*/
	cmdportid = sensorTgCfg.cmdportid_xtg;
    tmrUsWait(1000);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET);
	tmrUsWait(1000);
    sensorMI_SRST();
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1);    
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	halFrontClkCfg(MCLKMODE, PCLKMODE);
    halFrontSyncSigCfgSet(FRONT_SYNC_EXTHD_NUM, EXTHD_NUM, 1);
#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);/*config precmd position (cmdport -MODE_SYNC_EARLY) */
#endif
#if SP5K_SENSOR_MI5100p
    sp5kAeStsCbSet(sensorAeStsCB);
#endif    

	profLogPrintf(0,"MI51HDp Sensor driver version: %s ",MODEL_VERSION);
	profLogPrintf(0,"51HD plus check start.....");
	profLogPrintf(0,"sensor Chip ID  [0x%x]",MIsensorRead(0x00));
	profLogPrintf(0,"sensor Fusel ID [0x%x 0x%x 0x%x 0x%x]",MIsensorRead(0xfa),MIsensorRead(0xfb),MIsensorRead(0xfc),MIsensorRead(0xfd));
	if(((MIsensorRead(0xfd)>>5)&0x7F)==0x11)
		profLogPrintf(0,"sensor model check OK ==> 51HD plus");
	else
		profLogPrintf(0,"sensor model check NG ==> 51HD or 5100");


}

static void
sensorDoFuncStop()
{
}

static void
sensorDoFuncPreview720pBegin()
{
    if(g_IsSnap2Pv){
		sensor_event_wait(SENSOR_EVENT_VVALID_END);
    }
	g_IsSnap2Pv=0;
    sensorSyncSigInit(&sensorSyncSigPreview720pCfg);
    sensorMI_BatchWrite(cmd_720p_60fps_code[0]); 
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
}

static void
sensorDoFuncPreview1080pBegin()
{
    if(g_IsSnap2Pv){
		sensor_event_wait(SENSOR_EVENT_VVALID_END);
    }
	g_IsSnap2Pv=0; 
    sensorSyncSigInit(&sensorSyncSigPreview1080pCfg);
    sensorMI_BatchWrite(cmd_1080p_30fps_code[0]);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
}

static void
sensorDoFuncPreviewFullBegin()
{
    if(g_IsSnap2Pv){
		sensor_event_wait(SENSOR_EVENT_VVALID_END);
    }
	g_IsSnap2Pv=0; 
    sensorSyncSigInit(&sensorSyncSigSnapCfg);
    sensorMI_BatchWrite(cmd_still_code[0]); 
	sensorMI_BatchWrite(cmd_full_opti_code[0]);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
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
sensorDoFuncSnapEarly0()
{
}

static void
sensorDoFuncSnapExpBegin()
{
    if(!g_IsSnap2Pv){
		g_IsSnap2Pv=1;
        
        sensorSyncSigSet(&sensorSyncSigSnapCfg);
        sensorMI_BatchWrite(cmd_still_code[0]); 
	    sensorMI_BatchWrite(cmd_full_opti_code[0]);
        sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    }    
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
}

static void
sensorDoFuncSnapReadoutEnd()
{
}

/*****TODO***/
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
	SEN_DBG("sensorCfgInit++\n");
	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);
	/* if support burst
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_BURST; */
	/* if support seq
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_SEQ; */

	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_init();
	SEN_DBG("sensorCfgInit--\n");
}

static void
sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	SEN_DBG("sensorCfgStop++\n");
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	SEN_DBG("sensorCfgStop--\n");
}

static void
sensor_cmdport_exp_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
}

static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
}

#if SP5K_SENSOR_MI5100p

void sensorCmdPortExpLineSend(UINT16 expLine)
{
    UINT8 regData[3] ={
		0x09,0x00,0x00
	};
	regData[1] = (expLine>>8)&0xff;
	regData[2] = expLine&0xff;

	SEN_DBG("MI51HDp(MT9P006)_expLine = %d\n",expLine);
	sensor_event_wait(0x04);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regData);
}

static void sensorAeStsCB(UINT32 mode, sp5kAeCustomSts_t ae_sts)
{
    static UINT8 regData[1][3] ={
		{0x7F,0x00,0x00},
	};

    if((ae_sts.lv>>10) > 8){
        if(!hLV)
        {            
        regData[0][1] = 0x00;
        sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,1, regData[0]); 
        sensorSerialFlush(sensorTgCfg.cmdportid_exp);
        hLV=1;
            SEN_DBG("sensorAeStsCB !hLV--\n");
        }
    }else{
        if(hLV){
        	UINT32 smode;                                           
	        smode=sensorModeGet();                                  

            if(smode == 0x30){                                      
                regData[0][1] = 0xbe;                               
            }                                                       
            if(smode == 0x31){                                      
                regData[0][1] = 0x59;                               
            } 
            sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,1, regData[0]); 
            sensorSerialFlush(sensorTgCfg.cmdportid_exp);
            hLV=0;                                                              
            SEN_DBG("sensorAeStsCB hLV--\n");
        }
    }
}
#endif
