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
#ifndef APP_BUZZER_H
#define APP_BUZZER_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_com_api.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define BUZZER_PWM_OFF	0
#define BUZZER_PWM_LOOP	0xFFFF

#define BEEP_KEY_PRESS     {AppBuzzerOn(180,80);sp5kTimerIsrOneShotReg( 0, 180, AppBuzzerCB);}
#define BEEP_POWER_ON     {AppBuzzerOn(180,80);sp5kTimerIsrOneShotReg( 0, 180, AppBuzzerCB);}


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct SBuzzerParam
{
	UINT16 frequnecy;
	UINT16 duty;
	UINT16 duration;
}sBuzzerParam;

enum	//Buzzer play type
{
	BUZZER_BEEP = 0x01,
	BUZZER_POWER_ON,
	BUZZER_POWER_OFF,
	BUZZER_WARNING,
	BUZZER_CAPTURE,
};


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern UINT32 buzzerTimer ;


/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void AppBuzzerOn(UINT32 freq,UINT32 duty);
void AppBuzzerOff(void);
void AppBuzzerCB(void);
void appBuzzerPlay(UINT32 BuzzerType);
void appBuzzerStop(UINT8 bWaitFinish);
void appBuzzerEnable(BOOL bEnable, UINT32 duty);


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/



#endif  /* APP_BUZZER_H*/

