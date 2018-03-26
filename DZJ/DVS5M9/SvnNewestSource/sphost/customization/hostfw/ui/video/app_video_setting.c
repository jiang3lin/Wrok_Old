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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"

/* for display osd */
#include "sp5k_gfx_api.h"
#include "app_pal.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"
#include "sp5k_disk_api.h"

#include "gpio_custom.h"
#include "app_video.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_disk.h"
#include "app_still.h"
#include "app_calib_api.h"
#include "app_cdfs_api.h"
#include "app_ui_para.h"
#include "app_exif.h"

#if SPCA6330
#include "middleware/common_types.h"
#endif

/* include HDMI_MENU_BY_JPG  */
#include "app_menu.h"

#if ICAT_WIFI
#include "app_ptp.h"
#include "app_wifi_connection.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/*static struct {
	UINT16 w,h;
	UINT16 icon;
	UINT16 sensormode; 
	UINT8  audio;
	UINT32 frameRate ;
} */
static appVideoSize_t VideoSize[UI_VID_SIZE_MAX]={
	{ 1920,1080, ID_ICON_MOVIE_SIZE_1080P,SENSOR_MODE_1080P ,SP5K_MEDIA_AUDIO_PCM ,30},
	{ 1280, 720, ID_ICON_MOVIE_SIZE_720P,SENSOR_MODE_720P_30FPS ,SP5K_MEDIA_AUDIO_PCM ,30},
	{ 1280, 720, ID_ICON_MOVIE_SIZE_720P,SENSOR_MODE_720P_60FPS ,SP5K_MEDIA_AUDIO_PCM ,60},
	{ 1280, 960, ID_ICON_MOVIE_SIZE_D1,SENSOR_MODE_XGA_30FPS ,SP5K_MEDIA_AUDIO_PCM ,30},
	{  848, 480, ID_ICON_MOVIE_SIZE_720P,SENSOR_MODE_720P_60FPS ,SP5K_MEDIA_AUDIO_PCM ,60},
	/*#if SP5K_SENSOR_OV2715
	{ 720, 480, ID_ICON_MOVIE_SIZE_D1,SENSOR_MODE_SVGA_30FPS ,SP5K_MEDIA_AUDIO_PCM ,30},
	#endif*/
	{ 640, 480, ID_ICON_MOVIE_SIZE_640,SENSOR_MODE_VGA_120FPS ,SP5K_MEDIA_AUDIO_PCM ,120},
	{ 320, 240, ID_ICON_MOVIE_SIZE_320,SENSOR_MODE_QVGA_240FPS ,SP5K_MEDIA_AUDIO_PCM ,240},

};


