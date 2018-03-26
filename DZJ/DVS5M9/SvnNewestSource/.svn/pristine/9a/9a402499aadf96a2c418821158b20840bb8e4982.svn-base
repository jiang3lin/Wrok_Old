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
#define HOST_DBG 1
#include "common.h"
#include <stdio.h>
#include "sp5k_cdsp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_af_api.h"
#include "sp5k_rsvblk_api.h"
#include "api/sp5k_awb_api.h"

#include "api/sp5k_modesw_api.h"
#include "../customization/drivers/sensor/flow/sensor_model.h"
#include "api/sp5k_utility_api.h"

#include "customization\app_init.h"
#include "app_com_api.h"
#include "app_com_def.h"
#include "app_osd.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_zoom_api.h"
#include "app_util.h"
#include "app_rtc.h"
#include "app_still.h"
#include "app_video.h"
#include "app_strobe.h"
#include "app_led.h"
#include "app_battery.h"
#include "app_aaa.h"
#include "sp5k_cdsp_api.h"
#include "app_exif.h"
#include "app_view_osd.h"
#include "app_face_detect_ex.h"

#include "app_calib_api.h"

#include "api/sp5k_awb_api.h"
#if SPCA6330
#include "app_ptp.h"
#include "app_wifi_utility.h"
#endif
#if PRECOMPOSITION
#include "sp5k_utility_api.h"
#include "app_General.h"
#endif
#if ICAT_FCWS
#include "app_cvrws_api.h"
#endif
#include "middleware/sysg_resource_def.h"
#include "app_ui_para.h"
#include "app_icon_def.h"
#include "middleware/usb_pima_code.h"

#include "app_scdv_ui_para_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* JFIF header + EXIF header(thumbnail included) + QV*/
#define IMAGE_FILE_HEADER 64*1024

#if SNAP_CAPTURE_MODE == 1
#define CAPTURE_MODE_SINGLE		SP5K_CAPTURE_SINGLE_VLC
#define CAPTURE_MODE_BURST		SP5K_CAPTURE_BURST_VLC
#else
#define CAPTURE_MODE_SINGLE		SP5K_CAPTURE_SINGLE_RGB
#define CAPTURE_MODE_BURST		SP5K_CAPTURE_BURST_RGB
#endif
extern UINT8 IsHDR,IsHDRNightScene;
static UINT32 timelapseGap;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
sp5kCaptureStamp_t gStampInfo =
{
	.dateFmtId = SP5K_CAPTURE_DATE_YYYYMMDD,//0xFF,
	.dateLen = 0,
	.pdate = NULL,
	.scaleRatio = 100,
	.x=100,
	.y=100,
	.backgndColor = 0,//COLOR_Nocolor,
	.fontColor = 19,//COLOR_Orange_Idx1,
	.OutlineColor = 6,//COLOR_Nocolor,
};

 typedef struct {
	SINT16 roffset;
	SINT16 groffset;
	SINT16 boffset;
	SINT16 gboffset;
}WbOffset_t;

/*you can set size as follow list
{ 3648, 2736, ID_ICON_RESOLUTION_10M},
{ 3456, 2592, ID_ICON_RESOLUTION_9M },
{ 3264, 2448, ID_ICON_RESOLUTION_8M },
{ 3072, 2304, ID_ICON_RESOLUTION_7M},
{ 2816, 2112, ID_ICON_RESOLUTION_6M },
{ 2560, 1920, ID_ICON_RESOLUTION_5M },
{ 2272, 1704, ID_ICON_RESOLUTION_4M },
{ 2048, 1536, ID_ICON_RESOLUTION_3M },
{ 1600,  1200, ID_ICON_RESOLUTION_2M },
{ 1024,  768, ID_ICON_RESOLUTION_1M },

{ 3264, 2448, ID_ICON_RESOLUTION_8M },
{ 3264, 2176, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 3264, 1840, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},

{ 3072, 2304, ID_ICON_RESOLUTION_7M },
{ 3072, 2048, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 3072, 1728, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},

{ 3648, 2736, ID_ICON_RESOLUTION_10M},
{ 3648, 2432, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 3648, 2048, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},

{ 2560, 1920, ID_ICON_RESOLUTION_5M },
{ 2560, 1706, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 2560, 1440, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},
*/



