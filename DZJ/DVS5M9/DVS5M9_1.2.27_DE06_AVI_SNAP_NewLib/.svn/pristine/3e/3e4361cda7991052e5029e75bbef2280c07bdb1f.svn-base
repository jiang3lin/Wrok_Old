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
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_timer.h"
#include "app_hw_def.h"
#include "app_battery.h"
#include "app_errorosd.h"
#include "gpio_custom.h"
#include "app_view_osd.h"
#include "app_ui_para.h"
#include "sp5k_usb_api.h"

#define BATTERY_CHECK_TIME_OUT  appBackgroundTimerTickIntervalGet() //20 ms

/* 
DVS5M2 HW
825	--> 4.2V
805	--> 4.1V
785	--> 4.0V
765	--> 3.9V
745	--> 3.8V
722	--> 3.7V
702	--> 3.6V
682	--> 3.5V
*/

static appBattery_t battSetting[BATT_TYPE_MAX] = 
{ 
  /* sysBoot-lensBoot-lensMove-shutDown-L0-  L1-  L2-  L3-  	sampleNum-reserved */
  {   700,        700,        700,        700,     { 720, 730, 740,  748 },    15,             0 }, /* akaline */
  {   700,        700,        700,        700,     { 720, 736, 748,  0 },       15,             0 }, /* NiMH */		  
  //{   675,        675,        675,        675,     { 740, 770, 835,  0 },       15,             0 }, /* Lithium */
  {   692,        692,        692,        692,     { 692, 720, 750,  775 },       30,             0 }, /* Lithium for DVS5M2*/	
  {   600,        600,        600,        600,     { 612, 644, 688,  0 },       15,             0 }, /* Oxyride */	
};

static appBattery_t *pBattSetting = &battSetting[0];
UINT32 isBatteryCheckEnable = 1;
static UINT32 samplingCount = 0;
static UINT32 battVoltageSum = 0;
static UINT32 latestBattVoltageLevel = MAX_BATTERY_LEVEL_IN_USED;
	

UINT32
_batteryAdcValueGet(
	UINT32 *pvalue
)
{
	if(sp5kAdcRead(ADC_BATTERY_CH, pvalue)== SUCCESS)
	{	
		//printf("*pvalue = %d\n", *pvalue);
		return SUCCESS;
	}
	else
	{
		printf("[ERR] sp5kAdcRead(ADC_BATTERY_CH) failed !!!\n");
		return FAIL;
	}
}

void
appBatteryLevelPolling(
	void
)
{	
	UINT32 battAdcValue;
	UINT32 battVoltageAvg = 0;
	static UINT32 BattChargeTimes = 0, BattChargeLevel = MAX_BATTERY_LEVEL_IN_USED;
	
	if(!isBatteryCheckEnable)
	{
		return;
	}

	if(BATTERY_CHECK_DISABLE == 1)
	{
		latestBattVoltageLevel = MAX_BATTERY_LEVEL_IN_USED;
	}
	else if(IS_USB_IN)
	{
		/*if(IS_BATTERY_CHARGING == TRUE)
		{
			BattChargeTimes ++;
			if(BattChargeTimes * 20 < 500)	//see BACKGROUND_TIMER_TICK
			{
				return;
			}
			
			BattChargeTimes = 0;	
			if(BattChargeLevel >=  MAX_BATTERY_LEVEL_IN_USED)
			{
				BattChargeLevel = APP_BATT_LEVEL_0;
			}
			else
			{
				BattChargeLevel ++;
			}
			sp5kHostMsgSend(APP_UI_MSG_BATT_LEVEL_CHANGE, BattChargeLevel);
			return;
		}
		else*/
		{
			latestBattVoltageLevel = MAX_BATTERY_LEVEL_IN_USED;
		}
	}
	else if(_batteryAdcValueGet(&battAdcValue) != SUCCESS) 
	{
		return;
	}

	if(BattChargeTimes != 0)
	{
		BattChargeTimes = 0;	//fliter charge ic pulse
		sp5kHostMsgSend(APP_UI_MSG_BATT_LEVEL_CHANGE, latestBattVoltageLevel);	//refresh battery icon when out of battery
	}
	
	samplingCount++;	
	battVoltageSum = battVoltageSum + battAdcValue;

	if(samplingCount < pBattSetting->sampleNum) 
	{	
		return;
	}
	
	// calculate average voltage	
	battVoltageAvg = battVoltageSum / pBattSetting->sampleNum;
	samplingCount = 0;
	battVoltageSum = 0;

	//shut down check 
	if (battVoltageAvg < pBattSetting->shutDown) 
	{
		printf("[BATT-EMPTY] ShutDown !!(%d : %d\n",battVoltageAvg,pBattSetting->shutDown);
		appViewOsd_AllClean();
		appOsdMessageBoxShow(TRUE,ID_STR_IMAGE_SAVED);
		appTimeDelayMs(2500);
		appPowerOffUiMsgSend(APP_POWER_OFF_BATT_EMPTY);
		return;
	}

	/* (battVoltageAvg > shutDown voltage) and (battVoltageAvg < APP_BATT_LEVEL_0) */
	if (latestBattVoltageLevel == APP_BATT_LEVEL_0) 
	{
        printf("battery level 0 !\n");
        //return;  
	}

	/* battVoltageAvg > (latestBattVoltageLevel) */	
	if (battVoltageAvg > pBattSetting->level[latestBattVoltageLevel]) 
	{
	    if(latestBattVoltageLevel < MAX_BATTERY_LEVEL_IN_USED)
        {   
	    latestBattVoltageLevel = latestBattVoltageLevel + 1;
		sp5kHostMsgSend(APP_UI_MSG_BATT_LEVEL_CHANGE, latestBattVoltageLevel);
        //printf("battery AdcValue %d\n",battVoltageAvg);
        }
		return;
	}
	
	if (battVoltageAvg <= pBattSetting->level[latestBattVoltageLevel - 1] ) 
	{
	    if(latestBattVoltageLevel > APP_BATT_LEVEL_0)
        {
		latestBattVoltageLevel = latestBattVoltageLevel - 1;
		sp5kHostMsgSend(APP_UI_MSG_BATT_LEVEL_CHANGE, latestBattVoltageLevel);
        //printf("battery AdcValue %d\n",battVoltageAvg);
        }
		return; 
	}					
}

