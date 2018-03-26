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
#define HOST_DBG 0
#include <stdarg.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_ae_api.h"
#include "app_exif.h"
#include "app_version.h"
#include "app_aaa.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_still.h"
#include "app_lens_api.h"
#include "app_zoom_api.h"
#include "middleware/Pwrlog.h"
#include "app_aaa.h"
#include "sp5k_capture_api.h"
#include "sp5k_media_api.h"
#include "app_ui_para.h" 
#include "app_gps.h"  


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #if 0
#define APP_EXIF_MAKE	"Digital Camera Corporation"
#define APP_EXIF_MODEL	"Digital Camera"
#else
#define APP_EXIF_MAKE	"MEDION AG"
#define APP_EXIF_MODEL	"MD 87156"
#endif
#if 0
#define APP_EXIF_COPYRIGHT "Digital Camera"
#define APP_EXIF_MAKERNOTE "Digital Camera"
#define APP_EXIF_VERSION "0220"

#define MAKERNOTE_MAKER	"Digital Camera" 
#define MODEL_NAME	"CVR"
#define MAKE_MODEL			"Digital Camera" MODEL_NAME
#else
#define APP_EXIF_COPYRIGHT "Copyright 2008"
#define APP_EXIF_MAKERNOTE "Maker Note"
#define APP_EXIF_VERSION "0220"

#define MAKERNOTE_MAKER	"MEDION AG" 
#define MODEL_NAME			"MD 87156"
#define MAKE_MODEL			"MEDION AG" MODEL_NAME

#endif
unsigned char RiffStrdStart[12] = { 0x41, 0x56, 0x49, 0x46, 0x01, 0x1E, 0x02, 0x02, 0x07, 0x00};	/*AVIF....*/


#define RIFF_PROFILE_IMAGESIZE_INDEX	4
#define RIFF_PROFILE_FRAMERATE_INDEX	5
#define RIFF_PROFILE_SOUNDINFO_INDEX    6
#define RIFF_MAKERNOTE_IFD_NUM 		6

#define RIFF_PROFILE_IMAGESIZE_320		0x01
#define RIFF_PROFILE_IMAGESIZE_640		0x02
#define RIFF_PROFILE_IMAGESIZE_1280     	0x03
#define RIFF_PROFILE_IMAGESIZE_1920     	0x04

#define RIFF_PROFILE_SOUNDINFO     	0x01

#define RIFF_PROFILE_FRAMERATE_30FPS     	0x1E
#define RIFF_PROFILE_FRAMERATE_60FPS     	0x3C

#define STRD_TAG_TYPE_BYTE              1
#define STRD_TAG_TYPE_ASCII             2
#define STRD_TAG_TYPE_SHORT            3
#define STRD_TAG_TYPE_LONG              4


#define F_NO_X100		280
#define	MAX_APER_X100	300 /* 3.0=F#2.8 */
#define FOCAL_LEN_X100	460
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	UINT32 bufSize;
	UINT8 *bufBase;
	UINT8 *bufWPtr;
} exifMNBufCB_t;

typedef struct _Exif_tag
{
	unsigned short TagCode;
	unsigned short Type;
	UINT32	Count;
	UINT32	Value;
}SSTRDTAG, *PSSTRDTAG;

enum 
{
	POTHIFD_MAKE,
	POTHIFD_MODEL,
	POTHIFD_VIDEODATESTAMP,
	POTHIFD_MAX,
};


typedef struct _Strd_Chuck
{
	unsigned char StrdStart[12];
	SSTRDTAG P0thIfd[POTHIFD_MAX];
	unsigned char Makenote[sizeof(MAKERNOTE_MAKER)];
	unsigned char MakeModel[sizeof(MAKE_MODEL)];
}SSTRDCHUNK, *PSTRDCHUNK;


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if SPCA5110 && !SPCA5210
#define EXIF_MN_IQ_BUF_SIZE		128
#else
#define EXIF_MN_IQ_BUF_SIZE		128
#endif
#define EXIF_MN_PFH_BUF_SIZE	(128)
#define EXIF_MN_AF_BUF_SIZE		(128)
#define EXIF_MN_AE_BUF_SIZE		(512)
#define EXIF_3A_DEBUG_INFO_SIZE (32*1024)
#define EXIF_MN_BUF_SIZE	(EXIF_MN_IQ_BUF_SIZE+EXIF_MN_PFH_BUF_SIZE+EXIF_MN_AF_BUF_SIZE+EXIF_MN_AE_BUF_SIZE+EXIF_3A_DEBUG_INFO_SIZE)
static UINT8 mnBuf[EXIF_MN_BUF_SIZE]={0}; // maker note can record the 3a/iq data for debug

UINT32 aperture_x100[7][2]={
		{280, 560},
		{310, 630},
		{340, 690},
		{380, 760},
		{420, 850},
		{470, 950},
		{520, 1040}
};

UINT32 shutterSpeed[20][2]={
		{0,1},
		{16,1},
		{8,1},
		{4,1},
		{2,1},
		{1,1},
		{1,2},
		{1,4},
		{1,8},
		{1,15},
		{1,30},
		{1,60},
		{1,125},
		{1,250},
		{1,500},
		{1,1000},
		{1,2000},
		{1,4000},
		{1,8000},
		{1,16000},
};


static exifMNBufCB_t mnBufCB[EXIF_MN_TOTAL];
SSTRDTAG P0thIfd[sizeof(SSTRDCHUNK)] = {{0}};
unsigned char pAviStrdChuck[sizeof(SSTRDCHUNK)];

