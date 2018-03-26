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

#include "sp5k_capture_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_cal_api.h"
#include "sp5k_moe_api.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_still.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_face_detect_ex.h"
#include "app_zoom_api.h"
#include "app_calib_api.h"
#include "app_view_osd.h"
#include "app_still_hdr.h"
#include "sp5k_media_api.h"
#include "app_stringId.h"
#include "app_ui_para.h" 
#include "app_menu_tab.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define _FACE_TRACK_DO_FRAME_INTERVAL_ 2
#define RAW_BATCH_FILE		"D:\\RAW\\RAWBATCH.TXT"
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define FORCE_CHANGE_RAW 0
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static BOOL fRawBatchInited;
static UINT8 *RawBatch;
static UINT32 RawBatchPos,RawBatchSize;
UINT32 BlinkYUVCallBackSet =0; 
UINT32 BlinkGet=0; /*Have Blink Eyes*/
extern UINT32 BlinkSaveSelect; 
extern UINT32 CapWithFaceCheck;

UINT32 gSensorMode=0x30;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void SP_CapturePostWb(UINT32 addr, UINT32 RawFmt, void* ImgSize);
extern UINT8 IsHDR,IsHDRNightScene;	
extern void appAWBALG_CapPostWBSet( UINT32 addr, UINT32 RawFmt, void * rawInfo ); // for clean warning
extern void sp5kYuvBufferFree(void *ptr); // for clean warning
extern UINT32 sp5kSensorModeCfgSet(UINT32 mode,UINT32 sensormode); // for clean warning
extern void  appOsdMessageBoxShow(BOOL bShow, unsigned short usStrId); // for clean warning
extern void *sp5kYuvBufferAlloc(UINT32 bufW, UINT32 bufH); // for clean warning
extern void yuvScale(UINT8 *srcbuf, UINT32 yuvw, UINT32 yuvh,UINT8 *dstbuf, UINT32 qvw, UINT32 qvh); // for clean warning
extern void sample_MoeFMTChg(sp5kGfxObj_t *gfxObjSrc, sp5kGfxObj_t *gfxObjDst, UINT8 srcFMT, UINT8 dstFMT); // for clean warning
extern void sample_MoeImgFilter(UINT8 *psrc, UINT8 *pdst, UINT32 width, UINT32 height); // for clean warning
extern UINT32 sp5kModeswSensorModeGet(UINT32 mode); // for clean warning
extern  void app_SpeedRemainNumUpdate(void);
extern void appStillBackgroundDiskFreeUpdate(UINT32 dskFree);
extern UINT32 appStillCheckReady(UINT32 question);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appStillWbCb(UINT32 addr,UINT32 rawfmt,void* size);
static void sample_MoeBlendY(
  UINT8 *pSrc, /* source Y */
  UINT8 *pDst, /* destination Y */
  UINT8 *pAlpha, /* alpha mask */
  UINT32 fAlphaSrc, /* alpha is for source if fAlphaSrc!=0, otherwise alpha is for destination */
  UINT32 w, UINT32 h, /* dimension of Y & alpha */
  UINT8 *pwork, UINT32 worksize /* temp buffer pointer & size */
);



/*-------------------------------------------------------------------------
 *  Function Name : _rawBatchTokenGet
 *  Description :   
 *------------------------------------------------------------------------*/
UINT32
_rawBatchTokenGet(
	UINT8 *buf, 
	UINT32 bufsize
)
{
	UINT32 i,l=0;
	for (i=RawBatchPos ; i<RawBatchSize ; i++) {
		switch (RawBatch[i]) {
		case 10: case 13: case 9: case ' ':
			if (l) goto Exit_RawBatchTokenGet;
			continue;
			break;
		default:
			buf[l++] = RawBatch[i];
			if (l>=bufsize) goto Exit_RawBatchTokenGet;
			break;
		}
	}
Exit_RawBatchTokenGet:
	if (l+1<=bufsize)
		buf[l] = 0;
	RawBatchPos = i;
	return RawBatchPos<RawBatchSize && l+1<=bufsize ? SUCCESS : FAIL;
}

/*-------------------------------------------------------------------------
 *  Function Name : _rawBatchEntryGet
 *  Description :   
 *------------------------------------------------------------------------*/
