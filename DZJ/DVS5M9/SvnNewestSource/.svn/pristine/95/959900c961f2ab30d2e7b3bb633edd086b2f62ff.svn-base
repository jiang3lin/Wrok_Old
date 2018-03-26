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
#include "app_menu_core.h"
#include "App_menu_tab.h"
#include "App_menu.h"
#include "app_icon_def.h"
#include "app_ui_para.h"
#include "app_video.h"
#include "app_rtc.h"
#include "app_init.h"
#include "app_msg.h"
/**************************************************************************
 *                      G L O B A L    C O N S T A N T                    *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static menuCtrl_t menuCtrl;
 
static UINT32	menuIndex = 0;
static UINT8	layerIndex = 0;
static UINT8	menuCurrItem;// = 0;
static UINT8	layerScrollTab[4];
static UINT8	menuScrollMode = SCROLL_MODE_PAGE;

extern appRTC_t rtcTime;
extern void appVideoStop(void);
extern BOOL _setupMenuFunctionFormat(void);
extern void _setupMenuFunctionResetAll(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT16 menuProcStrIndexGet (UINT8 layer, UINT8 item, UINT8 sub);

/**
 * @fn        UINT8 menuProcMoveUp(void)
 * @brief     menuProcMoveUp
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcMoveUp(
	void
)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 i;
	UINT8 item;
  
	menuLayerSizeGet(layerIndex, &line, &col);
	nodeItem = menuProcNodeItemGet(layerIndex, menuCurrItem);

	for (i=1; i<nodeItem; i++)
	{
		if (menuCurrItem < i)
			item = nodeItem + menuCurrItem - i;
		else
			item = menuCurrItem - i;

		if (menuProcIsActive(layerIndex, item))
		{
			if (menuCurrItem > item)
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if (menuCurrItem < layerScrollTab[layerIndex])
				{
					layerScrollTab[layerIndex] = menuCurrItem;
				}
			}
			else
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if ((menuCurrItem+1) > (layerScrollTab[layerIndex] + line))
				{
					layerScrollTab[layerIndex] = menuCurrItem+1-line;
				}
			}
			return SUCCESS;
		}
	}
	if (menuProcIsActive(layerIndex, menuCurrItem))
		return SUCCESS;

	return FAIL;
}

/**
 * @fn        UINT8 menuProcMoveDown(void)
 * @brief     menuProcMoveDown
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcMoveDown(
	void
)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 i;
	UINT8 item;
  
	menuLayerSizeGet(layerIndex, &line, &col);
	nodeItem = menuProcNodeItemGet(layerIndex, menuCurrItem);

	for (i=1; i<nodeItem; i++)
	{
		if ((menuCurrItem + i) >= nodeItem)
			item = menuCurrItem + i - nodeItem;
		else
			item = menuCurrItem + i;

		if (menuProcIsActive(layerIndex, item))
		{
			if (menuCurrItem < item)
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if ((menuCurrItem+1) > (layerScrollTab[layerIndex] + line))
				{
					layerScrollTab[layerIndex] = menuCurrItem+1-line;
				}
			}
			else
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if (menuCurrItem < layerScrollTab[layerIndex])
				{
					layerScrollTab[layerIndex] = menuCurrItem;
				}
			}
			return SUCCESS;
		}
	}
	if (menuProcIsActive(layerIndex, menuCurrItem))
		return SUCCESS;

	return FAIL;
}

/**
 * @fn        UINT8 menuProcParent(void)
 * @brief     menuProcParent
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcParent(
	void
)
{
	layerScrollTab[layerIndex] = 0;
	menuIndex &= (~((UINT32)0xff << ((3-layerIndex)*8)));
	layerIndex--;
	menuCurrItem = ((menuIndex>>((3-layerIndex)*8))&0xff)-1;

	while (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		if (menuProcMoveDown() == FAIL)
		{
			menuProcParent();
		}
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 menuProcChild(UINT8 sel)
 * @brief     menuProcChild
 * @param     [in] sel
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcChild(
	UINT8 sel
)
{
	UINT8 line, col;

	if (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		return FAIL;
	}
	
	menuCurrItem = sel;
	layerIndex++;
	menuIndex += (((UINT32)menuCurrItem+1) << ((3-layerIndex)*8));

	menuLayerSizeGet(layerIndex, &line, &col);

	layerScrollTab[layerIndex] = 0;
	if (menuScrollMode == SCROLL_MODE_PAGE)
		layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
	else if ((menuCurrItem+1) > (layerScrollTab[layerIndex] + line))
	{
		layerScrollTab[layerIndex] = menuCurrItem + 1 - line;
	}

	if (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		if (menuProcMoveDown() == FAIL)
		{
			menuProcParent();
			return FAIL;
		}
	}
	return SUCCESS;
}

/**
 * @fn        UINT32 menuProcCurrPosGet(void)
 * @brief     menuProcCurrPosGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT32
menuProcCurrPosGet(
	void
)
{
	return menuIndex;
}

/**
 * @fn        UINT32 menuProcCurrRootGet(void)
 * @brief     menuProcCurrRootGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT32
menuProcCurrRootGet(
	void
)
{
	return (menuIndex & 0xff000000);
}

/**
 * @fn        UINT8 menuProcCurrPosSet(UINT32 pos)
 * @brief     menuProcCurrPosSet
 * @param     [in] pos
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcCurrPosSet(
	UINT32 pos
)
{
	UINT8 i;
	UINT8 nodeItem;
	UINT8 line, col;

	for (i=0; i< 4; i++)
		layerScrollTab[i] = 0x00;

	layerIndex = 0;
	menuCurrItem = 0;
	menuIndex = pos;

	if ((pos & 0xff000000) == 0)
		return FAIL;

	for (i=0; i< 4; i++)
	{
		if (pos & 0xff000000)
		{
			layerIndex = i;
			menuCurrItem = ((pos >> 24) & 0x000000ff) - 1;
			nodeItem = menuProcNodeItemGet(layerIndex, menuCurrItem);

			if (nodeItem == 0)
				return FAIL;

			menuLayerSizeGet(layerIndex, &line, &col);

			if (menuScrollMode == SCROLL_MODE_PAGE)
				layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
			else if (menuCurrItem >= line)
				layerScrollTab[i] = menuCurrItem - line + 1;

			pos <<= 8;
		}
		else
		{
			break;
		}
	}

	if (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		if (menuProcMoveDown() == FAIL)
		{
			menuProcParent();
			return FAIL;
		}
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 menuProcCurrItemGet(UINT8 layer)
 * @brief     menuProcCurrItemGet
 * @param     [in] layer
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcCurrItemGet(
	UINT8 layer
)
{
	if (layer > 3)
		return 0;
	return (UINT8)(((menuIndex >> ((3-layer)*8)) & 0xff)-1);
}

/**
 * @fn        UINT8 menuProcScrollGet(UINT8 layer)
 * @brief     menuProcScrollGet
 * @param     [in] layer
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcScrollGet(
	UINT8 layer
)
{
	if (layer > 3)
		return 0;
	return layerScrollTab[layer];
}

/**
 * @fn        UINT8 menuProcLayerGet(void)
 * @brief     menuProcLayerGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcLayerGet(
	void
)
{
	return layerIndex;
}

/**
 * @fn        UINT16 menuProcStrIndexGet(UINT8 layer, UINT8 item, UINT8 sub)
 * @brief     menuProcStrIndexGet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] sub
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
static UINT16
menuProcStrIndexGet(
	UINT8 layer,
	UINT8 item,
	UINT8 sub
)
{
	static UINT8 lastlayer = 0;
	static UINT8 lastitem = 0;
	static UINT8 lastsub = 0;
	static UINT16 strIndex = 0;
	static UINT32 lastindex = 0;

	UINT32 tmpIndex=0;
	UINT8 root;

	if (!menuIndex) {
		return MENU_UNKNOWN;
	}

	if ((layer != lastlayer) || (item != lastitem) || (sub != lastsub))
	{
		lastlayer = layer;
		lastitem = item;
		lastsub = sub;

		item++;
		sub++;
		switch (layer)
		{
			case 0:
				tmpIndex = (menuIndex & 0x00000000) | ((UINT32)item << 24) | ((UINT32)sub << 16);
				break;
	
			case 1:
				tmpIndex = (menuIndex & 0xff000000) | ((UINT32)item << 16) | ((UINT32)sub << 8);
				break;
	
			case 2:
				tmpIndex = (menuIndex & 0xffff0000) | ((UINT32)item << 8) | ((UINT32)sub << 0);
				break;
	
			case 3:
				tmpIndex = (menuIndex & 0xffffff00) | ((UINT32)item << 0);
				break;
	
			default:
				tmpIndex = 0;
				//printf("layer error\n");
				break;
		}

	   	root = (UINT8)(tmpIndex >> 24)-1;
		if ((tmpIndex & 0xff000000) != (lastindex & 0xff000000))
		{
			lastindex = tmpIndex;
			for (strIndex = CustomMenuRootStart[root]; strIndex < CustomMenuRootStart[root+1]; strIndex++)
			{
				if(tmpIndex == CustomMenuIndex[strIndex])
				{
					return strIndex;
				}
			}
		}
		else if (tmpIndex > lastindex)
		{
			lastindex = tmpIndex;
			for (strIndex = strIndex+1; strIndex < CustomMenuRootStart[root+1]; strIndex++)
			{
				if(tmpIndex == CustomMenuIndex[strIndex])
				{
					return strIndex;
				}
			}
		}
		else
		{
			lastindex = tmpIndex;
			for (strIndex = strIndex-1; strIndex >= CustomMenuRootStart[root]; strIndex--)
			{
				if(tmpIndex == CustomMenuIndex[strIndex])
				{
					return strIndex;
				}
				if (strIndex == 0)
				{
					break;
				}
			}
		}
		lastlayer = 0;
		lastitem = 0;
		lastsub = 0;
		strIndex = 0;
		lastindex = 0;
		return MENU_UNKNOWN;
	}
	else
		return strIndex;
}

/**
 * @fn        UINT32 menuProcStrGet(UINT8 layer, UINT8 item, UINT32 *pStrIdx)
 * @brief     menuProcStrGet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] pStrIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-27
 * @todo      N/A
 * @bug       N/A
*/
UINT32
menuProcStrGet(
	UINT8 layer,
	UINT8 item,
	UINT32 *pStrIdx
)
{
	UINT16 strIdx;

	strIdx = menuProcStrIndexGet(layer, item, 0xff);
	if(strIdx == MENU_UNKNOWN) {
		return FAIL;
	}

	if (pStrIdx) *pStrIdx = (UINT32)strIdx;

	return SUCCESS;
}

