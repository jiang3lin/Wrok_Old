/**************************************************************************
 *
 *        Copyright (c) 2005-2009 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _HAL_FRONT_H_
#define _HAL_FRONT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {/* Interrupt parameter modify by stanliu for 5110 */
	HAL_FRONT_INT_VD_RISE       =  0,
	HAL_FRONT_INT_VD_FALL       =  1,
	HAL_FRONT_INT_VD_ORG_RISE   =  2,
	HAL_FRONT_INT_VD_ORG_FALL   =  3,
	HAL_FRONT_INT_VVALID_RISE   =  4,
	HAL_FRONT_INT_VVALID_FALL   =  5,
	HAL_FRONT_INT_SIF1_DONE     =  6,
	HAL_FRONT_INT_SIF2_DONE     =  7,
	HAL_FRONT_INT_SNAP_DONE     =  8,
	HAL_FRONT_INT_MSHUTTER_RISE =  9,
	HAL_FRONT_INT_FLASH_RISE    = 10,
	HAL_FRONT_INT_HD_NUM0       = 11,
	HAL_FRONT_INT_HD_NUM1       = 12,
	HAL_FRONT_INT_HD_NUM2       = 13,
	HAL_FRONT_INT_HD_NUM3       = 14,
	HAL_FRONT_INT_TGGPIOI2_RISE = 15,
	HAL_FRONT_INT_TGGPIOI2_FALL = 16,
	HAL_FRONT_INT_TGGPIOI3_RISE = 17,
	HAL_FRONT_INT_TGGPIOI3_FALL = 18,
	HAL_FRONT_INT_LMI_FLOW_END  = 19,
	HAL_FRONT_INT_EXT_MSHUTTER  = 20,
	HAL_FRONT_INT_SIF1_DMA_DONE = 21,
	HAL_FRONT_INT_LMI_SNAP_DONE = 22,
	HAL_FRONT_INT_PWM0_DONE     = 32,
	HAL_FRONT_INT_PWM0_STOP     = 33,
	HAL_FRONT_INT_PWM0_STEPHIT 	= 34,
	HAL_FRONT_INT_PWM1_DONE     = 36,
	HAL_FRONT_INT_PWM1_STOP     = 37,
	HAL_FRONT_INT_PWM1_STEPHIT 	= 38,
	HAL_FRONT_INT_PWM2_DONE 	= 40,
	HAL_FRONT_INT_PWM2_STOP 	= 41,
	HAL_FRONT_INT_PWM2_STEPHIT 	= 42,
	HAL_FRONT_INT_PWM3_DONE 	= 44,
	HAL_FRONT_INT_PWM3_STOP 	= 45,
	HAL_FRONT_INT_PWM3_STEPHIT 	= 46,
	HAL_FRONT_INT_TOT,
} halFrontInt_t;

/**************************************************************************
 *                           D A T A   T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*
 * hal_front_int_en.c
 */
void halFrontIntEnable(UINT32 id, UINT32 en);

/*
 * hal_front_int_reg.c
 */
void *halFrontIrqReg(UINT32 id, void *pfunc);
void *halFrontIrqUnreg(UINT32 id);

/*
 * hal_front.c
 */
void   halFrontClkCfg(UINT32 mclkmode, UINT32 pclkmode);
void   halFrontClkSet(UINT32 tgpllen, UINT32 tgpllclkest, UINT32 mclkdiv, UINT32 pclkdiv);
void   halFrontClkPhaseCfg(UINT32 masterphase, UINT32 pixelphase, UINT32 adckphase);
void   halFrontHdnumIrqCfg(UINT32 id, UINT32 h, UINT32 p);

#undef halFrontVdIrqCfg
#define halFrontVdIrqCfg(risenum,fallnum)
#undef halFrontBShutterEn
#define halFrontBShutterEn(enable,type)
#undef halFrontMonitorSnap
#define halFrontMonitorSnap(snapnum)

#if SPCA5110
#undef halFrontTgPhaseCfg
#define halFrontTgPhaseCfg(fh1phase,fh2phase,rsgatephase,fcdsphase,fsphase)
#undef halFrontInputGate
#define halFrontInputGate(tggz)
#else
void   halFrontTgPhaseCfg(UINT32 fh1phase, UINT32 fh2phase, UINT32 rsgatephase, UINT32 fcdsphase, UINT32 fsphase);
void   halFrontInputGate(UINT32 tggz);
#endif