UINT32
appBatteryLevelLoad(
	UINT8 *pbuf
)
{
	if(pbuf == NULL) 
	{
		printf("[ERR] load battery setting file error!!! : %s(%d)\n",__FUNCTION__,__LINE__);
		return FAIL;
	}
	if(ReadFile(RES_CALIB_BATTERY_SET, (UINT8*)(pbuf), BATT_TYPE_MAX*sizeof(appBattery_t))!= SUCCESS)
	{
		printf("[ERR] read file[%s] fail\n", RES_CALIB_BATTERY_SET);
		/* set default */		
		return FAIL;
	}	
	return SUCCESS;
}

UINT32
appBatteryLevelSave(
	UINT8 *pbuf
)
{
	if(pbuf == NULL)
	{
		printf("[ERR] save battery setting file error!!! : %s(%d)\n",__FUNCTION__,__LINE__);
		return FAIL;
	}
	if(WriteFile(RES_CALIB_BATTERY_SET, (UINT8*)(pbuf), BATT_TYPE_MAX*sizeof(appBattery_t))!=SUCCESS) 
	{		
		printf("[ERR] write file[%s] fail\n", RES_CALIB_BATTERY_SET);
		return FAIL;
	}
	
	return SUCCESS;
}

void 
appBatteryCheckInit(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	profLogAdd(0, "batt Check init");


	#if HOST_DBG
	UINT32 i;
	printf("\n\nsysBoot : lensBoot : lensMove : shutDown : level0 : level1 : level2 : level3 : sampleNum \n");
	for(i=0; i<BATT_TYPE_MAX;i++ ) 
	{				
		printf("  %d :     %d :     %d :     %d :     %d :     %d :     %d :     %d :     %d\n",
			battSetting[i].sysBoot, battSetting[i].lensBoot, battSetting[i].lensMove, 
			battSetting[i].shutDown, battSetting[i].level[0], battSetting[i].level[1],
			battSetting[i].level[2], battSetting[i].level[3], battSetting[i].sampleNum);		
	}
	printf("\n\n");
	#endif
	
	//load battery level setting
	if(puiPara->batteryType >= BATT_TYPE_MAX)
	{
		puiPara->batteryType = BATTERY_TYPE_DEFAULT;
	}
	pBattSetting = &battSetting[puiPara->batteryType];
	printf("puiPara->batteryType = %d\n", puiPara->batteryType);
}


void
appBatteryCheckEnableSet(
	BOOL enable
)
{
	isBatteryCheckEnable = enable;
}

UINT32 
appBatteryLevelGet(
	void
)
{
	return latestBattVoltageLevel;
}

UINT32
appBatteryTypeChange(
	battType_e battType
)
{
	UINT32 flag = isBatteryCheckEnable;
	
	if(battType >= BATT_TYPE_MAX) 
	{
		return FAIL;
	}
	isBatteryCheckEnable = FALSE;
	pBattSetting = &battSetting[battType];
	isBatteryCheckEnable = flag;
	return SUCCESS;
}

UINT32
appLowBatteryCheck(
	 appBatteryCheckOption_e option
)
{
	UINT32 ret = FALSE;
	UINT32 battAdcValue;

	#if BATTERY_CHECK_DISABLE
	return FALSE;
	#endif
    if(IS_USB_IN)
		return FALSE;
	
	if (_batteryAdcValueGet(&battAdcValue) != SUCCESS) 
	{
		return FALSE;
	}
	
	switch(option) 
	{
		case APP_BATT_CHK_SYS_BOOT:
			if (battAdcValue < pBattSetting->sysBoot)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_LENS_BOOT:
			if (battAdcValue < pBattSetting->lensBoot)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_LENS_MOVE:
			if (battAdcValue < pBattSetting->lensMove)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_AF_MOVE:
			if (battAdcValue < pBattSetting->lensMove)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_STROBE_CHARGE:
			if (battAdcValue < pBattSetting->lensMove)
			{
				ret = TRUE;
			}
			break;
		default:
			printf("[ERR] : %s (%d)\n",__FUNCTION__, __LINE__);
			break;
	}	
	if (ret) 
	{
		printf("[BATT-EMPTY] appLowBatteryCheck(%d) = %d!!!\n",option,battAdcValue);
		appPowerOffUiMsgSend(APP_POWER_OFF_BATT_EMPTY);
	}
	return ret;
}

UINT32
appLensBootBatteryLevelGet(
	 void
)
{
	return pBattSetting->lensBoot;
}

UINT32
appBatteryAdcValueGet(
	void
)
{
	UINT32 value;

	if(_batteryAdcValueGet(&value) != SUCCESS)
	{
		return 0x00;
	}
	return value;
}

