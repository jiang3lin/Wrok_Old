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

#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_disp_api.h"

#include "gpio_custom.h"
#include "app_stringid.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_menu_frameosd.h"
#include "app_error_msg.h"
#include "app_jpg_draw.h"
#include "app_res_def.h"
#include "app_battery.h"
#include "app_calib_api.h"
#include "app_camcfg_main.h"
#include "app_script.h"
#include "customization/solution_def.h"/*willow++ for ShuoYin key@0527*/
#include "app_menu_tab.h"
#include "app_menu_core.h"
#include "app_menu_disp.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/



/*static unsigned char *gCcMenuTitleStr[] =
{
	"FW Update",
	"Factory Default",
	"USB ISP",
	"PV SaveRaw",
	"CAP SaveRaw",
	"RawBatch Enable",
	"Prefh Stamp",
	"AF Full Scan",
};*/

 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//static unsigned char gbCcMenuStartIndex;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
//static appCcMenu_t gCcMenu;
static appCalibData_t *pcalibData;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 menuPosBackup;  
extern UINT32 appFirmwareUpdate(void);
extern UINT8* appVersionGet(void);
extern void dbgOsd_Print(UINT32 row, const char *format, ...);
void appStillPvStateInit(UINT32 preState);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appCcMain_StateInit(void);
void appCcMain_StateClose(void);
void appCcMain_StateExit(UINT32 nxtSt, UINT32 param);
void appFactorySettingApply(void);

/*-------------------------------------------------------------------------
 *  File Name : appCcMenu_SubItemDetailStr
 *------------------------------------------------------------------------*/
void appCcMenu_SubItemDetailStr(
	unsigned char nCurTab,
	unsigned short **pStrId
)
{
 }


/*-------------------------------------------------------------------------
 *  File Name : appCcMenu_MainMenuDraw
 *------------------------------------------------------------------------*/
void appCcMenu_MainMenuDraw(void)
{
}

/*-------------------------------------------------------------------------
 *  File Name : appCcMenu_MainMenuCreate
 *------------------------------------------------------------------------*/
void
appCcMenu_MainMenuCreate(
	void
)
{
}

static void _CcMain_RefreshMenuOSD(void)
{
}

/*-------------------------------------------------------------------------
 *  File Name : appCcMenu_MainMenuReturn
 *------------------------------------------------------------------------*/
void
appCcMenu_MainMenuReturn(
	void
)
{
	_CcMain_RefreshMenuOSD();
}


void appCcMain_Action(UINT8 CurrTab)
{
	switch(CurrTab){
	default :
		break;
	case CCMENU_CARD_FW_UPDATE:
		sp5kAeModeSet(0);
        sp5kAwbModeSet(0);
		if (appFirmwareUpdate() != SUCCESS) {
			appErrMsgWithSound(ERR_MSG_FILE_ERROR,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_2SEC);
		}
		break;
	case CCMENU_FAC_DEFAULT:
		appFactorySettingApply();
		break;
	/*case CCMENU_USB_ISP:
		pcalibData->UsbSetting = (UINT8)(!pcalibData->UsbSetting);
		break;*/
	case CCMENU_PV_SAVE_RAW:
		pcalibData->PvRawSign = (UINT8)(!pcalibData->PvRawSign);
		break;
	case CCMENU_CAP_SAVE_RAW:
		pcalibData->StillRawSign = (UINT8)(!pcalibData->StillRawSign);
		break;
	case CCMENU_RAWBATCH_ENABLE:
		pcalibData->rawBatchEnable= (UINT8)(!pcalibData->rawBatchEnable);		
		break;
	case CCMENU_PREFH_STAMP:
		pcalibData->prefhStamp= (UINT8)(!pcalibData->prefhStamp);
		break;
	case CCMENU_AF_FULL_SCAN:
		pcalibData->afFullScan= (UINT8)(!pcalibData->afFullScan);
		break;
	}
}

void
appCcMain_StateClose(
	void
)
{
	appCalibDataSave();
}

void
appCcMain_StateExit(UINT32 nxtSt, UINT32 param)
{
	appCcMain_StateClose();
	appStateChange(nxtSt, param);
}

void
appCcMain_StateInit(
	void
)
{
	/*-- Get Calib data */
	pcalibData = appCalibDataGet();
	/* Disable PowerSave & Battery Check */
	appPowerSavingEnableSet(0);
	appBatteryCheckEnableSet(0);

	/*-- Create Main Menu */
	menuPosBackup = MENU_MARK_CAMCONFIG | 0x0a0c0100;
	menuProcCurrPosSet (menuPosBackup);
	menuProcDisp();
}



void
appCcMain_StateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
}


void
appCcMain_StateOnKey(
	UINT32 msg,
	UINT32 param
)

{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_KEY_PRESS_LEFT:
		break;
	case APP_KEY_PRESS_RIGHT:
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
		break;
	case APP_KEY_PRESS_MENU:
		CLEAR_OSD_SRC;
		appCcMain_StateExit(APP_STATE_CALIBRATION, STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_DEL:
		CLEAR_OSD_SRC;
		appCcMain_StateExit(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		break;
	}

}

void
appCcMain_State(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appCcMain_State");
		appCcMain_StateInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		/*-- Return to View */
		appCcMain_StateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appCcMain_StateOnKey(msg,param);
		}
		else{
			appCcMain_StateOnMsg(msg,param);
		}
		break;
	}

}



