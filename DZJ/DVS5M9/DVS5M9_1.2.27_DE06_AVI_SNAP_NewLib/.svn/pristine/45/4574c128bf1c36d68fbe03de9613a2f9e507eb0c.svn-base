/**************************************************************************
 *
 *        Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
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
 **************************************************************************/
#ifndef APP_VIDEO_API_H
#define APP_VIDEO_API_H
#include "app_aaa.h"
#include "app_btn_def.h"
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	OSD_VIDEO_SIMPLE, OSD_VIDEO_VERBOSE, OSD_VIDEO_NONE
};
#define OSD_VIDEO_DEFAULT	OSD_VIDEO_SIMPLE

#define CAN_I_START_RECORD	0
#define CAN_I_LEAVE_VIDEO	1
#define VIDEO_STATE_YES 0
#define VIDEO_STATE_NO 	1

#ifdef KIT_WITH_HDMI
#define VIDEO_AUDIO_SAMPLE_RATE	44100
#else
#define VIDEO_AUDIO_SAMPLE_RATE	8000
#endif
#define VIDEO_AUDIO_SAMPLE_BIT	16
#if SPCA6330
#define VIDEO_AUDIO_CHANNEL		2
#else
#define VIDEO_AUDIO_CHANNEL		1 /*Mono*/
#endif
#define VIDEO_FRAME_RATE		30
#define AVI_HDR_KB	8
#define APP_CAPTURE_IN_MOVIE	16384	/* 8 * 2 * 1024 */
#define APP_THRESHOLD_SPACE	10240
#if SP5K_SENSOR_OV2715
#define  SENSOR_MODE_1080P	0x30	
#define  SENSOR_MODE_720P_30FPS  0x31
#define  SENSOR_MODE_720P_60FPS  0x32
#define  SENSOR_MODE_VGA_120FPS  0x33
#define  SENSOR_MODE_QVGA_240FPS 0x34
#define  SENSOR_MODE_SVGA_30FPS  0x36
#elif SP5K_SENSOR_OV5653
#define  SENSOR_MODE_XGA_30FPS 0x30
#define  SENSOR_MODE_1080P       0x31
#define  SENSOR_MODE_720P_30FPS  0x32
#define  SENSOR_MODE_720P_60FPS  0x33
#define  SENSOR_MODE_VGA_120FPS  0x34
#define  SENSOR_MODE_QVGA_240FPS 0x35
#elif SP5K_SENSOR_MT9J003
#define  SENSOR_MODE_VGA_120FPS  0x30
#define  SENSOR_MODE_1080P       0x31
#define  SENSOR_MODE_720P_30FPS  0x32
#define  SENSOR_MODE_720P_60FPS  0x33
#define  SENSOR_MODE_VGA_120FPS  0x34
#define  SENSOR_MODE_QVGA_240FPS 0x34
#elif SP5K_SENSOR_OV16820
#define  SENSOR_MODE_1080P       0x33
#define  SENSOR_MODE_720P_30FPS  0x32
#define  SENSOR_MODE_720P_60FPS  0x35
#define  SENSOR_MODE_VGA_120FPS  0x34
#define  SENSOR_MODE_QVGA_240FPS 0x31
#define  SENSOR_MODE_FSIZE_7_5FPS 0x36
#elif SP5K_SENSOR_SONYIMX083
#define  SENSOR_MODE_1080P       0x33
#define  SENSOR_MODE_720P_30FPS  0x33
#define  SENSOR_MODE_720P_60FPS  0x32
#define  SENSOR_MODE_VGA_120FPS  0x30
#define  SENSOR_MODE_QVGA_240FPS 0x31
#elif SP5K_SENSOR_MI5100p
#define  SENSOR_MODE_1080P 0x30
#define  SENSOR_MODE_720P_30FPS  0x30
#define  SENSOR_MODE_720P_60FPS  0x31
#define  SENSOR_MODE_VGA_120FPS  0x32
#define  SENSOR_MODE_QVGA_240FPS 0x33
#elif SP5K_SENSOR_OV5633                /*junqiong.hu@20121207*/
#define  SENSOR_MODE_1080P       0x30
#define  SENSOR_MODE_720P_30FPS  0x30
#define  SENSOR_MODE_720P_60FPS  0x30
#define  SENSOR_MODE_VGA_120FPS  0x30
#define  SENSOR_MODE_QVGA_240FPS 0x30
#elif SP5K_SENSOR_AR0330
#define  SENSOR_MODE_1080P       0x30
#define  SENSOR_MODE_720P_30FPS  0x30
#define  SENSOR_MODE_720P_60FPS  NULL
#define  SENSOR_MODE_VGA_120FPS  NULL
#define  SENSOR_MODE_QVGA_240FPS NULL
#elif SP5K_SENSOR_AR0330CS
#define  SENSOR_MODE_1080P       0x31
#define  SENSOR_MODE_720P_30FPS  0x31
#define  SENSOR_MODE_720P_60FPS  0x32
#define  SENSOR_MODE_VGA_120FPS  0x30
#define  SENSOR_MODE_QVGA_240FPS 0x30
#elif SP5K_SENSOR_AR0330CS_PLCC
#define  SENSOR_MODE_1080P       0x30
#define  SENSOR_MODE_720P_30FPS  0x31
#define  SENSOR_MODE_720P_60FPS  0x31
#define  SENSOR_MODE_VGA_120FPS  NULL
#define  SENSOR_MODE_QVGA_240FPS NULL
#elif SP5K_SENSOR_AR0331
#define  SENSOR_MODE_1080P       0x31
#define  SENSOR_MODE_720P_30FPS  0x31
#define  SENSOR_MODE_720P_60FPS  0x30
#define  SENSOR_MODE_VGA_120FPS  0x32
#define  SENSOR_MODE_QVGA_240FPS 0x33
#else
#define  SENSOR_MODE_1080P       0x30
#define  SENSOR_MODE_720P_30FPS  0x30
#define  SENSOR_MODE_720P_60FPS  0x30
#define  SENSOR_MODE_VGA_120FPS  0x30
#define  SENSOR_MODE_QVGA_240FPS 0x30

