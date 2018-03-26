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

#ifndef _APP_OSD_DRAW_LCM_H_
#define _APP_OSD_DRAW_LCM_H_
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define OSD_STRING_SIZE	100

#define LCM_OSD_CENTER_X	48
#define LCM_OSD_CENTER_Y	16

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/


/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
extern void appLcmOsdDrawPowerOnLogo(void);
extern void appLcmOsdDrawPowerOffLogo(void);
extern void appLcmOsdDrawBattery(UINT8 bShow, UINT8 idx);
extern void appRecLcmOsdDrawBatterySecond(UINT8 bShow, UINT8 idx);


#endif	//_APP_OSD_DRAW_LCM_H_
