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
#include <time.h>

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"

#include "app_face_detect_ex.h"
#include "sp5k_rsvblk_api.h"

#include "sp5k_gfx_api.h"
#include "app_pal.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "app_zoom_api.h"

#include "sp5k_media_api.h"
#include "app_still.h"
#include "app_video.h"
#include "app_error_msg.h"
#include "app_view_param.h"
#include "app_calib_api.h"
#include "app_ui_para.h"
#include "app_view_osd.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_video.h"
#include "app_cdfs_api.h"
#include "app_exif.h"
#include "app_battery.h"
#include "app_aealg_debug.h"
#include "app_error_msg.h"
#include "app_init.h"
#include "app_gSensor.h"
#include "app_menu.h"
#if GPS_SUPPORT
#include "app_gps.h"
#endif
#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif

#if ICAT_WIFI
#include "app_wifi_connection.h"
#endif
#if ICAT_FCWS || ICAT_LDWS
#include "app_cvrws_api.h"
#if ICAT_LDWS
#include "sp5k_cvr_warning_system_api.h"
#endif
#endif

#include "app_diq.h"
#include "app_scdv_ui_para_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define VIDEO_REC_STATUS_RECORDING 0
#define VIDEO_REC_STATUS_PAUSE 1
#define VIDEO_REC_STATUS_ABORT 2
#define VIDEO_REC_STATUS_CLOSE 3
#define STRING_SIZE 100

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 VideoRecStatus;
#if CVR_WARN_SYS_INT_DBG
static UINT32 videoSeamlessFrameId=0;
#endif

UINT8 sFirstfName[32];
UINT8 sFirstTXTfName[32];
UINT8 sCurrentFName[32];
tmx_t rtc;   /*change struct tm rtc => tmx_t rtc, fan.xiao@20121112*/
sp5kMediaVideoStampParam_t stampPara;
static UINT32 stampPage = 0;
static UINT8 *stampDateBuf = NULL; /* for string buffer */
static UINT32 fntBeenLoad = 1;
static UINT32 stampDrawRatioW;
static UINT32 stampDrawRatioH;
static UINT32 GSensorTimer = 0;
static UINT8 GSensorReadFlag = 0;
SINT16 GSensorInfo[GSENSOR_DATA_CNT][3];
static UINT32 VideoRecPaReMsgReceive =TRUE;
#if VIDEO_REC_ALWAYS
static BOOL vidRecRestart = 1;
static BOOL vidManualStop = 0;
#endif
#if SP5K_CDFS_OPT
static UINT32 videoSizeCheck;
#endif
/* ADD BY SC.LIU  mark by jj*/
#if 0
BOOL doFirstTimer=TRUE;
BOOL getTimer=FALSE;
UINT32 timer=0xffffffff;
#endif
/*lester*/
aeDebugInfo_t aedebug;
UINT32 capFlag=0x55;/*0x55:you can capture */
UINT8 cutSkipFlag = 0;
UINT8 alreadyRecFileFlag = 0;	/* 1:means have already record files after power on */
UINT8 gsensorRecEventFlag = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 SnapSetflag;
extern UINT8 fileTypeSelect;
extern UINT8 eventLockFileName[32];
extern UINT8 stamp_GPSSpeedBuf[20];
extern UINT8 VideoRecStampGsensorBuf[25];
extern UINT8 countDownRecFlag;
extern UINT8 powerOffInVideoRec;
extern UINT8 gsensorDelayRecTimes;
extern UINT8 gsensorTriggerByIntFlag;
extern UINT8 gsensorTriggerRecordingStateFlag;
#if SP5K_MOTION_DETECT
extern UINT32 mdTime;
#endif
extern void appAudioRecVolumeRestore(void);
extern UINT32 osMemFree(void *ptr);
extern void *osMemAlloc(UINT32 size);
extern void appVideo_SetFaceTracking(UINT8 command);
extern void appErrMsgTimeISRWithSound(appErrMsgCaseOption_e errMsg,appErrMsgSoundOption_e errSound,UINT32 showTimeInMs);
extern void callStack(void);
extern void sioCtrl(UINT32);
extern UINT32 _IqGetCurrISO(IQ_STATE_t state);
extern UINT32 _IqGetOPState(void);


/*#define VIDEO_REC_STAMP*/  /*clean warning , this macro definded in prj.def*/
#ifdef VIDEO_REC_STAMP
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appVideoRecStateInit(void);
void appVideoRecLogFileAdd(UINT32 msgadd ,UINT32 param){

	char buf[80];
	tmx_t time;
	UINT8 /*fileChar[50] ,*/fname[32];
	static UINT16 fileCount =1 ;
	memset(buf, 0, 80);
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &time);
	sprintf(buf,"msg=%x  param=%x time %04d:%02d:%02d:%02d:%02d:%02d",msgadd ,param ,
						time.tmx_year+1900,
						time.tmx_mon,
						time.tmx_mday,
						time.tmx_hour,
						time.tmx_min,
						time.tmx_sec
	);
	sprintf(fname, "D:\\dropblock_%d.TXT", fileCount);
	fileCount ++;
	WriteFile(fname , (UINT8*)buf,80);


}

void appVideoRecOSDStampUpdate(
	void
 )
{
	UINT32 offsetX =0,offsetY =0;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX = 20;
		offsetY = 10;
	}
	#endif
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0, offsetX, offsetY, 240+offsetX, 60+offsetY);	/* clear */
	//appOsdSFNFileLoad(SP5K_RES_DATE_STAMP_OSD, "UI\\FONT\\DATSTAMP.SFN");
	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	sp5kGfxStringDraw(SP5K_GFX_PAGE_OSD_0, offsetX+2,offsetY+3, SP5K_GFX_ALIGN_TOP_LEFT, STR_ENCODE_ASCII, stampDateBuf);
}


 void appVideoRecStampStringGet(void)
 {

	UINT32 exp;
	UINT32 agc;
	/*extern UINT32 Rgain_scr;
	extern UINT32 Bgain_scr;
	extern UINT32 LV_scr;
	extern UINT32 CTC_scr;
	extern UINT32 appAeInfoExpidxGet();
	extern UINT32 appAeInfoAgcidxGet();*/  /*junqiong.hu@20121225 modify for mantis bug 0046601*/
	appCalibData_t *pCalib;

	uiPara_t* puiPara = appUiParaGet();
	pCalib = appCalibDataGet();
	if(pCalib->videoRecDateStamp){
		sp5kPreviewExpAgcGet(&exp,&agc);
	#if SP5K_ALGORITHM_AWB && SP5K_ALGORITHM_AE
		UINT32 rgain_scr=0;  /*junqiong.hu@20121225 modify for mantis bug 0046601*/
		UINT32 bgain_scr=0;
		UINT32 lv_scr=0;
		UINT32 ctc_scr=0;
		extern UINT32 appAeInfoExpidxGet();
		extern UINT32 appAeInfoAgcidxGet();
		#if ALGORITHM_AWB
			extern UINT32 Rgain_scr;
			extern UINT32 Bgain_scr;
			extern UINT32 LV_scr;
			extern UINT32 CTC_scr;
			rgain_scr = Rgain_scr;
			bgain_scr = Bgain_scr;
			lv_scr = LV_scr;
			ctc_scr = CTC_scr;
		#endif
		printf("<lester>appVideoRecStampStringGet\n");
		if(puiPara->VideoStampMode == UI_STAMP_DATE)
 		{
			switch(puiPara ->DateStyle)
			{
				case DATE_TIME_MMDDYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d  %d %d %d %d %d %d %d %d ", rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_year+1900,exp ,agc ,appAeInfoExpidxGet() ,appAeInfoAgcidxGet() ,rgain_scr ,bgain_scr ,lv_scr ,ctc_scr);
					break;
				case DATE_TIME_DDMMYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d  %d %d %d %d %d %d %d %d ",rtc.tmx_mday,rtc.tmx_mon,rtc.tmx_year+1900,exp ,agc,appAeInfoExpidxGet() ,appAeInfoAgcidxGet() ,rgain_scr ,bgain_scr ,lv_scr ,ctc_scr);
					break;
				case DATE_TIME_YYMMDD:
				default:
					sprintf(stampDateBuf,  "%d/%02d/%02d  %d %d %d %d %d %d %d %d ",rtc.tmx_year+1900, rtc.tmx_mon,rtc.tmx_mday,exp ,agc,appAeInfoExpidxGet() ,appAeInfoAgcidxGet() ,rgain_scr ,bgain_scr ,lv_scr ,ctc_scr);
					break;
			}

	 	}
	 	else if(puiPara->VideoStampMode == UI_STAMP_DATE_TIME)
	 	{
			switch(puiPara ->DateStyle)
			{
				case DATE_TIME_MMDDYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d %02d:%02d:%02d   %d %d %d %d %d %d %d %d ",
						rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_year+1900,
						rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec,exp ,agc,appAeInfoExpidxGet() ,appAeInfoAgcidxGet()  ,rgain_scr ,bgain_scr ,lv_scr ,ctc_scr);
					break;
				case DATE_TIME_DDMMYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d %02d:%02d:%02d   %d %d %d %d %d %d %d %d ",
						rtc.tmx_mday,rtc.tmx_mon,rtc.tmx_year+1900,
						rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec,exp ,agc,appAeInfoExpidxGet() ,appAeInfoAgcidxGet() ,rgain_scr ,bgain_scr ,lv_scr ,ctc_scr);
					break;
				case DATE_TIME_YYMMDD:
				default:
					sprintf(stampDateBuf,  "%d/%02d/%02d %02d:%02d:%02d   %d %d %d %d %d %d %d %d ",
						rtc.tmx_year+1900, rtc.tmx_mon,rtc.tmx_mday,
						rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec,exp ,agc,appAeInfoExpidxGet() ,appAeInfoAgcidxGet() ,rgain_scr ,bgain_scr ,lv_scr ,ctc_scr);
					break;
			}

	 	}
	#endif
	}else{
	 	if(puiPara->VideoStampMode == UI_STAMP_DATE)
	 	{
			switch(puiPara ->DateStyle)
			{
				case DATE_TIME_MMDDYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d ", rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_year+1900);
					break;
				case DATE_TIME_DDMMYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d ",rtc.tmx_mday,rtc.tmx_mon,rtc.tmx_year+1900);
					break;
				case DATE_TIME_YYMMDD:
				default:
					sprintf(stampDateBuf,  "%d/%02d/%02d ",rtc.tmx_year+1900, rtc.tmx_mon,rtc.tmx_mday);
					break;
			}

	 	}
	 	else if(puiPara->VideoStampMode == UI_STAMP_DATE_TIME)
	 	{
			switch(puiPara ->DateStyle)
			{
				case DATE_TIME_MMDDYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d %02d:%02d:%02d ",
						rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_year+1900,
						rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec);
					break;
				case DATE_TIME_DDMMYY:
					sprintf(stampDateBuf,  "%02d/%02d/%d %02d:%02d:%02d ",
						rtc.tmx_mday,rtc.tmx_mon,rtc.tmx_year+1900,
						rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec);
					break;
				case DATE_TIME_YYMMDD:
				default:
					sprintf(stampDateBuf,  "%d/%02d/%02d %02d:%02d:%02d ",
						rtc.tmx_year+1900, rtc.tmx_mon,rtc.tmx_mday,
						rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec);
					break;
			}

	 	}
	}

 }