BOOL gVideoRscEnTag=FALSE;
BOOL gVideoDistEnTag=FALSE;
UINT8 gVideoSizeTag =0;
/* ADD BY SC.LIU */
extern BOOL doFirstTimer;
extern BOOL getTimer;
extern UINT32 timer;
 /**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void appVideoSettingInit(UINT32 flg);
extern UINT32 sp5kSensorModeCfgSet(UINT32 mode,UINT32 sensormode); // for clean warning
extern UINT32 mediaRecAttrGet(UINT32 attrId); // for clean warning
extern UINT32 dcfFsNextFileNameGet(UINT32 fileType,UINT8 *pfilename); // for clean warning
extern void appVideoStop(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void 
appVideoStreamPerByteSet(
	UINT32 size_idx,
	UINT32 quality_idx ,
	UINT32 isSdCard
)
{


	return;
	UINT8 qualityRatio[5][3]={{17,14,11},               //`1080P
							  {17/2,14/2,11/2},	      //720P-30fps bitrate is 1080P/2
							  {17*2/3,14*2/3,11*2/3},   //720P-60FPS bitrate is 1080P*2/3
							  {17/3,14/3,11/3},         //640
							  {17/4,14/4,11/4},         //320 

	}; //superfine ,fine ,normal         // for clean warning
	UINT32 mjpgQualityRatio[5] = {150, 110, 130, 50, 30};
	uiPara_t* puiPara = appUiParaGet();
	
	
	switch( size_idx ) 
	{
	default:
	#if SPCA6330	
	case VIDEO_SIZE_1080P://16_9	   
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264){
			gVideoCB.streamBytesPerSec = qualityRatio[size_idx][quality_idx]*1000000/8;
		}else{
			gVideoCB.streamBytesPerSec = (isSdCard) ? (mjpgQualityRatio[size_idx]*30*1024) : (mjpgQualityRatio[size_idx]*30*1024);
		}

		if(puiPara->Seamless != UI_VIDEO_SEAMLESS_OFF){
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO,256);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO,256);
		}
		
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE,2*1024*1024);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT,12);
		sp5kAwbCfgSet(SP5K_AWB_ACCUM_PERIOD,1);
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 1); 

		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO,100);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO,64);
		break;
	#endif	
	case VIDEO_SIZE_720P_30FPS://16_9 720P 30FPS
	   
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264){
			gVideoCB.streamBytesPerSec =  qualityRatio[size_idx][quality_idx]*1000000/8;
		}else{
			gVideoCB.streamBytesPerSec = (isSdCard) ? (mjpgQualityRatio[size_idx]*30*1024) : (mjpgQualityRatio[size_idx]*30*1024);
		}

		if(puiPara->Seamless != UI_VIDEO_SEAMLESS_OFF){
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO,256);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO,256);
		}
		
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE,0);
		#if SPCA6330    /*Add by Lation@20130717*/
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT,0);
		#endif
		sp5kAwbCfgSet(SP5K_AWB_ACCUM_PERIOD,1);
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 1);

		#if SPCA6330    /*Add by Lation@20130717*/
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO,100);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO,64);
		#endif
		break;
	#if SPCA6330	
	case VIDEO_SIZE_720P_60FPS://16_9
		
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264){
			gVideoCB.streamBytesPerSec =  qualityRatio[size_idx][quality_idx]*1000000/8;
		}else{
			gVideoCB.streamBytesPerSec = (isSdCard) ? (mjpgQualityRatio[size_idx]*30*1024) : (mjpgQualityRatio[size_idx]*30*1024);
		}

		if(puiPara->Seamless != UI_VIDEO_SEAMLESS_OFF){
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO,420);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO,210);
		}
		
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE,2*1024*1024);
		#if SPCA6330    /*Add by Lation@20130717*/
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT,18);
		#endif
		sp5kAwbCfgSet(SP5K_AWB_ACCUM_PERIOD,2);
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 2);

            #if SPCA6330    /*Add by Lation@20130717*/
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO,0);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO,0);
		#endif
		break;	
	#endif	
	case VIDEO_SIZE_SVGA:
	case VIDEO_SIZE_640://640	
		
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264){
			gVideoCB.streamBytesPerSec =  qualityRatio[size_idx][quality_idx]*1000000/8;
		}else{
			gVideoCB.streamBytesPerSec = (isSdCard) ? (mjpgQualityRatio[size_idx]*30*1024) : (mjpgQualityRatio[size_idx]*30*1024);
		}

		if(puiPara->Seamless != UI_VIDEO_SEAMLESS_OFF){
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO,256);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO,256);
		}
		
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE,600*1024);
		#if SPCA6330    /*Add by Lation@20130717*/
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT,40);
		#endif
		sp5kAwbCfgSet(SP5K_AWB_ACCUM_PERIOD,4);
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 4);

            #if SPCA6330    /*Add by Lation@20130717*/
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO,100);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO,64);
		#endif
		break;
	case VIDEO_SIZE_QVGA://320	
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264){
			gVideoCB.streamBytesPerSec =  qualityRatio[size_idx][quality_idx]*1000000/8;
		}else{
			gVideoCB.streamBytesPerSec = (isSdCard) ? (mjpgQualityRatio[size_idx]*30*1024) : (mjpgQualityRatio[size_idx]*30*1024);
		}
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 4);
		break;	
		
	}
	
	HLPrintf1("\n streamBytesPerSec=%d\n",gVideoCB.streamBytesPerSec);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE, SP5K_MEDIA_CBR);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, gVideoCB.streamBytesPerSec<<3);
		
	if(puiPara->VideoFormat == VIDEO_FORMAT_H264){
		gVideoCB.aviKbPerSec = (gVideoCB.streamBytesPerSec /* video data */
			+ (gVideoCB.framePerSec)*(16+8) /* video hdr */
			+ VIDEO_AUDIO_SAMPLE_RATE*(VIDEO_AUDIO_SAMPLE_BIT/8)*(VIDEO_AUDIO_CHANNEL) /* audio data */
			+ 1024)>>10; /* other overhead (audio hdr, index, fat... */
	}
	else{
		gVideoCB.aviKbPerSec = (gVideoCB.streamBytesPerSec /* video data */
			+ (gVideoCB.framePerSec)*616 /* video hdr */
			+ VIDEO_AUDIO_SAMPLE_RATE*(VIDEO_AUDIO_SAMPLE_BIT/8)*(VIDEO_AUDIO_CHANNEL) /* audio data */
			+ 1024)>>10; /* other overhead (audio hdr, index, fat... */
	}
}
void 
appVideoSetQuality(
		UINT8 idx
)
{
	/*UINT32 cfg, ret;*/ // for clean warning
	uiPara_t* puiPara = appUiParaGet();
	
	appVideoStreamPerByteSet(puiPara->VideoSize,idx ,IS_CARD_EXIST);
	
} 
void 
appVidSizeSet(
	UINT8 size_idx
)
{

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, VideoSize[size_idx].w);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, VideoSize[size_idx].h);	
	profLogPrintf(0, "video size: %d %dx%d", size_idx, VideoSize[size_idx].w, VideoSize[size_idx].h);
	gVideoSizeTag =size_idx;
	#if VIDEO_WITH_1080P
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, VideoSize[size_idx].audio);
	//sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_UNKNOWN);
	sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, VideoSize[size_idx].sensormode);
	if(VideoSize[size_idx].frameRate == 60){  /*junqiong.hu@20121207 add for mantis 0046324*/
		appAeFrameRateSet(60);
	}
	else if(VideoSize[size_idx].frameRate == 120){ /*junqiong.hu@20121207 add for mantis 0046324*/
		appAeFrameRateSet(120);
	}
	else{
		appAeFrameRateSet(30);
	}
	#endif
	gVideoCB.framePerSec =  VideoSize[size_idx].frameRate ;
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, gVideoCB.framePerSec);
	#if HDMI_MENU_BY_JPG
	#if KIT_WITH_HDMI     /*Add by Lation@20130717*/
	if(!IS_HDMI_IN){
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
	}
	#endif
	#else
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
	#endif
	
	#ifdef NO_VIDEO_PREVIEW_MODE
	appViewOsdValidAreaShow(VideoSize[size_idx].w, VideoSize[size_idx].h);
	#endif
}

