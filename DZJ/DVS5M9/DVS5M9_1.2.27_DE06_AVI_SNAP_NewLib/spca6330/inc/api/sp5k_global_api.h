/**************************************************************************
 *
 *       Copyright (c) 2002-2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author:
 *
 **************************************************************************/
#ifndef _SP5K_GLOBAL_API_H_
#define _SP5K_GLOBAL_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <time.h>
#include "common.h"
#include "middleware/common_types.h"	/*tmx_t*/
#include "middleware/global_def.h"
#include "middleware/sensor_def.h"
#include "middleware/cdsp_def.h"
#include "middleware/snap_def.h"
#include "middleware/media_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/*
 * sp5kGpioCfg(), sp5kButtonGpioCfg()
 */
typedef enum _sp5kGpioGrp_e {
	SP5K_GPIO_GRP_TGL    = GPIO_GRP_TGL,
	SP5K_GPIO_GRP_LMI    = GPIO_GRP_LMI,
	SP5K_GPIO_GRP_FML    = GPIO_GRP_FML,
	SP5K_GPIO_GRP_FMH    = GPIO_GRP_FMH,
	SP5K_GPIO_GRP_GEN    = GPIO_GRP_GEN,
	SP5K_GPIO_GRP_DISPL  = GPIO_GRP_DISPL,
	SP5K_GPIO_GRP_DISPH  = GPIO_GRP_DISPH,
	SP5K_GPIO_GRP_POCU   = GPIO_GRP_POCU,
	SP5K_GPIO_GRP_USB    = GPIO_GRP_USB,
#if SPCA5330
	SP5K_GPIO_GRP_SAR    = GPIO_GRP_SAR,
	SP5K_GPIO_GRP_OIS    = GPIO_GRP_OIS,
#endif
#if SPCA6330
	SP5K_GPIO_GRP_CEC    = GPIO_GRP_CEC,
#endif
	SP5K_GPIO_GRP_TOT,
	SP5K_GPIO_GRP_NO_USE = 0xffff,
} sp5kGpioGrp_e;
/* for backward compatible. */
#define SP5K_GPIO_GRP_DISP  SP5K_GPIO_GRP_DISPL

/*
 * sp5kGpioCfgSet()
 */
typedef enum _sp5kGpioDir_e {
	SP5K_GPIO_DIR_INPUT,
	SP5K_GPIO_DIR_OUTPUT,
} sp5kGpioDir_e;

/*
 * sp5kGpioIsrReg(), sp5kGpioIntEnable()
 */
typedef enum _sp5kGpioInt_e {
	SP5K_GPIO_INT0  =  4, /**< GPIO interrupt #0, mapping to TG GPIO[2]. */
	SP5K_GPIO_INT1  =  5, /**< GPIO interrupt #1, mapping to TG GPIO[3]. */
	SP5K_GPIO_INT2  =  6, /**< GPIO interrupt #2, mapping to General GPIO[7]. */
	SP5K_GPIO_INT3  =  7, /**< GPIO interrupt #3, K31 mapping to General GPIO[8], K33/V33 mapping to General GPIO[6] */
	SP5K_GPIO_INT4  =  9, /**< GPIO interrupt #4, mapping to General GPIO[14]. */
	SP5K_GPIO_INT5  = 10, /**< GPIO interrupt #5, mapping to General GPIO[15]. */
	SP5K_GPIO_INT6  = 11, /**< GPIO interrupt #6, mapping to General GPIO[9]. */
	SP5K_GPIO_INT7  =  0, /**< GPIO interrupt #7, K31 mapping to USBDET, K33/V33 mapping to DIGITV[17]. */
	SP5K_GPIO_INT8  =  1, /**< GPIO interrupt #8, mapping to FMGPIO[45]. */
	SP5K_GPIO_INT9  =  2, /**< GPIO interrupt #9, mapping to FMGPIO[48]. */
	SP5K_GPIO_INT10 =  3, /**< GPIO interrupt #10, mapping to FMGPIO[51]. */
	SP5K_GPIO_INT11 =  8, /**< GPIO interrupt #11, mapping to General GPIO[13]. */
} sp5kGpioInt_e;

