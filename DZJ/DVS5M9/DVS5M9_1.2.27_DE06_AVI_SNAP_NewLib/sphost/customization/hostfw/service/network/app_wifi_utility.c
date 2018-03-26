/**************************************************************************
 *
 *       Copyright (c) 2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ndk_types.h>
#include <ndk_global_api.h>
#include <ndk_getopt.h>
#include <ndk_streaming.h>
#include <ctype.h>
#include <api/cmd.h>
#include <sp5k_dbg_api.h>
#include <sp5k_global_api.h>
#include <api/sp5k_fs_api.h>
#include <sp5k_pb_api.h>
#include <proflog.h>
#include <app_timer.h>
#include <app_wifi_utility.h>
#include <app_osd_api.h>
#include <gpio_custom.h>
#include "sp5k_media_api.h"
#include "app_view_param.h"
#include "app_view_param_def.h"
#include "sp5k_modesw_api.h"
#include "app_still.h"
#include "app_video.h"
#include "app_ui_para.h"
#include <app_zoom_api.h>
#include "app_cdfs_api.h"
#include "App_scdv_ui_para_def.h"
#include "app_wifi_connection.h"
#include "sp5k_disk_api.h"
#include "app_ptp.h"
#include "sp5k_utility_api.h"
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 gssidname[MAX_SSID_NAME_LEN]={0};
UINT8 gpassword[MAX_PASSWORD_LEN]={0};
static UINT8 ghapMode = 0;
static UINT8 g_init_state = 0;
/*static UINT32 ndktimer = TIMER_NULL;*/
extern void ndk_msleep(ULONG millisecs);
extern void usbPIMADriveNodeFlushAll(UINT32 firstFile);/*xuan.ruan@20131113 add for mantis bug 0048877*/

extern void appSdvStillSettingInit(void);
extern void appSdvVideoSettingInit(void);
extern void appCdvVideoSettingInit(void);

#ifdef ICAT_STREAMING
static NetStParams st_params;
static BOOL bPtpStatus = 0;
static BOOL bRtpStatus = 0;
#endif

/**************************************************************************
 *                         F U N C T I O N     D E F I N I T I O N                         *
 **************************************************************************/
extern void callStack(void);
static void appmediasrcPBVideo(void);

int appNdkNetDrvInit(
	const char* drvname
)
{
	if (!drvname) {
		printf("Need driver name\n");
		return -1;
	}
	else if (!strcmp(drvname, "rtk412") || !strcmp(drvname, "rtk") || !strcmp(drvname, "rtk413")) {
		printf("Loading RTK driver\n");
		return ndk_netdrv_init(drvname)/*wdrv_init_rtk_v412()*/;/*!< use Realtek WiFi 413 */
	}
	else {
		printf("Unknow driver\n");
		return -1;
	}
}

void ptpipConnectcb(UINT32 sessnum)
{
	printf("[PTP-IP] session num:%d\n",sessnum);
	profLogPrintf(0,"[PTP-IP]session num:%d",sessnum);
	if(!sessnum){

	}else{
		usbPIMADriveNodeFlushAll(1);/*xuan.ruan@20131113 add for mantis bug 0048877*/
	}
}

void appRtpUpdateMediaAttrs(
	NetStParams *sp,UINT8 vidsize
)
{
	sp->jpeg_width = ALIGN_TO(sp->jpeg_width, 8);
	if (sp->jpeg_width > 640) {
		sp->jpeg_width = 640;
	}

	sp->jpeg_height = ALIGN_TO(sp->jpeg_height, 8);
	if (sp->jpeg_height > 360) {
		sp->jpeg_height = 360;
	}
	
	#if 0
	if (sp->jpeg_q_factor > RTP_MJPG_Q_FINE)
		sp->jpeg_q_factor = RTP_MJPG_Q_FINE;
	else if (sp->jpeg_q_factor < RTP_MJPG_Q_ECONOMY)
		sp->jpeg_q_factor = RTP_MJPG_Q_ECONOMY;
	#else /* MJPG quality issue */
	sp->jpeg_q_factor = 80;
	#endif

	if (sp->jpeg_bitrate > RTP_MJPG_BR_FINE)
		sp->jpeg_bitrate = RTP_MJPG_BR_FINE;
	else if (sp->jpeg_bitrate < RTP_MJPG_BR_ECONOMY)
		sp->jpeg_bitrate = RTP_MJPG_BR_ECONOMY;

	appRtpResetMediaAttrs();

	#if 0
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_OPTION, 0);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE, SP5K_MEDIA_CBR);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MUTE_PERIOD, 100);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, sp->file_type);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 44100);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 2);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, 31);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MUTE, 0);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_HB, 500);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_LB, 100);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT, 60);//baiqi set seamless 60s(1min)
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MODE, SP5K_MEDIA_REC_ALC_DRC_MODE);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_ON);

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, sp->h264_width);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, sp->h264_height);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, sp->h264_bitrate);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, sp->h264_frmrate);
	sp5kMediaRecAttrSet(MEDIA_ATTR_H264_GOP_STRUCTURE, 0x10);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);

	if(vidsize == UI_VID_SIZE_FHD)
	{
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE, (2 * 1024 * 1024) );
  		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT, 12);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO ,192);
	    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO ,192);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO, 100);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO, 64);
		sp5kAwbCfgSet( SP5K_AWB_ACCUM_PERIOD, 2);
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 1);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_1080P);
		appAeFrameRateSet(30);
	}else if(vidsize==UI_VID_SIZE_HD_60FPS){
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE, (1300 * 1024) );
  		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT, 27);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO ,256);
	    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO ,256);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO, 100);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO, 64);
		sp5kAwbCfgSet( SP5K_AWB_ACCUM_PERIOD, 2 );
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 2);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_720P_60FPS);
		appAeFrameRateSet(60);
	}
	else if(vidsize == UI_VID_SIZE_HD_30FPS)
	{
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE, (1300 * 1024) );
  		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT, 27);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO ,256);
	    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO ,256);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO, 100);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO, 64);
		sp5kAwbCfgSet( SP5K_AWB_ACCUM_PERIOD, 2 );
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 1);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_720P_30FPS);
		appAeFrameRateSet(30);
	}
	else
	{
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE, (600 * 1024) );
  		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT, 50);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO ,256);
	    	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO ,256);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_MSGQ_NO, 100);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_MSGQ_NO, 64);
		sp5kAwbCfgSet( SP5K_AWB_ACCUM_PERIOD, 4 );
		sp5kAeCfgSet(SP5K_AE_ACCUM_PERIOD, 4);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_VGA_120FPS);
		appAeFrameRateSet(120);
	}
	#else
	if(appCheckSdvMainMode())
	{
		//only support SP5K_MODE_VIDEO_PREVIEW mode in wifi
		/*if(app_PTP_Get_DscOpMode(MODE_ACTIVE) == PTP_DSCOP_MODE_CAMERA)
		{
			appSdvStillSettingInit();
		}
		else if(app_PTP_Get_DscOpMode(MODE_ACTIVE) == PTP_DSCOP_MODE_VIDEO_OFF)*/
		{
			appSdvVideoSettingInit();
		}
	}
	else
	{
		appCdvVideoSettingInit();
	}	
	#endif
	
	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
	if (0) {
		UINT32 w, h;
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);
		MYINFO("width = %d, height = %d\n", w, h);
	}
}

void appRtpResetParams(
	NetStParams *sp,UINT8 vidsize
)
{
	UINT32 w,h;

	appVideoSizeGet(vidsize, &w, &h);
	
	sp->file_type     = SP5K_MEDIA_MOV;
	sp->h264_width   = w;
	sp->h264_height  = h;

	switch(vidsize)
	{
	 	case UI_VID_SIZE_FHD:
			sp->h264_bitrate  = 12000000;
			sp->h264_frmrate  = 30;
			break;
		case	UI_VID_SIZE_HD_60FPS:
			sp->h264_bitrate  = 10000000;
			sp->h264_frmrate  = 60;
			break;
		case	UI_VID_SIZE_HD_30FPS:
			sp->h264_bitrate  = 8000000;
			sp->h264_frmrate  = 30;
			break;			
		case	UI_VID_SIZE_VGA:
			sp->h264_bitrate  = 6000000;
			sp->h264_frmrate  = 120;
			break;
		default:
			sp->h264_width    = 1920;
			sp->h264_height   = 1080;
			sp->h264_bitrate  = 12000000;
			sp->h264_frmrate  = 30;
			break;
	}

	sp->jpeg_width    = 640;
	sp->jpeg_height   = 360;
	#if 0
	sp->jpeg_q_factor = 40;
	#else /* MJPG quality issue */
	sp->jpeg_q_factor = 80;
	#endif	
	sp->jpeg_bitrate  = 4500000;
	
	printf("[RTP][ParamReset]H(%d,%d,%d,%d)M(%d,%d,%d,%d)\n",
		sp->h264_width ,sp->h264_height,sp->h264_bitrate,sp->h264_frmrate,
		sp->jpeg_width,sp->jpeg_height ,sp->jpeg_q_factor,sp->jpeg_bitrate);	
}

static BOOL bIsRtpOptAllow = TRUE;