#define SP5K_GFX_ALIGN_WAY GFX_ALIGN_BOTTOM_RIGHT
static UINT32 stampDateBufW=0, stampDateBufH=0;
void appVideoRecStampUpdate(UINT32 first, UINT32 second)
{

	appCalibData_t *pCalib;
	pCalib = appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();
	UINT32 offsetX =0,offsetY =0;
	UINT32 strW,strH;

	first = first;
	second = second;
	
	if(puiPara->SystemDis==UI_SCDV_SYSTEM_DIS_ON)
		return;
	
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX = 20;
		offsetY = 10;
	}
	#endif
	//pCalib->aedebug =1;
	if(pCalib->aedebug == TRUE)
	{   UINT32 isoIdx = 0;
	    isoIdx = _IqGetCurrISO(_IqGetOPState());
		#if SP5K_ALGORITHM_AE
		//printf("appVideoRecStampUpdate\n");

		UINT32 exp;
		UINT32 agc;
		sp5kPreviewExpAgcGet(&exp,&agc);
		appAEALGLib_DebugtGet(&aedebug);
		sprintf(stampDateBuf,"%d %d %d %d %d %d %d %d %d",aedebug.frame_id, aedebug.LVx1000, (aedebug.aeLuma/4) \
								,(aedebug.Target/4), aedebug.flow, (aedebug.NewEvValuex1000/4) ,exp ,agc,isoIdx);

		#endif
	}
	else
	{
		sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc);
		appVideoRecStampStringGet();
	}

	sp5kGfxStringSizeGet(STR_ENCODE_ASCII, stampDateBuf, &strW, &strH);
	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_DATE_STAMP_OSD, 0, 0, 0);
	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_STAMP_PAGE_ID, &stampPage);
	sp5kGfxPageClear(stampPage, PAL_NOCOLOR);
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, offsetX, 200+offsetX, 60+offsetX, 240+offsetY, 0, 0);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 228+offsetX, offsetY,strW, strH, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, stampDrawRatioW, stampDrawRatioH, 0, 0);	
	sp5kGfxStringDraw(stampPage, stampDateBufW, stampDateBufH, SP5K_GFX_ALIGN_WAY, STR_ENCODE_ASCII, stampDateBuf);
	
	profLogPrintf(0,"ds  %s",stampDateBuf);
	#if CAM_TYPE_CVR
	UINT8 vtagstr[80];
	UINT8 buf[6];
	memset(vtagstr, 0 , sizeof(vtagstr));
	if(puiPara->G_Sensor != UI_GSENSOR_STATUS_OFF){
		strcat(vtagstr,VideoRecStampGsensorBuf);
	}

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x20, 3);
	strcat(vtagstr,buf);
	#if GPS_SUPPORT
	if(puiPara->GPS == UI_GPS_STATUS_ON)
		strcat(vtagstr,stamp_GPSSpeedBuf);
	#endif


	if(puiPara->LOGOMark == 1)
	{
		sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_ICON_4, 0, 0, 0);
		appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

		sp5kGfxStringDraw(stampPage, 0, 30, SP5K_GFX_ALIGN_WAY, STR_ENCODE_ASCII, "C");
	}

	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_DATE_STAMP_OSD, 0, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	if(puiPara->WaterMark == 1)
	{
		sp5kGfxStringDraw(stampPage, 400, 0, SP5K_GFX_ALIGN_WAY, STR_ENCODE_ASCII, vtagstr);
	}
	#endif

	if(puiPara->CdvGsensorTrigger == UI_SCDV_GSENSOR_TRIGGER_ON
		&& puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
	{
		UINT i = 0;
		UINT8 strGSensorStamp[80], strTemp[20];
		float AccelerateX, AccelerateY, AccelerateZ;
		//printf("---UI_SCDV_GSENSOR_TRIGGER_ON\n");
		appGSensorAccelerateGet(&AccelerateX, &AccelerateY, &AccelerateZ);
		strGSensorStamp[0] = '\0';
		
		sprintf(strTemp, "X:%f", AccelerateX);
		for(i = 0; i < strlen(strTemp); i ++)
		{
			if(strTemp[i] == '.')
			{
				strTemp[i + 3] = '\0';
				break;
			}
		}
		strcat(strGSensorStamp, strTemp);
		strcat(strGSensorStamp, " ");

		sprintf(strTemp, "Y:%f", AccelerateY);
		for(i = 0; i < strlen(strTemp); i ++)
		{
			if(strTemp[i] == '.')
			{
				strTemp[i + 3] = '\0';
				break;
			}
		}
		strcat(strGSensorStamp, strTemp);
		strcat(strGSensorStamp, " ");

		sprintf(strTemp, "Z:%f", AccelerateZ);
		for(i = 0; i < strlen(strTemp); i ++)
		{
			if(strTemp[i] == '.')
			{
				strTemp[i + 3] = '\0';
				break;
			}
		}
		strcat(strGSensorStamp, strTemp);		
		#if 0 //no Gsensor dateStamp 			
		//printf("strGSensorStamp = %s\n", strGSensorStamp);	
		UINT32 size_idx = puiPara->CdvVideoNormalSize;
		if(size_idx<4)
			sp5kGfxStringDraw(stampPage, 0, 50, SP5K_GFX_ALIGN_WAY, STR_ENCODE_ASCII, strGSensorStamp);	
		else
			sp5kGfxStringDraw(stampPage, 0, 20, SP5K_GFX_ALIGN_WAY, STR_ENCODE_ASCII, strGSensorStamp);
		#endif		
		
	}

	#if ICAT_WIFI
	if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
		appVideoRecOSDStampUpdate();
	}
	#else
	appVideoRecOSDStampUpdate();
	#endif

	#if ICAT_CVR_WARN_SYS_DBG
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	#if ICAT_FCWS
	if(puiPara->DAS_mode)
	{
		appFCWS_DbgMsg(stampPage);
	}
	#endif
	#if ICAT_LDWS
	if(puiPara->DAS_mode)
	{
		appLDWS_DbgMsg(stampPage);
	}
	#endif
	#endif
//	appVideoRecCustomLogoShow();
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_PAGE_SWITCH, 1);/*trigger ABbuffer*/
}