#define SP5K_GPIO_INT_TOT  12

/*
 * sp5kCntrCfg()
 */
typedef enum _sp5kCntr_e {
	SP5K_CNTR0 = 0,
	SP5K_CNTR_TOT,
} sp5kCntr_e;

/*
 * sp5kButtonEnable()
 */
typedef enum _sp5kButtonType_e {
	SP5K_BUTTON_TYPE_GPIO   = 0,
	SP5K_BUTTON_TYPE_ADC    = 1,
	SP5K_BUTTON_TYPE_MATRIX = 2,
	SP5K_BUTTON_TYPE_ALL    = 0xffff,
} sp5kButtonType_e;

/*to enable/disable all the buttons in that button type*/
#define SP5K_BUTTON_ID_ALL  0xffff

/*
 * sp5kGpioIsrReg(), sp5kAdcIntEnable()
 */
typedef enum _sp5kTrigLvl_e {
	SP5K_TRIG_LVL_RISE = 1,
	SP5K_TRIG_LVL_FALL = 2,
	SP5K_TRIG_LVL_BOTH = 3,
} sp5kTrigLvl_e;

/*
 * sp5kHostMsgSend()
 */
typedef enum _sp5kHostMsg_e {
	SP5K_MSG_USER0      = 0,
	SP5K_MSG_USER_MAX   = SP5K_MSG_USER0,
	SP5K_MSG_USER_TOTAL = SP5K_MSG_USER_MAX + 1,
} sp5kHostMsg_e;

typedef enum _sp5kHostInitMsg_e {
	SP5K_MSG_HOST_INIT = 0,
} sp5kHostInitMsg_e;

/*
 * rtc
 */
typedef enum _sp5kRtcOption_e {
	SP5K_DATE_TIME_OPTION     = 0,
	SP5K_ALARM_OPTION         = 1,
	SP5K_WAKEUP_OPTION        = 2,
	SP5K_DATE_TIME_BLOCK_WAIT = 0x80000000,
} sp5kRtcOption_e;

typedef enum _sp5kRtcCurrent_e {
	SP5K_RTC_CURRENT_0_5_UA,
	SP5K_RTC_CURRENT_0_9_UA,
	SP5K_RTC_CURRENT_1_41_UA,
	SP5K_RTC_CURRENT_1_91_UA,
	SP5K_RTC_CURRENT_2_91_UA,
	SP5K_RTC_CURRENT_4_91_UA,
	SP5K_RTC_CURRENT_6_94_UA,
	SP5K_RTC_CURRENT_9_96_UA,
	SP5K_RTC_CURRENT_MAX,
} sp5kRtcCurrent_e;

/*
 * timer
 */
typedef enum sp5kTimerId_e {
	SP5K_TIMER_ID_0  = 0,
	SP5K_TIMER_ID_1  = 1,
	SP5K_TIMER_ID_2  = 2,
	SP5K_TIMER_ID_3  = 3,
	SP5K_TIMER_ID_4  = 4,
	SP5K_TIMER_ID_5  = 5,
	SP5K_TIMER_ID_6  = 6,
	SP5K_TIMER_ID_7  = 7,
	SP5K_TIMER_MAX   = SP5K_TIMER_ID_7,
	SP5K_TIMER_TOTAL = SP5K_TIMER_MAX + 1,
} sp5kTimerId_e;

/*
 * time delay
 */
typedef enum _sp5kTimeDelay_e {
	SP5K_TIME_DELAY_100NS = 0,
	SP5K_TIME_DELAY_1US   = 1,
	SP5K_TIME_DELAY_10US  = 2,
	SP5K_TIME_DELAY_1MS   = 3,
} sp5kTimeDelay_e;

typedef enum _sp5kPowerCtrl_e {
	SP5K_POWER_OFF         = 0,
	SP5K_POWER_ON          = 1,
	SP5K_POWER_OFF_POLLING = 2,
} sp5kPowerCtrl_e;

#define SP5K_SCAN_IO_MAX  4

/*
 * sp5kPwrDetectGet()
 */
