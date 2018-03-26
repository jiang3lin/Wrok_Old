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

#include "app_com_api.h"
#include "app_com_def.h"
#include "app_state_ctrl.h"
#include "sp5k_rsvblk_api.h"
#include "app_jpg_draw.h"
#include "app_osd_api.h"
#include "app_playback_def.h"
#include "ndk_global_api.h"
#include "app_wifi_connection.h"
#include "app_wifi_utility.h"
#include "app_view_param.h"
#include "app_ptp.h"
#include "app_led.h"
#include "app_ui_para.h"
#include "app_zoom_api.h"
#include "app_still.h"
#include "app_video.h"

#include "app_icon_def.h"
#include "app_menu_tab.h"

#include "app_osd_api_lcm.h"
#include "app_osd_draw_lcm.h"
#include "app_rf_def.h"
#include "app_msg.h"
#include "sp5k_capture_api.h"

extern UINT8  CdvLapseWifiPreStatus; 
extern UINT8  SdvSlowWifiPreStatus;
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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appStillRemainNumGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 WiFiUtility = WIFI_UTILITY_NULL;
UINT32 WiFiConnectingTimer=TIMER_NULL;
static UINT32 connectingCounter = 0;
static UINT8 SeamlessCurSetting = 0;
static UINT8 VideoStampCurSetting = 0;
UINT8  nWifiRecKeySkip=0;
void appSdvWifiConnectionOsdShow(void)
{
	appLcmOsdClean();
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
	//Mode Icon
	appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SCDV_WIFI_MODE, FALSE);

	//ON
	appLcmOsdLargeTextShow("ON", FALSE);

	//Mode Tips
	appLcmOsdModeStringShow(ID_STR_WIFI, FALSE);

	//Bottom Tips
	appLcmOsdTipsStringShow(ID_STR_INIT___, FALSE);

	//Battery
	appLcmOsdDrawBattery(TRUE, -1);
	
	appLcmRefresh();
}

void appSdvWifiConnectionStateOsdShow(NDKSysEvt evt)
{
	appLcmOsdTipsTextShow("        ", FALSE);	//clean string with 8 space

	//Bottom Tips	
	switch(evt)
	{
		case NDK_SYSEVT_DHCPD_ALLOC_ADDR:		//ready
		case NDK_SYSEVT_HAPD_ASSOC:
			appLcmOsdTipsStringShow(ID_STR_READY, FALSE);
			break;
		case NDK_SYSEVT_HAPD_DISASSOC:		//Connectting
			appLcmOsdTipsStringShow(ID_STR_CONN___, FALSE);
			break;
		default:
			break;
	}
	
	appLcmRefresh();
}

void appNetSysEventHandler(
	NDKSysEvt evt,
	unsigned long param,
	unsigned long udata
)
{
	long n;
	uiPara_t* puiPara = appUiParaGet();

	appSdvWifiConnectionStateOsdShow(evt);

	switch (evt) {
	case NDK_SYSEVT_DHCPD_ALLOC_ADDR:
		///printf("\n %s : NDK_SYSEVT_DHCPD_ALLOC_ADDR\n",__FUNCTION__);
		///profLogPrintf(0,"[NDK]NDK_SYSEVT_DHCPD_ALLOC_ADDR ");
		CLEAR_OSD_SRC;
		connectingCounter=0;
		if(WiFiConnectingTimer != TIMER_NULL){
			appTimerClear(&WiFiConnectingTimer);
		}
		
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		///appOsdLib_TextDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "WiFi Connection Ready!");

		appWiFiConnection_UtilityStateSet(WIFI_UTILITY_OPEN_AND_ASSOC);
		//SeamlessCurSetting = puiPara->Seamless;
		//VideoStampCurSetting = puiPara->VideoStampMode;
		//puiPara->VideoStampMode= UI_STAMP_OFF;
		//puiPara->Seamless= UI_VIDEO_SEAMLESS_OFF;
		break;
	case NDK_SYSEVT_HAPD_ASSOC:
		CLEAR_OSD_SRC;
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		///appOsdLib_TextDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "WiFi Connection Ready!");

		///printf("%s : NDK_SYSEVT_HAPD_ASSOC\n",__FUNCTION__);
		///profLogPrintf(0,"[NDK]NDK_SYSEVT_HAPD_ASSOC");
		if (ndk_netif_ioctl(NDK_IOCG_WIFI_STA_NR, (long)"wlan0", &n) == 0){
		}
		break;

	case NDK_SYSEVT_HAPD_DISASSOC:
		/*S xuan.ruan@20131125 add for mantis bug 0048876*/
		CLEAR_OSD_SRC;
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		///appOsdLib_TextDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "Waiting for WiFi Connection...");
		
		/*E xuan.ruan@20131125 add for mantis bug 0048876*/
		///printf("%s : NDK_SYSEVT_HAPD_DISASSOC\n",__FUNCTION__);
		///profLogPrintf(0,"[NDK]NDK_SYSEVT_HAPD_DISASSOC");
		appWiFiConnection_UtilityStateSet(WIFI_AP_DISASSOC);
		if(WiFiConnectingTimer == TIMER_NULL){
			WiFiConnectingTimer = appTimerSet(1000,"WiFiConnectingTimer");
		}
		//puiPara->Seamless= SeamlessCurSetting;
		//puiPara->VideoStampMode= VideoStampCurSetting;
		break;

	default:
		break;
	}
}