/**
 * @fn        UINT32 menuProcChildStrGet(UINT8 layer, UINT8 item, UINT8 sel, UINT32 *pStrIdx)
 * @brief     menuProcChildStrGet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] sel
 * @param     [in] pStrIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-27
 * @todo      N/A
 * @bug       N/A
*/
UINT32
menuProcChildStrGet(
	UINT8 layer,
	UINT8 item,
	UINT8 sel,
	UINT32 *pStrIdx
)
{
	UINT32 strIdx;
	
	strIdx = menuProcStrIndexGet(layer, item, sel);
	if(strIdx == MENU_UNKNOWN) {
		return FAIL;
	}

	if (pStrIdx) *pStrIdx = strIdx;

	return SUCCESS;
}

/**
 * @fn        UINT16 menuProcIconIDGet(UINT8 layer, UINT8 item)
 * @brief     menuProcIconIDGet
 * @param     [in] layer
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT16
menuProcIconIDGet(
	UINT8 layer,
	UINT8 item
)
{
	UINT16  strIndex;
	
	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex == MENU_UNKNOWN)
		return 0;

	return CustomIconPtr[strIndex];
}

/**
 * @fn        UINT16 menuProcIconIDGetByPos(UINT32 pos)
 * @brief     menuProcIconIDGetByPos
 * @param     [in] pos
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT16 
menuProcIconIDGetByPos(
	UINT32 pos
)
{
	UINT16  strIndex;
	UINT8 	root = (UINT8)(pos >> 24)-1;

	for (strIndex = CustomMenuRootStart[root]; strIndex < CustomMenuRootStart[root+1]; strIndex++)
	{
		if(pos == CustomMenuIndex[strIndex])
		{
			return CustomIconPtr[strIndex];
		}
	}

	return 0;
}

/**
 * @fn        UINT8 menuProcChildIconIDGet(UINT8 layer, UINT8 item, UINT8 sel)
 * @brief     menuProcChildIconIDGet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] sel
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT16 
menuProcChildIconIDGet(
	UINT8 layer,
	UINT8 item,
	UINT8 sel
)
{
	UINT16  strIndex;

	strIndex = menuProcStrIndexGet(layer, item, sel);
	
	if(strIndex == MENU_UNKNOWN)
		return ID_ICON_NONE;
	return CustomIconPtr[strIndex];
}

/**
 * @fn        UINT8 menuProcNodeItemGet(UINT8 layer, UINT8 item)
 * @brief     menuProcNodeItemGet
 * @param     [in] layer
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcNodeItemGet(
	UINT8 layer,
	UINT8 item
)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex == MENU_UNKNOWN)
		return 0;
	return CustomMenuNodeItem[strIndex];
}

/**
 * @fn        UINT8 menuProcSubItemGet(UINT8 layer, UINT8 item)
 * @brief     menuProcSubItemGet
 * @param     [in] layer
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcSubItemGet(
	UINT8 layer,
	UINT8 item
)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff); 

	if(strIndex == MENU_UNKNOWN)
		return 0;
	return CustomMenuSubItem[strIndex];
}

/**
 * @fn        UINT8 menuProcParaGet(UINT8 layer, UINT8 item)
 * @brief     menuProcParaGet
 * @param     [in] layer
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcParaGet(
	UINT8 layer,
	UINT8 item
)
{
	UINT16 strIndex;
	UINT8 para;
	UINT8* pMPara = (UINT8*)appUiParaGet();

	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex != MENU_UNKNOWN)
	{
		para = CustomMenuParaTab[strIndex];
		if (para)
			return pMPara[para];
		else
			return MENU_PARA_NULL;
	}
	
	return 0;
}

/**
 * @fn        void menuProcParaSet(UINT8 layer, UINT8 item)
 * @brief     menuProcParaSet
 * @param     [in] layer
 * @param     [in] item
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
void
menuProcParaSet(
	UINT8 layer,
	UINT8 item
)
{
	UINT16 strIndex;
	UINT8 para;
	UINT8* pMPara = (UINT8*)appUiParaGet();

	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex != MENU_UNKNOWN)
	{
		para = CustomMenuParaTab[strIndex];
		if (para)
			pMPara[para] = item;
	}
}

//for parameter in parent item
void menuProcParaSetParent(	UINT8 layer, UINT8 item)
{
	UINT16 strIndex;
	UINT8 para;
	UINT8* pMPara = (UINT8*)appUiParaGet();

	strIndex = menuProcStrIndexGet(layer - 1, menuProcCurrItemGet(layer - 1), 0xff);

	if(strIndex != MENU_UNKNOWN)
	{
		para = CustomMenuParaTab[strIndex];
		if (para)
			pMPara[para] = item;
	}
}

/**
 * @fn        UINT8 menuProcIsActive(UINT8 layer, UINT8 item)
 * @brief     menuProcIsActive
 * @param     [in] layer
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcIsActive(
	UINT8 layer,
	UINT8 item
)
{
	UINT16 strIndex;


	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex != MENU_UNKNOWN)
	{
		if (CustomMenuFunc[strIndex] != NULL)
		{
			return CustomMenuFunc[strIndex](item);
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * @fn        UINT8 menuProcExecute(UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
 * @brief     menuProcExecute
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] para
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcExecute(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT16 strIndex;

	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex != MENU_UNKNOWN)
	{
		if (CustomMenuExe[strIndex] != NULL)
		{
			return CustomMenuExe[strIndex](layer, item, msg, para);
		}
		else
		{
			return MENU_CALL_DEFAULT;
		}
	}
	return MENU_CALL_DEFAULT;
}

/**
 * @fn        UINT8 menuScrollModeSet(UINT8 mode)
 * @brief     menuScrollModeSet
 * @param     [in] mode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuScrollModeSet(
	UINT8 mode
)
{
	if(mode < 2)
	{
		menuScrollMode = mode;
		return TRUE;
	}
	return FALSE;
}

/**
 * @fn        UINT8 menuProcDbgMsg(UINT8 mode)
 * @brief     menuProcDbgMsg
 * @param     [in] mode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcDbgMsg(
	UINT8 mode
)
{
	switch (mode) {
	case 0:
		break;
	default:
		#if 0
		printf("menuIndex = 0x%08x\n",menuIndex);
		#endif
		#if 0
		printf("layerIndex = %d\n",layerIndex);
		printf("menuCurrItem = %d\n",menuCurrItem);
		printf("layerScrollTab 0,1 = %d,%d\n",layerScrollTab[0],layerScrollTab[1]);
		printf("layerScrollTab 2,3 = %d,%d\n\n",layerScrollTab[2],layerScrollTab[3]);
		#endif
		break;
	}
	return 0;
}

/**
 * @fn        UINT32 menuHdlGet(uCtrl_t **ppMenu)
 * @brief     menuHdlGet
 * @param     [in] ppMenu
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-24
 * @todo      N/A
 * @bug       N/A
*/
UINT32
menuHdlGet(
	menuCtrl_t **ppMenu
)
{
	UINT8 layer, item, sel, subItem;
	menuCtrl_t *ptr = &menuCtrl;

	layer = menuProcLayerGet();
	item = menuProcCurrItemGet(layer);
	subItem = menuProcSubItemGet(layer, item);
	sel = menuProcParaGet(layer, item);

	ptr->layer = layer;
	ptr->item = item;
	ptr->subItem = subItem;
	ptr->sel = sel;

	if (ppMenu) *ppMenu = ptr;
	
	return SUCCESS;
}