#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appVideoCB_s {
	UINT32 CapTimer;
	UINT32 CapSec;
	UINT8 osd;
	int fCapture;
	int fPause;
	int fStop;
	UINT32 isDiskReady;
	UINT32 err;
	UINT32 framePerSec;
	UINT32 streamBytesPerSec;
	UINT32 aviKbPerSec;
	UINT32 isAutoPowerOffOn;
} appVideoCB_t;

typedef struct appVideoSize_s{
	UINT16 w,h;
	UINT16 icon;
	UINT16 sensormode; 
	UINT8  audio;
	UINT32 frameRate ;
} appVideoSize_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define VIDEO_SET_CAPTURE_MODE		(1<<0)
#define VIDEO_SET_CAPTURE_SIZE		(1<<1)
#define VIDEO_SET_ALL				0xffffffff
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern appVideoCB_t gVideoCB;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appVideoSettingInit(UINT32 flg);
UINT32 appVidFrameRateGet(UINT8);

void appVidSizeSet(UINT8 size_idx);
void appVideo_SetDistRsc(void);
void appStillInMovieInit(void);
void appRscEnable(UINT8 distIdx);
void appRscDisable(UINT8 distIdx);
void appVidFpsSet(UINT8 fps_idx);
UINT32 appVideoSizeGet(UINT8 size_idx, UINT32 *pW, UINT32 *pH);
UINT8 appVideoSizeIconGet(UINT8 size_idx);
UINT8 appVideoSizeIdxGet(UINT32 w, UINT32 h);
UINT8 appVideoFpsIconGet(UINT8 fps_idx);
UINT32 appVideoFileSizePerSecGet(UINT8 size_idx);
void appVideoStreamPerByteSet(UINT32 size_idx,UINT32 quality_idx ,UINT32 isSdCard);
UINT32 appVideoRemainSecGet();

void appVideoOsdDiskShow(BOOL bShow);
void appVideoOsdShow(UINT8 clean, UINT8 outInOsdFreeze, UINT8 osd);
void App_videoGPSDataSave(void);
void AppVideoRecFirstIndexFileNameGet(UINT8* pMediafilename ,UINT8* pTXTfilename);
void appVideoRecStampInit(void);
void AppVideoSeamlessNextFileNameGet(UINT8* pfilename);
void AppVideoSeamlessCB(void);
void AppVideoSeamlessSetNextDCFKey(void);
void AppVideoSeamlessCallback(void);
void appVideoSetQuality(UINT8 idx);
void appVideoLogFile_Print(const char *format, ...);

void appVideoLogFileInit(void);
void appVideoLogFileSave(void);
UINT32 appVideoSeamLessSecGet(void);
void appVideo_SetParkingMode(UINT8 parkingMode);

void appVideoRecAlwaysMenuEnter(void);
UINT32 appVideoRecAlwaysDo(void);
void appVideoRecManulStopSet(BOOL manualStop);
BOOL appVideoRecManulStopGet(void);
void appVideoRecAlwaysRestart(BOOL restart);
BOOL appVideoRecAlwaysRestartGet();
void appVideoRecAlwaysMenuMsgProc(UINT32 msg,UINT32 param);

#endif  /* APP_VIDEO_API_H */


