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
 *  Author: Allen Liu                                                  
 *                                                                        
 *  Version: 01.01.01, 20130613:
 *  0x30(720P):size 1296X800@60fps, 480Mbps/lane, 2 lanes 12-bit
 *  0x31(FHD):size 960X800@60fps, 480Mbps/lane, 2 lanes 12-bit
 *  0x40(720P):size 1296X800@60fps, 480Mbps/lane, 2 lanes 12-bit
 *  0x41(FHD):size 960X800@60fps, 480Mbps/lane, 2 lanes 12-bit
 *
 **************************************************************************/
#define MODEL_VERSION "$unp_tg_ov9714-01.01.01"
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

/*#define PROF_LOG_ADD(id,msg)            		profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_PRINT(id,str,args...)  	profLogPrintf(id,str,##args)*/

#define MASK_EXP_CMD	0

#define NO             	0
#define BY_PIXEL       	1
#define BY_HD          	0
#define NOUSE          	0
#define DISABLE        	0xFFFF

#define MODE_SYNC_EARLY	50
#define EXTEND_LINES		2

#if defined(HW_EVB_SPE7001)
#define SENSOR_FREX          			(1<<15)
#define SENSOR_PWRDN         			(1<<14)
#define SENSOR_RESET        	 		(1<<12)
#define OUT_SENSOR_GPIO_MASK     	SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR    		SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define SENOR_GPIO_GRP           		SP5K_GPIO_GRP_LMI
#endif

/* For size 1296*800, 60fps */
#define _720P_HSIZE  (1296)  	/* preview horiz. size*/
#define _720P_VSIZE  (800)  	/* preview vertical size*/
/* Default definition here */
#define _720P_LINETOTAL   		(0x1700)				/* preview Hd length*/
#define _720P_LINEBLANK   		(6)					/* preview Hd sync length*/
#define _720P_FRAMETOTAL  		(813)				/* preview Vd length*/
#define _720P_FRAMEBLANK  		(813)				/* preview Vd sync length*/
#define _720P_HOFFSET     		(0x11a0)  			/* preview Hd valid pixel data offset position*/
#define _720P_VOFFSET     		(EXTEND_LINES+2)   	/* preview Vd valid line data offset position*/
#define _720P_HRESHAPE_FALL	(0x3cc)				/* preview Hd reshap fall*/
#define _720P_HRESHAPE_RISE    	(0x430)				/* preview Hd reshap rise*/
#define _720P_VRESHAPE_COUNTER_SEL  BY_HD			/* preview Vd reshap control*/
#define _720P_VRESHAPE_FALL     (_720P_VOFFSET-2) 	/* preview Vd reshap fall*/
#define _720P_VRESHAPE_RISE     (_720P_VOFFSET-1) 	/* preview Vd reshap rise*/

/* For size 960*800, 60fps */
#define FHD_HSIZE  (960)  /* preview horiz. size*/
#define FHD_VSIZE  (800)  	/* preview vertical size*/
/* Default definition here */
#define FHD_LINETOTAL   			(0x1700)				/* preview Hd length*/
#define FHD_LINEBLANK   			(6)					/* preview Hd sync length*/
#define FHD_FRAMETOTAL  		(813)				/* preview Vd length*/
#define FHD_FRAMEBLANK  		(813)				/* preview Vd sync length*/
#define FHD_HOFFSET     			(0x11a0)  				/* preview Hd valid pixel data offset position*/
#define FHD_VOFFSET     			(EXTEND_LINES+2)   	/* preview Vd valid line data offset position*/
#define FHD_HRESHAPE_FALL		(0x880)				/* preview Hd reshap fall*/
#define FHD_HRESHAPE_RISE    	(0x8f0)				/* preview Hd reshap rise*/
#define FHD_VRESHAPE_COUNTER_SEL  BY_HD			/* preview Vd reshap control*/
#define FHD_VRESHAPE_FALL     (FHD_VOFFSET-2) 		/* preview Vd reshap fall*/
#define FHD_VRESHAPE_RISE     (FHD_VOFFSET-1) 		/* preview Vd reshap rise*/

/****************************Front Set ****************************/
/* TGPLL,MCLK,PCLK,ADCK ebnable,freq,phase signal definition here */
#define MCLKDRV     		1     /* mclk drivng, '1' : 8mA, '0' : 4mA */
#define MCLKMODE		1	/*mclk mode,'0'from I/O Pad,'1'from internal*/
#define PCLKMODE		2	/*pclk mode,'0'from I/O Pad,'1'from internal*/
#define TGPLLEN   		1	/*enable TGPLL,'0'disable,'1'enable*/
#define TGPLLCLKSET		8	/*TGPLL clk 0/192MHz,1/144MHz,2/135MHz,3/126MHz,4/120MHz,
							   5/108MHz,6/98.18MHz,7/97.2MHz,8/96MHz,9/90MHz,10/72MHz*/
#define MCLKDIV			4	/*MCLK div,tgpllen = 0 tgpll disable ,tgpllen = 1 master clock = (TGpll clk / mclkdiv)*/
#define PCLKDIV			2	/*pixel clock = (master clock) / pclkdiv*/
#define MASTERPHASE	0     	/*master clk phase*/
#define PIXELPHASE		3	/*pixel clk phase*/
#define ADCKPHASE		0	/*adck clk phase*/