void
appWiFiConnectionStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_UI_MSG_WIFI_DZOOM_TELE:
		appVideoDZoomTele();
		break;
	case APP_UI_MSG_WIFI_DZOOM_WIDE:
		appVideoDZoomWide();
		break;
	case SP5K_MSG_MODE_READY:
		//printf("\n@@-----SP5K_MSG_MODE_READY-----\n");
		break;
	case SP5K_MSG_AE_READY:
		//printf("\n@@-----SP5K_MSG_AE_READY-----\n");
		break;
	case SP5K_MSG_AWB_READY:
		//printf("\n@@-----SP5K_MSG_AWB_READY-----\n");
		break;
	case SP5K_MSG_TIMER:
		if(param == WiFiConnectingTimer){
			connectingCounter++;
			printf("connectingCounter = %d  \n", connectingCounter);
			if(connectingCounter == WIFI_SEARCH_TIMEOUT){
				printf("\n@@-----Connection time out-----\n");
				#if 0	/* No OSD/IMG active */
				CLEAR_OSD_SRC;
				#endif
				//appLedOff(APP_LED_ORANGE);
				//appLedOn(APP_LED_GREEN);
				//appWiFiConnectionOff();
			}
		}
		break;
	default:
		break;
	}
}

void BtnEnable(void)
{
	appBtnEnable(BTN_OK);
	printf("ok enable\n");
}

void
appWiFiConnectionStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	static UINT32 templ=0;
	uiPara_t* puiPara =appUiParaGet();
	switch (msg) 
	{
		case APP_KEY_PRESS_OK:
			appWiFiConnectionOff();
			nWifiRFMutex=0;
			nCdvLapseWifiRFMutex=0;
			//rec 
			appBtnDisable(BTN_OK);
			appMsgFlush(APP_MSG_KEY_QUE);
			appBtnEnable(BTN_OK);
			if(appCheckSdvMainMode() == FALSE)
				puiPara->CdvVideoMode=CdvLapseWifiPreStatus;
			else 
				puiPara->SdvVideoMode=SdvSlowWifiPreStatus;
			//appRfCopyBufferCallbackSet();
			if(appRFSkipFunction()==FALSE)
				sp5kPreviewCallbackSet(1, appRfCopyBufferCallback,320,240);
			break;
		case APP_KEY_PRESS_MODE:
			break;
		default:
			break;
	}
}


void
appWiFiConnectionStateInit(
	void
)
{
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;

	ALL_LED_OFF;

	appSdvWifiConnectionOsdShow();

	
	/*appJpgDraw_PfLayer(RES_JPG_BLACK, 0, 0, 320, 240);*/
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	//appPowerSavingEnableSet(FALSE);
	appAutoPowerOffEnable(FALSE);
	appLcmPowerSaveEnable(FALSE);
	if(appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE){
		appWiFiConnection_UtilityStateSet(WIFI_UTILITY_OPEN);
		if(WiFiConnectingTimer == TIMER_NULL){
			WiFiConnectingTimer = appTimerSet(1000,"WiFiConnectingTimer");
		}
		//appOsdLib_TextDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "Waiting for WiFi Connection...");
		ndk_sysevt_handler_set(appNetSysEventHandler, 0);
		APP_OSD_REFRESH_ON;
		appWiFiStartConnection(WIFI_INT);
		appSdvWifiConnectionStateOsdShow(NDK_SYSEVT_HAPD_DISASSOC);
	}else{
		//appOsdLib_TextDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "WiFi Connection Ready!");
		appSdvWifiConnectionStateOsdShow(NDK_SYSEVT_HAPD_ASSOC);
		APP_OSD_REFRESH_ON;
	}
}


