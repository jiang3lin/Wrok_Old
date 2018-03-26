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
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_timer.h"
#include "app_hw_def.h"
#include "gpio_custom.h"
#include "app_led.h"

 typedef enum {
	LED_STATUS_OFF  = 0,  
	LED_STATUS_ON, 
} ledStatus_e;

typedef struct ledFlickInfo_s {
	UINT8 enable;
	UINT8 ledStatus;
	UINT16 flickerMs; 
	UINT32 timeCount;	
} ledFlickInfo_t;


static ledFlickInfo_t ledFlicker[APP_LED_TOTAL] = { {FALSE, LED_STATUS_OFF, 0 , 0},{FALSE,LED_STATUS_OFF, 0, 0} };
static UINT8 ledIndicator = TRUE;

void
_ledOff(
	appLedOption_e led
)
{
	if(led == APP_LED_BUSY_FRONT) {
		LED_BUSY_FRONT_OFF;
	}
	else if(led == APP_LED_BUSY_REAR) {
		LED_BUSY_REAR_OFF;
	}
	else if(led == APP_LED_POWER) {
		LED_POWER_OFF;
	}
	else if(led == APP_LED_CHARGE) {
		LED_CHARGE_OFF;
	}
}

void
_ledOn(
	appLedOption_e led
)
{
	if(!ledIndicator)
	{
		_ledOff(led);
		return;
	}

	if(led == APP_LED_BUSY_FRONT) {
		LED_BUSY_FRONT_ON;
	}
	else if(led == APP_LED_BUSY_REAR) {
		LED_BUSY_REAR_ON;
	}
	else if(led == APP_LED_POWER) {
		LED_POWER_ON;
	}
	else if(led == APP_LED_CHARGE) {
		LED_CHARGE_ON;
	}
}

void
_ledFlickerReset(
	appLedOption_e led
)	
{	
	ledFlicker[led].enable = FALSE;
	ledFlicker[led].ledStatus = LED_STATUS_OFF;
	ledFlicker[led].timeCount = 0;
	ledFlicker[led].flickerMs = 0;
}

void appLedIndicatorEnable(BOOL bEnable)
{
	ledIndicator = bEnable;
	if(ledIndicator)
	{
		LED_RULES_NORMAL;
	}
	else
	{
		ALL_LED_OFF;
	}
}

void
appLedOn(
	appLedOption_e led
)
{
	_ledOn(led);
	_ledFlickerReset(led);
	ledFlicker[led].ledStatus = LED_STATUS_ON;
}

void
appLedOff(
	appLedOption_e led
)
{
	_ledOff(led);
	_ledFlickerReset(led);	
}

void
appLedFlickerSet(
	appLedOption_e led,
	UINT32 flickerMs
)	
{
	if (flickerMs == 0) {
		_ledOff(led);
	}
	else {
		ledFlicker[led].enable = TRUE;	
		ledFlicker[led].ledStatus = LED_STATUS_OFF;
		ledFlicker[led].timeCount = 0;
	}
	ledFlicker[led].flickerMs = flickerMs;
}


void 
appLedFlickerServiceRoutine(
	void
)
{
	UINT32 i;

	for(i = 0; i < APP_LED_TOTAL; i++ ) {
		if (ledFlicker[i].enable && ledFlicker[i].flickerMs) {
			ledFlicker[i].timeCount = ledFlicker[i].timeCount + appBackgroundTimerTickIntervalGet();
			if (ledFlicker[i].timeCount  >= ledFlicker[i].flickerMs) {				
				if (ledFlicker[i].ledStatus == LED_STATUS_ON) {
					_ledOff(i);
					ledFlicker[i].ledStatus = LED_STATUS_OFF;
					
				}
				else {
					_ledOn(i);
					ledFlicker[i].ledStatus = LED_STATUS_ON;
				}
				ledFlicker[i].timeCount = 0;
			}
		}
	}
}

