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
 *  01.01.01, 20130616 Chris                                              *
 *            Data Rate: 640Mbps/lane                                     *
 *            sensor mode:                                                *
 *              0x30: 1296x800@60fps                                      *
 *              0x31:  640x392@120fps                                     *
 *              0x32:  320x192@240fps                                     *
 *              0x40: 1296x800@60fps                                      *
 *                                                                        *
 *            Note:                                                       *
 *              1. I2C ID control by the SID pin                          *
 *                   1: 0x6C                                              *
 *                   0: 0x20                                              *
 *              2.MIPI 2 lane 12bit                                       *
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_ov9713-01.01.01"
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

#define SENSOR_PROF_LOG_EN   (1)
#define PROF_ID              (3)

#if SENSOR_PROF_LOG_EN
    #define _SENSOR_DBG_ 1
    #define SENSOR_PROF_LOG_ADD(str)                profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)      profLogPrintf(PROF_ID, fmt, ##arg)

    #define WRITELINE (*((volatile UINT16 *)(0xb0009064))  = (1))
    #define READLINE  (*((volatile UINT16 *)(0xb0009064)))

    #define PROF_LOG_LINE_COUNT(str,args...)\
    do{\
        WRITELINE;\
        profLogPrintf(0,"[OV9713] %s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(1,"[OV9713] %s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(2,"[OV9713] %s_"str" @ %d",__func__, ##args, READLINE);\
        profLogPrintf(3,"[OV9713] %s_"str" @ %d",__func__, ##args, READLINE);\
    }while(0)

    static UINT32 snap_time_log[7];
    #define TIME_TAG(id)   (snap_time_log[id])
    #define TIME_LOG(id)    snap_time_log[id]=tmrTimeClockGet()
    #define TIME_DIFF(a,b) (snap_time_log[a]-snap_time_log[b])

#else
    #define _SENSOR_DBG_ 0
    #define SENSOR_PROF_LOG_ADD(str)
    #define SENSOR_PROF_LOG_PRINT(fmt,arg...)
    #define PROF_LOG_LINE_COUNT(str,args...)
    #define TIME_TAG(id)
    #define TIME_LOG(id)
    #define TIME_DIFF(a,b)
#endif

#if(_SENSOR_DBG_)
    /*debug by TGGPIO2*/
    #define DBG_GPIO(x) if(x){WRITE8(0xb0009032, (READ8(0xb0009032)|(0x02)));}else{WRITE8(0xb0009032, (READ8(0xb0009032)&(~0x02)));}
#else
    #define DBG_GPIO(x)
#endif


#define SUSPEND_ENABLE             (0)
#define STREAMOF_ENABLE            (0)
#define LVDS_HCROP_ENABLE          (1)
#define MODE_SYNC_EARLY            (50)
#if defined(HW_EVB_SPE7001)
#define MIPI_RAW12                 (1)
#else
#define MIPI_RAW12                 (1)
#endif
#define BY_PIXEL                   (1)
#define BY_HD                      (0)
#define DATA_2LANE                 (1)    /* 1: 2lane, 0: 4lane. */
#define EVB                        (1)
#define SP5K_SET_LVDS_HCROP(a)     sensorSyncSigSnapExtSet(SENSOR_SNAP_EXT_HOFFSET2,a,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF)

static UINT32 g_IsSnap2Pv;
/*static UINT8  is_aborted;*/
static BOOL   fpvsusp;
#if SUSPEND_ENABLE
static UINT32 sensorSuspended;
#endif

#define SENSOR_FREX          (1<<15)
#define SENSOR_PWRDN         (1<<14)
#define SENSOR_RESET         (1<<12)
#define SENSOR_STROBE        (1<<13)
#define OUT_SENSOR_GPIO_MASK     SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define CFG_SENSOR_GPIO_DIR      SENSOR_FREX|SENSOR_PWRDN|SENSOR_RESET
#define SENOR_GPIO_GRP           SP5K_GPIO_GRP_LMI

/*****************************************************************************************/

#define SENSOR_MCLK      (  80000000)

#if SPCA6330 /* 6330 */
#define PIX_100KHZ       ( 80000)
#define SPCA_PCLK1X      ( 80000)
#else
#define PIX_100KHZ       ( 80000)
#define SPCA_PCLK1X      ( 80000)
#endif

#define MCLK2MCLK(clk)     (clk*SPCA_PCLK1X*100/SENSOR_MCLK)

#define PREVIEW_720P_HOUTPUT        (1296)
#define PREVIEW_720P_VOUTPUT        ( 800) /* 802 */
#define PREVIEW_VGA_HOUTPUT         ( 640)
#define PREVIEW_VGA_VOUTPUT         ( 400) /* 392 */
#define PREVIEW_QVGA_HOUTPUT        ( 320)
#define PREVIEW_QVGA_VOUTPUT        ( 200) /* 192 */

#define PREVIEW_EXTEND_LINES        (13)

#define PREVIEW_720P_HSIZE          (PREVIEW_720P_HOUTPUT)
#define PREVIEW_720P_VSIZE          (PREVIEW_720P_VOUTPUT)
#define PREVIEW_VGA_HSIZE           (PREVIEW_VGA_HOUTPUT)
#define PREVIEW_VGA_VSIZE           (PREVIEW_VGA_VOUTPUT)
#define PREVIEW_QVGA_HSIZE          (PREVIEW_QVGA_HOUTPUT)
#define PREVIEW_QVGA_VSIZE          (PREVIEW_QVGA_VOUTPUT)
#define CAPTURE_HSIZE               (PREVIEW_720P_HSIZE)
#define CAPTURE_VSIZE               (PREVIEW_720P_VSIZE)
/*****************************************************************************************/
#define PREVIEW_720P_LINETOTAL                   (  1598) /* uint : pclk1x */
#define PREVIEW_720P_LINEBLANK                   (0x0006) /* basefw No refrence , fixed 6 now */
#define PREVIEW_720P_FRAMETOTAL                  (PREVIEW_720P_VSIZE+1)
#define PREVIEW_720P_FRAMEBLANK                  (PREVIEW_720P_VSIZE+1)
#define PREVIEW_720P_HOFFSET                     (  300) /* uint : pclk1x */
#define PREVIEW_720P_HCROP_OFFSET                (    0) /* lvds crop offset (4N-1) uint : pixel */
#define PREVIEW_720P_VOFFSET                     (PREVIEW_EXTEND_LINES+2)
#define PREVIEW_720P_HRESHAPE_FALL               (   100) /* uint : pclk1x */
#define PREVIEW_720P_HRESHAPE_RISE               (   200) /* uint : pclk1x */
#define PREVIEW_720P_VRESHAPE_COUNTER_SEL        (BY_HD)
#define PREVIEW_720P_VRESHAPE_FALL               (PREVIEW_720P_VOFFSET-2)
#define PREVIEW_720P_VRESHAPE_RISE               (PREVIEW_720P_VOFFSET-1)

#define PREVIEW_VGA_LINETOTAL                    (  1600)
#define PREVIEW_VGA_LINEBLANK                    (0x0006)
#define PREVIEW_VGA_FRAMETOTAL                   (PREVIEW_VGA_VSIZE+1)
#define PREVIEW_VGA_FRAMEBLANK                   (PREVIEW_VGA_VSIZE+1)
#define PREVIEW_VGA_HOFFSET                      (   900)
#define PREVIEW_VGA_HCROP_OFFSET                 (     0)
#define PREVIEW_VGA_VOFFSET                      (PREVIEW_EXTEND_LINES+2)
#define PREVIEW_VGA_HRESHAPE_FALL                (   700)
#define PREVIEW_VGA_HRESHAPE_RISE                (   800)
#define PREVIEW_VGA_VRESHAPE_COUNTER_SEL         (BY_HD)
#define PREVIEW_VGA_VRESHAPE_FALL                (PREVIEW_EXTEND_LINES-2)
#define PREVIEW_VGA_VRESHAPE_RISE                (PREVIEW_EXTEND_LINES-1)

#define PREVIEW_QVGA_LINETOTAL                   (  1570)
#define PREVIEW_QVGA_LINEBLANK                   (0x0006)
#define PREVIEW_QVGA_FRAMETOTAL                  (PREVIEW_QVGA_VSIZE+1)
#define PREVIEW_QVGA_FRAMEBLANK                  (PREVIEW_QVGA_VSIZE+1)
#define PREVIEW_QVGA_HOFFSET                     (  1200)
#define PREVIEW_QVGA_HCROP_OFFSET                (     0)
#define PREVIEW_QVGA_VOFFSET                     (PREVIEW_EXTEND_LINES+2)
#define PREVIEW_QVGA_HRESHAPE_FALL               (  1000)
#define PREVIEW_QVGA_HRESHAPE_RISE               (  1100)
#define PREVIEW_QVGA_VRESHAPE_COUNTER_SEL        (BY_HD)
#define PREVIEW_QVGA_VRESHAPE_FALL               (PREVIEW_QVGA_VOFFSET-2)
#define PREVIEW_QVGA_VRESHAPE_RISE               (PREVIEW_QVGA_VOFFSET-1)

#define CAPTURE_LINETOTAL                        (PREVIEW_720P_LINETOTAL)
#define CAPTURE_LINEBLANK                        (PREVIEW_720P_LINEBLANK)
#define CAPTURE_FRAMETOTAL                       (PREVIEW_720P_FRAMETOTAL)
#define CAPTURE_FRAMEBLANK                       (PREVIEW_720P_FRAMEBLANK)
#define CAPTURE_HOFFSET                          (PREVIEW_720P_HOFFSET)
#define CAPTURE_HCROP_OFFSET                     (PREVIEW_720P_HCROP_OFFSET)
#define CAPTURE_VOFFSET                          (PREVIEW_720P_VOFFSET)
#define CAPTURE_HRESHAPE_FALL                    (PREVIEW_720P_HRESHAPE_FALL)
#define CAPTURE_HRESHAPE_RISE                    (PREVIEW_720P_HRESHAPE_RISE)
#define CAPTURE_VRESHAPE_COUNTER_SEL             (BY_HD)
#define CAPTURE_VRESHAPE_FALL                    (PREVIEW_720P_VRESHAPE_FALL)
#define CAPTURE_VRESHAPE_RISE                    (PREVIEW_720P_VRESHAPE_RISE)



#define	PREVIEW_HDNUM_FRAME_START                 (PREVIEW_720P_VOFFSET+1)  /* unit : line */
#define	PREVIEW_720P_HDNUM_FRAME_END              (2)   /* unit : line */
#define	PREVIEW_720P_HDNUM_CMDPORT                (100) /* unit : line */

#define	PREVIEW_VGA_HDNUM_FRAME_END               (2)
#define	PREVIEW_VGA_HDNUM_CMDPORT                 (100)

#define	PREVIEW_QVGA_HDNUM_FRAME_END              (2)
#define	PREVIEW_QVGA_HDNUM_CMDPORT                (100)

#define	CAPTURE_HDNUM_FRAME_START                 (PREVIEW_720P_VOFFSET+1)
#define	CAPTURE_HDNUM_FRAME_END                   (2)
#define	CAPTURE_HDNUM_CMDPORT                     (100)

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
														  /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_720P_CFAPATTERN       BAYER_BGGR
#define PREVIEW_VGA_CFAPATTERN        BAYER_BGGR
#define PREVIEW_QVGA_CFAPATTERN       BAYER_BGGR

#define	SNAP_CFAPATTERN		          BAYER_BGGR
#define	SNAP_DUMMYVD_NUM	          1             /*snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM		      1             /*snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM		      1             /*for typical CCD==n_dummy + n_readout + 1*/
#define	SNAP_EARLY_NUM		          0	            /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	SNAP_FRAME_EARLY_NUM		  2             /*for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	          0		        /*It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM			 	  0             /*It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN        0            	/*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	SKIP_SNAP_FRAME_READOUTVD_EN  2	            /*a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	          0		        /*sony FALSE;panasonic/ITG TRUE*/
#define	EXP_FRAME_NUM       	      2	            /*CMOS/CCD typically 2,the exposure VD add the readout start VD after the trigger*/
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

/* OV9713 the SCCB ID is controlled by the SID pin
   SID High: 0x6C, Low: 0x20                       */

#define	TG_SPI_I2C_MODE  0x20     	                                         /*I2C slave addr for write*/
#define	TG_FORMAT2  	 FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF  /*don't change*/
#define	TG_FORMAT3  	 0		                                             /*don't change*/
#define	TG_SPEED         FRONT_SSC_CLK_192K
 					                              /*FRONT_SSC_CLK_24K,FRONT_SSC_CLK_48K,FRONT_SSC_CLK_96K,FRONT_SSC_CLK_192K,
						                            FRONT_SSC_CLK_384K,FRONT_SSC_CLK_768K,FRONT_SSC_CLK_1500K,FRONT_SSC_CLK_3M*/

#define	TG_NBIT_REGADDR  16		                  /*ssc/spi register address numbers */
#define	TG_NBIT_REGDATA  8		                  /*ssc/spi register data numbers */

void sensor_frame_cb_reg(UINT32 svcid, void *f, UINT32 param);

/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
#if STREAMOF_ENABLE
static void sensorDoFuncStreamOff(UINT32 abort);
static void sensorDoFuncStreamOn (UINT32 abort);
#endif
/*static void sensorDoFuncOVStreamOff();
static void sensorDoFuncOVStreamOn();*/
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreview720pBegin();
static void sensorDoFuncPreviewVGABegin();
static void sensorDoFuncPreviewQVGABegin();

static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapDummy0();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
#if 0
static void sensorDoFuncSnapEarly0();
static void sensorDoFuncSnapEarly1();
#endif
static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();
static void sensorDoFuncSnapNext(UINT32 next_mode);
static UINT32 sensorResumeExecuteLoad();
static UINT32 sensorSuspendExecuteLoad();

extern UINT32 sensorModeGet();
extern UINT32 tmrTimeClockGet();
extern UINT32 halPllFrontClkSet(UINT32 pclkSel,UINT32 mclk);
/*-----------------------------------------------------------------------------------------------*/
static const UINT8 cmd_stream_off[][3]SENSOR_SIF1DMA_BUF={
    {0x01, 0x00 ,0x00}, /* 0x0100 MODE_SELECT 0x00 RW Bit[7:1]: Not used
                                                      Bit[0]:   Mode select
                                                                0: software_standby
                                                                1: Streaming    */
};

static const UINT8 cmd_stream_on[][3]SENSOR_SIF1DMA_BUF={
    {0x01, 0x00 ,0x01},
};

static const UINT8 cmd_sw_rst[][3]SENSOR_SIF1DMA_BUF={
    {0x01, 0x03 ,0x01}, /* 0x0103 SOFTWARE RESET 0x00 RW Bit[7:1]: Not used
                                                         Bit[0]:   software_reset
                                                                   1: reset    */
};

static const UINT8 cmd_init_code[][3]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; Mipi : 2 lane
    ; Mipi data rate: 640Mbps/Lane
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->60
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/


    {0x01, 0x00 ,0x00},     /* standby */
    {0x01, 0x03 ,0x01},     /* sw rst  */
    {0x01, 0x00 ,0x00},
    {0x01, 0x00 ,0x00},
    {0x01, 0x00 ,0x00},
    {0x01, 0x00 ,0x00},
    {0x01, 0x01 ,0x00},     /* IMAGE ORIENTATION  Bit[1]: v_flip
                                                  Bit[0]: h_mirror */
    {0x30, 0x01 ,0x01},     /* PAD CTRL Bit[4]:   strobe_in NOR opt
                                        Bit[1:0]: I/O strength */
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
    {0x03, 0x41 ,0x46}, /* FRAME LENGTH LINES Total vertical size[7:0] */
                        /* 832 + 6 */
    {0x03, 0x42 ,0x06}, /* LINE LENGTH PCK Total horizontal size[15:8] */
    {0x03, 0x43 ,0x3e}, /* LINE LENGTH PCK Total horizontal size[7:0] */
                        /* 1598 */
    {0x03, 0x44 ,0x00}, /* X ADDR START x_addr_start[11:8] */
    {0x03, 0x45 ,0x00}, /* X ADDR START x_addr_start[7:0] */
                        /* 0 */
    {0x03, 0x46 ,0x00}, /* Y ADDR START y_addr_start[11:8] */
    {0x03, 0x47 ,0x00}, /* Y ADDR START y_addr_start[7:0] */
                        /* 0 */
    {0x03, 0x48 ,0x05}, /* X ADDR END x_addr_end_int horizontal width[11:8] */
    {0x03, 0x49 ,0x1f}, /* X ADDR END x_addr_end_int horizontal width[7:0] */
                        /* 1311 */
    {0x03, 0x4a ,0x03}, /* Y ADDR END y_addr_end_int vertical height[11:8] */
    {0x03, 0x4b ,0x2f}, /* Y ADDR END y_addr_end_int vertical height[7:0] */
                        /* 815 */
    {0x03, 0x4c ,0x05}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
    {0x03, 0x4d ,0x10}, /* X OUTPUT SIZE Image output horizontal width[7:0] */
                        /* 1296 */
    {0x03, 0x4e ,0x03}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
    {0x03, 0x4f ,0x22}, /* Y OUTPUT SIZE Image output vertical height[7:0] */
                        /* 812 -> 802 */
    {0x03, 0x81 ,0x01}, /* X EVEN INC Bit[3:0]: h_even_inc
                                                Horizontal subsample even increase numbe */
    {0x03, 0x83 ,0x01}, /* X ODD INC Bit[3:0]: h_odd_inc
                                               Horizontal subsample odd increase number */
    {0x03, 0x85 ,0x01}, /* Y EVEN INC Bit[3:0]: v_even_inc
                                                Vertical subsample even increase number*/
    {0x03, 0x87 ,0x01}, /* Y ODD INC Bit[3:0]: v_odd_inc
                                               Vertical subsample odd increase number*/
    {0x30, 0x02 ,0x09}, /* PAD OEN2 Bit[5]: io_sid_oen
                                    Bit[3]: io_strobe_oen
                                    Bit[2]: io_sda_oen
                                    Bit[0]: io_gpio_oen */
    {0x30, 0x12 ,0x20}, /* A MIPI PK Bit[7:6]: pgm_hspredrv_iset
                                               Bias current control for LVDS
                                               Bit[5:4]: Lph
                                               Bit[3]: phy_mode
                                                       0: MIPI , 1: LVDS
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
    {0x30, 0xb5 ,0x04}, /* PLL1 OP PIX CLK DIV Bit[2:0]: pll1_op_pix_clk_div_r[2:0] 0x03: 16 , 0x04: 18 , 0x05: 110 , 0x10: 112 */
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
    {0x35, 0x01 ,0x20},
    {0x35, 0x02 ,0x00},
    {0x35, 0x03 ,0x0F}, /* change 0x3503[3]=1 , org is 0x07 */
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
    {0x38, 0x11 ,0x08}, /* NO description in register file */
    {0x38, 0x13 ,0x02}, /* NO description in register file */
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

#if 0
    {0x43, 0x11 ,0xF0},
    {0x43, 0x12 ,0x00},
    {0x43, 0x13 ,0x02},
                        /* 0x4311 REG11 0x04 RW Bit[7:0]: hsyvsy_neg_width[15:8]
                                         VSYNC width (in term of pixel numbers)
                           0x4312 REG12 0x00 RW Bit[7:0]: hsyvsy_neg_width[7:0]
                                                          VSYNC width (in term of pixel numbers)
                           0x4313 REG13 0x00 RW
                                                Bit[4]: VSYNC polarity
                                                Bit[3:2]: VSYNC output select
                                                Bit[1]: VSYNC mode 3
                                                Bit[0]: VSYNC mode 2 */

                        /* VSYNC mode 2 , and hsyvsy_neg_width = 1024 pixel */
    {0x43, 0x14 ,0x0F}, /* REG14 0x00 RW Bit[7:0]: seof_vsync_delay[23:16] VSYNC trigger to VSYNC delay */
#endif
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
    /* NOT in register file */
    {0x41, 0x42 ,0x24},

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
    /* ISP general control [0x5000 ~ 0x500F, 0x5080 ~ 0x5091] */
    {0x50, 0x00 ,0xff},
    {0x50, 0x01 ,0x1f},
    {0x50, 0x03 ,0x00},
    {0x50, 0x0a ,0x85},
    {0x50, 0x80 ,0x00},
    {0x50, 0x91 ,0xa0},

    /* NOT in register file */
    {0x53, 0x00 ,0x05},


    {0x01, 0x00 ,0x01}, /* MODE_SELECT 0: software_standby , 1: Streaming */

    /* manual AEC/AGC [0x3500 ~ 0x3A07, 0x5900 ~ 0x5910] */
    {0x35, 0x01 ,0x33}, /* EXPO0 Bit[7:0]: Expo0[15:8] Exposure in normal mode, the long exposure in HDR mode or the exposure
                                                       of first frame in frame exposure mode*/
    {0x35, 0x02 ,0xc0}, /* EXPO0 Bit[7:0]: Expo0[7:0] 8 LSB of exposure which is the  exposure in normal mode, the long
                                                      exposure in HDR mode or the exposer of first frame in frame exposure mode.
                                 Bit[3:0] is for fractional exposure, which is smaller than 0x0B.*/
    {0x3a, 0x02 ,0x40}, /* AEC CTRL02 AEC stable range high limit */
    {0x3a, 0x03 ,0x30}, /* AEC CTRL03 Bit[7:0]: AEC stable range low limit */
    {0x3a, 0x04 ,0x03}, /* AEC CTRL04 Maximum exposure[15:8] */
    {0x3a, 0x05 ,0x3c}, /* AEC CTRL05 Bit[7:0]: Maximum exposure[7:0] */

#if 0
    {0x50, 0x00 ,0xFD}, /* Default lenc is enable, 0x5000[1] set to 0 can disable */
#endif

#if 0
    {0x40, 0x09 ,0x80}, /* Black target level[7:0]  */
#endif

#if 1
    /* enable output VSYNC */
    {0x30, 0x04 ,0x80},
#endif

};


static const UINT8 cmd_still_code[][3]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; Mipi : 2 lane
    ; Mipi data rate: 640Mbps/Lane
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->60
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/

    /* general configuration and image size [0x0000 ~ 0x0387] */
#if 0
    {0x03, 0x40 ,0x03}, /* FRAME LENGTH LINES Total vertical size[15:8] */
    {0x03, 0x41 ,0x46}, /* FRAME LENGTH LINES Total vertical size[7:0] */
                        /* 832 + 6 */
#endif
    {0x03, 0x42 ,0x06}, /* LINE LENGTH PCK Total horizontal size[15:8] */
    {0x03, 0x43 ,0x3e}, /* LINE LENGTH PCK Total horizontal size[7:0] */
                        /* 1598 */

    {0x03, 0x4c ,0x05}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
    {0x03, 0x4d ,0x10}, /* X OUTPUT SIZE Image output horizontal width[7:0] */
                        /* 1296 */
    {0x03, 0x4e ,0x03}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
    {0x03, 0x4f ,0x22}, /* Y OUTPUT SIZE Image output vertical height[7:0] */
                        /* 812 -> 802 */
    {0x03, 0x81 ,0x01}, /* X EVEN INC Bit[3:0]: h_even_inc
                                                Horizontal subsample even increase numbe */
    {0x03, 0x83 ,0x01}, /* X ODD INC Bit[3:0]: h_odd_inc
                                               Horizontal subsample odd increase number */
    {0x03, 0x85 ,0x01}, /* Y EVEN INC Bit[3:0]: v_even_inc
                                                Vertical subsample even increase number*/
    {0x03, 0x87 ,0x01}, /* Y ODD INC Bit[3:0]: v_odd_inc
                                               Vertical subsample odd increase number*/

    /* sensor control [0x3700 ~ 0x374B] */
    {0x37, 0x0e ,0x80},
    {0x37, 0x0a ,0x31},

    /* timing control [0x3800 ~ 0x3832] */
    {0x38, 0x11 ,0x08}, /* NO description in register file */
    {0x38, 0x13 ,0x02}, /* NO description in register file */

    {0x38, 0x21 ,0x00},
    /* NOT in register file */
    {0x41, 0x44 ,0x54},

    {0x45, 0x02 ,0x60}, /* NO description in register file */

    /* NOT in register file */
    {0x41, 0x42 ,0x24},
    /* BLC control [0x4000 ~ 0x401E] */
    {0x40, 0x01 ,0x06}, /* START LINE */
    {0x40, 0x04 ,0x04}, /* LINE NUM */
#if 0
    /* manual AEC/AGC [0x3500 ~ 0x3A07, 0x5900 ~ 0x5910] */
    {0x35, 0x01 ,0x33}, /* EXPO0 Bit[7:0]: Expo0[15:8] Exposure in normal mode, the long exposure in HDR mode or the exposure
                                                       of first frame in frame exposure mode*/
    {0x35, 0x02 ,0xc0}, /* EXPO0 Bit[7:0]: Expo0[7:0] 8 LSB of exposure which is the  exposure in normal mode, the long
                                                      exposure in HDR mode or the exposer of first frame in frame exposure mode.
                                 Bit[3:0] is for fractional exposure, which is smaller than 0x0B.*/
#endif
    {0x3a, 0x04 ,0x03}, /* AEC CTRL04 Maximum exposure[15:8] */
    {0x3a, 0x05 ,0x3c}, /* AEC CTRL05 Bit[7:0]: Maximum exposure[7:0] */

};

static const UINT8 cmd_720p_code[][3]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; Mipi : 2 lane
    ; Mipi data rate: 640Mbps/Lane
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->60
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/
    /* general configuration and image size [0x0000 ~ 0x0387] */
    {0x03, 0x40 ,0x03}, /* FRAME LENGTH LINES Total vertical size[15:8] */
    {0x03, 0x41 ,0x46}, /* FRAME LENGTH LINES Total vertical size[7:0] */
                        /* 832 + 6 */
    {0x03, 0x42 ,0x06}, /* LINE LENGTH PCK Total horizontal size[15:8] */
    {0x03, 0x43 ,0x3e}, /* LINE LENGTH PCK Total horizontal size[7:0] */
                        /* 1598 */

    {0x03, 0x4c ,0x05}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
    {0x03, 0x4d ,0x10}, /* X OUTPUT SIZE Image output horizontal width[7:0] */
                        /* 1296 */
    {0x03, 0x4e ,0x03}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
    {0x03, 0x4f ,0x22}, /* Y OUTPUT SIZE Image output vertical height[7:0] */
                        /* 812 -> 802 */
    {0x03, 0x81 ,0x01}, /* X EVEN INC Bit[3:0]: h_even_inc
                                                Horizontal subsample even increase numbe */
    {0x03, 0x83 ,0x01}, /* X ODD INC Bit[3:0]: h_odd_inc
                                               Horizontal subsample odd increase number */
    {0x03, 0x85 ,0x01}, /* Y EVEN INC Bit[3:0]: v_even_inc
                                                Vertical subsample even increase number*/
    {0x03, 0x87 ,0x01}, /* Y ODD INC Bit[3:0]: v_odd_inc
                                               Vertical subsample odd increase number*/

    /* sensor control [0x3700 ~ 0x374B] */
    {0x37, 0x0e ,0x80},
    {0x37, 0x0a ,0x31},

    /* timing control [0x3800 ~ 0x3832] */
    {0x38, 0x11 ,0x08}, /* NO description in register file */
    {0x38, 0x13 ,0x02}, /* NO description in register file */

    {0x38, 0x21 ,0x00},
    /* NOT in register file */
    {0x41, 0x44 ,0x54},

    {0x45, 0x02 ,0x60}, /* NO description in register file */

    /* NOT in register file */
    {0x41, 0x42 ,0x24},
    /* BLC control [0x4000 ~ 0x401E] */
    {0x40, 0x01 ,0x06}, /* START LINE */
    {0x40, 0x04 ,0x04}, /* LINE NUM */

    /* manual AEC/AGC [0x3500 ~ 0x3A07, 0x5900 ~ 0x5910] */
    {0x35, 0x01 ,0x33}, /* EXPO0 Bit[7:0]: Expo0[15:8] Exposure in normal mode, the long exposure in HDR mode or the exposure
                                                       of first frame in frame exposure mode*/
    {0x35, 0x02 ,0xc0}, /* EXPO0 Bit[7:0]: Expo0[7:0] 8 LSB of exposure which is the  exposure in normal mode, the long
                                                      exposure in HDR mode or the exposer of first frame in frame exposure mode.
                                 Bit[3:0] is for fractional exposure, which is smaller than 0x0B.*/
    {0x3a, 0x04 ,0x03}, /* AEC CTRL04 Maximum exposure[15:8] */
    {0x3a, 0x05 ,0x3c}, /* AEC CTRL05 Bit[7:0]: Maximum exposure[7:0] */
};