UINT32 
_rawBatchEntryGet(
	UINT8 *fname, 
	sp5kWbGain_t *pwb
)
{
	UINT8 buf[8];
	if (_rawBatchTokenGet(fname, 32)!=SUCCESS) return FAIL;
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->rgain = atoi(buf);
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->grgain = atoi(buf);
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->bgain = atoi(buf);
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->gbgain = atoi(buf);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appChangeRawCb
 *  Description :   
 *------------------------------------------------------------------------*/
void 
appChangeRawCb(
	UINT32 Addr,
	UINT32 RawFmt,
	sp5kImgSize_t *  pImgSize
)
{
	UINT16 *imgAddr=(UINT16*)Addr;
	UINT32 bufw,bufh,imgw=0,imgh=0;
	UINT32 fd,fl;
	char fname[32];	
	sp5kWbGain_t wb;

	if (!fRawBatchInited) {
		fRawBatchInited = 1;
		ReadFileEx(RAW_BATCH_FILE, &RawBatch, &RawBatchSize);
		RawBatchPos = 0;
	}

	if (RawBatchSize) {
		if (_rawBatchEntryGet(fname, &wb)==FAIL) {
			printf("RawBatch end\n");
			sp5kFree(RawBatch);
			RawBatchSize = 0;
			return;
		}		
	}	

 	printf("RawChange:%s ...", fname);
	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_RDONLY); 
	printf("%s\n", fd ? "OK" : "NG");
	if (fd==0) return;
	fl = sp5kFsFileSizeGet(fd);
	bufw = pImgSize->width;
	bufh = pImgSize->height;
	if (bufw==3264 && bufh==2448 && fl==3240*2430*2) { // Sony 3H raw format 
		imgw = 3240; imgh = 2430;
	} else if (bufw*bufh*2 <= fl) {
		imgw = bufw; imgh = bufh;
	} else {
		printf("[ERR] raw change error!!\n");
	}
	
	if (imgw>0 && imgh>0) {
		if (bufw==imgw && bufh==imgh) {
			sp5kFsFileRead(fd, (UINT8*)imgAddr, imgw*imgh*2);
		} else {
			UINT16 *src,*dst;
			UINT32 y;
			src = imgAddr + (bufw*bufh - imgw*imgh)*2;
			dst = imgAddr;
			sp5kFsFileRead(fd, (UINT8*)src, imgw*imgh*2);
			for (y=0 ; y<imgh ; y++) {
				memcpy(dst, src, imgw*2);
				src += imgw;
				dst += bufw;
			}
		}
	}
	sp5kFsFileClose(fd);

	if (RawBatchSize) {		
		printf("RawBatch WB:%d %d/%d %d\n", wb.rgain, wb.grgain, wb.gbgain, wb.bgain);
		if(wb.rgain ==0 && wb.grgain ==0 && wb.gbgain == 0 && wb.bgain ==0) {
			appStillWbCb(Addr, RawFmt, pImgSize);
		}
		else {
			sp5kImgWbGainSet(&wb);
		}
	} 
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillRawCB
 *  Description :   
 *------------------------------------------------------------------------*/
UINT32
appStillRawCB(
	UINT32 addr,
	UINT32 rawfmt,
	void* size
)
{
	//sp5kImgSize_t*	p = (sp5kImgSize_t*)size;
	
	DBG_PRINT("__%s\n", __FUNCTION__);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillYuvCB
 *  Description :   
 *------------------------------------------------------------------------*/
UINT32
appStillYuvCB(
	UINT32 addr,
	UINT32 qvAddr,
	void* size
)
{
	sp5kImgSize_t*	psSizeInfo = (sp5kImgSize_t*)size;
	uiPara_t* puiPara = appUiParaGet();
	
	if(gStillCB.isReady2View && puiPara->PhotoFrame != UI_PHOTOFRAME_OFF) 
	{		
		appViewDrawPhotoFrame(puiPara->PhotoFrame);		
	}		
	
#if !CAM_TYPE_CVR	
	if (((IS_SCENE_MODE)&&(IS_BLINK_DETECTION_MODE)) || puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
	{
		if(CapWithFaceCheck == TRUE)
		{
			CapWithFaceCheck =FALSE;
		}
		else
		{
			return SUCCESS;
		}
		/*appStill_CfgCaptureConfirmStore(TRUE);*/  /*junqiong.hu@20121126*/
		
		BlinkGet =0;
		BlinkSaveSelect =0;
		BlinkYUVCallBackSet =TRUE;
		
		appFdBlinkingDetectCheck(addr,0,psSizeInfo);
		appFdBlinkDetectSemGet();
		if (BlinkGet== TRUE)
		{
			/*UINT32 next_state,next_param;*/	// for clean warning
			appStill_SetQV(43200000);/* 43200000*10ms = 5 days */
			appStateChange(APP_STATE_STILL_BLINK_DETECT,STATE_PARAM_NORMAL_INIT);
		}
		else
		{
			/*appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);*/
		}
		BlinkYUVCallBackSet =FALSE;
		return SUCCESS; 
	}
#endif	
	sp5kHostMsgSend(APP_UI_MSG_VIEW_YUV_CALLBACK, 0);
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  Function Name : appStillAFBktCb
 *  Description :   
 *------------------------------------------------------------------------*/
UINT32
appStillAFBktCb(
	UINT32 addr,
	UINT32 qvAddr,
	void* size
)
{
	
	if(IsBracketMode())
	{
	
		UINT8 CurrentStep, NextStep;
		CurrentStep = appLensQuery(13); 
		NextStep = CurrentStep + appBracketIntervalGet();
		
		printf("AF Bracket: %d-->%d\n", CurrentStep, NextStep);
		appFocusPosGo(NextStep);
		appFocusStopWait();
		
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2000);
	}
	
	sp5kHostMsgSend(APP_UI_MSG_VIEW_YUV_CALLBACK, 0);
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  Function Name : appStillBurstCB
 *  Description :   
 *------------------------------------------------------------------------*/
UINT32
appStillBurstCB(
	UINT32 res1,
	UINT32 res2,
	void* res3
)
{
	DBG_PRINT("__%s\n", __FUNCTION__);
	return SUCCESS;
}

/*
UINT32
appStillExifCB(
	UINT32 imageID,
	UINT32 res1,
	void* res2
)
{
}
*/

/*-------------------------------------------------------------------------
 *  Function Name : appStillWbCb
 *  Description :   This will do post white balance. It also be called in 
 * 					Costomer WB function.
 *------------------------------------------------------------------------*/
extern sp5kWbGain_t cwb;
UINT32
appStillWbCb(
	UINT32 addr,
	UINT32 rawfmt,
	void* size
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	RPrintf("WB callBack");
	if (puiPara->WBMode != UI_WB_CUSTOM)
	{
#if !SP5K_ALGORITHM_AWB
		DBG_PRINT("__%s\n", __FUNCTION__);
		sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
		SP_CapturePostWb(addr, rawfmt, size);
		sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP); 
#else
		appAWBALG_CapPostWBSet(addr, rawfmt, size);
#endif
	}
	else
	{
		sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
		sp5kImgWbGainSet(&cwb);
		sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP);
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewCWB
 *  Description :   This will do view white balance. It also be called in 
 * 					Costomer WB function.
 *------------------------------------------------------------------------*/
UINT32
appViewWbCb(
	sp5kWbGain_t *wb
)
{
	UINT32 one=0x400;
	UINT16 r,g,b;
	sp5kCaliPreviewRawCallbackSet(appCalibRawCb);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_PREVIEW_RAW);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	1000);
	sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_PREVIEW_RAW);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	1000);
	appGetRawAvgRGB(&r,&g,&b);
	//sp5kSystemCfgSet(SP5K_CDSP_INFO_CFG_GROUP, 0, one);
	wb->rgain = g*one/r; 
	wb->bgain = g*one/b; 
	wb->gbgain = wb->grgain = one; 
	printf("View CWB %d %d/%d %d\n", wb->rgain, wb->grgain, wb->gbgain, wb->bgain);
	#if SPCA5310 
   	sp5kImgModeSet(SP5K_MODE_STILL_PREVIEW);
	#endif
	sp5kImgWbGainSet(wb);
	#if SPCA5310 
	sp5kImgModeSetDone(SP5K_MODE_STILL_PREVIEW);
	#endif
	//sp5kSystemCfgSet(SP5K_CDSP_INFO_CFG_GROUP, 0, 0x40);	
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillBpcCb
 *  Description :   Bad pixel compensation
 *------------------------------------------------------------------------*/
UINT32
appStillBpcCb(
	UINT32 addr,
	UINT32 rawfmt,
	void* size
)
{
	DBG_PRINT("__%s\n", __FUNCTION__);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ViewMenuSelFunctionCB
 *  Description :   This callback will response and record the user setting
 *					in menu. It will take charge in still/video menu. Some 
 *					items need immeditly response to the user selection.
 *------------------------------------------------------------------------*/
void 
appStillRawCbSet(
	BOOL bLongShutter
)
{
	/*WB CallBack set*/
	if(IsHDR || IsHDRNightScene)
	{/*When HDR is ON , WB CallBack must be off ,else the program will dead*/
		sp5kStillCapWbCallbackSet(NULL);
	}
	else
	{
		sp5kStillCapWbCallbackSet((fp_sp5kCallback_t)appStillWbCb);
	}
	
	if(bLongShutter) 
	{		
		sp5kStillCapRawCallbackSet((fp_sp5kCallback_t)appStillBpcCb);
		//sp5kImgCdspBlockEnable(SP5K_CDSP_BP, 0);
	} 
	else 
	{
		// normal shutter: field raw cb for post WB, use SP BPC 		
		sp5kStillCapRawCallbackSet(NULL);
		//sp5kImgCdspBlockEnable(SP5K_CDSP_BP, 1);
	}

	if(appCalibRawBatchEnableGet() || FORCE_CHANGE_RAW==1)
	{
		UINT32 compressionRatio = 2;
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_BRC, 0);
		sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_COMPRESSION_RATIO, &compressionRatio);
		if(!bLongShutter)
		{
			sp5kStillCapWbCallbackSet(NULL);
			sp5kStillCapRawCallbackSet(NULL);
		}
		sp5kStillCapRawCallbackSet((fp_sp5kCallback_t)appChangeRawCb); 
	}
	
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillYuvCbSet
 *  Description :   To enable/disable yuv callback. If we want to use fieldQV
 *					we should disable this. If we have highlight function
 * 					it will also be implement in yub callback.
 *------------------------------------------------------------------------*/
void 
appStillYuvCbSet(
	BOOL isEnable
)
{
	
	if(isEnable) 
	{
		sp5kStillCapYUVCallbackSet(appStillYuvCB);
	}
	else 
	{
        #if PRECOMPOSITION 
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_YUV, AppCaptureYUVManipulation);
        #else
		sp5kStillCapYUVCallbackSet(NULL);
		#endif
	}	
}


/*-------------------------------------------------------------------------
 *  Function Name : appStillAFBktCbSet
 *  Description :  
 *------------------------------------------------------------------------*/
void 
appStillAFBktCbSet(
	BOOL isEnable
)
{
	if(isEnable) 
	{
		sp5kStillCapYUVCallbackSet(appStillAFBktCb);
	}
	else
	{
		sp5kStillCapYUVCallbackSet(NULL);
	}	
}


/*-------------------------------------------------------------------------
 *  Function Name : appStillPreviewCbSet
 *  Description :   To register preview callback. This is designed for face
 *					tracing.
 *------------------------------------------------------------------------*/

void 
appStillPreviewCbSet(
	BOOL isEnable
)
{
#if !CAM_TYPE_CVR || ICAT_FCWS
	static BOOL isEnable0=0;
	uiPara_t* puiPara = appUiParaGet();

	if (isEnable)
	{
		if (((IS_SCENE_MODE)&&(IS_SMILE_DETECTION_MODE))||puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT) 
		{ 
			appFdCreateEx(SMILE_DETECT);
		}
		else if (((IS_SCENE_MODE)&&(IS_BLINK_DETECTION_MODE))||puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
		{
			appFdCreateEx(BLINK_DETECT);
		}
		else
		{
			appFdCreateEx(FACE_DETECT_ONLY);
		}
	}
	else
	{
		appFdDestroyEx();
	}
	
	isEnable0 = isEnable;
#else
	isEnable = isEnable;
#endif
}

void appStillHDRCb(UINT32 addr, UINT32 rawFmt, void* imgSize)
{
	uiPara_t* puiPara = appUiParaGet();
	
	YPrintf("HDR - WB");
	if (puiPara->WBMode != UI_WB_CUSTOM)
	{
		#if !SP5K_ALGORITHM_AWB
			sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
		SP_CapturePostWb(addr, rawFmt, imgSize);
			sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP); 
		#else
			appAWBALG_CapPostWBSet(addr, rawFmt, imgSize);
		#endif
	}else{
		sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
		sp5kImgWbGainSet(&cwb);
		sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP);
	}

	appAeCaptureSet(2);
}