void appRtpStartStreaming(
	void
)
{
	NetStParams *sp = &st_params;
	uiPara_t* puiPara = appUiParaGet();

	UINT32 curMode;

	if(!bIsRtpOptAllow)
	{
		printf("%s,re-entry !!\n",__FUNCTION__);
		callStack();
		return;
	}

	printf(">>> appRtpStartStreaming\n");

	/*bIsRtpOptAllow = FALSE;*/
	
	sp5kModeGet(&curMode);

	profLogPrintf(0, "[CapPerform]appRtpStartStreaming +");

#if 0 /* for Android Camera */
	appDispLayer_IMG_Ctrl(0);
	appDispLayer_OSD_Ctrl(0);
#endif
	if(!app_PTP_ContRecMode_Get())
		HOST_ASSERT(curMode != SP5K_MODE_VIDEO_RECORD);

	printf("PTPContRecMode:%d\n",app_PTP_ContRecMode_Get());
	profLogPrintf(1,"PTPContRecMode:%d",app_PTP_ContRecMode_Get());
	printf("cur mode:0x%x\n",curMode);

	appRtpStreamStateSet(1); /* 4/11 apk won't apply op mode on entering, then the streaming state is still OFF */
	
	if(app_PTP_ContRecMode_Get()&&curMode==SP5K_MODE_VIDEO_RECORD){
		printf("DSC is doing Video Recording now!\n");		
		profLogAdd(1,"DSC is doing Video Recording now!");
	}
	else
	{
	
		if (curMode != SP5K_MODE_VIDEO_PREVIEW) 
		{
			//appStateChange(APP_STATE_VIDEO_PREVIEW,  STATE_PARAM_NORMAL_INIT);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_CAPTURE_FRAMES, 0);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL, 0);
		}

		#if FIX_SENSOR_MODE_4_CAM_MODE
		/* 47282 / 47614 : a fixed sensor mode for capture mode : s */
		if(app_PTP_Get_DscOpMode(MODE_ACTIVE)== PTP_DSCOP_MODE_CAMERA){
			appMediaAttrUpdate(UI_VID_SIZE_FHD);
		}	
		else
		#endif /* 47282 / 47614 : a fixed sensor mode for capture mode : e */
		{
			appMediaAttrUpdate(puiPara->VideoSize);
		}

		//for DVS5M2 still screen abnormal -s
		if(curMode == SP5K_MODE_STILL_PREVIEW)
		{
			appOsDelayMs(300);
		}
		//for DVS5M2 still screen abnormal -e
	}

	appRtpResetParams(sp,puiPara->VideoSize); /* mjpg attr will be reset by appUrlParseJpegAttrs(), so reset again here !! */
	profLogPrintf(0,"mjpg(%d,%d,%d,%d)",sp->jpeg_width,sp->jpeg_height,sp->jpeg_bitrate,sp->jpeg_q_factor);
	if (ndk_st_dualstream_start_brc(sp->jpeg_width, sp->jpeg_height, sp->jpeg_q_factor,30, sp->jpeg_bitrate) != 0) {

	/*if (ndk_st_dualstream_start(sp->jpeg_width, sp->jpeg_height, sp->jpeg_q_factor, 30) != 0) {*/
		profLogPrintf(0, "[CapPerform]appRtpStartStreaming error");
		return;
	}
	profLogPrintf(0, "[CapPerform]appRtpStartStreaming -");
	/*bIsRtpOptAllow = TRUE;*/
}

void appRtpStopStreaming(
	void
)
{
	UINT32 curMode;

	if(!bIsRtpOptAllow)
	{
		printf("%s,re-entry !!\n",__FUNCTION__);
		callStack();
		return;
	}

	printf("<<< appRtpStopStreaming\n");

	/*bIsRtpOptAllow = FALSE;*/
	
	profLogPrintf(0, "[CapPerform]appRtpStopStreaming +");
	sp5kModeGet(&curMode);

	while(curMode <  SP5K_MODE_VIDEO_PREVIEW)
	{
		sp5kModeGet(&curMode);
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
		printf("\n Wait for video preview mode ready!!\n");
	}
		
	HOST_ASSERT(curMode == SP5K_MODE_VIDEO_PREVIEW || curMode == SP5K_MODE_VIDEO_RECORD);

	#if 0 /* for Android Camera */
	appDispLayer_IMG_Ctrl(1);
	appDispLayer_OSD_Ctrl(1);
	#endif

	if (curMode == SP5K_MODE_VIDEO_RECORD) {
		// Stop record first
		printf("PTPContRecMode:%d\n",app_PTP_ContRecMode_Get());
		profLogPrintf(1,"PTPContRecMode:%d",app_PTP_ContRecMode_Get());
		if(app_PTP_ContRecMode_Get()) 
		{
			printf("___Keeping in video record mode___!!\n");			
			/*appVideoOsdShow(1,1,gVideoCB.osd);*/
		}
		else
		{
			printf("Video Record is Stopping!!\n");
			appVideoStop();
		}
	}

	ndk_st_dualstream_stop();

	profLogPrintf(0, "[CapPerform]appRtpStopStreaming -");
	/*bIsRtpOptAllow = TRUE;*/
}

void appInitStateSet(UINT8 state)
{
	g_init_state |= state;
}

void appInitStateUnset(UINT8 state)
{
	g_init_state &= ~state;
}

void appNetStateReset(UINT8 wifiParm)
{
	if(appPtpStatGet()&&((wifiParm & PTP_IP)== PTP_IP))
		appPtpStateSet(0);
	if(appRtpStreamStateGet()&&((wifiParm & RTP_STREM)== RTP_STREM))
		appRtpStreamStateSet(0);
	/*appTimerClear(&ndktimer);*/
}

UINT8 appInitStateGet()
{
	return g_init_state;
}

static void appRtpResetMediaAttrs(
	void
)
{
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_RTP_STREAMING_EN, 0);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DISABLE_STORAGE, 0);
}

static void appVideoSizeReset(
	NetStParams *stparam,
	UINT32 w,
	UINT32 h
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	if(puiPara->VideoSize== UI_VID_SIZE_FHD){
		stparam->h264_width   = w;
		stparam->h264_height  = h;
		stparam->h264_bitrate = 12000000;
		stparam->h264_frmrate = 30;
	}else if(puiPara->VideoSize == UI_VID_SIZE_HD_60FPS){
		stparam->h264_width	  = w;
		stparam->h264_height  = h;
		stparam->h264_bitrate = 10000000;
		stparam->h264_frmrate = 60;
		}else if(puiPara->VideoSize == UI_VID_SIZE_HD_30FPS){
			stparam->h264_width	  = w;
			stparam->h264_height  = h;
			stparam->h264_bitrate = 8000000;
			stparam->h264_frmrate = 30;
		}
		else{
			stparam->h264_width	  = w;
			stparam->h264_height  = h;
			stparam->h264_bitrate = 6000000;
			stparam->h264_frmrate = 120;
		}
		stparam->jpeg_height  = 360;
}

static BOOL appUrlStrNoCaseCmp(
	const char *src,
	const char *dst,
	size_t len
)
{
	while (*src && *dst && len > 0) {
		if (toupper(*src) != toupper(*dst)) {
			return FALSE;
		}

		++src;
		++dst;
		--len;
	}

	return len == 0 ? 1 : 0;
}

static BOOL appUrlGetNextElem(
	char **url_,
	char leading_char,
	char **name,
	int *len
)
{
	char *url = *url_;
	if (*url == 0) {
		return FALSE;
	}

	if ((leading_char && *url != leading_char) || !strchr("/?=&", *url)) {
		return FALSE;
	}

	char *p = url + 1;
	while (*p) {
		if (strchr("/?=&", *p)) {
			break;
		}
		++p;
	}
	*name = url + 1;
	*len = (int)(p - url - 1);
	*url_ = p;

	return TRUE;
}

static BOOL appUrlParseAttrs(struct UrlAttrDef *urlAttrs, char *url
	, BOOL (*urlAttrHandler)(int id, char* val))
{
	if (!url || *url == 0)
		return TRUE;

	if (*url != '?') // must start with ?
		return FALSE;

	struct UrlAttrDef *pAttr;
	char *str;
	int  len;

	while (1) {
		if (!appUrlGetNextElem(&url, 0, &str, &len))
			return FALSE;

		for (pAttr = urlAttrs; pAttr->name != NULL; ++pAttr) {
			if (appUrlStrNoCaseCmp(pAttr->name, str, len)) {
				if (!appUrlGetNextElem(&url, '=', &str, &len))
					return FALSE;
				char c = str[len];
				str[len] = 0;
				BOOL r = urlAttrHandler(pAttr->id, str);
				str[len] = c;
				if (!r)
					return FALSE;
				break;
			}
		}

		if (pAttr->name == NULL) // Unknow attributes
			return FALSE;

		if (*url == 0) // finished
			break;

		if (*url != '&') // name=value pair must start with character '&' except the first one.
			return FALSE;
	}

	return TRUE;
}

