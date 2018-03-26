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
#include "app_battery.h"

typedef void appBatteryChargeStartCBF_t(void);
typedef void appBatteryChargeStopCBF_t(void);
typedef UINT8 appBatteryCheckStatusCBF_t(void);

void appBatteryCharge_SetStartCBFRegister(appBatteryChargeStartCBF_t *aBatteryChargeStartCBF);
void appBatteryCharge_SetStartCBF(void);
void appBatteryCharge_SetStopCBFRegister(appBatteryChargeStopCBF_t *aBatteryChargeStopCBF);
void appBatteryCharge_SetStopCBF(void);
void appBatteryCharge_SetCheckStatusCBFRegister(appBatteryCheckStatusCBF_t *aBatteryCheckStatusCBF);
UINT8 appBatteryCharge_SetCheckStatusCBF(void);
UINT8 appBatteryCharge_CBRegister(void);
appDCSRC_e appBatteryCharge_LineStateGet(void);
UINT8 appBatteryCharge_Init(void);
UINT8 appBatteryCharge_ChargerStatusGet(void);

void appBatteryCharge_Start(void);
void appBatteryCharge_Stop(void);
UINT8 appBatteryCharge_CheckStatus(void);
