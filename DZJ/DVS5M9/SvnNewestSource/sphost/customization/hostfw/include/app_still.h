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
#ifndef _APP_STILL_H_
#define _APP_STILL_H_

#if 0 //to print the process of preview state init
#define VIEW_PERF_LOG  profLogAdd
#else
#define VIEW_PERF_LOG(...) 
#endif

#if 0 //to print each icons's draw time.
#define VIEW_OSD_PERF_LOG  profLogAdd
#else
#define VIEW_OSD_PERF_LOG(...) 
#endif

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_user_setting.h"
#include "app_aaa.h"
#include "app_btn_def.h" 
#include "app_disk.h"
#include "api/sp5k_gfx_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	AF_BUSY	= 0,
	AF_FAIL,
	AF_SUCCESS,
	AF_FINI,
	AF_MAX
};

enum {
	OSD_VIEW_SIMPLE, 
	OSD_VIEW_VERBOSE, 
	OSD_VIEW_NONE, 
	OSD_VIEW_GRID, 
	OSD_VIEW_CLEAN_ALL, 
	OSD_VIEW_S_ONE_HOLD,
	OSD_VIEW_MAX
};
#define OSD_VIEW_DEFAULT	OSD_VIEW_SIMPLE

enum {
	RAW_CB_SHUTTER_NORAML,
	RAW_CB_SHUTTER_LONG,
	RAW_CB_SHUTTER_MAX
};

typedef enum {
	PASM_MMODE_SEL_TV, 
	PASM_MMODE_SEL_AV, 
	PASM_MMODE_SEL_MAX 
} tvAvSel_e;

typedef enum 
{
	FOCUS_STATUS_VIEW,
	FOCUS_STATUS_OK,	
	FOCUS_STATUS_NG,	
	FOCUS_STATUS_MAX,
} focusStatus_e;

typedef enum {
	VIEW_ERR_NULL,	
	VIEW_ERR_NO_ERROR,	
	VIEW_ERR_CARD_ERROR,	
	VIEW_ERR_CARD_LOCKED,
	VIEW_ERR_MEMORY_FULL,
	VIEW_ERR_MEMORY_NOT_ENOUGH,
	VIEW_ERR_TO_POWER_OFF,
	VIEW_ERR_MAX
} viewErrorCode_t;

typedef enum {
	VIEW_FT_STATUS_DESTROY,
	VIEW_FT_STATUS_PAUSE,
	VIEW_FT_STATUS_RESETDATA_PAUSE,
	VIEW_FT_STATUS_RUN,
	VIEW_FT_STATUS_MAX,
} viewFtStatus_t;

#if SP5K_SENSOR_OV2715
#define  VIEW_SENSOR_MODE_4_3	0x35	
#define  VIEW_SENSOR_MODE_16_9  0x30

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#elif SP5K_SENSOR_OV5653
#define  VIEW_SENSOR_MODE_4_3	0x30	
#define  VIEW_SENSOR_MODE_16_9  0x30

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#elif SP5K_SENSOR_MT9J003
#define  VIEW_SENSOR_MODE_4_3	0x30	
#define  VIEW_SENSOR_MODE_16_9  0x35

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#elif SP5K_SENSOR_OV16820
#define  VIEW_SENSOR_MODE_4_3	0x30	
#define  VIEW_SENSOR_MODE_16_9  0x35

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#elif SP5K_SENSOR_SONYIMX083
#define  VIEW_SENSOR_MODE_4_3	0x30	
#define  VIEW_SENSOR_MODE_16_9  0x35

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#elif SP5K_SENSOR_MI5100p
#define  VIEW_SENSOR_MODE_4_3	0x30	
#define  VIEW_SENSOR_MODE_16_9  0x35

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#elif SP5K_SENSOR_OV5633
#define  VIEW_SENSOR_MODE_4_3	0x30	
#define  VIEW_SENSOR_MODE_16_9  0x30

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#else
#define  VIEW_SENSOR_MODE_4_3	0x30	
#define  VIEW_SENSOR_MODE_16_9  0x35