#define SP5K_PWR_SRC_0				0x00000001
#define SP5K_PWR_SRC_1				0x00000002
#define SP5K_PWR_SRC_2				0x00000004
#define SP5K_PWR_SRC_USB			0x00000008
#define SP5K_PWR_SRC_RTC			0x00010000
#define SP5K_PWR_RISE_EVT_0			0x00000100
#define SP5K_PWR_RISE_EVT_1			0x00000200
#define SP5K_PWR_RISE_EVT_2			0x00000400
#define SP5K_PWR_RISE_EVT_USB		0x00000800
#define SP5K_PWR_FALL_EVT_0			0x00001000
#define SP5K_PWR_FALL_EVT_1			0x00002000
#define SP5K_PWR_FALL_EVT_2			0x00004000
#define SP5K_PWR_FALL_EVT_USB		0x00008000
#define SP5K_PWR_SRC_BAK_0			0x01000000
#define SP5K_PWR_SRC_BAK_1			0x02000000
#define SP5K_PWR_SRC_BAK_2			0x04000000
#define SP5K_PWR_SRC_BAK_USB		0x08000000

/*
 * sp5kOisCfgSet()
 */
typedef enum _sp5kOisCfg_e {
    SP5K_OIS_CFG_SAMPLING_FREQ = 0,
    SP5K_OIS_CFG_CALLBACK_FUNCTION,
    SP5K_OIS_CFG_CALLBACK_THRESHOLD,
    SP5K_OIS_CFG_BUFFER_SIZE_RATIO,
    SP5K_OIS_CFG_PSEUDO_AUTO_CHANNEL_PATTERN,
    SP5K_OIS_CFG_PSEUDO_AUTO_START_CHANNEL,
    SP5K_OIS_CFG_PSEUDO_AUTO_ASYNC_READ,
} sp5kOisCfg_e;

/*
 * sp5kOisControl()
 */
typedef enum _sp5kOisCtrl_e {
	 SP5K_OIS_CTRL_STOP = 0,
	 SP5K_OIS_CTRL_START,
} sp5kOisCtrl_e;

/*
 * sp5kOisEnable()
 */
typedef enum _sp5kOisEnable_e {
	 SP5K_OIS_DISABLE = 0,
	 SP5K_OIS_ENABLE_AUTO,
	 SP5K_OIS_ENABLE_PSEUDO_AUTO,
} sp5kOisEnable_e;

/*
 *
 */
typedef enum _sp5kSysHaltOperate_e {
	SP5K_SYS_HALT_DEBUG, /* software watchdog timer */
	SP5K_SYS_HALT_GOTO_CMDLINE,
	SP5K_SYS_HALT_DELAY_POWEROFF,
	SP5K_SYS_HALT_IMMEDIATE_POWEROFF,
	SP5K_SYS_HALT_DELAY_REBOOT,
	SP5K_SYS_HALT_IMMEDIATE_REBOOT,
} sp5kSysHaltOperate_e;

/*
 * sp5k_sys_cfg_xxx.c
 */
