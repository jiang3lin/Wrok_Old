/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2010 by DXG Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of DXG        *
 *  Technology Co., Ltd. All rights are reserved by DXG Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of DXG Technology Co., Ltd.                       *
 *                                                                        *
 *  DXG Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  DXG Technology Co., Ltd.                                          *
 *  MIN ZHI IND., PARK,					            *
 *  LONG HUA,BAOAN,SHEN ZHEN,P.R.O.C.                                           *
 *                                                                        *
 *  Author: Wujf                                                  *
 *                                                                         *
 **************************************************************************/

#ifndef _APP_SCDV_GLOBAL_H_
#define _APP_SCDV_GLOBAL_H_


#define SCDV_VIDEO_AUDIO_SAMPLE_RATE		44100
#define SCDV_VIDEO_AUDIO_SAMPLE_BIT		16
#define SCDV_VIDEO_AUDIO_CHANNEL			1	// 2
#define SCDV_VIDEO_AUDIO_DEFAULT_VOL		31//31/2

#define SCDV_AUTO_RECORD_TIME				3000	//ms
#define SCDV_AUTO_STOP_TIME				3000	//ms
	
#define APP_WIFI_SSID			"DVS5M9"
#define APP_WIFI_DEVICE_NAME	"MD87156"

typedef struct appGlobal_s
{
	UINT32 appCdfsCount;
	BOOL appDcInFlag;
	UINT32 appAutoRecordTimerIsr;
 	BOOL appGTriggerPowerOff;
} appGlobal_t;

extern appGlobal_t appGlobal;


extern void appGlobalInit(void);
extern UINT8 appScdvDiskCheckMemoryFull(void);
extern UINT8 appScdvWaitForDiskReady(void);
extern void appModeChangeFlow(void);
extern void appOsDelayMsEx(UINT32 delayTime, UINT8* strFunction, UINT32 line);
#define appOsDelayMs(delayTime)	appOsDelayMsEx(delayTime, (UINT8*)__FUNCTION__, __LINE__)
extern BOOL appCheckFocusMode(void);


extern void appOsdFlipSet(void);
extern void appOsdBrightnessSet(void);
extern void appCdspFlipSet(BOOL bFlip, BOOL Immediate);
extern void appSensorFlipSet(void);
extern void appLedIndicatorSet(void);
extern void appBuzzerEnableSet(void);


//Other function
extern void appAutoPowerOffReset(void);
extern void appAutoPowerOffEnable(BOOL bEnable);
extern void appLcmPowerSaveReset(void);
extern void appLcmPowerSaveEnable(BOOL bEnable);
extern BOOL appLcmPowerSaveStateGet(void);
extern void appWifiSsidPasswordSet(void);
extern BOOL appCheckSdvMainMode(void);
extern void appPathSlashReplace(UINT8* pathString);
extern BOOL appVideoRecordStateGet(void);

#endif	//_APP_SCDV_GLOBAL_H_