static struct
{
	UINT16 w,h;
	UINT8 icon;
	UINT32 preSensorMode;
	UINT32 capSensorMode;
} ImgSize[]=
{
/* 	related function not ready, removed for temporarily by gx
	{ 5120, 3840, ID_ICON_RESOLUTION_20M},
	{ 4640, 3480, ID_ICON_RESOLUTION_16M },
	{ 4416, 3312, ID_ICON_RESOLUTION_14M },
	{ 4000, 3000, ID_ICON_RESOLUTION_12M},
*/
	#if !CAM_TYPE_CVR
	{ 3648, 2736, ID_ICON_RESOLUTION_10M,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 3456, 2592, ID_ICON_RESOLUTION_9M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 3264, 2448, ID_ICON_RESOLUTION_8M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 3072, 2304, ID_ICON_RESOLUTION_7M,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 3640, 2048, ID_ICON_RESOLUTION_7M_16_9,VIEW_SENSOR_MODE_16_9,CAP_SENSOR_MODE_16_9},
	{ 2560, 1920, ID_ICON_RESOLUTION_5M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 2048, 1536, ID_ICON_RESOLUTION_3M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 1600, 1200, ID_ICON_RESOLUTION_2M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 1920, 1080, ID_ICON_RESOLUTION_2M_16_9,VIEW_SENSOR_MODE_16_9,CAP_SENSOR_MODE_16_9},
	{ 1280, 960, ID_ICON_RESOLUTION_1M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 640, 480, ID_ICON_RESOLUTION_VGA,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	#else
	{ 3648, 2736, ID_ICON_RESOLUTION_10M,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	//{ 3456, 2592, ID_ICON_RESOLUTION_9M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 3264, 2448, ID_ICON_RESOLUTION_8M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 2816, 2112, ID_ICON_RESOLUTION_6M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	//{ 3072, 2304, ID_ICON_RESOLUTION_7M,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	//{ 3640, 2048, ID_ICON_RESOLUTION_7M_16_9,VIEW_SENSOR_MODE_16_9,CAP_SENSOR_MODE_16_9},
	//{ 2560, 1920, ID_ICON_RESOLUTION_5M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 2048, 1536, ID_ICON_RESOLUTION_3M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	//{ 1600, 1200, ID_ICON_RESOLUTION_2M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 1920, 1080, ID_ICON_RESOLUTION_2M_16_9,VIEW_SENSOR_MODE_16_9,CAP_SENSOR_MODE_16_9},
	//{ 1280, 960, ID_ICON_RESOLUTION_1M ,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	{ 640, 480, ID_ICON_RESOLUTION_VGA,VIEW_SENSOR_MODE_4_3,CAP_SENSOR_MODE_4_3},
	#endif
};


photoframeFile_t photoFrameTable[UI_PHOTOFRAME_MAX] =
{
	{NULL, NULL},
	{"FRAME001.JPG", "FRAMEDP1.JPG"},
	{"FRAME002.JPG", "FRAMEDP2.JPG"},
	{"FRAME003.JPG", "FRAMEDP3.JPG"},
	{"FRAME004.JPG", "FRAMEDP4.JPG"},
	{"FRAME005.JPG", "FRAMEDP5.JPG"},
};

static UINT8 compressRatio[UI_QUALITY_MAX]={ 5, 8, 12 };


/* 1. This only effect for scene mode
** 2. This will be reset in appStill_SetPasmSceneParam().
*/
UINT32 focus_idx=0;
UINT32 flash_idx=0;

/* 1. The first item is default setting.
** 2. When mode chang the idx will reset to default.
*/
typedef struct buttonParam_s
{
	UINT32 id;
	UINT8 focus[6];
	UINT8 flash[6];
	SINT32 shutterBoundary[2];
	UINT8 metering[4];
	UINT32 sharpness;
	UINT32 stablizer;
	UINT32 facetracking;
} buttonParam_t;

static buttonParam_t* pCurBtnPar=0;
static buttonParam_t buttonParam[PASM_MAX+SCENE_MAX] =
{
	// PASM
	{
		PASM_PROGRAM,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_RED_EYE, UI_FLASH_ON, UI_FLASH_SLOW_SYNC, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_16000S}
	},
	{
		PASM_APERTURE,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_RED_EYE, UI_FLASH_ON, UI_FLASH_SLOW_SYNC, UI_FLASH_MAX},
		{SHUTTER_8S, SHUTTER_1_2000S}
	},
	{
		PASM_SHUTTER,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_RED_EYE, UI_FLASH_ON, UI_FLASH_MAX},
		{SHUTTER_8S, SHUTTER_1_2000S}
	},
	{
		PASM_MANUAL,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_RED_EYE, UI_FLASH_ON, UI_FLASH_MAX},
		{SHUTTER_8S, SHUTTER_1_2000S}
	},
	{
		VIDEO_MODE,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1_15S, SHUTTER_1_2000S}
	},
	// SCENE
	{
		PASM_MAX + SCENE_SMILE_DETECTION,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_RED_EYE, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_ON
	},
	{
		PASM_MAX + SCENE_SHAKE_FREE,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_AUTO, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1_250S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_LANDSCAPE,
		{UI_FOCUS_INFINITY,	UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_MORE,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_OFF
	},
	{
		PASM_MAX + SCENE_BACK_LIGHT,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_ON, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_SPOT, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_NIGHT,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_SLOW_SYNC, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_4S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_OFF
	},
	{
		PASM_MAX + SCENE_BEACH_SNOW,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_AUTO, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_FIRE_WORKS,
		{UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_4S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_OFF,
		.facetracking = UI_FACETRACK_OFF
	},
	{
		PASM_MAX + SCENE_BUILDING,
		{UI_FOCUS_NORMAL, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_AUTO, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_MORE,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_OFF
	},
	{
		PASM_MAX + SCENE_PORTRAIT,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_RED_EYE, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1_15S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_FOOD,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_MAX},
		{UI_FLASH_AUTO, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_OFF
	},
	{
		PASM_MAX + SCENE_TEXT,
		{UI_FOCUS_MACRO, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_AUTO, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_OFF
	},
	{
		PASM_MAX + SCENE_CHILDREN,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_MAX},
		{UI_FLASH_RED_EYE, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_NORMAL,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_SUNSET,
		{UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_MORE,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_OFF
	},
	{
		PASM_MAX + SCENE_WDR,
		{UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_MORE,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_HDR,
		{UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1_30S, SHUTTER_1_2000S},/*fan.xiao modify*/
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_MORE,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_PANORAMA,
		{UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_MORE,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_MAX
	},
	{
		PASM_MAX + SCENE_BLINK,
		{UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_2000S},
		{UI_METER_MULTI, UI_METER_CENTER, UI_METER_MAX},
		.sharpness = UI_SHARP_MAX,
		.stablizer = UI_STABILIZER_MAX,
		.facetracking = UI_FACETRACK_ON
	}
};

#ifdef HW_DIST
BOOL gDistEnTag=FALSE;
#endif
BOOL gbMiniatureEn = FALSE;

#if PRECOMPOSITION
#define LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(addr) ((void *)((UINT32)(addr) | 0x20000000))
#define W 320
#define H 240

UINT8 *pPreCompositionBuf=NULL;
sp5kGfxPicture_t precompsition_pic;
UINT32 gnDigitalZoomRatio = 1000;
#endif

/*UINT16 MWB_mode =0;*/

/* Global data */
tmx_t rtcn;
/*static UINT32 stillStampPage = 0;*/  //for clean warning
static UINT32 stillStampDrawRatioW = 0;
static UINT32 stillStampDrawRatioH = 0;
static UINT8 *ConvertBuf = NULL;



/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appSnapFlashUiModeSet(UINT16 ui_mode); // for clean warning
extern void appIqSetWdr(UINT8 enable); // for clean warning
extern UINT32 osMemFree(void *ptr);;    //For remove warning
extern const sensorTgSnapCfg_t *psensorTgSnapCfgGet();
extern UINT8 icatch5330YuvWrite(UINT8 *fname, UINT8 *dataBuf, UINT32 imgW, UINT32 imgH, UINT32 yuvType);
extern UINT32 fsSimpleWrite(UINT8 *fname, void *buf, UINT32 size);
#if PRECOMPOSITION
extern void cpuDCacheFlush(void *addrstart, UINT32 size);
#endif

#if SP5K_ALGORITHM_AWB
extern void appAWBALG_WBModeSet( UINT32 selector); // for clean warning
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if PRECOMPOSITION
void yuvCropScale(
	UINT8 *srcbuf, UINT32 srcw, UINT32 srch,
	UINT8 *dstbuf, UINT32 dstw, UINT32 dsth,
	UINT32 cropx, UINT32 cropy, UINT32 cropw, UINT32 croph
)
{
	sp5kYuvScale_t s;
	srcbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(srcbuf);
	dstbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);
	cpuDCacheFlush(srcbuf, srcw*srch*2);

	s.psrcAddr = srcbuf;
	s.pdstAddr = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);
	s.srcWidth = ((srcw>>4)<<4);
	s.srcHeight = ((srch>>2)<<2);
	s.dstWidth = ((dstw>>4)<<4);
	s.dstHeight = ((dsth>>2)<<2);

	s.cropWidth = cropw;
	s.cropHeight = croph;
	s.cropOffX = cropx;
	s.cropOffY = cropy;

	sp5kUtilYuvScale(&s, SP5K_SCALE_FORMAT_YUV);
}


UINT32 APP_GetCurrDZoomRatio(void)
{
	/* 1000=1.0 x */
	return gnDigitalZoomRatio = appDZoomGetRation();;
}


UINT8 AppPreCompositionInfoGet(sp5kGfxPicture_t** pic)
{
	if (pPreCompositionBuf == NULL) {
		return FAIL;
	}
	else {
		*pic = &precompsition_pic;
		return SUCCESS;
	}
}


void AppPreCompositionBufferDestroy(void)
{
	if (pPreCompositionBuf != NULL) {
		sp5kYuvBufferFree(pPreCompositionBuf);
		pPreCompositionBuf = NULL;
	}
}


UINT32 AppPreCompositionBufferCreat(void)
{
	/* New YUV format */
	pPreCompositionBuf = sp5kYuvBufferAlloc(320, 240);

	return (UINT32)pPreCompositionBuf;
}


void
appViewDrawPreComposition(
	BOOL bShow,
	UINT32 xShift
)
{
	sp5kGfxPicture_t *pic;
	int x1, y1, x2, y2;
	const int LCDX=0, LCDY=0,LCDW=320, LCDH=240;

	if(AppPreCompositionInfoGet(&pic)==FAIL) {
		printf("%s buf empty\n",__FUNCTION__);
		return;
	}

	if(bShow) {
		x1 = LCDX;
		x2 = LCDX+LCDW;
		y1 = (240-LCDH)/2 + LCDY;
		y2 = y1 + LCDH;
		int x_1=40,x_gap=240,y_1=160,y_gap=40;	/* assistance region  */
		sp5kGfxPictureScale(SP5K_GFX_PAGE_PF_0, 0+xShift, 30, 320, 180,pic);

		sp5kGfxAttrSet(SP5K_GFX_PEN_ENABLE, 1, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x0000, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x0000, 0, 0, 0);

		sp5kGfxRectDraw(SP5K_GFX_PAGE_PF_0,x1, y1, x_1, y2);
		sp5kGfxRectDraw(SP5K_GFX_PAGE_PF_0,x_1+x_gap, y1, x2, y2);
		sp5kGfxRectDraw(SP5K_GFX_PAGE_PF_0,x_1, y1, x_1+x_gap, y_1);
		sp5kGfxRectDraw(SP5K_GFX_PAGE_PF_0,x_1, y_1+y_gap, x_1+x_gap, y2);
		printf("x1(%d), y1(%d), x_1(%d), y2(%d)\n", x1, y1, x_1, y2);
		printf("x_1+x_gap(%d), y1(%d), x2(%d), y2(%d)\n", x_1+x_gap, y1, x2, y2);
		printf("x_1(%d), y1(%d), x_1+x_gap(%d), y_1(%d)\n", x_1, y1, x_1+x_gap, y_1);
		printf("x_1(%d), y_1+y_gap(%d), x_1+x_gap(%d), y2(%d)\n", x_1, y_1+y_gap, x_1+x_gap, y2);

		sp5kGfxAttrSet(SP5K_GFX_LAYER_ALPHA, SP5K_GFX_PAGE_PF_0, 96, 0, 0);

		printf("View_SetStillAreaOSD\n");
		/*View_SetStillAreaOSD(&psCapture->uwImageWide,&psCapture->uwImageHeight, 1);*/

	}
	else {
		sp5kGfxAttrSet(SP5K_GFX_LAYER_ALPHA, SP5K_GFX_PAGE_PF_0, 0, 0, 0);
		AppPreCompositionBufferDestroy();
		/*View_SetStillAreaEx(&psCapture->uwImageWide, &psCapture->uwImageHeight, 1);*/
	}
}


UINT32
AppPreCompositionGetReferenceImage(
	UINT32 yuvAddr,
	UINT32 qvgaAddr,
	void * psSizeInfo
)
{
	UINT32 width, height, qvw, qvh;
	UINT32 coordinate_x, coordinate_y;
	UINT32 original_cropWidth, original_cropHeight;
	float ratio_x_y;
	sp5kImgSize_t *pYuvSize=(sp5kImgSize_t *)psSizeInfo;


	width = pYuvSize->cropWidth;
	height = pYuvSize->cropHeight;
	qvw = (UINT32)(W+4)&0xfffffff8;
	qvh = (UINT32)(W*height/width+4)&0xfffffff8;
	if (qvw>W) qvw = W;
	if (qvh>H) qvh = H;
	if (pPreCompositionBuf == (UINT32)NULL) {
		HOST_ASSERT_MSG(AppPreCompositionBufferCreat(), "");
	}

	width = (UINT32)(width + 15)&~(15);
	height = (UINT32)(height + 15)&~(15);
	qvw = (UINT32)(qvw + 15)&~(15);
	qvh = (UINT32)(qvh + 15)&~(15);

	original_cropWidth = pYuvSize->cropWidth;
	original_cropHeight = pYuvSize->cropHeight;
	ratio_x_y = (float)APP_GetCurrDZoomRatio()/1000.0;
	printf("Ratio is (%f)\n", ratio_x_y);
	pYuvSize->cropWidth = pYuvSize->cropWidth/ratio_x_y;
	pYuvSize->cropHeight = pYuvSize->cropHeight/ratio_x_y;
	pYuvSize->cropWidth = (UINT32)(pYuvSize->cropWidth + 15)&~(15);
	pYuvSize->cropHeight = (UINT32)(pYuvSize->cropHeight + 15)&~(15);
	coordinate_x = (original_cropWidth-pYuvSize->cropWidth)/2;
	coordinate_y = (original_cropHeight-pYuvSize->cropHeight)/2;
	printf("width(%d), height(%d), qvw(%d), qvh(%d), coordinate_x(%d), coordinate_y(%d), cropWidth(%d, cropHeight(%d)\n)",
	width, height, qvw, qvh, coordinate_x, coordinate_y, pYuvSize->cropWidth, pYuvSize->cropHeight);

	yuvCropScale((UINT8 *)yuvAddr, width, height, pPreCompositionBuf, qvw, qvh,
		coordinate_x, coordinate_y, pYuvSize->cropWidth, pYuvSize->cropHeight);

	/* past it on osd layer */
	sp5kGfxPicture_t* pic = &precompsition_pic;
	pic->pbuf = pPreCompositionBuf;
	pic->fmt = SP5K_GFX_FMT_YUV422;
	pic->frmW = qvw;
	pic->frmH = qvh;
	pic->roiX = 0;
	pic->roiY = 0;
	pic->roiW = qvw;
	pic->roiH = qvh;

	return SUCCESS;
}


void
AppCaptureYUVManipulation(
	UINT32 yuvAddr,
	UINT32 qvgaAddr,
	void * psSizeInfo
)
{
	sp5kYuvCbImgSize_t *size;
	size = (sp5kYuvCbImgSize_t *)psSizeInfo;

	AppPreCompositionGetReferenceImage(yuvAddr,0,psSizeInfo);
}
#endif

void appStill_FeatureEffectSet(void)
{
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->FeatureEffect)
	{
		case UI_FEATURE_EFFECT_HDR:
			gStillCB.cap.CurViewMode = SCENE_MODE;
			gStillCB.cap.CurScene = SCENE_HDR;
			break;
		case UI_FEATURE_EFFECT_PANORAMA:
			gStillCB.cap.CurViewMode = SCENE_MODE;
			gStillCB.cap.CurScene = SCENE_PANORAMA;
			break;
		case UI_FEATURE_EFFECT_NIGHT_SCENE:
			gStillCB.cap.CurViewMode = SCENE_MODE;
			gStillCB.cap.CurScene = SCENE_NIGHT;
			break;
		case UI_FEATURE_EFFECT_SPEEDY_CAPTURE:
			puiPara->ImageSize= UI_PIC_SIZE_VGA;
			gStillCB.cap.CurViewMode = PASM_PROGRAM;
			gStillCB.cap.CurScene = SCENE_NONE;
			break;
		case UI_FEATURE_EFFECT_16IN1:
			puiPara->ImageSize= UI_PIC_SIZE_VGA;
			gStillCB.cap.CurViewMode = PASM_PROGRAM;
			gStillCB.cap.CurScene = SCENE_NONE;
			appDZoomReset(APP_DZOOM_UI_MODE_STILL_VIEW); /*add for it  can only support zoom in <2 when 16 in 1 capture*/
			break;
		case UI_FEATURE_EFFECT_OFF:
		default:
			if(puiPara->TimeLapse == UI_TIMELAPSE_OFF)
			{
				puiPara->ImageSize = UI_PIC_SIZE_FULL;
			}
			gStillCB.cap.CurViewMode = PASM_PROGRAM;
			gStillCB.cap.CurScene = SCENE_NONE;
			break;
	}
}



/*-------------------------------------------------------------------------
 *  Function Name : appStillEstFileSize
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
appStillEstFileSize(
	UINT32 imgSizeIdx,
	UINT32 qualityIdx,
	UINT32 clustersize //in k byte
)
{
	UINT32 w, h, ratio;
	UINT32 predictVLCSize, predictFileSize;

	w = ImgSize[imgSizeIdx].w;
	h = ImgSize[imgSizeIdx].h;

	//for DVS5M2 see appSdvStillSizeSet
	uiPara_t* puiPara = appUiParaGet();
	switch(puiPara->SdvPhotoNormalSize)
	{
		//case UI_SCDV_PHOTO_NORMAL_SIZE_16M:
			//w = 4640;
			//h = 3480;
			//break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_5M:
			w = 2592;
			h = 1944;
			break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_3M:
			w = 2048;
			h = 1536;
			break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_2M_16X9:
			w = 1920;
			h = 1080;
			break;
	}	
	//for DVS5M2 see appSdvStillSizeSet

	ratio = compressRatio[qualityIdx];
	if(clustersize==0)
	{
		clustersize = 1024;
	}

	predictVLCSize = w * h * 2 / ratio;
	#if 1
	predictVLCSize = predictVLCSize * 110 / 100; /* BRC also has 10% tolerance but normally it is not that big */
	#else
	if (predictVLCSize >2500000)
	{
		predictVLCSize =2000000* 105 / 100;
	}
	else
	{
		predictVLCSize = predictVLCSize * 110 / 100; /* BRC also has 10% tolerance but normally it is not that big */
	}
	#endif

	predictFileSize = (predictVLCSize + IMAGE_FILE_HEADER)>>10;

	if (predictFileSize % clustersize)
	{
		predictFileSize += clustersize;
	}
	predictFileSize = (predictFileSize / clustersize) * clustersize;
	return predictFileSize;//in k byte
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetResolution
 *  Description :   This is for normal user seting. It only change the view
 					size by drawing black strips. The real sp5k setting will
 					be set by appStill_SetSp5kStillSize() before capture.
 *------------------------------------------------------------------------*/
void
appStill_SetResolution(
	UINT8 idx
)
{
	UINT32 currSensorMode;
	static UINT32 lastSensorMode = 0xff;

	appViewOsdValidAreaShow(ImgSize[idx].w, ImgSize[idx].h);

/*junqiong.hu@20121121*/
	sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, ImgSize[idx].capSensorMode);

	currSensorMode = ImgSize[idx].preSensorMode;
	sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, currSensorMode);

	/*current state group is still view || (previous state is still view && current state is menu)*/
	if(((IS_VIEW_GROUP(appActiveStateGet())) || \
		(IS_VIEW_GROUP(appPreviousStateGet()) && IS_MENU_STATE(appActiveStateGet()) ))\
		&& (currSensorMode != lastSensorMode)){

		lastSensorMode = currSensorMode;
		/*sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);*/
		appModeSet(SP5K_MODE_STILL_PREVIEW);
	}
/*junqiong.hu@20121121*/

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetSp5kStillSize
 *  Description :   This function will be called in capture/burst init for
 					real size. And it is set to biggest size when return to
 					view. Sp5k still/viedo view/capture/record mode change
 					all need to config the size before you change. In still,
 					we will always use max size for view and real size for
 					capture.
 *------------------------------------------------------------------------*/
void
appStill_SetSp5kStillSize(
	UINT8 idx
)
{
	UINT32 ret;
	sp5kImgSize_t capSize;
	static UINT32 factor=300;

	capSize.width = ImgSize[idx].w;
	capSize.height= ImgSize[idx].h;
	if( idx < UI_PIC_SIZE_3M) /*for 8M and 6M */
	factor =1000;
	else
	factor =800;
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_COMPRESSION_PRECISE_RATIO, &factor);

	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSize);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetQuality
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetQuality(
	UINT8 idx
)
{
	UINT32 cfg, ret;

	cfg = compressRatio[idx];
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_COMPRESSION_RATIO, &cfg);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetMetering
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetMetering(
	UINT8 idx
)
{
	appAeMeterSet(idx);
	appExif_SetMeterMode(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetWB
 *  Description :
 *------------------------------------------------------------------------*/
extern UINT16 AwbCT_Rgain[3];
extern UINT16 AwbCT_Bgain[3];
//extern _AWBALG_MODE_SEL _gvMWBSwitch;
void
appStill_SetWB(
	UINT8 idx
)
{
#if SP5K_ALGORITHM_AWB
	UINT16 one=0x400;
	UINT16 exifWbMode;
	sp5kWbGain_t wb;

	wb.gbgain = wb.grgain = one;
	switch (idx) /*junqiong.hu@20121225 modify for mantis bug 0046601*/
	{
		case UI_WB_AUTO:
			appAWBALG_WBModeSet(0);/*WB_MODE_AUTO*/
			break;
		case UI_WB_DAYLIGHT:
			appAWBALG_WBModeSet(1);/*WB_MODE_DAYLIGHT*/
			break;
		case UI_WB_CLOUDY:
			appAWBALG_WBModeSet(2);/*WB_MODE_CLOUDY*/
			break;
		case UI_WB_TUNGSTEN:
			appAWBALG_WBModeSet(3);/*WB_MODE_TUNGSTEN*/
			break;
		case UI_WB_FLUORESCENT_H:
			appAWBALG_WBModeSet(5);/*WB_MODE_FLUORESCENT_H*/
			break;
		case UI_WB_FLUORESCENT_L:
			appAWBALG_WBModeSet(4);/*WB_MODE_FLUORESCENT_L*/
			break;
		case UI_WB_FLUORESCENT:
			break;
		case UI_WB_CUSTOM:
			appAWBALG_WBModeSet(7);/*WB_MODE_CUSTOM*/
			if(appStillCwbGet(&wb) == FAIL)
			{
#if SP5K_ALGORITHM_AWB
				sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
				sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif
                          idx = UI_WB_AUTO;
			}
			break;
		case UI_WB_UNDERWATER:
			appAWBALG_WBModeSet(6);/*WB_MODE_UNDERWATER*/

			break;
	}
	/*if (idx != UI_WB_AUTO) {*/
	if (idx == UI_WB_CUSTOM)
	{
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
		#if 0   /*junqiong.hu@20121225 add for mantis bug 0046601*/
		sp5kImgWbGainSet(&wb);
		#else
		sp5kIqModeSet( SP5K_MODE_STILL_PREVIEW );
		sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_R_GAIN, wb.rgain );
		sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_GR_GAIN, wb.grgain );
		sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_B_GAIN, wb.bgain );
		sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_GB_GAIN, wb.gbgain );
		sp5kIqModeSetDone( SP5K_MODE_STILL_PREVIEW );
		#endif
		exifWbMode = SP5K_EXIF_TAGDATA_WB_MANUAL;
	}
	else
	{
		exifWbMode = SP5K_EXIF_TAGDATA_WB_AUTO;
	}
	appExifSetWBMode((UINT8*)&exifWbMode);

	gStillCB.awb.wbMode = idx;
	gStillCB.awb.rGain = wb.rgain;
	gStillCB.awb.bGain = wb.bgain;