void appVideoRecStampInit(void)
{
	UINT32 stampW, stampH;
	UINT32 x = 0, y = 0, w, h;
	UINT32 width, height;
	UINT32 basew, baseh;
	uiPara_t* puiPara = appUiParaGet();
	if (fntBeenLoad)
	{
		if( stampDateBuf != NULL )
		{
			osMemFree(stampDateBuf);
			stampDateBuf = NULL;
		}
		stampDateBuf = osMemAlloc(STRING_SIZE);
		if( stampDateBuf == NULL )
		{
			return;
		}
		fntBeenLoad = 0;
	}

	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc);

	appVideoRecStampStringGet();

	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_DATE_STAMP_OSD, 0, 0, 0);
	sp5kGfxStringSizeGet(STR_ENCODE_ASCII, stampDateBuf, &stampW, &stampH);
	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);
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
	//sp5kDispPvRegionGet(&x, &y, &w, &h);	remark for dvs5m2 (app and lcm datestamp size diffirent)
	#if 0
		width = stampW * w / basew;
		height = stampH * h / baseh;
		stampPara.width = width;
		stampPara.height = height;
		
		stampPara.x =10 * w / basew + x;
		stampPara.y =10 * h / baseh + y;
		stampDrawRatioW = /*1000 */ 300* w / basew;
		stampDrawRatioH = 300 * h /baseh;
		stampPara.fpMediaVideoStampInitCb = appVideoRecStampUpdate;
		stampPara.fpMediaVideoStampSwitchCb = NULL;
	#else
		width = stampW * w / basew;
		height = stampH * h / baseh;
		
		stampDateBufW =  width;				
		stampDateBufH  =  height;	
		x=0;
		#if 0
		if(puiPara->SdvVideoNormalSize<4)
			y=h-height-80;
		else
			y=h-height-30;
		#endif
		if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
		{
		if(puiPara->SdvVideoNormalSize==0)
			y=h-height-60-10;
		else if(puiPara->SdvVideoNormalSize==1)
			y=h-height-40-5-10;
		else if(puiPara->SdvVideoNormalSize==2)
			y=h-height-40-3-5;
		else if(puiPara->SdvVideoNormalSize==3)
			y=h-height-40-3-5;
		else if(puiPara->SdvVideoNormalSize==4)
			y=h-height-20-5;
		else if(puiPara->SdvVideoNormalSize==5)
			y=h-height-20-5-5;
		else if(puiPara->SdvVideoNormalSize==6)	
			y=h-height-20-5;
		else
			y=h-height-20;
		}
		else
		{
			if(puiPara->CdvVideoNormalSize==0)
			y=h-height-60-10;
		else if(puiPara->CdvVideoNormalSize==1)
			y=h-height-40-5-10;
		else if(puiPara->CdvVideoNormalSize==2)
			y=h-height-40-3-5;
		else if(puiPara->CdvVideoNormalSize==3)
			y=h-height-40-3-5;
		else if(puiPara->CdvVideoNormalSize==4)
			y=h-height-20-5;
		else if(puiPara->CdvVideoNormalSize==5)
			y=h-height-20-5-5;
		else if(puiPara->CdvVideoNormalSize==6)	
			y=h-height-20-5;
		else
			y=h-height-20;


		}
		stampPara.width = width;
		stampPara.height = height;
		UINT8 nXoff=10;

		if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
		{
			if(puiPara->SdvVideoNormalSize==0)
				nXoff=60+3-10-10-5-5;
			else if(puiPara->SdvVideoNormalSize==1)
				nXoff=60-10-10;
			else if(puiPara->SdvVideoNormalSize==2)
				nXoff=60-10-10-5-10-5;
			else if(puiPara->SdvVideoNormalSize==3)
				nXoff=60-10-10-5-10-5;
			else if(puiPara->SdvVideoNormalSize==4)
				nXoff=30-10-3;
			else if(puiPara->SdvVideoNormalSize==5)
				nXoff=30-10-3;
			else if(puiPara->SdvVideoNormalSize==6)	
				nXoff=20;
			else
				nXoff=15;
		}
		else
		{
			if(puiPara->CdvVideoNormalSize == 0)
				nXoff=60+3-10-10-5-5;
			else if(puiPara->CdvVideoNormalSize==1)
				nXoff=60-10-10;
			else if(puiPara->CdvVideoNormalSize==2)
				nXoff=60-10-10-5-10-5;
			else if(puiPara->CdvVideoNormalSize==3)
				nXoff=60-10-10-5-10-5;
			else if(puiPara->CdvVideoNormalSize==4)
				nXoff=30-10-3;
			else if(puiPara->CdvVideoNormalSize==5)
				nXoff=30-10-3;
			else if(puiPara->CdvVideoNormalSize==6)	
				nXoff=20;
			else
				nXoff=15;

		}
		//nXoff=nXoff-10;
		stampPara.x =10 * w / basew + x+nXoff;

		stampPara.y =10 * h / baseh + y;
		stampDrawRatioW = /*1000 */ 500* w / basew;
		stampDrawRatioH = 500 * h /baseh;
		stampPara.fpMediaVideoStampInitCb = appVideoRecStampUpdate;
		stampPara.fpMediaVideoStampSwitchCb = NULL;
	#endif
	//printf("sp5kDispPvRegionGet:%d, %d, %d, %d\n", x, y, w, h);
	//printf("--SCDV--%d, %d\n", puiPara->ScdvMainMode, puiPara->CdvVideoNormalSize);
	//LCM
	//sp5kDispPvRegionGet:0, 0, 320, 240
	//296, 32, 296, 42
	//APP
	//sp5kDispPvRegionGet:0, 0, 1280, 720
	//296, 32, 1184, 128

	/* To setup the date stamp parameter to baseFW */
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_PAGE_PARAM, (UINT32)&stampPara);
	/* To setup the date stamp flow by 1, means stamp the RGB565 format to video frame buffer */
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_OPTION, SP5K_MEDIA_STAMP_RGB565);

}

void appVideoRecStampExit(void)
{
	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
}

#endif /*VIDEO_REC_STAMP*/

#if GPS_SUPPORT
extern void appGpsInfoHdrWriteEn(UINT8 en);
 void appVideoLogTagInitial(void)
 {

#define VLC_FOURCC( a, b, c, d ) \
		 ( ((UINT32)a) | ( ((UINT32)b) << 8 ) \
			| ( ((UINT32)c) << 16 ) | ( ((UINT32)d) << 24 ) )
	 UINT32 default_compatible_brands[2] = {VLC_FOURCC(' ', ' ', 't', 'q'), 0};
	 mediaVideoTags_t myTag = {0};

	 myTag.major_brand = VLC_FOURCC(' ', ' ', 't', 'q');
	 myTag.minor_version = 0;
	 myTag.compatible_brands = (UINT32*)&default_compatible_brands;
	 myTag.compatible_brands_cnt = 2;
	 myTag.udta_atom.name = VLC_FOURCC('u', 'd', 'a', 't');
	 myTag.udta_atom.update_mode = 1;
	 myTag.udta_atom.location = 1;
	 //myTag.udta_atom.nDataSize = 0;

	 myTag.pref_volume = 0x100;
		 myTag.vendor = VLC_FOURCC('t', 'a', 'c', 'i');
	 myTag.component_manuf = VLC_FOURCC('t', 'a', 'c', 'i');;
	 myTag.data_ref_type = VLC_FOURCC('a', 'l', 'i', 's');
	 myTag.tkhd_flags = 0xf;

	 myTag.time_scale = 90000;
	 myTag.video_track_matrix[0] = 0x10000;
	 myTag.video_track_matrix[4] = 0x10000;
	 myTag.video_track_matrix[8] = 0x40000000;
	 myTag.temporal_quality = 0x200;
	 myTag.spatial_quality = 0x200;
	 myTag.video_handler_comp_name ="iCatch Video Handler";
	 myTag.video_handler_ref_comp_name = "iCatch Alias Data Handler";

	 myTag.audio_compression_id = -2;
	 myTag.audio_handler_comp_name ="iCatch Audio Handler";
	 myTag.audio_handler_ref_comp_name = "iCatch Alias Data Handler";
	 sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_REC_TAGS, (UINT32)&myTag);
 }
#endif

#define DUAL_STREAM_SAMPLE_CODE (0)
#if DUAL_STREAM_SAMPLE_CODE
#define DUALSTREAM_SAMPLE_BUFCNT (8)
#define DUALSTREAM_SAMPLE_BUFSIZE (60*1024)

#define DUALSTREAM_SAMPLE_JPG_COMP_RATIO (90)

UINT8 *mpszDualStreamDstFn  = "D:\\DUAL0000.JFIF";
UINT8 *mpPatchJpgEncSyncH264BackupBuf[DUALSTREAM_SAMPLE_BUFCNT];
//UINT32 mDualStreamBackupBufAddr[DUALSTREAM_SAMPLE_BUFCNT];
UINT32 mDualStreamBackupBufSize[DUALSTREAM_SAMPLE_BUFCNT];

TX_EVENT_FLAGS_GROUP mDualStreamEvtFlag;
UINT32 mDualStreamThrdHandle;

UINT32 mDualStreamTargetW = 640, mDualStreamTargetH = 360;
UINT32 mVpvCbCnt;

sp5kGfxObj_t mObjJfif;

UINT32 mLastJfifAddr;
UINT32 mLastJfifSize;

#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )

extern UINT32 halDmaMemcpy(void *pdst,const void *psrc,UINT32 size);

void
vpvYuvCb(
	UINT32 idx,
	UINT32 pbuf,
	UINT32 width,
	UINT32 hight
)
{
	UINT32 size=0;
	UINT32 tCurMode;

	sp5kModeGet(&tCurMode);

	/*printf("mVpvCbCnt=%u.\n", mVpvCbCnt);*/
	if (tCurMode==0x23)
	{
		UINT32 ts;
		ts = sp5kOsTimeGet();

		#if (DUAL_STREAM_SAMPLE_CODE==2)
		UINT32 ret = FAIL;
		UINT32 tIdx = 0;
		UINT32 tAddr = 0;

		ret = sp5kMediaRecDualStreamGetStream(&tIdx, &tAddr, &size);
		if ((ret==SUCCESS)&& (size!=0)) {
			profLogPrintf(0, "[%u]host[idx,addr,size]=[%u, 0x%08x, %u]", mVpvCbCnt, tIdx, tAddr, size);

			mLastJfifAddr = tAddr;
			mLastJfifSize = size;

			sp5kOsEventFlagsSet(&mDualStreamEvtFlag, 0x0001, TX_OR);
		}
		else {
			printf("\nsp5kMediaRecDualStreamGetStream() return NG (0x%x)\n", ret);
		}

		#elif (DUAL_STREAM_SAMPLE_CODE==1)

		mObjJfif.pbuf = sp5kYuvBufferAlloc(width,hight); //max yuv size
		#if 0
		ASSERT(mObjJfif.pbuf);
		#else
		if (!mObjJfif.pbuf) {
			printf("\n\n##### ASSERT #####\n");
			printf("malloc mObjJfif.pbuf for (%u,%u) NG\n", width, hight);
			printf("##### FIXME while(1) #####\n");
			while(1);
			return;
		}
		#endif

		sp5kGfxObj_t objYuv={pbuf,NULL,ROUND_UP_TO_DIVISIBLE(width,16),ROUND_UP_TO_DIVISIBLE(hight,16),0,0, SP5K_GFX_FMT_YUV422, width, hight};
		objYuv.roiX = DUALSTREAM_SAMPLE_JPG_COMP_RATIO;
		mObjJfif.fmt = SP5K_GFX_FMT_JFIF;
		mObjJfif.roiX = mObjJfif.roiY = 0;
		mObjJfif.roiW = width;
		mObjJfif.roiH = hight;
		mObjJfif.bufW = mDualStreamTargetW;
		mObjJfif.bufH = mDualStreamTargetH;
		/*printf("%s Line %d.\n", __FILE__, __LINE__);*/ /*PhilipDebug*/
		sp5kGfxObjectConvert(&objYuv, &mObjJfif);
		size = mObjJfif.roiX; /* size is inside roix */
		/*printf("%s Line %d. size=%u.\n", __FILE__, __LINE__, size); *//*PhilipDebug*/

		sp5kOsEventFlagsSet(&mDualStreamEvtFlag, 0x0001, TX_OR);

		#endif /*DUAL_STREAM_SAMPLE_CODE*/

		ts = sp5kOsTimeGet() - ts;
		profLogPrintf(0, "vpvCb: %u*10", ts);
	}
	mVpvCbCnt++;
}

