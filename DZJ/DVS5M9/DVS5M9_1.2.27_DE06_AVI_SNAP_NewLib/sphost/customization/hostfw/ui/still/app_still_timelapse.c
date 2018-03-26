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
 /**
  * @file		 app_still_timelapse.c
  * @brief		 App for timelapse
  * @author  junqiong.hu
  * @date		 2013-07-16
  */

#include "app_com_api.h"
#include "app_ui_para.h"
#include "app_view_param_def.h"
#include "sp5k_media_api.h"
#include "app_still.h"
#include "app_still_timelapse.h"
#include "app_cdfs_api.h"
#include "app_msg.h"
#include "sp5k_capture_api.h"
#include "app_video.h"
#include "sp5k_rsvblk_api.h"
#include "app_osd_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if OLD_TLS_OPT
#define TLS_TIMER_UNIT	100   //ms
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if OLD_TLS_OPT
UINT8 TimLapVidFile[64];
UINT8 timelapse_act=0;
static UINT32 TlsGapTimer = TIMER_NULL;
static UINT32 TlsTmrCnt = 0;
static UINT8 JpgfileName[128];
extern UINT32 SnapSetflag;
static UINT32 Tlscnt = 0;
#endif
UINT32 timelapseflag = TS_OFF;
UINT32 trigFlg = 0;
UINT8 standbyflag = 0;
UINT32 yuvCBIntFlg = 0;
UINT32 yuvCBRdyFlg = 0;
UINT8* fBuf = NULL;
UINT32 img_w=1920;
UINT32 img_h=1080;
UINT32 yuvSz = 0;
UINT32 timelapseStartTimer = 0;
UINT8 timelapse_act=0;
static tmx_t rtcn;
static UINT8 *ConvertBuf = NULL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if OLD_TLS_OPT
extern UINT32 appStillCheckReady(UINT32 question);
#endif
extern UINT32 sp5kSensorModeCfgSet(UINT32 mode,UINT32 sensormode); // for clean warning
extern void *sp5kYuvBufferAlloc(UINT32 bufW, UINT32 bufH); // for clean warning
extern void sp5kYuvBufferFree(void *ptr);
extern void ros_thread_sleep(UINT32 ms);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if OLD_TLS_OPT

void appStillTimelapseTaskInit(UINT32 Tflag)
{
	if(Tflag)
	{
		appTimelapseInit();
		timelapseflag = TS_ON;
		timelapse_act=1;
	}
	else
	{
		if(TlsGapTimer != TIMER_NULL){
			appTimerIsrUnreg(&TlsGapTimer);
		}
		timelapseflag = TS_OFF;
		Tlscnt = 0;
		timelapse_act=0;
	}

}

void appTimelapseInit(void)
{
	UINT32 Timlaptime;

	Timlaptime = appTimeLapseSecGet();
	TlsTmrCnt = Timlaptime/TLS_TIMER_UNIT;

	if(TlsGapTimer != TIMER_NULL)
	{
		appTimerIsrUnreg(&TlsGapTimer);
	}

	if(TlsTmrCnt)
	{
		TlsGapTimer= appTimerIsrReg(appTlsGapProc,TLS_TIMER_UNIT/appTimerIsrTickIntervalGet());
	}
}

void appTlsGapProc(void)
{
	UINT32 ret = FAIL;

	if(Tlscnt)
	{

		Tlscnt++;
		if(Tlscnt < TlsTmrCnt)
		{
			return;
		}
	}
	else
	{
		Tlscnt++;
	}
	if(appActiveStateGet() == APP_STATE_STILL_PREVIEW)
	{
		if(appStillCheckReady(CAN_I_START_SNAP)!=STILL_STATE_YES)
		{
			if(!timelapseflag)
			{
				printf("card memory full-- timelapse set off!!");
				timelapseflag = TS_OFF;
				sp5kHostMsgSend(APP_UI_MSG_TIMELAPSE_TASK_DEL,0);
			}
			RPrintf("WARNING: STILL_STATE_NO!!!");
			return;
		}
		SnapSetflag = 1;
		profLogAdd(3,"Send Snap MSG");
		printf("send S1 msg \n");
		sp5kHostMsgSend(APP_KEY_PRESS_S1,0xaf);

	}
	else
	{
		RPrintf("WARNING:Tls ISR,capture not be done!!!");
		return;
	}

	Tlscnt = 0;
	return;
}