#if 0		/* ADD BY SC.LIU */
/*-------------------------------------------------------------------------
 *  Function Name : appCaptureFrameCb
 *  Description :
 * \brief An typical frame callback that keeps the 3A information.
 * \param[in] id The nth frame that captured.
 * \param[in] pinfo Pointer to information of current yuv buffer.
 *------------------------------------------------------------------------*/
static UINT32
appCaptureFrameCb(
	UINT32 id, 
	yuvBufInfo_t *pinfo, 
	UINT32 rsv, 
	void *rsvd
)
{
	/* keep 3A info according to id */
	
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  Function Name : appCaptureProcCb
 *  Description :   
 * \brief An typical process callback
 * \param[in] id The nth frame that captured.
 * \param[in] pinfo Pointer to information of current yuv buffer.
 * \return TRUE (1) for save file, others abandon this frame.
 *
 * - Update the 3A information to exif.
 * - Draws the info on screen.
 *
 * When this callback is not registered, the file will be always save.
*------------------------------------------------------------------------*/
static void 
appCaptureProcCb(
	UINT32 id, 
	yuvBufInfo_t *pinfo, 
	UINT32 rsv, 
	void *rsvd
)
{
	/* Update 3A information */
	/* get 3A info by id */
	
	/* set 3A info to exif header */
//	sp5kExifEncodeTagValueSet();
	

	/* Draw on Screen */
	sp5kGfxObj_t obj;
	sp5kGfxObj_t dst;
	sp5kGfxRoi_t roi;
	
	obj.pbuf = pinfo->pbuf;
	obj.paplaneBuf = NULL;
	obj.bufW = pinfo->width;
	obj.bufH = pinfo->height;
	obj.roiX = pinfo->roiX;
	obj.roiY = pinfo->roiY;
	obj.roiW = pinfo->roiW;
	obj.roiH = pinfo->roiH;
	obj.fmt = GFX_DATA_FMT_YUV422;	

	dst.pbuf = sp5kMalloc(64 * 48 * 2);
	dst.paplaneBuf = NULL;
	dst.bufW = dst.roiW = 64;
	dst.bufH = dst.roiH = 48;
	dst.roiX = dst.roiY = 0;
	dst.fmt = GFX_DATA_FMT_RGB565;
	sp5kGfxObjectScale(&obj, &dst);
	/* get the xpos/ypos by id */
	roi.roiX = xpos;
	roi.roiY = ypos;
	roi.roiW = 64;
	roi.roiH = 48;
	sp5kGfxObjectDraw(SP5K_GFX_OSD_PAGE_TOT, &roi, 0, &dst);
	return 1; /* TRUE for saving this file */
}
#endif


#if 0/*SPCA6330 mark by jj*/
/*-------------------------------------------------------------------------
 *  Function Name : appStillInMovie
 *  Description :   Still In Movie Intialization
 *------------------------------------------------------------------------*/
void appStillInMovieInit(
	void
)
{	
	/* Set max capture to 8 */
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_CAPTURE_FRAMES, 8);
	/* Register the callback */
//	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_CAPTURE_FRAME_CB, appCaptureFrameCb);
//	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_CAPTURE_PROC_CB, appCaptureProcCb);
    /* Preserve more threshold, assume each 2M.
	 * The compress ratio is controlled by 
	 * sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_COMPRESSION_PRECISE_RATIO, ...);	 */
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DISK_THR_KB, 10 * 1024 + 8 * 2 * 1024);

	timer=0xffffffff;
	doFirstTimer=TRUE;
	getTimer=FALSE;
}
/* ADD BY SC.LIU */
#endif