void   halFrontVdWait(UINT32 mode, UINT32 number);
void   halFrontVValidWait(UINT32 mode, UINT32 number);
void   halFrontSiggenSet(UINT32 enable, UINT32 mode);
UINT32 halFrontSiggenGet(UINT32 *pmode);
void   halFrontSiggenSenTypeSet(UINT32 senType);
void   halFrontProbeSet(UINT32 enable, UINT32 mode);
void   halFrontDummyWr(UINT32 addr, UINT32 value);
UINT32 halFrontDummyRd(UINT32 addr);

void   halFrontFlashTrig(void);
void   halFrontLvdsSet(UINT32 enable);
UINT32 halFrontLvdsGet();


#if 0 /*FIXME*/ /****TODO*front clocks config moved to global*/
/*=================================================================*/
UINT32 halFrontSensorClkCfgSet (UINT32 cfgSel, UINT32 value);

typedef enum {
	FRONT_MCLK_SRC = 0x00000000,
	FRONT_MCLK_DIV,
	FRONT_MCLK_PHASE,
	FRONT_PCLK_SRC,
	FRONT_PCLK_DIV,
	FRONT_PCLK_IN_PHASE,
	FRONT_PCLK_OUT_PHASE,
	FRONT_MCLK_DIV_VDSYNC,
	FRONT_PCLK_RGB_IN_EDGE,
} halFrontSensorClkCfgSetSel_t;

enum {
	FRONT_MCLK_SRC_TGPLL_192M= 0,
	FRONT_MCLK_SRC_ADCLPI
};

enum{
	FRONT_PCLK_SRC_MCLK = 0,
	FRONT_PCLK_SRC_ADCKI
};
#endif

/*=================================================================*/
UINT32 halFrontReshapeSyncCfgSet (UINT32 cfgSel, UINT32 FallValue, UINT32 RiseValue);

typedef enum {
	FRONT_RESHAPE_H = 0x00000000,
	FRONT_RESHAPE_V,
	FRONT_RESHAPE_UPDATE, /*FallValue is halFrontReshapeUpdate_t*/
	FRONT_RESHAPE_AUTO,   /*FallValue 0:auto off; 1:auto on*/
} halFrontReshapeSyncCfgSetSel_t;
typedef enum {
 	FRONT_RESHARE_UPDATE_NOW,
 	FRONT_RESHAPE_UPDATE_ORIG_VD,
 	FRONT_RESHAPE_UPDATE_VD,
 	FRONT_RESHAPE_UPDATE_ORIG_HD_BY_VD,
} halFrontReshapeUpdate_t;

/*=================================================================*/
#define HAL_FRONT_SYNCSIG_DONT_CHANGE 0x7FFFFFFF

UINT32 halFrontSyncSigCfgSet (UINT32 cfg, UINT32 val1, UINT32 val2);
UINT32 halFrontSyncSigCfgGet (UINT32 cfg, UINT32 *pval1, UINT32 *pval2);

typedef enum {
	FRONT_SYNC_MODE = 0x00000000,
	FRONT_SYNC_SIZE,
	FRONT_SYNC_HDVD_IN_POLAR,
	FRONT_SYNC_HDVD_OUT_POLAR,
	FRONT_SYNC_EDGE,
	FRONT_SYNC_OFFSET,
	FRONT_SYNC_LINE,
	FRONT_SYNC_DUMMY_PIXEL,
	FRONT_SYNC_FRAME,
	FRONT_SYNC_UPDATE, /*Value1 is halFrontSyncSigCfgUpdateSel_t*/
	FRONT_SYNC_HD_VD_DIFF,
	FRONT_SYNC_BYPASS_CDSP_VALID,
	/*hdvd before reshape pol, useful for vari blank yuv sensors*/
	FRONT_SYNC_HDVD_RESHAPE_POLAR,
	FRONT_SYNC_VALID_FMT, /*val1 is halFrontSyncValidFmt_t*/
	FRONT_SYNC_EXTHD_NUM,
	FRONT_SYNC_EXTHD_POL,
	FRONT_SYNC_HOFFSET0, 
	FRONT_SYNC_HOFFSET2,	
	FRONT_SYNC_HDIV_SHIFT,	
} halFrontSyncSigCfgSetSel_t;