void appStillNightSceneCb(UINT32 addr, UINT32 rawFmt, void* imgSize)
{

	static UINT8 HDRCapNum = 0 ;
	uiPara_t* puiPara = appUiParaGet();
	
	HDRCapNum ++;

	/*For post WB*/
	YPrintf("NightScene - WB");

	if (HDRCapNum == 1){
		if (puiPara->WBMode != UI_WB_CUSTOM)
	{
			#if !SP5K_ALGORITHM_AWB
				sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
		SP_CapturePostWb(addr, rawFmt, imgSize);
				sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP); 
			#else
				appAWBALG_CapPostWBSet(addr, rawFmt, imgSize);
			#endif
		}else{
			sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
			sp5kImgWbGainSet(&cwb);
			sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP);
		}
	}
	
	if(HDRCapNum == 7)
	{
		HDRCapNum = 0;
	}

}


//////////////////////////////////////////////////////////////////////////
////
//// 	Speedy Capture
////
///////////////////////////////////////////////////////////////////////////
typedef struct yuvFrameInfo_s 
{
	UINT8 *pbuf;
	sp5kPrevYuvRoi_t roi;
} yuvFrameInfo_t;

static TX_THREAD * thdSpeedyBurst;

static yuvFrameInfo_t yuvFrameInfo[60];
static UINT8 *pyuvFrameBuf;
/*static UINT8  *pExif;*/ // for clean warning
static UINT32 yuvFrameBufSize;
static UINT32 jpegWidth, jpegHeight;
static UINT32 yuvCapIdx, yuvCapDone, yuvSaveDone;
static UINT32 yuvCapMaxNum = 30;
#if defined(CUSTOM_EXIF_TIME_SET)// for clean warning
static tmx_t my_tmx;
#endif

#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )

UINT8*
appMalloc(UINT8* pyuvframebuf,UINT32 yuvframebufsize);

static UINT32
_pvYuvCapCallback(
	UINT32 idx,
	UINT32 pbuf,
	void *p
)
{
	UINT32 ret;
	printf("yuvcb %d\n", idx);
	
	yuvCapIdx = idx;
	yuvFrameInfo[idx].pbuf = (UINT8 *)pbuf;
	memcpy(&(yuvFrameInfo[idx].roi), p, sizeof(sp5kPrevYuvRoi_t));
	if (idx == (yuvCapMaxNum-1))
	{
		yuvCapDone = 1;
	}
	ret = SUCCESS;
	return ret;
}

static void
_SpeedyBurstProc(
	ULONG lParam
)
{
	UINT32 i=0;
	#if defined(CUSTOM_EXIF_TIME_SET)
	UINT8 time[32];
	#endif
	 tmx_t my_tmx;
	uiPara_t* puiPara = appUiParaGet();
    sp5kImgSize_t capSize,capSizeBk;
	
	profLogAdd(0, "Save YUV to Jpeg S");

	sp5kRtcDateTimeGet(0, &my_tmx);
	if(puiPara->StampMode != UI_STAMP_OFF) {
		sp5kPreviewYuvCapDateTimeSet(&my_tmx);
	}

	#if defined(CUSTOM_EXIF_TIME_SET)
	sprintf((char *)time, "%04u:%02u:%02u:%02u:%02u:%02u",
		(unsigned)(my_tmx.tmx_year + 1900),
		(unsigned)(my_tmx.tmx_mon + 1), /* modified by Edward 20070521 */
		(unsigned)my_tmx.tmx_mday,
		(unsigned)my_tmx.tmx_hour,
		(unsigned)my_tmx.tmx_min,
		(unsigned)my_tmx.tmx_sec);

	printf("\n\n *** time:%s\n\n",time);
	#endif
	
	capSize.width = jpegWidth;
	capSize.height = jpegHeight;
	
	appStill_GetCurSize(&capSizeBk.width,&capSizeBk.height);   
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSize);
	for(i=0; i<=yuvCapIdx; i++) 
	{
		/*printf("yuv save i=%d\n",i);*/
		profLogPrintf(0, "jpgSave:%d", i);

		#if defined(CUSTOM_EXIF_TIME_SET)
		//config date-stamp time as exif time
		sp5kSystemCfgSet(SP5K_EXIF_CUSTOM_RTC_CFG,time);
		#endif

		sp5kPreviewJpegSave(yuvFrameInfo[i].pbuf, &(yuvFrameInfo[i].roi), jpegWidth, jpegHeight);
		profLogAdd(0, "jpgSave done");
	}
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSizeBk);

	#if defined(CUSTOM_EXIF_TIME_SET)
	//disable custom RTC config
	sp5kSystemCfgSet(SP5K_EXIF_CUSTOM_RTC_CFG,"");
	#endif
	/* EVB-5330 : 42054, speedy burst hang up */
	sp5kYuvBufferFree(pyuvFrameBuf);
	//sp5kFree(pExif);
	pyuvFrameBuf = NULL;
	yuvSaveDone = 0;	

	sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,gSensorMode);
	appModeSet(SP5K_MODE_STILL_PREVIEW);

	appBtnEnable(BTN_ALL);
	/*appOsdMessageBoxShow(FALSE, ID_STR_IMAGE_SAVED);*/
	app_SpeedRemainNumUpdate();

	profLogAdd(0, "Save YUV to Jpeg E");
	
}

static void
_Speedy16in1Proc(
	ULONG lParam
)
{
	UINT32 i=0, j=0, blkSize=0, count=0;
	yuvFrameInfo_t yuv16in1Info;
	UINT8 *pyuv16in1Buf;
    tmx_t my_tmx;
    uiPara_t* puiPara = appUiParaGet();
    sp5kImgSize_t capSize,capSizeBk;
	
	#if defined(CUSTOM_EXIF_TIME_SET)
	UINT8 time[32];
	#endif
	profLogAdd(0, "Save YUV to Jpeg S");

	sp5kRtcDateTimeGet(0, &my_tmx);
	if(puiPara->StampMode != UI_STAMP_OFF) {
		sp5kPreviewYuvCapDateTimeSet(&my_tmx);
	}

	#if defined(CUSTOM_EXIF_TIME_SET)
	sprintf((char *)time, "%04u:%02u:%02u:%02u:%02u:%02u",
		(unsigned)(my_tmx.tmx_year + 1900),
		(unsigned)(my_tmx.tmx_mon + 1), /* modified by Edward 20070521 */
		(unsigned)my_tmx.tmx_mday,
		(unsigned)my_tmx.tmx_hour,
		(unsigned)my_tmx.tmx_min,
		(unsigned)my_tmx.tmx_sec);

	printf("\n\n *** time:%s\n\n",time);
	#endif

	pyuv16in1Buf = sp5kYuvBufferAlloc(jpegWidth*4,jpegHeight*4);
	blkSize = 512*40;
	count= jpegWidth*jpegHeight*2/blkSize;

	for(i=0; i<yuvCapMaxNum/4; i++)
	{
		for(j=0; j<count; j++)
		{
			memcpy(pyuv16in1Buf+blkSize*(4*j)+(jpegWidth*4*jpegHeight*2*i), yuvFrameInfo[i*4].pbuf+blkSize*j, blkSize);
			memcpy(pyuv16in1Buf+blkSize*(4*j+1)+(jpegWidth*4*jpegHeight*2*i), yuvFrameInfo[i*4+1].pbuf+blkSize*j, blkSize);
			memcpy(pyuv16in1Buf+blkSize*(4*j+2)+(jpegWidth*4*jpegHeight*2*i), yuvFrameInfo[i*4+2].pbuf+blkSize*j, blkSize);
			memcpy(pyuv16in1Buf+blkSize*(4*j+3)+(jpegWidth*4*jpegHeight*2*i), yuvFrameInfo[i*4+3].pbuf+blkSize*j, blkSize);
		}
	}
	yuv16in1Info.roi.width = jpegWidth*4;
	yuv16in1Info.roi.height= jpegHeight*4;
	yuv16in1Info.roi.roix  = 0;
	yuv16in1Info.roi.roiy  = 0;
	yuv16in1Info.roi.roiw  = jpegWidth*4;
	yuv16in1Info.roi.roih  = jpegHeight*4;
	
/*junqiong.hu@20130108 add for mantis bug 0046679 */
	capSize.width = yuv16in1Info.roi.width;
	capSize.height = yuv16in1Info.roi.height;
	
	appStill_GetCurSize(&capSizeBk.width,&capSizeBk.height);   
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSize);
	sp5kPreviewJpegSave(pyuv16in1Buf, &(yuv16in1Info.roi), jpegWidth*4, jpegHeight*4);
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSizeBk);
	profLogAdd(0, "jpgSave done");

	#if defined(CUSTOM_EXIF_TIME_SET)
	//disable custom RTC config
	sp5kSystemCfgSet(SP5K_EXIF_CUSTOM_RTC_CFG,"");
	#endif
	/* EVB-5330 : 42054, speedy burst hang up */
	sp5kYuvBufferFree(pyuvFrameBuf);
	sp5kYuvBufferFree(pyuv16in1Buf);
	//sp5kFree(pExif);
	pyuvFrameBuf = NULL;
	pyuv16in1Buf = NULL;
	yuvSaveDone = 0;	

	sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,gSensorMode);
	appModeSet(SP5K_MODE_STILL_PREVIEW);

	appBtnEnable(BTN_ALL);
	/*appOsdMessageBoxShow(FALSE, ID_STR_IMAGE_SAVED);*/
	app_SpeedRemainNumUpdate();

	profLogAdd(0, "Save YUV to Jpeg E");
	
}

