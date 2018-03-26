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

/**
 * @file		app_ui_para.c
 * @brief		App for UI setting parameter
 * @author		Phil Lin
 * @since		2012-09-20
 * @date		2012-09-20
 */
 
#include "app_com_def.h"
#include "app_ui_para.h"
#include "app_still.h"
#include "app_dcf.h"
#include "app_fast_boot.h"
#include "app_battery.h"
#include "app_view_param_def.h"
#include "app_user_setting.h"

#include "app_gSensor.h"
#include "app_rf_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/*Don't modify this value!*/
#define UI_PARAM_SIZE		1024

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define ADF_FILENAME  "B:\\USER_RES\\ADF.DAT"
#define UDF_FILENAME  "B:\\USER_RES\\UDF.DAT"
#define MODE_SAVE_WHEN_POWER_OFF 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
static UINT8 uiPara[UI_PARAM_SIZE];

#if 0
UINT8  itemModify;
static UINT8 itemSts[] = {0};
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/**
 * @fn        UINT32 appUDFCheckSum(void)
 * @brief     appUDFCheckSum
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    yongsun
 * @since     2012-11-23
 * @todo      N/A
 * @bug       N/A
*/ 
UINT32 appUDFCheckSum(void)
{
	uiPara_t* ptestUiPara;
	UINT32 checksum;
	UINT16 i;

	ptestUiPara = appUiParaGet();

	checksum = 0;
	for( i=0; i<(sizeof(uiPara_t) - 4); i++)
	{
		checksum += *(((UINT8 *)ptestUiPara + i));
	}

	if(ptestUiPara->iCatchFlag[0]=='I'\
		&&ptestUiPara->iCatchFlag[1]=='C'\
		&&ptestUiPara->iCatchFlag[2]=='A'\
		&&ptestUiPara->iCatchFlag[3]=='T'\
		&&ptestUiPara->iCatchFlag[4]=='C'\
		&&ptestUiPara->iCatchFlag[5]=='H'
		)
	{
		if(checksum != ptestUiPara->Checksum)//checksum err
		{
			return FAIL;
		}
		else//check sum ok
		{
			return SUCCESS;
		}
	}
	else{
		return FAIL;
	}
}

/**
 * @fn        UINT32 appUDFLoad(void)
 * @brief     appUDFLoad
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-20
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appUDFLoad(
	void
)
{
	if(ReadFile(UDF_FILENAME, &uiPara[0], sizeof(uiPara_t))!= SUCCESS) 
	{
		DBG_PRINT("[ERR] read file[%s] fail\n", UDF_FILENAME);
		return FAIL;
	}

	return appUDFCheckSum();
}

/**
 * @fn        UINT32 appADFLoad(void)
 * @brief     appADFLoad
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-20
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appADFLoad(
	void
)
{
	if(ReadFile(ADF_FILENAME, &uiPara[0], sizeof(uiPara_t))!= SUCCESS) 
	{
		DBG_PRINT("[ERR] read file[%s] fail\n", ADF_FILENAME);
		return FAIL;
	}

	return appUDFCheckSum();
}


//-----------------------------------------------------------------------------
//appUiParaInit
//-----------------------------------------------------------------------------
/**
 * @brief	UI parameter initialization
 * @param	mode:init mode
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appUiParaInit(uiParaInitMode_t mode)
{
	uiPara_t *psPara = (uiPara_t*)&uiPara[0];

	
	if (mode == PARA_INIT_FROM_UDF) {
		//Load UDF from nand reserve block
		psPara->EmptyPara = 0;
		if(appUDFLoad()==SUCCESS)
		{
			appUiParaPowerOnSet();
			/*appUiParaInit_Print("UDF load!\n");*/
			/*psPara->USBMode = USB_DEFAULT;*/ /*mark for mantis bug 0048653*/
			gStillCB.cap.CurScene = psPara->CurScene;
			gStillCB.lastcap = gStillCB.cap;
			return;
		}
	} 

	if(appADFLoad()==SUCCESS)//UI config tool flow
	{
		appUiParaPowerOnSet();
		/*appUiParaInit_Print("adf load success\n");*/
	}
	else// FW flow
	{
		appFactorySettingApply();
		/*
		psPara->AEMode = UI_EXP_BIAS_ZERO;
		psPara->CurViewMode = PASM_DEFAULT;
		psPara->CurScene = SCENE_DEFAULT;	
		psPara->Volume= SOUND_VOLUME_DEFAULT;
		psPara->BeepSound= TRUE;
		psPara->StartupSound= POWERON_SOUND_DEFAULT;
		psPara->ShutterSound= SHUTTER_SOUND_DEFAULT;
		psPara->AutoReviewTime = QV_TIME_DEFAULT;
		psPara->SleepTime= POWER_SAVE_DEFAULT;	
		psPara->Language= LANG_DEFAULT;
		psPara->fileNumber = FILENUM_DEFAULT;
		psPara->DateStyle= DATE_TIME_DEFAULT;
		psPara->TVMode= TV_DEFAULT;
		psPara->USBMode= USB_DEFAULT;
		psPara->OpenPic= POWERON_LOGO_DEFAULT;
		psPara->LightFreq= LIGHT_FREQ_DEFAULT;	 
		psPara->PeriodicalCap= PERIODICAL_CAPTURE_DEFAULT;
		psPara->dcfDirKey = 0;
		psPara->dcfFileKey = 0;
		psPara->flash = UI_FLASH_OFF;
		pbuf->qvTime = QV_TIME_DEFAULT;
		pbuf->videoFormat = VIDEO_FORMAT_TYPE_DEFAULT;
		pbuf->mediaFormat = MEDIA_FORMAT_TYPE_DEFAULT;		
		pbuf->batteryType = BATTERY_TYPE_DEFAULT;
		*/
		/*appUiParaInit_Print("adf load fail\n");*/
		memcpy(&psPara->iCatchFlag[0], "ICATCH", sizeof(psPara->iCatchFlag));
	}
}

