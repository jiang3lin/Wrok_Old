/**************************************************************************
 *
 *       Copyright (c) 2006-2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Phil Lin
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "app_com_def.h"
#include "customization/app_init.h"
#include "app_ts_api.h"
#if FUNC_OPT_TP
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/*
	This definition could NOT more than GUI_OBJ_CTRL_MAX
*/
#define APP_GUI_OBJ_MAX    GUI_OBJ_CTRL_MAX  

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/

static guiObj_t *pGuiObj[APP_GUI_OBJ_MAX]; 
UINT16 guiObjIdx;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

#if 1
/**
 * @fn        UINT32 appGuiObjAdd(guiObjCusCtrl_t *pObj)
 * @brief     appGuiObjAdd
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-27
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appGuiObjAdd(
	guiObjCusCtrl_t *pObj
)
{
	UINT32 ret = SUCCESS;
	
	if (guiObjIdx >= APP_GUI_OBJ_MAX) 
	{
		ret |= FAIL;
		return ret;
	}
	
	ret |= guiObjCreate(pObj, &pGuiObj[guiObjIdx]);
	
	guiObjIdx++;
	return ret;
}

/**
 * @fn        UINT32 appGuiObjDraw(guiObjCusCtrl_t *pObj, UINT32 objIdx)
 * @brief     appGuiObjDraw
 * @param     [in] pObj
 * @param     [in] objIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-27
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appGuiObjDraw(
	guiObjCusCtrl_t *pObj,
	UINT32 objIdx
)
{
	UINT32 ret = SUCCESS;

	if (objIdx >= APP_GUI_OBJ_MAX || pGuiObj[objIdx] != NULL)
	{
		ret |= FAIL;
		return ret;
	}

	ret |= guiObjCreate(pObj, &pGuiObj[objIdx]);

	return ret;
}

/**
 * @fn        UINT32 appGuiObjClr(UINT32 idx)
 * @brief     appGuiObjClr
 * @param     [in] idx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appGuiObjClr(
	UINT32 idx
)
{
	UINT32 ret = SUCCESS;
	UINT32 i;

	if (idx != 0xFFFFFFFF) 
	{
		/* destory the sepcified GUI object */
		ret |= guiObjDestory(pGuiObj[idx]);
		pGuiObj[idx] = NULL;
	} 
	else 
	{
		/* destory all GUI object */
		for (i = 0; i < APP_GUI_OBJ_MAX; i++)
		{
			if (pGuiObj[i]) 
			{
				ret |= guiObjDestory(pGuiObj[i]);
				pGuiObj[i] = NULL;
			}
		}
		guiObjIdx = 0;
	}

	return ret;
}
#endif
#endif /* FUNC_OPT_TP */

