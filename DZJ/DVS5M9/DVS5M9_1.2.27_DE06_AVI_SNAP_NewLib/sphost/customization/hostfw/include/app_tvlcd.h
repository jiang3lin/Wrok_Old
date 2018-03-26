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
#ifndef _APP_TVLCD_H_
#define _APP_TVLCD_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "gpio_custom.h"
#include "app_user_setting.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {	
	APP_DISP_TV_NTSC = TV_NTSC,
	APP_DISP_TV_PAL = TV_PAL,
	APP_DISP_LCD, 
 	APP_DISP_HDMI ,
 	APP_DISP_NONE
} appDispMode_e;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define IS_LCD_OUT  ((appCurrDispModeGet() == APP_DISP_LCD)? 1: 0)
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appTvLcdInit(void);
void appTvLcdStart(BOOL fastBoot);
void appLcdTurnOn(void);
void appLcdTurnOff(void);
void appLcdTvSwitch(appDispMode_e dispDev, UINT32 isWaitSwitchRdy);
void appDispGfxInit(void);
appDispMode_e appCurrDispModeGet( void );

#endif  /* _APP_TVLCD_H_ */