static BOOL appUrlMjpgAttrHandler(
	int id,
	char* val
)
{
	NetStParams *sp = &st_params;

	switch (id) {
	case URL_MJPG_WIDTH:
		sp->jpeg_width = strtoul(val, NULL, 10);
		break;

	case URL_MJPG_HEIGHT:
		sp->jpeg_height = strtoul(val, NULL, 10);
		break;

	case URL_MJPG_Q_FACTOR:
		if (!strcasecmp(val, "FINE"))
			sp->jpeg_q_factor = RTP_MJPG_Q_FINE;
		else if (!strcasecmp(val, "NORMAL"))
			sp->jpeg_q_factor = RTP_MJPG_Q_NORMAL;
		else if (!strcasecmp(val, "ECONOMY"))
			sp->jpeg_q_factor = RTP_MJPG_Q_ECONOMY;
		else
			sp->jpeg_q_factor = strtoul(val, NULL, 10);
		break;
	case URL_MJPG_BITRATE:
		if (!strcasecmp(val, "FINE"))
			sp->jpeg_bitrate = RTP_MJPG_BR_FINE;
		else if (!strcasecmp(val, "NORMAL"))
			sp->jpeg_bitrate = RTP_MJPG_BR_NORMAL;
		else if (!strcasecmp(val, "ECONOMY"))
			sp->jpeg_bitrate = RTP_MJPG_BR_ECONOMY;
		else
			sp->jpeg_bitrate = strtoul(val, NULL, 10);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

static BOOL appUrlParseJpegAttrs(
	char *url
)
{
	HOST_ASSERT(url);

	NetStParams params_bak;
	memcpy(&params_bak, &st_params, sizeof(params_bak));

	if (!appUrlParseAttrs(mjpgUrlAttrs, url, appUrlMjpgAttrHandler))
		return FALSE;

	if (memcmp(&st_params, &params_bak, sizeof(params_bak)) != 0) {
		// changed
	}

	return TRUE;
}

static BOOL appRtpEventHandler(
	UINT32 event,
	UINT32 data
)
{

	switch (event) {
	case NDK_ST_EVT_RTSP_REQUEST: {
		char *url = (char *)data;
		MYINFO("URL = '%s'\n", url);

		if (appUrlStrNoCaseCmp(url, "MJPG", 4)) {
			return appUrlParseJpegAttrs(url + 4);
		}
		else {
			MYINFO("Stream not found\n");
			return TRUE;
		}
		break; }

	case NDK_ST_EVT_ON_STOPPED: {
		profLogPrintf(0,"[CapPerform]NDK_ST_EVT_ON_STOPPED mode=%d", ndk_st_get_mode());
		sp5kHostMsgSend(APP_UI_MSG_RTP_STOP,0);
		return TRUE; }

	case NDK_ST_EVT_ON_STARTED: {
		profLogPrintf(0,"[CapPerform]NDK_ST_EVT_ON_STARTED mode=%d", ndk_st_get_mode());
		sp5kHostMsgSend(APP_UI_MSG_RTP_START,0);
		return TRUE;}

	case NDK_ST_EVT_FRAME_DROPPED: {
		profLogPrintf(0, "drop frame %u", data);
		return TRUE; }
	}

	return FALSE;
}

BOOL appWiFiInit(
	BOOL bMptoolEn,
	const char *drvname
)
{
	/* Control MPtool behavior */
	#if NDK_MP_TOOL_EN
	ndk_netdrv_mpset(1); /* NOTE: NDK rev must be latter than 15995 */
	#endif
	if (appNdkNetDrvInit(drvname) != 0) { /* Loading Realtek WiFi driver */
		HOST_ASSERT(0);
		return -1;
	}
	
	ndk_netif_ioctl(NDK_IOCS_IF_UP, (long)"wlan0", NULL);
	if(ndk_netif_set_address_s("wlan0", "192.168.0.1", "255.255.255.0", "192.168.0.1")!=0){
		printf("Net address set failed!\n");
		return -1;
	}

	ndk_netif_ioctl(NDK_IOCS_WIFI_COUNTRY_US, (long)"wlan0", NULL);

	printf("Loading WiFi module succeed\n");
	appInitStateSet(WIFI_LOAD);
	return 0; /* succeed */
}

static BOOL appWiFiSuspend(
	void
)
{
	ndk_netif_ioctl(NDK_IOCS_WIFI_SUSPEND, (long)"wlan0", NULL);
	return 0;
}

static BOOL appWiFiResume(
	void
)
{
	ndk_netif_ioctl(NDK_IOCS_WIFI_RESUME, (long)"wlan0", NULL);
	return 0;
}

static void appDhcpServerStart(
	void
)
{
	struct in_addr local, start, netmask;
	//printf("DHCP server start....\n");
	inet_aton("192.168.1.1", &local);
	inet_aton("192.168.1.10", &start);
	inet_aton("255.255.255.0", &netmask);

	ndk_netif_set_address("wlan0", &local, &netmask, &local);
	ndk_dhcpd_start("wlan0", &start, 10);
	appInitStateSet(DHCP_SERVER);
}

static void appDhcpServerStop(
	void
)
{
	UINT8 state = appInitStateGet();

	if (state & DHCP_SERVER) {
		ndk_dhcpd_stop();
		appInitStateUnset(DHCP_SERVER);
	}
}

static void appHostAPStart(
	const char* cfg_file,
	const char* ssid_name,
	const char* passphrase,
	const char* channel_scan
)
{
	printf("Host AP start(%s,%s)\n",cfg_file,ssid_name);
	if(passphrase)
		printf("pwd:%s\n",passphrase);
	NDKHapdOpt opts[] = {
		{"ssid", ssid_name},
		{"wpa_passphrase", passphrase},
		{"channel", channel_scan},
		{"wpa", "2"},
		{NULL, NULL}
	};
	ndk_hapd_start(cfg_file, opts);
	appInitStateSet(HOST_AP);
}

static void appHostAPStop(
	void
)
{
	UINT8 state;

	state = appInitStateGet();

	if (state & HOST_AP) {
		ndk_hapd_stop();
		appInitStateUnset(HOST_AP);
	}
}

static void appPtpIpStart(
	void
)
{
	extern void ptpip_reg_n_start(void);
	extern void ptpip_timeout_set(UINT32 timeout_sec);
	extern void ptpip_connection_state_cb_reg(void (*cb)(UINT32));
	printf("ptp-ip start....\n");
	ptpip_timeout_set(PTPIP_ALIVE_TIME);
	ptpip_reg_n_start();
	appPtpStateSet(1);
	ptpip_connection_state_cb_reg(ptpipConnectcb);
	appInitStateSet(PTP_IP);
}

static void appPtpIpStop(
	void
)
{
	UINT8 state = appInitStateGet();

	extern void ptpip_unreg_n_stop(void);
	if (state & PTP_IP) {
		ptpip_unreg_n_stop();
		appInitStateUnset(PTP_IP);
	}
}

#ifdef ICAT_STREAMING
static void appMediaSrvStart(UINT32 w, UINT32 h)
{
	extern void cmdProcess(char *);	cmdProcess((char*)"os nice Cmd 31");
	extern void sioCtrl(UINT32); sioCtrl(3);
	uiPara_t* puiPara = appUiParaGet();

	NetStParams *sp = &st_params;
	const char *root_dir = "D:/";

	appMediaAttrUpdate(puiPara->VideoSize);

	sp->file_type  = SP5K_MEDIA_MOV;
	sp->jpeg_width = 640;
	sp->jpeg_height= 360;

	{
		struct NDKStCfg cfg;
		cfg.root_dir = root_dir;
		cfg.port = 554;
		cfg.st_mode = NDK_ST_DUAL_STREAMING;
		cfg.audio_on = 0;
		cfg.keepalive_secs = RTP_ALIVE_TIME;
		cfg.evt_handler = appRtpEventHandler;

		if (ndk_st_start_server(&cfg) != 0) {
			return;
		}
	}
	printf(">>> start media streaming server...........\n");
	profLogPrintf(0,"media streaming server is started");
	appInitStateSet(RTP_STREM);
}

static void appMediaSrvStop(
	void
)
{
	UINT8 state = appInitStateGet();

	if (state & RTP_STREM) {
		ndk_st_stop_server();
		appInitStateUnset(RTP_STREM);
	}
}

static void appFtpSrvStart(
	void
)
{
	UINT8 *drv;
	drv = sp5kMalloc(32*sizeof(UINT8));
	sprintf(drv,"%s",appActiveDiskGet()==2 ? "D:/":"C:/");
	ndk_ftpd_start(drv, 21);
	sp5kFree(drv);
	appInitStateSet(FTP_SRV);
}


static void appFtpSrvStop(
	void
)
{
	ndk_ftpd_stop();
	appInitStateUnset(FTP_SRV);
}

static void appStreamClose(
	void
)
{
	printf("<<< ndk streaming closed...........\n");
	ndk_st_close_streaming();
	appRtpStreamStateSet(0);
}
#endif
char buffer0[2*1024];
static BOOL appSSIDNameGet(
	UINT8 *ssidname
)
{
	UINT32 fd;
	UINT32 Len,i,j,size=0;
	UINT8  buffer[MAX_SSID_NAME_LEN+MAX_PASSWORD_LEN]; 
	UINT8  ssidbak[MAX_SSID_NAME_LEN];
	UINT   slen=0;
	/* ssid name cfg file for ssid name save assigned by user */
	fd = sp5kFsFileOpen("B:/UDF/SSID_PW.CFG",SP5K_FS_OPEN_RDONLY|SP5K_FS_OPEN_RDWR);
	#if 1
	UINT32 fsize=0,len0=0;
	if(!fd)
	{
			fd=sp5kFsFileOpen("B:/UDF/SSID_PW1.CFG",SP5K_FS_OPEN_RDONLY|SP5K_FS_OPEN_RDONLY);
			if(fd)
			{
				fsize = sp5kFsFileSizeGet(fd);
				len0 = sp5kFsFileRead(fd, buffer0, fsize);
				fsSimpleWrite("B:/UDF/SSID_PW.CFG", buffer0, len0);
				fd=sp5kFsFileOpen("B:/UDF/SSID_PW.CFG",SP5K_FS_OPEN_RDONLY|SP5K_FS_OPEN_RDWR);
				if(fd)
					printf("--SSID_PW.CFG CREATE\n");
			}
	}
	#endif
	if(!fd)
	{
		printf("ssid cfg file open fail!\n");
		sp5kFsFileClose(fd);
		return FAIL;
	}

	if(gssidname[0]) /* user assign custom ssid name */
	{
		memcpy(ssidname, gssidname, MAX_SSID_NAME_LEN*sizeof(UINT8));
		printf("[custom]ssidname = %s\n", ssidname);
	}
	else /* read default or user save ssid name */
#if 1	
	{
		size = sp5kFsFileSizeGet(fd);
		Len = sp5kFsFileRead(fd, buffer, size);

		for(i=0 ;i<Len ;i++ ){
			if(buffer[i]==0x0d || buffer[i]==0x0a ){
				UINT8 cnt=0;
                memcpy(ssidname, buffer, i);
				ssidname[i]='\0';
				if(buffer[i+1]==0x0d || buffer[i+1]==0x0a ){
					cnt+=1;
				}
				for(j=i+1+cnt;j<Len ;j++ ){ 
					if((buffer[j]==0x0d || buffer[j]==0x0a ) ||(j==Len-1)  ){
						memcpy(gpassword, buffer+i+1+cnt, j-i-1-cnt+1);
						if(cnt==0){
							gpassword[j-i-1-cnt+1]='\0';
						}else{
							gpassword[j-i-1-cnt]='\0';
						}
					   	break;
					}
				}
                break;
			}
		}
		
		printf("[cfg]ssidname = %s Password=%s,Len = %d i=%d j=%d\n", ssidname,gpassword ,Len ,i ,j);
		//slen=sizeof(ssidname);
		memcpy(ssidbak,"MD87156_",8);
		memcpy(ssidbak+8,ssidname+7,6);
		memcpy(ssidname,ssidbak,14);
		ssidname[14]='\0';
		printf("[cfg]ssidnamexx = %s Password=%s,Len = %d i=%d j=%d\n", ssidname,gpassword ,slen+1 ,i ,j);

	}
#else	
	{
		size = sp5kFsFileSizeGet(fd);
		Len = sp5kFsFileRead(fd, buffer, size);

		for(i=0 ;i<Len ;i++ ){
			if(buffer[i]==0x0d && buffer[i+1]==0x0A ){
                memcpy(ssidname, buffer, i);
				ssidname[i]='\0';
				for(j=i+2 ;j<Len ;j++ )
					if((buffer[j]==0x0d && buffer[j+1]==0x0A) ||(j==Len-1)  ){
					    
						memcpy(gpassword, buffer+i+2, Len-i-2);
					    if(buffer[j]==0x0d){;
						 gpassword[j-i-2]='\0';
						}
					   break; 
				}
                break;
			}	
		}	
		printf("[cfg]ssidname = %s Password=%s,Len = %d i=%d j=%d\n", ssidname,gpassword ,Len ,i ,j);

	}

#endif
	if(gssidname[0]){/* if no change, won't write the ssid name to file again */
		sp5kFsFileWrite( fd, buffer, (MAX_SSID_NAME_LEN+MAX_PASSWORD_LEN)*sizeof(UINT8) );
		printf("baiqi write wifi password\n");
	}

	sp5kFsFileClose(fd);
	return SUCCESS;
}
/*
A/RO_RES/UI/JPG/SSID_PW1.CFG\
A/RO_RES/UI/JPG/APMODE1.CFG\
A/RO_RES/UI/JPG/HAPD01.CFG\
*/	
//fsSimpleWrite(mpszDualStreamDstFn, mpPatchJpgEncSyncH264BackupBuf[k], mDualStreamBackupBufSize[k]);

static BOOL appAPModeGet(
	UINT8 *hapMode
)
{
	UINT32 fd;
	UINT32 Len,size;
	UINT8 mode = 0;
	char smode[1]={0};
	//char buffer0[2*1024];
		
	UINT32 fsize=0,len0=0;						
	fd = sp5kFsFileOpen("B:/UDF/APMODE.CFG",SP5K_FS_OPEN_RDONLY|SP5K_FS_OPEN_RDWR);		
	if(!fd)		
	{
			fd=sp5kFsFileOpen("B:/UDF/APMODE1.CFG",SP5K_FS_OPEN_RDONLY|SP5K_FS_OPEN_RDONLY);
			if(fd)
			{
				fsize = sp5kFsFileSizeGet(fd);
				len0 = sp5kFsFileRead(fd, buffer0, fsize);
				fsSimpleWrite("B:/UDF/APMODE.CFG", buffer0, len0);
				fd=sp5kFsFileOpen("B:/UDF/APMODE.CFG",SP5K_FS_OPEN_RDONLY|SP5K_FS_OPEN_RDWR);
				if(fd)
					printf("--APMODE.CFG CREATE\n");
			}
	}

	if(!fd)
	{
		printf("apmode cfg file open fail!\n");
		sp5kFsFileClose(fd);
		return FAIL;
	}

	if(ghapMode) /* user assign custom ap mode */
	{
		memcpy(&mode, &ghapMode, 1*sizeof(UINT8));
	}
	else /* read default or ssid name saved by user  */
	{
		size = sp5kFsFileSizeGet(fd);
		/*printf("[deb2] size = %d\n",size);*/
		Len = sp5kFsFileRead(fd, &mode, size*sizeof(UINT8));
		mode = atoi(&mode);
		printf("[deb2] mode = %d\n",mode);
	}

	*hapMode = mode;
	printf("hapMode = %d\n", *hapMode);

	if(ghapMode){/* if no change, won't write the ap mode to file again */
		sprintf(smode,"%d",*hapMode);
		sp5kFsFileWrite( fd, smode, 1*sizeof(UINT8) );
	}

	sp5kFsFileClose(fd);
	return SUCCESS;
}


void appNetDoExit(
	UINT8 parm
)
{
	UINT8 wifiParm = (UINT8)parm;

	/* [47844] key is not allowed till net init procedure is done, SP5K_MSG_USB_PTP_OP_END |0x80009805 */
	if ((wifiParm & BTN_LCK) == BTN_LCK){
		appBtnEnable(BTN_ALL); 
		callStack();
	}
	
	if ((wifiParm & FTP_SRV)== FTP_SRV){
		printf("exit:0x%x\n",(wifiParm & FTP_SRV));
		appFtpSrvStop();
	}
	
	if ((wifiParm & RTP_STREM)== RTP_STREM){
		printf("exit:0x%x\n",(wifiParm & RTP_STREM));
		appMediaSrvStop();
	}

	if ((wifiParm & PTP_IP)== PTP_IP){
		printf("exit:0x%x\n",(wifiParm & PTP_IP));
		appPtpIpStop();
		ndk_msleep(100); /* [workaround] : hapd can't stop normally issue */
	}

	if ((wifiParm & HOST_AP)== HOST_AP){
		printf("exit:0x%x\n",(wifiParm & HOST_AP));
		appHostAPStop();
	}

	if ((wifiParm & DHCP_SERVER)== DHCP_SERVER){
		printf("exit:0x%x\n",(wifiParm & DHCP_SERVER));
		appDhcpServerStop();
	}

	if((wifiParm & WIFI_LOAD)== WIFI_LOAD){
		printf("exit:0x%x\n",(wifiParm & WIFI_LOAD));
		if (appWiFiSuspend() != 0) {
			printf("WiFi suspend failed!\n");
			return;
		}
	}

	//fix close app on share mode can't open app again
	if(app_PTP_Get_DscOpMode(MODE_ACTIVE)== PTP_DSCOP_MODE_SHARE){
		app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_CAMERA);
	}
}

static void appNetDoInit(
	UINT32 parm
)
{
	UINT8 wifiParm = (UINT8)parm;
	NetStParams *sp = &st_params;
	UINT8 state;
	BOOL start_record = FALSE, stop_record = FALSE;
	uiPara_t* puiPara = appUiParaGet();

	/* [47844] key is not allowed till net init procedure is done, SP5K_MSG_USB_PTP_OP_END |0x80009805 */
	if ((wifiParm & BTN_LCK) == BTN_LCK){
		///appBtnDisable(BTN_ALL); 
		
		/*xuan.ruan@20131128 add for mantis bug 0048884*/
		#ifdef _EVB_HW_  //evb use button right as wifi ctrl
		appBtnEnable(BTN_RIGHT);
		#endif
		callStack();
	}

	if ((wifiParm & WIFI_LOAD) == WIFI_LOAD) {
		state = appInitStateGet();
		if (state & WIFI_LOAD) {
            /* WIFI LOADED */
			if (appWiFiResume() != 0) {
				printf("WiFi resume failed!\n");
				return;
			}
		}
		else {
			if(appWiFiInit(0,"rtk")!=0){
				printf("Loading WiFi module failed!\n");
				return;
			}
			appWifiSsidPasswordSet();
		}
	}

	if ((wifiParm & DHCP_SERVER) == DHCP_SERVER) {
		appDhcpServerStart();
	}

	if ((wifiParm & HOST_AP) == HOST_AP) {
		UINT8 ssidname[MAX_SSID_NAME_LEN]={0};
		UINT8 hapMode = 0;

		if(appSSIDNameGet(ssidname)){
			printf("ssid name get failed!\n");
			return;
		}

		if(appAPModeGet(&hapMode)){
			printf("ap mode get failed!\n");
			return;
		}

		if(hapMode==2)
		{
			const char* passphrase = "1234567890";
			const char* channelscan = "auto";
			if(gpassword[0]){
				appHostAPStart("B:/UDF/HAPD0.CFG", ssidname, gpassword, channelscan);
					
			}
			else{  
				appHostAPStart("B:/UDF/HAPD0.CFG", ssidname, passphrase , channelscan);
				
			}
		}
		else
		{
			const char* channelscan = "auto";
			if(gpassword[0])
				appHostAPStart("B:/UDF/HAPD0.CFG", ssidname, gpassword ,channelscan);
            else
				appHostAPStart("B:/UDF/HAPD0.CFG", ssidname, NULL ,channelscan);
			
		}

		/*ndktimer = appTimerSet(1000, "WiFiinfo");*/
	}

	if ((wifiParm & PTP_IP) == PTP_IP) {
		sp5kUsbPimaConfigSet(SP5K_PIMA_CONFIG_MTP_BK_BUFFER_SIZE, 512*1024); /* 512KB support about 18XX images buffer */
		sp5kTimeDelay(3, 500);
		sp5kUsbPtpOpFuncInstall(USB_PIMA_OC_DELETE_OBJECT, 0, NULL, pimaDelFileCb);
		appPtpIpStart();
		sp5kUsbPimaMtpBkCtl(SP5K_USB_PIMA_MTP_BK_MANUAL);
	}

	if ((wifiParm & FTP_SRV) == FTP_SRV) {
		state = appInitStateGet();
		if (!(state & FTP_SRV)){/* ftpd won't be stopped after WiFi off  */
			appFtpSrvStart();
		}
	}

#ifdef ICAT_STREAMING
	if ((wifiParm & RTP_STREM) == RTP_STREM) {
		UINT32 w,h;

		profLogPrintf(1,"[PTPEVT,PTPQ](%d,%d)",PTP_EVT_SEND_ENABLE,PTP_EVT_QUEUE_ENABLE);

	     /* device property get */
		appVideoSizeGet(puiPara->VideoSize, &w, &h);
		if(appPtpOpModeHandler(app_PTP_Get_DscOpMode(MODE_ACTIVE),&start_record,&stop_record)!=0)
		{
			return;
		}

		printf("****** RTP Streaming setting ******\n");
		printf("**  W(%d)\n**  H(%d)\n**  Video(%s)\n", w, h
			, start_record ? "rec+": (stop_record ? "rec-" : "n/a"));
		printf("***********************************\n");

		/* the param is set in appMediaSrvStart() originally, but since no teardown flow doesn't need to restart media server,
		so w&h should be reset here
		*/
		appRtpResetParams(sp,puiPara->VideoSize);
		if(appPtpVideoHandler(sp,start_record,stop_record)!=0)
		{
			return;
		}

        /* start media server */
		if((app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_CAMERA
			||app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_OFF
			||app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_ON)
			&&!appRtpStreamStateGet())
		{
			appRtpStreamStateSet(1);
			if (!ndk_st_streaming_is_started())
				appMediaSrvStart(w, h);
				 appmediasrcPBVideo();
		}
	}
#endif

	#if 0 /* no need, already sent in SP5K_MSG_USB_PTP_SESSION_OPEN */
	if(!IS_CARD_EXIST){
		appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SDCARD_OUT, 0, 0);
	}
	#endif
	
	if ((wifiParm & DO_ASYNC) == DO_ASYNC) {
		sp5kOsThreadDelete(NULL);
	}
}