void 
app_SpeedRemainNumUpdate(
	void
)
{
	appDiskInfo_t* pDsk; 
	UINT32 cfg;
	uiPara_t* puiPara = appUiParaGet();

	gStillCB.estFileSize = appStillEstFileSize(puiPara->ImageSize, puiPara->ImageQuality, gStillCB.diskClusterSize);			
	cfg = (puiPara->AEB == 1)? 3: IS_DRIVE_DOUBLE? 2: 1;
	gStillCB.estJobFilesSize = gStillCB.estFileSize*cfg;
	appStillBackgroundSet(gStillCB.estFileSize);
	pDsk = appCurDiskInfoGet();
	appStillBackgroundDiskFreeUpdate(pDsk->freeSz);
	appStillCheckReady(CAN_I_START_SNAP);
	appViewOsdShow(1,0,gStillCB.osd);
}

void
appStillSpeedyBurstDestroy(
	void
)
{
	/* kill saving file thread */
	if(thdSpeedyBurst) 
	{
		sp5kOsThreadDelete(thdSpeedyBurst);
		thdSpeedyBurst = 0;/*Edward 20100114*/
	}
	
}
UINT32
appStillSpeedyBurstJobDone(
	void
)
{	// busy :1 JobDone : 0
	return yuvSaveDone;
}


void appStillSpeedyCapture(UINT32	CapNum)
{

	UINT32 ret;
	#if 0  // for clean warning
	UINT32 i;	
	#endif
	/*sp5kGfxPicture_t picInfo;
	sp5kGfxPageCapabDesc_t pageCap;	
	UINT8 *pTemp=0;*/  // for clean warning
	UINT32 zoomFactor; 
	
	jpegWidth = 640;
	jpegHeight = 480;
	gSensorMode = sp5kModeswSensorModeGet(SP5K_MODE_STILL_PREVIEW);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, jpegWidth);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, jpegHeight);
	zoomFactor = appDZoomGetRation();   /*modify for mantis bug 0046758*/
	sp5kZoomFactorSet(SP5K_MODE_VIDEO_PREVIEW, zoomFactor);
	sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW,0x30);

	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
	

	if (thdSpeedyBurst) 
	{
		appStillSpeedyBurstDestroy();
	}


	yuvCapMaxNum = CapNum;
	yuvCapDone = 0;
	yuvSaveDone = 1;
	yuvCapIdx = -1;
	memset(yuvFrameInfo, 0, sizeof(yuvFrameInfo));
	yuvFrameBufSize = ROUND_UP_TO_DIVISIBLE(jpegWidth * jpegHeight * 2, 16) * (yuvCapMaxNum + 1);
	pyuvFrameBuf = sp5kYuvBufferAlloc(jpegWidth* (yuvCapMaxNum + 1),jpegHeight);
	if (pyuvFrameBuf) 
	{
		sp5kPreviewYuvCapCallbackSet(_pvYuvCapCallback);
		ret = sp5kPreviewYuvCapStart(yuvCapMaxNum, pyuvFrameBuf, yuvFrameBufSize);
		printf("[%s %d] start to capture preview YUV(%d) ret=%d\n", __FUNCTION__, __LINE__, yuvCapMaxNum, ret);
		if (ret == SUCCESS) 
		{
			
			profLogAdd(0, "Wait Preview YUV S");
			while (!yuvCapDone) 
			{
				sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
			}
			profLogAdd(0, "Wait Preview YUV E");
			#if defined(CUSTOM_EXIF_TIME_SET)
			//get RTC for exif and date-stamp
			sp5kRtcDateTimeGet( 0, &my_tmx);
			my_tmx.tmx_mon -= 1; /* 1~12 to 0~11 */
			#endif
			/* To add the datestamp, need to modify . */
			#if 0 
			if(psViewSet->nDateStamp != IMAGE_DATE_STAMP_DATE_OFF) {
				View_CfgDateStamp(IS_Q_MODE);
				#if 1 /*the original stamp may refer the bigest raw size. */
				UINT16 uwImgW, uwImgH;
				View_GetStillSize(PIC_SIZE_1,psViewSet->nAspectRatio, &uwImgW, &uwImgH);
				#if SPCA5330
				sp5kRtcDateTimeGet( 0, &my_tmx);
				View_SetDateStamp(psViewSet->nDateStamp, gsUI.Setup.nDateStyle );
				sp5kPreviewYuvCapDateTimeSet(&my_tmx);
				#endif
				#if !defined(CUSTOM_EXIF_TIME_SET)
				sp5kRtcDateTimeGet( 0, &my_tmx);
				my_tmx.tmx_mon -= 1; /* 1~12 to 0~11 */
				#endif
				#if SPCA5330
				#if 0
				gStampInfo.scaleRatio = ((UINT32)(gStampInfo.scaleRatio)*uwImgW/640);
				gStampInfo.x          = ((UINT32)(gStampInfo.x)*uwImgW/640);
				gStampInfo.y          = ((UINT32)(gStampInfo.y)*uwImgW/640);
				#endif
				#endif
				#endif
				sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, &gStampInfo);
				/*
				printf("gStampInfo.scaleRatio=%d\n", gStampInfo.scaleRatio);
				printf("gStampInfo.x=%d\n", gStampInfo.x);
				printf("gStampInfo.y=%d\n", gStampInfo.y);
				*/
				#if 0
				/* datestamp process 1~29, the 30th has processed. */
				snapStampProcess(yuvFrameInfo[yuvCapIdx].pbuf,
					&my_tmx, yuvFrameInfo[yuvCapIdx].roi.width,
					yuvFrameInfo[yuvCapIdx].roi.height, 0);
				#else
				profLogAdd(0, "stamp -s");
				for(i=0; i<=yuvCapIdx; i++) {
				#if SPCA5330
					//snapStampProcess(yuvFrameInfo[i].pbuf, &my_tmx, yuvFrameInfo[i].roi.width, yuvFrameInfo[i].roi.height, yuvFrameInfo[i].roi.width, yuvFrameInfo[i].roi.height, 0);
				#else
					snapStampProcess(yuvFrameInfo[i].pbuf, &my_tmx, yuvFrameInfo[i].roi.width, yuvFrameInfo[i].roi.height);
				#endif
				}
				profLogAdd(0, "stamp -e");
				#endif
			}
			#endif
			
			thdSpeedyBurst = sp5kOsThreadCreate("thdviewSpeedyBst", _SpeedyBurstProc, 0,  16, 16, 1, TX_AUTO_START);
			sp5kPreviewYuvCapStop();
			printf("[%s %d] stop capturing preview YUV\n", __FUNCTION__, __LINE__);
		}
		else
		{
			printf("[%s %d] ERROR !!!!\n", __FUNCTION__, __LINE__);
		}
		sp5kPreviewYuvCapCallbackSet(NULL);
    
	}
	else 
	{
		printf("[warning] no buffer for speedy burst\n");
	}
	profLogAdd(0, "speedy burst end");
}