typedef enum {
	FRONT_SYNC_UPDATE_MASTER,
	FRONT_SYNC_UPDATE_TOTAL, /*Value2 is halFrontSyncSigUpdateTotal_t*/
	FRONT_SYNC_UPDATE_SIZE,
} halFrontSyncSigCfgUpdateSel_t;
/*0:immd,1:orig vd,2:reshape vd*/
typedef enum {
 	FRONT_SYNC_UPDATE_TOTAL_NOW,
 	FRONT_SYNC_UPDATE_TOTAL_ORIG_VD,
 	FRONT_SYNC_UPDATE_TOTAL_VD,
 	FRONT_SYNC_UPDATE_TOTAL_ORIG_HD_BY_VD,
} halFrontSyncSigUpdateTotal_t;

typedef enum {
	FRONT_SYNC_VALID_FMT_SENSOR,
	FRONT_SYNC_VALID_FMT_TV_IN,
	FRONT_SYNC_VALID_FMT_SENSOR_MIPI,
} halFrontSyncValidFmt_t;

#define TG_CMOS_SP5K_SLAVE      0
#define TG_CMOS_SP5K_MASTER     1
#define TG_CCD_EXT_SP5K_SLAVE   2
#define TG_CCD_EXT_SP5K_MASTER  3
#define TG_TVIN_DEC             8

#define TG_BMAP_SP5K_MASTER  0x01
#define TG_BMAP_CCD_EXT      0x02
#define TG_BMAP_TVIN_DEC     0x08

/*=================================================================*/
UINT32 halFrontImgSrcCfgSet(UINT32 cfgSel, UINT32 Value);

typedef enum {
	FRONT_IMG_SRC_SET = 0x00000000,
	FRONT_IMG_SIGGEN_CFG, /* revoked */
	FRONT_IMG_SENSOR_BIT_WIDTH,
	FRONT_IMG_SENSOR_CAP_FIELD,
	FRONT_IMG_SENSOR_FIELD_SEL, /*sync bitmask 0x10*/
	FRONT_IMG_TV_CFG,
	FRONT_IMG_DIFF_GRAY_CODE_CFG,
	FRONT_IMG_SRC_IN_PCLK_EDGE,
} halFrontImgSrcCfgSetSel_t;

#define FRONT_IMG_SENSOR_FIELD_SEL_UPDATE_SYNC 0x10

typedef enum {
	FRONT_IMG_SRC_SENSOR, /* sensor with normal AFE */
	FRONT_IMG_SRC_SENSOR_GRAY,
	FRONT_IMG_SRC_SENSOR_DIFF,
	FRONT_IMG_SRC_SENSOR_DIFF_GRAY,
	FRONT_IMG_SRC_TV = 0x08,
} frontSensorSrc_t;

#if SPCA5110
typedef enum {
	FRONT_IMG_SIGGEN_BAYER_GR = 0,
	FRONT_IMG_SIGGEN_BAYER_R  = 1,
	FRONT_IMG_SIGGEN_BAYER_B  = 2,
	FRONT_IMG_SIGGEN_BAYER_GB = 3,
	FRONT_IMG_SIGGEN_SONY     = 4,
	FRONT_IMG_SIGGEN_SHARP    = 4,
	FRONT_IMG_SIGGEN_PANA     = 6,
} frontSiggenSentype_t;
#else
typedef enum {
	FRONT_IMG_SIGGEN_NORM = 0,
	FRONT_IMG_SIGGEN_SONY,
	FRONT_IMG_SIGGEN_SHARP,
	FRONT_IMG_SIGGEN_PANA,
	FRONT_IMG_SIGGEN_FFM, /*5110 not support*/
	FRONT_IMG_SIGGEN_YUV
} frontSiggenSentype_t;
#endif

typedef enum {
	FRONT_IMG_SENSOR_BIT_14,
	FRONT_IMG_SENSOR_BIT_10,
	FRONT_IMG_SENSOR_BIT_12,
} frontSensorBit_t;