/*paramId*/
typedef enum _sp5kSystemCfg_e {
	SP5K_SERVICE_TIME_COUNT   = 0, /* obsolete */
	SP5K_AF_GAMMA_TYPE        = 1,
	SP5K_AF_FILTER_WEIGHT     = 2,
	SP5K_CDSP_INFO_CFG_GROUP  = 3, /*param1:cdspInfoCfg_t in "cdsp_def.h"*/
	SP5K_SYS_CFG_CDSP_RGB_GAMMA_TYPE = 4,
	SP5K_CNTR_ISR_LPF         = 5, /* obsolete */
	SP5K_SENSOR_DEV_MODEL_CFG, /*param1:sensorDevCfg_t in "sensor_def.h"*/
	SP5K_SENSOR_SYS_CFG, /*param1:sensorStsCfg_t in "sensor_def.h"*/
	SP5K_SNAP_SYS_CFG, /*param1:snapStsCfg_t in "snap_def.h"*/
	SP5K_FIXED_PREV_DZ_CDSP_ASPECT,
	SP5K_AF_SYS_CFG,
	SP5K_USB_FULL_SPEED_CFG, /*param1: 1 => USB full speed, 0 => USB high speed*/
	SP5K_DISK_CALLBACK_CFG, /*param1: 1 => enable disk mount callback, 0 => disable*/
	SP5K_CDSP_FLIP_CFG, /*param1:cdspFlipVal_t in "cdsp_def.h",0: OFF, 1: H-flip, 2: V-flip, 3: HV-flip */
	SP5K_CDSP_AEAF_SW_FLIP_CFG, /*param1:cdspAaSwFlipSel_t, param2:cdspFlipVal_t in "cdsp_def.h"*/
	SP5K_DISP_TV_WINDOW_CFG, /* obsolete *//*param1:x0, param2:y0, param3:x1, param4:y1*/
	SP5K_CDSP_SYS_CFG,
	SP5K_BKPV_SKIP_FRAME_CFG, /*param1: frame count to skip*/
	SP5K_CDSP_COFILTER_EOF_CFG, /* obsolete from 5310 */
	SP5K_COLOR_KEY_Y_TH_SET,/*param1: low threshold of y value*/
	SP5K_COLOR_KEY_Y_TH_GET,/*param1: low threshold of y value*/
	SP5K_EXIF_CUSTOM_RTC_CFG, /*param1: pointer to datetime string*/
	SP5K_NAND_WP_DELAY_CFG, /*param1: delay count*/
	SP5K_MODESW_DUP_DEFAULT_CFG, /*param1: 0 => clear standby, 1 => backup last frame*/
	SP5K_VFS_FAT32_2GB_SD_CFG, /*param1: 1 => support FAT32 on 2GB SD, 0 => not support*/
	SP5K_EXIF_MAKERNOTE_SIZE_CFG, /*param1: max size of exif makernote*/
	SP5K_CDSP_MAX_LINE_WIDTH_CFG,/*obsolete?*/
	SP5K_DISP_SYS_CFG,
	SP5K_DISP_2ND_PATH_CFG,/*no function?*/
	SP5K_DISK_SD_HW_PIN_CFG,
	SP5K_MEM_SMALL_POOL_SIZE_CFG, /*param1: max size of small pool in bytes*/
	SP5K_RSV_THM_SIZE_CHECK_CFG, /* obsolete *//*param1: 1 => check thm size (default), 0 => not */
	SP5K_SOC_HW_VER_CFG, /*param1: 0=ver A, 1=ver B, only for cfg get*/
	SP5K_DRAMCLK_SWITCH_CFG, /*param1: 0=disable, 1=enable dramclk switch under snap*/
	SP5K_FAST_SENSOR_INIT_CFG, /* obsolete *//*param1: 0=normal, 1=fast sensor init*/
	SP5K_PWRMOS_SWITCH_CFG, /*param1: 0=disable, 1=enable card power MOS switch*/
	SP5K_MEM_ALLOC_DEBUG_PTR_CFG, /*param1: the address dump by os mem */
	SP5K_PERIPHERAL_SYS_CFG,
	SP5K_STDANDBY_FRAME_SIZE_CFG, /* param1: standby frame width, param2: standby frame height*/
	SP5K_BUTTON_SYS_CFG, /* param1: Set ADC Button release debounce,unit 20ms*/
	SP5K_DISK_SECTOR_NUMS_CFG,
	SP5K_NAND_MAP_TABLE_CFG,
	SP5K_SD_RW_DELAY_CFG,
	SP5K_SD_CLK_PHASE_CFG,
	SP5K_SD_BUS_SPEED_CFG,
	SP5K_SD_BUS_WIDTH_CFG,
	SP5K_SD_MEDIA_TYPE_CFG,
	SP5K_SD_IO_MODE_CFG,
	SP5K_CLK_CPU_DIV_CFG,
	SP5K_CLK_CPU_CLK_CFG,
	/*
	 * param1: sp5kSysHaltOperate_e
	 * param2: watchdog timer interval (us).
	 * param3: delay time (s) for power off.
	 */
	SP5K_SYS_HALT_OPERAT_CFG,
	/*
	 * param1[0:1]width factor, 0 - 1x, 1 - 1/2, 2 - 1/4.
	 * param1[16:17] height factor, 0 - 1x, 1 - 1/2, 2 - 1/4.
	 */
	SP5K_CDSP_2NDPATH_FACTOR_CFG,
	SP5K_CDSP_2NDPATH_HOST_EN_CFG,
	SP5K_AEAWB_WIN_NOCROP,

	/* force turn off h264 clock to save power (we're going to power off) */
	SP5K_H264_PV_CLK_FORCE_OFF_CFG,
	/*SDIO port SD Card configure*/
	SP5K_SDIO_RW_DELAY_CFG,
	SP5K_SDIO_CLK_PHASE_CFG,
	SP5K_SDIO_BUS_SPEED_CFG,
	SP5K_SDIO_BUS_WIDTH_CFG,
	SP5K_SDIO_MEDIA_TYPE_CFG,
	SP5K_SDIO_FUNC_NUM,
	SP5K_SDIO_CMD53_READ_NORSP,

	SP5K_SPI_1BIT_MAX_FREQ,
	SP5K_SPI_4BIT_MAX_FREQ,
	SP5K_MEDIA_SYS_CFG, /* defined in media_def.h */
} sp5kSystemCfg_e;