UINT32 appPtpOpModeHandler(PTPDscOpMode_e opModeState, BOOL *isStartRec, BOOL *isStopRec)
{
	UINT32 curMode, ret = 0;
	uiPara_t* puiPara = appUiParaGet();
	
	sp5kModeGet(&curMode);
	switch(opModeState){ 
		case PTP_DSCOP_MODE_CAMERA:
		case PTP_DSCOP_MODE_VIDEO_OFF:  /* PTP cmd to control the streaming mode */
			printf("[DSC OP] CAMERA mode / VIDEO mode OFF!\n");
			*isStopRec = TRUE;
			if(curMode==SP5K_MODE_STANDBY){
				sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
				sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
			}

			//for DVS5M2
			/*if(	app_PTP_Get_DscOpMode(MODE_ACTIVE) != app_PTP_Get_DscOpMode(MODE_PREV)
				&& curMode != SP5K_MODE_VIDEO_RECORD)
			{
				sp5kHostMsgSend(APP_UI_MSG_RTP_STOP,0);
				appMediaAttrUpdate(puiPara->VideoSize);
				sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
	   			sp5kHostMsgSend(APP_UI_MSG_RTP_START,0);
			}*/
			//for DVS5M2
			
			#if FIX_SENSOR_MODE_4_CAM_MODE
			/* 47282 / 47614 : a fixed sensor mode for capture mode : s */
			if(app_PTP_Get_DscOpMode(MODE_ACTIVE)== PTP_DSCOP_MODE_CAMERA
				&&puiPara->VideoSize!=UI_VID_SIZE_FHD){
				appMediaAttrUpdate(UI_VID_SIZE_FHD);
			}

			if(app_PTP_Get_DscOpMode(MODE_ACTIVE)== PTP_DSCOP_MODE_VIDEO_OFF
				&&app_PTP_Get_DscOpMode(MODE_PREV)==PTP_DSCOP_MODE_CAMERA
				&&puiPara->VideoSize!=UI_VID_SIZE_FHD)
			{
				sp5kHostMsgSend(APP_UI_MSG_RTP_STOP,0);
				appMediaAttrUpdate(puiPara->VideoSize);
				sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
	   			sp5kHostMsgSend(APP_UI_MSG_RTP_START,0);
			}
			/* 47282 / 47614 : a fixed sensor mode for capture mode : e*/
			#endif
			break;
		case PTP_DSCOP_MODE_VIDEO_ON:  /* PTP cmd to control the streaming mode */
			printf("[DSC OP] VIDEO mode ON!\n");
			*isStartRec = TRUE;
			if(curMode==SP5K_MODE_STANDBY){
				sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
				sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
			}
			break;
		case PTP_DSCOP_MODE_SHARE:
			printf("[DSC OP] SHARE mode!\n");
			
			#ifdef ICAT_STREAMING
			if(appRtpStreamStateGet())
			{
				appStreamClose();
			}
			#endif

			if(curMode!=SP5K_MODE_STANDBY){
				profLogPrintf(0,"enter standy:s");
				sp5kModeSet(SP5K_MODE_STANDBY);
				sp5kModeWait(SP5K_MODE_STANDBY);
				profLogPrintf(0,"enter standy:e");
				printf("idle mode ready!\n");
			}
			
			return 1;
			break;
		case PTP_DSCOP_MODE_APP:
			printf("[DSC OP] APP mode!\n");
			#ifdef ICAT_STREAMING
			if(appRtpStreamStateGet())
			{
				appStreamClose();
			}
			#endif
			if(app_PTP_ContRecMode_Get()&&curMode==SP5K_MODE_VIDEO_RECORD&&
				app_PTP_Get_DscOpMode(MODE_PREV)==PTP_DSCOP_MODE_VIDEO_ON){
				app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_VIDEO_ON);
			}
			else if(curMode==SP5K_MODE_VIDEO_PREVIEW&&
				app_PTP_Get_DscOpMode(MODE_PREV)==PTP_DSCOP_MODE_CAMERA){
				app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_CAMERA);
			}
			else if(curMode==SP5K_MODE_VIDEO_PREVIEW&&
				app_PTP_Get_DscOpMode(MODE_PREV)==PTP_DSCOP_MODE_VIDEO_OFF){
				app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_VIDEO_OFF);
			}
			else {
				app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_VIDEO_OFF);
			}
			app_PTP_clear_EventQ();
			/* Digital Zoom Reset : s */
			if(appDZoomGetRation()!=1000){
				appDZoomReset(APP_DZOOM_UI_MODE_VIDEO_VIEW);
				appStill_SetDZoom(puiPara->DigitalZoom);
			}
			/* Digital Zoom Reset : e */
			return 1;
			break;
		default:
			printf("[PTP] you got the wrong camera mode:%d\n",app_PTP_Get_DscOpMode(MODE_ACTIVE));
			return 1;
			break;
	}
	return ret;
}	