#if FUNC_HOST_TOUCH_PANEL

/**
 * @fn        UINT8 menuProcTPSet(UINT8 tpLine, UINT8 isChild)
 * @brief     menuProcTPSet
 * @param     [in] tpLine
 * @param     [in] isChild
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
menuProcTPSet(
	UINT8 tpLine,
	UINT8 isChild
)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 item;

	if (isChild) isChild = 1;

	menuLayerSizeGet(layerIndex + isChild, &line, &col);
	nodeItem = menuProcNodeItemGet(layerIndex + isChild, 0);

	item = layerScrollTab[layerIndex + isChild] + tpLine;
	//if ((tpLine < line) && (item < nodeItem))
	{	
		if (menuProcIsActive(layerIndex + isChild, item))
		{
			if (isChild)
				layerIndex++;
			menuCurrItem = item;
			menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
			menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));
	
			return SUCCESS;
		}
	}

	return FAIL;
}

/**
 * @fn        UINT8 menuProcPageUp(void)
 * @brief     menuProcPageUp
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcPageUp(
	void
)
{
	UINT8 line, col;
	UINT8 item;
	UINT8 layer;

	layer = menuProcLayerGet();
	if(!layer)
	{
		layer = 1;
	}
	menuLayerSizeGet(layer, &line, &col);
	item = layerScrollTab[layer];
	if (item >= line)
	{	
		layerScrollTab[layer] -= line;
		return SUCCESS;
	}

	return FAIL;
}

/**
 * @fn        UINT8 menuProcPageDown(void)
 * @brief     menuProcPageDown
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcPageDown(
	void
)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 item;
	UINT8 layer;

	layer = menuProcLayerGet();

	if(!layer)
	{
		layer = 1;
	}

	menuLayerSizeGet(layer, &line, &col);
	nodeItem = menuProcNodeItemGet(layer, 0);
	item = layerScrollTab[layer] + line;
	if (item < nodeItem)
	{	
		layerScrollTab[layer] += line;
		return SUCCESS;
	}

	return FAIL;
}

#endif
/**
 * @fn        UINT8  menuProcVidRecAlwaysParaExcute(UINT8 layer,UINT8 item,UINT32 msg,UINT8 uiPara) 
 * @brief     menuProcVidRecAlwaysParaExcute
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    juanjuan.yuan
 * @since     2013-11-20
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcVidRecAlwaysParaExcute(UINT8 layer,UINT8 item,UINT32 msg,UINT8 uiPara)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	#if VIDEO_REC_ALWAYS
	UINT32 pParam;

	if(uiPara != item)
	{
		appVideoStop();		
		menuProcParaSet(layer, item);
		appVideoRecAlwaysRestart(1);
		appBtnDisable(BTN_ALL);  
		appMsgFlush(APP_MSG_KEY_QUE);
		appHostMsgWait(SP5K_MSG_MEDIA_REC_STOP, &pParam, 500); /*add for mantis bug 0048984*/
		appBtnEnable(BTN_ALL);
		appMenu_MenuExit(0);
		ret = MENU_IGNORE_DEFAULT;
	}
	#endif
	return ret;
}

