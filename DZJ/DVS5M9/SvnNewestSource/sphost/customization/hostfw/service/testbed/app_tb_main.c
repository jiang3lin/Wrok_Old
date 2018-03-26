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
#include "app_tb_main.h"
#include "app_tb_util.h"
#include "app_lens_api.h"
#include "app_script.h"
#include "app_aaa.h"
#include "app_menu_tab.h"
#include "app_menu_core.h"
#include "app_menu_disp.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/



/*static unsigned char *gTbMenuTitleStr[] =
{
	"Test Robot",
	"+Sensor",
	"CIPA",
	"LCD Test",
	"KEY Test",
	"AE Meter",
	"+FLASH",
	"+AF",
	"BATTERY_CURVE",
	"AE debug",
	"AWB debug",
};*/


static BOOL isLcdTestNow = FALSE;
static BOOL isKeyTestNow = FALSE;


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//static unsigned char gbTbMenuStartIndex;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static appTbMenu_t gTbMenu;
static appCalibData_t *pcalibData;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 menuPosBackup; 
extern UINT32 appFirmwareUpdate(void);
extern UINT8* appVersionGet(void);
extern void dbgOsd_Print(UINT32 row, const char *format, ...);
extern void appTbCIPA_FileDelete(void);

void appStillPvStateInit(UINT32 preState);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appTB_StateInit(void);
void appTBMain_StateClose(void);
void appTBMain_StateExit(UINT32 nxtSt, UINT32 param);
//static BOOL _LcdTest(UINT32 keyMsg);      //Remove warning Lation@20121120
//static void _KeyTest(UINT32 keyMsg);       //Remove warning Lation@20121120
static void _LcdResJPGShow(char id);
static void _previewAeMeter(void);
/*-------------------------------------------------------------------------
 *  File Name : appTbMenu_SubItemDetailStr
 *------------------------------------------------------------------------*/
void appTbMenu_SubItemDetailStr(
	unsigned char nCurTab,
	unsigned short **pStrId
)
{
}


/*-------------------------------------------------------------------------
 *  File Name : appTBMenu_MainMenuDraw
 *------------------------------------------------------------------------*/
void appTBMenu_MainMenuDraw(void)
{
 }

/*-------------------------------------------------------------------------
 *  File Name : appTBMenu_MainMenuCreate
 *------------------------------------------------------------------------*/
void
appTBMenu_MainMenuCreate(
	void
)
{
 }

static void _TBMain_RefreshMenuOSD(void)
{
}

/*-------------------------------------------------------------------------
 *  File Name : appTBMenu_MainMenuReturn
 *------------------------------------------------------------------------*/
void
appTBMenu_MainMenuReturn(
	void
)
{
	_TBMain_RefreshMenuOSD();
}