UINT32	u32VariableDataOffset = sizeof(SSTRDTAG)*POTHIFD_MAX;
UINT32	uVariableDataGlobalOffset = 2;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void aeExif_DbgLogInit(void);
void aeExif_DbgLogSave(void);
extern void exifAscRTCGet(UINT8 *ptimeBuf);
extern UINT32 isoValue[];
extern void flashExif_DbgLogInit(void);
extern void flashExif_DbgLogSave(void);
extern UINT8* awb_db_buf;
extern UINT32 appAWBALG_WriteAppn(UINT32 addr, UINT32 size); // for clean warning
extern UINT32 appAEALG_WriteAppn(UINT32 addr, UINT32 size); // for clean warning
#if PRECOMPOSITION  
extern void *osMemAlloc(UINT32 size);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32
AppExifWriteAppnCb(
	UINT32 addr,
	UINT32 size,
	sp5kExifAppnCbInfo_t *pinfo
)
{
	static UINT32 flag = 0;
	static UINT32 leftSize, saveSize;
	UINT32 awbSize=0, aeSize=0;

	if (flag == 2) {
#if SP5K_ALGORITHM_AWB || SP5K_ALGORITHM_AE
		//write app6 for wb info.
		pinfo->index = 6;
		//awb_db_buf = (UINT8 *)sp5kMalloc(32*1024);
#if SP5K_ALGORITHM_AWB
		awbSize = appAWBALG_WriteAppn((UINT32)addr, 32*1024);
#endif
#if SP5K_ALGORITHM_AE
		aeSize = appAEALG_WriteAppn((UINT32)(addr+awbSize), 32*1024-awbSize);
#endif
		//memcpy(addr, awb_db_buf, (1024*32));
		flag = 3;

		//sp5kFree(awb_db_buf);

		return (1024*32);
#else
		//write app6 for wb info.
		pinfo->index = 6;
		memcpy(addr, awb_db_buf, (1024*12));
		flag = 3;
		return (1024*12);
#endif
	}
	
	if (flag == 3) 
	{
		flag = 0;
		saveSize = 0;
		return 0;
	}

	/* In case the internal buffer is not enough */
	if (pinfo->nailSize == 0) 
	{
		flag = 0;
		return 0;
	}
	pinfo->index = 5;//APP5
	if (flag == 0) 
	{
		leftSize = pinfo->nailSize;
		flag = 1;
	}
	if (leftSize < size) 
	{
		size = leftSize;
	}
	leftSize -= size;
	if (leftSize == 0)
	{
		flag = 2;
	}
	memcpy((UINT8 *)addr, (UINT8 *)pinfo->nailAddr + saveSize, size);
	saveSize += size;
	return size;
}



void
appExifInit(
	void
)
{
	UINT8* fwVer;
	UINT8 buf[8];
	UINT32* dwbuf=(UINT32*)buf;

	DBG_PRINT("__APP_EXIF_INIT__\n");

	fwVer = appVersionGet();
	
	sp5kImgSize_t size;
	UINT32 nailType = SP5K_CAPTURE_QV_NAIL_FMT_JPG;
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_NAIL_TYPE, &nailType);
	size.width = 640;
	size.height = 480;
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_RESOLUTION, &size);
	sp5kExifCfgSet(SP5K_EXIF_CFG_MPF_SUPPORT, 1); /* support mpf big thumb */
	sp5kSystemCfgSet(SP5K_EXIF_MAKERNOTE_SIZE_CFG, EXIF_MN_BUF_SIZE);	
	#if PRECOMPOSITION	/* MPF open for 3D image */ 
	sp5kMpfCfgSet(SP5K_MAX_IDX_IFD_NUMBER, 5);
	sp5kMpfCfgSet(SP5K_MAX_ATTR_IFD_NUMBER, 5);
	sp5kMpfCfgSet(SP5K_MAX_IMG_NUMBER, 3);
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_MPO_IMGNUM, 2); /*enable MPO, and set image number to 3*/
	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_MPF_ATTR_VALUE_SET, appMpf_setIfdValue );
	#endif
	
	#if GPS_SUPPORT
	sp5kExifCfgSet(SP5K_EXIF_CFG_GPS_IFD_SUPPORT, 1); 
	#endif
	sp5kExifInit();
	sp5kExifAppnParamSet(SP5K_EXIF_APPN_WRITE_CALLBACK, (UINT32)NULL); // for clean warning
//	sp5kExifAppnParamSet(SP5K_EXIF_APPN_WRITE_CALLBACK, (UINT32)AppExifWriteAppnCb);
	// These tas we don't need for DSC
	sp5kExifEncodeTagDelete(SP5K_IFD_TYPE_ZEROIFD, SP5K_IFD0_TAG_IMGDESC);
	sp5kExifEncodeTagDelete(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_AUDIOFILE);
	// These new tags will not change after poweron.
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_ZEROIFD, APP_EXIF_TAG_FW_VER, SP5K_IFD_DATATYPE_UNDEFINED, strlen(fwVer), fwVer);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_ZEROIFD, APP_EXIF_TAG_COPYRIGHT, SP5K_IFD_DATATYPE_UNDEFINED, strlen(APP_EXIF_COPYRIGHT),APP_EXIF_COPYRIGHT);
	// These new tags will change after poweron.
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_MAKERNOTE, SP5K_IFD_DATATYPE_UNDEFINED, EXIF_MN_BUF_SIZE, mnBuf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_EXP_MODE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_FOCALLENGTH_32MMFILE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SCENE_MODE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SHARPNESS, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_WB_MODE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_APERTURE, SP5K_IFD_DATATYPE_RATIONAL, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SHUTTER_SPEED, SP5K_IFD_DATATYPE_SRATIONAL, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_DIGITAL_ROOM_RATION, SP5K_IFD_DATATYPE_SHORT, 1, buf);
#if GPS_SUPPORT
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDEREF, SP5K_IFD_DATATYPE_ASCII, 2, buf);	
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDE, SP5K_IFD_DATATYPE_RATIONAL, 3, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDEREF, SP5K_IFD_DATATYPE_ASCII, 2, buf); 
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDE, SP5K_IFD_DATATYPE_RATIONAL, 3, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_ALTITUDE, SP5K_IFD_DATATYPE_RATIONAL, 1, buf);	
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_TIMESTAMP, SP5K_IFD_DATATYPE_RATIONAL, 3, buf); 
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_STATUS, SP5K_IFD_DATATYPE_ASCII, 2, buf);		
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_SPEED, SP5K_IFD_DATATYPE_RATIONAL, 1, buf); 	
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_DATESTAMP, SP5K_IFD_DATATYPE_ASCII, 11, buf);	
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_IMGDIRREF, SP5K_IFD_DATATYPE_ASCII, 2, buf);	
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_IMGDIR, SP5K_IFD_DATATYPE_RATIONAL, 1, buf);	
#endif
	// These sp5k tags will not change.
	appExifSetMake(APP_EXIF_MAKE);
	appExifSetModel(APP_EXIF_MODEL);
	appExifSetExifVersion(APP_EXIF_VERSION);
	dwbuf[0]=0x120;
	dwbuf[1]=0x003;
	appExifSetXResolution(buf);
	appExifSetYResolution(buf);
	// These is default value after power on.
	appExifSetWBMode(buf);
	dwbuf[0] = MAX_APER_X100;
	dwbuf[1] = 100;
	appExifSetMaxAperture(buf);

	appExif_MN_Init();
	
	appExif_SetAviChunk();
}