UINT32 appPtpVideoHandler(NetStParams *sp, BOOL isStartRec, BOOL isStopRec)
{
	UINT32 ret = 0;
	uiPara_t* puiPara = appUiParaGet();
	
	HOST_ASSERT(!(isStartRec && isStopRec));
	// Start Record
	if (appRtpStreamStateGet() && isStartRec) {
		// Enter VideoRecord mode
		UINT32 curMode;
		sp5kModeGet(&curMode);

		if (curMode == SP5K_MODE_VIDEO_RECORD){
			printf("[RTP]Already in record mode\n");
			return 1;
		}
		else
		{
			#if !SENSOR_MODE_CHANGE_WITH_VIDEOSIZE
			/* Mantis issue #47000, video size no change issue */
			appRtpUpdateMediaAttrs(sp,puiPara->VideoSize);
			#endif
			
			#if SP5K_CDFS_OPT
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
			#endif
			//appStateChange(APP_STATE_VIDEO_REC,  APP_STATE_MSG_INIT);
			if(appCheckSdvMainMode() == TRUE)
			{
				
				if(appSdvVideoDiskCheckMemeoryFull() == FALSE)
				{

					if(nWifiRecKeySkip==TRUE)
						return 1;
					nWifiRecKeySkip=TRUE;
					puiPara->SdvVideoMode=UI_SCDV_VIDEO_MODE_NORMAL;
					appStateChange(APP_STATE_SPORTDV_VIDEO_PREVIEW,  APP_STATE_MSG_INIT);				
					appOsDelayMs(100);
					//sp5kHostMsgSend(APP_KEY_PRESS_OK, PTP_KEY_MSG_PARAM);
					
					//appStateChange(APP_STATE_SPORTDV_VIDEO_REC,  APP_STATE_MSG_INIT);
					
					switch(puiPara->SdvVideoMode)
					{
						case UI_SCDV_VIDEO_MODE_NORMAL:
							appStateChange(APP_STATE_SPORTDV_VIDEO_REC, STATE_PARAM_NORMAL_INIT);
							break;
						case UI_SCDV_VIDEO_MODE_LAPSE:
							appStateChange(APP_STATE_SPORTDV_VIDEO_REC_SLOW, STATE_PARAM_NORMAL_INIT);
							break;
					}
					
					nWifiRecKeySkip=FALSE;
				}
				else
				{
					//memoryFullEvtFlg=1;
					appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 0, 0);
					return 1;

				}	
			}
			else
			{
				
				if(appCdvVideoDiskCheckMemeoryFull()==TRUE)
				{
					if(appCdvRecBeforeReleaseDiskSpace()==TRUE)
					{						
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 0, 0);
						return 1;
					}						
				}			
				if(nWifiRecKeySkip==TRUE)
						return 1;
				nWifiRecKeySkip=TRUE;
				puiPara->CdvVideoMode=UI_SCDV_VIDEO_MODE_NORMAL;
				appStateChange(APP_STATE_CARDV_VIDEO_PREVIEW,  APP_STATE_MSG_INIT);				
				appOsDelayMs(100);
				//sp5kHostMsgSend(APP_KEY_PRESS_OK, PTP_KEY_MSG_PARAM);				
				//appStateChange(APP_STATE_CARDV_VIDEO_REC,  APP_STATE_MSG_INIT);
				switch(puiPara->CdvVideoMode)
				{
					case UI_SCDV_VIDEO_MODE_NORMAL:
						appStateChange(APP_STATE_CARDV_VIDEO_REC, STATE_PARAM_NORMAL_INIT);
						break;
					case UI_SCDV_VIDEO_MODE_LAPSE:						
						appStateChange(APP_STATE_CARDV_VIDEO_REC, STATE_PARAM_NORMAL_INIT);						
						//appStateChange(APP_STATE_CARDV_VIDEO_REC_LAPSE, STATE_PARAM_NORMAL_INIT);
						break;
				}
				nWifiRecKeySkip=FALSE;
			}
		}
		printf("\n[RTP] Start record!\n");
		return 1;
	}
	// Stop Record
	else if (appRtpStreamStateGet() && isStopRec) {
		// Leave VideoRecord mode
		UINT32 curMode;
		sp5kModeGet(&curMode);
		printf("\n[RTP] Video Stop now!\n");
		if (curMode == SP5K_MODE_VIDEO_RECORD){
			///appBtnDisable(BTN_ALL);

			/*xuan.ruan@20131128 add for mantis bug 0048884*/
			#ifdef _EVB_HW_  //evb use button right as wifi ctrl
			appBtnEnable(BTN_RIGHT);
			#endif
			//appVideoStop();
			sp5kHostMsgSend(APP_KEY_PRESS_OK, PTP_KEY_MSG_PARAM);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);			
			appOsDelayMs(100);	//wait cdfs update

			UINT32 totalFile;
			static  UINT8 pMediafilename[32];
			sp5kDcfAttrElm_t pdcfAttr;

			sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
			if(sp5kDcfFsFileAttrGet(totalFile, &pdcfAttr, pMediafilename))
				printf("[RTP] Get dcf attr fail!\n");
			else
				printf("totalFile = %d, media file : %s\n",totalFile, pMediafilename);

			/* Notify mobile APP that new video object file is created */
			#if SP5K_CDFS_OPT/* tmp disable, wait for the video file path issue solved 20130305*/
			//pMediafilename[2] = '\\';
			//pMediafilename[8] = '\\';
			appPathSlashReplace(pMediafilename);
			#endif
			printf("media file : %s\n",pMediafilename);
			appStill_PIMA_Send_Event(PTP_EC_OBJECT_ADDED, (UINT32)pMediafilename, 0, 0);
			appBtnEnable(BTN_ALL); 
		}
		else{
			printf("[RTP]Not in record mode\n");

			//for DVS5M2 cdv video lapse
			if(appActiveStateGet() == APP_STATE_CARDV_VIDEO_REC_LAPSE)
			{
				sp5kHostMsgSend(APP_KEY_PRESS_OK, PTP_KEY_MSG_PARAM);
				while(appActiveStateGet() != APP_STATE_WIFI_CONNECTION)
				{
					appOsDelayMs(50);	//wait cdfs update
				}	
			}
			//for DVS5M2 cdv video lapse
			
			return 1;
		}

		printf("\n[RTP] Stop record!\n");
		return 1;
	}
	else if(appRtpStreamStateGet())
	{
		printf("\n[RTP] No Streaming mode switch !!\n");
		return 1;
	}
	return ret;
}