/*
 * sp5kSystemCfgGet(SP5K_CDSP_FLIP_CFG,...)
 */
typedef enum {
	 SP5K_FLIP_VAL_NOFLIP = CDSP_FLIP_VAL_NOFLIP,
	 SP5K_FLIP_VAL_HFLIP,
	 SP5K_FLIP_VAL_VFLIP,
	 SP5K_FLIP_VAL_BOTHFLIP,
	 SP5K_FLIP_VAL_TOTAL,
} sp5kFlipCfg_t;

/*
 * sp5kSystemCfgGet(SP5K_CDSP_AEAF_SW_FLIP_CFG,...)
 */
typedef enum {
	 SP5K_AA_SW_FLIP_CFG_ALL = CDSP_AA_SW_FLIP_CFG_ALL,
	 SP5K_AA_SW_FLIP_CFG_AEWIN,
	 SP5K_AA_SW_FLIP_CFG_AFWIN,
	 SP5K_AA_SW_FLIP_CFG_TOTAL
} sp5kAaSwFlipCfg_t;

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/* For SP5K callback phototype */
typedef UINT32 (*fpsp5kCallback_t)(UINT32, UINT32, void *);
typedef UINT32 (*fpsp5kVoidCallback_t)(void);
typedef UINT32 (*sp5kCntrCallback_t)(void);
typedef void (*fpsp5kOisCallback_t)(UINT16 *buf[4], UINT32 readyOffset);
typedef UINT32 (*fpSp5kMsgCallback_t)(UINT32 *msgId, UINT32 *param); /* it should be with same type of fpMsgCallback_t. The def should be from middleware .h */

typedef struct sp5kIoScan_s {
	UINT32 group;
	UINT32 pinNum;
	UINT32 valIni;
	UINT32 expireMs;
	UINT32 msgCmd[2];
	UINT32 msgParam[2];
	void (*pfunc[2])(UINT32);
} sp5kIoScan_t;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* sp5k_adc.c */
UINT32 sp5kAdcInit(void);
UINT32 sp5kAdcRead(UINT32 chnlId, UINT32 *pval);
void   sp5kAdcEnable(UINT32 en); /* added by Edward 20071119 */
UINT32 sp5kOisCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kOisEnable(UINT32 enable);
UINT32 sp5kOisControl(UINT32 ctrl);
UINT32 sp5kOisDataCountRead();

/* sp5k_button_xxx.c */
UINT32 sp5kButtonInit(UINT32 gpioBtnTot, UINT32 gpioMtxBtnTot, UINT32 adcBtnTot);
UINT32 sp5kButtonGpioInit(UINT32 gpioBtnTot);
UINT32 sp5kButtonAdcInit(UINT32 adcBtnTot);
UINT32 sp5kButtonMatrixInit(UINT32 mtxPinTot);
UINT32 sp5kButtonGpioCfgSet(UINT32 btnId, UINT32 grpId, UINT32 pinId, UINT32 polar, UINT32 debnc, UINT32 repeat);
UINT32 sp5kButtonAdcCfgSet(UINT32 btnId, UINT32 chnlId, UINT32 loBnd, UINT32 hiBnd, UINT32 debnc, UINT32 repeat);
UINT32 sp5kButtonEnable(UINT32 btnType, UINT32 btnId, UINT32 en);
UINT32 sp5kButtonMatrixCfgSet(UINT32 ,UINT32 ,UINT32 ,UINT32 );
UINT32 sp5kButtonMatrixPinSet(UINT32 ,UINT32 ,UINT32 ,UINT32 );