UINT32
appTimLapKeyMsgDispose(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->TimeLapse == UI_TIMELAPSE_OFF)
	{
		return ret;
	}

	if(timelapseflag == TS_ON)
	{
		if(IS_APP_KEY_MSG(msg))
		{
			if(msg & 0x01) /*key release*/
			{
				printf("Timelapse :key release!!!\n");
				ret = APPSTATE_MSG_IGNORE;
			}
			else if((msg != APP_KEY_PRESS_S2) &&(!(msg == APP_KEY_PRESS_S1 && param == 0xaf)))
			{
				sp5kHostMsgSend(APP_UI_MSG_TIMELAPSE_TASK_DEL,0);
				#if SP5K_CDFS_OPT
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
				appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
				appCdfsFileAdd(TimLapVidFile);
				#endif
				ret = APPSTATE_MSG_IGNORE;
			}
			else
			{
			}
		}
	}
	return ret;
}

void
appMediaTimeLapse(
	UINT8 *jpgFile
)
{	/* initial setting */
	UINT8 videoFile[64];
	UINT32 fileType = SP5K_MEDIA_AVI;
	UINT32 videoCodec = SP5K_MEDIA_VIDEO_MJPG;
	UINT32 frameRate = 30;
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->MediaFormat == MEDIA_FORMAT_AVI)
	{
	   fileType = SP5K_MEDIA_AVI;
	}
	else
	{
		fileType = SP5K_MEDIA_MOV;
	}
#if 0
	if(puiPara->VideoFormat == VIDEO_FORMAT_MJPG)
	{
		videoCodec = SP5K_MEDIA_VIDEO_MJPG;
	}
	else
	{
		videoCodec = SP5K_MEDIA_VIDEO_H264;
	}
#endif

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, fileType);
	/* Lapse should use MJPG */
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, videoCodec);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, frameRate);/* need sensor check */

	sprintf(videoFile,"%s",TimLapVidFile);
	if ( SUCCESS != sp5kMediaPlayControl(SP5K_MEDIA_PLAY_VIDEO_FRM_APPEND,(UINT32)videoFile, (UINT32)jpgFile))
	{
		printf("ERR [%s %d] Append Image File\n",__func__,__LINE__);
		return;
	}
}

void appJpgFileNameSet(UINT8 *jpgFile)
{
   strcpy(JpgfileName,jpgFile);
}

UINT8 *appJpgFileNameGet(void)
{
	return JpgfileName;
}

#endif

/*test timelapse h.264 sample code must update basefw to 17859 or higher*/
/*test code from timelapse H.264*/ /*IBBP test (enter standby mode)*/