/*-------------------------------------------------------------------------
 *  Function Name : appVideo_SetDistRsc
 *  Description :   Set DIST and RSC Level in Video Preview
 *------------------------------------------------------------------------*/
void 
appVideo_SetDistRsc(
	void
)
{	
	UINT32 mode;
	/* Add by SC.Liu */
	if ((pViewParam->viddist==1) && (pViewParam->vidrsc==1)) {
		mode = SP5K_MEDIA_REC_RSC_DIS_MID;
		sp5kMediaRecCfgSet(SP5K_MEDIA_DIS_COMP_TYPE,  2 /* i.e. vidDistVectorGetV5Rsc*/);		
	}
	else if (pViewParam->vidrsc==1) {
		mode = SP5K_MEDIA_REC_RSC_MID;
		sp5kMediaRecCfgSet(SP5K_MEDIA_DIS_COMP_TYPE,  2 /* vidDistVectorGetV5Rsc */);
	}
	else if (pViewParam->viddist==1) {
		mode = SP5K_MEDIA_REC_DIS_MID;
		sp5kMediaRecCfgSet(SP5K_MEDIA_DIS_COMP_TYPE,  1 /* vidDistVectorGetV5 */);
	}
	else {
		mode = SP5K_MEDIA_REC_DIS_OFF;
	}
	
	if (mode != SP5K_MEDIA_REC_DIS_OFF) {
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL, mode);
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
	}
	else {
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL, mode);
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
	}
	/* Add by SC.Liu */
}

/*-------------------------------------------------------------------------
 *  Function Name : appVideo_SetParkingMode
 *  Description :   when parking Mode,set 1 fps video recording para before video record
 *------------------------------------------------------------------------*/