#else

	UINT16 one=0x400;
	UINT16 exifWbMode;
	sp5kWbGain_t wb;

	wb.gbgain = wb.grgain = one;
	switch (idx)
	{
		case UI_WB_AUTO:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
			//MWB_mode=0;
                       //printf("^UI WB Set to Normal ^\n");
                        //printf("^ Change nMode to %d ^\n",_gvMWBSwitch);
			break;
		case UI_WB_DAYLIGHT:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
			sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,SP5K_AWB_FLOW_SUNNY);
                      // MWB_mode=1;
                       //printf("^ UI WB Set to Daylight ^\n");
                       // printf("^ Change nMode to %d ^\n",_gvMWBSwitch);
			break;
		case UI_WB_CLOUDY:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
			sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,SP5K_AWB_FLOW_CLOUDY);
			break;
		case UI_WB_TUNGSTEN:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
			sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,SP5K_AWB_FLOW_TUNGSTEN);
                    //MWB_mode=3;
                      // printf("^ UI WB Set to Tungsten ^\n");
                       // printf("^ Change nMode to %d ^\n",_gvMWBSwitch);
			break;
		case UI_WB_FLUORESCENT_H:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
                    sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,SP5K_AWB_FLOW_FLUORESCENT_H);
                    //MWB_mode=5;
			break;
		case UI_WB_FLUORESCENT_L:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
                    sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,SP5K_AWB_FLOW_FLUORESCENT_L);
                    //MWB_mode=4;
			break;
		case UI_WB_FLUORESCENT:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
                     sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,SP5K_AWB_FLOW_FLUORESCENT_L);
			break;
		case UI_WB_CUSTOM:
			if(appStillCwbGet(&wb) == FAIL)
			{
				sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
                            idx = UI_WB_AUTO;
			}
			break;
		case UI_WB_UNDERWATER:
			sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
                     sp5kAwbParamSet(SP5K_AWB_FLOW_MODE,SP5K_AWB_FLOW_UNDERWATER);

			break;
	}
	/*if (idx != UI_WB_AUTO) {*/
	if (idx == UI_WB_CUSTOM)
	{
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
		sp5kImgWbGainSet(&wb);

		exifWbMode = SP5K_EXIF_TAGDATA_WB_MANUAL;
	}
	else
	{
		exifWbMode = SP5K_EXIF_TAGDATA_WB_AUTO;
	}
	appExifSetWBMode((UINT8*)&exifWbMode);

	gStillCB.awb.wbMode = idx;
	gStillCB.awb.rGain = wb.rgain;
	gStillCB.awb.bGain = wb.bgain;

	DBG_PRINT("SetWB %d %d/%d %d\n", wb.rgain, wb.grgain, wb.gbgain, wb.bgain);
