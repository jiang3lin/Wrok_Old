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

 /*s Add by Aries 09/10/30*/

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
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "App_osd_api.h"
#include "App_com_api.h"

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

/*s Add by Aries 09/11/05*/
UINT32 BlinkSaveSelect =0; /* has user selected save/no save & select result */
/*e Add by Aries 09/11/05*/

/*s added by Leo Li @20091125*/
enum{
	SAVE_THIS_PHOTO_YES= 0x00,
	SAVE_THIS_PHOTO_NO ,
};
/*e added by Leo Li @20091125*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appStillCheckReady(UINT32 question);
extern void appViewOsd_DlgSetandDraw( unsigned short wTitle,  unsigned short SelectIndex);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  Function Name : appStillBlinkStateInit
 *  Description :   
 *------------------------------------------------------------------------*/
void 
appStillBlinkStateInit(
	UINT32 preState
)
{
	DBG_PRINT("%s : begin \n",__FUNCTION__);	
/*s added by Leo @20091125*/
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
       appViewOsd_DlgSetandDraw(0,0);
	APP_OSD_REFRESH_ON;
	appDispLayer_OSD_Ctrl(1); /*enable OSD layer*/
/*e added by Leo @20091125*/
	DBG_PRINT("%s : end \n",__FUNCTION__);	
}


/*-------------------------------------------------------------------------
 *  Function Name : appStillBlinkConfirmStoreSet
 *  Description :   
 *------------------------------------------------------------------------*/
void 
appStillBlinkConfirmStoreSet(
	UINT32 enable
)
{	
	appStill_SetQV(0);
	appStill_CfgCaptureConfirmStore(0);
	appStill_SetCaptureConfirmStore(enable);

}




/*-------------------------------------------------------------------------
 *  Function Name : appStillBlinkStateOnMsg
 *  Description :   
 *------------------------------------------------------------------------*/
void 
appStillBlinkStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{

	switch ( msg) {
	case SP5K_MSG_AE_READY:	
		break;
	case SP5K_MSG_STILL_CONFIRM_STORE:
		/*s Mask by Aries 09/11/05*/
		/*appAePreviewRestore();*/
		/*e Mask by Aries 09/11/05*/
		break;
	case APP_STATE_MSG_ABORT:
		break;
	default:
		break;
	}
}



/*-------------------------------------------------------------------------
 *  Function Name : appStillBlinkStateOnKey
 *  Description :   
 *------------------------------------------------------------------------*/
void 
appStillBlinkStateOnKey(
	UINT32 msg,
	UINT32 param
)	
{
/*s modified by Leo @20091124*/
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);	
	switch (msg) {		
	case APP_KEY_PRESS_S2:
		//ppStillBlinkConfirmStoreSet(FALSE);  /*None save Image*/
		//BlinkSaveSelect =0;
		//break;
	case APP_KEY_PRESS_SET:	
		//ppStillBlinkConfirmStoreSet(TRUE);  /*Save Image*/
        
		if(SAVE_THIS_PHOTO_NO==appPlayOsd_DlgBoxSelectGet()){
		  // CLEAR_OSD_SRC;
		   //appStillBlinkConfirmStoreSet(FALSE); 
		   BlinkSaveSelect =0;
			
		}else{
		//	CLEAR_OSD_SRC;
		//appStillBlinkConfirmStoreSet(TRUE); 
		BlinkSaveSelect =1;
		}	
		appStateChange(APP_STATE_STILL_CAPTURE,STATE_PARAM_NORMAL_INIT);	
		sp5kHostMsgSend(APP_UI_MSG_VIEW_BLINKDETECT_CONFIRM, 0);
		break;

	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (APP_KEY_PRESS_DOWN==msg)
			appPlayOsd_DlgBoxScrollSet(TRUE, APPVIEW_BLINK_DLG_START_X,APPVIEW_BLINK_DLG_START_Y,APPVIEW_BLINK_DLG_1STITEM_X, APPVIEW_BLINK_DLG_1STITEM_Y,APPVIEW_BLINK_DLG_ITEM_GAP);
		else
			appPlayOsd_DlgBoxScrollSet(FALSE, APPVIEW_BLINK_DLG_START_X,APPVIEW_BLINK_DLG_START_Y,APPVIEW_BLINK_DLG_1STITEM_X, APPVIEW_BLINK_DLG_1STITEM_Y,APPVIEW_BLINK_DLG_ITEM_GAP);
	
		break;

	default:		
		break;
	}

/*e modified by Leo @20091124*/
}

void 
appStillBlinkState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appStillBlinkState");
		appStillBlinkStateInit(appPreviousStateGet());
		break;		
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;			
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appStillBlinkStateOnKey(msg,param);
		}
		else{
			appStillBlinkStateOnMsg(msg,param);
		}
		break;
	}

}

 /*e Add by Aries 09/10/30*/