static const UINT8 cmd_vga_code[][3]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; Mipi : 2 lane
    ; Mipi data rate: 640Mbps/Lane
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->120
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/

    /* general configuration and image size [0x0000 ~ 0x0387] */
    {0x03, 0x40 ,0x01}, /* FRAME LENGTH LINES Total vertical size[15:8] */
    {0x03, 0x41 ,0xa8}, /* FRAME LENGTH LINES Total vertical size[7:0] */
                        /* 418 + 6 */
    {0x03, 0x42 ,0x06}, /* LINE LENGTH PCK Total horizontal size[15:8] */
    {0x03, 0x43 ,0x40}, /* LINE LENGTH PCK Total horizontal size[7:0] */
                        /* 1600 */

    {0x03, 0x4c ,0x02}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
    {0x03, 0x4d ,0x80}, /* X OUTPUT SIZE Image output horizontal width[7:0] */
                        /* 640 */
    {0x03, 0x4e ,0x01}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
    {0x03, 0x4f ,0x90}, /* Y OUTPUT SIZE Image output vertical height[7:0] */
                        /* 400 -> 400 */
    {0x03, 0x81 ,0x01}, /* X EVEN INC Bit[3:0]: h_even_inc
                                                Horizontal subsample even increase numbe */
    {0x03, 0x83 ,0x03}, /* X ODD INC Bit[3:0]: h_odd_inc
                                               Horizontal subsample odd increase number */
    {0x03, 0x85 ,0x01}, /* Y EVEN INC Bit[3:0]: v_even_inc
                                                Vertical subsample even increase number*/
    {0x03, 0x87 ,0x03}, /* Y ODD INC Bit[3:0]: v_odd_inc
                                               Vertical subsample odd increase number*/
    /* sensor control [0x3700 ~ 0x374B] */
    {0x37, 0x0e ,0x88},
    {0x37, 0x0a ,0x73},
    /* timing control [0x3800 ~ 0x3832] */
    {0x38, 0x11 ,0x08},
    {0x38, 0x13 ,0x04},
    {0x38, 0x21 ,0x01},
    /* NOT in register file */
    {0x41, 0x44 ,0x42},

    {0x45, 0x02 ,0x68}, /* NO description in register file */

    /* NOT in register file */
    {0x41, 0x42 ,0x14},
    /* BLC control [0x4000 ~ 0x401E] */
    {0x40, 0x01 ,0x03}, /* START LINE */
    {0x40, 0x04 ,0x02}, /* LINE NUM */
    /* manual AEC/AGC [0x3500 ~ 0x3A07, 0x5900 ~ 0x5910] */
    {0x35, 0x01 ,0x19}, /* EXPO0 Bit[7:0]: Expo0[15:8] Exposure in normal mode, the long exposure in HDR mode or the exposure
                                                       of first frame in frame exposure mode*/
    {0x35, 0x02 ,0xe0}, /* EXPO0 Bit[7:0]: Expo0[7:0] 8 LSB of exposure which is the  exposure in normal mode, the long
                                                      exposure in HDR mode or the exposer of first frame in frame exposure mode.
                                 Bit[3:0] is for fractional exposure, which is smaller than 0x0B.*/
    {0x3a, 0x04 ,0x01}, /* AEC CTRL04 Maximum exposure[15:8] */
    {0x3a, 0x05 ,0x9e}, /* AEC CTRL05 Bit[7:0]: Maximum exposure[7:0] */

};

