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
#define HOST_DBG 0

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_pip_api.h"

#include "app_still.h"
#include "app_view_param.h"
#include "app_pal.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"
#include "app_menu_disp.h"
#include "app_menu_tab.h"
#include "app_menu_core.h"
#include "app_menu.h"
#include "gpio_custom.h"
#include "app_video.h"
#include "app_playback_osd.h"
#include "app_ptp.h"
#include "app_init.h"

#include "app_scdv_menu_disp.h"
#include "app_ui_para.h"
#include "app_scdv_ui_para_def.h"
#include "app_rf_def.h"

extern UINT8 fileTypeSelect;

extern UINT32 appMenu_SubMenuEnter(UINT32 msg, UINT32 param, UINT32 bBackParent);


void appScdvMenuStateMenuExit(void)
{
	uiPara_t* puiPara = appUiParaGet();
	
	__FUNC_TRACK__;
	switch (puiPara->ScdvMainMode)
	{
		case UI_SCDV_MAIN_MODE_SPORT:
			appStateChange(APP_STATE_SPORTDV_SYSTEM,STATE_PARAM_NORMAL_INIT);
			break;
		case UI_SCDV_MAIN_MODE_CAR:
			appStateChange(APP_STATE_CARDV_SYSTEM,STATE_PARAM_NORMAL_INIT);
			break;	
		default:
			appStateChange(APP_STATE_SPORTDV_SYSTEM,STATE_PARAM_NORMAL_INIT);
			break;
	}
}


void appScdvMenuPosInit(void)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT32 menuPos;
	
	__FUNC_TRACK__;

	switch(appPreviousStateGet())
	{
		case APP_STATE_SPORTDV_SYSTEM:
		case APP_STATE_CARDV_SYSTEM:
			switch (puiPara->ScdvMainMode)
			{
				case UI_SCDV_MAIN_MODE_SPORT:
					menuPos = MENU_MARK_SPORT_DV;
					break;
				case UI_SCDV_MAIN_MODE_CAR:
					menuPos = MENU_MARK_CAR_DV;
					break;	
				default:
					menuPos = MENU_MARK_SPORT_DV;
					break;
			}
			break;
		case APP_STATE_SPORTDV_PB_DELETE:
			menuPos = MENU_MARK_DELETE_EXIT;
			break;
		case APP_STATE_CARDV_PB_DELETE_LOOPING:
			menuPos = MENU_MARK_DELETE_LOOP_EXIT;
			break;
		case APP_STATE_CARDV_PB_DELETE_EVENT:
			menuPos = MENU_MARK_DELETE_EVENT_EXIT;
			break;
		#ifdef USE_DVS5M9_HW
		case APP_STATE_SCDV_RF_PAIRING:
			menuPos = MENU_MARK_RF_REMOTER;
			break;
		#endif
	}

	if(IS_HDMI_IN)
	{
		menuPos = MENU_MARK_SCDV_PB_VIDEO;
	}

	printf("menupos=0x%x\n",menuPos);	
	menuProcCurrPosSet (menuPos);
}

UINT32 appScdvMenuSubMenuEnter(UINT32 msg, UINT32 param, UINT32 bBackParent)
{
	menuCtrl_t *pMenu = NULL;

	if(IS_HDMI_IN)
	{
		return appMenu_SubMenuEnter(msg, param, bBackParent);
	}

	if (menuHdlGet(&pMenu) != SUCCESS)
	{
		return FAIL;
	}
	
	if (pMenu->subItem > 0) 
	{
		//if no param, will go last item
		if(pMenu->sel == MENU_PARA_NULL)
		{
			if(pMenu->layer == MENU_LAYER_0)
			{
				pMenu->sel = 0;
			}
			else
			{
				pMenu->sel = pMenu->subItem - 1;
			}
		}
		
		menuProcChild (pMenu->sel);
		appScdvMenuDisplay();
	} 
	else if (bBackParent && pMenu->layer >= MENU_LAYER_1) 
	{
		menuProcParaSet(pMenu->layer, pMenu->item);
		menuProcParent ();
		appScdvMenuDisplay();
	}
	
	return SUCCESS;
}

UINT32 appScdvMenuParentMenuEnter(UINT32 msg, UINT32 param)
{
	menuCtrl_t *pMenu;

	if (menuHdlGet(&pMenu) != SUCCESS)
	{
		return FAIL;
	}

	pMenu->layer = menuProcLayerGet();
	pMenu->item = menuProcCurrItemGet(pMenu->layer);

	if (pMenu->layer >= MENU_LAYER_1) 
	{
		menuProcParent();
		appScdvMenuDisplay();
	}

	return SUCCESS;
}

void appScdvMenuStateClose(void)
{
	if(menuProcCurrRootGet() == MENU_MARK_SCDV_PB_SELECT)
	{
		menuProcCurrPosSet(0);
		appOsd_FontIconFontSet(ID_FONT_ICON);
		CLEAR_OSD_SRC;
		CLEAR_PIP_SRC;
		
		appOsd_ColorBlendingSet(PAL_GREEN_2,PAL_BLEND_100);
		appOsd_ColorBlendingSet(PAL_GREEN_3,PAL_BLEND_100);
		appOsd_ColorBlendingSet(PAL_GREEN_4,PAL_BLEND_100);
	}
}

void appScdvMenuStateInit(void)
{
	LED_RULES_NORMAL;

	#ifndef RF_PREVIEW_KEEP
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	#endif

	if(appScdvWaitForDiskReady() == TRUE)
	{

	}

	appScdvMenuPosInit();	
	if(menuProcCurrRootGet() == MENU_MARK_SCDV_PB_SELECT)
	{
		fileTypeSelect = TRUE;
	
		appOsd_FontIconFontSet(ID_FONT_ICON);

		/*Open PIP/PF/VCT layer for some times these may be close*/
		sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_0, 1, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_VCT_0, 1, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0,1, 0, 0);

		menuDispParaInit();
	}	
	appScdvMenuDisplay();
}

void appScdvMenuStateOnMsg(UINT32 msg, UINT32 param)
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

void appScdvMenuStateOnKey(UINT32 msg, UINT32 param)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	menuCtrl_t *pMenu;
	UINT8 layer;

	if (menuHdlGet(&pMenu) != SUCCESS) 
	{
		return;
	}

	if (menuProcExecute(pMenu->layer, pMenu->item, msg, param) != MENU_IGNORE_DEFAULT) 
	{
		switch (msg) 
		{
			case APP_KEY_PRESS_POWER: 
				menuProcMoveDown();
				appScdvMenuDisplay();
				break;
			case APP_KEY_PRESS_OK:								
				appScdvMenuSubMenuEnter(msg, param, TRUE);
				break;
		}
	}
}

void appScdvMenuState(UINT32 msg,UINT32 param)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	//UINT32 procTime = appMsTimeGet();		
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:		
			RPrintf("appScdvMenuState");
			appScdvMenuStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appScdvMenuStateClose();
			appStateCloseDone();
			break;	
		default:
			
			if (IS_APP_KEY_MSG(msg)) 
			{
				appScdvMenuStateOnKey(msg,param);
			}
			else
			{
				appScdvMenuStateOnMsg(msg,param);
			}
			break;
	}

	//printf("procTime:%d\n", appMsTimeGet() - procTime);
} 