void
appExifAscRTCGet(UINT8 *ptimeBuf)
{
	exifAscRTCGet(ptimeBuf);
}

void
appExif_SetExpProgram(
	UINT32 curMode,
	UINT32 curScene
)
{
	UINT8 buf[4];
	UINT16* uwbuf = (UINT16*)buf;

	switch(curMode)
	{
	case PASM_PROGRAM:
		uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_NORMAL;
		break;
	case PASM_APERTURE:
		uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_APERTURE;
		break;
	case PASM_SHUTTER:
		uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_SHUTTER;
		break;
	case PASM_MANUAL:
		uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_MANUAL;
		break;
	case SCENE_MODE:
		switch(curScene) 
		{
		case SCENE_PORTRAIT:
			uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_PORTRAIT;
			break;
		case SCENE_LANDSCAPE:
			uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_LANDSCAPE;
			break;
		default:
			uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_NOTDEFINED;
			break;
		}
		break;
	default:
		uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_NOTDEFINED;
		break;
	}
	appExifSetExpProgram(buf);
	DBG_PRINT("%s: %d\n",__FUNCTION__, uwbuf[0]);
}

void
appExif_SetExpBias(
	UINT32 expBiasIdx
)
{
	UINT8 buf[8];
	SINT32* lbuf = (SINT32*)buf;
	static SINT16 expBiasLut[]=
	{
		[UI_EXP_BIAS_N200]=SP5K_EXIF_TAGDATA_EXP_BIAS_N200,
		#if !CAM_TYPE_CVR
		[UI_EXP_BIAS_N166]=SP5K_EXIF_TAGDATA_EXP_BIAS_N166,
		[UI_EXP_BIAS_N133]=SP5K_EXIF_TAGDATA_EXP_BIAS_N133,
		#endif
		[UI_EXP_BIAS_N100]=SP5K_EXIF_TAGDATA_EXP_BIAS_N100,
		#if !CAM_TYPE_CVR
		[UI_EXP_BIAS_N066]=SP5K_EXIF_TAGDATA_EXP_BIAS_N066,
		[UI_EXP_BIAS_N033]=SP5K_EXIF_TAGDATA_EXP_BIAS_N033,
		#endif
		[UI_EXP_BIAS_ZERO]=SP5K_EXIF_TAGDATA_EXP_BIAS_ZERO,
		#if !CAM_TYPE_CVR
		[UI_EXP_BIAS_P033]=SP5K_EXIF_TAGDATA_EXP_BIAS_P033,
		[UI_EXP_BIAS_P066]=SP5K_EXIF_TAGDATA_EXP_BIAS_P066,
		#endif
		[UI_EXP_BIAS_P100]=SP5K_EXIF_TAGDATA_EXP_BIAS_P100,
		#if !CAM_TYPE_CVR
		[UI_EXP_BIAS_P133]=SP5K_EXIF_TAGDATA_EXP_BIAS_P133,
		[UI_EXP_BIAS_P166]=SP5K_EXIF_TAGDATA_EXP_BIAS_P166,
		#endif
		[UI_EXP_BIAS_P200]=SP5K_EXIF_TAGDATA_EXP_BIAS_P200,
	};

	lbuf[0]=expBiasLut[expBiasIdx];
	lbuf[1]=100;
	appExifSetExpBias(buf);
	DBG_PRINT("%s: %s%d/%d\n",__FUNCTION__, lbuf[0]>0? "+":"-", lbuf[0]>0? lbuf[0]:(-lbuf[0]), lbuf[1]);
}

void
appExif_SetMeterMode(
	UINT32 Metering
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	if(Metering==UI_METER_CENTER) 
	{
		wbuf[0] = SP5K_EXIF_TAGDATA_METERMODE_CENTERWEIGHTED;
	}
	else if(Metering==UI_METER_MULTI) 
	{
		wbuf[0] = SP5K_EXIF_TAGDATA_METERMODE_MULTISPOT;
	}
	else if(Metering==UI_METER_SPOT) 
	{
		wbuf[0] = SP5K_EXIF_TAGDATA_METERMODE_SPOT;
	}
	appExifSetMeterMode(buf);
	DBG_PRINT("%s: %d\n",__FUNCTION__, wbuf[0]);
}