void appVideo_SetParkingMode(UINT8 parkingMode)
{
	if(parkingMode)/*cfg 1 fps video recording*/
	{
		//sp5kMediaRecCfgSet(SP5K_MEDIA_REC_H264_PFRAME_SKIP, 1);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE,SP5K_MEDIA_GOP_IPPP);
		printf("\033[35mprking mode on\033[0m\n");
	}
	else /*recover  normal 30 fps video recording*/
	{
		//sp5kMediaRecCfgSet(SP5K_MEDIA_REC_H264_PFRAME_SKIP, 0);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE,SP5K_MEDIA_GOP_IBBP);
		printf("\033[35mprking mode off\033[0m\n");
	}
}

UINT32 
appVideoSizeGet(
	UINT8 size_idx, 
	UINT32 *pW, 
	UINT32 *pH
)
{
	HOST_ASSERT(size_idx<UI_VID_SIZE_MAX);
	if (pW)
	{
		*pW = VideoSize[size_idx].w;
	}
	if (pH) 
	{
		*pH = VideoSize[size_idx].h;
	}
	return SUCCESS;
}

UINT8 
appVideoSizeIconGet(
	UINT8 size_idx
)
{
	return VideoSize[size_idx].icon;
}

UINT8
appVidSenModeGet(
	UINT16 *sensormode
)
{

    if(gVideoSizeTag>=UI_VID_SIZE_MAX)
        return FAIL;

   	if (sensormode)
	{
		*sensormode = VideoSize[gVideoSizeTag].sensormode;
	}
    else
        return FAIL;

    return SUCCESS;
}

UINT8 
appVideoSizeIdxGet(
	UINT32 w, 
	UINT32 h
)
{
	int i;
	for (i=0 ; i<UI_VID_SIZE_MAX ; i++) 
	{
		if (VideoSize[i].w==w && VideoSize[i].h==h)
		{
			return i;
		}
	}
	return UI_VID_SIZE_MAX;
}

appVideoSize_t * 
appVideoSizeParaGet(
	UINT32 idx
)
{
	if(UI_VID_SIZE_MAX > idx)
	{
		return &VideoSize[idx];
	}
	return NULL;
}
UINT32
appVidFrameRateGet(
	UINT8 size_idx
)
{
	return VideoSize[size_idx].frameRate;
}

#if 0	/* modify by SC.LIU */
/*-------------------------------------------------------------------------
 *  Function Name : appDiskSpaceGet
 *  Description :   
 *------------------------------------------------------------------------*/

UINT32 appDiskSpaceGet(
	void
)
{
	appDiskInfo_t* pDisk;
	UINT32 space=0;
	
	pDisk = appCurDiskInfoGet(); 
	space = (pDisk->freeSz - APP_DISK_DPOF_RSVD_SIZE_KB - AVI_HDR_KB);
	if(space < (appVideoFileSizePerSecGet(pViewParam->vidsize) + appCaptureInMovieCountGet(pViewParam->vidsize, pUiSetting->videoFormat, 8)))
	{
		noCapture=FALSE;
		return 0;	
	}
	
	noCapture=TRUE;
	return 1;
}
#endif

UINT32 
appVideoFileSizePerSecGet(
	UINT8 size_idx
)
{
	return gVideoCB.aviKbPerSec;	
}


UINT32 
appVideoRemainSecGet(
	void
)
{
	appDiskInfo_t* pDisk;
	UINT32 n=0;
	int space=0;
	uiPara_t* puiPara = appUiParaGet();
	UINT32 ret;
	
	ret = appVideoFileSizePerSecGet(puiPara->VideoSize); /*add for mantis bug 0048963*/
	if(ret == 0)
	{
		return FAIL;
	}
	
	pDisk = appCurDiskInfoGet(); 
	space = (pDisk->freeSz - APP_DISK_DPOF_RSVD_SIZE_KB - AVI_HDR_KB - APP_CAPTURE_IN_MOVIE - APP_THRESHOLD_SPACE);

	if(space>0) 
	{
		n = space / appVideoFileSizePerSecGet(puiPara->VideoSize);
		n = n>1? (n-1) : 0; 
	}	
	if(n == 0)
	{
		gVideoCB.err = VIEW_ERR_MEMORY_FULL;
	}		
	
	printf("space = %s%d kb, n = %d sec", space>0? "+" : "-", space>0?  space : -space, n);
	
	return n;
}


