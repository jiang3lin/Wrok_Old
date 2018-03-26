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
#include "app_osd.h"
#include "sp5k_capture_api.h"

#include "app_state_ctrl.h"
#include "app_still.h"
#include "app_view_osd.h"
#include "app_strobe.h"
#include "app_view_param.h"

#include "sp5k_ftrack_api.h"
#include "app_mmface_detect.h"


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
UINT32 noFaceTimeCount;
UINT32 smileDetectDelay;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appStillCheckReady(UINT32 question);
extern UINT32 appFdFaceDataGetEx(appFdResult_t *fdResult);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void 
appStillSmileStateInit(
	UINT32 preState
)
{
	UINT32 cfg = 0;

	/* osd */
	if(preState != APP_STATE_STILL_AAA) 
	{
		gStillCB.focusStatus = FOCUS_STATUS_VIEW;
		appViewOsdShow(0,0,gStillCB.osd);
		appViewOsd_FocusWindowDraw(FALSE, 0);	
	}			
	/* led */
	if(!appStrobeQueryReady() && gStillCB.flashMode!=UI_FLASH_OFF)
	{
		LED_FLICKER_STROBE_CHARGE
	}
	else 
	{
		LED_NORMAL_ON
	}
	/* setting */
	appAePreviewRestore();
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, &cfg);	
	appStill_SetFlash(gStillCB.flashMode);
	appStill_SetFaceTracking(VIEW_FT_STATUS_RUN);
	noFaceTimeCount=0;
	smileDetectDelay = 3;
	if(gStillCB.s2SnapTicket == 1) 
	{	
		gStillCB.s1AaaTicket =0;
		gStillCB.s2SnapTicket =0;
		appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
	}
}

void 
appStillSmileDetectCheck(
	UINT32* nface,
	UINT32* smile
)
{
	UINT32 faceIdx=0;
	appFdResult_t faceInfo;
	appFdFaceDataGetEx(&faceInfo);
	*nface = faceInfo.totalFaceNum;
	*smile = faceInfo.rectFace[faceIdx].isSmile;
}



void 
appStillSmileStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 next_state, next_param, nface, smile, status;	
	
	switch ( msg) 
	{
	case SP5K_MSG_AE_READY:	
		appAePreviewAdjust(param);
		break;
	case SP5K_MSG_FACE_TRACK_READY:
	case SP5K_MSG_FACE_TRACK_TIMEOUT:
		{
			UINT32 nface=0;
			SINT32 err=0;
			sp5kFaceTrackFaceRes_t facelist[12];
			err = appFdResultGet(param, &nface, facelist);

			if (err!=-1)
			{
				appFdDataUpdate(nface,facelist);
				if (smileDetectDelay == 0) 
				{
					appStillSmileDetectCheck(&nface,&smile);
					 if(nface && smile) 
					 {	
						status = appStillCheckReady(CAN_I_START_SNAP);	

						if(status != STILL_STATE_FLASH_CHARGIN ) 
						{						
							appStillFlow_GetNextState(&next_state, &next_param);

							appStateChange(next_state, next_param);
						}
					 }
					 else if (nface ==0)
					 {
						appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
						appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);		
					 }	
				}
				else
				{
					smileDetectDelay--;
				}
				appFd_FaceRectUpdateMsgSend();
			}	
		}
		break;
	case SP5K_MSG_FACE_TRACK_STOP:
		break;
	case SP5K_MSG_FACE_TRACK_INVALID:
		break;
	case APP_UI_MSG_FACE_DETECT_OSD_UPDATE:
		appStillOsd_UpdateSmileOsd(&nface, &smile, TRUE);
		noFaceTimeCount = (nface)? 0 : noFaceTimeCount+1;
		status = appStillCheckReady(CAN_I_START_SNAP);	
		if(status == STILL_STATE_MEMORY_FULL ) 
		{
			appStillOsd_UpdateSmileOsd(&nface, &smile, FALSE);
			appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
			appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);		
		}	
		break;
	case APP_STATE_MSG_ABORT:
		appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
		appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
	default:
		break;
	}
}

void 
appStillSmileStateOnKey(
	UINT32 msg,
	UINT32 param
)	
{
	switch (msg) 
	{		
	case APP_KEY_PRESS_S2:
		appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
		break;
	default:		
		break;
	}

}

void 
appStillSmileState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appStillSmileState");
		appStillSmileStateInit(appPreviousStateGet());
		break;		
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;			
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			
			appStillSmileStateOnKey(msg,param);
		}
		else
		{
			appStillSmileStateOnMsg(msg,param);
		}
		break;
	}

}

