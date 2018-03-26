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



#ifndef APP_TB_MAIN_H
#define APP_TB_MAIN_H
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TKTYPE_TBMENU_PERPAGE	 5

#define TKTYPE_TBMENU_MAIN_STR_BAR_X  23
#define TKTYPE_TBMENU_MAIN_STR_BAR_Y  48
#define TKTYPE_TBMENU_MAIN_BAR_Y_GAP  31

#define TKTYPE_TBMENU_MAIN_STR_X  38
#define TKTYPE_TBMENU_MAIN_STR_Y  48
#define TKTYPE_TBMENU_MAIN_STR_Y_GAP  31


#define TKTYPE_TBMENU_STATUS_STR_X  238



/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct appTbMenu_s
{
	UINT8 vmTotalTabs;
	UINT8 vmCurTab;
	UINT8 vmLastTab;
	UINT8 vmRecentlyTab;
	UINT8 vmTab[25];
	UINT32 vmEnableTab;
} appTbMenu_t, *PappTbMenu_t;


enum TbMenuItem_e
{
	TBMENU_TEST_ROBOT,	/*-- keep top-most */
	TBMENU_SENSOR_TEST,	/*-- keep top as possible */
	TBMENU_CIPA,
	TBMENU_LCD_TEST,
	TBMENU_KEY_TEST,
	TBMENU_AE_METER,
	TBMENU_FLASH,
	TBMENU_AF,
	TBMENU_BAT_CURVE,
	TBMENU_AE_DEBUG,
	TBMENU_AWB_DEBUG,
	TBMENU_TOTAL
};

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/


/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/


#endif  /* APP_TB_MAIN_H */


