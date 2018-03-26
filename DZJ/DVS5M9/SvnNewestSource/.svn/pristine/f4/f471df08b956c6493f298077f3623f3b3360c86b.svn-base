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

#include "app_menu.h"
#include "app_menu_event.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_menu_disp.h"
#include "app_ui_para.h"
#include "app_osd_api.h"
#include "app_icon_def.h"
#include "app_pal.h"

#include "gpio_custom.h"

#include "sp5k_gfx_api.h"
#include "sp5k_pip_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_version.h"
#include "app_playback_osd.h"

#include "app_osd_api_lcm.h"
#include "app_osd_draw_lcm.h"
#include "app_scdv_ui_para_def.h"
#include "app_ui_para.h"
#include "app_rtc.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void menuProcDisp (void);


void appScdvMenuDisplayHdmi(void)
{
	menuProcDisp();
}

void appScdvMenuDisplay(void)
{
	UINT16 icon;
	UINT32 strIdx;
	
	UINT8 nodeItem = 0;
	UINT8 subItem=0;
	UINT8 selItem;
	UINT8 layer = 0;
	UINT8 item = 0;
	UINT8 line, col;
	
	UINT16 i;

	UINT32 menuRoot = menuProcCurrRootGet();
	uiPara_t* puiPara = appUiParaGet();

	if(menuProcCurrRootGet() == MENU_MARK_SCDV_PB_SELECT)
	{
		appScdvMenuDisplayHdmi();
		return;
	}

	appLcmOsdClean();

	layer = menuProcLayerGet();
	switch(layer)
	{
		case MENU_LAYER_0:
			item = menuProcCurrItemGet(layer);					
			menuProcStrGet(layer, item, &strIdx);			
			icon = menuProcIconIDGet(layer, item);		
			switch(menuProcCurrPosGet())
			{
				case MENU_MARK_SPORT_DV:
				case MENU_MARK_CAR_DV:
					appLcmOsdLargeIconShow(LcmOsdIconIndex0, icon, TRUE);

					icon = menuProcIconIDGetByPos(MENU_MARK_SYSTEM1);					
					appLcmOsdLargeIconShow(LcmOsdIconIndex1, icon, FALSE);

					icon = menuProcIconIDGetByPos(MENU_MARK_SYSTEM2);					
					appLcmOsdLargeIconShow(LcmOsdIconIndex2, icon, FALSE);
					
					appLcmOsdBottomStringShow(strIdx, FALSE);	
					break;
				case MENU_MARK_SYSTEM1:
				case MENU_MARK_SYSTEM2:
					if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
					{
						icon = menuProcIconIDGetByPos(MENU_MARK_SPORT_DV);					
					}
					else if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
					{
						icon = menuProcIconIDGetByPos(MENU_MARK_CAR_DV);					
					}
					appLcmOsdLargeIconShow(LcmOsdIconIndex0, icon, FALSE);

					icon = menuProcIconIDGetByPos(MENU_MARK_SYSTEM1);					
					appLcmOsdLargeIconShow(LcmOsdIconIndex1, icon, (menuProcCurrPosGet() == MENU_MARK_SYSTEM1) ? (TRUE) : (FALSE));

					icon = menuProcIconIDGetByPos(MENU_MARK_SYSTEM2);					
					appLcmOsdLargeIconShow(LcmOsdIconIndex2, icon, (menuProcCurrPosGet() == MENU_MARK_SYSTEM2) ? (TRUE) : (FALSE));
					
					appLcmOsdBottomStringShow(strIdx, FALSE);	
					break;
				case MENU_MARK_MENU_EXIT:
				default:
					appLcmOsdLargeIconShow(LcmOsdIconIndex0, icon, TRUE);
					appLcmOsdBottomStringShow(strIdx, FALSE);			
					break;
			}
			break;
		case MENU_LAYER_1:
		case MENU_LAYER_2:
		case MENU_LAYER_3:
			item = 0;

			nodeItem = menuProcNodeItemGet(layer, item);
			nodeItem -= menuProcScrollGet(layer);

			menuLayerSizeGet(layer, &line, &col);			
			if (nodeItem > line)
			{
				nodeItem = line;
			}
			item = menuProcScrollGet(layer);
			
			for(i=0; i<nodeItem; i++,item++)
			{
				menuProcStrGet(layer, item, &strIdx);
				icon = menuProcIconIDGet(layer, item);
				//printf("layer = %d, item = %d, nodeItem = %d, ", layer, item, nodeItem);
				//printf("icon = 0x%04x, strIdx = 0x%04x\n", icon, strIdx);
				
				/*subItem = menuProcSubItemGet(layer, item);				
				if (subItem > 0)
				{
					selItem = menuProcParaGet (layer, item);
					icon = menuProcChildIconIDGet(layer, item, selItem);
				}*/

				if(menuProcCurrItemGet(layer) == item)
				{
					appLcmOsdLargeIconShow(i, icon, TRUE);
					appLcmOsdBottomStringShow(strIdx, FALSE);
				}
				else
				{
					appLcmOsdLargeIconShow(i, icon, FALSE);
				}
			}
			break;
	}
	
	appLcmRefresh();
}

