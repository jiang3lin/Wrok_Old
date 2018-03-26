/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h" /*for clean warning*/
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
#include "app_ui_para.h"
#if SPCA6330
#include "app_ptp.h"
#endif
#include "app_init.h"

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
#if HDMI_MENU_BY_JPG 
UINT32 pipHDMIMenuFlag = 0;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 menuPosBackup;
extern UINT8 fileTypeSelect;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT32 appMenu_MenuExit(UINT32 param)
 * @brief     appMenu_MenuExit
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-24
 * @todo      N/A
 * @bug       N/A
*/
void
appMenu_MenuExit(
	UINT32 param
)
{
	UINT32 nextSts;
	
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
			
	switch (STATE_GROUP(appPreviousStateGet()))
	{
		case APP_STATE_GROUP_VIEW:
			nextSts = APP_STATE_STILL_PREVIEW;
			break;
		case APP_STATE_GROUP_MENU:
		case APP_STATE_GROUP_VIDEO:		
			nextSts = appVideoRecAlwaysDo();
			break;	
		case APP_STATE_GROUP_AUDIO:
			nextSts = APP_STATE_AUDIO_REC;
			break;
		case APP_STATE_GROUP_PLAYBACK:
			nextSts = APP_STATE_PB_MAIN;
			break;
		case APP_STATE_GROUP_USB:
				nextSts = APP_STATE_MENU;
			break;
		default:
			nextSts = APP_STATE_STILL_PREVIEW;
			break;
	}
	appStateChange(nextSts,STATE_PARAM_NORMAL_INIT);
}

/**
 * @fn        UINT32 appMenu_SubMenuEnter(UINT32 msg, UINT32 param, UINT32 bBackParent)
 * @brief     appMenu_SubMenuEnter
 * @param     [in] msg
 * @param     [in] param
 * @param     [in] bBackParent
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-24
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenu_SubMenuEnter(
	UINT32 msg,
	UINT32 param,
	UINT32 bBackParent
)
{
	menuCtrl_t *pMenu = NULL;

	if (menuHdlGet(&pMenu) != SUCCESS) return FAIL;

	/*if (menuProcExecute(pMenu->layer, pMenu->item, msg, param) == MENU_IGNORE_DEFAULT) {
		return FAIL;
	}*/
	
	if (pMenu->subItem > 0) {
		menuProcChild (pMenu->sel);
		menuProcDisp();
	} else if (bBackParent && pMenu->layer >= MENU_LAYER_2) {
	
		menuProcParaSet(pMenu->layer, pMenu->item);
		#if ICAT_WIFI
		appStill_PIMA_DPC_Update(pMenu->item);
		#endif

		menuProcParent ();
		menuProcDisp();
	}
	
	return SUCCESS;
}

/**
 * @fn        UINT32 appMenu_ParentMenuEnter(UINT32 msg, UINT32 param)
 * @brief     appMenu_ParentMenuEnter
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-24
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenu_ParentMenuEnter(
	UINT32 msg,
	UINT32 param
)
{
	menuCtrl_t *pMenu;

	if (menuHdlGet(&pMenu) != SUCCESS) return FAIL;

	pMenu->layer = menuProcLayerGet();
	pMenu->item = menuProcCurrItemGet(pMenu->layer);
	/*if (menuProcExecute(pMenu->layer, pMenu->item, msg, param) == MENU_IGNORE_DEFAULT) {
		return FAIL;
	}*/

	if (pMenu->layer >= MENU_LAYER_2) {
		//menuProcParaSet(pMenu->layer, pMenu->item);
		menuProcParent();
		menuProcDisp();
	}

	return SUCCESS;
}
/**
 * @fn        void appMenu_EvtOnMsg(UINT32 msg, UINT32 param)
 * @brief     appMenu_EvtOnMsg
 * @param     [in] msg
 * @param     [in] param
 * @retval    NONE
 * @see       NULL
 * @author    juanjuan.yuan
 * @since     2014-1-16
 * @todo      N/A
 * @bug       N/A
*/
void
appMenu_EvtOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	/*when video record in background,some sp5k msg should be process in these function,avoid video record file error*/
	appVideoRecAlwaysMenuMsgProc(msg,param);
}