void appTimelapseDateStamp(void)
{
	sp5kGfxPageCapabDesc_t cap;
	sp5kGfxObj_t gfxObjSrc,gfxObjDst;
	sp5kGfxPicture_t srcPic, dstPic;

	UINT8 stillStampDateBuf[128];

	ConvertBuf = sp5kYuvBufferAlloc(img_w, (img_h+15)&(~15));

	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtcn);
	sprintf(stillStampDateBuf,  "%d/%02d/%02d  %02d:%02d:%02d",rtcn.tmx_year+1900, rtcn.tmx_mon,rtcn.tmx_mday,rtcn.tmx_hour, rtcn.tmx_min, rtcn.tmx_sec);

	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
	//sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, 10, 0, 0, 0);
	sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR);
	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_DATE_STAMP_OSD, 0, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_ORANGEB, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
	sp5kGfxStringDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, STR_ENCODE_ASCII, stillStampDateBuf); /*Stamp String*/

	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_PIP_0, &cap);

	//fsSimpleWrite("D:\\RGB565.RGB", cap.pbuf, cap.frmW * cap.frmH * 2);
	printf(" %s\n", stillStampDateBuf);

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
	gfxObjDst.bufW = img_w;
	gfxObjDst.bufH = (img_h+15)&(~15);

	gfxObjDst.roiW = gfxObjSrc.roiW;
	gfxObjDst.roiH = gfxObjSrc.roiH;
	gfxObjDst.paplaneBuf = NULL;
	gfxObjDst.fmt = SP5K_GFX_FMT_YUV422;


	sp5kGfxObjectConvert(&gfxObjSrc, &gfxObjDst);

	//fsSimpleWrite("D:\\CAP.YUV", ConvertBuf, img_w * ((img_h+15)&(~15)) * 2);

	/* YUV & OSD blend */
	srcPic.pbuf = gfxObjDst.pbuf;
	srcPic.frmW = gfxObjDst.bufW;
	srcPic.frmH = gfxObjDst.bufH;
	srcPic.roiX = 0;
	srcPic.roiY = 0;
	srcPic.roiW = gfxObjSrc.roiW;	/* it must be multiple of 16x and multiple of roiW of src and dst can't over 12x */
	srcPic.roiH = gfxObjSrc.roiH;	/* it must be multiple of 16x and multiple of roiH of src and dst can't over 12x */
	srcPic.fmt  = SP5K_GFX_FMT_YUV422;

	dstPic.pbuf = fBuf;
	dstPic.frmW = img_w; /*psSizeInfo->width;*/
	dstPic.frmH = img_h;  /*psSizeInfo->height;*/
	dstPic.roiX = 0;
	dstPic.roiY = 0;
	dstPic.roiW = srcPic.roiW/*uwImgW*/;
	dstPic.roiH = srcPic.roiH/*uwImgH*/;
	dstPic.fmt  = SP5K_GFX_FMT_YUV422;

	sp5kGfxPictureBlend(&dstPic, 128, 1, 0, &srcPic);

#if 0
	fsSimpleWrite("D:\\DST.YUV", dstPic.pbuf, dstPic.frmW* dstPic.frmH*2);
	fsSimpleWrite("D:\\SRCGFX.YUV", srcPic.pbuf, srcPic.frmW * srcPic.frmH* 2);
#endif
	//fsSimpleWrite("D:\\Result.YUV", dstPic.pbuf, dstPic.frmW * dstPic.frmH * 2);

	sp5kYuvBufferFree(ConvertBuf);
	CLEAR_PIP_SRC;
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 1, 0);

}


UINT32 appTimelapsePreviewCallback(
	UINT32 fid, 	/* Frame ID */
	UINT8 *pbuf,	/* Frame buffer pointer */
	UINT32 w,	/* Buffer width */
	UINT32 h	/* Buffer height */
)
{
	__FUNC_TRACK__;

	if (yuvCBRdyFlg == 0){
		printf("pv#%d %dx%d 1st pixel=%x\n", fid, w, h, *pbuf);
		yuvCBRdyFlg = 1;
		yuvSz = w * h * 2;
		memcpy(fBuf, pbuf, yuvSz);
	}
	return 0;
}

UINT32 appTimelapseSwTrigCb(UINT8 *pbuf, sp5kYuvCbImgSize_t *pinfo, UINT32 rsvd0, UINT32 rsvd1)
{
	uiPara_t* puiPara = appUiParaGet();

	__FUNC_TRACK__;
	
	if (standbyflag)
	{
		sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
		standbyflag=0;
	}


	if (trigFlg) {
		if (yuvCBRdyFlg == 0){
			sp5kPreviewCallbackSet(15, appTimelapsePreviewCallback, img_w, img_h);
			yuvCBIntFlg = 1;
		}
		trigFlg = 0;
		while(!yuvCBRdyFlg)
			ros_thread_sleep(1);
	}
	else if (yuvCBRdyFlg){
		if(puiPara->VideoStampMode)
		{
			appTimelapseDateStamp();
		}
		printf("yuvCBRdy %x, sz %d\n", yuvCBRdyFlg, yuvSz);
		memcpy(pbuf, fBuf, yuvSz);
		sp5kPreviewCallbackSet(0, NULL, 0, 0);
		yuvCBRdyFlg = 0;

		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP,1);//keep the last frame
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		standbyflag=1;


		return SUCCESS;
	}
	return FAIL;
}