void app16in1Capture(UINT32 CapNum)
{

	UINT32 ret;
    /*UINT32 i;
	sp5kGfxPicture_t picInfo;
	sp5kGfxPageCapabDesc_t pageCap;
	UINT8 *pTemp=0;*/   // for clean warning
	UINT32 zoomFactor; 
	
	jpegWidth = 640;
	jpegHeight = 480;
	gSensorMode = sp5kModeswSensorModeGet(SP5K_MODE_STILL_PREVIEW);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, jpegWidth);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, jpegHeight);
	zoomFactor = appDZoomGetRation();
	sp5kZoomFactorSet(SP5K_MODE_VIDEO_PREVIEW, zoomFactor);
	sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW,0x30);

	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
	

	/*Edward 20100114*/
	if (thdSpeedyBurst) 
	{
		appStillSpeedyBurstDestroy();
	}


	yuvCapMaxNum = CapNum;
	yuvCapDone = 0;
	yuvSaveDone = 1;
	yuvCapIdx = -1;
	memset(yuvFrameInfo, 0, sizeof(yuvFrameInfo));
	yuvFrameBufSize = ROUND_UP_TO_DIVISIBLE(jpegWidth * jpegHeight * 2, 16) * (yuvCapMaxNum + 1);
	pyuvFrameBuf = sp5kYuvBufferAlloc(jpegWidth* (yuvCapMaxNum + 1),jpegHeight);
	if (pyuvFrameBuf) 
	{
		sp5kPreviewYuvCapCallbackSet(_pvYuvCapCallback);
		ret = sp5kPreviewYuvCapStart(yuvCapMaxNum, pyuvFrameBuf, yuvFrameBufSize);
		printf("[%s %d] start to capture preview YUV(%d) ret=%d\n", __FUNCTION__, __LINE__, yuvCapMaxNum, ret);
		if (ret == SUCCESS) 
		{
			
			profLogAdd(0, "Wait Preview YUV S");
			while (!yuvCapDone) 
			{
				sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
			}
			profLogAdd(0, "Wait Preview YUV E");
			#if defined(CUSTOM_EXIF_TIME_SET)
			//get RTC for exif and date-stamp
			sp5kRtcDateTimeGet( 0, &my_tmx);
			my_tmx.tmx_mon -= 1; /* 1~12 to 0~11 */
			#endif
			
			thdSpeedyBurst = sp5kOsThreadCreate("thdviewSpeedyBst", _Speedy16in1Proc, 0,  16, 16, 1, TX_AUTO_START);
			sp5kPreviewYuvCapStop();
			printf("[%s %d] stop capturing preview YUV\n", __FUNCTION__, __LINE__);
		}
		else
		{
			printf("[%s %d] ERROR !!!!\n", __FUNCTION__, __LINE__);
		}
		sp5kPreviewYuvCapCallbackSet(NULL);
    
	}
	else 
	{
		printf("[warning] no buffer for speedy burst\n");
	}
	profLogAdd(0, "speedy burst end");
}


/*-------------------------------------------------------------------------
 *  Function Name : appMiniatureEffectPvCb
 *  Description :  Miniature effect pv callback
 *------------------------------------------------------------------------*/
 
#define ENABLE_YUV_SCALE 1
#define ENABLE_MOE_Y_BLEND 1
#define ENABLE_MOE_YUV_BLEND (!ENABLE_MOE_Y_BLEND)
#define MAX_ALPHA_RANGE	256