/**
 * @fn        UINT8  menuProcVidRecAlwaysRtcExcute(void)
  * @brief     menuProcVidRecAlwaysRtcExcute
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author   juanjuan
 * @since     2013-11-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcVidRecAlwaysRtcExcute(void)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	#if VIDEO_REC_ALWAYS
	uiPara_t* puiPara = appUiParaGet();

	appVideoStop();		
	rtcTime.year = puiPara->rtcYear + 2000;
	rtcTime.mon = puiPara->rtcMonth + 1;
	rtcTime.day = puiPara->rtcDay + 1;
	rtcTime.hour = puiPara->rtcHour;
	rtcTime.min = puiPara->rtcMinute;
	rtcTime.sec = 0;
	appRtcSet(&rtcTime);
	appVideoRecAlwaysRestart(1);
	appMenu_MenuExit(0);/**/
	ret = MENU_IGNORE_DEFAULT;
	#endif
	return ret;
}
/**
 * @fn        UINT8  menuProcVidRecAlwaysTimeZonesExcute(void)
  * @brief     menuProcVidRecAlwaysTimeZonesExcute
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author   juanjuan
 * @since     2014-01-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcVidRecAlwaysTimeZonesExcute(UINT8 layer,UINT8 item)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	#if VIDEO_REC_ALWAYS
	uiPara_t* puiPara = appUiParaGet();

	appVideoStop();
	appTimezoneChange(puiPara->TimeZones,item);
	menuProcParaSet(layer, item);
	appVideoRecAlwaysRestart(1);
	appMenu_MenuExit(0);/**/
	ret = MENU_IGNORE_DEFAULT;
	#endif
	return ret;
}