#endif

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetISO
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetISO(
	UINT8 idx
)
{
	appAeCapIsoSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetExposure
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetExposure(
	UINT8 idx
)
{
	DBG_PRINT("%s: idx=%d\n", __FUNCTION__, idx);
	appAeEvBiasSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDriveMode
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDriveMode(
	UINT8 idx
)
{
	UINT32 cap_mode, ret;
	uiPara_t* puiPara = appUiParaGet();

	//gStillCB.cap.qvSec = (idx==UI_DRIVE_OFF)? 360 : 0;
	if(IsHDR || IsHDRNightScene){
		idx = UI_DRIVE_OFF;
	}

	switch(idx)
	{
	case UI_DRIVE_OFF:
	case UI_DRIVE_TIMER_2S:
	case UI_DRIVE_TIMER_10S:
		gStillCB.remainImg = 1;
		gStillCB.estJobFilesSize = gStillCB.estFileSize;
		cap_mode = CAPTURE_MODE_SINGLE;
		break;
	case UI_DRIVE_DOUBLE:
		gStillCB.remainImg = 2;
		gStillCB.estJobFilesSize = gStillCB.estFileSize*2;
		cap_mode = CAPTURE_MODE_SINGLE;
		break;
	case UI_DRIVE_BURST:
		gStillCB.remainImg = 0xffff;
		gStillCB.estJobFilesSize = gStillCB.estFileSize;
		cap_mode = CAPTURE_MODE_BURST;
		break;
	}
	gStillCB.flashMode = (idx==UI_DRIVE_BURST)? UI_FLASH_OFF: puiPara->flash;
	appStill_SetFlash(gStillCB.flashMode);

	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, &cap_mode);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetAEB
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetAEB(
	UINT8 idx
)
{
	UINT32 cap_mode, ret;
	uiPara_t* puiPara = appUiParaGet();
	appAeAebSet(idx);

	if(puiPara->DriverMode != UI_DRIVE_OFF)
	{
		DBG_PRINT("%s: AEB only availbe in UI_DRIVE_OFF mode\n",__FUNCTION__);
		return;
	}

	if(idx)
	{
		//gStillCB.cap.qvSec = 0;
		gStillCB.remainImg = 3;
		gStillCB.estJobFilesSize = gStillCB.estFileSize*3;
		cap_mode = CAPTURE_MODE_BURST;
	}
	else
	{
		//gStillCB.cap.qvSec = (puiPara->DriverMode == UI_DRIVE_OFF)? 360 : 0;
		gStillCB.remainImg = 1;
		gStillCB.estJobFilesSize = gStillCB.estFileSize;
		cap_mode = CAPTURE_MODE_SINGLE;
	}
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, &cap_mode);
}

void appStill_SetNightScene(
	UINT8 idx
)
{
	UINT32 cap_mode, ret;
	static UINT8 lastMode = 0xff;
	uiPara_t* puiPara = appUiParaGet();

/*	printf("\033[32mIN NightScene SET(%d)(%d)!\033[0m",idx,lastMode);*/

	/*before we set HDR mode(ON or OFF) for capture ,we must enter StandBy mode, else the program will dead*/
	if(lastMode == idx)
	{
		return;
	}
	lastMode = idx;

	if(idx)
	{

		if(puiPara->DriverMode != UI_DRIVE_OFF)
		{
			DBG_PRINT("%s: HDR only availbe in UI_DRIVE_OFF mode\n",__FUNCTION__);
			return;
		}

		//gStillCB.cap.qvSec = 0;
		gStillCB.remainImg = 1;
		gStillCB.estJobFilesSize = gStillCB.estFileSize*1;
		cap_mode = CAPTURE_MODE_SINGLE;
		DBG_PRINT("%s: HDR CAPTURE_MODE_SINGLE\n",__FUNCTION__);

		if (IsHDRNightScene){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}

		/*sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, SP5K_SENSOR_MODE_PREVIEW);*/

		sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, 3);
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,appStillNightSceneCb);
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_ACC );
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_NUM, 8 );
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_ACC_OPT, SP5K_CAPTURE_HDR_ACC_AVERAGE_ON );
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_EV_BIAS, 0 );
		sp5kIqModeSet(SP5K_MODE_STILL_SNAP);

		ret =sp5kResourceFileLoad(SP5K_RES_HDR_WEI,"A:\\RO_RES\\IQ\\WTBL.BIN");

		if(ret == SUCCESS)
		{
			ret =sp5kResourceFileLoad(SP5K_RES_HDR_DIV,"A:\\RO_RES\\IQ\\DIVTBL.BIN");
			if(ret != SUCCESS)
			{
				RPrintf("Load Error2!");
				sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_OFF );
				sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, SNAP_HEAP_RAW_BLOCKS);
				sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,NULL);
			}
		}
		else
		{

			RPrintf("Load Error1!");
			sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_OFF );
			sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, SNAP_HEAP_RAW_BLOCKS);
			sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,NULL);
		}
		sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP);
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);

		appStillViewIqSet();
	}
	else
	{
		printf("close HDR Night Scene setting!\n");
		//gStillCB.cap.qvSec = (puiPara->DriverMode == UI_DRIVE_OFF)? 360 : 0;
		gStillCB.remainImg = 1;
		gStillCB.estJobFilesSize = gStillCB.estFileSize;
		cap_mode = CAPTURE_MODE_SINGLE;

		if (IsHDRNightScene){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}

		/*disable HDR option*/
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_OFF );
		sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, SNAP_HEAP_RAW_BLOCKS);
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,NULL);

		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	}
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, &cap_mode);
}