void appTimelapseStart(void)
{
	UINT32 interval = 0;
	UINT32 w,h;

	uiPara_t* puiPara = appUiParaGet();

	printf("start sw record..\n");

	if(1)//puiPara->VideoSize == UI_VID_SIZE_FHD)
	{
		img_w=1920;
		img_h=1088;

		w = 1920;
		h  = 1080;
	}
	else if(puiPara->VideoSize == UI_VID_SIZE_HD_60FPS)
	{
		img_w=1280;
		img_h=720;

		w = 1280;
		h  = 720;
	}
	else
	{
		img_w=1280;
		img_h=720;

		w = 1280;
		h  = 720;
	}

	timelapseflag = TS_ON;
	timelapse_act=1;
	yuvSz = img_w * img_h * 2;
	/* The feature should work in standby mode */
//	sp5kModeSet(SP5K_MODE_STANDBY);
//	sp5kModeWait(SP5K_MODE_STANDBY);
	standbyflag=1;

	/* 1920x1080 h264 video as an example */
	//sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_1080P );
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, w);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, h);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 30);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, MEDIA_VIDEO_H264);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, MEDIA_AUDIO_UNKNOWN);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_OPTION, 0);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, 0x0);	/* 0x00 for IBBP, 0x10 for IPPP, 0x20 for IIII */

	interval = 1000;//appTimeLapseSecGet();
	timelapseStartTimer= appTimerSet(interval, "timelpse Start Timer");

	/* malloc buffer */
	if (fBuf == NULL){
		fBuf = sp5kYuvBufferAlloc(img_w, img_h);
		if (fBuf == NULL){
			printf("Buffer malloc failed!!");			
			return;
		}
	}

	/* Register the callback to enable the feature */
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SWTRIG_CB, (UINT32)appTimelapseSwTrigCb);

	sp5kMediaRecControl(SP5K_MEDIA_REC_SWTRIG_START);
	printf("record start...\n");
}

void appTimelapseTrigger(void)
{
	trigFlg = 1;
}

void appTimelapseStop(void)
{
	if(yuvCBRdyFlg) {
		sp5kPreviewCallbackSet(0, NULL, 0, 0);
		yuvCBRdyFlg = 0;
	}
	/* invoked when received the finished signal */
	sp5kMediaRecControl(SP5K_MEDIA_REC_SWTRIG_STOP);

	/* clear the callback to disable the feature. */
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SWTRIG_CB, (UINT32)NULL);

	appTimerClear(&timelapseStartTimer);
	if (fBuf){
		sp5kYuvBufferFree(fBuf);
		fBuf = NULL;
	}
	timelapseflag = TS_OFF;
	timelapse_act=0;


	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
	printf("sw record stopped.\n");

}

UINT32
appTimLapKeyMsgDispose(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->TimeLapse == UI_TIMELAPSE_OFF)
	{
		return ret;
	}

	if(timelapse_act)
	{
		if(IS_APP_KEY_MSG(msg))
		{
			if(msg & 0x01) /*key release*/
			{
				printf("Timelapse :key release!!!\n");
				ret = APPSTATE_MSG_IGNORE;
			}
			else if((msg != APP_KEY_PRESS_S2) &&(!(msg == APP_KEY_PRESS_S1 && param == 0xaf)))
			{
				//sp5kHostMsgSend(APP_UI_MSG_TIMELAPSE_TASK_DEL,0);
				ret = APPSTATE_MSG_IGNORE;
			}
			else
			{
			}
		}
	}
	return ret;
}