void appMediaAttrUpdate(UINT8 vidsize)
{
	NetStParams *sp = &st_params;
	
	printf("Update Param\n");
	appRtpResetParams(sp,vidsize);
	appRtpUpdateMediaAttrs(sp,vidsize);
}

/* External using API */
void appPtpStateSet(BOOL en)
{
	bPtpStatus = en;
}

BOOL appPtpStatGet()
{
	return bPtpStatus;
}

void appRtpStreamStateSet(
	BOOL en
)
{
	profLogPrintf(0,"rtp streaming state set:%d",en);
	bRtpStatus = en;
}

BOOL appRtpStreamStateGet()
{
	profLogPrintf(0,"rtp streaming state get:%d",bRtpStatus);
	return bRtpStatus;
}

BOOL appIsStreamingActive()
{

#ifdef ICAT_STREAMING
	return ndk_st_streaming_is_started();
#else
	return 0;
#endif

}

UINT32 appNetNumGet()
{
	static long sta_nr = 0;
	long n;
	if (ndk_netif_ioctl(NDK_IOCG_WIFI_STA_NR, (long)"wlan0", &n) == 0) {
		if (n != sta_nr) {
			profLogPrintf(1,"sta number:%u -> %u", (unsigned int)sta_nr, (unsigned int)n);
			sta_nr = n;
		}
	}
	else
		return 0;

	return n;
}

void appNetSTAInfoGet(long mac,int expire,int expire_max)
{
#if 0
	struct if_point ifp;
	char buf[256];
	//long long mac = 0;
	int count = 0;
	char *pch;

	ifp.pointer = buf;
	ifp.length = NDK_ARRAY_SIZE(buf);

	if (ndk_netif_ioctl(NDK_IOCG_WIFI_STA_INFO, (long)"wlan0", (long *)&ifp) != 0) {
		printf("[IOCTL] NDK_IOCG_WIFI_STA_INFO error\n");
		return;
	}

	pch = buf;

	/* FIXME: it seems ndk_sscanf() doesn't support %llx pattern */
	while (ndk_sscanf(pch, "MAC:%x,EXP:%d/%d\n*", &mac, &expire, &expire_max) == 3) {
		printf("[%d] MAC address %12llX have no reponse during %d second\n", count, mac, (expire_max - expire) * 2);
		pch = memchr(pch, '\n', buf + NDK_ARRAY_SIZE(buf) - pch);
		if (pch == NULL) {
			break;
		}
		++pch;
		++count;
	}
#endif
}

void appWiFiStartConnection(UINT8 wifiParm)
{
	printf("%s,0x%x\n",__FUNCTION__,wifiParm);
	if ((wifiParm & DO_ASYNC) == DO_ASYNC) {
		sp5kOsThreadCreate("netStart", appNetDoInit, (UINT32)wifiParm
		                   , ndk_thread_get_priority(), 0, 0
		                   , TX_AUTO_START);
	}
	else {
		appNetDoInit((UINT32)wifiParm);
	}
}

void appWiFiStopConnection(UINT8 wifiParm)
{
	profLogAdd(0,"appWiFiStopConnection");
	printf("%s,0x%x\n",__FUNCTION__,wifiParm);
	appExceptionHandle();
	appNetDoExit(wifiParm);
	appNetStateReset(wifiParm);
}