/**
 * @fn        void appMenu_EvtOnKey(UINT32 msg, UINT32 param)
 * @brief     appMenu_EvtOnKey
 * @param     [in] msg
 * @param     [in] param
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-20
 * @todo      N/A
 * @bug       N/A
*/
void
appMenu_EvtOnKey(
	UINT32 msg,
	UINT32 param
)
{
	menuCtrl_t *pMenu;
	UINT8 layer;
	/*appBeep(TRUE, FALSE);*/

	if (menuHdlGet(&pMenu) != SUCCESS) {
		/*printf("menu hdl get fail\n");*/
	};

	layer = menuProcLayerGet();

	if (menuProcExecute(pMenu->layer, pMenu->item, msg, param) != MENU_IGNORE_DEFAULT) {
	
		switch (msg) {
		case APP_KEY_PRESS_S1:
			break;
		case APP_KEY_PRESS_TELE:
			break;
		case APP_KEY_PRESS_WIDE:
			break;
		case APP_KEY_RELEASE_WIDE:
		case APP_KEY_RELEASE_TELE:
			break;
		case APP_KEY_PRESS_MENU:
			if(appPreviousStateGet() == APP_STATE_USB_PRNT || appPreviousStateGet() == APP_STATE_USB_PRINTING)
			{
				if(menuProcCurrRootGet() != MENU_MARK_SCENEMODE)
				{
					appMenu_ParentMenuEnter(msg, param);
				}
				break;
			}
			else
			{
				if(layer == MENU_LAYER_0 || layer == MENU_LAYER_1)
				{
					appMenu_MenuExit(param);
				}
				else if(layer == MENU_LAYER_2)
				{
					appMenu_ParentMenuEnter(msg, param);
				}
			}
			break;
		case APP_KEY_PRESS_MODE:
			#if CAM_TYPE_CVR
			if(fileTypeSelect)
			{
				fileTypeSelect=0;
				AppPbDestroyshowJPGInPIP();
				appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			}
			#endif
			break;
		case APP_KEY_PRESS_SET:
			appMenu_SubMenuEnter(msg, param, 1);
	 		break;
		case APP_KEY_PRESS_RIGHT:
         #if !CAM_TYPE_CVR
			if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
			{
				menuProcMoveDown();
				menuProcDisp();
			}
			else
			{
				appMenu_SubMenuEnter(msg, param, 0);
			}			
        #endif
			break;
		case APP_KEY_PRESS_LEFT:
         #ifdef HW_PRJ_6330
            #if CAM_TYPE_CVR
			fileTypeSelect=0;
			AppPbDestroyshowJPGInPIP();
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
            #endif
         #endif
         #if !CAM_TYPE_CVR
			if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
			{
				menuProcMoveUp();
				menuProcDisp();
			}
			else
			{
				appMenu_ParentMenuEnter(msg, param);
			}
         #endif
			break;
		case APP_KEY_REPEAT_UP:	
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			break;
		case APP_KEY_PRESS_DOWN:
		case APP_KEY_REPEAT_DOWN:		 
			menuProcMoveDown();
			menuProcDisp();
			break;
		case APP_KEY_PRESS_DEL:
			break;
		default:
			break;
		}
		
	}
	
}

/**
 * @fn        void appMenu_PosPreset(UINT32 pos)
 * @brief     appMenu_PosPreset
 * @param     [in] pos
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
void
appMenu_PosPreset(
	UINT32 pos
)
{
	menuProcCurrPosSet(pos);
}

/**
 * @fn        void appMenu_PosInit(void)
 * @brief     appMenu_PosInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
void
appMenu_PosInit(
	void
)
{
	UINT32 menuPos;
__FUNC_TRACK__;
	if((menuProcCurrPosGet() != 0) && (menuProcCurrRootGet() != MENU_MARK_SETUP))
	{
		return;
	}
	
	switch (STATE_GROUP(appPreviousStateGet()))
	{
		case APP_STATE_GROUP_VIEW:
			menuPos = MENU_MARK_STILL;
			break;
		case APP_STATE_GROUP_VIDEO:
			menuPos = MENU_MARK_VIDEO;
			break;	
		case APP_STATE_GROUP_PLAYBACK:
			menuPos = MENU_MARK_PLAYBACK;
			break;
		#if !CAM_TYPE_CVR	
		case APP_STATE_GROUP_USB:
			if((appPreviousStateGet()==APP_STATE_USB_PRINT2NDMENU)||\
				(appPreviousStateGet()==APP_STATE_USB_PRINTIMAGESEL))
				{
					menuPos = MENU_MARK_DPSImage;
					break;
				}
			menuPos = MENU_MARK_DPS;
			break;
		#endif	
		default:
			menuPos = MENU_MARK_STILL;
			break;
	}
__FUNC_TRACK__;

	if(fileTypeSelect)
	{
	__FUNC_TRACK__;

		menuPos = MENU_MARK_FILETYPE_EVENT;
	}
	menuPosBackup = menuPos;
	printf("menupos=0x%x\n",menuPos);
	menuProcCurrPosSet (menuPos);
}

/**
 * @fn        void appMenuStateInit(void)
 * @brief     appMenuStateInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-21
 * @todo      N/A
 * @bug       N/A
*/
void 
appMenuStateInit(
	void
)
{
	appOsd_FontIconFontSet(ID_FONT_ICON);
#if 0	
	appOsd_FontTextFontSet(ID_FONT_ENG);
	appOsd_FontTextStringSet(ID_STR_TBL_ENG);
#endif
	/*Open PIP/PF/VCT layer for some times these may be close*/
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_VCT_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0,1, 0, 0);

	appMenu_PosInit();
	menuDispParaInit();