/**
 * \param  val
 *     bit[1:0] uvsel: 0=Cb, 1=Y1, 2=Cr, 3=Y2;
 *     bit[2]   selffielden: to generate 601 mode field signal (5110 revoked)
 *     bit[3]   fieldpol:  invert field signal to CDSP; (5210 revoked)
 *     bit[4]   bt656en:   0=601 mode, 1=656 mode;
 *     bit[6:5] tvsyncen:  0=TV decoding v:hvalid, 3=SSG v:hvalid;
 *     bit[7]   fldsvden:  field signal transition sync to VD; (5110 revoked)
 *     bit[8]   ys128en:   subtract 128 from Y;
 *     bit[9]   us128en:   subtract 128 from U;
 *     bit[10]  vs128en:   subtract 128 from V;
 */
enum {
	FRONT_TV_UVSEL_CB = 0,
	FRONT_TV_UVSEL_Y1,
	FRONT_TV_UVSEL_CR,
	FRONT_TV_UVSEL_Y2
};

#define FRONT_TV_601_FIELD_GEN   4
#define FRONT_TV_FIELD_INV       8
#define FRONT_TV_601             0
#define FRONT_TV_656             16
#define FRONT_TV_HSYNC_MASTER    32
#define FRONT_TV_VSYNC_MASTER    64
#define FRONT_TV_SYNC_MASTER     96
#define FRONT_TV_SYNC_SLAVE      0
#define FRONT_TV_LEVEL_SHIFT_Y   0x100
#define FRONT_TV_LEVEL_SHIFT_U   0x200
#define FRONT_TV_LEVEL_SHIFT_V   0x400

enum {
	FRONT_IMG_DIFF_PIXEL_2 = 0,
	FRONT_IMG_DIFF_PIXEL_1,
	FRONT_IMG_DIFF_PIXEL_4,
	FRONT_IMG_DIFF_PIXEL_6
};

#define FRONT_IMG_DIFF_OB_REF_MASK 0x01F0

/*=================================================================*/
UINT32 halFrontSerialCfgSet(UINT32 cfgSel,  UINT32 Value);

typedef enum {
	FRONT_SSC_PHY_CFG = 0x00000000,
	FRONT_SPI_PHY_CFG,
#if SPCA5110
	FRONT_SIF1_SYNC_CFG,
	FRONT_SIF2_SYNC_CFG,
	FRONT_SIF2_MODE_CFG, /* directly set mode */
#if SPCA5310
	FRONT_SIF2_PIN_CFG, /* SIF2 use TG12/13 or LMI8~ */
	FRONT_SIF2_SEN_CFG, /* polarity & cs */
	FRONT_SIF2_SPI_PHY_CFG, /* similar to FRONT_SPI_PHY_CFG */
	FRONT_SIF1_IDLE_CFG,/*idle time between SSC/SPI*/
	FRONT_SIF2_IDLE_CFG,/*idle time between SSC/SPI*/
#endif
	/*backward compatible*/
	FRONT_SSC_SYNC_CFG = FRONT_SIF1_SYNC_CFG,
	FRONT_SPI_SYNC_CFG = FRONT_SIF1_SYNC_CFG,
#else
	FRONT_SSC_SYNC_CFG,
	FRONT_SPI_SYNC_CFG,
#endif
} halFrontSerialCfgSetSel_t;
typedef enum {
	FRONT_SRL_SYNC_NOW,
	FRONT_SRL_SYNC_VD,
	FRONT_SRL_SYNC_VVALID,
	FRONT_SRL_SYNC_HDNUM0,
} halFrontSerialSync_t;

/*SPI*/ /*Bits 0:7*/
#define FRONT_SPI_SIF2     0x01

#define FRONT_SPI_MODE0    0x00
#define FRONT_SPI_MODE1    0x10
#define FRONT_SPI_MODE2    0x20
#define FRONT_SPI_MODE3    0x30
/*Also need to set TGGPIO Polarity for SPI_MODE1, see API xxTGGPIOxxxPolSet()*/
/*Bit 8:15 (0x9020) divider of 48MHz  SPI source clock = 48MHz */
/*Designers strongly suggesting to use values in the range of 16-254*/
/*Bit 16 SPI sensel (0x9070[0])*/
/*Bit 24:31 cdsini(0x900D)*/
#define FRONT_SPI_CLK_MASK    0xff00
#if SPCA5110
 #define SPI_INIT_MANU         0x08000000
 #define SPI_SEN_INIT_HI       0x01000000
 #define SPI_SEN_INIT_LO       0x00000000
 #define SPI_SCK_INIT_HI       0x02000000
 #define SPI_SCK_INIT_LO       0x00000000
 #define SPI_SDO_POS_MANU      0x80000000
 #define SPI_SDO_POS_SCK_LOW   0x00000000
 #define SPI_SDO_POS_SCK_FALL  0x10000000
 #define SPI_SDO_POS_SCK_HI    0x20000000