static const UINT8 cmd_qvga_code[][3]SENSOR_SIF1DMA_BUF={
    /*-----------------------------------------------
    ; MCLK: 24Mhz
    ; Mipi : 2 lane
    ; Mipi data rate: 640Mbps/Lane
    ; SystemCLK     : xxx->MHz
    ; FPS	        : xxx->240
    ; HTS		    : xxx->
    ; VTS		    : xxx->
    ; Tline 	    : xxx->
    ; Max exp line	: xxx->
    ;-------------------------------------------------*/

    /* general configuration and image size [0x0000 ~ 0x0387] */
    {0x03, 0x40 ,0x00}, /* FRAME LENGTH LINES Total vertical size[15:8] */
    {0x03, 0x41 ,0xde}, /* FRAME LENGTH LINES Total vertical size[7:0] */
                        /* 212 + 10 */
    {0x03, 0x42 ,0x06}, /* LINE LENGTH PCK Total horizontal size[15:8] */
    {0x03, 0x43 ,0x22}, /* LINE LENGTH PCK Total horizontal size[7:0] */
                        /* 1570 */

    {0x03, 0x4c ,0x01}, /* X OUTPUT SIZE Image output horizontal width[11:8] */
    {0x03, 0x4d ,0x40}, /* X OUTPUT SIZE Image output horizontal width[7:0] */
                        /* 320 */
    {0x03, 0x4e ,0x00}, /* Y OUTPUT SIZE Image output vertical height[11:8] */
    {0x03, 0x4f ,0xc8}, /* Y OUTPUT SIZE Image output vertical height[7:0] */
                        /* 200 -> 200 */
    {0x03, 0x81 ,0x01}, /* X EVEN INC Bit[3:0]: h_even_inc
                                                Horizontal subsample even increase numbe */
    {0x03, 0x83 ,0x07}, /* X ODD INC Bit[3:0]: h_odd_inc
                                               Horizontal subsample odd increase number */
    {0x03, 0x85 ,0x05}, /* Y EVEN INC Bit[3:0]: v_even_inc
                                                Vertical subsample even increase number*/
    {0x03, 0x87 ,0x03}, /* Y ODD INC Bit[3:0]: v_odd_inc
                                               Vertical subsample odd increase number*/

    /* sensor control [0x3700 ~ 0x374B] */
    {0x37, 0x0e ,0x88},
    {0x37, 0x0a ,0xf4},

    /* timing control [0x3800 ~ 0x3832] */
    {0x38, 0x11 ,0x04},
    {0x38, 0x13 ,0x02},
    {0x38, 0x21 ,0x01},

    /* NOT in register file */
    {0x41, 0x44 ,0x32},

    {0x45, 0x02 ,0x68}, /* NO description in register file */

    /* NOT in register file */
    {0x41, 0x42 ,0x24},
    /* BLC control [0x4000 ~ 0x401E] */
    {0x40, 0x01 ,0x03}, /* START LINE */
    {0x40, 0x04 ,0x02}, /* LINE NUM */
    /* manual AEC/AGC [0x3500 ~ 0x3A07, 0x5900 ~ 0x5910] */
    {0x35, 0x01 ,0x0d}, /* EXPO0 Bit[7:0]: Expo0[15:8] Exposure in normal mode, the long exposure in HDR mode or the exposure
                                                       of first frame in frame exposure mode*/
    {0x35, 0x02 ,0x20}, /* EXPO0 Bit[7:0]: Expo0[7:0] 8 LSB of exposure which is the  exposure in normal mode, the long
                                                      exposure in HDR mode or the exposer of first frame in frame exposure mode.
                                 Bit[3:0] is for fractional exposure, which is smaller than 0x0B.*/
    {0x3a, 0x04 ,0x00}, /* AEC CTRL04 Maximum exposure[15:8] */
    {0x3a, 0x05 ,0xd2}, /* AEC CTRL05 Bit[7:0]: Maximum exposure[7:0] */

};