void
appExif_SetFlash(
	UINT32 flashMode,
	UINT32 isFired

)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	switch (flashMode) 
	{
	case UI_FLASH_AUTO:
		wbuf[0] = (isFired)? 25: 24;
		break;
	case UI_FLASH_RED_EYE:
		wbuf[0] = (isFired)? 89: 0;
		break;
	case UI_FLASH_ON:
		wbuf[0] = 13;
		break;
	case UI_FLASH_SLOW_SYNC:
		wbuf[0] = (isFired)? 29: 0;
		break;
	case UI_FLASH_OFF:
		wbuf[0] = 16;
		break;
	}
	appExifSetFlash(buf);
	DBG_PRINT("%s: %d\n",__FUNCTION__, wbuf[0]);
}

void
appExif_SetLightSource(
	UINT32 effect,
	UINT32 wbMode
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	if(effect != UI_EFFECT_NORMAL) 
	{
		wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_UNKNOWN;
	}
	else
	{
		switch (wbMode) 
		{
		default:
		case UI_WB_AUTO:
		case UI_WB_CUSTOM:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_UNKNOWN;
			break;
		case UI_WB_DAYLIGHT:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT;
			break;
		case UI_WB_CLOUDY:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_CLOUDY_WEATHER;
			break;
		case UI_WB_TUNGSTEN:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_TUNGSTEN;
			break;
		case UI_WB_FLUORESCENT_H:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT_FLUORESCENT;
			break;
		case UI_WB_FLUORESCENT_L:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT_FLUORESCENT;
			break;
		case UI_WB_FLUORESCENT:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAY_WHITE_FLUORESCENT;
			break;
		}
	}
	appExifSetLightSource(buf);
	DBG_PRINT("%s: %d\n",__FUNCTION__, wbuf[0]);
}

void
appExif_SetSceneCaptureType(
	UINT32 curMode,
	UINT32 curScene
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	if(curMode != SCENE_MODE) 
	{
		wbuf[0] = SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_STANDARD;
	}
	else
	{
		switch (curScene) 
		{
		case SCENE_LANDSCAPE:
			wbuf[0] = SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_LANDSCANPE;
			break;
		case SCENE_NIGHT:
			wbuf[0] = SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_NIGHTSCENE;
			break;
		case SCENE_PORTRAIT:
			wbuf[0] = SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_PORTRAIT;
			break;
		default:
			wbuf[0] = SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_STANDARD;
			break;
		}
	}
	appExifSetSceneCaptureType(buf);
	DBG_PRINT("%s: %d\n",__FUNCTION__, wbuf[0]);
}

void
appExif_SetSharpness(
	UINT32 sharpness
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	switch (sharpness) 
	{
	case UI_SHARP_MORE:
		wbuf[0] = SP5K_EXIF_TAGDATA_SHARPNESS_HARD;
		break;
	case UI_SHARP_NORMAL:
		wbuf[0] = SP5K_EXIF_TAGDATA_SHARPNESS_SOFT;
		break;
	case UI_SHARP_LESS:
		wbuf[0] = SP5K_EXIF_TAGDATA_SHARPNESS_NORMAL;
		break;
	default:
		break;
	}
	appExifSetSharpness(buf);
	DBG_PRINT("%s: %d\n",__FUNCTION__, wbuf[0]);
}