#else
 #define SPI_CLK_INIT_HI       0x01000000
 #define SPI_CLK_INIT_LO       0x00000000
 #define SPI_SDO_INIT_ENABLE   0x10000000
 #define SPI_SDO_INIT_DISABLE  0x00000000
 #define SPI_SDO_INIT_HI       0x20000000
 #define SPI_SDO_INIT_LO       0x00000000
#endif

/*SSC*/ /*Bit 0:7 (0x9008)*/
#define FRONT_SSC_RESTART_ON        2
#define FRONT_SSC_RESTART_OFF       0
#define FRONT_SSC_READ_NO_SUBADDR   0
#define FRONT_SSC_READ_HAS_SUBADDR  1
#if SPCA5110
 #define FRONT_SSC_READ              0x08
 #define FRONT_SSC_WRITE             0
 #define FRONT_SSC_BURST             0x04
 #define FRONT_SSC_NORMAL            0
 #define FRONT_SSC_NO_SUBADDR        0x10
 #define FRONT_SSC_HAS_SUBADDR       0
 #define FRONT_SSC_SUBADDR_16BIT     0x20
 #define FRONT_SSC_SUBADDR_8BIT      0
 #define FRONT_SSC_SLAVEADDR_8BIT    0
 #define FRONT_SSC_SIF1              0
 #define FRONT_SSC_SIF2              0x80

 #define FRONT_REG_SIFMODE_READ              0x02
 #define FRONT_REG_SIFMODE_BURST             0x01
 #define FRONT_REG_SIFMODE_BURST_READ_NOSTOP 0x04
 #define FRONT_REG_SIFMODE_SPI               0x08
 #define FRONT_REG_SIFMODE_SPI_MODE0         0x08
 #define FRONT_REG_SIFMODE_SPI_MODE1         0x09
 #define FRONT_REG_SIFMODE_SPI_MODE2         0x0a
 #define FRONT_REG_SIFMODE_SPI_MODE3         0x0b
#endif

#define FRONT_SSC_IS_BURST_READ_RESTART(cfg) ( ((cfg) & (FRONT_SSC_READ | FRONT_SSC_BURST | FRONT_SSC_RESTART_ON)) == (FRONT_SSC_READ | FRONT_SSC_BURST | FRONT_SSC_RESTART_ON) )

/*Bit 8:15 divider*/

typedef enum {   /* CPU clock = 48MHz */
	FRONT_SSC_CLK_24K = 0,
	FRONT_SSC_CLK_48K,
	FRONT_SSC_CLK_96K,
	FRONT_SSC_CLK_192K,
	FRONT_SSC_CLK_384K,
	FRONT_SSC_CLK_768K,
	FRONT_SSC_CLK_1500K,
	FRONT_SSC_CLK_3M
} halFrontSscClk_t;

/*Also setting corresponsive "stopdelay (0x9012)" and "i2cclkcetpos(0x9018)"*/
/*Bit 16~23 : Slave address*/
/*Bit 24~31 : Slave address hi*/

/*=================================================================*/
void halFrontSerialWrite(UINT32 port, UINT32 length, const UINT8 *pdata);
void halFrontSerialWriteExt(UINT32 port, UINT32 length, const UINT8 *pdata,
		UINT32 nosend);
/*length: number of register for SSC and number of bits for SPI*/

typedef enum {
#if SPCA5000
	FRONT_SRL_SIF1,
	FRONT_SRL_SIF2,
	FRONT_SRL_SSC1_SINGLE = FRONT_SRL_SIF1,
	FRONT_SRL_SSC2_SINGLE = FRONT_SRL_SIF2,
	FRONT_SRL_SSC1_BURST  = FRONT_SRL_SIF1,
	FRONT_SRL_SSC2_BURST  = FRONT_SRL_SIF2,
	/*backward compatible*/
	FRONT_SRL_SSC_SINGLE  = FRONT_SRL_SSC1_SINGLE,
	FRONT_SRL_SSC_BURST   = FRONT_SRL_SSC1_BURST,
	FRONT_SRL_SPI         = FRONT_SRL_SIF1,
#else
	FRONT_SRL_SSC_SINGLE,
	FRONT_SRL_SSC_BURST,
	FRONT_SRL_SPI,
#endif
} halFrontSrlPort_t;