void
appWiFiConnectionState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appWiFiConnectionState");
		appWiFiConnectionStateInit();
		appBtnEnable(BTN_OK);
		//printf("--wifi ok eable\n");
		break;
	case APP_STATE_MSG_CLOSE:
		printf("@@appWiFiConnectionState close --@@\n");
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appWiFiConnectionStateOnKey(msg,param);
		}
		else
		{
			appWiFiConnectionStateOnMsg(msg,param);
		}
		break;
	}
}

UINT8 appWiFiConnection_UtilityStateGet(void)
{
	return WiFiUtility;
}

void appWiFiConnection_UtilityStateSet(UINT8 nWiFiUtility)
{
	WiFiUtility = nWiFiUtility;
}

void appWiFiConnectionOff(void)
{
	UINT32 sp5kMode;
	uiPara_t* puiPara = appUiParaGet();
	
	connectingCounter=0;
	appWiFiConnection_UtilityStateSet(WIFI_UTILITY_CLOSE);

	if (WiFiConnectingTimer!=TIMER_NULL)
		appTimerClear(&WiFiConnectingTimer);

	sp5kModeGet(&sp5kMode);

	#if 0 /* [SY][wdv5300] no need */
	if(appActiveStateGet()!=APP_STATE_VIDEO_REC&&sp5kMode!=SP5K_MODE_VIDEO_RECORD){
		/* WiFi OFF, DSC pv freeze issue, tmp fix 20130307 */
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
	}
	#endif
	

	/* streaming start /stop is only allow on video pv / rec mode, before WiFi off, please check if the sp5k mode is right */
	appWiFiStopConnection(WIFI_INT&(~FTP_SRV));
	if(appActiveStateGet()==APP_STATE_VIDEO_REC || sp5kMode==SP5K_MODE_VIDEO_RECORD)
	{
		printf("Video recording:0x%x\n",appActiveStateGet());
	}
	#if 1
	else
	{
		if(appCheckSdvMainMode() == TRUE)
		{
			appStateChange(APP_STATE_SPORTDV_WIFI_MODE, APP_STATE_MSG_INIT);
		}
		else
		{
			appStateChange(APP_STATE_CARDV_WIFI_MODE, APP_STATE_MSG_INIT);
		}
	}
	#else
	else if(appPreviousStateGet()==APP_STATE_VIDEO_PREVIEW)
	{
		appStateChange(APP_STATE_VIDEO_PREVIEW, APP_STATE_MSG_INIT);
	}
	else
	{
		appStateChange(APP_STATE_STILL_PREVIEW, APP_STATE_MSG_INIT);
	}
	#endif
	
	app_PTP_clear_EventQ();

	/* Digital Zoom Reset : s */
	if(appDZoomGetRation()!=1000){
		appDZoomReset(APP_DZOOM_UI_MODE_VIDEO_VIEW);
		appStill_SetDZoom(puiPara->DigitalZoom);
	}
	/* Digital Zoom Reset : e */
	
	printf("%s:(%d,0x%x,0x%x)\n",__FUNCTION__,
		appWiFiConnection_UtilityStateGet(),
		appActiveStateGet(),
		app_PTP_Get_DscOpMode(MODE_ACTIVE));

	//WIFI_POWER(WIFI_OFF);

}

void appWiFiConnectionOn(void)
{
	UINT32 sp5kMode;


	//WIFI_POWER(WIFI_ON);

	sp5kModeGet(&sp5kMode);

	#if 1
	appDispLayer_IMG_Ctrl(0);
	appDispLayer_OSD_Ctrl(0);
	/* disable 2nd path yuv before entering preview mode */
	sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_HOST_EN_CFG, 0);
    #endif

	if(appActiveStateGet()==APP_STATE_VIDEO_REC || sp5kMode==SP5K_MODE_VIDEO_RECORD){
		appWiFiConnectionStateInit();
	}else{
		appStateChange(APP_STATE_WIFI_CONNECTION, APP_STATE_MSG_INIT);
	}
}