void
dualStreamSampleThrd(
	ULONG para
)
{
	static UINT32 tSaveVrecCnt = 0;

	ULONG  ulActualEvents;
	UINT32 tSize, tAddr;
	para = para;
	while( 1 ) {
		sp5kOsEventFlagsGet( &mDualStreamEvtFlag,
								0x0001,
								TX_OR_CLEAR,
								&ulActualEvents,
								TX_WAIT_FOREVER
								);

		#if (DUAL_STREAM_SAMPLE_CODE==2)
		tSize = mLastJfifSize;
		tAddr = mLastJfifAddr;

		#elif (DUAL_STREAM_SAMPLE_CODE==1)
		tSize = mObjJfif.roiX;
		tAddr = mObjJfif.pbuf;

		#endif

		mDualStreamBackupBufSize[tSaveVrecCnt] = tSize;

		halDmaMemcpy(mpPatchJpgEncSyncH264BackupBuf[tSaveVrecCnt], (void *)tAddr, tSize);
		//printf("tSaveVrecCnt=%u.\n", tSaveVrecCnt);
		tSaveVrecCnt++;
		if(tSaveVrecCnt>=DUALSTREAM_SAMPLE_BUFCNT){
			tSaveVrecCnt = 0;
		}

		#if (DUAL_STREAM_SAMPLE_CODE==1)
		sp5kYuvBufferFree(mObjJfif.pbuf);
		mObjJfif.pbuf = 0;
		#endif
	}


}


void
dualStreamSampleInit(
	void
)
{
	UINT k;

	printf("%s Line %d. %s()\n", __FILE__, __LINE__, __func__); /*PhilipDebug*/

	/*FIXME: assume each frame VLC takes 60 KBytes */
	for(k=0;k<DUALSTREAM_SAMPLE_BUFCNT;k++){
		mpPatchJpgEncSyncH264BackupBuf[k] = sp5kMalloc(DUALSTREAM_SAMPLE_BUFSIZE);
		HOST_ASSERT(mpPatchJpgEncSyncH264BackupBuf[k]);
	}

	sp5kOsEventFlagsCreate(&mDualStreamEvtFlag, "DualStreamEvt");
	mDualStreamThrdHandle = (UINT32)sp5kOsThreadCreate("DualStreamThrd", dualStreamSampleThrd, 0, 17, 17, 0, TX_AUTO_START);

	#if (DUAL_STREAM_SAMPLE_CODE==2)
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_WIDTH, 640);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_HEIGHT, 360);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_DUALSTREAM_Q_FACTOR, DUALSTREAM_SAMPLE_JPG_COMP_RATIO);

	sp5kMediaRecCfgSet(MEDIA_REC_DUALSTREAM_BUF_SIZE, (50*1024));
	sp5kMediaRecCfgSet(MEDIA_REC_DUALSTREAM_BUF_CNT, (10));
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DUALSTREAM_JFIF_EN, 1);
	#endif

	#if (DUAL_STREAM_SAMPLE_CODE==2)
	/*30fps*/
	sp5kVideoUrgentCallbackSet(1 /*interval*/, vpvYuvCb/*pfunc*/);
	#elif (DUAL_STREAM_SAMPLE_CODE==1)
	/*15fps*/
	sp5kVideoUrgentCallbackSet(2 /*interval*/, vpvYuvCb/*pfunc*/);
	#endif

}

void
dualStreamSampleDeinit(
	void
)
{
	UINT32 ulActualEvents;
	UINT32 k;

	printf("%s Line %d. %s()\n", __FILE__, __LINE__, __func__); /*PhilipDebug*/

	sp5kVideoUrgentCallbackSet(0, NULL);

	#if 0
	sp5kOsEventFlagsGet( &mDualStreamEvtFlag,
							0x0001,
							TX_OR_CLEAR,
							&ulActualEvents,
							TX_WAIT_FOREVER
							);
	#endif

	sp5kOsThreadDelete((TX_THREAD *)mDualStreamThrdHandle);
	mDualStreamThrdHandle = 0;

	sp5kOsEventFlagsDelete(&mDualStreamEvtFlag);
	mDualStreamEvtFlag = 0;

	printf("Dump %u files, please wait...\n", DUALSTREAM_SAMPLE_BUFCNT);
	#if 1
	for (k=0; k<DUALSTREAM_SAMPLE_BUFCNT; k++) {
		fsSimpleWrite(mpszDualStreamDstFn, mpPatchJpgEncSyncH264BackupBuf[k], mDualStreamBackupBufSize[k]);

		/**
		Handle filename
		*/
		if (mpszDualStreamDstFn[10]!='9') {
			mpszDualStreamDstFn[10]++;
			}
			else {
				mpszDualStreamDstFn[10]='0';
				if (mpszDualStreamDstFn[9]!='9') {
					mpszDualStreamDstFn[9]++;
				}
				else {
					mpszDualStreamDstFn[9]='0';
					if (mpszDualStreamDstFn[8]!='9') {
						mpszDualStreamDstFn[8]++;
					}
					else {
					mpszDualStreamDstFn[8]='0';
					if (mpszDualStreamDstFn[7]!='9') {
						mpszDualStreamDstFn[7]++;
					}
					else {
						mpszDualStreamDstFn[7]='0';
					}
				}
			}
		}
	}
	#else
	fsSimpleWrite(mpszDualStreamDstFn, (void *)mLastJfifAddr, mLastJfifSize);

	printf("\"%s\". <filename)\n", mpszDualStreamDstFn);
	printf("%u. <total frame cnt>\n", DUALSTREAM_SAMPLE_BUFCNT);
	printf("<frameIdx, frameOffset, frameSize>\n");
	for (k=0; k<DUALSTREAM_SAMPLE_BUFCNT; k++) {
		printf("%02u, 0x%08x, %u.\n", k, (UINT32)mpPatchJpgEncSyncH264BackupBuf[k], mDualStreamBackupBufSize[k]);
	}
	#endif
	printf("Dump %u files complete\n", DUALSTREAM_SAMPLE_BUFCNT);
	for (k=0; k<DUALSTREAM_SAMPLE_BUFCNT; k++){
		sp5kFree(mpPatchJpgEncSyncH264BackupBuf[k]);
		mpPatchJpgEncSyncH264BackupBuf[k] = NULL;
	}
	sp5kVideoUrgentCallbackSet(1 , NULL);

}
#endif /*DUAL_STREAM_SAMPLE_CODE*/


void appVideoSeamlessGPSFileDel(void)
{
	UINT8 sCurrentName[32],tmpTxTBuff[32];
	AppVideoRecFirstIndexFileNameGet(sCurrentName, tmpTxTBuff);
	DBG_PRINT("DEL GPS 1st Index file=%s\n   %s\n   %s\n", sCurrentName ,tmpTxTBuff ,sFirstTXTfName);
 	if(strcmp(sFirstTXTfName, tmpTxTBuff))
 	{
		printf("del name=%s\n",sFirstTXTfName);
		sp5kFsFileDelete(sFirstTXTfName);
		memset(sFirstTXTfName ,0 ,32);
		strcpy(sFirstTXTfName , tmpTxTBuff );
		printf("First file name=%s\n",sFirstTXTfName);
 	}
}

UINT8 appEventLockTrigger(
	UINT8 sel,
	SINT16 x_axis,
	SINT16 y_axis,
	SINT16 z_axis
)
{
	UINT8 ret = FALSE;

	switch(sel)
	{
		case UI_GSENSOR_STATUS_LOW:
			if(x_axis > 512 || x_axis < -512 || y_axis > 512 || y_axis < -512
				|| z_axis > 512 || z_axis < -512)
				ret= TRUE;
			break;
		case UI_GSENSOR_STATUS_NORMAL:
			if(x_axis > 384 || x_axis < -384 || y_axis > 384 || y_axis < -384
				|| z_axis > 384 || z_axis < -384)
				ret= TRUE;
			break;
		case UI_GSENSOR_STATUS_HIGH:
			if(x_axis > 256 || x_axis < -256 || y_axis > 256 || y_axis < -256
				|| z_axis > 256 || z_axis < -256)
				ret= TRUE;
			break;
		default:
			break;
	}

	return ret;
}

void
DoVedioPauseControl(void)
{
	UINT32 elsTime;
	appCalibData_t *pCalib;
	pCalib = appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();

	if (VideoRecStatus == VIDEO_REC_STATUS_RECORDING )
	{
		if (gVideoCB.CapSec<2)
		{
			VideoRecPaReMsgReceive =TRUE;
			return;
		}
		LED_NORMAL_ON;
		appTimerClear(&gVideoCB.CapTimer);
		gVideoCB.fPause = 1;
		VideoRecStatus = VIDEO_REC_STATUS_PAUSE;
		sp5kMediaRecControl(SP5K_MEDIA_REC_PAUSE);
		DBG_PRINT("Video rec msg: SP5K_MEDIA_REC_PAUSE\n");
		appViewOsd_VideoRecControlDraw(VIDEO_REC_RECORDING);

	}
	else
	{
		LED_FLICKER_VIDEO_RECORDING;
		if(pCalib->aedebug == TRUE)
		gVideoCB.CapTimer = appTimerSet(100, "CapVid");
		else
		gVideoCB.CapTimer = appTimerSet(500, "CapVid");

		gVideoCB.fPause = 0;
		VideoRecStatus = VIDEO_REC_STATUS_RECORDING;
		sp5kMediaRecControl(SP5K_MEDIA_REC_RESUME);
		DBG_PRINT("Video rec msg: SP5K_MEDIA_REC_RESUME\n");
		appViewOsd_VideoRecControlDraw(VIDEO_REC_PAUSE);
		// show the real recording time.
		if(puiPara->VideoStampMode){

		}else{
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
			gVideoCB.CapSec = (elsTime+500)/1000;
			appVideoOsdElsTimeShow();
		}
	}
};