UINT32
appExifEncodeCb(
	UINT32 imgId,
	UINT32 reserved1,
	void *reserved2
)
{
	/*s Modify by Aries 10/05/19*/
	//UINT32 agc;
	SINT32 r;
	UINT8 buf[32];
	UINT32 *dwbuf=(UINT32 *)buf;
	SINT32 *lbuf=(SINT32 *)buf;
	UINT16 *wbuf=(UINT16 *)buf;
	//sp5kEvUnit_t Ev;
	appAeSet_t* pAe;
	UINT32 awbSize=0, aeSize=0;
	uiPara_t* puiPara = appUiParaGet();
	#if GPS_SUPPORT
	UINT8 tmpBuf[32] = {0}; 
	UINT32 gpsLen = 0;
	UINT32 gpsVal = 0;
#endif

	profLogAdd(0, "Exif CB -s");
	DBG_PRINT("__APP STILL EXIF CB__\n");
	printf("@@@@@@@@@@@@@\n");
	printf("__APP STILL EXIF CB__\n");


	dwbuf[0] = shutterSpeed[gStillCB.ae.shutter][0];
	dwbuf[1] = shutterSpeed[gStillCB.ae.shutter][1];
	appExifSetExpTime(buf);
	
	printf("@@@@@@@@@@@@@\n");
	printf("exp time =%d/%d\n", dwbuf[0], dwbuf[1]);

	DBG_PRINT("exp time =%d/%d\n", dwbuf[0], dwbuf[1]);

	// TV = - log  (shutter)
	//			 2
	// 4.9 = - log	(1/30)
	//			  2
	dwbuf[2] = dwbuf[0];
	dwbuf[3] = dwbuf[1];
	r = log2xyND(dwbuf[2],dwbuf[3],&lbuf[0],&lbuf[1]);
	lbuf[0] = -lbuf[0];
	appExifSetShutSpeed(buf);
	DBG_PRINT("TV =%d/%d\n", lbuf[0], lbuf[1]);

	dwbuf[0] = aperture_x100[gStillCB.lensZoomSeg][gStillCB.ae.iris-APERTURE_2_8];
	dwbuf[1] = 100;
	appExifSetFNum(buf);
	DBG_PRINT("f num =%d/%d\n", dwbuf[0], dwbuf[1]);

	// AV = 2* log (f number)
	//			  2
	// 5.67 = 2* log (7.1)
	//			    2
	dwbuf[2] = dwbuf[0];
	dwbuf[3] = dwbuf[1];
	r = log2xyND(dwbuf[2],dwbuf[3],&dwbuf[0],&dwbuf[1]);
	dwbuf[0] = dwbuf[0]*2;
	appExifSetAperture(buf);
	DBG_PRINT("AV =%d/%d\n", dwbuf[0], dwbuf[1]);

	wbuf[0] = isoValue[gStillCB.ae.iso];
	appExifSetIsoSpeed(buf);
	DBG_PRINT("ISO =%d\n", wbuf[0]);

	pAe = appAeSetGet();
	appExif_SetExpBias(pAe->evbias);

	appExif_SetLightSource(puiPara->ImageEffect, puiPara->WBMode);

	appExif_SetFlash(puiPara->flash, gStillCB.needFlash);

	dwbuf[0] = dwbuf[0] = 2400;
	//(UINT16)appLensQuery(LENS_QUERY_ZOOM_STEP_POS); //lensZoomCurPosGet();
	dwbuf[1] = 1000;
	appExifSetFocalLength(buf);

	dwbuf[0] = appDZoomGetRation();
	dwbuf[1] = 1000;
	appExifSetDigitalZoomRatio(buf);
	
#if GPS_SUPPORT
	appGpsInfoGet(GPS_INFO_LATITUDE_IND, buf, sizeof(buf)); /*GPS Latitude*/
	appExifSetGpsLatitudeRef(buf);
	appGpsInfoGet(GPS_INFO_LATITUDE, tmpBuf, sizeof(tmpBuf));
	gpsLen = strlen(tmpBuf);
	if(gpsLen > 8)
		tmpBuf[8] = '\0';
	gpsVal = atoi(tmpBuf);
	dwbuf[0] = gpsVal/1000000;
	dwbuf[1] = 1;
	dwbuf[2] = (gpsVal%1000000)/100;
	dwbuf[3] = 1;
	dwbuf[4] = (gpsVal%10000) * 60;
	dwbuf[5] = 10000;
	appExifSetGpsLatitude(buf);
	
	appGpsInfoGet(GPS_INFO_LONGITUDE_IND, buf, sizeof(buf)); /*GPS Longitude*/
	appExifSetGpsLongitudeRef(buf);
	appGpsInfoGet(GPS_INFO_LONGITUDE, tmpBuf, sizeof(tmpBuf));

	gpsLen = strlen(tmpBuf);
	if(gpsLen > 9)
		tmpBuf[9] = '\0';
	gpsVal = atoi(tmpBuf);
	dwbuf[0] = gpsVal/1000000;
	dwbuf[1] = 1;
	dwbuf[2] = (gpsVal%1000000)/100;
	dwbuf[3] = 1;
	dwbuf[4] = (gpsVal%10000) * 60;
	dwbuf[5] = 10000;
	appExifSetGpsLongitude(buf);

	appGpsInfoGet(GPS_INFO_MSLALTITUDE, tmpBuf, sizeof(tmpBuf)); /*GPS Altitude*/
	if(tmpBuf[0] == '+')
	{
		buf[0] = 0;
	}
	else if(tmpBuf[0] == '-')
	{
		buf[0] = 1;
	}
	appExifSetGpsAltitudeRef(buf);
	dwbuf[0] = atoi(&tmpBuf[1]);
	appExifSetGpsAltitude(buf);
	
	appGpsInfoGet(GPS_INFO_TIME, tmpBuf, sizeof(tmpBuf)); /*GPS TimeStamp*/
	gpsVal = atoi(tmpBuf);
	dwbuf[0] = gpsVal/10000 + 8;
	if(dwbuf[0]>= 24)
	{
	 	dwbuf[0]-= 24;
	}
	dwbuf[1] = 1;
	dwbuf[2] = (gpsVal%10000)/100;
	dwbuf[3] = 1;
	dwbuf[4] = gpsVal/100;
	dwbuf[5] = 1;
	appExifSetGpsTimestamp(buf);
	
	appGpsInfoGet(GPS_INFO_GPSSTATUS, buf, sizeof(buf)); /*GPS Status*/
	appExifSetGpsStatus(buf);
	
	appGpsInfoGet(GPS_INFO_SPEED, tmpBuf, sizeof(tmpBuf)); /*GPS Speed*/
	dwbuf[0] = atoi(tmpBuf);
	dwbuf[1] = 1000;
	appExifSetGpsSpeed(buf);
	
	appGpsInfoGet(GPS_INFO_DATE, buf, sizeof(buf)); /*GPS DateStamp*/
	appExifSetGpsDatestamp(buf);

	appGpsInfoGet(GPS_INFO_IMGDIRECTION, tmpBuf, sizeof(tmpBuf)); /*GPS ImgDirection*/
	dwbuf[0] = atoi(tmpBuf);
	dwbuf[1] = 100;
	appExifSetGpsDirection(buf);
#endif
	// user may want to add the information for testing.
	SP_IQLogInit();

	//SP_IQAWBLog();

	aeExif_DbgLogInit();
	aeExif_DbgLogSave();

	flashExif_DbgLogInit();
	flashExif_DbgLogSave();

#if SP5K_ALGORITHM_AWB
		awbSize = appAWBALG_WriteAppn((UINT32)(&mnBuf[EXIF_MN_IQ_BUF_SIZE+EXIF_MN_PFH_BUF_SIZE+EXIF_MN_AF_BUF_SIZE+EXIF_MN_AE_BUF_SIZE]), EXIF_3A_DEBUG_INFO_SIZE);
#endif
#if SP5K_ALGORITHM_AE
		aeSize = appAEALG_WriteAppn((UINT32)((&mnBuf[EXIF_MN_IQ_BUF_SIZE+EXIF_MN_PFH_BUF_SIZE+EXIF_MN_AF_BUF_SIZE+EXIF_MN_AE_BUF_SIZE])+awbSize), EXIF_3A_DEBUG_INFO_SIZE-awbSize);
#endif	

	appExifSetMakerNote(EXIF_MN_BUF_SIZE, mnBuf);

	profLogAdd(0, "Exif CB -e");
	return SUCCESS;
	/*e Modify by Aries 10/05/19*/
}