void
appStill_SetHDR(
	UINT8 idx
)
{
	UINT32 cap_mode, ret;
	static UINT8 lastMode = 0xff;
	uiPara_t* puiPara = appUiParaGet();

/*	printf("\033[32mIN HDR SET(%d)(%d)!\033[0m",idx,lastMode);*/

	/*before we set HDR mode(ON or OFF) for capture ,we must enter StandBy mode, else the program will dead*/
	if(lastMode == idx)
	{
		return;
	}
	lastMode = idx;

	if(idx)
	{
		UINT8 hdrSetErr = 0;
		if(puiPara->DriverMode != UI_DRIVE_OFF)
		{
			DBG_PRINT("%s: HDR only availbe in UI_DRIVE_OFF mode\n",__FUNCTION__);
			return;
		}

		//gStillCB.cap.qvSec = 0;
		gStillCB.remainImg = 1;
		gStillCB.estJobFilesSize = gStillCB.estFileSize*1;
		cap_mode = CAPTURE_MODE_SINGLE;
		DBG_PRINT("%s: HDR CAPTURE_MODE_SINGLE\n",__FUNCTION__);

		if (IsHDR){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}

		/*sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, SP5K_SENSOR_MODE_PREVIEW);*/

		sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, 3);
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,appStillHDRCb);
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_NORMAL );
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_NUM, 2 );
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_ACC_OPT, SP5K_CAPTURE_HDR_ACC_AVERAGE_ON );
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_EV_BIAS, 20 );  /* move to ae.c*/
		sp5kIqModeSet(SP5K_MODE_STILL_SNAP);

		ret =sp5kResourceFileLoad(SP5K_RES_HDR_WEI,"A:\\RO_RES\\IQ\\WTBL.BIN");
		if(ret == SUCCESS)
		{
			ret =sp5kResourceFileLoad(SP5K_RES_HDR_DIV,"A:\\RO_RES\\IQ\\DIVTBL.BIN");
			if(ret != SUCCESS)
			{
				hdrSetErr = 1;
				RPrintf("HDR Load Error2!");
				sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_OFF );
				sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, SNAP_HEAP_RAW_BLOCKS);
				sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,NULL);
			}
		}
		else
		{
			hdrSetErr = 1;
			RPrintf("HDR Load Error1!");
			sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_OFF );
			sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, SNAP_HEAP_RAW_BLOCKS);
			sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,NULL);
		}
		sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP);
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);

		appStillViewIqSet();
		if(!hdrSetErr){
			printf("\033[32mHDR SET ON!\033[0m");
		}
	}
	else
	{
		//gStillCB.cap.qvSec = (puiPara->DriverMode == UI_DRIVE_OFF)? 360 : 0;
		gStillCB.remainImg = 1;
		gStillCB.estJobFilesSize = gStillCB.estFileSize;
		cap_mode = CAPTURE_MODE_SINGLE;

		if (IsHDR){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}

		/*disable HDR option*/
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_HDR_MODE, SP5K_CAPTURE_HDR_MODE_OFF );
		sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_RAW_BUF_NUM, SNAP_HEAP_RAW_BLOCKS);
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_HDR,NULL);

		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	}
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, &cap_mode);
}
/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetSharpness
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetSharpness(
	UINT8 idx
)
{
	UINT8 tmp=4;
	if (idx ==UI_SHARP_MORE)
	{
		tmp =2;
	}
	else if (idx ==UI_SHARP_LESS)
	{
		tmp =6;
	}
	else
	{
		tmp =4;
	}
	appIqSharpnessSet(1,tmp);
	appExif_SetSharpness(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetHight
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetHighlight(
	UINT8 idx
)
{
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDateStamp
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDateStamp(
	UINT32 stamp_in,
	UINT32 dateformat_in
)
{
	UINT16 stampFontH;
	uiPara_t* puiPara = appUiParaGet();
	UINT16 uwImgW = ImgSize[puiPara->ImageSize].w;
	UINT16 uwImgH = ImgSize[puiPara->ImageSize].h;
	UINT32 stamp, dateformat;

	DBG_PRINT("__%s\n",__FUNCTION__);

	if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_16IN1){     /*mantis 0046746*/
		uwImgW = uwImgW * 4;
		uwImgH = uwImgH * 4;
	}

	#if (FIELD_QV_ENABLE	== 1)
		// We can not use field QV if it does some YUV process.
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, (puiPara->StampMode == UI_STAMP_OFF)? 0: 1);
	#else
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	#endif


	// check the input is correct.
	stamp = (stamp_in==0)? puiPara->StampMode : stamp_in;
	//dateformat = (dateformat_in==NULL)? pSetup->dateformat : dateformat_in;
	dateformat = (dateformat_in==0)? SP5K_CAPTURE_DATE_YYYYMMDD: (dateformat_in+1);
	gStampInfo.dateFmtId = dateformat;

	switch(stamp)
	{
		default:
		case UI_STAMP_OFF:
			gStampInfo.dateFmtId = 0xFF;
			break;
		case UI_STAMP_DATE:
			gStampInfo.dateFmtId &= (~SP5K_CAPTURE_DATE_TIME_NO_SECOND);
			break;
		case UI_STAMP_DATE_TIME:
			gStampInfo.dateFmtId |= SP5K_CAPTURE_DATE_TIME_NO_SECOND;
			break;
	}

	/*
	Scale Ratio (100=1x) : (Expected_Size / Font_Size)*100
	where Expect_Size is estimated to be a specific ratio of Image Height
	==> Expect_Size = imgH * (A/100)
	==> ScaleRatio = { [imgH * (A/100)] / Font_Size } * 100
				   = imgH * A / Font_Size
	where A represents the % ratio of the stameHeight vs imagHeight
	*/
	stampFontH = 24; // must use font body size to approach
	gStampInfo.scaleRatio = (uwImgW * 5 * 3 / 4) / stampFontH;

	if (gStampInfo.dateFmtId & SP5K_CAPTURE_DATE_TIME_NO_SECOND)
	{
		gStampInfo.x = (uwImgW * 69 / 100) & (~0x0001) ;
	}
	else
	{
		gStampInfo.x = (uwImgW * 77 / 100) & (~0x0001) ;
	}
	gStampInfo.y = (uwImgH * 85 / 100) & (~0x0001);
#if 0
	if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_16IN1){		/* Mantis 45223 */
		gStampInfo.scaleRatio = 720;
		gStampInfo.x = 2960;
		gStampInfo.y = 2970;
	}
#endif
	sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, (stamp==0)? 0:&gStampInfo);

	DBG_PRINT("stamp=%d\n", stamp);
	DBG_PRINT("uwImgW=%d, uwImgH=%d\n", uwImgW, uwImgH);
	DBG_PRINT("scale=%d, x=%d, y=%d, dataid=0x%x\n", gStampInfo.scaleRatio, gStampInfo.x, gStampInfo.y, gStampInfo.dateFmtId);

}


/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDist
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDist(
	UINT8 idx
)
{
	#ifdef HW_DIST
	if((idx == UI_STABILIZER_ON)&&(gDistEnTag ==FALSE))
	{
		gDistEnTag=TRUE;
		sp5kStillCapDistStart();
	}
	else if ((idx == UI_STABILIZER_OFF)&&(gDistEnTag ==TRUE))
	{
		gDistEnTag =FALSE;
		sp5kStillCapDistStop();
	}

	#endif
}