void
appVideoStop(void)
{
	DBG_PRINT("video stop\n");
        appCalibData_t *pCalib;
        pCalib = appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();

	sp5kAeModeSet(SP5K_AE_MODE_OFF);
    sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
	VideoRecStatus = VIDEO_REC_STATUS_CLOSE;

	#if GPS_SUPPORT
	if(puiPara->MediaFormat == MEDIA_FORMAT_MOV/* &&  GPS on && G-senor on*/){
		printf("GPS wirte end\n");
		appUsrDataWriteEnd();
	}
	#endif
	appTimerClear(&gVideoCB.CapTimer);

	gsensorTriggerRecordingStateFlag = 0;
	gsensorDelayRecTimes = 0;
	gsensorRecEventFlag = 0;
	alreadyRecFileFlag = 0;
	countDownRecFlag = 0;

	/*Open PIP/PF/VCT layer since inital close those layer*/
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_VCT_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0,1, 0, 0);

	/* DIST + RSC OFF */ /* add by sc.liu */
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL,MEDIA_REC_RSC_DIS_OFF);

	#ifdef VIDEO_REC_STAMP
	if(puiPara->VideoStampMode)
	{
		//appVideoSeamlessGPSFileDel();
		//App_videoGPSDataSave();

		#if CVR_WARN_SYS_INT_DBG
		videoSeamlessFrameId=0;
		//appVideoLogFileSave();
		appVideoLogFileInit();
		#endif

		if(pCalib->aedebug == TRUE)
		{
			//appVideoLogFileSave();
    		appVideoLogFileInit();
		}

		//appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0,30, 30, 320, 60);
		/*appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_0,0, 30, 320, 240);*/  /*mask for mantis bug 0047302*/
		appVideoRecStampExit();
	}
	#endif
	printf("begin s\n");
	callStack();
	appVideo_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	printf("mode s\n");
	appModeSet(SP5K_MODE_VIDEO_PREVIEW);
	printf("mode e\n");
	//sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	//appHostMsgWait(SP5K_MSG_MODE_READY,NULL,20000);
	//VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
	#if ICAT_WIFI
	if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
		CLEAR_OSD_SRC;
	}
	#endif
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	printf("mode1 s\n");
#if SP5K_ALGORITHM_AWB
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif
	printf("mode2 e\n");

	#if SP5K_CDFS_OPT
	if(appCdfsAddFileStsGet())
	{
	appCdfsFileAdd(appCdfsLastAviNameGet());
	}
    #endif
	#if ICAT_LDWS
	if(puiPara->DAS_mode){
		sp5kVideoUrgentCallbackSet(4,NULL);
	}
	#endif

	#if DUAL_STREAM_SAMPLE_CODE
	dualStreamSampleDeinit();
	#endif

	/*if open datestamp,the scale factor had be set to stampDrawRatioW&stampDrawRatioH, must reset it */
	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
}

static void _videoLensStopCBF(void)
{
	appAudioRecVolumeRestore();
}


#if 0	/* ADD BY SC.LIU */
void appVideoRecCustomLogoShow(
	void
)
{
	sp5kGfxIconFileDraw(stampPage, 0, 0,SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TK_CUSTOMLOGO, 0x0000);
	sp5kGfxIconFileDraw(SP5K_GFX_PAGE_PIP_0, 0, 30,SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TK_CUSTOMLOGO36, 0x0000);
}
#endif

#if SPCA6330
void appCaptureInMovie(
	void
)
{	/* ADD BY SC.LIU */
	/*sp5kMediaRecControl(SP5K_MEDIA_REC_FRAME_CAPTURE);*/
	sp5kMediaRecControl(SP5K_MEDIA_REC_JFIF_CAPTURE);

}
#endif
void appVideoRecManulStopSet(BOOL manualStop)
{
#if VIDEO_REC_ALWAYS
	vidManualStop = manualStop;
#endif
}

BOOL appVideoRecManulStopGet(void)
{
#if VIDEO_REC_ALWAYS
	return vidManualStop;
#else
	return FALSE;
#endif
}
void appVideoRecAlwaysRestart(BOOL restart)
{
#if VIDEO_REC_ALWAYS
	vidRecRestart = restart;
#endif
}


BOOL appVideoRecAlwaysRestartGet()
{
	#if VIDEO_REC_ALWAYS
	if(appVideoRecManulStopGet())/*when manual stop record,we should not restart record auto*/
	{
		return FALSE;
	}
	else
	{
		return vidRecRestart;
	}
	#else
	return FALSE;
	#endif
}
void appVideoRecAlwaysMenuMsgProc(
	UINT32 msg,
	UINT32 param
)
{
	#if VIDEO_REC_ALWAYS
	uiPara_t* puiPara = appUiParaGet();

	switch ( msg) {
		case SP5K_MSG_MEDIA_REC_DROP_FRAME:
			printf("MenuMsgProc: MEDIA_REC_DROP_FRAME param=%x\n",param & 0xf0000000 );
			//Ethan if drop frame stop rrecording (H624)
			appVideoRecLogFileAdd(msg,param);
			if(((param & 0xF0000000) == 0x90000000)||((param & 0xF0000000) == 0x10000000)){
				break;
			}else{
				__FUNC_TRACK__;
				appVideoStop();
				appMenu_MenuExit(0);
			}
			break;
		case SP5K_MSG_MEDIA_FILE_SIZE_NOTIFY:
			printf("MenuMsgProc: MEDIA_BETTER_FILE_SIZE\n");
			if (param == 0x4){
				SnapSetflag =1;
				__FUNC_TRACK__;
				appVideoStop();
				appMenu_MenuExit(0);
			}
			break;
		case SP5K_MSG_MEDIA_AVI_REC_FPS_LOW://happen with slow card and enough space(>2G)
			printf("MenuMsgProc: MEDIA_AVI_REC_FPS_LOW\n");
			break;
		case SP5K_MSG_MEDIA_REC_DISK_FULL:
			printf("MenuMsgProc: MEDIA_REC_DISK_FULL\n");
			if(puiPara->Seamless)
			{
				break;
			}
		case SP5K_MSG_DISK_ERROR:
			printf("MenuMsgProc: SP5K_MSG_DISK_ERROR\n");
			appTimerClear(&gVideoCB.CapTimer);
			__FUNC_TRACK__;
			appVideoStop();
			VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
			gVideoCB.err = VIEW_ERR_MEMORY_FULL;
			appMenu_MenuExit(0);
			break;
	}
	#endif
}
void appVideoRecAlwaysMenuEnter(void)
{
	#if VIDEO_REC_ALWAYS
	/*open PIP/PF/VCT layer before enter menu*/
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_VCT_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0,1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0); /*add for mantis bug 0048961*/
	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0); /*add for mantis bug 0048984*/
	appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
	#endif
}
UINT32 appVideoRecAlwaysDo(void)
{
	UINT32 nextSts;
	UINT32 mode ;
	sp5kModeGet(&mode);

	switch(appActiveStateGet())
	{
		case APP_STATE_VIDEO_PREVIEW:
			#if VIDEO_REC_ALWAYS
			if(appVideoRecAlwaysRestartGet())
			{
				appVideoRecAlwaysRestart(0);
				sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
				sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
			}
			#endif
			break;
		case APP_STATE_MENU:
			#if VIDEO_REC_ALWAYS
			if(appPreviousStateGet()==APP_STATE_VIDEO_REC||appPreviousStateGet()==APP_STATE_MENU
				|| (appPreviousStateGet()==APP_STATE_VIDEO_SYSTEM && mode==SP5K_MODE_VIDEO_RECORD))
			{
				nextSts = APP_STATE_VIDEO_REC;
			}
			else
			#endif
			{
				nextSts = APP_STATE_VIDEO_PREVIEW;
			}
			break;
		case APP_STATE_VIDEO_REC:
			#if VIDEO_REC_ALWAYS
			if(appPreviousStateGet()==APP_STATE_MENU)
			{
				appViewOsd_VideoRecControlDraw(VIDEO_REC_PAUSE);
				if(appCdfsEventFlagGet()&VIDEO_EVENT_IN_VIDEO)
				{
					appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
				}
				printf("\033[35m back to video record \033[0m\n");
				 /*Close PIP/PF/VCT layer when video recording*/
				sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_0, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_VCT_0, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0,0, 0, 0);
			}
			else
			#endif
			{
				appVideoRecStateInit();
			}
			break;
		default:
			#if VIDEO_REC_ALWAYS
			nextSts = APP_STATE_VIDEO_PREVIEW;
			#else
			nextSts = APP_STATE_STILL_PREVIEW;
			#endif
			break;
	}
	return nextSts;
}

UINT32 appSeamlessRecTimeGet(UINT8 idx)
{
	UINT32 time = 0;

	switch(idx)
	{
		case UI_VIDEO_SEAMLESS_1MIN:
			time = 60;
			break;
		case UI_VIDEO_SEAMLESS_3MIN:
			time = 180;
			break;
		case UI_VIDEO_SEAMLESS_5MIN:
			time = 300;
			break;
		case UI_VIDEO_SEAMLESS_OFF:
		default:
			break;
	}

	return time;
}

#if CAM_TYPE_CVR
void appGsensorTriggerVideoRecProc(void)
{
	UINT8 dst[32];
	UINT8 tmp_path[32];
	UINT32 time;
	uiPara_t* puiPara = appUiParaGet();

	if(gsensorTriggerByIntFlag == 100)
	{
		time = appSeamlessRecTimeGet(puiPara->Seamless) / 3;

		if((gVideoCB.CapSec >= 1) && (gVideoCB.CapSec <= time))
		{
			printf("---Gsensor Trigger In Seamless Rec 1/3---\n");
			printf("already:%d,skip:%d---\n",alreadyRecFileFlag,cutSkipFlag);
			if(alreadyRecFileFlag == 1 && cutSkipFlag == 0)
			{
				alreadyRecFileFlag = 0;
				memset(tmp_path, 0, 32);
				strcpy(tmp_path, eventLockFileName);

				appCdfsFileDel(tmp_path);
				appCdfsAbsolutePathToFile(tmp_path);	//get the short file name
				appCdfsEventProcess(tmp_path, VIDEO_EVENT_IN_VIDEO);

				memset(dst, 0, 32);
				appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT, dst);
				sprintf(dst, "%s/%s", dst,tmp_path);

				appCdfsFileFolderSet(CDFS_FILE_FOLDER_EVENT);
				appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
				appCdfsFileAdd(dst);
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
				appCdfsEventFlagClean(VIDEO_EVENT_IN_VIDEO);
			}

			cutSkipFlag = 2;
			appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
		}
		else if((gVideoCB.CapSec > time) && (gVideoCB.CapSec <= time*2))
		{
			printf("---Gsensor Trigger In Seamless Rec 2/3---\n");
			cutSkipFlag = 2;
			appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
		}
		else if((gVideoCB.CapSec > time*2) && (gVideoCB.CapSec <= time*3))
		{
			printf("---Gsensor Trigger In Seamless Rec 3/3---\n");
			cutSkipFlag = 3;
			gsensorRecEventFlag = 1;
			appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
		}

		gsensorTriggerByIntFlag = 101;
	}

}