void appTBMain_Action(UINT8 CurrTab)
{
	switch(CurrTab){
	default :
		break;
	case TBMENU_TEST_ROBOT:
		if (FAIL==appScriptAutoRun(IS_CARD_EXIST ? SP5K_DRIVE_SD : SP5K_DRIVE_NAND)) {
			printf("\n*** Script file not found !! **********\n\n");
			CLEAR_OSD_SRC;
			dbgOsd_Print(4, "No Script File");
			APP_SLEEP_MS(2000);
			_TBMain_RefreshMenuOSD();
		} else {
			CLEAR_OSD_SRC;
			dbgOsd_Print(4, "Wall E ...");
			APP_SLEEP_MS(5000);
			CLEAR_OSD_SRC;
			appTBMain_StateExit(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case TBMENU_SENSOR_TEST:
		appTBMain_StateExit(APP_STATE_TEST_SENSOR, STATE_PARAM_NORMAL_INIT);
		break;
	case TBMENU_CIPA:
		pcalibData->CIPASign^=1;
		pcalibData->CIPACapCnt=0;
		appCalibDataSave();
		appTbCIPA_FileDelete();
		appPowerOffUiMsgSend(APP_POWER_OFF_NORMAL);
		break;
	case TBMENU_LCD_TEST:
		isLcdTestNow = TRUE;
		CLEAR_OSD_SRC;
		sp5kModeSet(SP5K_MODE_STANDBY);
		dbgOsd_Print(1, "Left --- MENU --- Right");
		sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
		_LcdResJPGShow('1');
		break;
	case TBMENU_KEY_TEST:
		CLEAR_OSD_SRC;
		dbgOsd_Print(1, "KEY TEST");
		isKeyTestNow = TRUE;
		break;
	case TBMENU_AE_METER:
		_previewAeMeter();
		break;
	case TBMENU_FLASH:
		appTBMain_StateExit(APP_STATE_TEST_FLASH, STATE_PARAM_NORMAL_INIT);
		break;
	case TBMENU_AF:
		appTBMain_StateExit(APP_STATE_TEST_AF, STATE_PARAM_NORMAL_INIT);
		break;
	case TBMENU_BAT_CURVE:
		appTBMain_StateExit(APP_STATE_TEST_BAT_CURVE, STATE_PARAM_NORMAL_INIT);
		break;
	case TBMENU_AE_DEBUG:
		pcalibData->aedebug^=1;
		appCalibDataSave();
		break;
	case TBMENU_AWB_DEBUG:
		pcalibData->awbdebug^=1;
		appCalibDataSave();
		break;
	}
}
void
appTBMain_StateClose(
	void
)
{
	//do nothing
}

void
appTBMain_StateExit(UINT32 nxtSt, UINT32 param)
{
	appTBMain_StateClose();
	appStateChange(nxtSt, param);
}

void
appTBMain_StateInit(
	void
)
{
	gTbMenu.vmTotalTabs = TBMENU_TOTAL;
	gTbMenu.vmCurTab = 0;

	/*-- Get Calib data */
	pcalibData = appCalibDataGet();
	/* Disable PowerSave & Battery Check */
	appPowerSavingEnableSet(0);
	appBatteryCheckEnableSet(0);

	/*-- Create Main Menu */
	menuPosBackup = MENU_MARK_TESTBENCH | 0x0a0d0100; 
	menuProcCurrPosSet (menuPosBackup);
	menuProcDisp();
}



void
appTBMain_StateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	default:
		break;
	}
}


void
appTBMain_StateOnKey(
	UINT32 msg,
	UINT32 param
)

{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch(msg){
			case APP_KEY_PRESS_SET:
			appTBMain_Action(gTbMenu.vmCurTab);
			break;
		case APP_KEY_PRESS_UP:
			if (gTbMenu.vmCurTab ==0){
				gTbMenu.vmCurTab = gTbMenu.vmTotalTabs-1;
			}else{
				gTbMenu.vmCurTab--;
			}
			menuProcMoveUp();
			menuProcDisp();
			break;
			case APP_KEY_PRESS_DOWN:
			if (gTbMenu.vmCurTab>=(gTbMenu.vmTotalTabs-1)){
				gTbMenu.vmCurTab =0;
			}else{
				gTbMenu.vmCurTab++;
			}
			menuProcMoveDown(); 
			menuProcDisp();
			break;
		case APP_KEY_PRESS_DEL:
		case APP_KEY_PRESS_MENU:   
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_NORMAL_INIT);
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS,	2000);	
			break;
		default:break;
			
	}
}

void
appTBMain_State(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appTBMain_State");
		appTBMain_StateInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		/*-- Return to View */
		appTBMain_StateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appTBMain_StateOnKey(msg,param);
		}
		else{
			appTBMain_StateOnMsg(msg,param);
		}
		break;
	}

}