void appExif_MN_Init(void)
{
	UINT8 *pBuf;

	pBuf = (UINT8 *)mnBuf;

	/*-- Initialize EXIF Maker Note buffer control block
	*/
	mnBufCB[EXIF_MN_IQ].bufBase = pBuf;
	mnBufCB[EXIF_MN_IQ].bufWPtr = mnBufCB[EXIF_MN_IQ].bufBase;
	mnBufCB[EXIF_MN_IQ].bufSize = EXIF_MN_IQ_BUF_SIZE;
	pBuf += EXIF_MN_IQ_BUF_SIZE;

	mnBufCB[EXIF_MN_PFH].bufBase = pBuf;
	mnBufCB[EXIF_MN_PFH].bufWPtr = mnBufCB[EXIF_MN_PFH].bufBase;
	mnBufCB[EXIF_MN_PFH].bufSize = EXIF_MN_PFH_BUF_SIZE;
	pBuf += EXIF_MN_PFH_BUF_SIZE;

	mnBufCB[EXIF_MN_AF].bufBase = pBuf;
	mnBufCB[EXIF_MN_AF].bufWPtr = mnBufCB[EXIF_MN_AF].bufBase;
	mnBufCB[EXIF_MN_AF].bufSize = EXIF_MN_AF_BUF_SIZE;
	pBuf += EXIF_MN_AF_BUF_SIZE;

	mnBufCB[EXIF_MN_AE].bufBase = pBuf;
	mnBufCB[EXIF_MN_AE].bufWPtr = mnBufCB[EXIF_MN_AE].bufBase;
	mnBufCB[EXIF_MN_AE].bufSize = EXIF_MN_AE_BUF_SIZE;
	pBuf += EXIF_MN_AE_BUF_SIZE;

	#if HOST_DBG
	{
		int i;
		printf("\n");
		printf("----------------------------------\n");
		for (i=0; i<EXIF_MN_TOTAL; i++)
		{
			printf("blk[%d]: base(0x%x), wptr(0x%x), size(%d) \n",
					i, mnBufCB[i].bufBase,
					mnBufCB[i].bufWPtr,
					mnBufCB[i].bufSize);
		}
		printf("----------------------------------\n");
		printf("\n");
	}
	#endif
}

void appExif_MN_BufClear(UINT8 bufID)
{
	UINT8 *bufBase;
	UINT32 bufSize;

	DBG_PRINT("%s ...", __FUNCTION__);

	/*-- Check if bufID is valid */
	if (bufID >= EXIF_MN_TOTAL) 
	{
		DBG_PRINT("MN_Print: Invalid ID(%d) !!! \n", bufID);
		return;
	}

	/*-- Initialize EXIF Maker Note buffer control block
	*/
	bufBase = mnBufCB[bufID].bufBase;
	mnBufCB[bufID].bufWPtr = bufBase;
	bufSize = mnBufCB[bufID].bufSize;
	memset(bufBase, 0, bufSize);
}

UINT32  appExif_MN_Printf(UINT8 bufID, const char *szFmt, ...)
{
    va_list vaPtr;
    unsigned char szBuffer[128];
	UINT32 buflen,ret;

	UINT8 *bufWPtr, *bufBase;
	UINT32 bufSize;

	DBG_PRINT("%s ...", __FUNCTION__);

	/*-- Check if bufID is valid */
	if (bufID >= EXIF_MN_TOTAL)
	{
		DBG_PRINT("MN_Print: Invalid ID(%d) !!! \n", bufID);
		return FAIL;
	}
	memset(szBuffer, 0, 128);

    va_start(vaPtr, (const char *)szFmt);
    vsprintf((char *)szBuffer, (const char *)szFmt, vaPtr);
	va_end(vaPtr);
	buflen=strlen(szBuffer);
    
	/*-- strings size checkup */
	if (buflen>(128-1)) 
	{
		DBG_PRINT("MN_Print: Strings size overflow(%d) !!! \n", buflen);
		return FAIL;
	}

	bufWPtr = mnBufCB[bufID].bufWPtr;
	bufBase = mnBufCB[bufID].bufBase;
	bufSize = mnBufCB[bufID].bufSize;

	if((bufWPtr - bufBase + buflen) > bufSize ) 
	{
		DBG_PRINT("MN_Print: Buf overflow(%d) !!! \n", bufSize);
		return FAIL;
	}
	memcpy(bufWPtr, szBuffer, buflen);
	bufWPtr += buflen;
	mnBufCB[bufID].bufWPtr = bufWPtr;

	
	ret=appExifSetMakerNote(buflen, mnBufCB[bufID].bufWPtr);  
    
	return SUCCESS;
}

UINT32 appExif_MN_WriteBytes(UINT8 bufID, UINT8 *pdata, UINT32 size)
{
	UINT8 *bufWPtr, *bufBase;
	UINT32 bufSize;

	DBG_PRINT("%s ...", __FUNCTION__);

	/*-- Check if bufID is valid */
	if (bufID >= EXIF_MN_TOTAL) 
	{
		DBG_PRINT("MN_Print: Invalid ID(%d) !!! \n", bufID);
		return FAIL;
	}

	bufWPtr = mnBufCB[bufID].bufWPtr;
	bufBase = mnBufCB[bufID].bufBase;
	bufSize = mnBufCB[bufID].bufSize;

	if((bufWPtr - bufBase + size) > bufSize ) 
	{
		DBG_PRINT("MN_Print: Buf overflow(%d) !!! \n", bufSize);
		DBG_PRINT("base(%x), wptr(%x), size(%d) \n", bufBase, bufWPtr, size);
		return FAIL;
	}

	memcpy(bufWPtr, pdata, size);

	bufWPtr += size;
	mnBufCB[bufID].bufWPtr = bufWPtr;

	return SUCCESS;
}

