/**************************************************************************
 *
 *        Copyright (c) 2004-2008 by Sunplus mMedia Inc., Ltd.
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
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Matt Wang
 *
 **************************************************************************/
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "middleware/disp_def.h"
#include "middleware/sensor_def.h"
#include "middleware/proflog.h"
#include "api/sp5k_disk_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_usb_api.h"
#include "api/sp5k_msg_def.h"
#include "customization/dev_init.h"
#include "customization/app_init.h" /*for fastboot hostInit()*/

/**************************************************************************
 *                             C O N S T A N T S
 **************************************************************************/
/* User has to make sure the GPIO setting is correct */
#define DISK_ENUM_SPLIT  1  /*fastboot*/

/**************************************************************************
 *                     E X T E R N A L   R E F E R E N C E
 **************************************************************************/
UINT32 diskDevAlloc(UINT32 devMax);
UINT32 diskNandOpen(void);
UINT32 diskSpiOpen(void);
UINT32 diskSdOpen(void);

UINT32 dispCompositeTvNtscOpen(void);
UINT32 dispCompositeTvPalOpen(void);
void sioCtrl(UINT32 opt);

extern void diskScan(UINT32 drive);
extern void hostMsgMaxNumSet(UINT32 num);
#define APP_VERSION				1/*VQ*/
#define APP_SUB_VERSION			72	/*VQ*/
#define APP_REVISION			0	/*VQ*/

#define _QUOTE(x)	#x
#define QUOTE(x)	_QUOTE(x)
UINT8 signature[64]		__attribute__ ((section (".signature")))=
	APP_MODEL " v" QUOTE(APP_VERSION) "." QUOTE(APP_SUB_VERSION) "." QUOTE(APP_REVISION) "@" HW_MODEL;

UINT32 bUsbPowerOn  __attribute__((section(".signature"))) = 0x0;  
/**************************************************************************
 *                 F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void devDispEnum(void);
#if DISK_ENUM_SPLIT==0
void devDiskEnum(void);
#else
void devDiskEnumBoot(void);
void devDiskEnumMedia(void);
#endif
void devFrontEnum(void);
void devAudioEnum(void);
void devKeyBtnEnum(void);
void devUsbEnum(void);
void devOtgEnum(void);

/**************************************************************************
 *                           G L O B A L   D A T A
 **************************************************************************/
static sp5kIoScan_t diskIoCfg = {
	.group    = SP5K_SD_DETECT_GRP,
	.pinNum   = SP5K_SD_DETECT_ID,
	.valIni   = !SP5K_SD_DETECT_POL,
	.expireMs = 500/*1000*/,
	.msgCmd   = {SP5K_MSG_DISK_REMOVAL, SP5K_MSG_DISK_INSERT},
	.msgParam = {2, 2}, /* DRIVE_SD */
	.pfunc    = {diskScan, NULL},
};

static sp5kIoScan_t usbIoCfg = {
	.group    = SP5K_VBUS_DETECT_GRP,
	.pinNum   = SP5K_VBUS_DETECT_ID,
	.valIni   = !SP5K_VBUS_DETECT_POL,
	.expireMs = 1000,
	.msgCmd   = {SP5K_MSG_USB_DEVICE_VBUS_IN, SP5K_MSG_USB_DEVICE_VBUS_OFF},
	.msgParam = {0, 0},
	.pfunc    = {NULL, NULL},
};

/**************************************************************************
 *                                                                        *
 * Function Name: devInit                                                 *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *                                                                        *
 * Returns:                                                               *
 *                                                                        *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/
void devRtcInit(void);
void
devInitPrimary(
	void
)
{
	UINT32 err;
	codeSentry(2);
	devRtcInit();
	sioCtrl(0);  /* 1:off, 0:on */
	appLensFastBootInit();
#if DISK_ENUM_SPLIT==0
	devDiskEnum();
#else
	devDiskEnumBoot();
#endif

#ifdef HOST_MSG_MAX_SIZE
	hostMsgMaxNumSet(HOST_MSG_MAX_SIZE);
#endif
	hostInit();

#if DISK_ENUM_SPLIT
}
void
devInitSecondary(
	void
)
{
	/*
	 * media disk driver enum.
	 */
	devDiskEnumMedia();
	
#endif
	devDispEnum();
	#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO 
	devAudioEnum();
	#else
	
	audioCodecInit();
	#endif
	
	#if SPCA5330
	audioCodec2Init();	/* secondary audio device. */
	#endif
    
	devKeyBtnEnum();
	devUsbEnum();
	
}

#if DISK_ENUM_SPLIT==0
void devInitSecondary(void) { }
#endif