/*-----------------------------------------------------------------------------------------------*/
static const sensorLVDSCfg_t sensorMIPICfg =
{
    .mipi_nockrsten  =0 ,
	.wordcntsel 	 =0 ,	/* 0: hsize from From LVDS embeded data (CSI2)      */
	.Lvdsmode		 =0 ,	/* 0: 8bit ,analog marco bit mode.                  */
#if DATA_2LANE
	.Lvdslanenum	 =1 ,
#else
    #error "OV9713/OV9714 don't support 4-lane"
    .Lvdslanenum	 =0 ,
#endif
	                        /* 0: 4 lane, 1: 2lane
                                  OV9713 :   2lane only */
    .Lvdslanemode	 =0 ,	/* 0: enable lane3 in 4 lanes mode                  */
	.Lvdslaneswap	 =1 ,	/* 1: LSB first                                     */
#if MIPI_RAW12
	.Lvdsraw		 =1 ,	/* 1: raw 12                                        */
	.Ctrlword_sol	 =0x2C,
#else
    .Lvdsraw		 =0 ,   /* 0: raw 10                                        */
    .Ctrlword_sol	 =0x2B, /* OV7913 output formats: 8/10/12-bit RGB RAW       */
#endif
    .Lvdsoutsel 	 =0 ,	/* 0:                                               */
	.Lvdseofsel 	 =2 ,	/* 2: detect eof sync code, 1: count eol sync code, 0: count sol sync code */
 	.Lvdseolsel 	 =0 ,	/* 0: count pixel number                            */
	.lvdsvdsrc		 =1 ,	/* 1: decode emedded data header sof as vd source   */
	.lvds_dphy_sel	 =1 ,	/* 1: MIPI D-PHY                                    */
    .Ctrlword_sof	 =0x00 ,
    .Ctrlword_eof	 =0x01 ,
    /*
    .Ctrlword_sol	 =0x02 ,
    .Ctrlword_eol	 =0x03 ,
    */
    .syncwrod0		 =0xB8 ,
	.syncwrod1		 =0 ,
	.syncwrod2		 =0 ,
	/* HW signal spec */
	.vmode			 =0 ,
	.S2pphase		 =4 ,
	.tsettle		 =3 ,
	.teot			 =7 ,
	/* Extention Part */
	.Exthd_num		 =PREVIEW_EXTEND_LINES-1,
	.Exthd_pol		 =0 ,
	.Exthd_mode 	 =0 ,
#if SPCA6330 /* 6330 */
    #if LVDS_HCROP_ENABLE
    .cropenable 	 =0 ,
    #endif
#endif
};