#define PCLKNXSEL_INT	0x00
#define PCLKNXSEL_LVDS	0x01
#define PCLKNXSEL_EXT	0x02
#define PCLK1XSEL_NX	0x00
#define PCLK1XSEL_NX_2	0x04
#define PCLK1XSEL_INT	0x08

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL	TYPE_CMOS	    			/*TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						      							TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define TGTYPE			TG_CMOS_SP5K_SLAVE 	/*TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
						      							TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
						      						
#define _720P_CFAPATTERN		BAYER_BGGR	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define FHD_CFAPATTERN			BAYER_BGGR	/* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define SNAP_DUMMYVD_NUM		1                   /*snap dummy Vd need numbers*/
#define SNAP_EARLY_NUM			0	            	/*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define SNAP_READOUTVD_NUM	1	            	/*snap readout Vd need numbers*/
#define SNAP_CAPTUREVD_NUM	1	            	/*for typical CCD==n_dummy + n_readout + 1*/

#define FLDSEL_START_NUM			0		/*It arrange the start field data to the start position of the sdram,0,1,2 */
#define FLDSEL_END_NUM				0	 	/*It arrange the end field data to the end position of the sdram,0,1,2 */
#define SKIP_SNAP_READOUTVD_EN  	0		/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define VSHUTTER_RESEND 			0		/*sony FALSE;panasonic/ITG TRUE*/

#define _720P_HDNUM_FRAME_START  	110		/*the front interrupt line number of the preview new frame*/
#define _720P_HDNUM_FRAME_END	2		/*the rear interrupt line number of the preview new frame*/
#define _720P_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/
#define FHD_HDNUM_FRAME_START  	110		/*the front interrupt line number of the preview new frame*/
#define FHD_HDNUM_FRAME_END		2		/*the rear interrupt line number of the preview new frame*/
#define FHD_HDNUM_CMDPORT		400		/*the command port interrupt line number of the preview new frame*/

#define EXP_FRAME_NUM       	2	    /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/

/**************the command port condition definition****************************/
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
#define	TG_SPI_I2C_MODE  	0x20     	       /*I2C slave addr for write*/
#define	TG_FORMAT2  	 	FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	 	0		      /*don't change*/
#define	TG_SPEED    	 		FRONT_SSC_CLK_384K /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
													FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/
#define	TG_NBIT_REGDATA  	8	/*ssc/spi register data numbers */
#define	TG_NBIT_REGADDR  	16	/*ssc/spi register address numbers */

/**************global variable *******************/
UINT32 cmdportid_tg;

extern UINT32 sensorModeGet(void);
/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreview720PBegin();
static void sensorDoFuncPreviewFHDBegin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapDummy0();
static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();

/******************** uCode ****************************/
static const UINT8 afe_init_code_SWReset[][3]={
{0x01,0x03,0x01}
};

