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
#include "gpio_custom.h"
#include "app_tvlcd.h"
#include "app_sound.h"
#include "sp5k_disp_api.h"
#include "app_ui_para.h"

void
appTvPlugStateInit(
	void
)

{
	uiPara_t* puiPara = appUiParaGet();
	
	if ((IS_TV_IN && (appCurrDispModeGet() == puiPara->TVMode)) 
		|| (!IS_TV_IN && (appCurrDispModeGet() == APP_DISP_LCD))) 
	{
	    	printf("\n[WARN] cancle TV plug action(%d,%d,%d) !!!\n",IS_TV_IN,puiPara->TVMode,appCurrDispModeGet());
		sp5kHostMsgSend(APP_UI_MSG_DUMMY, 0x00);	
		return;
	}
	DBG_PRINT("LCD <-> TV start\n");
	/*s Add by Aries 10/04/08*/
	#if SPCA5310 
	sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 0, 0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 0, 0, 0, 0);
	#endif
	/*s Add by Aries 10/04/08*/
	if (IS_TV_IN) 
	{
		LCD_BACKLIGHT_OFF; // BL OFF here???
		appLcdTvSwitch(puiPara->TVMode,0);
 	}
	else 
	{
		appLcdTvSwitch(APP_DISP_LCD,0);
 	}		
}


void 
appTvPlugState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appTvPlugState");
		appTvPlugStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;	
	case SP5K_MSG_DISP_SWITCH_READY:
		DBG_PRINT("$$ SP5K_MSG_DISP_SWITCH_READY : %d\n",IS_TV_IN);
		appDispGfxInit();		
		if ((!IS_TV_IN) && (appCurrDispModeGet() == APP_DISP_LCD))
		{
			LCD_BACKLIGHT_ON;
		}
		appAudioDevOn();
		if(appPreviousStateGet() == APP_STATE_VIDEO_REC){
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_TV_PLUG);
		}else{
			if(IS_PLAYBACK_GROUP(appPreviousStateGet())){ /*add by yn for 0046536 */
				appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_TV_PLUG);
			}else{
				appStateChange(appPreviousStateGet(),STATE_PARAM_TV_PLUG);
			}
		}
		/*s Add by Aries 10/04/08*/
		#if SPCA5310 
		sp5kDispColorKeySet(SP5K_DISP_COLOR_KEY_RGB565, 0x000000, 0);
		sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 1, 0, 0, 0);
		sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);
		#endif
		/*s Add by Aries 10/04/08*/
		break;
	case APP_UI_MSG_DUMMY:
		appStateChange(appPreviousStateGet(),STATE_PARAM_TV_PLUG);
		break;
	default:
		
		break;
	}

} 