/**************************************************************************
 *
 * Function Name: devDispEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devDispEnum(
	void
)
{
	UINT32 handle;

	/* display module init */

	#if (KIT_WITH_HDMI)
	dispDevAlloc(4);
	#else
	dispDevAlloc(3);
	#endif
	#if 0
	/* device 1: lcd panel. */
	/* Register the callback function for HI/LO operation for SPI */
	spiCtrlFunSet(SPI_CTRL_CLK, spiUserCtrlCLK);
	spiCtrlFunSet(SPI_CTRL_DAT, spiUserCtrlDAT);
	spiCtrlFunSet(SPI_CTRL_CS, spiUserCtrlCS);
	#endif
	handle = dispLcdInit();

	dispDevActSet(handle);
	dispPowerOn(handle);
	dispTvNtscOpen();  /* device 2: composite tv ntsc. */
	dispTvPalOpen();   /* device 3: composite tv pal. */
	#if SPCA6330
	dispHdmiOpen();    /* device 4: hdmi tv. */
	#endif
	#if 0
	sp5kDispCallbackReg(4);
	#endif
}

/**************************************************************************
 *                                                                        *
 * Function Name: devDiskSdCallBack                                       *
 *                                                                        *
 * Purposes:                                                              *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *   None                                                                 *
 * Returns:                                                               *
 *   None                                                                 *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/
BOOL IS_EYEFI=0;
void
devDiskSdCallBack(
	UINT32 type,
	UINT8 *pCid,
	UINT32 num
)
{
#define DRIVE_SD		2
#define IO_CFG_WRITE	1
#define IO_SPEED_NORMAL	(1 << 1)

#define SD_TYPE_V11		1
#define SD_TYPE_V20		2
#define SD_TYPE_SDHC	3

	vfsDevIoCtrl(DRIVE_SD, IO_CFG_WRITE, 0);

	if (pCid != NULL && num == 16) 
	{
		/* Edward 20080814 fix VCA340Z SPT-0801 Transcend SD 80x 1GB card */
		/* CID:1C 53 56 53 44 43 20 20 10 00 00 06 C3 00 85 53 */
		if (type == SD_TYPE_V20 && pCid[0] == 0x1c && pCid[1] == 0x53 && pCid[2] == 0x56 &&
			pCid[13] == 0x00 && pCid[14] == 0x85) 
		{
			vfsDevIoCtrl(DRIVE_SD, IO_CFG_WRITE, IO_SPEED_NORMAL);
		}
		#if 0
		if (type == SD_TYPE_SDHC){
			if (pCid[0] == 0x41 && pCid[1] == 0x34 && pCid[2] == 0x32 && pCid[13] == 0x00 && pCid[14] == 0x84){
				vfsDevIoCtrl(DRIVE_SD, IO_CFG_WRITE, IO_SPEED_NORMAL);
			}
		}
		#endif
		if ( pCid[3]==0x45 && pCid[4]==0x59 && pCid[5]==0x45 && pCid[6]==0x46 && pCid[7]==0x49)
		{
			printf("\n    leo test     IS_EYEFI       \n");
			 IS_EYEFI=1;
			UINT32 cfg;
			vfsDevIoCtrl(DRIVE_SD, IO_CFG_WRITE, IO_SPEED_NORMAL);
			sp5kSystemCfgGet(SP5K_DISK_SD_HW_PIN_CFG, &cfg);
			/* set driving to high */
			sp5kSystemCfgSet(SP5K_DISK_SD_HW_PIN_CFG, cfg | 0x02);/* driving to high */
			sdRwDly(0x02, 5000, 5000);
		}
	}
}

/**************************************************************************
 *
 * Function Name: devDiskEnum{Boot,Media}
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
#if DISK_ENUM_SPLIT==0
void
devDiskEnum(
#else
void
devDiskEnumBoot(
#endif
	void
)
{
	#if ( SP5K_DISK_NAND == 1 && SP5K_DISK_SPI == 1)
	SP5K_DISK_TOTAL = SP5K_DISK_TOTAL - 1; /* SPI and NAND do not exist together */
	#endif
	UINT32 devNr = SP5K_DISK_TOTAL; /* tiwu: how many? */

	printf("Total Dev [%d]\n", devNr);
	diskDevAlloc(devNr); /* maximum storage device option */

	#if ( SP5K_DISK_NAND == 1 )
	diskNandOpen();
	#elif ( SP5K_DISK_SPI == 1 )
	diskSpiOpen();
	#endif
#if DISK_ENUM_SPLIT
}

