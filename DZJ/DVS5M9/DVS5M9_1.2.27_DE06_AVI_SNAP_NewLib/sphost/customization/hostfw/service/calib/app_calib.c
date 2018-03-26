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
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_calib.h"
#include "app_calib_api.h"
#include "app_calib_private.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_disp_api.h"
#include "app_stringid.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_menu_frameosd.h"
#include "app_error_msg.h"
#include "app_jpg_draw.h"
#include "app_res_def.h"
#include "app_battery.h"
#include "app_tb_util.h"
#include "app_lens_api.h"
#include "Sp5k_usb_api.h"/*willow++ for PC CAMERA@20100719*/
#include "app_still.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_ts_api.h"


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
appCalibmenu_t gCalibMenu;
UINT8 CalibMode=0;
appCalibData_t *pCalib;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appFirmwareUpdate(void);
extern UINT8* appVersionGet(void);
void tbUtil_CalibCdspSet(BOOL isObOn);
void appCalibReport(BOOL isWrFile);
extern void appMenuCalibrationStateInit(void);
extern UINT8 appCalibAPITestSettingGet(void);//haiyan.he added
extern void appCalibLSC(void);//haiyan.he added
extern void appCalibPVOB(void);//haiyan.he added
extern UINT8 appCalibVideoStampSettingGet(void);//haiyan.he added
extern UINT8 appCalibDzoomEnableGet(void);//haiyan.he added
extern void  menuProcDisp(void);//haiyan.he added
extern UINT8 appCalibAPITestSettingSet(UINT8 enable);//haiyan.he added
extern UINT8 appCalibVideoStampSettingSet(UINT8 enable);//haiyan.he added
extern UINT8 appCalibDzoomEnableSet(UINT8 enable);//haiyan.he added




/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appCalibStateInit(void);

UINT8 calib_wb_done = 0;

/*-------------------------------------------------------------------------
 *  File Name : appCalibZoomLens
 *------------------------------------------------------------------------*/
 void appCalibZoomLens(void)
 {
	appStateChange(APP_STATE_CALIB_ZOOM, STATE_PARAM_NORMAL_INIT);
 }

 void appCalibAction(UINT8 CurrTab)
{
	switch(CurrTab)
	{
		case CALIBMENU_CALIB_INFO:
			appCalibReport(1);
			break;
		case CALIBMENU_FOCUS_CLBT:
			if(CalibMode!=CALIB_MODE_AF)
			{
				appCalibAfInit();
			}
			break;
		case CALIBMENU_OB_CLBT:
			appCalibOb();
			break;
		case CALIBMENU_MSHUTTER_CLBT:
			appCalibMShutter();
			break;
		case CALIBMENU_ISO_CLBT:
			appCalibIso();			
			break;
		case CALIBMENU_ISO_OB_CLBT:
			appCalibIsoOb();
			break;
		case CALIBMENU_WB_CLBT:
			appCalibAwb();	
			break;			
		case CALIBMENU_FLASH_CLBT:
			appCalibFlash();
			break;
		case CALIBMENU_VIEW_DPC_CLBT:
			appCalibViewBp();
			break;
		case CALIBMENU_CAPTURE_DPC_CLBT:
			appCalibCaptureBp();
			break;
		case CALIBMENU_BATTERY_CLBT:
			break;
		case CALIBMENU_CAM_CONFIG:
			/*-- switch to CAMCFG_MAIN state */
			appStateChange(APP_STATE_TEST_CAMCFG, STATE_PARAM_NORMAL_INIT);
			break;	
		case CALIBMENU_TEST_BENCH:
			/*-- switch to TEST_MAIN state */
			appStateChange(APP_STATE_TEST_MAIN, STATE_PARAM_NORMAL_INIT);
			break;
		case CALIBMENU_PC_CAMERA:
			/*LCD_STANDBY;
			LCD_BACKLIGHT_OFF; */
			if(appCalibAPITestSettingGet())
			{
				appCalibAPITestSettingSet(0);
				
			}	
			else
			{
				sp5kUsbSwCon(0) ;
				appStateChange(APP_STATE_STILL_PREVIEW ,STATE_PARAM_NORMAL_INIT);
				appModeSet(SP5K_MODE_VIDEO_PREVIEW);
				sp5kUsbModeSet(SP5K_USB_MODE_APITEST );		
				sp5kUsbSwCon(1);
				appCalibAPITestSettingSet(1);
				sp5kAeModeSet(2);
			}	
			appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
			break;
		case CALIBMENU_STAND_ALONE_LSC:
			appCalibLSC();
			break;
		case CALIBMENU_PV_OB_CAL:
			appCalibPVOB();
			break;
		case CALIBMENU_VIDEO_DEBUG_DATESTAMP:
			{
			if(appCalibVideoStampSettingGet())
				appCalibVideoStampSettingSet(0);
			else
				appCalibVideoStampSettingSet(1);
			}
			appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
			break;
		case CALIBMENU_DIGITALZOOM:		
			if(appCalibDzoomEnableGet()){
				appCalibDzoomEnableSet(0);
			}else{
				appCalibDzoomEnableSet(1);
			}
			appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
			break;
		case CALIBMENU_TOUCHPANEL:		
			CLEAR_OSD_SRC;
			CLEAR_PIP_SRC;
			appGuiObjClr(-1);
			appStateChange(APP_STATE_CALIB_TS, STATE_PARAM_NORMAL_INIT);
			break;
		default:
			break;
			
	
		}
	
}