#define  CAP_SENSOR_MODE_4_3 0x40
#define  CAP_SENSOR_MODE_16_9 0x40
#endif


#define VIEW_SET_STATE_START		(1<<1)
#define VIEW_SET_DISK				(1<<2)
#define VIEW_SET_DATE_STAMP			(1<<3)
#define VIEW_SET_ALL				0xffffffff

#define CAN_I_START_SNAP	0
#define CAN_I_LEAVE_STILL	1
#define STILL_STATE_YES 0
#define STILL_STATE_NO 	1
#define STILL_STATE_FLASH_CHARGIN 	2
#define STILL_STATE_MEMORY_FULL		3

#define IS_PASM_PMODE (gStillCB.cap.CurViewMode == PASM_PROGRAM)
#define IS_PASM_AMODE (gStillCB.cap.CurViewMode == PASM_APERTURE)
#define IS_PASM_SMODE (gStillCB.cap.CurViewMode == PASM_SHUTTER)
#define IS_PASM_MMODE (gStillCB.cap.CurViewMode == PASM_MANUAL)
#define IS_SCENE_MODE (gStillCB.cap.CurViewMode == SCENE_MODE)
#define IS_VIDEO_MODE (gStillCB.cap.CurViewMode == VIDEO_MODE)
#define IS_AUDIO_MODE (gStillCB.cap.CurViewMode == AUDIO_MODE)
#define IS_PASM_MMODE_TV (IS_PASM_MMODE&&(gStillCB.tvAvSel==PASM_MMODE_SEL_TV)) //shutter speed
#define IS_PASM_MMODE_AV (IS_PASM_MMODE&&(gStillCB.tvAvSel==PASM_MMODE_SEL_AV)) //aperture value
#define IS_PASM_OR_SCENE_MODE (IS_PASM_PMODE||IS_PASM_AMODE||IS_PASM_SMODE||IS_PASM_MMODE||IS_SCENE_MODE)
#define IS_PASM_MODE (IS_PASM_PMODE||IS_PASM_AMODE||IS_PASM_SMODE||IS_PASM_MMODE)
#define IS_VIEW_MODE_CHANGE (gStillCB.cap.CurViewMode != gStillCB.lastcap.CurViewMode)

#define IS_AEB_ON (pViewParam->aeb == 1)
#if 1
#define IS_DRIVE_DOUBLE (0)
#else
#define IS_DRIVE_DOUBLE (puiPara->DriverMode == UI_DRIVE_DOUBLE)
#endif

#define IS_SMILE_DETECTION_MODE (gStillCB.cap.CurScene == SCENE_SMILE_DETECTION)
/*s Add by Aries 09/10/30*/
#define IS_BLINK_DETECTION_MODE (gStillCB.cap.CurScene == SCENE_BLINK)
/*e Add by Aries 09/10/30*/
#define IS_PANORAMA_MODE (gStillCB.cap.CurScene == SCENE_PANORAMA)

#define STROB_TEST_TIME 10000
#define FLASH_CHARGE_STOP 0xFF

#define FLASH_RECHARGE_TIME_PERIOD (3*60000)  // 3min
#define PF_PATH		"A:\\RO_RES\\UI\\JPG\\"

#define FIELD_QV_ENABLE   0

#define CAP_AUTO_ENABLE	1	/*Edward 20090105*/

#define CAP_FILE_ERROR  1  /*just for file error debug*/

