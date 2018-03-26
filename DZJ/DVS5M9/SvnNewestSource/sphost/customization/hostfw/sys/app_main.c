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
#include "app_com_def.h"
#include "app_com_api.h"
#include "customization/solution_def.h"
#include "gpio_custom.h"
#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_usb_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_dev_plug.h"
#include "app_tvlcd.h"
#include "app_script.h"
#include "app_calib_api.h"
#include "app_playback_def.h"
#include "app_rtc.h"
#include "app_battery.h"
#include "app_lyt_show.h"
#include "app_still.h"
#include "app_aaa.h"
#include "app_battery_ctrl.h"
#include "app_wifi_utility.h"
#include "app_ptp.h"
#include "app_sys_def.h"
#include "app_ui_para.h"
#include "app_still_timelapse.h"
#include "app_view_param_def.h"
#include "app_osd_api.h"
#if MTBF_DBG_EN
#include "../service/misc/app_mtbf_cmd.h"
#endif
#include "app_cdfs_api.h"
#include "app_wifi_connection.h"
#include "app_msg.h"
#include "app_video.h"
#include "app_rf_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static BOOL fPowerOff=0;
static BOOL isFirstDiskMount = FALSE;
static BOOL isGoing2PowerOff = FALSE;
BOOL bBatteryIn = FALSE;
BOOL bPccamConn = FALSE;
static BOOL dbgInfo2SD=FALSE; /* FALSE: disable TRUE:enable */
UINT8 gsensorTriggerRecordingStateFlag = 0;
UINT8 gsensorTriggerByIntFlag = 0;
UINT8 gsensorDelayRecTimes = 0;
UINT8 carchargerDetectFlag = 0;	/* When power on with Carcharger in, auto start videorec in CVR */
UINT8 powerOffInVideoRec = 0;
UINT8 countDownRecFlag = 0;
UINT32 ckTimer=TIMER_NULL;

extern unsigned int nCaptureRemainNum;
 /**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appStateCtrlInitial(void);
UINT32 appBtnMsgToKey(UINT32 msgId,UINT32 msgParam);
void appStateOnProc(UINT32 msg,UINT32 param);
void appUiModeChange(void);
void appPsuedoPowerOff(void);
void appDiskMountMsgHandler(	UINT32 msg,	UINT32 param);
void appDevHotPlugHandler(UINT32 msg,UINT32 param);
void appPowerSaveTimeOutReset(void);
void appUsbMsdcModeSet(void);
void debugIntoSDCheck(void);
extern UINT32 dbgPrintLogInit(UINT32 size);
extern void dbgPrintLogFnameSet(UINT8 *fname);
extern UINT32 appLogMsgToStorage(UINT32 msg,UINT32 param);

#if SP5K_UART_SWITCH
void UARTCtrl(void);
#endif

/* check the mark file,it's determind need to write debug info into SD  */
void debugIntoSDCheck(void)
{
	UINT32 fd;

	fd = sp5kFsFileOpen("D:\\iCatOS_SD_logon.mrk",FS_OPEN_RDONLY);
	if(fd){
		dbgInfo2SD = TRUE;
		sp5kFsFileClose(fd);
	}
}

#if SP5K_UART_SWITCH
/* if not found the mark file,close UART */
void UARTCtrl(void)
{
	UINT32 fd;

	fd = sp5kFsFileOpen("D:\\iCatOS_Uart_logon.mrk",FS_OPEN_RDONLY);
	if(!fd){
		sioCtrl(1);
	}else{
		sioCtrl(0);
		sp5kFsFileClose(fd);
	}
}
#endif