static void _LcdResJPGShow(char id)
{
	char szLCDResFName[]="A:\\RO_RES\\LCD\\LCDTEST1.JPG";

	if (id>='1' && id<='5') {
		szLCDResFName[21] = id;
		appJpgFileDraw(JPG_DRAW_IMG_LAYER, szLCDResFName, 0,0,0,0);
	} else {
		appJpgFileDraw(JPG_DRAW_IMG_LAYER, "A:\\RO_RES\\UI\\JPG\\SCRBLACK.JPG", 0,0,0,0);
	}
}
#if 0
static BOOL _LcdTest(UINT32 keyMsg)
{
	static UINT8 fileno=1;

	DBG_PRINT("LCD Test : keyMsg(0x%08x) \n", keyMsg);
	if(keyMsg == APP_KEY_PRESS_LEFT){
		if(fileno > 1) 	fileno--;
		else fileno=5;
		_LcdResJPGShow('0'+fileno); /* LCDTEST1~5.JPG */
		return 1;
	}else if(keyMsg == APP_KEY_PRESS_RIGHT ){
		if(fileno < 5) fileno ++;
		else fileno=1;
		_LcdResJPGShow('0'+fileno); /* LCDTEST1~5.JPG */
		return 1;
	}else if(keyMsg == APP_KEY_PRESS_MENU){
		fileno=1;
		return 0;
	}

	return 1;

}


static void _KeyTest(UINT32 keyMsg)
{
	UINT8 *szkeyGroup[] = {
							"ERR",
							"S1",
							"S2",
							"WIDE",
							"TELE",
							"UP",
							"DOWN",
							"LEFT",
							"RIGHT",
							"MENU",
							"DEL",
							"SET",
							"MODE",
							"PB",
						     };
	UINT8 *szdevGroup[] = {
							"ERR",
							"TV",
							"USB",
							"CARD",
						     };
	UINT8 *szkeyStatus[] = {"ERR","RELEASE", "PRESS"};
	UINT8 szkeyAct[32];

	if(KEY_GROUP(keyMsg) < KEY_GROUP_DEVICE) {
		CLEAR_OSD_SRC;
		sprintf(szkeyAct,"%s %s\n",szkeyGroup[(KEY_GROUP(keyMsg) >> 8)], szkeyStatus[KEY_STATE(keyMsg)]);
	}
	else if((KEY_GROUP(keyMsg)&0xF000) == KEY_GROUP_DEVICE) {
		CLEAR_OSD_SRC;
		sprintf(szkeyAct,"%s %s\n",szdevGroup[((KEY_GROUP(keyMsg) >> 8) & 0x0F)], szkeyStatus[KEY_STATE(keyMsg)]);
	}
	printf("Key Test =>%s\n",szkeyAct);

	dbgOsd_Print(4, szkeyAct);
}
#endif

/*-------------------------------------------------------------------------
 * Function : _previewAeMeter
 *------------------------------------------------------------------------*/
static void _previewAeMeter(void)
{
	UINT16 aeLuma;
	SINT32 aeExp;
	UINT32 aeAgc;
	UINT32 aeIris;
	UINT8 aeStep;
	sp5kAeResult_t aeResult;

	CLEAR_OSD_SRC;

	while(1) {
		if (tbUtil_TestLoopAbort()) break;

		/*-- Run & get luma */
		tbUtil_QuickSweepHostMessage(2500);
		tbUtil_Beep();
		CLEAR_OSD_SRC;

		aeLuma = tbUtil_GetAeResult(&aeResult);
		if (aeLuma) {
			aeExp = aeResult.evInfo.expIdx;
			aeAgc = aeResult.evInfo.agcIdx;
			aeLuma = aeResult.aeLuma;
			aeStep = aeResult.aeStep;
			aeIris = appLensQuery(LENS_QUERY_IRIS_SEG_POS);

			dbgOsd_Print(2, "aeLuma(%d)", aeLuma);
			dbgOsd_Print(3, "aeExp(%d) aeAgc(%d)", aeExp, aeAgc);
			dbgOsd_Print(4, "aeStep(%d)", aeStep);
			dbgOsd_Print(5, "aeIris(%d)", aeIris);
		} else {
			dbgOsd_Print(4, "AE Error");
		}
	}

}