void
devDiskEnumMedia(
	void
)
{
#endif

	#if (SP5K_DISK_RAM == 1 )
		sp5kDiskRamAttrSet(0x00600000);/* 6MB */
		diskRamOpen();
	#endif

	#if ( SP5K_DISK_SD == 1 )
		diskSdOpen();
		/* Initial the GPIO */
		sp5kDiskDriverPinCfg(
			SP5K_DRIVE_SD,SP5K_DISK_DETECT_PIN,
			SP5K_SD_DETECT_GRP,
			SP5K_SD_DETECT_ID,
			SP5K_SD_DETECT_POL);
		#ifdef SP5K_SD_PWREN_GRP
			sp5kDiskDriverPinCfg(
				SP5K_DRIVE_SD,SP5K_DISK_POWER_ON_PIN,
				SP5K_SD_PWREN_GRP,
				SP5K_SD_PWREN_ID,
				SP5K_SD_PWREN_POL);
		#endif
		#ifdef SP5K_SD_WRP_GRP
			sp5kDiskDriverPinCfg(
				SP5K_DRIVE_SD,SP5K_DISK_WRITE_PROTECT_PIN,
				SP5K_SD_WRP_GRP,
				SP5K_SD_WRP_ID,
				SP5K_SD_WRP_POL);
		#endif
	#endif
	
	#if ( SP5K_DISK_MS == 1 )
		diskMsOpen();
		/* Initial the GPIO */
		sp5kDiskDriverPinCfg(
			SP5K_DRIVE_MS,SP5K_DISK_DETECT_PIN,
			SP5K_SD_DETECT_GRP,
			SP5K_SD_DETECT_ID,
			SP5K_SD_DETECT_POL);
		sp5kDiskDriverPinCfg(
			SP5K_DRIVE_MS,SP5K_DISK_POWER_ON_PIN,
			SP5K_SD_PWREN_GRP,
			SP5K_SD_PWREN_ID,
			SP5K_SD_PWREN_POL);
	#endif
	
	#if ( SP5K_DISK_XD == 1 )
		diskxDOpen();
		/* Initial the GPIO */
		sp5kDiskDriverPinCfg(
			SP5K_DRIVE_XD,SP5K_DISK_DETECT_PIN,
			SP5K_SD_DETECT_GRP,
			SP5K_SD_DETECT_ID,
			SP5K_SD_DETECT_POL);
		sp5kDiskDriverPinCfg(
			SP5K_DRIVE_XD,SP5K_DISK_POWER_ON_PIN,
			SP5K_SD_PWREN_GRP,
			SP5K_SD_PWREN_ID,
			SP5K_SD_PWREN_POL);
	#endif
	
	#if ( SP5K_DISK_NAND == 1 )
		sp5kHostMsgSend(SP5K_MSG_DISK_INSERT, SP5K_DRIVE_NAND);
	#endif
	
	sp5kScanRegister(&diskIoCfg);
}

/**************************************************************************
 *
 * Function Name: devFrontEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devFrontEnum(
	void
)
{
/*shutter, if applicable, must init before sensor service installed*/
	sensorMshInit();


/*select sensor TG master clock source, select either ONE of the 3*/
#if SP5K_FRONT_MCLK_SRC_NOT_CONNECT /*sensor/XTG has own MCLK*/
	sensorMclkSrcCfgSet(SENSOR_MCLK_SRC_NOT_CONNECT);
#elif SP5K_FRONT_MCLK_SRC_SP5K_TGPLL
	sensorMclkSrcCfgSet(SENSOR_MCLK_SRC_SP5K_TGPLL);
#elif SP5K_FRONT_MCLK_SRC_ADCLPI_PAD
	sensorMclkSrcCfgSet(SENSOR_MCLK_SRC_ADCLPI_PAD);
#endif

/*select sensor SyncSigGen clock source master, select either ONE of the 2
 *NOTE: not all the sensor drivers support both modes
 *such sensor drivers will ignore this setting
 */
#if SP5K_FRONT_SYNC_SIG_SP5K_MASTER
	sensorSyncSigMasterCfgSet(SENSOR_SYNC_SIG_SP5K_MASTER);
#elif SP5K_FRONT_SYNC_SIG_SP5K_SLAVE
	sensorSyncSigMasterCfgSet(SENSOR_SYNC_SIG_SP5K_SLAVE);
#endif

	/*macros defined in macro_expand.h*/
	devFrontTgInstall();
	devFrontAfeInstall();
	devFrontTvInDecInstall();
}
#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO 