/*#define AF_CURVE_SHOW*/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct appStillCB_s {
	void *viewParam;
	appCaptureSetting_t cap;
	appCaptureSetting_t lastcap;
	appAeStatus_t ae;
	appAwbStatus_t awb;
	UINT32 diskClusterSize;		// In KB
	UINT32 estFileSize;			// estimated file size	
	UINT32 estJobFilesSize;		// estimated job files size, like AEB (file*3)
	UINT32 remainImg;			// job files
	UINT32 s1press;				// to record the s1 status
	UINT32 s2press;				// to record the s2 status
	UINT32 s1AaaTicket;			// record the s1 press message in Preview and Capture st.
	UINT32 s2SnapTicket;		// record the s2 press message in Capture.
	UINT32 tvAvSel;				// only for PASM_MMODE, Tv(shutter speed) and Av(Aperture value)
	UINT32 flashCharging;		// to blink the led.
	UINT32 lensZoomSeg;			// 0~6 (wide->tele)
	UINT32 amodeApertureIdx;		// AMODE f number
	UINT32 smodeShutterIdx;		// SMODE shutter speed
	UINT32 needFlash;
	UINT32 focusStatus;
	UINT32 needDarkFrameSub;		// This will enable while we have long shutter speed.
	UINT32 isDiskReady;
	UINT32 ftStatus;
	UINT32 isLensReady;
	UINT32 isXDenoiseOn;
	UINT32 curPhotoFrameIdx;
	UINT32 diskSpaceInImgId;
	UINT32 flashChargeMonitorTimer;	// flash's power will decrease with time, we need to monitor it.
	UINT32 flashMode;				// this is a copy of pViewParam->flash, except burst mode.
	UINT32 awbSpeed;
	UINT32 isReady2View;
	UINT32 err;
	UINT32 otStatus;			/* Add by sc.liu */
	UINT32 btnum;
	//UINT32 viewHistogramTimer;
	UINT8 osd;					// osd disply mode
} appStillCB_t;

typedef struct photoframeFile_s{	
	UINT8* pf_big;
	UINT8* pf_small;
}photoframeFile_t;
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define PRECOMPOSITION	0	/* MPF open for 3D image */
#define FLOW_EXTRCT_ST_PAR(x) 	(x>=STATE_PARAM_FREE_CARRY? (x&0x000000FF) : x) 
#define FLOW_EXTRCT_CTRL_PAR(x) (x&0x00000F00) 

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern appStillCB_t gStillCB;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
 #if PRECOMPOSITION  
void yuvCropScale(UINT8 *srcbuf, UINT32 srcw, UINT32 srch,UINT8 *dstbuf, UINT32 dstw, UINT32 dsth,UINT32 cropx, UINT32 cropy, UINT32 cropw, UINT32 croph);
void AppPreCompositionBufferDestroy(void);
UINT32 AppPreCompositionBufferCreat(void);
void appViewDrawPreComposition(BOOL bShow, UINT32 xShift);
UINT32 AppPreCompositionGetReferenceImage(UINT32 yuvAddr, UINT32 qvgaAddr, void * psSizeInfo);
void AppCaptureYUVManipulation(UINT32 yuvAddr,UINT32 qvgaAddr,void * psSizeInfo);
UINT8 AppPreCompositionInfoGet(sp5kGfxPicture_t** pic);
UINT32 APP_GetCurrDZoomRatio(void);
#endif
void appViewInit(void);
 UINT32 appStillEstFileSize(UINT32 imgSizeIdx, UINT32 qualityIdx, UINT32 clustersize);
UINT32 appStillWaitJobFinish(void);

/* state flow control*/
void appStillFlow_SetDriveFlow(UINT32 driveMode, UINT32 aeb, UINT32 viewMode, UINT32 scene);
void appStillFlow_GetNextState(UINT32* pState, UINT32* pParam);

