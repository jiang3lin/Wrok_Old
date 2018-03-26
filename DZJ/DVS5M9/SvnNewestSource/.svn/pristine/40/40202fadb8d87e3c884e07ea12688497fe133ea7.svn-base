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
#include "sp5k_utility_api.h"

#include "app_aaa.h"
#include "gpio_custom.h"
#include "app_ui_para.h" 
#include "app_icon_def.h"
#include "app_menu_tab.h"
#include "app_osd_api_lcm.h"
#include "app_osd_draw_lcm.h"
#include "app_scdv_ui_para_def.h"
#include "app_rf_def.h"

static UINT32 gRfPairingTimer = 0;
UINT8 appScdvRfMode()
{
		
	uiPara_t* puiPara = appUiParaGet();
	return puiPara->SystemRemoter ;

}

void appScdvRfInit(void)
{
	#ifdef USE_DVS5M9_HW
	
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->SystemRemoter == UI_SCDV_SYSTEM_REMOTER_OFF)
	{
		appRfRelease();
	}
	else
	{
		appRfInit(puiPara->SystemRfCuid, puiPara->SystemRfType);
	}

	#endif
}


void appScdvRfPairingOsdShow(void)
{
	uiPara_t* puiPara = appUiParaGet();
	static UINT32 Times = 0;
	
	appLcmOsdClean();

	//Mode Icon
	switch(Times % 3)
	{
		case 0:
			appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SYSTEM_REMOTER_SCANNING, TRUE);
			appLcmOsdLargeIconShow(LcmOsdIconIndex1, ID_ICON_SYSTEM_REMOTER_SCANNING, FALSE);
			appLcmOsdLargeIconShow(LcmOsdIconIndex2, ID_ICON_SYSTEM_REMOTER_SCANNING, FALSE);
			break;
		case 1:
			appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SYSTEM_REMOTER_SCANNING, FALSE);
			appLcmOsdLargeIconShow(LcmOsdIconIndex1, ID_ICON_SYSTEM_REMOTER_SCANNING, TRUE);
			appLcmOsdLargeIconShow(LcmOsdIconIndex2, ID_ICON_SYSTEM_REMOTER_SCANNING, FALSE);
			break;
		case 2:
			appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SYSTEM_REMOTER_SCANNING, FALSE);
			appLcmOsdLargeIconShow(LcmOsdIconIndex1, ID_ICON_SYSTEM_REMOTER_SCANNING, FALSE);
			appLcmOsdLargeIconShow(LcmOsdIconIndex2, ID_ICON_SYSTEM_REMOTER_SCANNING, TRUE);
			break;
	}
	Times ++;

	//Tips
	appLcmOsdBottomStringShow(ID_STR_RF_SCANNING, FALSE);
	
	appLcmRefresh();
}

void appScdvRfPairingStateClose(void)
{
	uiPara_t* puiPara = appUiParaGet();

	appTimerClear(&gRfPairingTimer);
	appRfInit(puiPara->SystemRfCuid, puiPara->SystemRfType);
	
	appAutoPowerOffEnable(TRUE);
	appLcmPowerSaveEnable(TRUE);
	
	appStateChange(APP_STATE_SCDV_MENU, STATE_PARAM_NORMAL_INIT);
}

void appScdvRfPairingStateInit(void)
{
	uiPara_t* puiPara = appUiParaGet();

	LED_RULES_NORMAL;

	appAutoPowerOffEnable(FALSE);
	appLcmPowerSaveEnable(FALSE);

	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);

	appScdvRfPairingOsdShow();

	appRfInit(puiPara->SystemRfCuid, puiPara->SystemRfType);
	appRfPairingStart();
	gRfPairingTimer = appTimerSet(200, "Rf Pairing timer");

	//button setting
	appBtnEnable(BTN_ALL);
	RPrintf("appScdvRfPairingState end");
}

void appScdvRfPairingStateOnMsg(UINT32 msg, UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT8 CUID[5], RfType;

	switch (msg) 
	{
		case SP5K_MSG_TIMER:
			if(param == gRfPairingTimer)
			{
				appScdvRfPairingOsdShow();
				if(appRfParingFinish(CUID, &RfType) == TRUE)
				{
					memcpy(puiPara->SystemRfCuid, CUID, 5 * sizeof(UINT8));
					puiPara->SystemRfType = RfType;
					appUIParaSave();
					appScdvRfPairingStateClose();
				}
			}
			break;
		default:
			break;
	}
}

void appScdvRfPairingStateOnKey(UINT32 msg, UINT32 param)
{
	//DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER: 
		case APP_KEY_PRESS_OK:
			appScdvRfPairingStateClose();
			break;
	}
}

void appScdvRfPairingState(UINT32 msg,UINT32 param)
{	
	//DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:		
			RPrintf("appScdvRfPairingState");
			appScdvRfPairingStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appStateCloseDone();
			break;	
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appScdvRfPairingStateOnKey(msg,param);
			}
			else
			{
				appScdvRfPairingStateOnMsg(msg,param);
			}
			break;
	}
} 

UINT8 appRFSkipFunction()
{

	uiPara_t* puiPara = appUiParaGet();
	if(puiPara->SystemRemoter == UI_SCDV_SYSTEM_REMOTER_OFF ||puiPara->SystemRfType==0)
		return  1;
	else
		return 0;


}

