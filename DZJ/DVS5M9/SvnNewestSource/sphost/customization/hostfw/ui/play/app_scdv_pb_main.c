/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2010 by DXG Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of DXG        *
 *  Technology Co., Ltd. All rights are reserved by DXG Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of DXG Technology Co., Ltd.                       *
 *                                                                        *
 *  DXG Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  DXG Technology Co., Ltd.                                          *
 *  MIN ZHI IND., PARK,					            *
 *  LONG HUA,BAOAN,SHEN ZHEN,P.R.O.C.                                           *
 *                                                                        *
 *  Author: Wujf                                                  *
 *                                                                         *
 **************************************************************************/
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_disp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"

#include "app_aaa.h"
#include "gpio_custom.h"

void appScdvPbMainStateClose(void)
{
	#if 0
	gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
	gpbIsViewIdle = TRUE;
	#if (KIT_WITH_HDMI)
	#if HDMI_MENU_BY_JPG
		if(IS_HDMI_IN){
			pipHDMIMenuFlag = 0;
			sp5kPipTerm();
		}
	#else
		AppPbDestroyshowJPGInPIP();
	#endif
	//AppPbDestroyshowJPGInPIP();
	#endif
	if (appLensQuery(LENS_QUERY_MOTION_STATE)) 
	{
		appLensStopWait();
	}
	#endif
}

void appScdvPbMainStateInit(void)
{
	LED_RULES_NORMAL;
	LCD_BACKLIGHT_ON;

	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);

	if(appScdvWaitForDiskReady() == TRUE)
	{

	}

	#if 0
	printf("filetype=%d\n",puiPara->fileType);	
	appMsgFlush(APP_MSG_UI_QUE);  /*add for mantis bug 0047610*/
	appAutoPowerOffEnable(TRUE);/*++ for  power saving while playback main state*/
	if(puiPara->fileType == FILETYPE_VIDEO)
	{
		__FUNC_TRACK__;
		appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
	}
	else if(puiPara->fileType == FILETYPE_JPG)
	{
		__FUNC_TRACK__;
		appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
		appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	}
	else if(puiPara->fileType == FILETYPE_EVENT)
	{
		__FUNC_TRACK__;
		appCdfsFileFolderSet(CDFS_FILE_FOLDER_EVENT);
		appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
	}
	else if(puiPara->fileType == FILETYPE_AUDIO)
	{
		__FUNC_TRACK__;
		appCdfsFileFolderSet(CDFS_FILE_FOLDER_AUDIO);
		appCdfsFolderInit(CDFS_FILE_FOLDER_AUDIO);
	}
	
	if (!IS_PLAYBACK_GROUP(appPreviousStateGet())&& (!pbflag)) 
	{		
		PB_CLR_ALL; 
		playDeviceConfig();
		playInitCreate();
		gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
	}
	else
	{
	}
	if (gpbMainCb.mainStat==PB_MAIN_STAT_NULL) 
	{
		appPbMainState_Init();
	}
	else if(IS_MENU_STATE(appPreviousStateGet()))
	{		
		CLEAR_OSD_SRC;			
		gpbMainCb.mainStat = PB_MAIN_STAT_MENU_SUSPEND;
		
		if(puiPara->fileType != FILETYPE_JPG || IS_HDMI_IN)
		{				
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
			appPbPIPInit();
		}
		pbflag = 0;
		sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2 /* INIT Triggered */);
	}
	else if (appPrevStat == APP_STATE_PB_VIDEO)
	{
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appPbPIPInit();
		pbflag = 0;
		sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2);
	}
	else if (appPrevStat == APP_STATE_PB_IMG_DEL)
	{
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appPbPIPInit();
		pbflag = 0;
		sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2);
	}
	
	#if HDMI_MENU_BY_JPG
	#if (KIT_WITH_HDMI)   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		menuHDMIPIPInit();
	}
	#endif
	#endif
	
	gpbMainCb.isIniting = TRUE;
	#endif
}

void appScdvPbMainStateOnMsg(UINT32 msg, UINT32 param)
{
	switch (msg) 
	{
		case SP5K_MSG_DISK_MOUNT_COMPLETE:
		case APP_UI_MSG_VIEW_WAIT_DISK:
			if(appScdvWaitForDiskReady() == TRUE)
			{

			}			
			break;
	}
}

void appScdvPbMainStateOnKey(UINT32 msg, UINT32 param)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER: 
			appStateChange(APP_STATE_SPORTDV_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
			break;
		case APP_KEY_PRESS_OK:
			appStateChange(APP_STATE_HDMI_PB_VIDEO, STATE_PARAM_NORMAL_INIT);
			break;
	}
}

void appScdvPbMainState(UINT32 msg,UINT32 param)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:		
			RPrintf("appScdvPbMainState");
			appScdvPbMainStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appScdvPbMainStateClose();
			appStateCloseDone();
			break;	
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appScdvPbMainStateOnKey(msg,param);
			}
			else
			{
				appScdvPbMainStateOnMsg(msg,param);
			}
			break;
	}
} 