UINT32 
appMiniatureEffectPvCb(
	UINT32 idx,
	UINT8 *yuvAddr, 
	UINT32 srcW, 
	UINT32 srcH
)
{
#if CAM_TYPE_CVR
	idx = idx;
	yuvAddr = yuvAddr;
	srcW = srcW;
	srcH = srcH;
#else
	UINT8 *pTemp=NULL,*psrc=NULL;
	UINT8 *pAlpha=NULL,*pWork=NULL;
	UINT8 Alpha_slope=1;
	UINT32 scaleW=0, scaleH=0;
	UINT32 ScaleRatioX=1, ScaleRatioY=1;
	UINT32 srcUH=0,srcDH=0; 
	UINT32 blurNBlendH=0, blurH=0;/* blur but no blending section, blur section */
	UINT32 uJumpLine=0;
	#if (ENABLE_MOE_Y_BLEND==0)&&(ENABLE_MOE_YUV_BLEND==1) // for clean warning
	UINT32 PhotoParam[2],ImageParam[2];
	UINT32 mode, ret = 0 ;
	sp5kGfxObj_t Msk, img;
	#endif
	UINT32 i=0,j=0, AlphaV=0, AlphaLoopH=256 ;
	sp5kGfxObj_t gfxObjSrc;
	sp5kGfxObj_t gfxObjOp;
	#if (ENABLE_YUV_SCALE==0) // for clean warning
	sp5kGfxObj_t gfxMcpySrc,gfxMcpyDst;
	#endif

	profLogPrintf(0,"appMiniatureEffectPvCb s,0x%x,%d,%d",(UINT32)yuvAddr,srcW,srcH);
	profLogPrintf(0,"yuv scale,moe y blend,moe yuv blend [%d,%d,%d]",ENABLE_YUV_SCALE,ENABLE_MOE_Y_BLEND,ENABLE_MOE_YUV_BLEND);


#if 0
		/*load a JPG	------> get ImgBuf*/
		sprintf(szFile, "D:\\INPUT.YUV");
		fsReadFile(szFile,(UINT8 *)yuvAddr, width*height*2);
#endif

		/* decide scale factor */
#if ENABLE_YUV_SCALE 
	if( srcW > 320 && (3*srcW==4*srcH))/* 4 : 3 */
	{
		ScaleRatioX = srcW / 320;
		ScaleRatioY = srcH / 240;
	}
	else if(srcW > 320 && (9*srcW==16*srcH))/* 16 : 9 */
	{
		ScaleRatioX = srcW / 320;
		ScaleRatioY = srcH / 180;
	}
	else /* w <= 320 */
	{
		ScaleRatioX = 1;
		ScaleRatioY = 1;
	}
	profLogPrintf(0,"ScaleRatioX,ScaleRatioY [%d,%d]",ScaleRatioX,ScaleRatioY);
#else
	ScaleRatioX = 1;
	ScaleRatioY = 1;
#endif

	srcW = (srcW + 15)>>4<<4;
	srcH = (srcH + 15)>>4<<4;

	/* w,h definition */
	srcUH =(srcH/3)>>4<<4; /* blur height in up section */
	srcDH = srcH - srcUH;  /* start point for blur in down section */
	
	scaleW = ((srcW/ScaleRatioX)+15)>>4<<4;
	scaleH = ((srcUH/ScaleRatioY)+15)>>4<<4;
	
	if( scaleH * ScaleRatioY != srcUH)
	{
		srcUH = scaleH*ScaleRatioY;
		srcDH = srcH - srcUH;
	}
	
	srcDH = srcDH>>4<<4;
		
#if ENABLE_MOE_Y_BLEND
	AlphaLoopH = (scaleH/4 + 15)>>4<<4;
#else
	AlphaLoopH = (srcUH/4 + 15)>>4<<4;
#endif
		
	profLogPrintf(0,"srcUH,srcDH [%d,%d]", srcUH, srcDH);
		
	if(AlphaLoopH > MAX_ALPHA_RANGE)
	{
		AlphaLoopH = MAX_ALPHA_RANGE;
	}
		
#if ENABLE_MOE_Y_BLEND
	blurNBlendH = scaleH - AlphaLoopH;
	uJumpLine = scaleW;
#else
	blurNBlendH = srcUH - AlphaLoopH;
	uJumpLine = srcW;
#endif
		
	profLogPrintf(0,"blurNBlendH,AlphaLoopH [%d,%d]", blurNBlendH, AlphaLoopH);
		

	/* YUV buffer 1 */
	pTemp = sp5kYuvBufferAlloc(scaleW,scaleH);

	if ( !pTemp )
	{
		profLogPrintf(0,"yuv buffer allocate fail! %d\n",__LINE__);
		return FAIL;
	}
		
#if ENABLE_MOE_Y_BLEND
	pAlpha = sp5kMalloc( scaleW*AlphaLoopH );
#else
	pAlpha = sp5kMalloc( srcW*AlphaLoopH );
#endif
		

	if ( !pAlpha )
	{
		sp5kYuvBufferFree(pTemp);
		profLogPrintf(0,"alpha buffer allocate fail! %d\n",__LINE__);
		return FAIL;
	}

#if ENABLE_MOE_Y_BLEND
	/* tmp work buffer for MOE Y blending process */
	pWork = sp5kMalloc( scaleW*scaleH );
	
	if ( !pWork )
	{
		sp5kYuvBufferFree(pTemp);
		sp5kFree(pAlpha);
		profLogPrintf(0,"work buffer allocate fail! %d\n",__LINE__);
		return FAIL;
	}
#endif


	/* YUV buffer 2 */
	psrc = sp5kYuvBufferAlloc(scaleW, scaleH);
	memset(psrc,0,scaleW*scaleH*2);
	
	if ( !psrc )
	{
		sp5kYuvBufferFree(pTemp);
		sp5kFree(pAlpha);
		sp5kFree(pWork);
		profLogPrintf(0,"yuv buffer allocate fail! %d\n",__LINE__);
	}

		
	for(j=0; j< Alpha_slope; j++)/* Alpha_slope,default = 1, for pv performance purpose */
	{
		blurH = srcUH;

		/***********************************************************************
		**		Upper part: moe fmt change, moe filter, alpha blending
		**
		***********************************************************************/
		
	#if (ENABLE_YUV_SCALE==0)
		/* 1. copy yuvAddr data to pTemp */
		gfxMcpySrc.pbuf = yuvAddr;
		gfxMcpyDst.pbuf = pTemp;
		gfxMcpySrc.fmt = gfxMcpyDst.fmt = SP5K_GFX_FMT_YUV422;
		gfxMcpySrc.roiX = gfxMcpySrc.roiY = gfxMcpyDst.roiX = gfxMcpyDst.roiY = 0;
		gfxMcpySrc.bufW = gfxMcpySrc.roiW = (scaleW + 15)>>4<<4;
		gfxMcpySrc.bufH = gfxMcpySrc.roiH = (scaleH + 15)>>4<<4;
		gfxMcpyDst.bufW = gfxMcpyDst.roiW = (scaleW + 15)>>4<<4;
		gfxMcpyDst.bufH = gfxMcpyDst.roiH = (scaleH + 15)>>4<<4;
		gfxMcpySrc.paplaneBuf = gfxMcpyDst.paplaneBuf = NULL;
		profLogPrintf(0,"sp5kGfxObjectScale:1 s");
		sp5kGfxObjectScale(&gfxMcpySrc, &gfxMcpyDst);
		profLogPrintf(0,"sp5kGfxObjectScale:1 e");
	#else
		/*
		 * 1. scale yuvAddr data to pTemp 
		 */
		profLogPrintf(0,"yuvScale:1 s,0x%x,%d,%d,%d,%d",(UINT32)yuvAddr,srcW,srcUH,scaleW,scaleH);
		yuvScale(yuvAddr, srcW, srcUH, pTemp, scaleW, scaleH); 
		profLogPrintf(0,"yuvScale:1 e");
	#endif

		/* 2. format unpack : unpack Y( pTemp to psrc)	------> get Org Y buf unpackage */
		gfxObjSrc.pbuf = pTemp; /* Src */
		gfxObjOp.pbuf = psrc;	 /* Dst */
		gfxObjSrc.roiX = gfxObjSrc.roiY = gfxObjOp.roiX = gfxObjOp.roiY = 0;
		gfxObjSrc.bufW= gfxObjOp.bufW  = gfxObjSrc.roiW = gfxObjOp.roiW = scaleW;
		gfxObjSrc.bufH = gfxObjOp.bufH = gfxObjSrc.roiH = gfxObjOp.roiH = scaleH;
		gfxObjSrc.fmt = gfxObjOp.fmt = SP5K_GFX_FMT_YUV422;
		gfxObjSrc.paplaneBuf = gfxObjOp.paplaneBuf = NULL;
		profLogPrintf(0,"sample_MoeFMTChg:1 s,%d,%d",scaleW,scaleH);
		sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_YUV_PLANE, SP5K_MOE_DATA_FMT_8BIT);
		profLogPrintf(0,"sample_MoeFMTChg:1 e");

		/* 3. Moe Image Filter : Blur Y(psrc up to psrc down) */ 
		profLogPrintf(0,"sample_MoeImgFilter:1 s,%d,%d", scaleW, scaleH);
		sample_MoeImgFilter(gfxObjOp.pbuf, (gfxObjOp.pbuf+scaleW*scaleH), scaleW, scaleH);
		profLogPrintf(0,"sample_MoeImgFilter:1 e");


		/* Alpha value prepare */
		for(i=0; i<AlphaLoopH; i++)
		{
			AlphaV = i*128/AlphaLoopH;
			memset(pAlpha+i*uJumpLine,AlphaV,uJumpLine);
		}

	#if ENABLE_MOE_Y_BLEND
		/*
		  * 4. alpha blending, psrc up with psrc down, on Y channel 
		 */
		profLogPrintf(0,"sample_MoeBlendY:1 s,%d,%d,%d",scaleW,AlphaLoopH,blurNBlendH);
		sample_MoeBlendY(psrc + scaleW*blurNBlendH,psrc + scaleW*(scaleH+blurNBlendH),
		pAlpha,1, scaleW, AlphaLoopH, pWork, scaleW*scaleH);
		profLogPrintf(0,"sample_MoeBlendY:1 e");
	#else
		/* 4. format pack : combine to org IM to get Mask Img (psrc to pTemp)*/
		gfxObjSrc.pbuf = (gfxObjOp.pbuf+scaleW*scaleH);
		gfxObjOp.pbuf = pTemp;
		profLogPrintf(0,"sample_MoeFMTChg:2 s,%d,%d",scaleW,scaleH);
		sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_8BIT, SP5K_MOE_DATA_FMT_PACK_Y);
		profLogPrintf(0,"sample_MoeFMTChg:2 e");
	#endif


	#if ENABLE_MOE_Y_BLEND
		/*
		 * 5. format pack : combine to org IM to get Mask Img (psrc to pTemp)
		*/
		gfxObjSrc.pbuf = (gfxObjOp.pbuf + scaleW*scaleH); /* Src */
		gfxObjOp.pbuf = pTemp;									   /* Dst */
		profLogPrintf(0,"sample_MoeFMTChg:2 s,%d,%d",scaleW,scaleH);
		sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_8BIT, SP5K_MOE_DATA_FMT_PACK_Y);
		profLogPrintf(0,"sample_MoeFMTChg:2 e");
	#else
		
	#if 0
		memcpy(psrc, yuvAddr+w*scrHeight*2, w*AlpphaLoop*2);
	#else
		/* 5. copy the Alphaloop region of yuv to psrc : backup Alphaloop region purpose */
		gfxMcpySrc.pbuf = yuvAddr + srcW*blurNBlendH*2;
		gfxMcpyDst.pbuf = psrc;
		gfxMcpySrc.fmt = gfxMcpyDst.fmt = SP5K_GFX_FMT_YUV422;
		gfxMcpySrc.roiX = gfxMcpySrc.roiY = gfxMcpyDst.roiX = gfxMcpyDst.roiY = 0;
		gfxMcpySrc.bufW = gfxMcpySrc.roiW = (srcW + 15)>>4<<4;
		gfxMcpySrc.bufH = gfxMcpySrc.roiH = (AlphaLoopH + 15)>>4<<4;
		gfxMcpyDst.bufW = gfxMcpyDst.roiW = (srcW + 15)>>4<<4;
		gfxMcpyDst.bufH = gfxMcpyDst.roiH = (AlphaLoopH + 15)>>4<<4;
		gfxMcpySrc.paplaneBuf = gfxMcpyDst.paplaneBuf = NULL;
		profLogPrintf(0,"sp5kGfxObjectScale:11 s");
		sp5kGfxObjectScale(&gfxMcpySrc, &gfxMcpyDst);
		profLogPrintf(0,"sp5kGfxObjectScale:11 e");
	#endif
		
	#endif

	#if (ENABLE_YUV_SCALE==0)
		/* 6. copy blurred image data to org yuv buffer : (pTemp to yuvAddr) */
		gfxMcpySrc.pbuf = pTemp;
		gfxMcpyDst.pbuf = yuvAddr;
		gfxMcpySrc.fmt = gfxMcpyDst.fmt = SP5K_GFX_FMT_YUV422;
		gfxMcpySrc.roiX = gfxMcpySrc.roiY = gfxMcpyDst.roiX = gfxMcpyDst.roiY = 0;
		gfxMcpySrc.bufW = gfxMcpySrc.roiW = (scaleW + 15)>>4<<4;
		gfxMcpySrc.bufH = gfxMcpySrc.roiH = (blurH + 15)>>4<<4;
		gfxMcpyDst.bufW = gfxMcpyDst.roiW = (scaleW + 15)>>4<<4;
		gfxMcpyDst.bufH = gfxMcpyDst.roiH = (blurH + 15)>>4<<4;
		gfxMcpySrc.paplaneBuf = gfxMcpyDst.paplaneBuf = NULL;
		profLogPrintf(0,"sp5kGfxObjectScale:2 s");
		sp5kGfxObjectScale(&gfxMcpySrc, &gfxMcpyDst);
		profLogPrintf(0,"sp5kGfxObjectScale:2 e");
	#else
		/* 6. scale pTemp data to yuvAddr, to original size */
		profLogPrintf(0,"yuvScale:2 s,0x%x,%d,%d,%d,%d",(UINT32)pTemp,scaleW,scaleH,srcW,blurH);
		yuvScale(pTemp, scaleW, scaleH, yuvAddr, srcW, blurH);
		profLogPrintf(0,"yuvScale:2 e");
	#endif

	#if 0
		char fname[64]; 	
		sprintf(fname, "D:\\MINI_1.YUV");
		icatch5330YuvWrite(fname, yuvAddr, w, BlurHeight, 0);
	#endif

	#if (ENABLE_MOE_Y_BLEND==0)&&(ENABLE_MOE_YUV_BLEND==1)
		Msk.pbuf		= psrc;
		Msk.paplaneBuf	= pAlpha;
		Msk.bufW		= srcW;
		Msk.bufH		= AlphaLoopH;
		Msk.roiX		= 0;
		Msk.roiY		= 0;
		Msk.fmt 		= SP5K_GFX_FMT_YUV422;
		PhotoParam[0]	= srcW;
		PhotoParam[1]	= AlphaLoopH;
		
		img.pbuf		= yuvAddr + srcW*blurNBlendH*2;
		img.paplaneBuf	= NULL;
		img.bufW		= srcW;
		img.bufH		= AlphaLoopH;
		img.roiX		= 0;
		img.roiY		= 0;
		img.fmt 		= SP5K_GFX_FMT_YUV422;
		ImageParam[0]	= srcW;
		ImageParam[1]	= AlphaLoopH;

		/* 7. Alpha blending : org yuv & blurred yuv */
		profLogPrintf(0,"sp5kResPhotoframeAlphaMskBlendRoi:1 s");
		ret = sp5kResPhotoframeAlphaMskBlendRoi(&Msk, &img,PhotoParam,ImageParam);
		profLogPrintf(0,"sp5kResPhotoframeAlphaMskBlendRoi:1 e");
		if( ret!=SUCCESS )
		{
			profLogPrintf(0,"\n in %s %d blend error \n", __FUNCTION__, __LINE__);
			return 0;
		} 
	#endif
		
		/*********************************************************************
		**		Lower part: moe fmt change, moe filter, alpha blending
		**
		*********************************************************************/
		
	#if (ENABLE_YUV_SCALE==0)
		gfxMcpySrc.pbuf = yuvAddr + srcW*srcDH*2;
		gfxMcpyDst.pbuf = pTemp;
		gfxMcpySrc.fmt = gfxMcpyDst.fmt = SP5K_GFX_FMT_YUV422;
		gfxMcpySrc.roiX = gfxMcpySrc.roiY = gfxMcpyDst.roiX = gfxMcpyDst.roiY = 0;
		gfxMcpySrc.bufW = gfxMcpySrc.roiW = (scaleW + 15)>>4<<4;
		gfxMcpySrc.bufH = gfxMcpySrc.roiH = (blurH + 15)>>4<<4;
		gfxMcpyDst.bufW = gfxMcpyDst.roiW = (scaleW + 15)>>4<<4;
		gfxMcpyDst.bufH = gfxMcpyDst.roiH = (blurH + 15)>>4<<4;
		gfxMcpySrc.paplaneBuf = gfxMcpyDst.paplaneBuf = NULL;
		profLogPrintf(0,"sp5kGfxObjectScale:3 s");
		sp5kGfxObjectScale(&gfxMcpySrc, &gfxMcpyDst);
		profLogPrintf(0,"sp5kGfxObjectScale:3 e");
	#else
		/* 
		 * 1. scale yuvAddr data to pTemp 
		 */
		profLogPrintf(0,"yuvScale:3 s,0x%x,%d,%d,%d,%d",(UINT32)yuvAddr + srcW*srcDH*2,srcW,blurH,scaleW,scaleH);
		yuvScale(yuvAddr + srcW*srcDH*2, srcW, blurH, pTemp, scaleW, scaleH); 
		profLogPrintf(0,"yuvScale:3 e");
	#endif

		memset(psrc,0,scaleW*scaleH*2);

		/* 2. format unpack : unpack Y( pTemp to psrc)	------> get Org Y buf unpackage */
		gfxObjSrc.pbuf = pTemp; /* Src */
		gfxObjOp.pbuf = psrc;	 /* Dst */
		gfxObjSrc.roiX = gfxObjSrc.roiY = gfxObjOp.roiX = gfxObjOp.roiY = 0;
		gfxObjSrc.bufW= gfxObjOp.bufW  = gfxObjSrc.roiW = gfxObjOp.roiW = scaleW;
		gfxObjSrc.bufH = gfxObjOp.bufH = gfxObjSrc.roiH = gfxObjOp.roiH = scaleH;
		gfxObjSrc.fmt = gfxObjOp.fmt = SP5K_GFX_FMT_YUV422;
		gfxObjSrc.paplaneBuf = gfxObjOp.paplaneBuf = NULL;
		profLogPrintf(0,"sample_MoeFMTChg:3 s,%d,%d",scaleW,scaleH);
		sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_YUV_PLANE, SP5K_MOE_DATA_FMT_8BIT);
		profLogPrintf(0,"sample_MoeFMTChg:3 e");

		/* 3. Moe Image Filter : Blur Y(psrc up to psrc down) */ 
		profLogPrintf(0,"sample_MoeImgFilter:2 s,%d,%d",scaleW,scaleH);
		sample_MoeImgFilter(gfxObjOp.pbuf, (gfxObjOp.pbuf + scaleW*scaleH), scaleW, scaleH);
		profLogPrintf(0,"sample_MoeImgFilter:2 e");

		/* Alpha value prepare */
		for(i=0; i<AlphaLoopH; i++)
		{
			AlphaV = (127-i*128/AlphaLoopH);
			memset(pAlpha+i*uJumpLine, AlphaV, uJumpLine);
		}

	#if ENABLE_MOE_Y_BLEND
		/* 
		 * 4. alpha blending, psrc up with psrc down, on Y channel
		 */
		profLogPrintf(0,"sample_MoeBlendY:2 s,%d,%d",scaleW,AlphaLoopH);
		sample_MoeBlendY(psrc,psrc + scaleW*scaleH,
		pAlpha, 1, scaleW, AlphaLoopH, pWork,scaleW*scaleH);
		profLogPrintf(0,"sample_MoeBlendY:2 e");
	#else
		gfxObjSrc.pbuf = (gfxObjOp.pbuf+scaleW*scaleH);
		gfxObjOp.pbuf = pTemp;
		profLogPrintf(0,"sample_MoeFMTChg:4 s,%d,%d",scaleW,scaleH);
		sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_8BIT, SP5K_MOE_DATA_FMT_PACK_Y);
		profLogPrintf(0,"sample_MoeFMTChg:4 e");
	#endif

	#if ENABLE_MOE_Y_BLEND
		/*
		 * 5. format pack : combine to org IM to get Mask Img (psrc to pTemp)
		*/
		gfxObjSrc.pbuf = (gfxObjOp.pbuf+scaleW*scaleH); /* Src */
		gfxObjOp.pbuf = pTemp;									 /* Dst */
		profLogPrintf(0,"sample_MoeFMTChg:4 s,%d,%d",scaleW,scaleH);
		sample_MoeFMTChg(&gfxObjSrc, &gfxObjOp, SP5K_MOE_DATA_FMT_8BIT, SP5K_MOE_DATA_FMT_PACK_Y);
		profLogPrintf(0,"sample_MoeFMTChg:4 e");
	#else
		gfxMcpySrc.pbuf = yuvAddr + srcW*srcDH*2;
		gfxMcpyDst.pbuf = psrc;
		gfxMcpySrc.fmt = gfxMcpyDst.fmt = SP5K_GFX_FMT_YUV422;
		gfxMcpySrc.roiX = gfxMcpySrc.roiY = gfxMcpyDst.roiX = gfxMcpyDst.roiY = 0;
		gfxMcpySrc.bufW = gfxMcpySrc.roiW = (srcW + 15)>>4<<4;
		gfxMcpySrc.bufH = gfxMcpySrc.roiH = (AlphaLoopH + 15)>>4<<4;
		gfxMcpyDst.bufW = gfxMcpyDst.roiW = (srcW + 15)>>4<<4;
		gfxMcpyDst.bufH = gfxMcpyDst.roiH = (AlphaLoopH + 15)>>4<<4;
		gfxMcpySrc.paplaneBuf = gfxMcpyDst.paplaneBuf = NULL;
		profLogPrintf(0,"sp5kGfxObjectScale:31 s");
		sp5kGfxObjectScale(&gfxMcpySrc, &gfxMcpyDst);
		profLogPrintf(0,"sp5kGfxObjectScale:31 e");
	#endif
		
	#if (ENABLE_YUV_SCALE==0)
		gfxMcpySrc.pbuf = pTemp;
		gfxMcpyDst.pbuf = yuvAddr + srcW*srcDH*2;
		gfxMcpySrc.fmt = gfxMcpyDst.fmt = SP5K_GFX_FMT_YUV422;
		gfxMcpySrc.roiX = gfxMcpySrc.roiY = gfxMcpyDst.roiX = gfxMcpyDst.roiY = 0;
		gfxMcpySrc.bufW = gfxMcpySrc.roiW = (scaleW + 15)>>4<<4;
		gfxMcpySrc.bufH = gfxMcpySrc.roiH = (blurH + 15)>>4<<4;
		gfxMcpyDst.bufW = gfxMcpyDst.roiW = (scaleW + 15)>>4<<4;
		gfxMcpyDst.bufH = gfxMcpyDst.roiH = (blurH + 15)>>4<<4;
		gfxMcpySrc.paplaneBuf = gfxMcpyDst.paplaneBuf = NULL;
		profLogPrintf(0,"sp5kGfxObjectScale:4 s");
		sp5kGfxObjectScale(&gfxMcpySrc, &gfxMcpyDst);
		profLogPrintf(0,"sp5kGfxObjectScale:4 e");
	#else
		/*
		 * 6. scale pTemp data to yuvAddr, to original size 
		*/
		profLogPrintf(0,"yuvScale:4 s,0x%x,%d,%d,%d,%d",(UINT32)pTemp,scaleW,scaleH,srcW,blurH);
		yuvScale(pTemp, scaleW, scaleH, yuvAddr + srcW*srcDH*2, srcW, blurH);
		profLogPrintf(0,"yuvScale:4 e");
	#endif
		

	#if (ENABLE_MOE_Y_BLEND==0)&&(ENABLE_MOE_YUV_BLEND==1)
		/*Msk.pbuf		= psrc;*/
		img.pbuf		= yuvAddr + srcW*srcDH*2;
		
		profLogPrintf(0,"sp5kResPhotoframeAlphaMskBlendRoi:2 s");
		ret = sp5kResPhotoframeAlphaMskBlendRoi(&Msk, &img,PhotoParam,ImageParam);
		profLogPrintf(0,"sp5kResPhotoframeAlphaMskBlendRoi:2 e");

	#if 0
		sprintf(szFile, "D:\\OUT.JPG");
		sp5kResourceJpgSave(yuvAddr, w, h, w,h, szFile);
		/*jpgPlay(szFile,0,0,width,height);*/
	#endif
	
	if( ret!=SUCCESS )
	{
		profLogPrintf(0,"\n in %s %d blend error \n", __FUNCTION__, __LINE__);
		return 0;
	} 
	#endif
	}
	/*__EXIT__:*/
	if(psrc != NULL){
		sp5kYuvBufferFree(psrc);
		psrc = NULL;
	}
	if(pTemp != NULL){
		sp5kYuvBufferFree(pTemp);
		pTemp = NULL;
	}
	if(pAlpha != NULL){
		sp5kFree(pAlpha);
		pAlpha = NULL;
	}
	#if ENABLE_MOE_Y_BLEND
	if(pWork != NULL){
		sp5kFree(pWork);
		pWork = NULL;
	}
	#endif
	profLogPrintf(0,"appMiniatureEffectPvCb e");
