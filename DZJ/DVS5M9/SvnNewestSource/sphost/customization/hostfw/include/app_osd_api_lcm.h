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

#ifndef _APP_OSD_API_LCM_H_
#define _APP_OSD_API_LCM_H_
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//#define LCM_USE_OSD_LAYER
#define LCM_USE_USR_LAYER
//#define LCM_USE_OSD_FORMAT_IDX8
#define LCM_USE_OSD_FORMAT_RGB565

#define ID_FONT_TEXT_ENGLISH_24			"UI\\FONT\\ENGLISH_24.SFN"
#define ID_FONT_TEXT_ENGLISH_8			"UI\\FONT\\ENGLISH_8.SFN"
//ID_STR_XXX See app_menu_tab.h or _MENU_TAB_H_, base iCatchTek.xls\String

#define ID_STRING_TABLE_ENGLISH			"UI\\SST\\ENGLISH.SST"

#define ID_FONT_ICON_32X24					"UI\\ICON\\ICON_32X24.SFN"
//ID_ICON_XXX See app_icon_def.h or _APP_ICON_DEF_H_

#define ID_FONT_ICON_16X8					"UI\\ICON\\ICON_16X8.SFN"

//for ID_FONT_ICON_16X8 index
#define ID_ICON_BATTERY_000				0x0000
#define ID_ICON_BATTERY_001				0x0001
#define ID_ICON_BATTERY_011				0x0002
#define ID_ICON_BATTERY_111				0x0003

//for TEXT show api (appLcmOsdTipsTextShow)
#define ID_TEXT_CARD_FULL					"\x02\x46\x55\x4c\x4c"				//FULL
#define ID_TEXT_CARD_ERROR					"\x02\x45\x52\x52\x4F\x52"			//ERROR
#define ID_TEXT_CARD_MOUNTING				"\x02\x4D\x4F\x55\x4E\x54"			//MOUNT
#define ID_TEXT_CARD_NULL					"\x02\x4E\x4F\x43\x41\x52\x44"		//NOCARD
#define ID_TEXT_CARD_LOCKED				"\x02\x4C\x4F\x43\x4B\x45\x44"		//LOCKED
#define ID_TEXT_CARD_ICON					'\x02'
#define ID_TEXT_REC_ICON					'\x01'

#define LCM_OSD_BATTERY_X					80
#define LCM_OSD_BATTERY_Y					24

#define LCM_OSD_ICON0_X					0
#define LCM_OSD_ICON0_Y					0
#define LCM_OSD_ICON1_X					32
#define LCM_OSD_ICON1_Y					0
#define LCM_OSD_ICON2_X					64
#define LCM_OSD_ICON2_Y					0

#define LCM_OSD_LARGE_TEXT_X				96
#define LCM_OSD_LARGE_TEXT_Y				0

#define LCM_OSD_MODE_TEXT_X				0+16	//0
#define LCM_OSD_MODE_TEXT_Y				24+4	//24

#define LCM_OSD_TIPS_TEXT_X				32 + 24
#define LCM_OSD_TIPS_TEXT_Y				24

#define LCM_OSD_BOTTOM_TEXT_X				48
#define LCM_OSD_BOTTOM_TEXT_Y				24

#define LCM_OSD_ICON_W						32
#define LCM_OSD_ICON_H						24

#define LCM_OSD_TEXT_W						16
#define LCM_OSD_TEXT_H						24

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum
{
	LcmOsdIconIndex0 = 0x00,
	LcmOsdIconIndex1,
	LcmOsdIconIndex2,
}lcmOsdIconIndex_t;

/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

extern void dispRefresh(void);
extern void dispClean(void);
extern void dispFlip(BOOL bFlip);

extern void appLcmRefresh(void);
extern void appLcmClean(void);
extern void appLcmFlip(BOOL bFlip);
extern void appLcmBrightness(UINT32 duty);

extern void appLcmOsdInit(void);
extern void appLcmOsdIconColorInvert(BOOL bColorInvert);
extern void appLcmOsdTextColorInvert(BOOL bColorInvert);
extern void appLcmOsdClean(void);
extern void appLcmOsdBatteryIconShow(UINT32 BatteryIconId, BOOL bColorInvert);
extern void appLcmOsdLargeIconShow(lcmOsdIconIndex_t IconIndex, UINT32 IconID, BOOL bColorInvert);
extern void appLcmOsdLargeTextShow(CHAR* String, BOOL bColorInvert);
extern void appLcmOsdLargeStringShow(UINT32 StringID, BOOL bColorInvert);
extern void appLcmOsdModeTextShow(CHAR* String, BOOL bColorInvert);
extern void appLcmOsdModeStringShow(UINT32 StringID, BOOL bColorInvert);
extern void appLcmOsdTipsTextShow(CHAR* String, BOOL bColorInvert);
extern void appLcmOsdTipsStringShow(UINT32 StringID, BOOL bColorInvert);
extern void appLcmOsdBottomTextShow(CHAR* String, BOOL bColorInvert);
extern void appLcmOsdBottomStringShow(UINT32 StringID, BOOL bColorInvert);
extern void appLcmOsdIconShow(SINT32 x, SINT32 y, UINT32 align, UINT32 IconId, BOOL bLargeFont, BOOL bColorInvert);
extern void appLcmOsdTextShow(SINT32 x, SINT32 y, UINT32 align, UINT8 *szStr, BOOL bLargeFont, BOOL bColorInvert);
extern void appLcmOsdStringShow(SINT32 x, SINT32 y, UINT32 align, UINT32 StrId, BOOL bLargeFont, BOOL bColorInvert);
extern void appLcmOsdTest(BOOL bColorInvert);

#endif	//_APP_OSD_API_LCM_H_