/*=================================================================*/
/*this is obsolete*/
UINT32 halFrontSerialRead(UINT32 port, UINT32 addr, UINT32 length, UINT8* pdata);
/*use these 2*/
UINT32 halFrontSerialReadSet(UINT32 port, UINT32 addr, UINT32 length);
UINT32 halFrontSerialReadGet(UINT32 port, UINT32 length, UINT8* pdata);

void halFrontSerialWait(UINT32 port);
UINT32 halFrontSerialWaitTimeout(UINT32 port, UINT32 ms);

/*=================================================================*/

void halFrontFlashCfgSet(UINT32 cfgSel, UINT32 Value);

typedef enum {
	FRONT_FLASH_MODE = 0x00000000, /*value: halFrontFlashMode_t*/
	FRONT_FLASH_DELAY,
	FRONT_FLASH_DURATION,
	FRONT_FLASH_POLAR,
	FRONT_FLASH_DELAY_PIXEL,
} halFrontFlashCfgSetSel_t;

typedef enum {
	FRONT_FLASH_SYNC_NOW = 0,
	FRONT_FLASH_SYNC_VIEW_SUB,
	FRONT_FLASH_SYNC_CAP_SUB,
	FRONT_FLASH_SYNC_VIEW_H,
	FRONT_FLASH_SYNC_CAP_H
} halFrontFlashMode_t;

/*=================================================================*/

void halFrontMshCfgSet(UINT32 cfgSel, UINT32 Value);

typedef enum {
	FRONT_MSH_MODE = 0x00000000,
	FRONT_MSH_DELAY_LINE,
	FRONT_MSH_DELAY_PIXEL,
} halFrontMshCfgSetSel_t;

/*=================================================================*/
void halFrontPowerSave(UINT32 cfgSel,    UINT32 value);

typedef enum {
	FRONT_POWER_SHIFT_SEN_FPCLK = 0,
	FRONT_POWER_SHIFT_SEN_FPCLKNX = 4,
	FRONT_POWER_SHIFT_SIF_F48M = 5,
} halFrontPowerCfgSetShift_t;
typedef enum {
	FRONT_POWER_CCDTG = 0x01,
	FRONT_POWER_SYNCGEN = 0x02,
	FRONT_POWER_TVCTR = 0x04,
	FRONT_POWER_FLASH = 0x08,
	FRONT_POWER_BMAP_SEN_FPCLK = 0x0f,
	FRONT_POWER_CLK2X = 0x10,
	FRONT_POWER_BMAP_SEN_FPCLKNX = 0x10,
	FRONT_POWER_SSC = 0x20,
	FRONT_POWER_SPI = 0x40,
	FRONT_POWER_BMAP_SIF_F48M = 0x60,
	FRONT_POWER_BMAP_ALL = 0x7f,
} halFrontPowerCfgSetSel_t;

/*=================================================================*/
void halFrontRst(UINT32 cfgSel);

typedef enum {
	FRONT_RESET_FRONT = 0,
	FRONT_RESET_SIF1,
	FRONT_RESET_SIF2,
	FRONT_RESET_LMI,
	FRONT_RESET_SIFDMA,
	FRONT_RESET_TOTAL,
} halFrontRstCfgSetSel_t;

/*=================================================================*/
void halFrontCapIntervalSet(UINT32 val,UINT32 vdmask);

/*=================================================================*/
void halFrontSnapTrig(UINT32 snapNum);

/*=================================================================*/
void halFront2CdspCtr(UINT32 on,UINT32 sync);
UINT32 halFront2CdspGet(void);


void halCdspMasterClockGet(UINT32 *cdspclk,UINT32 *mclk);
UINT32 halPllFrontClkSet(UINT32 pclkSel, UINT32 mclk);



#ifdef __cplusplus
}
#endif

#endif  /* _HAL_FRONT_H_ */