/*sensor cfg*/
static const sensorTgPreviewCfg_t sensorTgPreview720pCfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreview720pBegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
#if LVDS_HCROP_ENABLE
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_720P_HSIZE,
		.ySize = PREVIEW_720P_VSIZE,
	},
#else
    .crop = NULL,
#endif
	.hsize = PREVIEW_720P_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */  ,
	.vsize = PREVIEW_720P_VSIZE,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_MIPI_RAW,
 	.cfa_pattern = PREVIEW_720P_CFAPATTERN,
    .hratio = PREVIEW_720P_HSIZE,     /* 16:9 fixed hratio = 3200 , vratio = 1800 */
    .vratio = PREVIEW_720P_VSIZE,     /* 4:3  fixed hratio = 4000 , vratio = 3000 */
    .fps_mhz = 59741082,              /* frame rate * 1000000 */
    .sensor_pclk = SENSOR_MCLK,       /* Hz */
    .sensor_htotal = 1598 ,           /* per line of pixel HW , htotal/sensor_pclk */
    .pix_100khz = PIX_100KHZ/100 ,    /* pclk1x speed Datarate * lanes / bit x HSize / HTotal */
};


static const sensorTgPreviewCfg_t sensorTgPreviewVGACfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = sensorDoFuncPreviewVGABegin,
		.previewShortExp = sensorDoFuncPreviewShortExp,
		.previewLongExpBegin = NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd = NULL,
		.previewExpEnd = sensorDoFuncPreviewExpEnd,
	},
#if LVDS_HCROP_ENABLE
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
#else
    .crop = NULL,
#endif
	.hsize = PREVIEW_VGA_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
	.vsize = PREVIEW_VGA_VSIZE,
	.sensor_model = SENSORMODEL,
    .model_info = SENSORMODEL_MIPI_RAW,
 	.cfa_pattern = PREVIEW_VGA_CFAPATTERN,
    .hratio = PREVIEW_VGA_HSIZE,      /* 16:9 fixed hratio = 3200 , vratio = 1800 */
    .vratio = PREVIEW_VGA_VSIZE,      /* 4:3  fixed hratio = 4000 , vratio = 3000 */
    .fps_mhz = 117924528,             /* frame rate * 1000000 */
    .sensor_pclk = SENSOR_MCLK,       /* Hz */
    .sensor_htotal = 1600 ,           /* per line of pixel HW , htotal/sensor_pclk */
    .pix_100khz = PIX_100KHZ/100 ,    /* pclk1x speed Datarate * lanes / bit x HSize / HTotal */
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
#if LVDS_HCROP_ENABLE
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_QVGA_HSIZE,
		.ySize = PREVIEW_QVGA_VSIZE,
	},
