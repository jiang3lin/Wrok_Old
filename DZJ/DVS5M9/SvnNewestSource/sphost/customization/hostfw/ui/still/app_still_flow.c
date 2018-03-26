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

#include "app_still.h"
#include "app_state_ctrl.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_ui_para.h" 

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//The carry message mask 0xFF
#define ST_SELF_TWO_SECOND 2 
#define ST_SELF_TEN_SECOND 10
#define ST_AAA_NORMAL 0
#define ST_AAA_FORCE_CAPTURE 1

//The flow control mask 0xF00 
#define ST_CTL_RETURN_TO_LAST_STATE 0x100
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct 
{
	UINT32 state;
	UINT32 param;
} state_flow_unit_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
state_flow_unit_t flowSingle[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_STILL_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};

#if PANORAMA_MODE
state_flow_unit_t flowPanorma[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_STILL_PAN0RAMA ,	0},
	{APP_STATE_STILL_PREVIEW, 		0},
};
#endif

state_flow_unit_t flowBurst[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_BURST_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};

state_flow_unit_t flowSelfTwoSec[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_STILL_SELF_TIMER,	ST_SELF_TWO_SECOND},
	{APP_STATE_STILL_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};

state_flow_unit_t flowSelfTenSec[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_STILL_SELF_TIMER,	ST_SELF_TEN_SECOND},	
	{APP_STATE_STILL_AAA, 			ST_AAA_FORCE_CAPTURE},
	{APP_STATE_STILL_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};

state_flow_unit_t flowSelfDouble[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_STILL_SELF_TIMER,	ST_SELF_TEN_SECOND},	
	{APP_STATE_STILL_AAA, 			ST_AAA_FORCE_CAPTURE},	
	{APP_STATE_STILL_CAPTURE, 		0},
	{APP_STATE_STILL_SELF_TIMER,	ST_SELF_TWO_SECOND},
	{APP_STATE_STILL_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};

state_flow_unit_t flowSmileDetect[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_STILL_SMILE_DETECT,	0},	
	{APP_STATE_STILL_CAPTURE, 		ST_CTL_RETURN_TO_LAST_STATE},
	{APP_STATE_STILL_PREVIEW, 		0},
};

state_flow_unit_t* stillFlowList[] = 
{
	[UI_DRIVE_OFF] 		= flowSingle,
	[UI_DRIVE_TIMER_2S] = flowSelfTwoSec,
	[UI_DRIVE_TIMER_10S]= flowSelfTenSec,
	[UI_DRIVE_DOUBLE]	= flowSelfDouble,
	[UI_DRIVE_BURST]	= flowBurst,
};

state_flow_unit_t flowBlinkDetect[] = 
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},	
	{APP_STATE_STILL_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};


state_flow_unit_t* pCurFlow = flowSingle;
UINT32 CurStateIdx = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 IsHDR,IsHDRNightScene;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void 
appStillFlow_SetDriveFlow(
	UINT32 driveMode,
	UINT32 aeb,
	UINT32 viewMode,
	UINT32 sceneMode
)
{
	uiPara_t* puiPara = appUiParaGet();

	if((sceneMode == SCENE_SMILE_DETECTION && viewMode == SCENE_MODE)||puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT) 
	{
		DBG_PRINT("%s: smile", __FUNCTION__);
		pCurFlow = flowSmileDetect;
	}
	#if PANORAMA_MODE
	else if (sceneMode == SCENE_PANORAMA&& viewMode == SCENE_MODE)
	{
		DBG_PRINT("%s: Panorama", __FUNCTION__);
		pCurFlow = flowPanorma;
	}
	#endif
	else if ((sceneMode == SCENE_BLINK && viewMode == SCENE_MODE)||puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
	{
		DBG_PRINT("%s: Blink", __FUNCTION__);
		pCurFlow = flowBlinkDetect;
	}
	else if(IsHDR || IsHDRNightScene) 
	{
		DBG_PRINT("%s: HDR", __FUNCTION__);
		pCurFlow = flowSingle;
	}
	else if(aeb)
	{
		DBG_PRINT("%s: aeb", __FUNCTION__);
		pCurFlow = stillFlowList[UI_DRIVE_BURST];
	}
	#if CAM_TYPE_CVR
	else if(puiPara->PhotoBurst)
	{
		pCurFlow = flowBurst;
	}
	#endif
	else 
	{
		DBG_PRINT("%s: other:%d", __FUNCTION__, driveMode);
		pCurFlow = stillFlowList[driveMode];
	}	
	CurStateIdx = 0;
}
void 
appStillFlow_GetNextState(
	UINT32* pState,
	UINT32* pParam
)
{
	// check cur state param
	if(FLOW_EXTRCT_CTRL_PAR(pCurFlow[CurStateIdx].param) == ST_CTL_RETURN_TO_LAST_STATE) 
	{
		CurStateIdx--;	
	}	
	else 
	{		
		CurStateIdx++;
	}

	*pState = pCurFlow[CurStateIdx].state;
	*pParam = pCurFlow[CurStateIdx].param + STATE_PARAM_FREE_CARRY;
	DBG_PRINT("%s: st= 0x%x, par+0xF000= 0x%x ", __FUNCTION__, *pState, *pParam);
}