void
appKeyMsgDebugPrint(
	UINT32 keyMsg
)
{
	switch( KEY_GROUP(keyMsg) )
	{
	case KEY_GROUP_S1:
		printf("[*]KEY_S1 : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_S2:
		printf("[*]KEY_S2 : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_WIDE:
		printf("[*]KEY_WIDE : %d (1-release, 2-press, 3-repeat)]\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_TELE:
		printf("[*]KEY_TELE : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_UP:
		printf("[*]KEY_UP : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_DOWN:
		printf("[*]KEY_DOWN : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_LEFT:
		printf("[*]KEY_LEFT : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_RIGHT:
		printf("[*]KEY_RIGHT : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_MENU:
		printf("[*]KEY_MENU : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_DEL:
		printf("[*]KEY_DEL : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_SET:
		printf("[*]KEY_SET : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_MODE:
		printf("[*]KEY_MODE : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_TV:
		printf("[*]KEY_TV : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_HDMI:
		printf("[*]KEY_HDMI : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_USB:
		printf("[*]KEY_USB : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_CARD:
		printf("[*]KEY_CARD : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_MODEDIAL:
		printf("[*]KEY_MODEDIAL : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_PB:
		printf("[*]KEY_PB : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_OK:
		printf("[*]KEY_GROUP_OK : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	case KEY_GROUP_POWER:
		printf("[*]KEY_GROUP_POWER : %d (1-release, 2-press, 3-repeat)\n",KEY_STATE(keyMsg));
		break;
	default:
		printf("[*]KEY_ERROR!!! : %08x\n",keyMsg);
		break;
	}

}

void
appPowerOffUiMsgSend(
	UINT32 param
)
{
	sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, param);
	isGoing2PowerOff = TRUE;
}


void
appPowerOffWakeupSet(
	UINT32 param
)
{
	appRTC_t tt;
	UINT32 wakeuptime =0;

	appCalibDataSave();

	wakeuptime = param; /*Unit:Sec*/
	appRtcGet(&tt);

	printf("\nWakeupGet : %d : %d : %d : %d : %d : %d\n\n",tt.year, tt.mon,tt.day,tt.hour,tt.min,tt.sec);

	tt.sec=tt.sec+wakeuptime;
	if (tt.sec>59)
	{
		tt.min =tt.min+1;
		tt.sec = tt.sec-60;
	}

	if (tt.min>59)
	{
		tt.hour =tt.hour+1;
		tt.min =tt.min-60;
	}

	printf("\nWakeupSet : %d : %d : %d : %d : %d : %d\n\n",tt.year, tt.mon,tt.day,tt.hour,tt.min,tt.sec);
	appRtcWakeUpSet(&tt);
	sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
	isGoing2PowerOff = TRUE;
}

extern appVideoCB_t gVideoCB;
appState_msg_flow_e
appStateCommonProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_IGNORE;
	static UINT32 countNum=0;
	static UINT32 detectResult=0;
	#if ICAT_WIFI
	static UINT16 ptpOptcode = 0x1000;/*USB_PIMA_OC_UNDEFINED*/
	#endif
	UINT32 bandingset=0;
	uiPara_t* puiPara = appUiParaGet();

	switch( msg )
	{
	case SP5K_MSG_HOST_TASK_INIT:  /* the 1'st msg after boot*/
		appStateCtrlInitial();
		break;
	//case SP5K_MSG_POWER_OFF:
	case APP_KEY_LONG_PRESS_POWER:
		DBG_PRINT("SP5K_MSG_POWER_OFF\n");
		/*
		 * Here user do pre-power down preparation
		 * The times = 0 for no need to polling
		 */
		if (!fPowerOff)
		{
			isGoing2PowerOff = TRUE;
			fPowerOff = 1;
			#if ICAT_WIFI
			appExceptionHandle();
			#endif
			appStateChange(APP_STATE_POWER_OFF,APP_POWER_OFF_NORMAL);
		}
		break;
	case SP5K_MSG_POWER_OFF_KEY_RELEASE:
		//do nothing
		break;

	case SP5K_MSG_AUTO_BANDING_RESULT:
		HLPrintf1("frequence=%d\n",param);     //Remove warning Lation@20121120
		if(detectResult == param)
		{
			countNum++;
			if(countNum > 5)
			{
				countNum=0;
				sp5kModeCfgSet(SP5K_MODE_CFG_BAND_DETECT_ENABLE, 0);
				if (detectResult ==50){ /*50 Hz*/
					bandingset = 0;
				}else{
					bandingset = 1;
				}
				appStill_SetExpFlickerMode(bandingset);

				appAePvReset();
			}
		}
		else
		{
			countNum=0;
			detectResult = param;

		}
		break;
	case SP5K_MSG_DISK_INSERT:
		profLogPrintf(0, "disk in(%d)",param);
		DBG_PRINT("SP5K_MSG_DISK_INSERT(%d) ",param);
		if (param == SP5K_DRIVE_NAND)
		{  // do not mount nand if power on with card inserted
			if (!IS_CARD_EXIST)
			{
				isFirstDiskMount = TRUE;
				appDiskMount(param);
			}
		}
		else
		{
			if (isFirstDiskMount)
			{ // send card in msg after power on
				DBG_PRINT("send msg :APP_KEY_CARD_IN(%d) ",param);
				sp5kHostMsgSend(APP_KEY_CARD_IN, param);
			}
			else
			{
				isFirstDiskMount = TRUE;
			#if 0/*def ICAT_WIFI*/
				appDiskMount(SP5K_DRIVE_NAND);
			#else
				if(appActiveStateGet() != APP_STATE_POWER_ON){
					sp5kHostMsgSend(APP_KEY_CARD_IN, param);
				}else{
					appDiskMount(param);
				}
			#endif
			}
		}
		break;
	case SP5K_MSG_DISK_REMOVAL:
		DBG_PRINT("SP5K_MSG_DISK_REMOVAL(%d) ",param);
		appLogMsgToStorage(msg,param); /*add for SD log */
		
		if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		{
			//sp5kHostMsgSend(SP5K_MSG_MEDIA_REC_STOP, param);
			//printf("--wifi card out\n");
			
		}	
		sp5kHostMsgSend(APP_KEY_CARD_OUT, param);
		break;
	case SP5K_MSG_DISK_MOUNT_COMPLETE:
		profLogPrintf(0, "mount rdy(%d)",param);
		appLogMsgToStorage(msg,param); /*add for SD log */
		#if 0
		if(IS_CARD_EXIST){
			debugIntoSDCheck();
			if(dbgInfo2SD == TRUE){
				dbgPrintLogInit(16384);  /* 16KB */
				dbgPrintLogFnameSet("D:\\DEBUGLOG.TXT");
			}
		}
        #endif
		#if SP5K_UART_SWITCH
		UARTCtrl();
		#endif
		/* Who did it !!! appScriptAutoRun(param); */
		// fall through
	case SP5K_MSG_DISK_MOUNT_START:
	case SP5K_MSG_DISK_MOUNT_FAIL:
	case SP5K_MSG_DISK_ERROR:
		appDiskMountMsgHandler(msg,param);
		ret = APPSTATE_MSG_DEFAULT;
		break;
	case SP5K_MSG_USB_POWER_CONFIGURED:
		if(param)
		{
			bBatteryIn = appBatteryAdcValueGet() > 100 ? TRUE:FALSE;

			if(appBatteryCharge_SetCheckStatusCBF() != BATTERY_STATUS_FULL && bBatteryIn){
	    		appBatteryCharge_SetStartCBF();
				RED_LED_ON;
			}
		}
	#if ICAT_WIFI
	case SP5K_MSG_USB_PTP_SESSION_OPEN:
		printf("[PTP] SP5K_MSG_USB_PTP_SESSION_OPEN received!\n");
		nWifiRecKeySkip=FALSE;
		puiPara->VideoSize= UI_VID_SIZE_FHD;
		app_PTP_clear_EventQ();

		if(!IS_CARD_EXIST){
			appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SDCARD_OUT, 0, 0);
		}
		appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_BATTERY_LEVEL, appBatteryLevelGet(), 0);
		break;
	case SP5K_MSG_USB_PTP_SESSION_CLOSE:
		printf("[PTP] SP5K_MSG_USB_PTP_SESSION_CLOSE received!\n");
		break;
#if 1
	case SP5K_MSG_USB_PTP_OP_START:
		/*printf("SP5K_MSG_USB_PTP_OP_START:0x%x\n",param);*/
		if(param==/*USB_PIMA_OC_FORMAT_STORE*/0x100F)
		{
			printf("[PIMA]Format storage operation:s\n");
		}
		else if(param==/*USB_PIMA_OC_DELETE_OBJECT*/0x100B)
		{
			printf("[PIMA]Delete file operation:s\n");
		}
		else if(0x9805/*USB_PIMA_EOC_GET_OBJECT_PROP_LIST*/==(param&0xFFFF)) /* Highest bit = 1 means basefw execute this operation, param = 0x80009805 ; = 0 means APP, param = 0x00009805 */
		{
			printf("[PIMA]get object list operation:s\n");
		}
		ptpOptcode = param;

		break;
	case SP5K_MSG_USB_PTP_OP_END:
		/*printf("SP5K_MSG_USB_PTP_OP_END:0x%x\n",param);*/
		if(param==SUCCESS)
		{
			/*printf("PTP operation OK!\n");*/
			if(0x100F==ptpOptcode){
				#if SP5K_CDFS_OPT
				static UINT8 dir1[128]= "D:\\VIDEO";
				static UINT8 dir2[128]= "D:\\JPG";
				static UINT8 dir3[128]= "D:\\EVENT";
				#else
				UINT8 dir1[128]="D:\\DCIM\\100MEDIA";

				//sprintf(dir1,"%s",sp5kFsGetCurPath());
				//dir1[2]='\\';
				//dir1[7]='\\';
				printf("\n\ndir2:%s\n\n",dir1);
				#endif

				//for DVS5M2 file folder
				appCdfsFolder2Path(CDFS_FILE_FOLDER_VIDEO, dir1);
				appCdfsFolder2Path(CDFS_FILE_FOLDER_JPG, dir2);
				appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT, dir3);
				appPathSlashReplace(dir1);
				appPathSlashReplace(dir2);
				appPathSlashReplace(dir3);
				//for DVS5M2 file folder

				printf("[PIMA]Format storage operation:e\n");
				if(sp5kDcfFsInit(appActiveDiskGet(),0,0)){
					printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
				}

				if(sp5kDcfFsActive(appActiveDiskGet())){
					printf("[ERR] %s(%d)\n",__FUNCTION__,__LINE__);
				}

				#if ICAT_WIFI
				CLEAR_OSD_SRC;
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextDraw(160, 120, SP5K_GFX_ALIGN_CENTER, "WiFi Connection Ready!");
				if(IS_CARD_EXIST){
					appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_SD, 0, 0);
				}else{
					appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_NAND, 0, 0);
				#endif
				}

				#if SP5K_CDFS_OPT
				appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);/*create folder SDV/EVENT in SD Card*/
				appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);/*create folder SDV/JPG in SD Card*/
				appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);/*create folder SDV/VIDEO in SD Card*/
				appStill_PIMA_Send_Event(PTP_EC_OBJECT_ADDED, (UINT32)dir1, 0, 0);/*add folder SDV to list*/
				
				appStill_PIMA_Send_Event(PTP_EC_OBJECT_ADDED, (UINT32)dir1, 0, 0);/*add folder VIDEO to list*/
				appStill_PIMA_Send_Event(PTP_EC_OBJECT_ADDED, (UINT32)dir2, 0, 0);/*add folder JPG to list*/
				appStill_PIMA_Send_Event(PTP_EC_OBJECT_ADDED, (UINT32)dir3, 0, 0);/*add folder EVENT to list*/
				#endif
			}
			else if(0x100B==ptpOptcode)
			{
				/* pimaDelFileCb already handle the post process */
				printf("[PIMA]Delete file operation:e\n");
			}
		}
		else if(0x9805==(param&0xFFFF)){ /* Highest bit = 1 means basefw execute this operation, param = 0x80009805 ; = 0 means APP, param = 0x00009805 */
			printf("[PIMA]get object list operation:e\n");
			if((param>>31)==1)/* [47844] key is not allowed till net init procedure is done */
				appBtnEnable(BTN_ALL);
		}
		else
		{
			printf("PTP operation fail!\n");
		}
		#endif
	#endif
	break;
	//----UI message ----------------------------------------
	case APP_UI_MSG_LENS_ERR:
		printf("[ERR]APP_UI_MSG_LENS_ERR(%d)\n",param );
		appPowerOffUiMsgSend(APP_POWER_OFF_LENS_ERROR);
		break;
	case APP_UI_MSG_STROBE_CHARGE_ERR:
		printf("[ERR]APP_UI_MSG_STROBE_CHARGE_ERR\n");
		appPowerOffUiMsgSend(APP_POWER_OFF_CHARGE_ERROR);
		//stop strobe charge here
		//stop LED blinking
		break;
	case APP_UI_MSG_POWER_OFF:
		printf("[INFO]APP_UI_MSG_POWER_OFF(%d)\n",param );
		if (param == APP_POWER_OFF_APO)
		{
			if(IS_USB_GROUP(appActiveStateGet())
				|| IS_CALIB_GROUP(appActiveStateGet()))
			{
				printf("[INFO] Ignore auto power off message!!\n");
				return ret;
			}
		}
		else if(param == APP_POWER_OFF_BATT_EMPTY)
		{
			printf("[INFO] Battery Empty!!! system is going to power off now!\n");
		}
		appStateChange(APP_STATE_POWER_OFF,param);
		break;
	case APP_KEY_USB_IN:
	case APP_KEY_USB_OUT:
		#if USB_FORCE_TO_BOOT
		if (msg == APP_KEY_USB_IN)
		{
			/* switch to Idle mode first to make sure buffer enough */
			//sp5kModeSet(SP5K_MODE_STANDBY);
			//sp5kModeWait(SP5K_MODE_STANDBY);

			/* set USB mode to USB mode */
			sp5kUsbModeSet(SP5K_USB_MODE_APITEST );
			sp5kUsbMaxPowerSet(SP5K_USB_MAXPOWER_HIGH);
			sp5kUsbSwCon(1);
		}
		break;
		#endif
		if(appCalibUsbSettingGet() ||
			IS_CALIB_GROUP(appActiveStateGet()) ||
			appCalibAPITestSettingGet())
		{
			if (msg == APP_KEY_USB_IN)
			{
				if(appCalibUsbSettingGet())
				{
					sp5kUsbModeSet(SP5K_USB_MODE_PCCAM );
				}
				else if(appCalibAPITestSettingGet())
				{
					sp5kUsbModeSet(SP5K_USB_MODE_APITEST );
				}
				else
				{
					appUsbMsdcModeSet();
				}
				sp5kUsbMaxPowerSet(SP5K_USB_MAXPOWER_HIGH);
				sp5kUsbSwCon(1);
			}
			else
			{
				sp5kUsbSwCon(0);
			}
			return APPSTATE_MSG_DEFAULT;
		}

	case APP_KEY_CARD_IN:
	case APP_KEY_CARD_OUT:
	case APP_KEY_TV_IN:
	case APP_KEY_TV_OUT:
	case APP_KEY_HDMI_IN:
	case APP_KEY_HDMI_OUT:
		if(IS_CALIB_GROUP(appActiveStateGet()))
		{
			return APPSTATE_MSG_DEFAULT;
		}
		appDevHotPlugHandler(msg,param);
		break;
	case APP_UI_MSG_RTC_WAKEUP:
		appPowerOffWakeupSet(param);
		break;

	#if SPCA6330
	case SP5K_MSG_USB_UI_TOOL_PARSE:
		app_lytUSBView();
		break;
	case SP5K_MSG_USB_PCCAM_CONNECT:
		bPccamConn = TRUE;
		break;
	case SP5K_MSG_USB_PCCAM_DISCONNECT:
		bPccamConn = FALSE;
		break;
	#endif
	#if SOUND_PLAY_NEW_PROC
	case SP5K_MSG_MEDIA_SOUND_FINISH:
		appSoundPlayEvtSet();
		break;
	#endif
	#if ICAT_WIFI
	case APP_UI_MSG_PTP_CMD_SET:
		appStill_PTPToUI_Set();
		break;
	case APP_UI_MSG_RTP_START:
		appRtpStartStreaming();
		break;
	case APP_UI_MSG_RTP_STOP:
		appRtpStopStreaming();
		break;
	case APP_UI_MSG_EXCEP_HANDLE:
		appExceptionHandle();
		break;
	#endif
	#if MTBF_DBG_EN
	case APP_UI_MSG_MTBF_JOB_DONE:
		appMTBFJobFinEvtPut();
		break;
	case APP_UI_MSG_MTBF_TASK_DEL:
		appMTBFTestInit(0);
		break;
	#endif
#if CAM_TYPE_CVR
	case APP_UI_MSG_GSENSOR_REC:
		if((puiPara->G_Sensor == UI_GSENSOR_STATUS_SPORT) && (appActiveStateGet() != APP_STATE_POWER_ON))
		{
			/* In Gsensor Sport Mode, don't protect file */
			break;
		}
		else if(gsensorTriggerRecordingStateFlag)
		{
			break;
		}
		#if KIT_WITH_HDMI
		else if(IS_HDMI_IN){
			break;
		}
		#endif

		if(appActiveStateGet() == APP_STATE_VIDEO_REC)
		{
			if(puiPara->Seamless == UI_VIDEO_SEAMLESS_OFF)
			{
				printf("---Gsensor Trigger In Videorec:Seamless Off---\n");
				gsensorTriggerRecordingStateFlag = 1;
				gsensorTriggerByIntFlag = 2;
				gsensorDelayRecTimes = 60;
				countDownRecFlag = 1;
				appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
			}
			else
			{
				printf("---Gsensor Trigger In Videorec:Seamless On---\n");
				gsensorTriggerRecordingStateFlag = 1;
				gsensorTriggerByIntFlag = 100;
			}
		}
		else if(appActiveStateGet() == APP_STATE_STILL_PREVIEW)
		{
			printf("---Gsensor Trigger In Stillview---\n");
			gsensorTriggerRecordingStateFlag = 1;
			if(nCaptureRemainNum > 0)
			{
				sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
			}
			else
			{
				gsensorTriggerRecordingStateFlag = 0;
			}
		}
		else if(appActiveStateGet() == APP_STATE_POWER_ON)
		{
			printf("---Gsensor Trig In Poweron---\n");
			gsensorTriggerRecordingStateFlag = 1;
			gsensorTriggerByIntFlag = 1;
			gsensorDelayRecTimes = 60;
			carchargerDetectFlag = 1;
			countDownRecFlag = 1;
			appCdfsEventFlagSet(VIDEO_EVENT_IN_VIDEO);
		}
		break;
#endif
	#if OLD_TLS_OPT
	case APP_UI_MSG_TIMELAPSE_MERGE:
		printf("Send Msg : APP_UI_MSG_TIMELAPSE_MERGE\n");
		appMediaTimeLapse(appJpgFileNameGet());
		break;
	case APP_UI_MSG_TIMELAPSE_TASK_DEL:
		printf("Send Msg : APP_UI_MSG_TIMELAPSE_TASK_DEL\n");
		appStillTimelapseTaskInit(0);
		break;
	#endif

	// for dvs5m2 rf
	case APP_UI_MSG_RF_RCWD01_REC_START:
	case APP_UI_MSG_RF_RCWD01_REC_STOP:
	case APP_UI_MSG_RF_RCW5K8_REC:
		//appMsgFlush(APP_MSG_UI_QUE);
		if(appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE && appBtnEnableGet() == TRUE)
		{
			if(	(msg == APP_UI_MSG_RF_RCWD01_REC_START && appVideoRecordStateGet() == TRUE)
				||(msg == APP_UI_MSG_RF_RCWD01_REC_STOP && appVideoRecordStateGet() == FALSE))
			{
				break;
			}
			
			if(appActiveStateGet() ==APP_STATE_USB_MSDC ||(IS_HDMI_IN))
				break;
		
			if(appActiveStateGet() == APP_STATE_SPORTDV_STILL_CAPTURE					
					|| appActiveStateGet() == APP_STATE_SPORTDV_STILL_CAP_BURST
					|| appActiveStateGet() == APP_STATE_SPORTDV_STILL_CAP_SELF_TIMER)
			break;		

			//if(appActiveStateGet()!=APP_STATE_SPORTDV_VIDEO_PREVIEW ||appActiveStateGet()!=APP_STATE_CARDV_VIDEO_PREVIEW)
				//RFSnapMovieMutex =1;
		
			if(appCheckSdvMainMode() == TRUE)
			{
				if(	appActiveStateGet() != APP_STATE_SPORTDV_VIDEO_PREVIEW
					&& appActiveStateGet() != APP_STATE_SPORTDV_VIDEO_REC
					&& appActiveStateGet() != APP_STATE_SPORTDV_VIDEO_REC_SLOW)
				{
					appStateChange(APP_STATE_SPORTDV_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
					sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 300);
				}
			}
			else
			{
				if(	appActiveStateGet() != APP_STATE_CARDV_VIDEO_PREVIEW
					&& appActiveStateGet() != APP_STATE_CARDV_VIDEO_REC
					&& appActiveStateGet() != APP_STATE_CARDV_VIDEO_REC_LAPSE)
				{
					appStateChange(APP_STATE_CARDV_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
					sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 300);
				}
			}
			sp5kHostMsgSend(APP_KEY_PRESS_OK, KEY_MSG_PARAM_DIRECT_RUN);
			
			//sp5kHostMsgSend(APP_UI_MSG_RF_RCWD01_REC_OK, KEY_MSG_PARAM_DIRECT_RUN);
		}
		break;
	case APP_UI_MSG_RF_RCWD01_SNAP:
	case APP_UI_MSG_RF_RCW5K8_SNAP:
		
		//appMsgFlush(APP_MSG_UI_QUE);
		
		if(appActiveStateGet() ==APP_STATE_USB_MSDC 
			||appActiveStateGet() == APP_STATE_SPORTDV_VIDEO_REC
			||appActiveStateGet() == APP_STATE_SPORTDV_VIDEO_REC_SLOW
			||(IS_HDMI_IN))
			break;
		if(appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE && appBtnEnableGet() == TRUE)
		{

			//if(gVideoCB.CapSec<=1&&appVideoRecordStateGet() == TRUE)
				//break;			
			if(appCheckSdvMainMode() == TRUE)
			{
				if(	appActiveStateGet() != APP_STATE_SPORTDV_STILL_PREVIEW
					&& appActiveStateGet() != APP_STATE_SPORTDV_STILL_CAPTURE
					&& appActiveStateGet() != APP_STATE_SPORTDV_STILL_CAP_LAPSE
					&& appActiveStateGet() != APP_STATE_SPORTDV_STILL_CAP_BURST
					&& appActiveStateGet() != APP_STATE_SPORTDV_STILL_CAP_SELF_TIMER)
				{
 					appStateChange(APP_STATE_SPORTDV_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
					sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
				}
				sp5kHostMsgSend(APP_KEY_PRESS_OK, KEY_MSG_PARAM_DIRECT_RUN);					
				//sp5kHostMsgSend(APP_UI_MSG_RF_RCWD01_SNAP_OK, KEY_MSG_PARAM_DIRECT_RUN);
			}
		}
		break;
	// for dvs5m2 rf

	default:
		ret = APPSTATE_MSG_DEFAULT;
		break;
	}

	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}


appState_msg_flow_e
appStateGroupBaseProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	switch( STATE_GROUP(appActiveStateGet()) )
	{
		case APP_STATE_GROUP_SYS:
			ret = appGroupSysProc(msg,param);
			break;
		case APP_STATE_GROUP_VIEW:
			ret = appGroupViewProc(msg,param);
			break;
		case APP_STATE_GROUP_VIDEO:
			ret = appGroupVideoProc(msg,param);
			break;
		case APP_STATE_GROUP_AUDIO:
			ret = appGroupAudioProc(msg,param);
			break;
		case APP_STATE_GROUP_PLAYBACK:
			ret = appGroupPlaybackProc(msg,param);
			break;
		case APP_STATE_GROUP_USB:
			ret = appGroupUsbProc(msg,param);
			break;
		case APP_STATE_GROUP_CALIB:
			ret = appGroupCalibrationProc(msg,param);
			break;
		default:
			break;
	}
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}

UINT8 nDisableHdmiDetect=1;
void
appMain(
	UINT32 sp5kmsg,
	UINT32 sp5kparam
)
{
	UINT32 msg = sp5kmsg;
	UINT32 param = sp5kparam;
	UINT32 key;
	UINT32 tCurMode=0;
	#if HOST_DBG
	UINT32 inTime=0, outTime=0;
	#endif

	#if HOST_DBG
	if ((msg&0xffffff00) != (SP5K_MSG_AE_READY&0xffffff00))
	{
		profLogPrintf(0, "..[0x%08x, 0x%08x]",sp5kmsg, sp5kparam);
		///HLPrintf2("Msg(0x%08x),Param( 0x%08x)",sp5kmsg, sp5kparam);
		inTime = appMsTimeGet();
	}
	#endif
	/* decode sp5k gpio/adc button msg to app key msg*/

	if (IS_SP5K_BTN_MSG(sp5kmsg))
	{
		key = appBtnMsgToKey(sp5kmsg,sp5kparam);
		if (key == APP_KEY_ERROR || key == APP_KEY_SKIP)
		{
			#if HOST_DBG
			if ((msg&0xffffff00) != (SP5K_MSG_AE_READY&0xffffff00))
			{
				printf("### %d : msg=%08x param=%08x done\n\n", appMsTimeGet(),key, 0);
			}
			#endif

			return;
		}
		msg = key; param = 0;
		//printf("--msg=0x%x para=0x%x\n",msg,param);
	}

	/* power saving time out reset */
	if(IS_APP_KEY_MSG(msg))
	{
		if(IS_APP_KEY_PRESS_MSG(msg) && !IS_APP_KEY_USB_GROUP(msg))
		{
			sp5kModeGet(&tCurMode);			
			#if 0
			if (tCurMode==SP5K_MODE_VIDEO_RECORD&&msg==APP_KEY_PRESS_POWER)
			{
				
			}
			else
			appBuzzerPlay(BUZZER_BEEP);
			#else
			if (tCurMode!=SP5K_MODE_VIDEO_RECORD)
			{			
				appBuzzerPlay(BUZZER_BEEP);
			}	
			#endif
			
			///printf("--hdmi beep=0x%x para=0x%x\n",msg,param);
		}

		/*pbMainCB_t *ppbmainCb = appPb_MainCbPtrGet();*/
		if (isGoing2PowerOff/* || ppbmainCb->isIniting==TRUE*/)
		{
			printf("[WARN] Ignore KEY MSG because system is going to power off (0x%x)...\n",msg);
			return;
		}
		#if 0
		if(IS_APO_COUNT_DOWN)
		{
			if(IS_LCD_OUT && !IS_BACKLIGHT_ON)
			{
				LCD_BACKLIGHT_ON;
				LED_NORMAL_ON;
				if(KEY_GROUP(msg) != KEY_GROUP_DEVICE)
				{
					appPowerSaveTimeOutReset();
					printf("[INFO] LCD wake up... and drop key msg\n ");
					return;
				}
			}
		}
		#endif

		if(param != KEY_MSG_PARAM_DIRECT_RUN)
		{
			appGlobal.appGTriggerPowerOff = FALSE;
		}

		appPowerSaveTimeOutReset();
		appAutoPowerOffReset();
		if(appLcmPowerSaveStateGet() == TRUE)
		{
			//printf("--hdmi power skip\n");	
			appLcmPowerSaveReset();
			appOsdBrightnessSet();
			if(	!(IS_APP_KEY_CARD_GROUP(msg) || IS_APP_KEY_USB_GROUP(msg))
				&& msg != APP_KEY_LONG_PRESS_POWER && param != KEY_MSG_PARAM_DIRECT_RUN
				&& msg != APP_KEY_HDMI_IN && param != APP_KEY_HDMI_OUT)
			{

				//printf("--hdmi skip\n");
				return;
			}
		}
		else
		{
			appLcmPowerSaveReset();
		}

		#if 0	//CAM_TYPE_CVR
		if((IS_LCD_OUT) && (!IS_BACKLIGHT_ON))
		{
			LCD_BACKLIGHT_ON;
			LED_NORMAL_ON;
			return;
		}
		#endif

		#if HOST_DBG
		appKeyMsgDebugPrint(msg);
		#endif
		//printf("--hdmi debug print\n");
	}

	if (appStateCommonProc(msg,param) == APPSTATE_MSG_DEFAULT)
	{
		if (appStateGroupBaseProc(msg,param) == APPSTATE_MSG_DEFAULT)
		{
			appStateOnProc(msg,param);
		}
	}

	#if HOST_DBG
	if ((msg&0xffffff00) != (SP5K_MSG_AE_READY&0xffffff00))
	{
		outTime = appMsTimeGet();
		/*printf("### %d : msg=%08x param=%08x done (out-in=%d)\n\n", outTime, msg, param, outTime-inTime);*/
	}
	#endif
}