#endif /* CAM_TYPE_CVR */
	return SUCCESS;
}

/* Experiment shows that 8bit result is exactly the same as 16bit pTmp1 & pTmp2,
   But 8bit is 10% faster */
static void sample_MoeBlendY(
  UINT8 *pSrc, /* source Y */
  UINT8 *pDst, /* destination Y */
  UINT8 *pAlpha, /* alpha mask */
  UINT32 fAlphaSrc, /* alpha is for source if fAlphaSrc!=0, otherwise alpha is for destination */
  UINT32 w, UINT32 h, /* dimension of Y & alpha */
  UINT8 *pwork, UINT32 worksize /* temp buffer pointer & size */
)
{
	UINT32 y;
	UINT32 h1 =((worksize/w)/4) & ~15;
	UINT32 l=(w*h1+511) & ~511;
	HOST_ASSERT(l*4 <= worksize);
	UINT8 *pTmp1=pwork;
	UINT8 *pTmp2=pTmp1 + l;
	UINT8 *pAlphaFull=pTmp2 + l;
	UINT8 *pAlphaComp=pAlphaFull + l; /* complement of pAlpha */
	UINT32 *pAlpha1,*pAlpha2;
 
	sp5kMoeObj_t src1={NULL,w,h1,0,0,SP5K_MOE_DATA_FMT_8BIT,SP5K_MOE_DATA_TYPE_UNSIGNED,0,SP5K_MOE_DATA_ABS_DIS};
	sp5kMoeObj_t src2={NULL,w,h1,0,0,SP5K_MOE_DATA_FMT_8BIT,SP5K_MOE_DATA_TYPE_UNSIGNED,0,SP5K_MOE_DATA_ABS_DIS};
	sp5kMoeObj_t dst ={NULL,w,h1,0,0,SP5K_MOE_DATA_FMT_8BIT,SP5K_MOE_DATA_TYPE_UNSIGNED,0,SP5K_MOE_DATA_ABS_DIS};
	sp5kMoeClipCfg_t clip={0,0x80};
 
	UINT32 moeHandle = sp5kMoeHandleInit();
	HOST_ASSERT(moeHandle);
	sp5kGfxObj_t obj={pAlphaFull, NULL, w, h1, 0, 0, SP5K_GFX_FMT_IDX8, w, h1};
	sp5kGfxObjectFill(&obj, 0x80);
	for (y=0 ; y<h ; y+=h1) {
		if (y+h1>h) { h1 = h - y; }
		/* pAlphaComp = 0x80 - pAlpha ***********************************************/
		src1.pbuf = pAlphaFull;
		src2.pbuf = pAlpha + y*w;
		dst.pbuf = pAlphaComp;
		sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src2, &dst, &clip, w, h1, SP5K_MOE_MAT_CAL_SUB);
		if (fAlphaSrc) {
			pAlpha1 = src2.pbuf;
			pAlpha2 = dst.pbuf;
		} else {
			pAlpha1 = dst.pbuf;
			pAlpha2 = src2.pbuf;
		}
		/* pTmp1 = (pSrc * pAlpha1) >> 7 ********************************************/
		src1.pbuf = pSrc + y*w;
		src2.pbuf = pAlpha1;
		dst.pbuf = pTmp1; dst.shift = 7;
		sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src2, &dst, NULL, w, h1, SP5K_MOE_MAT_CAL_MUL);
		/* pTmp2 = (pDst * pAlpha2) >> 7 ********************************************/
		src1.pbuf = pDst + y*w;
		src2.pbuf = pAlpha2;
		dst.pbuf = pTmp2;
		sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src2, &dst, NULL, w, h1, SP5K_MOE_MAT_CAL_MUL);
		/* pDst = (pTmp1 + PTmp2) *************************************************/
		src1.pbuf = pTmp1;
		src2.pbuf = pTmp2;
		dst.pbuf = pDst + y*w; dst.shift = 0;	
		sp5kMoeMatrixPointByPointSet(moeHandle, &src1, &src2, &dst, NULL, w, h1, SP5K_MOE_MAT_CAL_ADD);
		sp5kMoeHandleRun(moeHandle);
		sp5kMoeHandleReset(moeHandle);
	}
	sp5kMoeHandleRelease(moeHandle);
}