#else
    .crop = NULL,
#endif
	.hsize = PREVIEW_QVGA_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
	.vsize = PREVIEW_QVGA_VSIZE,
	.sensor_model = SENSORMODEL,
	.model_info = SENSORMODEL_MIPI_RAW,
	.cfa_pattern = PREVIEW_QVGA_CFAPATTERN,
    .hratio = PREVIEW_QVGA_HSIZE,     /* 16:9 fixed hratio = 3200 , vratio = 1800 */
    .vratio = PREVIEW_QVGA_VSIZE,     /* 4:3  fixed hratio = 4000 , vratio = 3000 */
    .fps_mhz = 229526257,             /* frame rate * 1000000 */
    .sensor_pclk = SENSOR_MCLK,       /* Hz */
    .sensor_htotal = 1570 ,           /* per line of pixel HW , htotal/sensor_pclk */
    .pix_100khz = PIX_100KHZ/100 ,    /* pclk1x speed Datarate * lanes / bit x HSize / HTotal */
};

static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
    &sensorTgPreview720pCfg,  /* 0x30 - index 0 (720P) */
    &sensorTgPreviewVGACfg,   /* 0x31 - index 1 (VGA)  */
    &sensorTgPreviewQVGACfg   /* 0x32 - index 2 (QVGA) */
};

#if SNAP_DUMMYVD_NUM
static sensorDoFunc_t sensorDoFuncSnapDummyTbl[] = {
	sensorDoFuncSnapDummy0,
};
#endif


static sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			#if 0
			.psnapEarly = (sensorDoFunc_t []){
				sensorDoFuncSnapEarly0,
			},
			#endif
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
#if SNAP_DUMMYVD_NUM
			.psnapDummy = sensorDoFuncSnapDummyTbl,
#else
            .psnapDummy = NULL,
#endif
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
		.pix_100khz = PIX_100KHZ/100,
	};

/****TODO****frame snap mode*/
#if 0
static sensorTgSnapCfg_t sensorTgSnapFrameCfg = {
	.fpdo = &(const sensorSnapDo_t) {
		.psnapEarly = (sensorDoFunc_t []){
			sensorDoFuncSnapEarly0,
			sensorDoFuncSnapEarly1,
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
};
#endif

static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	 &sensorTgSnapRollCfg /* 0x40 index 0 */
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
	.hsize           = PREVIEW_720P_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
	.vsize           = PREVIEW_720P_VSIZE,
	.lastlinepix     = PREVIEW_720P_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
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
	.hsize           = PREVIEW_VGA_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
	.vsize           = PREVIEW_VGA_VSIZE,
	.lastlinepix     = PREVIEW_VGA_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
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
	.hsize           = PREVIEW_QVGA_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
	.vsize           = PREVIEW_QVGA_VSIZE,
	.lastlinepix     = PREVIEW_QVGA_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
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
	.hsize           = CAPTURE_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
	.vsize           = CAPTURE_VSIZE,
	.lastlinepix     = CAPTURE_HSIZE /* + SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP */ ,
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
};

static const sensorTgDev_t sensorTgDev = {
	.tgCfg = &sensorTgCfg,
	.tgSvc = &(const sensorTgSvc_t) {
		.resload = sensorCfgInit,
		.resstop = sensorCfgStop,
		.init = sensorDoFuncInit,
		.stop = sensorDoFuncStop,                  /****TODO***/
		.pwrsave = NULL,                           /****TODO***/
		.expwrite = sensor_cmdport_exp_write,
		.expvtbldo = sensor_cmdport_exp_vtbl_do,
        .crop = NULL,                              /****TODO***/
#if STREAMOF_ENABLE
		.streamoff = sensorDoFuncStreamOff,
		.streamon  = sensorDoFuncStreamOn,
#endif
	},
	.previewCfg = sensorTgPreviewCfgTbl,
	.snapCfg = sensorTgSnapCfgTbl,
    .defSyncSigCfg  = &sensorSyncSigPreview720pCfg,
	.verStr = MODEL_VERSION,
};

void
sensorTg_ov9713_Install()
{
	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/

    sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

static UINT8 OVsensorRead(UINT16 addr)
{
	UINT32 cmdportid;
	UINT8 regread[3];

    cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	sensorSerialBlockRead(cmdportid,1,regread,0);
	return regread[2];
}

static void
sensorOV_Init(void)
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
	SENSOR_PROF_LOG_PRINT("OV9713 model-ID 0x300A=0x%x",  OVsensorRead(0x300a));
    SENSOR_PROF_LOG_PRINT("OV9713 model-ID 0x300B=0x%x",  OVsensorRead(0x300b));

    sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code)/sizeof(cmd_init_code[0])), cmd_init_code[0],0);

}

static void
sensorReportsize()
{
    PROF_LOG_LINE_COUNT("----- sensorReportsize -----");
	PROF_LOG_LINE_COUNT("mclkdiv   cdspclkdiv  = (%d,%d)",READ8(0xb0000082),READ8(0xb0000083));
	PROF_LOG_LINE_COUNT("front output size     = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
	PROF_LOG_LINE_COUNT("imgsource output size = (%d,%d)",READ16(0xb0002088),READ16(0xb000208A));
	PROF_LOG_LINE_COUNT("rgbhscale output size = (%d,%d)",READ16(0xb000208C),READ16(0xb000208E));
	WRITE16(0xb0009064,1);
	PROF_LOG_LINE_COUNT("line count            = (%d)",READ16(0xb0009064));
    PROF_LOG_LINE_COUNT("----------------------------");

#if 0
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
#endif
}


static void
sensorDoFuncInit()
{
	UINT32 cmdportid;
    UINT32 ret = 0 ;
    SENSOR_PROF_LOG_PRINT("________Sensor Driver MODEL_VERSION <<%s>>", sensorTgDev.verStr);
	SENSOR_PROF_LOG_PRINT("________Sensor Driver Build Time    <<%s>>", BUILD_TIME);

#if 0
	/* TGGPIO configuration here */
	LOGI_REG_DECLARE(frontReg0_t,pfrontReg0);
	pfrontReg0->tggpioosel &= ~0x80;
	pfrontReg0->tggpiooe  |= 0x80;
    /* TGGPIO7 , Register File said "BIT7 Reserved ..... */
#endif

    /*TG PLL=MCLK out=24MHz*/
    ret = sensorTgPllClkExtSet(24000000);

    SENSOR_PROF_LOG_PRINT("TG PLL Clock = %d",ret);

/* Master clock and Pixel clock configuration here */
#if SPCA6330 /* 6330 */
    #if 1 /*EVB*/
        halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS, SPCA_PCLK1X);
        /*Always Use preview mode 9 when pclk1x > 150MHz*/
        sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_VIEW_MODE9,1); /*enable preview mode 9*/
    #else
        halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,121000);     /* 121   MHz  CDSP 243MHz*/
        halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,150000);     /* 150   MHz  CDSP 300MHz*/
        halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS,148500);     /* 148.5 MHz  CDSP 297MHz*/
    #endif

    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR_SNAP_MODE9,1); /*enable snap mode 9*/
#else
    halPllFrontClkSet(PCLK1XSEL_INT|PCLKNXSEL_LVDS, SPCA_PCLK1X);
#endif

	halFrontClkPhaseCfg(MASTERPHASE, PIXELPHASE, ADCKPHASE);
	/* Sensor Power On Sequence */
	sp5kGpioCfgSet(SENOR_GPIO_GRP,OUT_SENSOR_GPIO_MASK,CFG_SENSOR_GPIO_DIR);

#if 0
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN);
	sp5kGpioCfgSet(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET);
