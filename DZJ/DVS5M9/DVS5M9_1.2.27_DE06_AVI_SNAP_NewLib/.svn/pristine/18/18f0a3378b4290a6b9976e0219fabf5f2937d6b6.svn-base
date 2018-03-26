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
#include "sp5k_disp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"
#include "app_aaa.h"


void 
appNullStateClose(
	void
)

{
	//do nothing
}

void
appNullStateInit(
	void
)

{
	//sp5kModeSet(SP5K_MODE_STANDBY);
}



void
appNullStateOnMsg(
	UINT32 msg,
	UINT32 param
)

{
	//do nothing
}


void 
appNullStateOnKey(
	UINT32 msg,
	UINT32 param
)

{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_KEY_PRESS_LEFT: 
		//sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
		break;
	case APP_KEY_PRESS_RIGHT:
		//sp5kModeSet(SP5K_MODE_MULTI_PLAYBACK);
		break;
	case APP_KEY_PRESS_S2:
		break;
	}
}

void 
appNullState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) {
	case APP_STATE_MSG_INIT:		
		RPrintf("appNullState");
		appNullStateInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;	
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appNullStateOnKey(msg,param);
		}
		else{
			appNullStateOnMsg(msg,param);
		}
		break;
	}

} 