void AppVideoSeamlessNextFileNameGet(UINT8* pFname)
{
	UINT32 file_type;	

	file_type = mediaRecAttrGet(MEDIA_ATTR_FILE_TYPE);
	
	if (dcfFsNextFileNameGet(file_type, pFname)) 
	{
		DBG_PRINT("Fail file_type=%x\n",file_type);
		return ;
	}
}

void App_videoGPSDataSave(void)
{
	//For demo
	
	
	return ;
	UINT32 i ,len;		
	UINT8 fname[32], tmpBuff[32] ;
	UINT8 testchar[1000];		
	UINT32 totalFile;
	UINT32 currMode;

	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);	
	sp5kModeGet(&currMode);
			
	memset(&testchar ,0x0, 1000);	
	memset(&tmpBuff ,0x0, 32);
	for(i=0 ;i<=1000 ; i++)
	{
		 testchar[i]='T';
	}
	
	/* Get current file index  and set the TXT file */
		
	AppVideoSeamlessNextFileNameGet(fname);
		
		
	strtok(fname ,".");	
	len = strlen(fname );
	strncpy(tmpBuff,fname,len);
	strcat(tmpBuff, ".TXT");
	WriteFile(tmpBuff , (UINT8*)testchar,1000);

}


void AppVideoRecFirstIndexFileNameGet(UINT8* pMediafilename ,UINT8* pTXTfilename)
{
	sp5kDcfAttrElm_t pdcfAttr;
	UINT32 len;
	UINT8 fname[32];
	UINT32 totalFile;
	
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
	if(totalFile == 0)
	{
		AppVideoSeamlessNextFileNameGet(pMediafilename);
	}
	
	else if (FAIL==sp5kDcfFsFileAttrGet(1, &pdcfAttr, pMediafilename) )
	{
		DBG_PRINT("DCF Get file name error! \n");
		return ;
	}
	memset(pTXTfilename ,0x0, 32);
	strcpy(fname, pMediafilename);
	strtok(fname ,".");	
	len = strlen(fname );
	strncpy(pTXTfilename, fname, len);

	strcat(pTXTfilename,".TXT");
	
	printf("First DCF =%s TEX file=%s   %d\n",pMediafilename,pTXTfilename,len);
	

}

void AppVideoSeamlessSetNextDCFKey(void)
{
#if 0 /*20120613 juling-- follow same dcf rule*/
	UINT32 yearM,hourM,ret ,i,index;
	struct tm rtc;
	UINT8 fileChar[6] ,fname[32];
	UINT32 currMode;
    
	appCalibData_t *pCalib;
	pCalib = appCalibDataGet();
	
	sp5kModeGet(&currMode);

	if(pCalib->aedebug == TRUE)
	{
		if((currMode==SP5K_MODE_VIDEO_RECORD) &&(pViewParam->videoStamp))
		{	
			appVideoLogFileSave();
			appVideoLogFileInit();	
		
		}
	}
	
	sp5kDcfFsSortModeSet(SP5K_DCF_SORT_BY_DATE);	
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc);	/*	get current RTC time */
	hourM = (rtc.tm_hour*100 )+ rtc.tm_min ;
	yearM = (rtc.tm_mon*100 )+ rtc.tm_mday ;
	sprintf(fileChar ,"%.4d" , yearM);		
	ret=sp5kDcfFsFreeCharSet("MEDIA" ,&fileChar, SP5K_DCF_NEXTKEY_MAX);
	sp5kDcfFsNextKeySet(100 ,hourM);
#endif
}
void appAWBALG_WBParamSet(UINT32 selector,UINT32 value);

//lester for ae debug
#define VIDEO_LOG 5000
char videoDebugBuff[VIDEO_LOG];
UINT8 fname[32];
UINT16 videoLogCount =0;
static UINT16 wcount=0;