#if HDMI_MENU_BY_JPG
#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		menuHDMIRsvPreload();
	}
#endif
#endif
	menuProcDisp();
}

/**
 * @fn        void appMenuCalibrationStateInit(void)
 * @brief     appMenuCalibrationStateInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-21
 * @todo      N/A
 * @bug       N/A
*/
void 
appMenuCalibrationStateInit(
	void
)
{
	appOsd_FontIconFontSet(ID_FONT_ICON);
#if 0	
	appOsd_FontTextFontSet(ID_FONT_ENG);
	appOsd_FontTextStringSet(ID_STR_TBL_ENG);
#endif

	menuPosBackup = MENU_MARK_CALIBRATION | 0x00010000;
	menuProcCurrPosSet (menuPosBackup);
	menuDispParaInit();
	menuProcDisp();
}


/**
 * @fn        void appMenuState(UINT32 msg, UINT32 param)
 * @brief     appMenuState
 * @param     [in] msg
 * @param     [in] param
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-20
 * @todo      N/A
 * @bug       N/A
*/
void
appMenuState(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 procTime;
	
	//printf("msg:0x%x, para:0x%x\n", msg, param);

	procTime = appMsTimeGet();	
	switch (msg) 
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appMenuState");
		appMenuStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		menuProcCurrPosSet(0);
		appOsd_FontIconFontSet(ID_FONT_ICON);
#if 0		
		appOsd_FontTextFontSet(ID_FONT_ENG22X28);
		appOsd_FontTextStringSet(ID_STRTK_TBL_ENG);
#endif		
		CLEAR_OSD_SRC;
		CLEAR_PIP_SRC;
		
		appOsd_ColorBlendingSet(PAL_GREEN_2,PAL_BLEND_100);
		appOsd_ColorBlendingSet(PAL_GREEN_3,PAL_BLEND_100);
		appOsd_ColorBlendingSet(PAL_GREEN_4,PAL_BLEND_100);
		
#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			#if HDMI_MENU_BY_JPG
			sp5kRsvPreloadRelease();
			if(STATE_GROUP(appPreviousStateGet()) == APP_STATE_GROUP_PLAYBACK){
				sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
				#if 0
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
				
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+0, 0, 0,1920,1080);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+1, 0, 0,1920,1080);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+2, 0, 0,1920,1080);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+3, 0, 0,1920,1080);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+4, 0, 0,1920,1080);
				#endif

				sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_BASE+0, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_BASE+1, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_BASE+2, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_BASE+3, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_BASE+4, 0, 0, 0);
				
				sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
			}else if(pipHDMIMenuFlag){
				pipHDMIMenuFlag = 0;
				sp5kPipTerm();
			}
			#else
			appOsd_ColorBlendingSet(PAL_GREEN_2,PAL_BLEND_100);
			appOsd_ColorBlendingSet(PAL_GREEN_3,PAL_BLEND_100);
			appOsd_ColorBlendingSet(PAL_GREEN_4,PAL_BLEND_100);
			#endif
		}
#endif 
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			if(IS_APP_KEY_PRESS_MSG(msg)) /*junqiong.hu@20121218 add for mantis bug 0046196*/
			{
				appBeep(TRUE,FALSE);
			}
			/*key msg*/
			appMenu_EvtOnKey(msg, param);
		}
		else
		{
			/*other msg*/
			appMenu_EvtOnMsg(msg, param);
		}
		break;
	}

	//printf("procTime:%d\n", appMsTimeGet() - procTime);
}