/* sp5k_cntr.c */
UINT32 sp5kCntrCfgSet(UINT32 cntrId, UINT32 pinId, UINT32 trigLvl, UINT32 sampPeriod);
UINT32 sp5kCntrTargetValSet(UINT32 cntrId, UINT32 trgVal);
UINT32 sp5kCntrEnable(UINT32 cntrId, UINT32 en);
UINT32 sp5kCntrRead(UINT32 cntrId, UINT32 *pval);
/* Callback functions */
void sp5kCntrCallbackSet(sp5kCntrCallback_t);

/* gpio.c */
#define sp5kGpioCfgSet gpioCfgSet /*(UINT32 grpId, UINT32 pinMsk, UINT32 dir);*//*dir:0=in, 1=out*/
#define sp5kGpioRead   gpioRead     /*(UINT32 grpId, UINT32 pinMsk, UINT32 *pval);*/
#define sp5kGpioWrite  gpioWrite   /*(UINT32 grpId, UINT32 pinMsk, UINT32 val);*/
#define sp5kGpioPullSet	gpioPullSet	/*(UINT32 grpId, UINT32 pinMsk, UINT32 enable);*/
#define sp5kGpioDirSet gpioDirSet /*(UINT32 grpId, UINT32 pinMsk, UINT32 dir);*//*dir:0=in, 1=out*/
#define sp5kGpioFuncSet gpioFuncSet /*(UINT32 grpId, UINT32 pinMsk, UINT32 val);*//*val:0=func, 1=io*/
#define sp5kGpioDrvSet gpioDrvSet /*(UINT32 grpId, UINT32 pinMsk, UINT32 hdw, UINT32 ldw);*//*hdw,ldw:0~3*/
UINT32  sp5kGpioIsrReg(UINT32 intId, UINT32 intLvl, void *isr);
UINT32  sp5kGpioIsrUnreg(UINT32 intId, UINT32 intLvl);
UINT32  sp5kGpioIntEnable(UINT32 intId, UINT32 en);

/* sp5k_pwm.c */
UINT32 sp5kPwmCfgSet(UINT32 chnlId, UINT32 freqDevider, UINT32 duty);
/**
 @brief Extenstion configure PWM output setting.
 For K33/V33, 1 tick for PWM tickBase is (1/48MHz) = 20.83 ns.
 With this API, you can control PWM frequence (Hz) ONLY by (lowTicks+highTicks).
 Min=0.
 Max=4095 for chnlId==0, Max=2047 for the others.
 @param[in] id ID of PWM.
 @param[in] lowTicks count of LOW (Min=0. Max=4095 for chnlId==0, Max=2047 for the others)
 @param[in] highTicks count of HIGH (Min=0. Max=4095 for chnlId==0, Max=2047 for the others)
 @retval FAIL Fail
 @retval SUCCESS Success
 */
UINT32 sp5kPwmCfgSetExt(UINT32 chnlId, UINT32 lowTicks, UINT32 highTicks);
UINT32 sp5kPwmTrig(UINT32 startMsk, UINT32 stopMsk);

/* sp5k_msg.c */
UINT32 sp5kHostMsgSend( UINT32, UINT32 );
UINT32 sp5kHostMsgReceive(UINT32 *,UINT32 *);
UINT32 sp5kHostMsgFilterCallbackSet( fpSp5kMsgCallback_t );