static const UINT8 afe_init_code[][3]={
/*-----------------------------------------------
    MCLK: 24Mhz
    Mipi : 2 lane
    Mipi data rate: 480Mbps/Lane
    Pixel clock: 80MHz
    FPS	       : 60fps
    HTS		: 1598(0x342:0x343)
    VTS		: 836(0x340:01x341)
    Tline 	    	: 19.98us
    Max exp line	: 832 ( VTS-4)
    -------------------------------------------------*/
/*stream off*/
{0x01, 0x00 ,0x00},
/* IMAGE ORIENTATION Bit[1]: v_flip, Bit[0]: h_mirror */
{0x01, 0x01 ,0x00},
/* PAD CTRL Bit[4]: strobe_in NOR opt, Bit[1:0]: I/O strength */
{0x30, 0x01 ,0x01},
/* sensor control [0x3700 ~ 0x374B] */
{0x37, 0x12 ,0x7a},
{0x37, 0x14 ,0x90},
{0x37, 0x29 ,0x04},
{0x37, 0x31 ,0x14},
/* analog control [0x3600 ~ 0x3684] */
{0x36, 0x24 ,0x77},
{0x36, 0x21 ,0x44},
{0x36, 0x23 ,0xcc},
/* sensor control [0x3700 ~ 0x374B] */
{0x37, 0x1e ,0x30},
/* analog control [0x3600 ~ 0x3684] */
{0x36, 0x01 ,0x44},
{0x36, 0x03 ,0x63},
{0x36, 0x35 ,0x05},
{0x36, 0x32 ,0xa7},
{0x36, 0x34 ,0x57},
/* sensor control [0x3700 ~ 0x374B] */
{0x37, 0x0b ,0x01},
{0x37, 0x11 ,0x0e},
/* general configuration and image size [0x0000 ~ 0x0387] */
{0x03, 0x40 ,0x03}, /* FRAME LENGTH LINES Total vertical size[15:8] */
{0x03, 0x41 ,0x44}, /* FRAME LENGTH LINES Total vertical size[7:0] *//* 836 */
{0x03, 0x42 ,0x06}, /* LINE LENGTH PCK Total horizontal size[15:8] */
{0x03, 0x43 ,0x3e}, /* LINE LENGTH PCK Total horizontal size[7:0] *//* 1598 */
{0x03, 0x44 ,0x00}, /* X ADDR START x_addr_start[11:8] */
{0x03, 0x45 ,0x00}, /* X ADDR START x_addr_start[7:0] *//* 0 */
{0x03, 0x46 ,0x00}, /* Y ADDR START y_addr_start[11:8] */
{0x03, 0x47 ,0x00}, /* Y ADDR START y_addr_start[7:0] *//* 0 */
{0x03, 0x48 ,0x05}, /* X ADDR END x_addr_end_int horizontal width[11:8] */
{0x03, 0x49 ,0x1f}, /* X ADDR END x_addr_end_int horizontal width[7:0] *//* 1311 */
{0x03, 0x4a ,0x03}, /* Y ADDR END y_addr_end_int vertical height[11:8] */
{0x03, 0x4b ,0x2f}, /* Y ADDR END y_addr_end_int vertical height[7:0] *//* 815 */
{0x03, 0x4c ,0x05}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
{0x03, 0x4d ,0x10}, /* X OUTPUT SIZE Image output horizontal width[7:0] *//* 1296 */
{0x03, 0x4e ,0x03}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
{0x03, 0x4f ,0x2c}, /* Y OUTPUT SIZE Image output vertical height[7:0] *//* 812 */
{0x03, 0x81 ,0x01}, /* X EVEN INC Bit[3:0]: h_even_inc, Horizontal subsample even increase numbe */
{0x03, 0x83 ,0x01}, /* X ODD INC Bit[3:0]: h_odd_inc, Horizontal subsample odd increase number */
{0x03, 0x85 ,0x01}, /* Y EVEN INC Bit[3:0]: v_even_inc, Vertical subsample even increase number*/
{0x03, 0x87 ,0x01}, /* Y ODD INC Bit[3:0]: v_odd_inc, Vertical subsample odd increase number*/
{0x30, 0x02 ,0x09}, /* PAD OEN2 Bit[5]: io_sid_oen
                                Bit[3]: io_strobe_oen
                                Bit[2]: io_sda_oen
                                Bit[0]: io_gpio_oen */
{0x30, 0x12 ,0x20}, /* A MIPI PK Bit[7:6]: pgm_hspredrv_iset
                                Bias current control for LVDS
                                Bit[5:4]: Lph
                                Bit[3]: phy_mode, 0: MIPI , 1: LVDS
                                Bit[2]: predrv_sw
                                Bit[1:0]: Ictl Bias current control for MIPI */
{0x30, 0x13 ,0x12}, /* A MIPI PK Bit[7:6]: pgm_vcm MIPI high speed common-mode voltage control
                                 Bit[5:4]: pgm_lptx Register for driving strength of  low speed transmitter
                                 Bit[3]: bp_d_hs_en_lat Bypass high speed data lane enable latch
                                 Bit[2]: bp_c_hs_en_lat Bypass high speed clock lane enable latch
                                 Bit[1]: mipi_pad MIPI mode pad switch input (default is MIPI mode) */
{0x30, 0x14 ,0x84}, /* MIPI SC O Bit[7:6]: mipi_ck_skew_o Clock lane skew adjustment
                                 Bit[5]: lane_dis2_op Second data lane disable
                                 Bit[4]: mipi_pd Power down mipi_phy except low speed receiver
                                 Bit[2]: mipi_en MIPI mode enable
                                 Bit[1]: Not used
                                 Bit[0]: First data lane disable */
{0x30, 0x1f ,0x83}, /* MISC CTRL Bit[7:6]: mipi_d1_skew_o First data lane skew adjustment
                                 Bit[5]: mipi_clk_lane_ctrl
                                     0: Clock lane hold lp00 when pd_mipi
                                     1: Clock lane is high-z when pd_mipi
                                 Bit[4]: mipi_ctr_en
                                     0: Disable the function
                                     1: Enable MIPI remote reset and suspend control sc
                                 Bit[3]: mipi_rst_sel
                                     0: MIPI remote reset all registers
                                     1: MIPI remote reset all digital module
                                 Bit[1]: lane_num
                                     0: 1-lane
                                     1: 2-lane
                                 Bit[0]: cen_global_o*/
{0x30, 0x20 ,0x02}, /* MISC CTRL1 Bit[1:0]: mipi_d2_skew_o Second data lane skew adjustment*/
{0x30, 0x24 ,0x20}, /* PAD SEL2 Bit[7]: io_fsin_sel
                                Bit[6]: io_sid_sel
                                Bit[2]: io_strobe_sel
                                Bit[1]: io_sda_sel */
{0x30, 0x90 ,0x01}, /* PLL10 Bit[2:0]: pll2_pre_div_o[2:0] */
{0x30, 0x91 ,0x05}, /* PLL11 Bit[5:0]: pll2_divp_o[5:0] */
{0x30, 0x92 ,0x03}, /* PLL12 Bit[4:0]: pll2_divs_o[4:0] */
{0x30, 0x93 ,0x01}, /* PLL13 Bit[1:0]: pll2_seld5_o[1:0] 0x: /1 , 10: /2 , 11: /2.5 */
{0x30, 0x94 ,0x02}, /* PLL14 Bit[1:0]: pll2_divr */
{0x30, 0x9a ,0x01}, /* PLL1A Bit[4:0]: pll3_divs_o[4:0] */
{0x30, 0x9b ,0x01}, /* PLL1B Bit[1:0]: pll3_seld5_o[1:0] */
{0x30, 0x9c ,0x01}, /* PLL1C Bit[1:0]: pll3_divr[1:0] */
{0x30, 0xa3 ,0x19}, /* PLL23 Bit[5:0]: pll3_divp_o[5:0] */
{0x30, 0xa4 ,0x01}, /* PLL REG Bit[7:0]: Debug mode */
{0x30, 0xa5 ,0x03}, /* PLL25 Bit[2:0]: pll3_pre_div_opt[2:0] */
{0x30, 0xb3 ,0x28}, /* PLL MULTIPLIER Bit[7:0]: pll_multiplier[7:0] */
{0x30, 0xb5 ,0x04}, /* PLL1 OP PIX CLK DIV Bit[2:0]: pll1_op_pix_clk_div_r[2:0] 0x03: 6 , 0x04: 8 , 0x05: 10 , 0x10: 12 */
{0x30, 0xb6 ,0x02}, /* PLL1 OP SYS CLK DIV Bit[3:0]: pll1_op_sys_clk_div[3:0] */
{0x30, 0xb7 ,0x02}, /* PLL1 OP 2LANE CLK DIV Bit[1:0]: pll1_op_2lane_clk_div[1:0] */
{0x30, 0xb8 ,0x01}, /* PLL1 R PREDIV Bit[2:0]: pll1_r_prediv[2:0] */
/* NOT in Register file */
{0x31, 0x03 ,0x00},
/* group hold control [0x3200 ~ 0x320C] */
{0x32, 0x00 ,0x00},
{0x32, 0x01 ,0x03},
{0x32, 0x02 ,0x06},
{0x32, 0x03 ,0x08},
{0x32, 0x08 ,0x00},
{0x32, 0x09 ,0x00},
{0x32, 0x0a ,0x00},
{0x32, 0x0b ,0x01},
/* manual AEC/AGC [0x3500 ~ 0x3A07, 0x5900 ~ 0x5910] */
{0x35, 0x00 ,0x00},
{0x35, 0x01 ,0x33},
{0x35, 0x02 ,0xc0},
{0x35, 0x03 ,0x0f}, /*Bit[3]=1:agc no delay*/
{0x35, 0x04 ,0x00},
{0x35, 0x05 ,0x20},
{0x35, 0x06 ,0x00},
{0x35, 0x07 ,0x00},
{0x35, 0x08 ,0x20},
{0x35, 0x09 ,0x00},
{0x35, 0x0a ,0x00},
{0x35, 0x0b ,0x10},
{0x35, 0x0c ,0x00},
{0x35, 0x0d ,0x10},
{0x35, 0x0f ,0x10},
/* analog control [0x3600 ~ 0x3684] */
{0x36, 0x28 ,0x12},
{0x36, 0x33 ,0x14},
{0x36, 0x41 ,0x83},
{0x36, 0x43 ,0x14},
{0x36, 0x60 ,0x00},
{0x36, 0x62 ,0x10},
{0x36, 0x67 ,0x00}, /* GPIO output data select Bit[4]: pad_gpio0_opt 0: VSYNC , 1: HSYNC*/
/* sensor control [0x3700 ~ 0x374B] */
{0x37, 0x0d ,0xc0},
{0x37, 0x0e ,0x80},
{0x37, 0x0a ,0x31},
{0x37, 0x14 ,0x80},
{0x37, 0x1f ,0xa0},
{0x37, 0x30 ,0x86},
{0x37, 0x32 ,0x14},
/* timing control [0x3800 ~ 0x3832] */
{0x38, 0x11 ,0x08}, /* Hoffset */
{0x38, 0x13 ,0x02}, /* Voffset */
{0x38, 0x20 ,0x80},
{0x38, 0x21 ,0x00},
{0x38, 0x2a ,0x00},
{0x38, 0x32 ,0x80},
/* LED control [0x3900 ~ 0x391C] */
{0x39, 0x02 ,0x08},
{0x39, 0x03 ,0x00},
{0x39, 0x12 ,0x80},
{0x39, 0x13 ,0x00},
{0x39, 0x07 ,0x80},
{0x39, 0x03 ,0x00},
/* AEC */
{0x3a, 0x02 ,0x40}, /* AEC CTRL02 AEC stable range high limit */
{0x3a, 0x03 ,0x30}, /* AEC CTRL03 Bit[7:0]: AEC stable range low limit */
{0x3a, 0x04 ,0x03}, /* AEC CTRL04 Maximum exposure[15:8] */
{0x3a, 0x05 ,0x40}, /* AEC CTRL05 Bit[7:0]: Maximum exposure[7:0] */
/* strobe control [0x3B00 ~ 0x3B05] */
{0x3b, 0x00 ,0x00},
{0x3b, 0x02 ,0x00},
{0x3b, 0x03 ,0x00},
{0x3b, 0x04 ,0x00},
/* NOT in registre file */
{0x3b, 0x40 ,0x00},
{0x3b, 0x41 ,0x00},
{0x3b, 0x42 ,0x00},
{0x3b, 0x43 ,0x00},
{0x3b, 0x44 ,0x00},
{0x3b, 0x45 ,0x00},
{0x3b, 0x46 ,0x00},
{0x3b, 0x47 ,0x00},
{0x3b, 0x48 ,0x00},
{0x3b, 0x49 ,0x00},
{0x3b, 0x4a ,0x00},
{0x3b, 0x4b ,0x00},
{0x3b, 0x4c ,0x00},
{0x3b, 0x4d ,0x00},
{0x3b, 0x4e ,0x00},
/* NOT in register file */
{0x3e, 0x00 ,0x00},
{0x3e, 0x01 ,0x00},
{0x3e, 0x02 ,0x0f},
{0x3e, 0x03 ,0xdb},
{0x3e, 0x06 ,0x03},
{0x3e, 0x07 ,0x20},
/* NOT in register file */
{0x3f, 0x04 ,0xd0},
{0x3f, 0x05 ,0x00},
{0x3f, 0x00 ,0x03},
{0x3f, 0x11 ,0x06},
{0x3f, 0x13 ,0x20},
/* NOT in register file */
{0x41, 0x40 ,0x98},
{0x41, 0x41 ,0x41},
{0x41, 0x42 ,0x24},
{0x41, 0x43 ,0x03},
{0x41, 0x44 ,0x54},
/* frame control [0x4240 ~ 0x4243] */
{0x42, 0x40 ,0x02}, /* ISP FRAME CTRL0 Bit[7:0]: Debug mode */
/* format control [0x4300 ~ 0x4316] */
{0x43, 0x00 ,0xff}, /* CLIP MAX HI Bit[7:0]: clip_max[11:4] Data max high byte */
{0x43, 0x01 ,0x00}, /* CLIP MIN HI Bit[7:0]: clip_min[11:4] Data min high byte */
{0x43, 0x07 ,0x30}, /* EMBED CTRL Bit[7:4]: embed_line_st
                                  Bit[3]: embed_start_man
                                  Bit[2]: dpc_threshold_opt
                                    0: For white pixel
                                    1: For black pixel
                                  Bit[1]: embed_byte_order
                                  Bit[0]: embedded_en */
{0x43, 0x15 ,0x00}, /* REG15 Bit[7:0]: seof_vsync_delay[15:8] VSYNC trigger to VSYNC delay */
{0x43, 0x16 ,0x30}, /* REG16 Bit[7:0]: seof_vsync_delay[7:0] VSYNC trigger to VSYNC delay */
                    		   /* seof_vsync_delay 48 */
/* VFIFO control [0x4500 ~ 0x460F] */
{0x45, 0x00 ,0xa2}, /* NO description in register file */
{0x45, 0x01 ,0x14}, /* NO description in register file */
{0x45, 0x02 ,0x60}, /* NO description in register file */
/* frame HDR control [0x4580~0x4587] */
{0x45, 0x80 ,0x00}, /* HDR FRAME REG0 */
{0x45, 0x83 ,0x83}, /* HDR FRAME REG1 */
/* MIPI top control registers */
{0x48, 0x26 ,0x32}, /* HS PREPARE MIN Bit[7:0]: hs_prepare_min[7:0] Minimum value of hs_prepare, unit ns*/
{0x48, 0x37 ,0x10}, /* PCLK PERIOD Bit[7:0]: pclk_period[7:0] Period of pclk2x, pclk_div=1, and 1-bit decimal */
/* VFIFO  control registers */
{0x46, 0x02 ,0xf2}, /* R2 Bit[7:4]: Rm
                          	   Bit[3]: Test1
                               Bit[1]: Frame rst enable
                               If set to 1'b1, reset FIFO when sof_i comes
                               Bit[0]: RAM bypass enable
                               If set to 1'b1, SRAM is bypassed completely */
/* BLC control [0x4000 ~ 0x401E] */
{0x40, 0x01 ,0x06}, /* START LINE */
{0x40, 0x02 ,0x45}, /* BLC CTRL2 Bit[7]: format_change_en
                                         Format change trigger enable signal When set to 1, BLC will be applied to
                                         N frames in bit[5:0]
                                 Bit[6]: blc_auto_en , BLC auto mode enable signal
                                         0: black levels are set by registers (0x400C~0x401B)
                                         1: Black levels are calculated values
                                 Bit[5:0]: reset_frame_num
                                           Specify the frame number for reset and/or format change BLC trigge */
{0x40, 0x04 ,0x04}, /* LINE NUM */
{0x40, 0x05 ,0x40}, /* BLC CTRL5 Bit[1]: blc_always_up_en
                                         0: Normal freeze , 1: BLC always update */
{0x40, 0x09 ,0x80}, /* Black target level[7:0]  */
/* ISP general control [0x5000 ~ 0x500F, 0x5080 ~ 0x5091] */
{0x50, 0x00 ,0xff},
{0x50, 0x01 ,0x1f},
{0x50, 0x03 ,0x00},
{0x50, 0x0a ,0x85},
{0x50, 0x80 ,0x00},
{0x50, 0x91 ,0xa0},
/* NOT in register file */
{0x53, 0x00 ,0x05},
#if 0
/* enable output VSYNC */
{0x30, 0x04 ,0x80},
{0x43, 0x13 ,0x01},
{0x43, 0x11 ,0x20},
#endif
{0x01, 0x00 ,0x01} /* MODE_SELECT 0: software_standby , 1: Streaming */
};