#endif

    /* OV9713 PWRDN low active !! */
    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,0);            /* PWD Low   */
    /* OV9713 RESET low active !! */
    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,0);            /* RESET Low */
	tmrUsWait(10000);


    sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_PWRDN,SENSOR_PWRDN); /* PWD High   */
	/* tmrUsWait(50000); */ /* t0 + t1 + t2 min = 0 */
	sp5kGpioWrite(SENOR_GPIO_GRP,SENSOR_RESET,SENSOR_RESET); /* RESET High  */
	tmrUsWait(25000); /* t3 min = 8192 XVCLK (341 us for 24MHz) + 1ms for SCCB stable */

    /*I2C Init code Here */
	cmdportid = sensorTgCfg.cmdportid_xtg;
	tmrUsWait(2000);
    sensorOV_Init();

    tmrUsWait(25000); /*25ms*/

	sensorLVDSCfgSet(&sensorMIPICfg);
	sensorSyncSigReshapeModeSet(FRONT_RESHAPE_MANUAL_MODE);

    tmrUsWait(100000);	/* delay 100 ms*/

    sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

    sensorResumeInstall(sensorResumeExecuteLoad);
	sensorSuspendInstall(sensorSuspendExecuteLoad);

#if MODE_SYNC_EARLY
	sensorStsCfg(SENSOR_SYS_CFG_MODE_SYNC_EARLY, MODE_SYNC_EARLY);
#endif

#if STREAMOF_ENABLE
	sensorStsCfg(SENSOR_SYS_CFG_FLOW_USE_ABORT, STREAMOF_ENABLE);
#endif

#if 0
    sensorStsCfg(SENSOR_SYS_CFG_DUAL_SENSOR_AE_EN,0); /* dual sensor ae disable */
#endif

    SENSOR_PROF_LOG_PRINT("sensorDoFuncInit done");

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

static void
sensorDoFuncPreview720pBegin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_LINE_COUNT("sensorDoFuncPreview720pBegin");

#if 0
    if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
#else
    sensorEventVvalidWait(1);
#endif
    g_IsSnap2Pv=0;
    fpvsusp=1;
    sensorSerialFlush(cmdportid);
    sensorSerialBulkWrite(cmdportid,(sizeof(cmd_720p_code)/sizeof(cmd_720p_code[0])), cmd_720p_code[0],0);

#if 0
    if(is_aborted){
        SENSOR_PROF_LOG_PRINT("sensorSyncSigInit");
        sensorSyncSigInit(&sensorSyncSigPreview720pCfg);
    } else {
        SENSOR_PROF_LOG_PRINT("sensorSyncSigSet");
        sensorSyncSigSet(&sensorSyncSigPreview720pCfg);
    }
#else
    sensorSyncSigInit(&sensorSyncSigPreview720pCfg);
#endif

    SP5K_SET_LVDS_HCROP(PREVIEW_720P_HCROP_OFFSET);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    PROF_LOG_LINE_COUNT("MIPI 720P_30fps done");
}


static void
sensorDoFuncPreviewVGABegin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_LINE_COUNT("sensorDoFuncPreviewVGABegin");
#if 0
    if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
#else
    sensorEventVvalidWait(1);
#endif
    g_IsSnap2Pv=0;
    fpvsusp=1;
    sensorSerialFlush(cmdportid);
    sensorSerialBulkWrite(cmdportid,(sizeof(cmd_vga_code)/sizeof(cmd_vga_code[0])), cmd_vga_code[0],0);

#if 0
    if(is_aborted){
        SENSOR_PROF_LOG_PRINT("sensorSyncSigInit");
        sensorSyncSigInit(&sensorSyncSigPreviewVGACfg);
    }else{
        SENSOR_PROF_LOG_PRINT("sensorSyncSigSet");
        sensorSyncSigSet(&sensorSyncSigPreviewVGACfg);
    }
#else
    sensorSyncSigInit(&sensorSyncSigPreviewVGACfg);
#endif

    SP5K_SET_LVDS_HCROP(PREVIEW_VGA_HCROP_OFFSET);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    PROF_LOG_LINE_COUNT("MIPI VGA_30fps done");
}

static void
sensorDoFuncPreviewQVGABegin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_LINE_COUNT("sensorDoFuncPreviewQVGABegin");
#if 0
    if(g_IsSnap2Pv){
        sensorEventVvalidWait(1);
    }
#else
    sensorEventVvalidWait(1);
#endif
    g_IsSnap2Pv=0;
    fpvsusp=1;
    sensorSerialFlush(cmdportid);
    sensorSerialBulkWrite(cmdportid,(sizeof(cmd_qvga_code)/sizeof(cmd_qvga_code[0])), cmd_qvga_code[0],0);

#if 0
    if(is_aborted){
        SENSOR_PROF_LOG_PRINT("sensorSyncSigInit");
        sensorSyncSigInit(&sensorSyncSigPreviewQVGACfg);
    }else{
        SENSOR_PROF_LOG_PRINT("sensorSyncSigSet");
        sensorSyncSigSet(&sensorSyncSigPreviewQVGACfg);
    }
#else
    sensorSyncSigInit(&sensorSyncSigPreviewQVGACfg);
#endif

    SP5K_SET_LVDS_HCROP(PREVIEW_QVGA_HCROP_OFFSET);
    sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
    PROF_LOG_LINE_COUNT("MIPI QVGA_30fps done");
}

static void
sensorDoFuncPreviewShortExp()
{
}

static void
sensorDoFuncPreviewExpEnd()
{
}

#if 0
static void
sensorDoFuncSnapEarly0()
{

}
static void
sensorDoFuncSnapEarly1()
{

}
#endif

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

/*static void
FrontReset()
{
    WRITE8(0xb0009010,1);
    WRITE8(0xb0009010,0);
}*/

static void
sensorDoFuncSnapExpBegin()
{
    UINT32 cmdportid;
    cmdportid = sensorTgCfg.cmdportid_xtg;

#if (_SENSOR_DBG_)
    DBG_GPIO(1);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(0);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(1);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(0);
    tmrUsWait(1000); /*1ms*/
    DBG_GPIO(1);
#endif

    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
    PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(0);
    if(!g_IsSnap2Pv){
        g_IsSnap2Pv=1;
        PROF_LOG_LINE_COUNT("g_IsSnap2Pv");
        sensorSerialFlush(cmdportid);
        PROF_LOG_LINE_COUNT("SerialFlush(cmdportid)");
        #if 0
        sensorEventVvalidWait(1);  /* for better performance */
        #endif
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_off)/sizeof(cmd_stream_off[0])), cmd_stream_off[0],0);
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_still_code)/sizeof(cmd_still_code[0])), cmd_still_code[0],0);
        PROF_LOG_LINE_COUNT("cmd_still_code");

        #if 0
        PROF_LOG_LINE_COUNT("sensorSyncSigInit");
        sensorSyncSigInit(&sensorSyncSigSnapCfg);
        #else
        PROF_LOG_LINE_COUNT("sensorSyncSigSet");
        sensorSyncSigSet(&sensorSyncSigSnapCfg);
        #endif

        SP5K_SET_LVDS_HCROP(CAPTURE_HCROP_OFFSET);

        sensorCDSPExtSet(SENSOR_CDSP_EXT_DESENOR,1);
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0])), cmd_stream_on[0],0);
    }
    PROF_LOG_LINE_COUNT("e");
#if (_SENSOR_DBG_)
    DBG_GPIO(0);
#endif
}

static void
sensorDoFuncSnapShortExp()
{
}

#if (_SENSOR_DBG_)
static void
DEBUGIO(UINT8 level)
{
    DBG_GPIO(level);
}
#endif


static void
sensorDoFuncSnapExpEnd()
{
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(1);
    PROF_LOG_LINE_COUNT("e");
}

#if SNAP_DUMMYVD_NUM
static void
sensorDoFuncSnapDummy0()
{
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
    PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(2);
    PROF_LOG_LINE_COUNT("e");
}
#endif

static void
sensorDoFuncSnapField0VvldEndCB()
{
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    PROF_LOG_LINE_COUNT("front output size = (%d,%d)",READ16(0xb00090CC),READ16(0xb00090CE));
    TIME_LOG(3);
    PROF_LOG_LINE_COUNT("e");
}

static void
sensorDoFuncSnapReadoutField0()
{
    sensor_frame_cb_reg(2/*SENSOR_IRQ_SVC_VVALID_END*/,sensorDoFuncSnapField0VvldEndCB,0);
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(4);
}


