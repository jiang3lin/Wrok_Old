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
#ifndef APP_RTC_H
#define APP_RTC_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_RTC_RELIABLE_CODE    	0x5A
#define APP_RTC_DEFAULT_YEAR		2015
#define APP_RTC_MAX_YEAR			2044
#define APP_RTC_MIN_YEAR			APP_RTC_DEFAULT_YEAR

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appRTC_s {
	UINT16 year;
	UINT16 mon;
	UINT16 day;
	UINT16 hour;
	UINT16 min;
	UINT16 sec;
} appRTC_t;

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appRtcDefaultSet(void);
void appRtcSet(appRTC_t *ptime);
void appRtcWakeUpSet(appRTC_t *ptime);
void appRtcGet(appRTC_t *ptime);
BOOL appRtcLostCheck(void);
BOOL appLeapYearCheck(UINT16 year);
void  appTimezoneChange(UINT32 oldzone,UINT32 newzone);
BOOL appRtcRangeCheck(appRTC_t *ptime);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#endif  /* APP_RTC_H*/