static const UINT8 afe_720p_code[][3]={
/*stream off*/
{0x01, 0x00 ,0x00}, /* MODE_SELECT 0: software_standby , 1: Streaming */
/* general configuration and image size [0x0000 ~ 0x0387] */
{0x03, 0x4c ,0x05}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
{0x03, 0x4d ,0x10}, /* X OUTPUT SIZE Image output horizontal width[7:0] *//* 1296 */
{0x03, 0x4e ,0x03}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
{0x03, 0x4f ,0x2c}, /* Y OUTPUT SIZE Image output vertical height[7:0] *//* 812 */
/* timing control [0x3800 ~ 0x3832] */
{0x38, 0x11 ,0x08}, /* Hoffset */
{0x38, 0x13 ,0x02}, /* Voffset */
/*stream on*/
{0x01, 0x00 ,0x01} /* MODE_SELECT 0: software_standby , 1: Streaming */
};

static const UINT8 afe_fhd_code[][3]={
/*stream off*/
{0x01, 0x00 ,0x00}, /* MODE_SELECT 0: software_standby , 1: Streaming */
/* general configuration and image size [0x0000 ~ 0x0387] */
{0x03, 0x4c ,0x03}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
{0x03, 0x4d ,0xc0}, /* X OUTPUT SIZE Image output horizontal width[7:0] *//* 960 */
{0x03, 0x4e ,0x03}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
{0x03, 0x4f ,0x2c}, /* Y OUTPUT SIZE Image output vertical height[7:0] *//* 812 */
/* timing control [0x3800 ~ 0x3832] */
{0x38, 0x11 ,0xb0}, /* Hoffset */
{0x38, 0x13 ,0x02}, /* Voffset */
/*stream on*/
{0x01, 0x00 ,0x01} /* MODE_SELECT 0: software_standby , 1: Streaming */
};