/**
 * @fn        UINT8  menuProcVidRecAlwaysResetExcute(void)
  * @brief     menuProcVidRecAlwaysResetExcute
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author   juanjuan
 * @since     2013-11-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcVidRecAlwaysResetExcute(void)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	#if VIDEO_REC_ALWAYS
	appVideoStop();		
	_setupMenuFunctionResetAll();
	appVideoRecAlwaysRestart(1);
	appMenu_MenuExit(0);/**/
	ret = MENU_IGNORE_DEFAULT;
	#endif
	return ret;
}
/**
 * @fn        UINT8  menuProcVidRecAlwaysFormatExcute(void)
  * @brief     menuProcVidRecAlwaysFormatExcute
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author   juanjuan
 * @since     2013-11-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcVidRecAlwaysFormatExcute(void)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	#if VIDEO_REC_ALWAYS
	appVideoStop();	
	_setupMenuFunctionFormat();
	appVideoRecAlwaysRestart(1);
	appMenu_MenuExit(0);/**/
	ret = MENU_IGNORE_DEFAULT;
	#endif
	return ret;
}
/**
 * @fn        UINT8  menuProcVidRecAlwaysVoiceRecExcute(void)
  * @brief     menuProcVidRecAlwaysVoiceRecExcute
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author   juanjuan
 * @since     2013-11-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
menuProcVidRecAlwaysVoiceRecExcute(void)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	#if VIDEO_REC_ALWAYS
	appVideoStop();	
	appVideoRecAlwaysRestart(1);
	appMenu_MenuExit(0);/**/
	ret = MENU_IGNORE_DEFAULT;
	#endif
	return ret;
}

