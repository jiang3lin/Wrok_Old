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



#ifndef APP_CAMCFG_MAIN_H
#define APP_CAMCFG_MAIN_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TKTYPE_CCMENU_PERPAGE	 5

#define TKTYPE_CCMENU_MAIN_STR_BAR_X  23
#define TKTYPE_CCMENU_MAIN_STR_BAR_Y  48
#define TKTYPE_CCMENU_MAIN_BAR_Y_GAP  31

#define TKTYPE_CCMENU_MAIN_STR_X  38
#define TKTYPE_CCMENU_MAIN_STR_Y  48
#define TKTYPE_CCMENU_MAIN_STR_Y_GAP  31


#define TKTYPE_CCMENU_STATUS_STR_X  238



/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct appCcMenu_s
{
	UINT8 vmTotalTabs;
	UINT8 vmCurTab;
	UINT8 vmLastTab;
	UINT8 vmRecentlyTab;
	UINT8 vmTab[25];
	UINT32 vmEnableTab;
} appCcMenu_t, *PappCcMenu_t;



enum CcMenuItem_e
{
	CCMENU_CARD_FW_UPDATE,	/*-- keep top-most item */
	CCMENU_FAC_DEFAULT,		/*-- keep top items as possible */
	CCMENU_USB_ISP,
	CCMENU_PV_SAVE_RAW,
	CCMENU_CAP_SAVE_RAW,
	CCMENU_RAWBATCH_ENABLE,
	CCMENU_PREFH_STAMP,
	CCMENU_AF_FULL_SCAN,
	CCMENU_TOTAL
};

typedef struct appCcMenuItemSelect_s{
	unsigned short gCcCommon[2];	/*-- For those common items */
	unsigned short gCcUsbIsp[2];
	unsigned short gCcPvSaveRaw[2];
	unsigned short gCcCapSaveRaw[2];
	unsigned short gCcRawbatchEnable[2];
	unsigned short gCcPrefhStamp[2];
	unsigned short gCcAfFullScan[2];
	/* Put new items above ---------------- */
	unsigned short gNullClbt[2];
}appCcMenuItemSelect_t, *PappCcMenuItemSelect_s;


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/


/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/



#endif  /* APP_CAMCFG_MAIN_H */