void appExceptionHandle()
{
	appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_EXCEPTION, 0, 0); /* notify the APP to do connection close */
	ndk_msleep(1500);/* wait for mobile process*/
}

/* app_net_cmd.c is removed */
void net_system_init()
{
	ndk_global_init(20);
	printf("WiFi system init done.\n");
}

/* AP mode <-> STA mode switch */
#if 0
void util_apmode_enter();
void util_apmode_exit();
void util_stamode_enter(UINT8 *mob_ssid,UINT8 *mob_pass);
void util_sta_test();

#define MOB_WPA2 (1)
void util_apmode_enter()
{
	printf(">>> enter AP mode\n");
	appWiFiStartConnection(0x07);
}

void util_apmode_exit()
{
	printf("<<< exit AP mode\n");
	appHostAPStop();
	appDhcpServerStop();
}

void util_stamode_enter(UINT8 *mob_ssid,UINT8 *mob_pass)
{
	char buf[64];
	ndk_wpas_start();
	ndk_msleep(300);

	ndk_wpac_begin();
	ndk_wpac_request("add_network");

	sprintf(buf, "set_network 0 ssid '\"%s\"'", mob_ssid);
	ndk_wpac_request(buf);

	#if MOB_WPA2
	sprintf(buf, "set_network 0 psk '\"%s\"'", mob_pass);
	ndk_wpac_request(buf);
	ndk_wpac_request("set_network 0 key_mgmt WPA-PSK");
	#else
	ndk_wpac_request("set_network 0 NONE WPA-PSK");
	#endif

	ndk_wpac_request("enable_network 0");
	ndk_wpac_end();
	ndk_dhcp_start("wlan0");
}

void util_sta_test()
{
	util_apmode_enter();
	#if 0
	static long sta_num=0;
	while( !sta_num ){
		ndk_netif_ioctl(NDK_IOCG_WIFI_STA_NR, (long)"wlan0", &sta_num);
	}
	#endif
	profLogPrintf(0,"exit AP mode +");
	util_apmode_exit();
	util_stamode_enter("lumia_920_tien","0916816138");
	profLogPrintf(0,"enter STA mode -");
}
#endif

BOOL appMacAddressGet(UINT8 *strMac)
{
	UINT8 state;
	UINT8 hwaddr[32] = "\0"; 
	memset(hwaddr, 0, 32);
	
	#if 0
	//appNetDoInit(WIFI_LOAD);
	state = appInitStateGet();
	if (state & WIFI_LOAD) 
	{
		if (appWiFiResume() != 0) 
		{
			printf("WiFi resume failed!\n");
			return FALSE;
		}
	}
	else 
	{
		if(appWiFiInit(0,"rtk")!=0)
		{
			printf("Loading WiFi module failed!\n");
			return FALSE;
		}
	}
	ndk_netif_ioctl(NDK_IOCG_IF_HWADDR, (long)"wlan0", (long *)hwaddr);	//Need WIFI Init First
	//appNetDoExit(WIFI_LOAD);
	#else
	state = appInitStateGet();
	if(state & WIFI_LOAD)
	{
		ndk_netif_ioctl(NDK_IOCG_IF_HWADDR, (long)"wlan0", (long *)hwaddr);	//Need WIFI Init First
	}
	else
	{
		printf("WIFI NOT Load!\n");
		return FALSE;
	}
	#endif

	if(strMac != NULL)
	{
		sprintf(strMac, "%02x%02x%02x%02x%02x%02x", hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
		strupr(strMac);
	}
	
	printf(	"Get mac addr = %x:%x:%x:%x:%x:%x\n", 
	                hwaddr[0],hwaddr[1],hwaddr[2], 
	                hwaddr[3],hwaddr[4],hwaddr[5]);

	return TRUE;
}

#if  1
/*---------------------RTSP Playback-------------------------------------------*/
typedef struct media_info_s {
	NDKStMediaSrcPushBufferFunc push_func;
	void *push_param;
	char fname[256];
} media_info_t;
static media_info_t *rtspHandle = NULL;
static NDKStMediaSrcCallback g_msrc_cb;
static struct timeval iicount;

UINT8 *g_videoBufferAddr ,gfseek=0;
SINT32 g_videoBufferLength ,g_videoElapseTime ;
static UINT32 gRTSPimgW;
static UINT32 gRTSPimgh;

TX_EVENT_FLAGS_GROUP  evRTSPPBInit;

#define RTSPPB_INIT_THREAD_PRIORITY 	14
#define RTSPPB_INIT_THREAD_TIME_SLICE   5
#define EVENT_RTSPPB_INIT			0x0001
#define EVENT_RTSPPB_SENT      0x0002 
void appRTSPPBSent(void);
void appRTSPPBStart(void);
void appMediaSrcOnGotBuffer(UINT8 *buf, unsigned long user_data);
void app_PbVideoScaleYUV2JFIF( UINT32 yuvAddr, UINT32 YUVwidth ,UINT32 YUVHeight ,UINT32 frm );

UINT32 g_yuvAddr ;
UINT32 g_YUVwidth ;
UINT32 g_YUVHeight;
UINT32 g_frm ;

/*-------------------------------------------------------------------------
 *  File Name : appRTSPPBInitThread
 *------------------------------------------------------------------------*/
void
appRTSPPBInitThread(
	ULONG init_data
)
{
	ULONG ulActualEvents;	
	UINT32 c;
	
	while(1) {
		
		sp5kOsEventFlagsGet(&evRTSPPBInit,EVENT_RTSPPB_INIT |EVENT_RTSPPB_SENT,TX_OR_CLEAR,&ulActualEvents,TX_WAIT_FOREVER);
		
		if (ulActualEvents & EVENT_RTSPPB_INIT) {			
			//appMediaSrcOnGotBuffer(g_videoBufferAddr, g_videoBufferLength) ;
           
		}else{
		    app_PbVideoScaleYUV2JFIF(g_yuvAddr, g_YUVwidth ,g_YUVHeight , g_frm );
			//appMediaSrcOnGotBuffer(g_videoBufferAddr, g_videoBufferLength) ;
			//appRTSPPBSent();
		}      
		    
		
	}
}

void
appRTSPPBInitStart(
	UINT32 event
)
{
		
	sp5kOsEventFlagsSet(&evRTSPPBInit,event,TX_OR);
	
}


void 
appRTSPPBTaskInit(
	void
)	
{
	TX_THREAD *ptrdRTSPPB;

	DBG_PRINT("%s start\n",__FUNCTION__);
	profLogAdd(0, "RTSPPB init");

	sp5kOsEventFlagsCreate(&evRTSPPBInit, "RTSPPBInitEvent");

	ptrdRTSPPB = sp5kOsThreadCreate( 
					  "RTSPPBInitThread",
			                appRTSPPBInitThread,
			                0,
			                RTSPPB_INIT_THREAD_PRIORITY,
			                RTSPPB_INIT_THREAD_PRIORITY,
			                RTSPPB_INIT_THREAD_TIME_SLICE,
			                TX_AUTO_START );

}


void 
appRTSPPBStart(void)
{
	profLogAdd(0, "RTSPPB start");
	appRTSPPBInitStart(EVENT_RTSPPB_INIT);
	
}

void 
appRTSPPBSent(void)
{
	profLogAdd(0, "RTSPPB send");
	appRTSPPBInitStart(EVENT_RTSPPB_SENT);
	
}


void appMediaSrcOnGotBuffer(UINT8 *buf, unsigned long user_data)
{
	NDKStMediaBuffer mb;
	
	//if ((buf->bufflags & MEDIA_SRC_ES_BITMASK) == MEDIA_SRC_ES_AUDIO)
	//	mb.f_buftype = NDK_ST_MEDIABUFFER_AUDIO;
	//else if ((buf->bufflags & MEDIA_SRC_ES_BITMASK) == MEDIA_SRC_ES_VIDEO)
		mb.f_buftype = NDK_ST_MEDIABUFFER_VIDEO;
	//else
	//	return;
    iicount.tv_usec += 33000 ;
	if (iicount.tv_usec > 1000000) {
		iicount.tv_usec -= 1000000;
		++iicount.tv_sec;
	}
	
	mb.bufobj       = buf;
	mb.f_keyframe   = 1 ;//buf->bufflags & MEDIA_SRC_KEY_FRAME ?   1 : 0;
	mb.f_paramframe = 0 ;//buf->bufflags & MEDIA_SRC_PARAM_FRAME ? 1 : 0;
	mb.data         = buf;
	mb.length       = user_data;
	mb.duration     = 33 ;//buf->duration;
	mb.pts          = iicount;
    
	if (rtspHandle && rtspHandle->push_func) {
		rtspHandle->push_func(&mb, rtspHandle->push_param);
	}
}

#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )



void
app_PbVideoScaleYUV2JFIFCallBack( UINT32 yuvAddr, UINT32 YUVwidth ,UINT32 YUVHeight ,UINT32 frm )
{
	g_yuvAddr=yuvAddr ;
	g_YUVwidth=YUVwidth ;
	g_YUVHeight = YUVHeight ;
	g_frm = frm ;
	profLogAdd(0,"app_PbVideoScaleYUV2JFIFCallBack --");
	appRTSPPBSent();
}



void
app_PbVideoScaleYUV2JFIF( UINT32 yuvAddr, UINT32 YUVwidth ,UINT32 YUVHeight ,UINT32 frm )
{
 sp5kYuvCbImgSize_t *size;
 UINT8 *pTemp=NULL;
 char fname[32];
 static UINT32 id=0; 
 UINT32 PBVideosamplebuff=0;
// printf("~~~~ NDK %s  %d\n",__FUNCTION__,__LINE__);
 if(gfseek)
 	return ;
 #if 0
 printf("%s %d jpgWidth:%d jpgHeight:%d\n",__func__,__LINE__,YUVwidth,size->jpgHeight);
 printf("%s %d yuvWidth:%d yuvHeight:%d\n",__func__,__LINE__,size->yuvWidth,size->yuvHeight);
 printf("%s %d roiX    :%d roiY     :%d\n",__func__,__LINE__,size->roiX,size->roiY);
 printf("%s %d roiWidth:%d roiHeight:%d\n",__func__,__LINE__,size->roiWidth,size->roiHeight);
 #endif

//printf("Q@\n");
profLogAdd(0,"app_PbVideoScaleYUV2JFIF S");
 sp5kGfxObj_t  dstObj;
 
 sp5kGfxObj_t srcObj={(UINT8 *)yuvAddr,NULL,ROUND_UP_TO_DIVISIBLE(YUVwidth,16),ROUND_UP_TO_DIVISIBLE(YUVHeight,16),0,0, SP5K_GFX_FMT_YUV422, YUVwidth, YUVHeight}; 
 srcObj.fmt = SP5K_GFX_FMT_YUV420;
 dstObj.bufH = 360 ;//size->jpgHeight;
 dstObj.bufW = 640 ;//size->jpgWidth;
 
 srcObj.roiX = dstObj.roiX = 60;

 dstObj.roiH = 360 ;
 dstObj.roiW = 640 ;
 dstObj.fmt = SP5K_GFX_FMT_JFIF;
 dstObj.pbuf = (UINT8 *)sp5kYuvBufferAlloc(640 ,360);
 
 UINT32 nRet = sp5kGfxObjectConvert(&srcObj, &dstObj);
 profLogAdd(0,"app_PbVideoScaleYUV2JFIF E");
 g_videoBufferAddr = dstObj.pbuf ;
 g_videoBufferLength = dstObj.roiX ;
 //appRTSPPBSent();
 
 #if 0
 if (nRet == SUCCESS) {
  sprintf(fname, "D:\\TEMP%04d.JPG", id);  
  fsSimpleWrite(fname, dstObj.pbuf,dstObj.roiX);
  } 
 #endif
  
 appMediaSrcOnGotBuffer(g_videoBufferAddr, g_videoBufferLength) ;	
 //printf("VLC =%d\n",g_videoBufferLength);
 
/* sp5kYuvBufferFree(dstObj.pbuf);
 if(nRet == SUCCESS) 
    return dstObj.pbuf;
 else
 	return 0;  */

}

static NDKStMediaSrcHandle
_MediaSrcOpen(
	NDKStMediaSrcPushBufferFunc push_func,
	void *push_param,
	void *src_arg,
	...)
{
	NDKStMediaSrcHandle handle = NULL;
	
	g_videoElapseTime =0 ;
	if (rtspHandle == NULL) {
		rtspHandle = sp5kMallocCache(sizeof(media_info_t));
		rtspHandle->push_func = push_func;
		rtspHandle->push_param = push_param;
		strcpy(rtspHandle->fname, src_arg);
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appRTSPPBTaskInit();
		iicount.tv_sec = 0;
		iicount.tv_usec = 0;
		//appRTSPPBStart();
		sp5kMediaPlayFileNameSet(src_arg);
		sp5kStillPlayCfgSet(SP5K_SPB_RING_BUFFER_MAX_NUM,8);	
		sp5kModeSet(SP5K_MODE_VIDEO_PLAYBACK);
		sp5kModeWait(SP5K_MODE_VIDEO_PLAYBACK);
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &gRTSPimgW);
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &gRTSPimgh);
		//appHostMsgWaitExact(SP5K_MSG_MODE_READY,SP5K_MODE_VIDEO_PLAYBACK,5000);
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &g_videoElapseTime) ;
		
		sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_VIDEO_FRAME_CB ,(UINT32)app_PbVideoScaleYUV2JFIFCallBack );
		
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_START, 0, 0);
		handle = (NDKStMediaSrcHandle)rtspHandle;
	}
	return handle;
}