//-----------------------------------------------------------------------------
//appUIParaSave
//-----------------------------------------------------------------------------
/**
 * @brief	UI parameter save
 * @param	mode:power off mode
 * @retval	NULL
 * @see
 * @author	yongsun
 * @since	2012-11-23
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT32
appUIParaSave(
	void
)
{
	uiPara_t *psPara = appUiParaGet();
	UINT16 i;
	
	psPara->Checksum = 0;

	#if MODE_SAVE_WHEN_POWER_OFF
	psPara->CurViewMode =gStillCB.cap.CurViewMode;
	psPara->CurScene = gStillCB.cap.CurScene;
	#else
	psPara->CurViewMode = PASM_DEFAULT;
	psPara->CurScene = SCENE_DEFAULT;
	#endif
	
	appDcfKeySettingUpdate(&psPara->dcfDirKey, &psPara->dcfFileKey);
	//simply add every ui para to checksum
	for( i=0; i<(sizeof(uiPara_t) - 4); i++)
	{
		(psPara->Checksum) += *(((UINT8 *)psPara + i));
	}
	
	if(WriteFile(UDF_FILENAME, &uiPara[0], sizeof(uiPara_t))!=SUCCESS) 
	{		
		DBG_PRINT("[ERR] write file[%s] fail\n", UDF_FILENAME);
		return FAIL;
	}
	
	appFastBootLensModeSet(psPara->CurViewMode);
	
	appFastBootLensBattSet(psPara->batteryType,appLensBootBatteryLevelGet());
	
	return SUCCESS;

}

//-----------------------------------------------------------------------------
//appUiParaGet
//-----------------------------------------------------------------------------
/**
 * @brief	Get ui parameter pointer
 * @param	NULL
 * @retval	ui parameter pointer
 * @see
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
uiPara_t* appUiParaGet(void)
{
	return (uiPara_t*)&uiPara[0];
}

void appUiParaPowerOnSet(void)
{
	//recover some uiPara Setting


	//set some uiPara
	appSensorFlipSet();
	appOsdFlipSet();
	appOsdBrightnessSet();
	appLedIndicatorSet();
	appBuzzerEnableSet();
	//appGsensorSensortivitySet();	see appGsensorInit
}

#if 0
void appUiItemOrderCfg(void)
{
	UINT8 idx;
	idx=	uiPara[0x100];
	//printf("@@idx--->0x%bx\n",idx);
	if(idx >= ITEM_FIRST && idx != ITEM_NULL)
	{
		itemModify=1;
	}
	else
	{
		itemModify=0;
	}
}
/**
 * @fn        UINT8 appUiItemStsSet(UINT8 idx, idx,UINT8 val)
 * @brief     appUiItemStsSet
 * @param     [in] idx
 * @param     [in] val
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    ZhuXiaolin
 * @since     2012-7-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appUiItemStsSet(UINT8 idx,UINT8 val)
{
	if(!itemModify) val = 0;
	if(idx < ITEM_EMPTY || idx > ITEM_MAX)
	{
		return 0;
	}
	itemSts[idx] = val;
}
/**
 * @fn        UINT8 appUiItemStsGet(UINT8 idx)
 * @brief     appUiItemStsGet
 * @param     [in] idx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    ZhuXiaolin
 * @since     2012-7-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8  appUiItemStsGet(UINT8 idx)
{
	if(!itemModify) return 0;
	if(idx < ITEM_EMPTY || idx > ITEM_MAX)
	{
		return 0;
	}
	return itemSts[idx];
}
UINT8 appUiItemOrderGet(UINT8 idx,UINT8 *pTable,UINT8 *length)
{
	UINT8 *pDate=&uiPara[0x100];
	UINT8 num=0;
	
	if(itemModify)
	{
	if(idx < ITEM_EMPTY || idx > ITEM_MAX)
	{
		return FALSE;
	}
	if(*pDate ==ITEM_NULL) {
		return FALSE;
	}
	while(*pDate != idx)//fine index
	{
		pDate++;
		if(*pDate ==ITEM_NULL) {
			return FALSE;
		}
	}

	pDate++;
	while(*pDate<ITEM_EMPTY)
	{
		*pTable = *pDate;
		//printf("pdate[%bu]=%bu  ",num,*pDate);

		pTable++;
		pDate++;
		num++;
	}
	//printf("\nnum=%bu\n",num);
	*length=num;
	return TRUE;
	}
	return FALSE;
}
#endif