/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg = 
{
	.mipi_nockrsten	=0 ,
	.wordcntsel 	  	=0 ,
	.Lvdsmode		=0 ,
	.Lvdslanenum	 	=1 , /* 1: 2lane, 0: 4lane.*/  
	.Lvdslanemode	=0 ,
	.Lvdslaneswap	 	=1 ,
	.Lvdsraw		 	=1 , /*0: raw10, 1: raw12*/
	.Lvdsoutsel 	 	=0 ,
	.Lvdseofsel 	 	=1 ,
	.Lvdseolsel 	 	=0 ,
	.vmode			=0 ,
	.lvdsvdsrc		=1 ,
	.lvds_dphy_sel	=1 ,
	.S2pphase		=4 ,
	.tsettle		 	=3 ,
	.teot			 	=7 ,
	.Ctrlword_sol	 	=0x2C , /*raw10: 0x2B, raw12: 0x2C*/
	.Ctrlword_sof	 	=0x00 ,
	.syncwrod0		=0xB8 ,
	.syncwrod1		=0 ,
	.syncwrod2		=0 ,
	.Exthd_num		=(EXTEND_LINES-1) ,
	.Exthd_pol		=0 ,
	.Exthd_mode 	 	=0 ,
	.cropenable 	 	=0,
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
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = _720P_HSIZE,
		.ySize = _720P_VSIZE,
	},
	.hsize = _720P_HSIZE+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = _720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = _720P_CFAPATTERN,
	.hratio = 1296,
	.vratio = 800,
	.fps_mhz = 59950000,
	.sensor_pclk = 80000000,
	.sensor_htotal = 1598,
	.pix_100khz = 800,
};

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
		.xOffset = 0,
		.yOffset = 0,
		.xSize = FHD_HSIZE,
		.ySize = FHD_VSIZE,
	},
	.hsize = FHD_HSIZE+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize = FHD_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = FHD_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = 59950000,
	.sensor_pclk = 80000000,
	.sensor_htotal = 1598,
	.pix_100khz = 800,
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
	&sensorTgPreview720PCfg,
	&sensorTgPreviewFHDCfg,
};