void 
appCalibStateClose(
	void
)

{
	//do nothing
}

void
appCalibStateInit(
	void
)
{
	tbUtil_PrintfCtrl(1); //always turn on debug print
	appPowerSavingEnableSet(FALSE); //disable power saving function
	
	appBtnEnable(BTN_ALL);
	HOST_ASSERT(CalibMode<CALIB_MODE_MAX);

	switch (CalibMode) {
	case CALIB_MODE_IDLE:
		appLensMShutCallbackRegister();
		pCalib = appCalibDataGet();
		appPowerSavingEnableSet(0);
		appBatteryCheckEnableSet(0);
 		appOsd_GfxOsdRefreshSet(FALSE);
		CLEAR_OSD_SRC;
 		appJpgFileDraw(JPG_DRAW_PIP_LAYER, "A:\\RO_RES\\UI\\JPG\\SCRBLACK.JPG", 0,0,0,0);
		sp5kDispDevAttrSet(JPG_DRAW_PIP_LAYER, 1, 0, 0, 0);
		appMenuCalibrationStateInit();
  		appOsd_GfxOsdRefreshSet(TRUE);
		break;
	case CALIB_MODE_AAA_LAB:
		appModeAaaLab_OnEnter();
		break;
	}
}

 void
appCalibStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	HOST_ASSERT(CalibMode<CALIB_MODE_MAX);

	switch (CalibMode) 
	{
		case CALIB_MODE_IDLE:
			break;
		case CALIB_MODE_AAA_LAB:
			appModeAaaLab_OnMsg(msg, param);		
			break;
		case CALIB_MODE_AF:
			appCalibAf(msg, param);	
			break;
	}
}
 void 
 appCalibStateOnKey(
	 UINT32 msg,
	 UINT32 param
 )
 
 {
 	menuCtrl_t *pMenu;
	
	/*appBeep(TRUE, FALSE);*/

#if 1
	if (menuHdlGet(&pMenu) != SUCCESS) {
		/*printf("menu hdl get fail\n");*/
	};
#endif
	 DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
 
	 if (CalibMode==CALIB_MODE_AAA_LAB)
		 return;
 
	 switch (msg) {
	 case APP_KEY_PRESS_LEFT: 
		 //sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
		 break;
	 case APP_KEY_PRESS_RIGHT:
		 //sp5kModeSet(SP5K_MODE_MULTI_PLAYBACK);
		 break;
	 case APP_KEY_PRESS_UP:
		 menuProcMoveUp();
		 menuProcDisp();
		 break;
 
	 case APP_KEY_PRESS_DOWN:
		 menuProcMoveDown();
		 menuProcDisp();
		 break;
	 case APP_KEY_PRESS_SET:
		 appCalibAction(pMenu->item);
		 break;
	 case APP_KEY_PRESS_DEL:
	 case APP_KEY_PRESS_MENU:	
		 CLEAR_OSD_SRC;
		 appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		 sp5kTimeDelay(SP5K_TIME_DELAY_1MS,  2000);  
		 break;
	 }
 }

void 
appCalibState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:		
		RPrintf("appCalibState");
		appCalibStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appCalibDataSave();
		appStateCloseDone();
		break;	
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appCalibStateOnKey(msg,param);
 		}
		else
		{
			appCalibStateOnMsg(msg,param);
		}
		break;
	}

} 