/* sp5k_rtc.c */
UINT32 sp5kRtcReliableGet(UINT32 *);
UINT32 sp5kRtcReliableSet(UINT32);
UINT32 sp5kRtcDateTimeGet(UINT32, tmx_t *);
UINT32 sp5kRtcDateTimeSet(UINT32, tmx_t *);
UINT32 sp5kRtcExtraInfoGet(UINT8 *);
UINT32 sp5kRtcExtraInfoSet(UINT8 *);
UINT32 sp5kRtcCurrentSet(UINT32 current);
void   sp5kCustomTimeCheckCallbackSet(fpTimeCallback_t);
void   sp5kRtcSwEmulationSet(UINT32 enable);

/* sp5k_timer.c */
UINT32 sp5kTimerCfgSet(UINT32 ,UINT32);
UINT32 sp5kTimerCfgGet(UINT32 ,UINT32 *,UINT32 *);
UINT32 sp5kTimerEnable(UINT32,UINT32);
UINT32 sp5kTimeDelay(UINT32, UINT32);
UINT32 sp5kTimerIsrReg(UINT32 ,UINT32 ,void *);
UINT32 sp5kTimerIsrOneShotReg(UINT32 dontCare,UINT32 ms,void *pfunc);
UINT32 sp5kTimerUsOneShotReg(UINT32 dontCare, UINT32 us, void *pfunc);
UINT32 sp5kTimerUsIsrReg(UINT32 repeat, UINT32 us, void *pfunc);
UINT32 sp5kMsTimeGet(void);

/* sp5k_version.c, firmware version */
UINT32 sp5kSpcaVersionGet(UINT8 *);

/* sp5k_ioscan.c */
UINT32 sp5kScanRegister(sp5kIoScan_t *pcfg);

/* sp5k_sys_cfg_xxx.c */
UINT32 sp5kSystemCfgSet(UINT32 paramId, ...);
UINT32 sp5kSystemCfgGet(UINT32 paramId, ...);

/*splitting from sp5kSystemCfgS/Get()*/
UINT32 sp5kDiskCfgGet(UINT32 paramId, ...);
UINT32 sp5kDiskCfgSet(UINT32 paramId, ...);
UINT32 sp5kDiskSdHwPinCfgSet(UINT32 sdio);
UINT32 sp5kDiskSdHwPinCfgGet(UINT32 *sdio);
UINT32 sp5kFsFat32On2GbSdSet(UINT32 en);

/**
 * \note pwron0~2 status:
 * bit[0:2]=status, bit[8:10]=rising evt, bit[12:14]=falling evt
 * bit[16]=rtc wakeup
 * bit[24:26]=backup status at power on
 * check SP5K_PWR_SRC_NORMAL and others definitions above
 */
UINT32 sp5kPwrDetectGet(UINT32 msk, UINT32 *pval);

/**
 * \brief enable/disable spread spectrum
 * \param[in] en 1:enable, 0:disable
 * \return void
 */
void sp5kGlobSSEnableSet(UINT32 en);

/**
 * \brief get the status of spread spectrum
 * \param[in] void
 * \return 0:disable, 1:enable
 */
UINT32 sp5kGlobSSEnableGet(void);

/**
 * \brief configure clock divider, modulation clock and rate of spread spectrum
 * \param[in] clkDiv the clock select, 0:9MHz, 1:6.75MHz
 * \param[in] modClk the modulation clock from 0 (/8), 1 (/12) to 14 (/64)
 * \param[in] modRate the modulation rate from 0(+1f), 1(+-1f) to 15 (+-15f)
 * \return void
 */
void sp5kGlobSSCfgSet(UINT32 clkDiv, UINT32 modClk, UINT32 modRate);

/**
 * \brief get clock divider, modulation clock and rate of spread spectrum
 * \param[out] pclkDiv the clock select, 0:9MHz, 1:6.75MHz
 * \param[out] pmodClk the modulation clock from 0 (/8), 1 (/12) to 14 (/64)
 * \param[out] pmodRate the modulation rate from 0 (+1f), 1 (+-1f) to 15 (+-15f)
 * \return void
 */
void sp5kGlobSSCfgGet(UINT32 *pclkDiv, UINT32 *pmodClk, UINT32 *pmodRate);

void sp5kCmdExec(char *cmd);

#endif  /* _SP5K_GLOBAL_API_H_ */

