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

/**************************************************************************
 *                        H E A D E R   F I L E S                            *
 **************************************************************************/
#include "common.h"
#include <stdio.h>
	 
#include "app_com_def.h" 
#include "app_playback_def.h"
#include "app_icon_def.h"
#include "app_ui_para.h" 
#include "app_view_param_def.h"
#include "app_video.h"

#include "app_scdv_ui_para_def.h"


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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
UINT32 appMenuAct_SdvVideoMode (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SdvPhotoMode (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_ScdvSensorFlip (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SdvCarDv (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_CdvVideoMode (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_CdvLooping (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_CdvGsensorTrigger (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_CdvGsensorSensitivity (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_CdvMicPhone (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_CdvSdvMode (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemDis (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemWdr (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemLedIndicator (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemOsdFlip (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemOsdBrightness (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemBeepVolume (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemDateStamp (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemAutoPowerOff (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_System1Exit (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemDateTime (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemRemoter (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemFrequnecy (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemFormat (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemDefault (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemWifiPassword (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemVersion (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_SystemExit (UINT8 item)
{
	return TRUE;
}

UINT32 appMenuAct_ScdvPbFileType(UINT8 item)
{
	uiPara_t* puiPara = appUiParaGet();


	if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
	{
		return TRUE;
	}
	else if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
	{
		return FALSE;
	}

	return FALSE;
}