void appCountDownRecProc(void)
{
	UINT32 tmpTime;
	uiPara_t* puiPara = appUiParaGet();

	if(countDownRecFlag == 1)
	{
		if(puiPara->Seamless == UI_VIDEO_SEAMLESS_OFF)
		{
			if(gsensorDelayRecTimes)
			{
				gsensorDelayRecTimes--;
				printf("Seamless Off-%d\n", gsensorDelayRecTimes);
				if(gsensorDelayRecTimes == 0)
				{
					sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);	//stop rec
					if(gsensorTriggerByIntFlag == 1)
					{
						gsensorTriggerByIntFlag = 0;
						powerOffInVideoRec = 1;
					}
					else if(gsensorTriggerByIntFlag == 2)
					{
						/* keep on recording when seamless off */
						gsensorTriggerByIntFlag = 3;
					}
				}
			}
		}
		else
		{
			if(gVideoCB.CapSec + gsensorDelayRecTimes <= appSeamlessRecTimeGet(puiPara->Seamless))
			{
				if(gsensorDelayRecTimes)
				{
					gsensorDelayRecTimes--;
					printf("Seamless On-%d\n",gsensorDelayRecTimes);
					if(gsensorDelayRecTimes == 0)
					{
						sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
						if(gsensorTriggerByIntFlag == 1)
						{
							gsensorTriggerByIntFlag = 0;
							powerOffInVideoRec = 1;
						}
						else if(gsensorTriggerByIntFlag == 2)
						{
							gsensorTriggerByIntFlag = 0;
						}
					}
				}
			}
			else
			{
				tmpTime = appSeamlessRecTimeGet(puiPara->Seamless);
				sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT, tmpTime + gsensorDelayRecTimes);
				countDownRecFlag = 2;
			}
		}
	}

	if(countDownRecFlag == 2)
	{
		gsensorDelayRecTimes--;
		if(gsensorDelayRecTimes == 0)
		{
			sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT, appSeamlessRecTimeGet(puiPara->Seamless));

			if(gsensorTriggerByIntFlag == 1)
			{
				gsensorTriggerByIntFlag = 0;
				appPowerOffUiMsgSend(APP_POWER_OFF_APO);
			}
			else if(gsensorTriggerByIntFlag == 2)
			{
				gsensorTriggerByIntFlag = 0;
			}
		}
	}
}
#endif
extern void appSdvVideoRecTimeOsdShow(void);

void
appVideoRecStateInit(void)
{
	UINT32 mode;
	/*UINT8 fname[32];*/
	appCalibData_t *pCalib;

	/*xuan.ruan@20131120 add for mantis bug 0048819*/
	UINT32 cfg;
	sp5kSystemCfgGet(SP5K_DISK_SD_HW_PIN_CFG, &cfg);
	/* set SD driving to high 6mA -> 8mA*/
	sp5kSystemCfgSet(SP5K_DISK_SD_HW_PIN_CFG, cfg | 0x02);

	pCalib = appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();

	sp5kModeGet(&mode);

	#if GPS_SUPPORT
	if(puiPara->MediaFormat == MEDIA_FORMAT_MOV/* &&  GPS on && G-senor on*/)
	{
		BPrintf("appVideoLogTagAdd from appVideoSettingInit");
         	appGpsInfoHdrWriteEn(1);
		appGpathSave();
		appVideoLogTagInitial();
	}
	#endif

	#ifdef NO_VIDEO_PREVIEW_MODE
	while (mode!= SP5K_MODE_STILL_PREVIEW)
	#else
	while (mode!= SP5K_MODE_VIDEO_PREVIEW)
	#endif
	{
		APP_SLEEP_MS(20);
		sp5kModeGet(&mode);
	}
	gVideoCB.CapTimer = appTimerSet(500, "CapVid");
	#if CAM_TYPE_CVR
	if(puiPara->G_Sensor != UI_GSENSOR_STATUS_OFF){
		GSensorTimer = appTimerSet(200,"Gsensor");
		GSensorReadFlag = 0;
		memset(GSensorInfo,0,sizeof(GSensorInfo));
	}
	#endif

	#if ICAT_LDWS
	extern UINT8 gVideoSizeTag;
	if(puiPara->DAS_mode){
		if (appVidFrameRateGet(gVideoSizeTag) == 30) {
			sp5kVideoUrgentCallbackSet(1, appCvrWarningSystemFrameProc);
		}
		else {
			sp5kVideoUrgentCallbackSet(2, appCvrWarningSystemFrameProc);
		}
	}
	#endif
	#if CVR_WARN_SYS_INT_DBG
	appVideoLogFileInit();
	#endif

	#ifdef VIDEO_REC_STAMP
	if(puiPara->VideoStampMode)
	{
		appVideoRecStampInit();
	}
	#endif

	if(pCalib->aedebug == TRUE)
		appVideoLogFileInit();

	appPreloadSoundAbort();
	appLensStopCallbackSet(_videoLensStopCBF);
	appExif_SetAviChunk();
	LED_FLICKER_VIDEO_RECORDING;
	#if 0/* NEW solution use jfif only SPCA6330*/
	if(puiPara->Seamless == UI_VIDEO_SEAMLESS_OFF){		/* ADD BY SC.LIU */
		appStillInMovieInit();
	}
	#endif

	#if DUAL_STREAM_SAMPLE_CODE
	dualStreamSampleInit();
	#endif
    /*Close PIP/PF/VCT layer when video recording*/
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_0, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_VCT_0, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0,0, 0, 0);

	appModeSet(SP5K_MODE_VIDEO_RECORD);

	appBtnEnable(BTN_PB|BTN_MODE);
	#ifdef VIDEO_REC_STAMP
	if(puiPara->VideoStampMode)
	{
		AppVideoRecFirstIndexFileNameGet(sFirstfName , sFirstTXTfName);
		AppVideoSeamlessNextFileNameGet(sCurrentFName);
		ViewOsdPresentStatus=0;
	}else{
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			appViewOsd_AllClean();
			if(countDownRecFlag && gsensorTriggerByIntFlag && (puiPara->Seamless!= UI_GSENSOR_STATUS_SPORT))
			{
				appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
			}
		}
		#else
		appViewOsd_AllClean();
		if(countDownRecFlag && gsensorTriggerByIntFlag && (puiPara->Seamless!= UI_GSENSOR_STATUS_SPORT))
		{
			appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
		}
		#endif
	}
	#endif
	appAePreviewRestore();
	appVideo_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	gVideoCB.fCapture=1;
	gVideoCB.fPause = 0;
	gVideoCB.CapSec = 0;
	gVideoCB.fStop = 0;
	/*gVideoCB.CapTimer = appTimerSet(1000, "CapVid"); */
	VideoRecStatus = VIDEO_REC_STATUS_RECORDING;

	if(puiPara->VideoStampMode == 0)
	{
		sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
 			appViewOsd_VideoRecControlDraw(VIDEO_REC_PAUSE);
 		}
 		#else
		appViewOsd_VideoRecControlDraw(VIDEO_REC_PAUSE);
 		#endif
	}
	else{
		appViewOsd_VideoRecControlDraw(VIDEO_REC_PAUSE);
//		appVideoOsdElsTimeShow();
	}
	appPowerSavingEnableSet(FALSE);

	if(puiPara->VideoFaceTracking)
	{
		#if ICAT_LDWS
		if(puiPara->DAS_mode){
			appVideo_SetFaceTracking(VIEW_FT_STATUS_RUN);
		}
		else
		{
			appVideo_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
		}
		#else
		appVideo_SetFaceTracking(VIEW_FT_STATUS_RUN);
		#endif
	}
	else
	{
		appVideo_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	}

	gVideoCB.isAutoPowerOffOn=0;
}


void
appVideoRecStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 elsTime;
	UINT32 tmpU32;
	SINT16 tmp16_x,tmp16_y,tmp16_z;

	extern UINT32 appAeInfoExpidxGet();
	extern UINT32 appAeInfoAgcidxGet();

   	appCalibData_t *pCalib;
	pCalib = appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();
#if EZ_DBG_HW
	static UINT32 batAdcCheck=0;
