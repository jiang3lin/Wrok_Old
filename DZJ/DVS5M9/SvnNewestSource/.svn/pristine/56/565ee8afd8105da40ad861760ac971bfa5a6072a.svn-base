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
#ifndef _APP_WIFI_CONNECTION_H_
#define _APP_WIFI_CONNECTION_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "ndk_global_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#define WIFI_SEARCH_TIMEOUT 60

enum {
	WIFI_UTILITY_NULL,
	WIFI_UTILITY_CLOSE,
	WIFI_AP_DISASSOC,
	WIFI_UTILITY_OPEN,
	WIFI_UTILITY_OPEN_AND_ASSOC,
};

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appWiFiConnectionState(UINT32 msg, UINT32 param);
void appWiFiConnectionStateOnKey(UINT32 msg, UINT32 param);
void appWiFiConnectionStateOnMsg(UINT32 msg, UINT32 param);
void appNetSysEventHandler(NDKSysEvt evt, unsigned long param, unsigned long udata);
void appWiFiConnectionOff(void);
void appWiFiConnection_StrDraw(UINT8* str);
void appWiFiConnectionOn(void);
void appWiFiConnection_UtilityStateSet(UINT8 nWiFiUtility);
UINT8 appWiFiConnection_UtilityStateGet(void);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

extern UINT8  nWifiRecKeySkip;
#endif  /* _APP_WIFI_CONNECTION_H_ */