static void _MediaSrcClose(NDKStMediaSrcHandle h)
{
	if (rtspHandle && rtspHandle == h) {
		gfseek =1;
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);
		APP_SLEEP_MS(500);
		sp5kFree(rtspHandle);
		gfseek =0;
		rtspHandle = NULL;
	}
}

static UINT32 _MediaSrcPause(NDKStMediaSrcHandle h)
{
	UINT32 ret = FAIL;

	if (rtspHandle && rtspHandle == h) {
		/* pause */
		printf("_MediaSrcPause\n");
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_PAUSE, 0, 0);		
		ret = SUCCESS;
	}
	return ret;
}

static UINT32 _MediaSrcResume(NDKStMediaSrcHandle h)
{
	UINT32 ret = FAIL;

	if (rtspHandle && rtspHandle == h) {
		/* resume */
		//printf("_MediaSrcResume\n");
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);		
		ret = SUCCESS;
	}
	return ret;
}

/*return TRUE: at the end of source. FALSE: not.*/
static BOOL _MediaSrcEndOfSource(NDKStMediaSrcHandle h)
{
	UINT32 ret = FALSE ;
	SINT32 duration;
    
	if (rtspHandle && rtspHandle == h) {
		/* eof? */
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, (UINT32 *)&duration); 
		//printf("!!time %d  %d\n",g_videoElapseTime ,duration );
		if(g_videoElapseTime == duration ){
			gfseek =1;
			printf("%s is todo\n", __FUNCTION__);
			sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);
			
			//appHostMsgWaitExact(0,SP5K_MSG_MEDIA_PLAY_END,8000);
			gfseek =0;
			ret = TRUE;
		}
	}
	return ret;
}

static UINT32 _MediaSrcSeekTo(NDKStMediaSrcHandle h, UINT32 position)
{
	UINT32 ret = FAIL;

	if (rtspHandle && rtspHandle == h) {
		if(position == 0){
			position = 0;
		}
		/* resume */
		gfseek =1;
		profLogPrintf(0,"seek S  from %d",position);
		printf("%s is todo %d\n", __FUNCTION__,position);
		iicount.tv_sec = position/1000;
		iicount.tv_usec = (position % 1000)*1000;
		profLogPrintf(0,"seek SS  from %d",position);
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEEK, position, 0);
		profLogPrintf(0,"seek EE  from %d",position);
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);
			
	    profLogAdd(0,"seek e");
		gfseek =0;
		ret = SUCCESS;
	}
	return ret;
}

static UINT32 _MediaSrcGetAttribute(NDKStMediaSrcHandle h, UINT32 attr, UINT32 *val)
{
	UINT32 ret = FAIL;

	if (rtspHandle && rtspHandle == h) {
		ret = SUCCESS;
		switch (attr) {
			case SP5K_MEDIA_ATTR_WIDTH:
				*val = 640;
				break;
			case SP5K_MEDIA_ATTR_HEIGHT:
				*val = 360;
				break;
			case SP5K_MEDIA_ATTR_VIDEO_CODEC:
				*val = SP5K_MEDIA_VIDEO_MJPG;
				break;
			case SP5K_MEDIA_ATTR_DURATION:
			case SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE:
			
			case SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS:
			case SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE:
			case SP5K_MEDIA_ATTR_AUDIO_CHANNELS:
				/* IMPORTANT: wait here if PB mode is not ready */
				sp5kModeWait(SP5K_MODE_VIDEO_PLAYBACK);
				sp5kMediaPlayAttrGet(attr, val);
				break;
			case SP5K_MEDIA_ATTR_AUDIO_CODEC:	
				*val = SP5K_MEDIA_AUDIO_UNKNOWN ;
				break ;
			default:
				printf("unknown attr %x required\n", attr);
				ret = FAIL;
				break;
		}
	}

	return  ret;
}

static void _MediaSrcFreeBufferObject(NDKStMediaBufferObject obj)
{
	//printf("%s is todo\n", __FUNCTION__);
	profLogAdd(0,"MediaSrcFreeBuffer");
	sp5kYuvBufferFree((void *)obj);
}

static void appmediasrcPBVideo(void)
{
	
	g_msrc_cb.open = _MediaSrcOpen;
		
	g_msrc_cb.close = _MediaSrcClose;
	g_msrc_cb.pause = _MediaSrcPause;
	g_msrc_cb.resume = _MediaSrcResume;
	g_msrc_cb.end_of_source = _MediaSrcEndOfSource;
	g_msrc_cb.seek_to = _MediaSrcSeekTo;
	g_msrc_cb.get_attribute = _MediaSrcGetAttribute;
	g_msrc_cb.free_buffer_object = _MediaSrcFreeBufferObject;
   
	ndk_st_register_mediasrc(NDK_ST_MEDIASRC_FILE, &g_msrc_cb);
	printf("---ndk_st_register_mediasrc\n");
}

#endif