#endif
	#if CVR_WARN_SYS_INT_DBG
	vehicleDetectGetInfo_t vdInfor;
	#endif

	switch ( msg) {
	case SP5K_MSG_MODE_READY:
		if(param == 0x21 && gVideoCB.err == VIEW_ERR_MEMORY_FULL )
		{
			appTimerClear(&gVideoCB.CapTimer);
			#if 1
			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
			#else
			printf("SP5K_MSG_MODE_READY B\n");

			sp5kMediaRecControl(SP5K_MEDIA_REC_ABORT);
			VideoRecStatus = VIDEO_REC_STATUS_ABORT;
			#endif
			gVideoCB.err = VIEW_ERR_MEMORY_FULL;
			#if ICAT_WIFI
			if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
				appVideoOsdShow(0,0,gVideoCB.osd);
			}
			#else
			appVideoOsdShow(0,0,gVideoCB.osd);
			#endif
    	}
    	else
    	{
		    DBG_PRINT("mode ready: 0x%x\n", param);
    	}
		break;
	case SP5K_MSG_MEDIA_REC_START:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{
			DBG_PRINT("__OmMsg: REC_START(OKAY)\n");
		}
		else
		{
			DBG_PRINT("__OmMsg: REC_START(FAIL) !!\n");
			appTimerClear(&gVideoCB.CapTimer);
			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
			gVideoCB.err = VIEW_ERR_TO_POWER_OFF;
		}
		break;

	case SP5K_MSG_MEDIA_REC_STOP:
		printf("SP5K_MSG_MEDIA_REC_STOP:0x%x\n",param);
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{
			if (VideoRecStatus == VIDEO_REC_STATUS_CLOSE )
			{
				appTimerClear(&gVideoCB.CapTimer);
				if(gVideoCB.err == VIEW_ERR_TO_POWER_OFF)
				{
					DBG_PRINT("_OnMsg: power-off\n");
					appPowerOffUiMsgSend(APP_POWER_OFF_VIDEO_REC_ERROR);
				}
				else if(powerOffInVideoRec)
				{
					appStateChange(APP_STATE_POWER_OFF, APP_POWER_OFF_NORMAL);
				}
				else
				{
					if(appNextStateGet() !=APP_STATE_NULL)
					 {
						appTimerClear(&gVideoCB.CapTimer);
						appStateCloseDone();
					 }
					 else
					 {
						appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
					 }
				}
			}
		}
		else
		{
			printf("__OmMsg: REC_STOP(FAIL) !!\n");
			/*-- There wont have this msg out of middleware.
			 * Instead, middleware will send SP5K_MSG_DISK_ERROR.
			 */
		}
		break;
	/* view handling */
	case SP5K_MSG_AE_READY:
        #if SP5K_MOTION_DETECT
		if(puiPara->MotionDetect == 1)
		{
			appMotionDetectHdr();
		}
        #endif
		#if SP5K_ALGORITHM_AE
		#else
		appAePreviewAdjust(param);
		break;
		#endif
	case SP5K_MSG_AWB_READY:
		appAwbPreviewAdjust(param);
		break;
	case SP5K_MSG_MEDIA_SEAMLESS_CHANGE_FILE:
		gsensorTriggerRecordingStateFlag = 0;
		cutSkipFlag--;
        #if CVR_WARN_SYS_INT_DBG
		videoSeamlessFrameId = 0;
		AppVideoSeamlessSetNextDCFKey();
		#endif
		break;
	#if ICAT_LDWS
	case SP5K_MSG_LANE_SHIFT_WARNING:
		appLaneDepartureWarningUpdate(param);
		break;
	#endif
   // case SP5K_MSG_MEDIA_SEMALESS_CNT_NOTIFY :
	case SP5K_MSG_TIMER:
	    #if CVR_WARN_SYS_INT_DBG
		appVideoLogFile_Print("Id: %d, B:%d, L:%d, R:%d\n", videoSeamlessFrameId, ldwsBothCnt, ldwsRightCnt, ldwsLeftCnt);
		#endif

		#if ICAT_WIFI
		{
			extern UINT32 WiFiConnectingTimer;
			if(param == WiFiConnectingTimer){
				//appLedOff(APP_LED_GREEN);
				//appLedFlickerSet(APP_LED_ORANGE,500);
				appWiFiConnectionStateOnMsg(msg,param);
			}
		}
		#endif

		#if SP5K_MOTION_DETECT
		if(param == mdTime)
		{
			if(mdTime != TIMER_NULL)
			{
				appTimerClear(&mdTime);
			}
			__FUNC_TRACK__;
			appVideoStop();
		}
		#endif
		#if 0 /*mark by jj*/
		if (param == timer){
 			appErrMsgTimeISRWithSound(ERR_MSG_CAPTURE_DONE,ERR_MSG_ONLY, 1000);
			appTimerClear(&timer);
			getTimer=TRUE;
		}
		#endif
		if (param == gVideoCB.CapTimer)
		{
			#if EZ_DBG_HW
			if(batAdcCheck){
				UINT32 batVal=0,curtime=0;

				batVal = appBatteryAdcValueGet();
				curtime = sp5kMsTimeGet();
				printf("[BAT:Time:%5d:Value:%4d]\n",curtime/1000,batVal);
				batAdcCheck=0;
			}else{
				batAdcCheck=1;
			}
			#endif

			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
			//printf("elsTime: %d   %d\n", elsTime ,rtc.tm_year);

			if(pCalib->aedebug == TRUE)
			{
				UINT8 i=0;
				#if SP5K_ALGORITHM_AE
				appAEALGLib_DebugtGet(&aedebug);

				//printf("<lester> show stamp = %d\n",aedebug.frame_id);

				for(i=0;i<aedebug.region_num;i++)
				{
					if(i==0)
						appVideoLogFile_Print("LV=%d HC=%d LC=%d exp=%d agc=%d \n",aedebug.LVx1000,aedebug.HighLightCnt,aedebug.LowLightCnt,appAeInfoExpidxGet() ,appAeInfoAgcidxGet());

					appVideoLogFile_Print("id=%d,idx%d= %d ,%d ,%d ,\n",aedebug.frame_id, i,aedebug.AERegionLuma[i], aedebug.WeightLuma[i], aedebug.DnyAERegionLuma[i]);

					if(i==(aedebug.region_num-1))
						appVideoLogFile_Print("\n");
				}
				#endif
			}

			elsTime = (elsTime+500)/1000;
			if(gVideoCB.CapSec != elsTime)
			{
				APP_OSD_REFRESH_OFF;  /*add for mantis bug 0046600*/
				#ifdef VIDEO_REC_STAMP
				if(puiPara->VideoStampMode)
				{
					/*if(elsTime%15 ==0)
					{
					appVideoSeamlessGPSFileDel();
					}*/
					sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc);
					appVideoRecStampUpdate(1, 1);
					sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);/*junqiong.hu@20130104 add for mantis bug 0046617*/
					sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
				}
				#endif
				gVideoCB.CapSec = elsTime;
				appSdvVideoRecTimeOsdShow();
				if(puiPara->VideoStampMode == 0)
				{
					sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
					#if ICAT_WIFI
					if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
						appVideoOsdElsTimeShow();
					}
					#else
					appVideoOsdElsTimeShow();
					#endif
				}

				if(pCalib->aedebug != TRUE)
				{
					if(gVideoCB.CapSec%2==1)
					{
						#if ICAT_WIFI
						if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
							appViewOsd_VideoRecIconDraw(TRUE);
						}
						#else
						appViewOsd_VideoRecIconDraw(TRUE);
						#endif
					}
					else
					{
						#if ICAT_WIFI
						if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
							appViewOsd_VideoRecIconDraw(FALSE);
						}
						#else
						appViewOsd_VideoRecIconDraw(FALSE);
						#endif
					}
				}

				#if GPS_SUPPORT
				if(puiPara->MediaFormat == MEDIA_FORMAT_MOV/* &&  GPS on && G-senor on*/){
					appGpsWrite2TxtEn(1);
					appUsrDataWrite();
					memset(GSensorInfo,0,sizeof(GSensorInfo));
				}
				#endif
				GSensorReadFlag = 0;

				#if CAM_TYPE_CVR
				appGsensorTriggerVideoRecProc();
				appCountDownRecProc();
				#endif
				APP_OSD_REFRESH_ON;
			}
			//This param means file size is up to 4GB,now the param is meant which timer coming,so mask it
			#if !CAM_TYPE_CVR
			if((appVideoRemainSecGet()<1 && !puiPara->Seamless) || (param == 4))
			{
				__FUNC_TRACK__;
				appVideoStop();
			}
			#endif
			// avoid the pb key interrupt video record less than one second.
			if(gVideoCB.CapSec == 2)
			{
				appBtnEnable(BTN_PB);
			}
			/*printf("video msg: TIMER: elsTime=%d, CapSec=%d (dif=%d)\n", elsTime,gVideoCB.CapSec,gVideoCB.CapSec*1000 - elsTime);*/
			// this is for s1-press before record time greater than one.
			if(gVideoCB.fStop == 1 && gVideoCB.CapSec>1
				&& !puiPara->Seamless)
			{
				DBG_PRINT("Seamless recording stop\n");
				__FUNC_TRACK__;
				appVideoStop();
			}
		}
		#if CAM_TYPE_CVR
		else if(param == GSensorTimer)
		{
			if(GSensorReadFlag < GSENSOR_DATA_CNT)
			{
				appGsensorXYZRead(&tmp16_x, &tmp16_y, &tmp16_z);
				if(appEventLockTrigger(puiPara->G_Sensor, tmp16_x, tmp16_y, tmp16_z) == TRUE)
				{
					if(gsensorTriggerRecordingStateFlag == 0)
					{
						RPrintf("G-sensor Trigger Rec\n");
						sp5kHostMsgSend(APP_UI_MSG_GSENSOR_REC, 3);
					}
				}

				if(tmp16_x & 0x8000)
				{
					//x
					tmp16_x = (-tmp16_x);
					if(tmp16_x/1000!=0)
					tmp16_x=tmp16_x/10;
					tmp16_x = -tmp16_x;

					//y
					tmp16_y = (-tmp16_y);
					if(tmp16_y/1000!=0)
					tmp16_y=tmp16_y/10;
					tmp16_y = -tmp16_y;


					//z
					tmp16_z = (-tmp16_z);
					if(tmp16_z/1000!=0)
					tmp16_z=tmp16_z/10;
					tmp16_z = -tmp16_z;
				}
				else
				{
					//x
					if(tmp16_x/1000!=0)
					tmp16_x=tmp16_x/10;

					//y
					if(tmp16_y/1000!=0)
					tmp16_y=tmp16_y/10;

					//z
					if(tmp16_z/1000!=0)
					tmp16_z=tmp16_z/10;
				}

				GSensorInfo[GSensorReadFlag][0] = tmp16_x;
				GSensorInfo[GSensorReadFlag][1] = tmp16_y;
				GSensorInfo[GSensorReadFlag][2] = tmp16_z;

				GSensorReadFlag++;
			}
		}