static const sensorTgSnapCfg_t sensorTgSnap720PCfg = {
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
		.psnapDummy = (sensorDoFunc_t []){
			sensorDoFuncSnapDummy0,	
		},
		.psnapReadoutField = (sensorDoFunc_t []) {
			sensorDoFuncSnapReadoutField0,
		},
		.snapFieldEnd = sensorDoFuncSnapFieldEnd,
		.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
		.snapNext = NULL,		
	},
	.crop = NULL, /****TODO***/
	.hsize = _720P_HSIZE,
	.vsize = _720P_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = _720P_CFAPATTERN,
	.n_dummy = SNAP_DUMMYVD_NUM,
	.n_readout = SNAP_READOUTVD_NUM,
	.n_capvd = SNAP_CAPTUREVD_NUM,
	.n_snap_early = SNAP_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
	.pix_100khz = 800,
};

static const sensorTgSnapCfg_t sensorTgSnapFHDCfg = {
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
		.psnapDummy = (sensorDoFunc_t []){
			sensorDoFuncSnapDummy0,	
		},
		.psnapReadoutField = (sensorDoFunc_t []) {
			sensorDoFuncSnapReadoutField0,
		},
		.snapFieldEnd = sensorDoFuncSnapFieldEnd,
		.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,
		.snapNext = NULL,		
	},
	.crop = NULL, /****TODO***/
	.hsize = FHD_HSIZE,
	.vsize = FHD_VSIZE,
	.sensor_model = SENSORMODEL,
	.cfa_pattern = FHD_CFAPATTERN,
	.n_dummy = SNAP_DUMMYVD_NUM,
	.n_readout = SNAP_READOUTVD_NUM,
	.n_capvd = SNAP_CAPTUREVD_NUM,
	.n_snap_early = SNAP_EARLY_NUM,
	.skip_snap_readout_vd = SKIP_SNAP_READOUTVD_EN,
	.fldsel_start = FLDSEL_START_NUM,
	.fldsel_end = FLDSEL_END_NUM,
	.pix_100khz = 800,
};

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnap720PCfg,
	&sensorTgSnapFHDCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview720PCfg = {
	.linetotal       = _720P_LINETOTAL,
	.lineblank       = _720P_LINEBLANK,
	.frametotal      = _720P_FRAMETOTAL,
	.frameblank      = _720P_FRAMEBLANK,
	.hreshape_fall   = _720P_HRESHAPE_FALL,
	.hreshape_rise   = _720P_HRESHAPE_RISE,
	.vreshape_fall   = _720P_VRESHAPE_FALL,
	.vreshape_rise   = _720P_VRESHAPE_RISE,
	.hoffset         = _720P_HOFFSET,
	.voffset         = _720P_VOFFSET,
	.hsize           = _720P_HSIZE+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = _720P_VSIZE,
	.lastlinepix     = _720P_HSIZE+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = _720P_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = _720P_HDNUM_FRAME_START,
	.hdnum_frame_end = _720P_HDNUM_FRAME_END,
	.hdnum_cmdport   = _720P_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFHDCfg = {
	.linetotal       = FHD_LINETOTAL,
	.lineblank       = FHD_LINEBLANK,
	.frametotal      = FHD_FRAMETOTAL,
	.frameblank      = FHD_FRAMEBLANK,
	.hreshape_fall   = FHD_HRESHAPE_FALL,
	.hreshape_rise   = FHD_HRESHAPE_RISE,
	.vreshape_fall   = FHD_VRESHAPE_FALL,
	.vreshape_rise   = FHD_VRESHAPE_RISE,
	.hoffset         = FHD_HOFFSET,
	.voffset         = FHD_VOFFSET,
	.hsize           = FHD_HSIZE+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.vsize           = FHD_VSIZE,
	.lastlinepix     = FHD_HSIZE+SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = FHD_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = FHD_HDNUM_FRAME_START,
	.hdnum_frame_end = FHD_HDNUM_FRAME_END,
	.hdnum_cmdport   = FHD_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigSnap720PCfg = {
	.linetotal       = _720P_LINETOTAL,
	.lineblank       = _720P_LINEBLANK,
	.frametotal      = _720P_FRAMETOTAL,
	.frameblank      = _720P_FRAMEBLANK,
	.hreshape_fall   = _720P_HRESHAPE_FALL,
	.hreshape_rise   = _720P_HRESHAPE_RISE,
	.vreshape_fall   = _720P_VRESHAPE_FALL,
	.vreshape_rise   = _720P_VRESHAPE_RISE,
	.hoffset         = _720P_HOFFSET,
	.voffset         = _720P_VOFFSET,
	.hsize           = _720P_HSIZE,
	.vsize           = _720P_VSIZE,
	.lastlinepix     = _720P_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = _720P_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = _720P_HDNUM_FRAME_START,
	.hdnum_frame_end = _720P_HDNUM_FRAME_END,
	.hdnum_cmdport   = _720P_HDNUM_CMDPORT,
};

static const sensorSyncSigCfg_t sensorSyncSigSnapFHDCfg = {
	.linetotal       = FHD_LINETOTAL,
	.lineblank       = FHD_LINEBLANK,
	.frametotal      = FHD_FRAMETOTAL,
	.frameblank      = FHD_FRAMEBLANK,
	.hreshape_fall   = FHD_HRESHAPE_FALL,
	.hreshape_rise   = FHD_HRESHAPE_RISE,
	.vreshape_fall   = FHD_VRESHAPE_FALL,
	.vreshape_rise   = FHD_VRESHAPE_RISE,
	.hoffset         = FHD_HOFFSET,
	.voffset         = FHD_VOFFSET,
	.hsize           = FHD_HSIZE,
	.vsize           = FHD_VSIZE,
	.lastlinepix     = FHD_HSIZE,
	.hdvdopos        = 0,
	.conf_totalsync  = 0,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.conf_hdpclkedge = 0,
	.conf_hvldbypass = 0,
	.conf_vvldbypass = 0,
	.frameblank_base = 0,
	.vreshape_base   = FHD_VRESHAPE_COUNTER_SEL,
	.conf_yuvhdpol	 = 0, /*yuvhd just before hdres 0:active low*/
	.conf_yuvvdpol	 = 0, /*yuvvd just before vdres 0:active low*/
	.hdnum_frame_start = FHD_HDNUM_FRAME_START,
	.hdnum_frame_end = FHD_HDNUM_FRAME_END,
	.hdnum_cmdport   = FHD_HDNUM_CMDPORT,
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
	.defSyncSigCfg  = &sensorSyncSigPreview720PCfg,
	.verStr = MODEL_VERSION,
};

UINT8 OVsensorRead(UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[3];
	/*UINT8 readval=0;*/
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	sensorSerialBlockRead(cmdportid,1,regread,0);
	printf("\nREAD [%x] = [%x]\n",addr,regread[2]);
	return regread[2];
}

void sensorTg_ov9714_Install()
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
	profLogPrintf(3,"rawmux output size    = (%d,%d)",READ16(0xb0002090),READ16(0xb0002092));
}
	
static void sensorDoFuncInit()
{
	PROF_LOG_PRINT("________Sensor Driver MODEL_VERSION <<%s>>", sensorTgDev.verStr);
	PROF_LOG_PRINT("________Sensor Driver Build Time    <<%s>>", BUILD_TIME);
    	PROF_LOG_ADD(3, "sensorDoFuncInit()___enter");

	sensorTgPllClkExtSet(24000000);/*24MHz*/                                                

	halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,300000); /* 300MHz or 288MHz*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9*/
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/

	/* Master clock and Pixel clock configuration here */                                       
	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);                                
                                             