/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetStabilizer
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetStabilizer(
	UINT8 idx
)
{

	#ifdef HW_DIST
	appStill_SetDist(idx);
	#else
	UINT32 shH, shL;

	shH = pCurBtnPar->shutterBoundary[1];
	if(idx == UI_STABILIZER_ON)
	{
		shL = (SHUTTER_1_60S>pCurBtnPar->shutterBoundary[0]? SHUTTER_1_60S : pCurBtnPar->shutterBoundary[0]);
	}
	else
	{
		shL = pCurBtnPar->shutterBoundary[0];
	}
	appAeCapShutBoundarySet(gStillCB.cap.CurScene, shH, shL);
	#endif
	appAeStabilizerSet(idx);

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetFaceTracking
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetFaceTracking(
	UINT8 command
)
{
#if !CAM_TYPE_CVR || ICAT_FCWS
	int t1;
	t1 = appMsTimeGet();
	switch(command)
	{
	case VIEW_FT_STATUS_DESTROY:
		if(gStillCB.ftStatus != VIEW_FT_STATUS_DESTROY)
		{
			appStillPreviewCbSet(FALSE);
			gStillCB.ftStatus = command;
			printf("Facetracking destroy!\n");
		}
		break;
	case VIEW_FT_STATUS_PAUSE:
		if(gStillCB.ftStatus == VIEW_FT_STATUS_RUN)
		{
			appFdProcPauseEx();
			gStillCB.ftStatus = command;
			printf("Facetracking pause\n");
		}
		break;
	case VIEW_FT_STATUS_RESETDATA_PAUSE:
		if(gStillCB.ftStatus == VIEW_FT_STATUS_RUN)
		{
			appFdProcStopEx();
			gStillCB.ftStatus = command;
			printf("Facetracking stop\n");
		}
		break;
	case VIEW_FT_STATUS_RUN:
		appStillPreviewCbSet(TRUE);
		#if ICAT_FCWS
		appForwardCollisionWarnSysParamSet();
		#endif
		if(gStillCB.ftStatus != VIEW_FT_STATUS_RUN)
		{
			appFdProcStartEx();
			gStillCB.ftStatus = command;

		}
		printf("Facetracking start\n");
		break;
	default:
		printf("%s: unkonw command\n",__FUNCTION__);
		break;
	}
	printf("%s: comsume time =  %d ms\n", __FUNCTION__, appMsTimeGet()-t1);
#else
	command = command;
#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetPhotoFrame
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetPhotoFrame(
	UINT8 idx
)
{
	uiPara_t* puiPara = appUiParaGet();

	sp5kCaptureFrame_t capFrame={0,0};

	printf("\n\n%s: cur=%d, idx=%d\n\n", __func__, gStillCB.curPhotoFrameIdx, idx);
	if(puiPara->ImageSize == UI_PIC_SIZE_16_9
		|| puiPara->ImageSize == UI_PIC_SIZE_3_2
		|| gStillCB.curPhotoFrameIdx == idx)
	{
		return;
	}
	gStillCB.curPhotoFrameIdx = idx;

	#if (FIELD_QV_ENABLE	== 1)
	// We can not use field QV if it does some YUV process.
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, (idx == UI_PHOTOFRAME_OFF)? 0: 1);
	#else
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	#endif

	if (idx != 0)
	{
		UINT8 buf[sizeof(PF_PATH)+20];
		sprintf(buf, PF_PATH "%s", photoFrameTable[idx].pf_big);
		capFrame.pathLen = strlen(buf);
		capFrame.pf = buf;
		sp5kStillCapCfgSet(SP5K_CAPTURE_FRAME, &capFrame);
		appViewDrawPhotoFrame(idx);

		DBG_PRINT("__capFrame big img = %s", capFrame.pf);
		DBG_PRINT("__capFrame small img = %s", buf);
	}
	else
	{
		sp5kStillCapCfgSet(SP5K_CAPTURE_FRAME, NULL);
		appViewDrawPhotoFrame(idx);
	}

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDZoom
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDZoom(
	UINT8 idx
)
{
	//DBG_PRINT("%s: idx=%d",__FUNCTION__, idx);
	if(idx==0)
	{
		appDZoomReset(APP_DZOOM_UI_MODE_STILL_VIEW);
		appDZoomReset(APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
	appDZoomEnableCtrl(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetFlash
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetFlash(
	UINT8 idx
)
{
	BOOL isStorbeCharging = appStrobeQueryChargeEnable();

	//DBG_PRINT("appStrobeQueryChargeEnable() = %d\n",isStorbeCharging);
	if(idx==FLASH_CHARGE_STOP)
	{
		appStrobeChargeStop();
		GREEN_LED_ON
	}
	else if(idx==UI_FLASH_OFF && isStorbeCharging)
	{
		DBG_PRINT("stop strobe charge and power off\n");
		appStrobeChargeStop();
		appStrobePowerOff();
		GREEN_LED_ON
		if(gStillCB.flashChargeMonitorTimer)
		{
			appTimerClear(&gStillCB.flashChargeMonitorTimer);
		}
	}
	else if(idx!=UI_FLASH_OFF && !isStorbeCharging)
	{
		DBG_PRINT("start strobe charge\n");
		appStrobePowerOn();
		appStrobeChargeStart();
		LED_FLICKER_STROBE_CHARGE
	}
	appSnapFlashUiModeSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetAF
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetAF(
	UINT8 idx
)
{
	appFocusModeChange(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetShutterSpeed
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetShutterSpeed(
	UINT8 idx
)
{
	appAeCapShutterSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetApertureValue
 *  Description :   change the f number.
 *------------------------------------------------------------------------*/
void
appStill_SetApertureValue(
	UINT8 idx
)
{
	appAeCapIrisSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetQV
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetQV(
	UINT32 qv
)
{
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, &qv);

}


/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetConfirmTimeOut
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetConfirmTimeOut(
	UINT32 time
)
{
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, 0, time);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetCaptureConfirmStore
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetCaptureConfirmStore(
	UINT32 storeflag
)
{
	sp5kStillCapStoreConfirm(storeflag);
}


/*-------------------------------------------------------------------------
 *  Function Name : appStill_CfgCaptureConfirmStore
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_CfgCaptureConfirmStore(
	UINT32 enable
)
{
	/*0: auto store ,1:manually confirm*/
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CONFIRM_STORE, enable);
}


/*-------------------------------------------------------------------------
 *  Function Name : appStill_FindCurBtnParam
 *  Description :
 *------------------------------------------------------------------------*/
void appStill_FindCurBtnParam(	UINT8 view_idx,UINT8 scene_idx)
{
	UINT32 i, idx, size;

	idx = (view_idx==SCENE_MODE)? (PASM_MAX+ scene_idx):(view_idx);
	size = SCENE_MAX+PASM_MAX;
	for(i=0; i<size; i++)
	{
		if(idx == buttonParam[i].id)
		{
			pCurBtnPar = &(buttonParam[i]);
			break;
		}
	}
	DBG_PRINT("%s: view=%d, scene=%d, pCurBtnPar->id = %d",__FUNCTION__, view_idx, scene_idx, pCurBtnPar->id);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_FlashChange
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeFlash(void)
{
	uiPara_t* puiPara = appUiParaGet();

	if(pCurBtnPar==0)
	{
		appStill_FindCurBtnParam(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
	}

	flash_idx++;
	if(pCurBtnPar->flash[flash_idx] == UI_FLASH_MAX)
	{
		flash_idx = 0;
	}
	puiPara->flash= pCurBtnPar->flash[flash_idx];
	gStillCB.flashMode = puiPara->flash;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ChangeAf
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeAf(void)
{
	UINT32 idx, viewMode = gStillCB.cap.CurViewMode;
	uiPara_t* puiPara = appUiParaGet();

	if(pCurBtnPar==0)
	{
		appStill_FindCurBtnParam(viewMode, gStillCB.cap.CurScene);
	}

	if(viewMode == SCENE_MODE)
	{
		focus_idx++;
		if(pCurBtnPar->focus[focus_idx] == UI_FOCUS_MAX)
		{
			focus_idx = 0;
		}
		puiPara->af = pCurBtnPar->focus[focus_idx];
	}
	else
	{
		idx = puiPara->af+1;
		puiPara->af = (idx >= UI_FOCUS_MAX)? 0: idx;
	}
}
/*-------------------------------------------------------------------------
 *  Function Name : appStill_ChangeShutterSpeed
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeShutterSpeed(
	UINT32 updownkey
)
{
	uiPara_t* puiPara = appUiParaGet();

	DBG_PRINT("_%s: in\n", __FUNCTION__);
	if(updownkey)
	{
		puiPara->shutter--;
	}
	else
	{
		puiPara->shutter++;
	}

	if(puiPara->shutter>SHUTTER_1_2000S)
	{
		puiPara->shutter=SHUTTER_8S;
	}
	else if(puiPara->shutter<SHUTTER_8S)
	{
		puiPara->shutter=SHUTTER_1_2000S;
	}
	appStill_SetShutterSpeed(puiPara->shutter);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ChangeApertureValue
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeApertureValue(
	UINT32 updownkey
)
{
	pViewParam->aperture = (pViewParam->aperture == APERTURE_2_8)?
		APERTURE_5_6 : APERTURE_2_8;
	appStill_SetApertureValue(pViewParam->aperture);

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetExpFlickerMoe
 *  Description :   change to 50HZz/60Hz
 *------------------------------------------------------------------------*/
void
appStill_SetExpFlickerMode(
	UINT8 idx
)
{
	appAeExpFlickerModeSet(idx);
}


/*-------------------------------------------------------------------------
 *  Function Name : appStill_GetViewParam
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
appStill_GetViewParam(
	UINT8 view_idx,
	UINT8 scene_idx
)
{
	UINT32 viewParamIdx=0;
	uiPara_t* puiPara = appUiParaGet();

	switch(view_idx)
	{
	case PASM_PROGRAM:
		if(puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
			viewParamIdx = PRM_MODE_SCENE_NOT_PEOPLE;
		else if(puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT)
			viewParamIdx = PRM_MODE_SMILE;
		else
			viewParamIdx = PRM_MODE_P;
		break;
	case PASM_APERTURE:
		viewParamIdx = PRM_MODE_A;
		break;
	case PASM_SHUTTER:
		viewParamIdx = PRM_MODE_S;
		break;
	case PASM_MANUAL:
		viewParamIdx = PRM_MODE_M;
		break;
	case SCENE_MODE:
		switch(scene_idx)
		{
		case SCENE_SMILE_DETECTION:
			viewParamIdx = PRM_MODE_SMILE;
			break;
		default:
		case SCENE_BACK_LIGHT:
		case SCENE_CHILDREN:
		case SCENE_PORTRAIT:
		case SCENE_BEACH_SNOW:
			viewParamIdx = PRM_MODE_SCENE_PEOPLE;
			break;
		case SCENE_SHAKE_FREE:
		case SCENE_LANDSCAPE:
		case SCENE_NIGHT:
		case SCENE_BUILDING:
		case SCENE_FOOD:
		case SCENE_TEXT:
		case SCENE_FIRE_WORKS:
		case SCENE_SUNSET:
		case SCENE_WDR	:
		case SCENE_HDR :
		case SCENE_PANORAMA :
		case SCENE_BLINK:
			viewParamIdx = PRM_MODE_SCENE_NOT_PEOPLE;
			break;
		}
		break;
		#if PANORAMA_MODE
			case SCENE_PANORAMA:
			viewParamIdx = PRM_MODE_SCENE_NOT_PEOPLE ;
			break;
	#endif
	case VIDEO_MODE:
		viewParamIdx = PRM_MODE_VIDEO;
		break;
	}
	DBG_PRINT("viewParamIdx= %d\n", viewParamIdx);

	return viewParamIdx;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetPasmSceneParam
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetPasmSceneParam(
	UINT8 view_idx,
	UINT8 scene_idx
)
{
	static UINT32 lastView=0xffff;
	static UINT32 lastScene=0xffff;
	uiPara_t* puiPara = appUiParaGet();

	if(lastView == view_idx && lastScene == scene_idx)
	{
		return;
	}
	lastView = view_idx;
	lastScene = scene_idx;

		/* init mode value */
	appStill_FindCurBtnParam(view_idx, scene_idx);
	printf("\033[32mScene SET(%u)(%u)\033[0m\n",view_idx,lastScene);
	if(view_idx == SCENE_MODE)
	{
		focus_idx=0;
		if(lastScene == SCENE_HDR )
		{
			IsHDR= TRUE;
			IsHDRNightScene = FALSE;
			appStill_SetNightScene(FALSE);
			appStill_CfgCaptureConfirmStore(FALSE);
			appStill_SetHDR(TRUE);
		}
		else if(lastScene == SCENE_NIGHT )
		{
			IsHDRNightScene = TRUE;
			IsHDR= FALSE;
			appStill_SetHDR(FALSE);
			appStill_CfgCaptureConfirmStore(FALSE);
			appStill_SetNightScene(TRUE);
		}
		else if(lastScene == SCENE_WDR )
		{
			puiPara->WDRMode= TRUE;
			appStill_CfgCaptureConfirmStore(FALSE);
			appIqSetWdr(puiPara->WDRMode);
		}
		else if(lastScene == SCENE_BLINK || puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
		{
			if(puiPara->FaceTracking != UI_FACETRACK_BLINKING_DETECT)
				puiPara->FaceTracking = UI_FACETRACK_ON ;
			appStill_SetFaceTracking(VIEW_FT_STATUS_RUN);
			appStill_CfgCaptureConfirmStore(TRUE);
			gStillCB.cap.CurScene = SCENE_BLINK ;
			appStillYuvCbSet(TRUE);
		}
		else
		{
			IsHDR= FALSE;
			IsHDRNightScene = FALSE;
			appStill_SetNightScene(FALSE);
			appStill_SetHDR(FALSE);
			puiPara->WDRMode= FALSE;
			appIqSetWdr(puiPara->WDRMode);
			appStill_CfgCaptureConfirmStore(FALSE);
		}
	}
	else
	{/*fan.xiao add for close the HDR mode when we change from HDR/Night scene mode  to other  program mode(not the sceness mode)*/
		IsHDR = FALSE;
		IsHDRNightScene= FALSE;
		appStill_SetNightScene(FALSE);
		appStill_SetHDR(FALSE);
	}

	if(view_idx != PASM_PROGRAM)
	{
		flash_idx=0;
	}

	/* reset view param */
	#if ICAT_WIFI && SPCA6330 /* mantis bug #47002, #47004 , view param setting issue */
	if(!appRtpStreamStateGet())
	#endif
		UiParamViewChangeMode(appStill_GetViewParam(view_idx, scene_idx));

	#if ICAT_WIFI && SPCA6330
	/*appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_DAILSW,appStill_GetViewParam(view_idx, scene_idx),0);*/
	#endif
	appIqEffectSet(puiPara->ImageEffect);
	if (view_idx == SCENE_MODE)
	{
		puiPara->af = pCurBtnPar->focus[0];
		if(pCurBtnPar->metering[0] != UI_METER_MAX)
		{
			puiPara->AEMetering= pCurBtnPar->metering[0];
		}
		if(pCurBtnPar->sharpness != UI_SHARP_MAX)
		{
			pViewParam->edge =puiPara->Sharpness;
		}
		if(pCurBtnPar->stablizer != UI_STABILIZER_MAX)
		{
			puiPara->Stabilizer = pCurBtnPar->stablizer;
		}
		if(pCurBtnPar->facetracking != UI_FACETRACK_MAX)
		{
			puiPara->FaceTracking = pCurBtnPar->facetracking;
			if(lastScene == SCENE_BLINK)
			{
				puiPara->FaceTracking = UI_FACETRACK_BLINKING_DETECT ;
			}
		}
	}

	if (view_idx != PASM_PROGRAM)
	{
		puiPara->flash = pCurBtnPar->flash[0];
	}
	gStillCB.flashMode = (puiPara->DriverMode == UI_DRIVE_BURST)? UI_FLASH_OFF : puiPara->flash;

	appStill_SetAEB(puiPara->AEB);
	appStill_SetExposure(puiPara->AEMode);

	/* ae relative set */
	if(view_idx == VIDEO_MODE)
	{
		appAePreviewShutBoundarySet(view_idx, SHUTTER_1_500S, SHUTTER_1_30S);
	}
	else
	{
		appAePreviewShutBoundarySet(view_idx==SCENE_MODE? PASM_PROGRAM : view_idx, SHUTTER_1_16000S, SHUTTER_1_15S);
		appAePreviewIsoBoundarySet(view_idx==SCENE_MODE? PASM_PROGRAM : view_idx,STD_ISO_640,STD_ISO_100);
		appStill_SetShutterSpeed(puiPara->shutter);
		appStill_SetApertureValue(pViewParam->aperture);
	}
	appAeCapShutBoundarySet(scene_idx, pCurBtnPar->shutterBoundary[1], puiPara->Stabilizer?
		SHUTTER_1_60S>pCurBtnPar->shutterBoundary[0]? SHUTTER_1_60S : pCurBtnPar->shutterBoundary[0] : pCurBtnPar->shutterBoundary[0]);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ResetAllParam()
 *  Description :   This routine is responsible to reset the parameters in
 *					pViewParam, gStillCB and gVideoCB.
 *------------------------------------------------------------------------*/
void
appStill_ResetAllParam(void)
{
	/*uiPara_t* puiPara = appUiParaGet();*/

	UiParamViewReset();

	focus_idx = 0;
	flash_idx = 0;

	/*if(IS_VIDEO_MODE)
	{
		appVideoSettingInit(VIDEO_SET_ALL);
		appVidSizeSet(puiPara->VideoSize);
	}
	else if(IS_PASM_OR_SCENE_MODE)
	{
		appStillSettingInit(VIEW_SET_ALL);
		appStill_SetResolution(puiPara->ImageSize);
	}*/
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_GetCurSize()
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_GetCurSize(
	UINT32* w,
	UINT32* h
)
{
	uiPara_t* puiPara = appUiParaGet();

	*w = ImgSize[puiPara->ImageSize].w;
	*h = ImgSize[puiPara->ImageSize].h;
}

void
appStill_SetMiniatureStatus(
BOOL en
)
{
	gbMiniatureEn = en;
}

BOOL
appStill_GetMiniatureStatus(
 void
)
{
	return gbMiniatureEn;
}

#if OLD_TLS_OPT
/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetTimeLapse()
 *  Description :
 *------------------------------------------------------------------------*/
#if !CAM_TYPE_CVR

void appStill_SetTimeLapse(UINT32 idx)
{
	UINT32 time=0;

	switch(idx)
	{
		case UI_TIMELAPSE_1SEC:
			time = 1;
			break;
		case UI_TIMELAPSE_3SEC:
			time = 3;
			break;
		case UI_TIMELAPSE_5SEC:
			time = 5;
			break;
		case UI_TIMELAPSE_1MIN:
			time = 60;
			break;
		case UI_TIMELAPSE_5MIN:
			time = 300;
			break;
		case UI_TIMELAPSE_OFF:
		default:
			break;
	}
	timelapseGap = 1000*time;
}

#else
void appStill_SetTimeLapse(UINT32 idx)
{
	UINT32 time=0;

	switch(idx)
	{
		case UI_TIMELAPSE_OFF:
		default:
			break;
		case UI_TIMELAPSE_10SEC:
			time = 10;
			break;
		case UI_TIMELAPSE_20SEC:
			time = 20;
			break;
		case UI_TIMELAPSE_30SEC:
			time = 30;
			break;
		case UI_TIMELAPSE_1MIN:
			time = 60;
			break;
		case UI_TIMELAPSE_5MIN:
			time = 300;
			break;

		case UI_TIMELAPSE_10MIN:
			time = 600;
			break;
		case UI_TIMELAPSE_30MIN:
			time = 1800;
			break;
		case UI_TIMELAPSE_60MIN:
			time = 3600;
			break;
	}
	timelapseGap = 1000*time;
}
#endif

#else
void appStill_SetTimeLapse(UINT32 idx)
{
	UINT32 time=0;

	switch(idx)
	{
		case UI_TIMELAPSE_OFF:
		default:
			break;
		case UI_TIMELAPSE_1SEC:
			time = 1;
			break;
		case UI_TIMELAPSE_5SEC:
			time = 5;
			break;
		case UI_TIMELAPSE_10SEC:
			time = 10;
			break;
		case UI_TIMELAPSE_30SEC:
			time = 30;
			break;
		case UI_TIMELAPSE_1MIN:
			time = 60;
			break;
		case UI_TIMELAPSE_10MIN:
			time = 600;
			break;
		case UI_TIMELAPSE_30MIN:
			time = 1800;
			break;
		case UI_TIMELAPSE_60MIN:
			time = 3600;
			break;
	}
	timelapseGap = 1000*time;
}
#endif

UINT32 appTimeLapseSecGet(void)
{
	return timelapseGap;
}


/*-------------------------------------------------------------------------
 *  Function Name : cmdSnap
 *  Description :   command line test function
 *------------------------------------------------------------------------*/
void
cmdSnap(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	UINT32 temp;
	WbOffset_t wb;
	uiPara_t* puiPara = appUiParaGet();

	if (argc==1)
	{
		printf("Snap usage:\n"
			" snap drive [0|1|2|3|4] 		: ??\n"
			" snap quality [0|1|2] 			: normal/fine/best\n"
			" snap size [7|169|32|5|3|1|0]	: 7M~VGA\n"
			" snap stamp [0|1|2] 			: off/date/date&time\n"
			" snap aeb [0|1] 				: off/on\n"
			" snap edge [0|1|2] 			: hard/middle/soft\n"
			" snap effect []				: ???\n"
			" snap qv []                    : quick view time in sec\n"
			" snap bg                       : show background info\n"
			" snap dzoom []                 : enable dzoom\n"
			" snap param                    : show pViewParam members\n"
			" snap mode [view] [scene]      : p/a/s/m/sc = 0~4 \n"
			" snap strobe [width] [position] [cycle]\n"
		);
	}
	else if (strcmp(arg[1], "drive")==0)
	{
		puiPara->DriverMode = v[2];
	}
	else if (strcmp(arg[1], "quality")==0)
	{
		puiPara->ImageQuality= v[2];
	}
	else if (strcmp(arg[1], "size")==0)
	{
		switch (v[2])
		{
			default:
			case 7: temp=0; break;
			case 169: temp=1; break;
			case 32: temp=2; break;
			case 5: temp=3; break;
			case 3:	temp=4; break;
			case 1: temp=5; break;
			case 0:	temp=6; break;
		}
		puiPara->ImageSize = temp;
	}
	else if (strcmp(arg[1], "stamp")==0)
	{
		puiPara->StampMode = v[2];
	}
	else if (strcmp(arg[1], "aeb")==0)
	{
		puiPara->AEB = v[2];
	}
	else if (strcmp(arg[1], "edge")==0)
	{
		puiPara->Sharpness = v[2];
	}
	else if (strcmp(arg[1], "effect")==0)
	{
		puiPara->ImageEffect = v[2];
	}
	else if (strcmp(arg[1], "qv")==0)
	{
		gStillCB.cap.qvSec = v[2];
	}
	else if (strcmp(arg[1], "bg")==0)
	{
		printf("not support \n");
	}
	else if (strcmp(arg[1], "dzoom")==0)
	{
		appStill_SetDZoom(v[2]);
	}
	else if (strcmp(arg[1], "param")==0)
	{
		printf("puiPara->ImageSize=%d\n",puiPara->ImageSize);
		printf("puiPara->VideoSize=%d\n",puiPara->VideoSize);
		printf("puiPara->quality=%d\n",puiPara->ImageQuality);
		printf("puiPara->AEMetering=%d\n",puiPara->AEMetering);
		printf("puiPara->WBMode=%d\n",puiPara->WBMode);
		printf("puiPara->ISO=%d\n",puiPara->Iso);
		printf("puiPara->AEMode=%d\n",puiPara->AEMode);
		printf("puiPara->riverMode=%d\n",puiPara->DriverMode);
		printf("puiPara->AEB=%d\n",puiPara->AEB);
		printf("pViewParam->highlight=%d\n",pViewParam->highlight);
		printf("puiPara->Sharpness=%d\n",puiPara->Sharpness);
		printf("puiPara->ImageEffect=%d\n",puiPara->ImageEffect);
		printf("puiPara->StamoMode=%d\n",puiPara->StampMode);
		printf("puiPara->Stabilizer=%d\n",puiPara->Stabilizer);
		printf("puiPara->FaceTracking=%d\n",puiPara->FaceTracking);
		printf("puiPara->PhotoFrame=%d\n",puiPara->PhotoFrame);
		printf("puiPara->DigitalZoom=%d\n",puiPara->DigitalZoom);
		printf("puiPara->af=%d\n",puiPara->af);
		printf("puiPara->shutter=%d\n",puiPara->shutter);
		//printf("pViewParam->macro=%d\n",pViewParam->macro);
		printf("puiPara->flash=%d\n",puiPara->flash);
		printf("puiPara->osd=%d\n",pViewParam->osd);
	}
	else if (strcmp(arg[1], "mode")==0)
	{
		appStill_SetPasmSceneParam(v[2], v[3]);
	}
	else if (strcmp(arg[1], "ft")==0)
	{
		extern void appStillOsd_AllFacesDraw(BOOL bShow);
		extern void appStillOsd_MainFaceDraw(BOOL idx);

		static int toggle=0;
		appStillOsd_AllFacesDraw(TRUE);
		toggle = toggle? 0:1;
		appStillOsd_MainFaceDraw(toggle);
		return;
	}
	else if (strcmp(arg[1], "osdtest")==0)
	{
		extern void appStillOsd_TestDraw(void);
		appStillOsd_TestDraw();
		return;
	}
	else if (strcmp(arg[1], "osdt2")==0)
	{
		extern void appViewOsd_ModeIconTest(void);
		appViewOsd_ModeIconTest();
		return;
	} else if (strcmp(arg[1], "strobeget")==0) {
		extern void appStillStrobeStateGet(void);
		appStillStrobeStateGet();
		return;
	} else if (strcmp(arg[1], "strobe")==0)
	{
		extern void appStillStrobeStateSet(UINT32 flashWidth, UINT32 flashHPos, UINT32 testTime);
		if(v[2]) {
			appStillStrobeStateSet(v[2], v[3], v[4]);
		}
		sp5kHostMsgSend(APP_UI_MSG_VIEW_STROB_TEST_START, 0);
		return;
	}
	else if (strcmp(arg[1], "wboffset")==0)
	{

		wb.roffset = -AE_DEFAULT_OB - v[2]*600;
		wb.groffset = -AE_DEFAULT_OB - v[3]*600;
		wb.boffset = -AE_DEFAULT_OB - v[4]*600;
		wb.gboffset = -AE_DEFAULT_OB - v[5]*600;
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_R ,wb.roffset);
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GR, wb.groffset);
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_B , wb.boffset);
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GB, wb.gboffset);
		return;
	}
	else if (strcmp(arg[1], "showflash")==0)
	{
		printf("pViewParam->flash=%d, gStillCB.flashMode=%d\n",puiPara->flash, gStillCB.flashMode);
		return;
	}
	else if (strcmp(arg[1], "awbspeed")==0)
	{
		sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, v[2]);
		return;
	}
	else if (strcmp(arg[1], "afwin")==0)
	{
		sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST,v[2]);
		return;
	}
	else if (strcmp(arg[1], "flash_ch")==0)
	{
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 36);//1
		IO_STROBE_CHARGE_OFF; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);	//2
		IO_STROBE_CHARGE_OFF; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);	//3
		IO_STROBE_CHARGE_OFF; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);	//4
		return;
	}
	else if (strcmp(arg[1], "image")==0)
	{
		sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
		sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
		return;
	}

	appStillSettingInit(VIEW_SET_ALL);
}

UINT32 appStill_CustumStamp(
	UINT32 addr,
	UINT32 qvAddr,
	void* size
)
{
    UINT32 basew, baseh, x, y, w, h, ret=0;
    sp5kGfxPicture_t srcPic, dstPic;
    sp5kGfxPageCapabDesc_t cap;
    sp5kGfxObj_t gfxObjSrc,gfxObjDst;

    UINT8 stillStampDateBuf[128];

    const sensorTgSnapCfg_t *pSensorTgSnapTemp = psensorTgSnapCfgGet();

    appOsdSFNFileLoad(SP5K_RES_DATE_STAMP_OSD, "UI\\FONT\\DATESTAMP_48.SFN");

    UINT16 SnapRawSize_H = pSensorTgSnapTemp->hsize;
    UINT16 SnapRawSize_V = pSensorTgSnapTemp->vsize;


    printf("StampSize_H = %d, StampSize_V = %d\n", SnapRawSize_H, SnapRawSize_V);
    //stillStampDateBuf = sp5kMalloc(pSensorTgSnapTemp->hsize*pSensorTgSnapTemp->vsize);

    ConvertBuf = sp5kYuvBufferAlloc(SnapRawSize_H, (SnapRawSize_V+15)&(~15));

    printf("(SnapRawSize_V+15)&(~15) = %d\n", (SnapRawSize_V+15)&(~15));

    /* Get RTC string and change the font */
    sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtcn);
    //appVideoRecStampStringGet();
    sprintf(stillStampDateBuf,  "%d/%02d/%02d  %02d:%02d:%02d",rtcn.tmx_year+1900, rtcn.tmx_mon,rtcn.tmx_mday,rtcn.tmx_hour, rtcn.tmx_min, rtcn.tmx_sec);
    sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_DATE_STAMP_OSD, 0, 0, 0);

    #if 0
    /* String Scale */
    sp5kDispPvRegionGet(&x, &y, &w, &h);
    printf("x = %d, y = %d, w = %d, h = %d\n", x, y, w, h);
    basew = 320;
    if (w * 3 == h * 4)  /*4:3*/
    {
        baseh = 240;
    }
    else if (w * 9 == h * 16)  /*16:9*/
    {
        baseh = 180;
    }
    else
    {
        baseh = 240;
    }
    #endif

    stillStampDrawRatioW = 1000;
    stillStampDrawRatioH = 1000;



    //printf("stillStampDrawRatioW = %d, stillStampDrawRatioH = %d\n", stillStampDrawRatioW, stillStampDrawRatioH);
    /* Draw OSD page & string scale */
    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
    sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR);
    appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
    sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, stillStampDrawRatioW, stillStampDrawRatioH, 0, 0);
    sp5kGfxStringDraw(SP5K_GFX_PAGE_PIP_0, 20, 20, SP5K_GFX_ALIGN_TOP_LEFT, STR_ENCODE_ASCII, stillStampDateBuf); /*Stamp String*/
    //sp5kGfxIconDraw(SP5K_GFX_PAGE_PIP_0, 20, 20, SP5K_GFX_ALIGN_TOP, ID_ICON_BATTERY_FULL);
    //sp5kGfxIconDraw(SP5K_GFX_PAGE_PIP_0, 150, 150, SP5K_GFX_ALIGN_TOP, ID_ICON_BATTERY_FULL);   /*Stamp Icon*/

    sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_PIP_0, &cap);

    printf("cap.frmW = %d\ncap.frmH = %d\ncap.frmRoiX = %d\ncap.frmRoiY = %d\n", cap.frmW, cap.frmH, cap.frmRoiX, cap.frmRoiY);
    printf("cap.frmRoiW = %d\ncap.frmRoiH = %d\ncap.winX = %d\ncap.winY = %d\n", cap.frmRoiW, cap.frmRoiH, cap.winX, cap.winY);
    printf("cap.winW = %d\ncap.winH = %d\ncap.type = %d\ncap.fmt = %d\n", cap.winW, cap.winH, cap.type, cap.fmt);
    printf("cap.isShow = %d\ncap.alpha = %d\n", cap.isShow, cap.alpha);

    /* Gfx Object Convert */
    gfxObjSrc.pbuf = cap.pbuf;
    gfxObjSrc.roiX = gfxObjSrc.roiY = 0;
    gfxObjSrc.bufW = cap.frmW;
    gfxObjSrc.bufH = cap.frmH;
    gfxObjSrc.roiW = cap.frmW;
    gfxObjSrc.roiH = 128/*48*/;
    gfxObjSrc.paplaneBuf = NULL;
    gfxObjSrc.fmt = SP5K_GFX_FMT_RGB565;

    gfxObjDst.pbuf = ConvertBuf;
    gfxObjDst.roiX = gfxObjDst.roiY = 0;
    gfxObjDst.bufW = SnapRawSize_H;
    gfxObjDst.bufH = (SnapRawSize_V+15)&(~15);

    gfxObjDst.roiW = gfxObjSrc.roiW;
    gfxObjDst.roiH = gfxObjSrc.roiH;
    gfxObjDst.paplaneBuf = NULL;
    gfxObjDst.fmt = SP5K_GFX_FMT_YUV422;


    sp5kGfxObjectConvert(&gfxObjSrc, &gfxObjDst);

    fsSimpleWrite("D:\\CAP.YUV", gfxObjDst.pbuf, gfxObjDst.bufW*gfxObjDst.bufH * 2);

    /* YUV & OSD blend */
    srcPic.pbuf = gfxObjDst.pbuf;
    srcPic.frmW = gfxObjDst.bufW;
    srcPic.frmH = gfxObjDst.bufH;
    srcPic.roiX = 0;
    srcPic.roiY = 0;
    srcPic.roiW = gfxObjSrc.roiW - 384;	/* it must be multiple of 16x and multiple of roiW of src and dst can't over 12x */
    srcPic.roiH = gfxObjSrc.roiH;	/* it must be multiple of 16x and multiple of roiH of src and dst can't over 12x */
    srcPic.fmt  = SP5K_GFX_FMT_YUV422;

    dstPic.pbuf = (UINT8*)addr;
    dstPic.frmW = SnapRawSize_H; /*psSizeInfo->width;*/
    dstPic.frmH = SnapRawSize_V;  /*psSizeInfo->height;*/
    dstPic.roiX = 384;
    dstPic.roiY = 0;
    dstPic.roiW = srcPic.roiW - 384/*uwImgW*/;
    dstPic.roiH = srcPic.roiH/*uwImgH*/;
    dstPic.fmt  = SP5K_GFX_FMT_YUV422;

#if 1
    icatch5330YuvWrite("D:\\DST.YUV", dstPic.pbuf, dstPic.frmW, dstPic.frmH, 0);
    fsSimpleWrite("D:\\SRCGFX.YUV", srcPic.pbuf, srcPic.frmW * srcPic.frmH* 2);
#endif

    ret = sp5kGfxPictureBlend(&dstPic, 128, 1, 0, &srcPic);

    sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0+1, PAL_NOCOLOR);
    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);
    sp5kYuvBufferFree(ConvertBuf);
    //osMemFree(stillStampDateBuf);
    sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
    return ret;
}

void SampleCode_CaptureDateStamp(void)
{
    sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_YUV, appStill_CustumStamp);
    sp5kModeSet(SP5K_MODE_STILL_SNAP);
}