void AppVideoSeamlessCallback(void)
{
	appCalibData_t *pCalib;
	uiPara_t* puiPara = appUiParaGet();
	pCalib = appCalibDataGet();
	if(pCalib->aedebug == TRUE){

	}

	if(pCalib->awbdebug == TRUE){

	}
	appExif_SetAviChunk(); //GQ add ,fix  mantis 45349

	#if SP5K_CDFS_OPT
	profLogAdd(0,"VideoSeamlessDelFile S");
	if(puiPara->Seamless!= UI_VIDEO_SEAMLESS_OFF)
	{
		profLogAdd(0,"appCdfsFreeSpaceCheck S");
		if(appCdfsFreeSpaceCheck(CDFS_FILE_FOLDER_VIDEO, TRUE) == FAIL)
		{
			if(gVideoCB.CapSec>=2)
			{
				__FUNC_TRACK__;
				appVideoStop();
				gVideoCB.err = VIEW_ERR_MEMORY_FULL;
			}
		}
		profLogAdd(0,"appCdfsFreeSpaceCheck E");
	}
	profLogAdd(0,"VideoSeamlessDelFile E");

	#if !ICAT_WIFI
	if(appCdfsAddFileStsGet())
	{
		appCdfsFileAdd(appCdfsLastAviNameGet());
	}	
	#else
	{
		UINT8 pMediafilename[32]={'\0'};	

		sprintf(pMediafilename,"%s",appCdfsPreFileNameGet());
		pMediafilename[2] = '\\';
		pMediafilename[8] = '\\';

		appStill_PIMA_Send_Event(PTP_EC_OBJECT_ADDED, (UINT32)pMediafilename, 0, 0);
	}
	#endif
	#else
	{
		#if ICAT_WIFI
		UINT32 totalFile;
		UINT8 pMediafilename[32];
		sp5kDcfAttrElm_t pdcfAttr;

		if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE){
			sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
			sp5kDcfFsFileAttrGet(totalFile, &pdcfAttr, pMediafilename);
		
			appStill_PIMA_Send_Event(PTP_EC_OBJECT_ADDED, (UINT32)pMediafilename, 0, 0);
		}
		#endif
	}
	#endif
	
	profLogAdd(0,"appCdfsFileAdd E");
	profLogAdd(0,"AppVideoSeamlessCallback E");
 
}
void appVideoLogFileInit(void)
{
   memset(videoDebugBuff, 0, VIDEO_LOG);
   videoLogCount =0	;
}

void appVideoLogFile_Print(const char *format, ...)
{
 	char buf[1024];
	UINT8 tempCount =0 ;
	va_list	ap;
    UINT32 currMode;

	sp5kModeGet(&currMode);
	if(currMode != SP5K_MODE_VIDEO_RECORD)
		return ;
	if(videoLogCount > VIDEO_LOG )
	{
		printf("<lester>over video log size\n");
		return ;
	}
	memset(buf, 0, 1024);
	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	tempCount = strlen(buf);
	memcpy(&videoDebugBuff[videoLogCount], buf, tempCount);
	videoLogCount += tempCount ;

	if((videoLogCount+100) > VIDEO_LOG )
	{
		appVideoLogFileSave();
		appVideoLogFileInit();
		wcount++;
	}
	//printf("%s", buf);
}

void appVideoLogFileSave(void)
{
   UINT32 totalFile;
	char log[32];

	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);	
	
	/* Get current file index  and set the TXT file */		
	AppVideoSeamlessNextFileNameGet(fname);		
		
	strtok(fname ,".");	
	sprintf(log, "%s_%d.TXT",fname, wcount);
	WriteFile(log , (UINT8*)videoDebugBuff,VIDEO_LOG);	
	//printf("<lester>videoLogCount %s %s %d\n",fname ,log ,wcount);
}

void 
cmdVideo(
	UINT32 argc,
	UINT8 *arg[], 
	UINT32 v[]
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	if (argc==1) 
	{
		printf("video usage:\n"
			" video size [0|1|2] 		: 640/169/320\n"
			" video metering [0|1|2] 	: ??\n"
			" video wb[0|1|2] 			: ??\n"
		);
	} 
	else if (strcmp(arg[1], "size")==0) 
	{
		appVidSizeSet(v[2]);
	} 
	else if (strcmp(arg[1], "metering")==0)
	{
		puiPara->AEMetering = v[2];
	} 
	else if (strcmp(arg[1], "wb")==0) 
	{
		puiPara->WBMode = v[2];
	}	
}