UINT32 appExif_MN_Write8(UINT8 bufID, UINT8 data)
{
	return appExif_MN_WriteBytes(bufID, &data, 1);
}

UINT32 appExif_MN_Write16(UINT8 bufID, UINT16 data)
{
	return appExif_MN_WriteBytes(bufID, (UINT8 *)&data, 2);
}

UINT32 appExif_MN_Write32(UINT8 bufID, UINT32 data)
{
	return appExif_MN_WriteBytes(bufID, (UINT8 *)&data, 4);
}

UINT32 appExif_MN_WriteUID(UINT8 bufID, UINT32 uid)
{
	UINT32 bigendian;

	bigendian =   ((uid & 0xFF000000)>>24)
				+ ((uid & 0x00FF0000)>>8)
				+ ((uid & 0x0000FF00)<<8)
				+ ((uid &0x000000FF)<<24);
	return appExif_MN_WriteBytes(bufID, (UINT8 *)&bigendian, 4);
}


void appExif_SetAviVariableDataOffset(UINT32 offset)
{
	u32VariableDataOffset = offset;
	uVariableDataGlobalOffset += offset;
}

void appExif_SetAviRiffASCII (PSSTRDTAG pointer, unsigned short offset, unsigned short tagcode, UINT32 count,  char* pcStr)
{
	pointer[offset].TagCode = tagcode;
	pointer[offset].Type 	= STRD_TAG_TYPE_ASCII;
	pointer[offset].Count	= count;
	pointer[offset].Value	= uVariableDataGlobalOffset;

	if ( *pcStr == 0 ) return;	
	
	memcpy( ((unsigned char*)pointer) + u32VariableDataOffset, pcStr, count);

	u32VariableDataOffset += count;
	uVariableDataGlobalOffset  += count;
		
}

void appExif_SetAviRiffShort ( PSSTRDTAG pointer, unsigned short offset, unsigned short tagcode, UINT32 count, UINT32 value)
{
	pointer[offset].TagCode = tagcode;
	pointer[offset].Type 	= STRD_TAG_TYPE_SHORT;
	pointer[offset].Count 	= count;
	pointer[offset].Value 	= value;

}


void appExif_AddAviPaddingData ( unsigned char* pbPtr, unsigned char* pbData, UINT32 uSize)
{
	memcpy( pbPtr+u32VariableDataOffset, pbData, uSize );
	u32VariableDataOffset += uSize;
	uVariableDataGlobalOffset += uSize;
}



void appExif_SetAviChunk(void)
{


	unsigned char* pbAviExifPtr;
	/*PSTRDCHUNK strdtag=&pAviStrdChuck;*/ // for clean warning
	view_param_t* puiView;

	uVariableDataGlobalOffset = 2;	/*dw++@20120529. Bugfix #44981.*/

	puiView = appViewParamGet();

	pbAviExifPtr = pAviStrdChuck;

	/*ImageSize*/
	/*RiffStrdStart[RIFF_PROFILE_IMAGESIZE_INDEX] = RIFF_PROFILE_IMAGESIZE_1920;*/
	RiffStrdStart[RIFF_PROFILE_IMAGESIZE_INDEX] = (puiView->vidsize==UI_VID_SIZE_FHD)?RIFF_PROFILE_IMAGESIZE_1920:RIFF_PROFILE_IMAGESIZE_1280;

	/*sound INFO*/
	/*RiffStrdStart[RIFF_PROFILE_FRAMERATE_INDEX] = RIFF_PROFILE_FRAMERATE_30FPS;*/
	RiffStrdStart[RIFF_PROFILE_FRAMERATE_INDEX] =  (puiView->vidsize==UI_VID_SIZE_HD_60FPS)?RIFF_PROFILE_FRAMERATE_60FPS:RIFF_PROFILE_FRAMERATE_30FPS;
	/*sound INFO*/
	RiffStrdStart[RIFF_PROFILE_SOUNDINFO_INDEX] = RIFF_PROFILE_SOUNDINFO;

	memcpy( pbAviExifPtr, RiffStrdStart, sizeof(RiffStrdStart) );
	pbAviExifPtr += sizeof(RiffStrdStart);

	/* 0TH-IFD*/
	appExif_SetAviVariableDataOffset(sizeof(SSTRDTAG)*POTHIFD_MAX);
	appExif_SetAviRiffASCII(P0thIfd, 0, 0x0100, sizeof(MAKERNOTE_MAKER), MAKERNOTE_MAKER);
	appExif_AddAviPaddingData( (unsigned char*)P0thIfd, "\x0", 1);

	appExif_SetAviRiffASCII(P0thIfd, 1, 0x0101, sizeof(MAKE_MODEL), MAKE_MODEL);
	appExif_AddAviPaddingData( (unsigned char*)P0thIfd, "\x0", 1);

	if(puiView->videoStamp){
		appExif_SetAviRiffShort(P0thIfd, 2, 0x0102, 1, 1); /*0 :Datestamp Off;1 Datestamp On*/
	}else{
		appExif_SetAviRiffShort(P0thIfd, 2, 0x0102, 1, 0); /*0 :Datestamp Off;1 Datestamp On*/
	}
	memcpy( pbAviExifPtr, P0thIfd,u32VariableDataOffset);
	uVariableDataGlobalOffset += sizeof (RiffStrdStart);

	sp5kMediaRecAviChunkSet(MEDIA_AVI_CHUNK_STRD, pAviStrdChuck, uVariableDataGlobalOffset);


}