void appScdvMenuSystemDateTimeDisplay(void)
{
	UINT16 icon;
	UINT32 strIdx;
	
	UINT8 nodeItem = 0;
	UINT8 subItem=0;
	UINT8 selItem;
	UINT8 layer = 0;
	UINT8 item = 0;
	UINT8 line, col;
	
	UINT16 i;
	
	BOOL bSelect = FALSE;
	UINT8 osdString[OSD_STRING_SIZE];

	appLcmOsdClean();

	layer = menuProcLayerGet();
	item = menuProcCurrItemGet(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	menuLayerSizeGet(layer, &line, &col);			
	if (nodeItem > line)
	{
		nodeItem = line;
	}
	item = menuProcScrollGet(layer);
	
	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer, item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		//printf("layer = %d, item = %d, nodeItem = %d, ", layer, item, nodeItem);
		//printf("icon = 0x%04x, strIdx = 0x%04x\n", icon, strIdx);
		
		/*subItem = menuProcSubItemGet(layer, item);				
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}*/

		bSelect = (menuProcCurrItemGet(layer) == item) ? (TRUE) : (FALSE);
		switch(item)
		{
			case UI_SCDV_SYSTEM_DATE_TIME_YEAR:
				selItem = menuProcParaGet (layer, item);
				sprintf(osdString, "%02d", selItem + APP_RTC_DEFAULT_YEAR % 2000);				
				appLcmOsdTextShow(i * LCM_OSD_ICON_W, 0, SP5K_GFX_ALIGN_TOP_LEFT, osdString, TRUE, bSelect);
				break;
			case UI_SCDV_SYSTEM_DATE_TIME_MONTH:
			case UI_SCDV_SYSTEM_DATE_TIME_DAY:
				selItem = menuProcParaGet (layer, item);
				sprintf(osdString, "%02d", selItem + 1);				
				appLcmOsdTextShow(i * LCM_OSD_ICON_W, 0, SP5K_GFX_ALIGN_TOP_LEFT, osdString, TRUE, bSelect);
				break;
			case UI_SCDV_SYSTEM_DATE_TIME_HOUR:
			case UI_SCDV_SYSTEM_DATE_TIME_MINUTE:
			case UI_SCDV_SYSTEM_DATE_TIME_SECOND:
				selItem = menuProcParaGet (layer, item);
				sprintf(osdString, "%02d", selItem);				
				appLcmOsdTextShow(i * LCM_OSD_ICON_W, 0, SP5K_GFX_ALIGN_TOP_LEFT, osdString, TRUE, bSelect);
				break;
			case UI_SCDV_SYSTEM_DATE_TIME_OK:
			case UI_SCDV_SYSTEM_DATE_TIME_EXIT:
				appLcmOsdLargeIconShow(i, icon, bSelect);
				break;
		}

		if(bSelect == TRUE)
		{
			appLcmOsdBottomStringShow(strIdx, FALSE);
		}
	}

	appLcmRefresh();
}