#if defined(HW_EVB_SPE7001) /*EVB case  */
	/* Sensor Power On Sequence */
	sp5kGpioCfgSet(SENOR_GPIO_GRP,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);

    /* OV9713 PWRDN low active !! */
    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,0);            /* PWD Low   */
    /* OV9713 RESET low active !! */
    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0);            /* RESET Low */
	tmrUsWait(10000);

    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High   */
	/* tmrUsWait(50000); */ /* t0 + t1 + t2 min = 0 */
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High  */
	tmrUsWait(25000); /* t3 min = 8192 XVCLK (341 us for 24MHz) + 1ms for SCCB stable */
#endif

   	OVsensorRead(0x300b); /*sensor ID */

       sensorSerialBulkWrite(cmdportid_tg,(sizeof(afe_init_code_SWReset)/sizeof(afe_init_code_SWReset[0])), afe_init_code_SWReset[0],0);	
	tmrUsWait(10000);
	sensorSerialBulkWrite(cmdportid_tg,(sizeof(afe_init_code)/sizeof(afe_init_code[0])), afe_init_code[0],0);

	sensorLVDSCfgSet(&sensorLVDSCfg);
	/*Set re-shape manual mode .. before sensorSyncSigInit*/
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

	tmrUsWait(10000);
	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

