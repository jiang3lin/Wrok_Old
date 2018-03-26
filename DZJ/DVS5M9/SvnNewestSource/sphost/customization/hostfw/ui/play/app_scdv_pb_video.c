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

void appScdvPbVideoStateClose(void)
{
	//do nothing
}

void appScdvPbVideoStateInit(void)
{
	LED_RULES_NORMAL;
	LCD_BACKLIGHT_ON;

	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);

	if(appScdvWaitForDiskReady() == TRUE)
	{

	}
}

void appScdvPbVideoStateOnMsg(UINT32 msg, UINT32 param)
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

void appScdvPbVideoStateOnKey(UINT32 msg, UINT32 param)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER: 
			appStateChange(APP_STATE_HDMI_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			break;
		case APP_KEY_PRESS_OK:	//pase / play


			break;
	}
}

void appScdvPbVideoState(UINT32 msg,UINT32 param)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:		
			RPrintf("appScdvPbVideoState");
			appScdvPbVideoStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appScdvPbVideoStateClose();
			appStateCloseDone();
			break;	
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appScdvPbVideoStateOnKey(msg,param);
			}
			else
			{
				appScdvPbVideoStateOnMsg(msg,param);
			}
			break;
	}
} 