void appScdvMenuSystemWifiPassWordDisplay(void)
{
	UINT16 icon;
	UINT32 strIdx;
	
	UINT8 nodeItem = 0;
	UINT8 subItem=0;
	UINT8 selItem;
	UINT8 layer = 0;
	UINT8 item = 0;
	UINT8 line, col;
	
	UINT16 i;
	
	BOOL bSelect = FALSE;
	UINT8 osdString[OSD_STRING_SIZE];

	appLcmOsdClean();

	layer = menuProcLayerGet();
	item = menuProcCurrItemGet(layer);

	if(item == UI_SCDV_SYSTEM_WIFI_PW_EXIT)
	{
		menuProcStrGet(layer, item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		appLcmOsdLargeIconShow(LcmOsdIconIndex0, icon, TRUE);
		appLcmOsdBottomStringShow(strIdx, FALSE);
	}
	else
	{
		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		menuLayerSizeGet(layer, &line, &col);
		if (nodeItem > line)
		{
			nodeItem = line;
		}
		item = menuProcScrollGet(layer);
		
		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer, item, &strIdx);
			icon = menuProcIconIDGet(layer, item);
			//printf("layer = %d, item = %d, nodeItem = %d, ", layer, item, nodeItem);
			//printf("icon = 0x%04x, strIdx = 0x%04x\n", icon, strIdx);
			
			/*subItem = menuProcSubItemGet(layer, item);				
			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}*/

			bSelect = (menuProcCurrItemGet(layer) == item) ? (TRUE) : (FALSE);
			switch(item)
			{
				case UI_SCDV_SYSTEM_WIFI_PW_0:
				case UI_SCDV_SYSTEM_WIFI_PW_1:
				case UI_SCDV_SYSTEM_WIFI_PW_2:
				case UI_SCDV_SYSTEM_WIFI_PW_3:
				case UI_SCDV_SYSTEM_WIFI_PW_4:
				case UI_SCDV_SYSTEM_WIFI_PW_5:
				case UI_SCDV_SYSTEM_WIFI_PW_6:
				case UI_SCDV_SYSTEM_WIFI_PW_7:
				case UI_SCDV_SYSTEM_WIFI_PW_8:
				case UI_SCDV_SYSTEM_WIFI_PW_9:	
					selItem = menuProcParaGet (layer, item);
					sprintf(osdString, "%d", selItem);				
					appLcmOsdTextShow(i * LCM_OSD_TEXT_W, 0, SP5K_GFX_ALIGN_TOP_LEFT, osdString, TRUE, bSelect);
					break;
				case UI_SCDV_SYSTEM_WIFI_PW_OK:
				//case UI_SCDV_SYSTEM_WIFI_PW_EXIT:
					appLcmOsdLargeIconShow(i / 2, icon, bSelect);
					break;
			}

			if(bSelect == TRUE)
			{
				appLcmOsdBottomStringShow(strIdx, FALSE);
			}
		}
	}

	appLcmRefresh();
}

void appScdvMenuSystemVersionDisplay(void)
{
	UINT8 verString[OSD_STRING_SIZE];
	UINT8 newString[OSD_STRING_SIZE];
	//Show Version
	memset(verString, '\0', OSD_STRING_SIZE);
	strcpy(verString, appVersionGet() + 4);
	appLcmOsdTextShow(0, 0, SP5K_GFX_ALIGN_TOP_LEFT, verString, TRUE, FALSE);
	
	memset(verString, '\0', OSD_STRING_SIZE);	
	#if 1
	strcpy(verString, __DATE__);
	strcpy(verString + 3, verString + 4);
	strupr(verString);
	strcat(verString, " ");
	strncat(verString, __TIME__, 5);	
	strcpy(newString, "MD87156 "); //5M9_DE06	
	//strncat(newString, verString,6);
	strncat(newString, "MAY25.",6);
	#endif
	printf("newString = %s\n", newString);	
	appLcmOsdBottomTextShow(newString, FALSE);
	
	appLcmRefresh();
}