#endif
		if(capFlag < 0x55)/*capture in movie limit 1 picture in 5s*/
		{
			capFlag++;
			if(capFlag>8)
			{
				capFlag=0x55;
			}
		}
		#if 0/*SP5K_CDFS_OPT seamless record size has been checked in AppVideoSeamlessCallback*/
		if(pViewParam->VideoSeamlessMode != UI_VIDEO_SEAMLESS_OFF)
		{
			videoSizeCheck++;
			if(videoSizeCheck == 20)/*10 second*/
			{
				videoSizeCheck = 0;
				if(appCdfsFreeSpaceCheck(CDFS_FILE_FOLDER_VIDEO, TRUE) == FAIL)
				{
					if(gVideoCB.CapSec>=2)
					{
						appVideoStop();
						gVideoCB.err = VIEW_ERR_MEMORY_FULL;
					}
				}
			}
		}
		#endif
		break;

	/* media rec */
	case SP5K_MSG_MEDIA_ELAPSED_TIME_UPDATE:
		DBG_PRINT("_OnMsg: MEDIA_ELAPSED_TIME_UPDATE\n");
		break;
	case SP5K_MSG_MEDIA_REC_DROP_FRAME:
		printf("_OnMsg: MEDIA_REC_DROP_FRAME param=%x\n",param & 0xf0000000 );
		//Ethan if drop frame stop rrecording (H624)
		appVideoRecLogFileAdd(msg,param);
		if(((param & 0xF0000000) == 0x90000000)||((param & 0xF0000000) == 0x10000000)){
			break;
		}else{
			__FUNC_TRACK__;
			appVideoStop();
		}
		break;
	case SP5K_MSG_MEDIA_REC_PAUSE_FINISH:
	#if SPCA6330
	case SP5K_MSG_MEDIA_REC_RESUME_FINISH:
	#endif
		sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0); /*add for mantis bug 0047524*/
		VideoRecPaReMsgReceive =TRUE;
		break;
	case SP5K_MSG_MEDIA_FILE_SIZE_NOTIFY:
		if (param == 0x4){
			SnapSetflag =1;
			__FUNC_TRACK__;
			appVideoStop();
		}
		DBG_PRINT("_OnMsg: MEDIA_BETTER_FILE_SIZE\n");
		break;
	case SP5K_MSG_MEDIA_AVI_REC_FPS_LOW://happen with slow card and enough space(>2G)
		break;
	case SP5K_MSG_MEDIA_REC_DISK_FULL:
		printf("video msg: MEDIA_REC_DISK_FULL\n");
		if(puiPara->Seamless)
		{
			break;
		}
	case SP5K_MSG_DISK_ERROR:
		printf("video msg: SP5K_MSG_DISK_ERROR  param=%x\n",param);
		appTimerClear(&gVideoCB.CapTimer);
		__FUNC_TRACK__;

		appVideoStop();
		VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
		gVideoCB.err = VIEW_ERR_MEMORY_FULL;
		appVideoOsdShow(0,0,gVideoCB.osd);
		break;
	case APP_STATE_MSG_ABORT:
		DBG_PRINT("video msg: APP_STATE_MSG_ABORT\n");
		break;

	/* DZOOM Position bar update */
	case APP_UI_MSG_DZOOM_POS_UPDATE:
		tmpU32 = 0xFFUL & param;
		appViewOsd_ZoomFrameDraw(TRUE);
		appViewOsd_DigZoomPosDraw((UINT8)tmpU32);
		break;
	/* TELE/WIDE Self Repeat */
	case APP_UI_MSG_LENS_DZOOM_TELE:
		if (!IO_KEY_TELE_GET)
		{
			appVideoDZoomTele();
		}
		break;
	case APP_UI_MSG_LENS_DZOOM_WIDE:
		if (!IO_KEY_WIDE_GET)
		{
			appVideoDZoomWide();
		}
		break;
	#ifdef HW_FACETRACK
	case SP5K_MSG_FACE_TRACK_READY:
	case SP5K_MSG_VEHICLE_TRACK_READY:
		/*FCWS*/
		#if ICAT_FCWS
		/*FCWS*/
		#if ICAT_CVR_WARN_SYS_DBG
		APP_OSD_REFRESH_OFF;
		appForwardCollisionWarningInfo();
		APP_OSD_REFRESH_ON;
		#endif
		{
			vehicleDetectGetInfo_t vdInfors;
			static UINT8 fcwsWarnMsgCnt = 0;
			sp5kVehicleDetectResultGet(&vdInfors);
			if (vdInfors.vehicleWarn == 1) {
				fcwsWarnMsgCnt++;
			}
			if (fcwsWarnMsgCnt >= FCWS_WARN_CNT_DBG) {
				fcwsWarnMsgCnt = 0;
				appBeep(FALSE, FALSE);
			}
		}

		#if CVR_WARN_SYS_INT_DBG
		videoSeamlessFrameId++;
		sp5kVehicleDetectResultGet(&vdInfor);
		appVideoLogFile_Print("Id: %d, D=%d, LV=%d, C=%d\n", videoSeamlessFrameId, vdInfor.vehicleDistance, Vehicle_LV, fcwsWarnCnt);
		#endif
		#endif
	case SP5K_MSG_FACE_TRACK_TIMEOUT:
		{
			UINT32 nface=0;
			SINT32 err=0;
			sp5kFaceTrackFaceRes_t facelist[12];
			err = appFdResultGet(param, &nface, facelist);
			if (err!=-1)
			{
				appFdDataUpdate(nface,facelist);
				appFd_FaceRectUpdateMsgSend();
			}
		}
		break;
	case SP5K_MSG_FACE_TRACK_STOP:
		break;
	case SP5K_MSG_FACE_TRACK_INVALID:
		break;
	case APP_UI_MSG_FACE_DETECT_OSD_UPDATE:
		if(!appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			appStillOsd_UpdateFaceOsd(FALSE);
			#if FACE_WITH_AE
			appAeFaceCfg();
			#endif
		}
		break;
#endif
	#if SP5K_MOTION_DETECT
	case APP_UI_MSG_MD_STOP:
		if(mdTime != TIMER_NULL)
		{
			appTimerClear(&mdTime);
		}
		else
		{
			mdTime = appTimerSet(3000, "VideoRec");
		}
		break;
	#endif
	default:
		break;
	}
}

void
appVideoRecStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
	case APP_KEY_PRESS_S2:
		break;
	case APP_KEY_PRESS_OK:
	case APP_KEY_PRESS_S1:
		/* if record timer greater than one, we can stop immediately */
		if(gVideoCB.CapSec>=2)
		{
			appVideoRecManulStopSet(1);/*when manual stop record,we should not restart record auto*/
			appVideoStop();
		}
		else
		{
			/*gVideoCB.fStop=1;*/
		}
		break;
	case APP_KEY_PRESS_MODE:
		if(gVideoCB.CapSec>=2)
		{
			appVideoStop();
			#if !CAM_TYPE_CVR
			appStateChange(APP_STATE_AUDIO_REC,STATE_PARAM_NORMAL_INIT);
			#else
     		appVideoRecAlwaysRestart(1);
			appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
			#endif
		}
		break;
	case APP_KEY_PRESS_PB:
		#if !CAM_TYPE_CVR
		if(gVideoCB.CapSec>=2)
		{
			appVideoStop();
			#if SP5K_CDFS_OPT
			fileTypeSelect=1;
			appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
			#else
			appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
			#endif
		}
		#endif
		break;
#if CAM_TYPE_CVR
    #ifdef HW_PRJ_6330
	case APP_KEY_PRESS_LEFT:
		if(gVideoCB.CapSec>=2)
		{
			appVideoStop();
			#if SP5K_CDFS_OPT
			appVideoRecAlwaysRestart(1);
			appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
			#else
			appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
			#endif
		}
		break;
	#endif
#endif
	case APP_KEY_PRESS_RIGHT:
		#if 1
		if (VideoRecPaReMsgReceive ==FALSE){
			break;
		}else{
			VideoRecPaReMsgReceive =FALSE;
		}
		#else
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264)
			break;
		#endif
		if (VideoRecStatus==VIDEO_REC_STATUS_RECORDING
			||VideoRecStatus==VIDEO_REC_STATUS_PAUSE)
		{
			DoVedioPauseControl();
		}

		break;
	case APP_KEY_PRESS_UP:
		#if SP5K_CDFS_OPT
		appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
		#endif
		break;
	case APP_KEY_PRESS_TELE:
		if(puiPara->VideoStampMode)
			break;
		appVideoZoomTele();
		break;
	case APP_KEY_PRESS_WIDE:
		if(puiPara->VideoStampMode)
			break;
		appVideoZoomWide();
		break;
	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
		appZoomStop();
		break;
	case APP_KEY_PRESS_MENU:
		appVideoRecAlwaysMenuEnter();
		break;
	#if SPCA6330
	case APP_KEY_PRESS_SET:
		if(gVideoCB.CapSec<2)
		{
			break;
		}
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264)
		{
			if(capFlag != 0x55)//limit 1 picture in 5s
			{
				break;
			}
			#if SP5K_CDFS_OPT
			if(appCdfsFileNameValidCheck(SP5K_DCF_FILETYPE_JPG) == SUCCESS)
			{
				appCdfsEventFlagSet(JPG_EVENT_IN_VIDEO);
			}
			else
			{
				break;
			}
			#endif
			appBtnDisable(BTN_ALL);
			appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
			_errorMsgDraw(ERR_MSG_CAPTURE_DONE,TRUE);
			appCaptureInMovie();
			APP_SLEEP_MS(100);
			_errorMsgDraw(ERR_MSG_CAPTURE_DONE,FALSE);
			appBtnEnable(BTN_ALL);
			capFlag = 0;
		}
		else
		{
			printf("video format is not H264,do not support capture in movie!!");
		}
		break;
	#endif
	case APP_KEY_PRESS_DOWN:
		break;
	default:
		break;
	}
}

void
appVideoRecState(
	UINT32 msg,
	UINT32 param
)
{

	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appVideoRecState");
		#if ICAT_CVR_WARN_SYS_DBG
		sioCtrl(0);
		appCvrwsOsdDraw();
		#else
		#if ICAT_FCWS || ICAT_LDWS
		//sioCtrl(1);
		#endif
		#endif
		appVideoRecAlwaysDo();
		break;
	case APP_STATE_MSG_CLOSE:
		/*reset scale factor*/
		sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
		LED_NORMAL_ON;
		DBG_PRINT("%s : close\n",__FUNCTION__);
		appLensStopCallbackSet(NULL);
		if(!IS_CARD_EXIST && appNextStateGet()==APP_STATE_POWER_OFF)
		{
			appStateCloseDone();
		}
		else
		{
			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
		}
		break;
	case APP_STATE_MSG_ABORT:
		__FUNC_TRACK__;
		appVideoStop();
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appVideoRecStateOnKey(msg,param);
		}
		else
		{
			appVideoRecStateOnMsg(msg,param);
		}
		break;
	}
}



