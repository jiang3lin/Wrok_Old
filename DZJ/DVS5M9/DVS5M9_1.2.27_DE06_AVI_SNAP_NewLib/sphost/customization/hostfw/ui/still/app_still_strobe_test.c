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

#include "sp5k_global_api.h"
#include "sp5k_sensor_api.h"

#include "app_timer.h"
#include "app_still.h"
#include "app_strobe.h"
#include "app_view_param_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 StillStrobeStateTestTime = STROB_TEST_TIME;
static UINT32 StillStrobeStateT = TIMER_NULL;
static UINT32 StillStrobeStateC = 0;
static UINT32 StillStrobeStateTimeOutCounter = 0;
static UINT32 GetStrobData = 0;
UINT32* pStrobValueArray=NULL;

UINT32 flash_width=120;
UINT32 flash_h_pos=1;
sp5kAeResult_t aeResult;
sp5kAwbResult_t awbResult;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
appStillStrobeStateSet(
	UINT32 flashWidth,
	UINT32 flashHPos,
	UINT32 testTime
)
{		
	flash_width = flashWidth;
	flash_h_pos = flashHPos;
	StillStrobeStateTestTime = (testTime==0)? 1 : 
		(testTime>STROB_TEST_TIME)? STROB_TEST_TIME : testTime;		
}

void
appStillStrobeStateGet(void)
{	
	UINT32 i;
	if(pStrobValueArray==NULL) {
		printf("There is no avaiable strobe data\n");
		return; 
	}
	for(i=0; i<StillStrobeStateTestTime; i++) {
		printf("%d,\n",pStrobValueArray[i]);
	}
}

void
appStillStrobeStateInit(void)
{		
	sp5kFlashlightInit(SP5K_FLASHLIGHT_INACTIVE_LO);	
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_WIDTH, flash_width);
	sp5kPreflashCfgSet(SP5K_PREFLASH_CFG_H_POS, flash_h_pos);
	appStill_SetFlash(UI_FLASH_ON);
	// In preview mode, extern the exposure time to max.
	// Let the flash can fall into the valid exp period.
	sp5kPreviewExpAgcSet(EXP_1_32 ,ISO_100);
	
	StillStrobeStateT = appTimerSet(500, "strobe timer");
	StillStrobeStateC = 0;
	GetStrobData=0;
	StillStrobeStateTimeOutCounter = 0;
	printf("Init mem\n");
	if(pStrobValueArray==NULL) {
		pStrobValueArray = (UINT32*)sp5kMalloc(STROB_TEST_TIME+10);
	}	
	printf("Init ok\n");
}

void 
appStillStrobeStateClose(void)
{	
	appTimerClear(&StillStrobeStateT);
}

void 
appStillStrobeStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{	
	switch (msg) {		
	case SP5K_MSG_TIMER:
		if(param == StillStrobeStateT && appStrobeQueryReady() && GetStrobData==0) {
			StillStrobeStateTimeOutCounter=0;
			appStill_SetFlash(UI_FLASH_OFF);
			sp5kPreflashTrig();
			appStill_SetFlash(UI_FLASH_ON);
			GetStrobData=1;
		}
		else {
			StillStrobeStateTimeOutCounter++;
		}	
		if(StillStrobeStateTimeOutCounter>20000) {
			printf("strobe charge fail \n");
		}
		break;	
	case SP5K_MSG_PREFLASH_READY:
		sp5kPreflashResultGet(param, &aeResult, &awbResult);
		if(pStrobValueArray!=NULL) {
			pStrobValueArray[StillStrobeStateC++] = aeResult.aeLuma;		
		}	
		DBG_PRINT("valid=%d, aeStep=%d, aeDevStep=%d, aeLuma=%d\n", 
			aeResult.valid, aeResult.aeStep, aeResult.aeDevStep, aeResult.aeLuma);			
		
		if(StillStrobeStateC>StillStrobeStateTestTime) {
			appStillStrobeStateClose();			
			appStateChange(appPreviousStateGet(), APP_STATE_MSG_INIT);
		}
		GetStrobData=0;
		break;
	default:
		break;
	}	
}

void 
appStillStrobeStateOnKey(
	UINT32 msg,
	UINT32 param
)
{	
	switch (msg) {		
	case APP_KEY_PRESS_S1: 			
		appStillStrobeStateClose();
		appStateChange(appPreviousStateGet(), APP_STATE_MSG_INIT);
		break;	
	default:
		break;
	}	
}

void 
appStillStrobeTestState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appStillStrobeTestState");
		appStillStrobeStateInit();
		break;		
	case APP_STATE_MSG_CLOSE:		
		appStillStrobeStateClose();
		appStateCloseDone();
		break;			
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appStillStrobeStateOnKey(msg,param);
		}
		else{
			appStillStrobeStateOnMsg(msg,param);
		}
		break;
	}

}