#include "middleware/mixer_def.h"
/**************************************************************************
 *                                                                        *
 * Function Name: devAudioEnum                                            *
 *                                                                        *
 * Purposes:                                                              *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *   None                                                                 *
 * Returns:                                                               *
 *   None                                                                 *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/
void
devAudioEnum(
	void
)
{
	mixerPlayIni_t ini;

	ini.pdevName  = "MIXER";
	ini.devId     = 0;
	ini.ioId      = 0;
	ini.usBufLen  = 40000;
	ini.subTrkTot = 1;
	mixerPlayOpen(&ini);

	audioCodecInit();
}
#endif
/**************************************************************************
 *
 * Function Name: devKeyBtnEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devKeyBtnEnum(
	void
)
{
#if SP5K_BUTTON_DIRECT_GPIO_NUMBER || SP5K_BUTTON_ADC_NUMBER || SP5K_BUTTON_MATRIX_GPIO_NUMBER
	UINT32 ret = SUCCESS;
#endif
	sp5kAdcInit();

#if SP5K_BUTTON_DIRECT_GPIO_NUMBER
	ret = sp5kButtonGpioInit(SP5K_BUTTON_DIRECT_GPIO_NUMBER);
	if( ret != SUCCESS )
	{
		printf("GPIO Button Driver Init Fail\n");
	}
#endif

#if SP5K_BUTTON_ADC_NUMBER
	ret = sp5kButtonAdcInit(SP5K_BUTTON_ADC_NUMBER);
	if( ret != SUCCESS )
	{
		printf("ADC Button Driver Init Fail\n");
	}
#endif

#if SP5K_BUTTON_MATRIX_GPIO_NUMBER
	ret = sp5kButtonMatrixInit(SP5K_BUTTON_MATRIX_GPIO_NUMBER);
	if( ret != SUCCESS )
	{
		printf("Matrix Button Driver Init Fail\n");
	}
#endif
}

/**************************************************************************
 *
 * Function Name: devUsbEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devUsbEnum(
	void
)
{
#ifndef FPGA
/*	sp5kScanRegister(&usbIoCfg);
	sp5kUsbDriverPinCfg(SP5K_USB_DETECT_PIN,SP5K_VBUS_DETECT_GRP,SP5K_VBUS_DETECT_ID,SP5K_VBUS_DETECT_POL);
*/
#endif
}

/**************************************************************************
 *
 * Function Name: usbPowerOnGet
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
UINT32
usbPowerOnGet(
	void
)
{
	return 0;
}


/**************************************************************************
 *
 * Function Name: devOtgEnum
 *
 * Descriptions:
 *
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devOtgEnum(
	void
)
{
	#if 0
	/* Initial the otg GPIO */
	usbDriverPinCfg(SP5K_OTG_ID_DETECT_PIN, SP5K_OTG_ID_DETECT_GRP, SP5K_OTG_ID_DETECT_ID , SP5K_GPIO_ACTIVE_LOW );
	#endif
}


/**************************************************************************
 *                                                                        *
 * Function Name: devRtcInit                                              *
 *                                                                        *
 * Purposes:                                                              *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *   None                                                                 *
 * Returns:                                                               *
 *   None                                                                 *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/
extern void (*fpTimeGetCb)(struct tm *ptime); /* time.c */
extern void CustomerTimeGetCb(struct tm *ptime); /* app_Rtc.c */
UINT32 devRtcFail=0;
typedef struct tagRTC
{
	unsigned short Year;
	unsigned short Month;
	unsigned short Day;
	unsigned short Hour;
	unsigned short Min;
	unsigned short Sec;
} SRTC, * PSRTC;

void _rtcDefaultGet(PSRTC psRtc)
{

	SRTC rtc={2012,1,1,0,0,0};
	
	psRtc->Year = rtc.Year;
	psRtc->Month = rtc.Month;
	psRtc->Day = rtc.Day;
	psRtc->Hour = rtc.Hour;
	psRtc->Min = rtc.Min;
	psRtc->Sec = rtc.Sec;
}
void App_RtcDefaultSet(void)
{
	SRTC rtc;
	tmx_t t;

	_rtcDefaultGet(&rtc);
	t.tmx_year = rtc.Year - 1900;
	t.tmx_mon = rtc.Month;
	t.tmx_mday = rtc.Day;
	t.tmx_hour = rtc.Hour;
	t.tmx_min = rtc.Min;
	t.tmx_sec = rtc.Sec;
	sp5kRtcDateTimeSet(0, &t);
	printf("\n\n>>>>RTC default set: %d:%d:%d\n\n",rtc.Year,rtc.Month,rtc.Day);	
}

BOOL rtcWakeupPowerOnChk(void)
{
	BOOL ret;
	UINT32 val;
	sp5kPwrDetectGet(0x1000f, &val);
	ret = (val == 0x10000)? 1 : 0;
	printf("rtc wakeup irq: %d\n",ret);
	return ret;
}
void devRtcInit(void)
{
	UINT32 r,reliableCode;
	r = sp5kRtcReliableGet(&reliableCode);
	if ((r!=SUCCESS) ||(reliableCode != RTC_RELIABLE_CODE && reliableCode != 0x5b)) 
	{
		if (rtcWakeupPowerOnChk())
		{
			goto __powerDown;
		}

		printf("!!RTC Lost(%x)\n", reliableCode);
		App_RtcDefaultSet();
		devRtcFail = 1;
	}
	return;

	__powerDown:
	sp5kPowerCtrl(SP5K_POWER_OFF, 0);
	while (1)
	{
	;
	}
}