void appExif_GetAviChunkInfo(UINT32 *poffset,UINT32 *size)
{

	UINT32 strdsize;
	UINT32 strdOffset;

	sp5kMediaPlayAviChunkInfoGet(MEDIA_AVI_CHUNK_STRD,&strdOffset,&strdsize);
	*poffset =strdOffset;
	*size = strdsize;
	#if 0
	printf("strdChunksize:%d\n",strdsize);
	printf("strdChunkOffset:%d\n",strdOffset);
	#endif

}


void appExif_GetAviChunk(void)
{

	UINT32 strdChunksize;
	UINT32 strdChunkOffset;
	UINT8 *pbuf;

	appExif_GetAviChunkInfo(&strdChunkOffset,&strdChunksize);

	pbuf = (UINT8 *)sp5kMalloc(strdChunksize);
	memset(pbuf, 0, strdChunksize);
	sp5kMediaPlayAviChunkBufSet(MEDIA_AVI_CHUNK_STRD, pbuf, strdChunksize);

	#if 0
	printf("strdChunksize:%d\n",strdChunksize);
	printf("strdChunkOffset:0x%x\n",strdChunkOffset);
	memdump(pbuf,strdChunksize);
	#endif

	sp5kFree(pbuf);

}

#if PRECOMPOSITION 
void appMpf_setIfdValue(
	UINT32 snapCnt,
	UINT32 rsvd2,
	void *rsvd3
)
{
	/*MPO host sample code*/
	UINT32 defaultVal;
	UINT32 imgNumber= 2;
	UINT32 angleVal;
	UINT32 blLenVal;
	switch(snapCnt) {
	case 1:
		angleVal = 0x00000000;
		blLenVal = 0x00000041;
		/* index IFD for first image */
		/* MP Index IFD - MP Format Version */
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_VERSION, SP5K_MPF_DATATYPE_UNDEFINED, 4,"0100");
		/* MP Index IFD - Number of Images */
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_NUMOFIMG, SP5K_MPF_DATATYPE_LONG, 1, &imgNumber);

		/* MP Index IFD - MP Entry */
		UINT8 *mpEntryVal = (UINT8 *)osMemAlloc(imgNumber * 16);
		UINT8 *uniIdVal = (UINT8 *)osMemAlloc(imgNumber * 33);
		memset(mpEntryVal,0x00,imgNumber * 16);
		memset(uniIdVal,0x00,imgNumber * 33);

		UINT32 hostMpType;
		hostMpType = (0x20 << 24) | SP5K_MPF_MP_ENTRY_TYPE_MVDISPARITY;
		memcpy(mpEntryVal,&hostMpType,4);
		hostMpType = (0x0 << 24) | SP5K_MPF_MP_ENTRY_TYPE_MVDISPARITY;
		memcpy(mpEntryVal+16,&hostMpType,4);
		memcpy(mpEntryVal+32,&hostMpType,4);

		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_MPENTRY, SP5K_MPF_DATATYPE_UNDEFINED, imgNumber * 16, mpEntryVal);

		/* MP Index IFD - Individual Image Unique ID List	(additional) */
		memset(uniIdVal,0x31,33);		/*	ASCII: 0x31 => 1 */
		memset(uniIdVal+33,0x32,33);	/*	ASCII: 0x32 => 2 */
		memset(uniIdVal+66,0x33,33);	/*	ASCII: 0x33 => 3 */
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_IMGUNID, SP5K_MPF_DATATYPE_UNDEFINED, imgNumber * 33, uniIdVal);

		/* MP Attribute IFD */
		defaultVal = 1;
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_IDXOFIMG, SP5K_MPF_DATATYPE_LONG, 1, &snapCnt);
		/* Viewpoint Number:	represents the position of camera during shooting, this tag shall be specified for Disparity and Multi-Angle Images. */
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASEVIEWPOINTNUM, SP5K_MPF_DATATYPE_LONG, 1, &defaultVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_CONVERGENCEANGLE, SP5K_MPF_DATATYPE_SRATIONAL, 1, &angleVal);
		/* the distance between two viewpoint */
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASELINELENGTH, SP5K_MPF_DATATYPE_RATIONAL, 1, &blLenVal);

		break;
	case 2:
		defaultVal = 1;
		angleVal = 0x00000002;
		angleVal = 0x00000041;
		/* MP Attribute IFD */
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_VERSION, SP5K_MPF_DATATYPE_UNDEFINED, 4,"0100");
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_IDXOFIMG, SP5K_MPF_DATATYPE_LONG, 1, &snapCnt);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASEVIEWPOINTNUM, SP5K_MPF_DATATYPE_LONG, 1, &defaultVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_CONVERGENCEANGLE, SP5K_MPF_DATATYPE_SRATIONAL, 1, &angleVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASELINELENGTH, SP5K_MPF_DATATYPE_RATIONAL, 1, &blLenVal);
	#if PRECOMPOSITION
		appViewDrawPreComposition(FALSE, 0);
	#endif
		break;
	case 3:
		defaultVal = 1;
		angleVal = 0x00000004;
		angleVal = 0x00000082;
		/* MP Attribute IFD */
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_VERSION, SP5K_MPF_DATATYPE_UNDEFINED, 4,"0100");
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_IDXOFIMG, SP5K_MPF_DATATYPE_LONG, 1, &snapCnt);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASEVIEWPOINTNUM, SP5K_MPF_DATATYPE_LONG, 1, &defaultVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_CONVERGENCEANGLE, SP5K_MPF_DATATYPE_SRATIONAL, 1, &angleVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASELINELENGTH, SP5K_MPF_DATATYPE_RATIONAL, 1, &blLenVal);
		break;
	}
}
#endif