#if MODE_SYNC_EARLY /*config pre command IRQ */
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif

	PROF_LOG_ADD(3, "sensorDoFuncInit()___done");
}

static void sensorDoFuncStop()
{
}

static void sensorDoFuncPreview720PBegin()
{
	/*SSG short*/
	/*SSG windows P*/
	/*TG init*/
	/*CDSP preview prepare*/
	/*statistics prepare*/
	/*front_preview_short_exp_frame_set*/
	/*serial port has fw sync mechanism, do it here instead of after vvalid*/
	sensorEventVvalidWait(1);
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_720p_code)/sizeof(afe_720p_code[0]), afe_720p_code[0],0);
	sensorSyncSigInit(&sensorSyncSigPreview720PCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	PROF_LOG_ADD(3, "mipi 720P cmd done");
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
	sensorSerialBulkWrite(cmdportid_tg, sizeof(afe_fhd_code)/sizeof(afe_fhd_code[0]), afe_fhd_code[0],0);
	sensorSyncSigInit(&sensorSyncSigPreviewFHDCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	PROF_LOG_ADD(3, "mipi FHD cmd done");
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
	UINT32 smode=sensorModeGet();

	if (smode==0x40)
		sensorSyncSigInit(&sensorSyncSigSnap720PCfg);
	else
		sensorSyncSigInit(&sensorSyncSigSnapFHDCfg);
	sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
	PROF_LOG_ADD(3, "SnapExpBegin");
}

static void sensorDoFuncSnapEarly0()
{
	PROF_LOG_ADD(3, "SnapEarly0");
}

static void sensorDoFuncSnapDummy0()
{
	PROF_LOG_ADD(3, "SnapDummy0");
}

static void sensorDoFuncSnapBshutterBegin()
{
	PROF_LOG_ADD(3, "SnapBshutterBegin");
}

static void sensorDoFuncSnapBshutterFollow()
{
	PROF_LOG_ADD(3, "SnapBshutterFollow");
}

static void sensorDoFuncSnapBshutterEnd()
{
	PROF_LOG_ADD(3, "SnapBshutterEnd");
}

static void sensorDoFuncSnapShortExp()
{
	PROF_LOG_ADD(3, "SnapShortExp");
}

static void sensorDoFuncSnapExpEnd()
{
	PROF_LOG_ADD(3, "SnapExpEnd");
}

static void sensorDoFuncSnapReadoutField0()
{
	PROF_LOG_ADD(3, "SnapReadoutField0");
}

static void sensorDoFuncSnapFieldEnd()
{
	PROF_LOG_ADD(3, "SnapFieldEnd");
}

static void sensorDoFuncSnapReadoutEnd()
{
	PROF_LOG_ADD(3, "SnapReadoutEnd");
	sensorReportsize();
}

/*static void sensorDoFuncSnapNext(UINT32 mode)
{
}*/

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
	cmdportid_tg =
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
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	sensorLVDSEnSet(0);
	/*Disable mipi interface*/
}

static void sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
#if !MASK_EXP_CMD
      sensorSerialAsyncWrite(cmdportid_tg, nreg, pdata);
#endif
}

static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
	/*PROF_LOG_PRINTF(3,"sensor_cmdport_exp_vtbl_do\n");*/
}

#if SP5K_SENSOR_OV9714
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regData[3][3] ={
		{0x35,0x00,0x00},
		{0x35,0x01,0x00},
		{0x35,0x02,0x00},
	};
	regData[0][2] = (expLine>>12)&0xf;
	regData[1][2] = (expLine>>4)&0xff;
	regData[2][2] = (expLine&0xf)<<4;
	
	profLogPrintf(3,"expLine(reg0) = %d [0x%x%x 0x%x]",expLine,regData[0][0],regData[0][1],regData[0][2]);
	profLogPrintf(3,"expLine(reg1) = %d [0x%x%x 0x%x]",expLine,regData[1][0],regData[1][1],regData[1][2]);
	profLogPrintf(3,"expLine(reg2) = %d [0x%x%x 0x%x]",expLine,regData[2][0],regData[2][1],regData[2][2]);
	sensorSerialAsyncWrite(cmdportid_tg,3, regData[0]);
	sensorSerialFlush(cmdportid_tg);
	sensorEventNewFrameWait(1);
}
#endif