static void
sensorDoFuncSnapFieldEnd()
{
#if (_SENSOR_DBG_)
    DEBUGIO(1);
#endif
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    TIME_LOG(5);
#if (_SENSOR_DBG_)
    DEBUGIO(0);
#endif
}

static void
sensorDoFuncSnapNext(UINT32 next_mode)
{
#if (_SENSOR_DBG_)
    DEBUGIO(1);
#endif
    PROF_LOG_LINE_COUNT("s");
    if((next_mode&0xf0)==SENSOR_MODE_SNAP){
        PROF_LOG_LINE_COUNT("@@@@ get sensor next mode is snap");
    }else{
        PROF_LOG_LINE_COUNT("@@@@ get sensor next mode is preview");
    }
    PROF_LOG_LINE_COUNT("e");
#if (_SENSOR_DBG_)
    DEBUGIO(0);
#endif
}

static void
sensorDoFuncSnapReadoutEnd()
{
    TIME_LOG(6);
    PROF_LOG_LINE_COUNT("s");
    PROF_LOG_LINE_COUNT("front2cdsp gating = (%d)",READ8(0xb0009080)&0x01);
	PROF_LOG_LINE_COUNT("snap trig         = (%d)",READ8(0xb0009068)&0x01);
    PROF_LOG_LINE_COUNT("SnapExpS--->ExpEnd:    %d.%d ms",TIME_DIFF(1,0)/1000,TIME_DIFF(1,0)%1000);
    PROF_LOG_LINE_COUNT("ExpEnd----->Dummy0:    %d.%d ms",TIME_DIFF(2,1)/1000,TIME_DIFF(2,1)%1000);
    PROF_LOG_LINE_COUNT("Dummy0----->Field0:    %d.%d ms",TIME_DIFF(4,2)/1000,TIME_DIFF(4,2)%1000);
    PROF_LOG_LINE_COUNT("Field0----->FieldEnd:  %d.%d ms",TIME_DIFF(5,4)/1000,TIME_DIFF(5,4)%1000);
	PROF_LOG_LINE_COUNT("FieEnd----->ReadoutEnd:%d.%d ms",TIME_DIFF(6,5)/1000,TIME_DIFF(6,5)%1000);
    #if SENSOR_PROF_LOG_EN
        sensorReportsize();
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
    PROF_LOG_LINE_COUNT("s");
    sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, nreg, pdata);
    sensorSerialFlush(sensorTgCfg.cmdportid_exp);
#if 0
    if((sensorModeGet()&0xF0)==0x40){
        PROF_LOG_LINE_COUNT("Cap [nreg=%d][0x%02x%02x: %d(Dec)][0x%02x%02x: %d(Dec)][0x%02x%02x: %d(Dec)]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7],pdata[8]);
        PROF_LOG_LINE_COUNT("Cap          [0x%02x%02x: %d(Dec)][0x%02x%02x: %d(Dec)]",pdata[9],pdata[10],pdata[11],pdata[12],pdata[13],pdata[14]);
    }else{
        PROF_LOG_LINE_COUNT("prev [nreg=%d][0x%02x%02x: %d(Dec)][0x%02x%02x: %d(Dec)][0x%02x%02x: %d(Dec)]",nreg,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7],pdata[8]);
        PROF_LOG_LINE_COUNT("prev          [0x%02x%02x: %d(Dec)][0x%02x%02x: %d(Dec)]",pdata[9],pdata[10],pdata[11],pdata[12],pdata[13],pdata[14]);
    }
#endif
    PROF_LOG_LINE_COUNT("e");
}

static void
sensor_cmdport_exp_vtbl_do(
		UINT32 vtbl_type,
		UINT32 vcount
		)
{
}
#if 0
static void
sensorDoFuncOVStreamOff(void)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
#if 0
    #if SENSOR_PROF_LOG_EN    /*debug*/
        profLogInit(PROF_ID, 0x10000);
        profLogModeSet(PROF_ID, 0);

        /* profLogInit(2, 0x8000);
        profLogModeSet(2, 0); */
    #endif
#endif
    PROF_LOG_LINE_COUNT("sensorModeGet= %x", sensorModeGet());
	PROF_LOG_LINE_COUNT("s");
    sensorSerialFlush(cmdportid);
	sensorSerialAsyncWrite(cmdportid,(sizeof(cmd_stream_off)/sizeof(cmd_stream_off[0])), cmd_stream_off[0]);
    sensorSerialFlush(cmdportid);
 	PROF_LOG_LINE_COUNT("e");
}

static void
sensorDoFuncOVStreamOn(void)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	PROF_LOG_LINE_COUNT("s");
    sensorSerialFlush(cmdportid);
	sensorSerialAsyncWrite(cmdportid,(sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0])), cmd_stream_on[0]);
    sensorSerialFlush(cmdportid);
	PROF_LOG_LINE_COUNT("e");
}
#endif

#if STREAMOF_ENABLE
static void
sensorDoFuncStreamOff(UINT32 abort)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==0){
        PROF_LOG_LINE_COUNT("sensorModeGet= %x", sensorModeGet());
		PROF_LOG_LINE_COUNT("s");
        sensorSerialFlush(cmdportid);
		sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_off)/sizeof(cmd_stream_off[0])), cmd_stream_off[0],0);
 		PROF_LOG_LINE_COUNT("e");
		is_aborted = 1;
	}
}

static void
sensorDoFuncStreamOn(UINT32 abort)
{
    UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	if(is_aborted==1 ){
	    PROF_LOG_LINE_COUNT("s");
        sensorSerialFlush(cmdportid);
		sensorSerialBulkWrite(cmdportid,(sizeof(cmd_stream_on)/sizeof(cmd_stream_on[0])), cmd_stream_on[0],0);
		PROF_LOG_LINE_COUNT("e");
		is_aborted = 0;
	}
}
#endif

static UINT32 sensorResumeExecuteLoad()
{
    #if SUSPEND_ENABLE
    	if(sensorSuspended ==1){
            /* sensorResumeExecute() */
	    	PROF_LOG_LINE_COUNT("senResu s");
            sensorDoFuncOVStreamOn();
            tmrUsWait(5000);/*5ms*/
 	    	PROF_LOG_LINE_COUNT("senResu e");
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
	    	PROF_LOG_LINE_COUNT("senSusp s");
            sensorDoFuncOVStreamOff();
            tmrUsWait(5000);/*5ms*/
	    	PROF_LOG_LINE_COUNT("senSusp e");
	    	sensorSuspended = 1;
	    }
	#endif
	return SUCCESS;
}

#if SP5K_SENSOR_OV9713
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	static UINT8 regData[][3] ={
		{0x35,0x00,0x00},
		{0x35,0x01,0x00},
		{0x35,0x02,0x00},
        {0x03,0x40,0x00},
        {0x03,0x41,0x00},
	};

#if 0
	regData[0][2] = (expLine>>16)&0xf;
	regData[1][2] = (expLine>>8)&0xff;
	regData[2][2] = expLine&0xff;
#endif


    regData[0][2] = (( expLine & 0xF000 ) >> 12 );
	regData[1][2] = (( expLine & 0x0FF0 ) >>  4 );
 	regData[2][2] = (( expLine & 0x000F ) <<  4 );

    expLine = expLine + 4 ;
    regData[3][2] = (( expLine & 0xFF00 ) >>  8 );
    regData[4][2] =  ( expLine & 0x000F ) ;

	SENSOR_PROF_LOG_PRINT("expLine(reg0) = %d [%x %x %d]",expLine,regData[0][0],regData[0][1],regData[0][2]);
	SENSOR_PROF_LOG_PRINT("expLine(reg1) = %d [%x %x %d]",expLine,regData[1][0],regData[1][1],regData[1][2]);
	SENSOR_PROF_LOG_PRINT("expLine(reg2) = %d [%x %x %d]",expLine,regData[2][0],regData[2][1],regData[2][2]);
    SENSOR_PROF_LOG_PRINT("expLine(reg3) = %d [%x %x %d]",expLine,regData[3][0],regData[3][1],regData[3][2]);
    SENSOR_PROF_LOG_PRINT("expLine(reg4) = %d [%x %x %d]",expLine,regData[4][0],regData[4][1],regData[4][2]);
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,5, regData[0]);
    sensorSerialFlush(sensorTgCfg.cmdportid_exp);
	sensorEventNewFrameWait(1);
}
#endif