/* Still Setting funtion */
UINT32 appStill_GetViewParam(UINT8 view_idx, UINT8 scene_idx);
void appStill_SetPasmSceneParam(UINT8 view_idx, UINT8 scene_idx);
void appStillSettingInit(UINT32 flg);
void appStill_SetResolution(UINT8 idx);
void appStill_SetQuality(UINT8 idx);
void appStill_SetMetering(UINT8 idx);
void appStill_SetWB(UINT8 idx);
void appStill_SetISO(UINT8 idx);
void appStill_SetExposure(UINT8 idx);
void appStill_SetDriveMode(UINT8 idx);
void appStill_SetAEB(UINT8 idx);
void appStill_SetHDR(UINT8 idx);
void appStill_SetSharpness(UINT8 idx);
void appStill_SetHighlight(UINT8 idx);
void appStill_SetDateStamp(UINT32 stamp,UINT32 dateformat);
void appStill_SetDist(UINT8 idx);
void appStill_SetStabilizer(UINT8 idx);
void appStill_SetFaceTracking(UINT8 idx);
void appStill_SetPhotoFrame(UINT8 idx);
void appStill_SetDZoom(UINT8 idx);
void appStill_SetFlash(UINT8 idx);
void appStill_SetAF(UINT8 idx);
void appStill_SetShutterSpeed(UINT8 idx);
void appStill_SetApertureValue(UINT8 idx);
/*s Add by Aries 09/10/30*/
void appStill_SetQV(UINT32 qv);
void appStill_SetConfirmTimeOut(UINT32 time);
void appStill_SetCaptureConfirmStore(UINT32 storeflag);
void appStill_CfgCaptureConfirmStore(UINT32 enable);
/*e Add by Aries 09/10/30*/
void appStill_SetExpFlickerMode(UINT8 idx);
/*e Add by Aries 09/08/11*/
void appStill_ChangeFlash(void);
void appStill_ChangeAf(void);
void appStill_ChangeShutterSpeed(UINT32 updownkey);
void appStill_ChangeApertureValue(UINT32 updownkey);
void appStill_ResetAllParam(void);
void appStill_GetCurSize(UINT32* w, UINT32* h);
BOOL appStill_GetMainFaceData(UINT32* x0, UINT32* y0, UINT32* x1, UINT32* y1);
void appStill_SetSp5kStillSize(UINT8 idx);
 UINT32 appStillCapFlashStatusGet(void);
 UINT32 appStillCapExpTimeGet(void);
UINT32 appStillCapShutCloseTimeGet(void);
/* sp5k still call back function (exif cb is provide by app_exif )*/ 
UINT32 appStillBurstCB(UINT32 res1, UINT32 res2, void* res3);
void appStillRawCbSet(BOOL bLongShutter);
void appStillYuvCbSet(BOOL isEnable);
void appStillPreviewCbSet(BOOL isEnable);
UINT32 appViewWbCb(sp5kWbGain_t *wb);

/* Still IQ function */
void appStillViewIqSet(void);
void appStillSnapIqSet(void);
void appIqEffectSet(UINT32 value);
void appIqSharpnessSet(UINT32 mode,UINT32 value);
BOOL appIqAhdStatusGet(	void);
BOOL appIqDarkFrameEnableGet(void);
void appAaaAfWinSet(UINT32* isFaceAF);
UINT32 appStillCwbGet(sp5kWbGain_t* pWb);
#ifdef AF_CURVE_SHOW
void appAfValueCurveDraw(UINT8 focusStatus);
#endif
/* OSD function (pleae refere to app_still_osd.h) */

/* Background functions */
void appStillBackground(UINT32 msg,	UINT32 param);
void appStillBackgroundInit(UINT32 cluSzInByte,	UINT32 dskFree,	UINT32 estFileSize);
void appStillBackgroundSet(UINT32 estFileSize);
UINT32 appStillBackgroundGetFree(UINT32* dskFree);

void appStillUtil_CaptureSet(void);
void appStillUtil_CalibCaptureSet(BOOL enable);
void appStillUtil_CaptureWithFaceCheck(void);
void appStillHDRCb(UINT32 addr, UINT32 rawFmt, void* imgSize);
void appStillNightSceneCb(UINT32 addr, UINT32 rawFmt, void* imgSize);

void appStillSpeedyCapture(UINT32	CapNum);
void app16in1Capture(UINT32	CapNum);

/* miniature effect cb */
UINT32 appMiniatureEffectPvCb(UINT32 idx,UINT8 *yuvAddr, UINT32 w, UINT32 h);
void appStill_SetMiniatureStatus(BOOL en);
BOOL appStill_GetMiniatureStatus(void);
void appStill_SetTimeLapse(UINT32 idx); 
UINT32 appTimeLapseSecGet(void);



#endif  /* __APP_VIEW_API_H__ */
