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
#include "app_hw_def.h"
#include "customization/dev_init.h"
#include "gpio_custom.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX_KEY_TYPE	2
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 gpioKeyMap[GPIO_BTN_NUM] = 
{
	KEY_CODE(APP_KEY_PRESS_TELE),
	KEY_CODE(APP_KEY_PRESS_WIDE),
	KEY_CODE(APP_KEY_PRESS_S1),
	KEY_CODE(APP_KEY_PRESS_S2),
	KEY_CODE(APP_KEY_TV_IN),
	KEY_CODE(APP_KEY_PRESS_PB),
	KEY_CODE(APP_KEY_PRESS_OK),
	KEY_CODE(APP_KEY_PRESS_POWER),
};


UINT32 adcKeyMap[ADC_BTN_NUM] = 
{
	KEY_CODE(APP_KEY_PRESS_UP),
	KEY_CODE(APP_KEY_PRESS_DOWN),
	KEY_CODE(APP_KEY_PRESS_LEFT),
	KEY_CODE(APP_KEY_PRESS_RIGHT),
	KEY_CODE(APP_KEY_PRESS_SET),
	#ifdef HW_PRJ_6330
	KEY_CODE(APP_KEY_PRESS_PB),
	#else
	KEY_CODE(APP_KEY_PRESS_DEL),
	#endif
	KEY_CODE(APP_KEY_PRESS_MENU),
	KEY_CODE(APP_KEY_PRESS_MODE),
};

static UINT8 keyInitStatus[GPIO_BTN_NUM + ADC_BTN_NUM] ;
static BOOL firstKeyIgnoreFlag[GPIO_BTN_NUM + ADC_BTN_NUM];

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appPwerOnInfoGet(BOOL *pbPowerOn,BOOL *mutePowerOn,BOOL *testModePowerOn);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appGpioMsg2Key(UINT32 msgId,UINT32 msgParam);
UINT32 appAdcMsg2Key(UINT32 msgId,UINT32 msgParam);
void appGpioMsg2LongKey(UINT32 *pkey);


void
appKeyInitStatusGet(
	void
)
{
	UINT32 i;
	keyInitStatus[BTN_ID_TELE] = 0;
	keyInitStatus[BTN_ID_WIDE] = 0;
	keyInitStatus[BTN_ID_S1] = 0;
	keyInitStatus[BTN_ID_S2] = 0;
	keyInitStatus[BTN_ID_TV] = 0;
	keyInitStatus[BTN_ID_PB] = 0;
	keyInitStatus[BTN_ID_OK] = 0;
	keyInitStatus[BTN_ID_POWER] = 0;

	keyInitStatus[BTN_ID_RIGHT + BTN_ID_GPIO_TOTAL] = 0;
	keyInitStatus[BTN_ID_UP + BTN_ID_GPIO_TOTAL] = 0;
	keyInitStatus[BTN_ID_DOWN + BTN_ID_GPIO_TOTAL] = 0;
	keyInitStatus[BTN_ID_LEFT + BTN_ID_GPIO_TOTAL]  = 0;
	keyInitStatus[BTN_ID_MODE + BTN_ID_GPIO_TOTAL] = 0;
	keyInitStatus[BTN_ID_DEL + BTN_ID_GPIO_TOTAL] = 0;
	keyInitStatus[BTN_ID_SET + BTN_ID_GPIO_TOTAL] = 0;
	keyInitStatus[BTN_ID_MENU + BTN_ID_GPIO_TOTAL] = 0;
	
	for (i = 0; i < (GPIO_BTN_NUM + ADC_BTN_NUM); i++) 
	{
		if(keyInitStatus[i]) 
		{
			firstKeyIgnoreFlag[i] = TRUE;
			printf("keyInitStatus[%d] = %d\n",i,keyInitStatus[i]);
		}
		else 
		{
			firstKeyIgnoreFlag[i] = FALSE;
		}
	}	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: appBtnMsgToKey                                         *
 *                                                                        *
 **************************************************************************/
extern UINT8 nDisableHdmiDetect;
UINT32
appBtnMsgToKey(
	UINT32 msgId,
	UINT32 msgParam
)
{
	UINT32 key = APP_KEY_ERROR;

	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msgId,msgParam);

	switch (msgId) {
	/* button message */
	case SP5K_MSG_GPIO_BUTTON_PRESS:
	case SP5K_MSG_GPIO_BUTTON_RELEASE:
	case SP5K_MSG_GPIO_BUTTON_REPEAT:
		key = appGpioMsg2Key(msgId, msgParam);
		appGpioMsg2LongKey(&key); 
		break;
	case SP5K_MSG_ADC_BUTTON_PRESS:
	case SP5K_MSG_ADC_BUTTON_RELEASE:
	case SP5K_MSG_ADC_BUTTON_REPEAT:
		key = appAdcMsg2Key(msgId, msgParam);
		break;
	case SP5K_MSG_USB_DEVICE_VBUS_IN:
		key = APP_KEY_USB_IN;
		break;
	case SP5K_MSG_USB_DEVICE_VBUS_OFF:
		key = APP_KEY_USB_OUT;
		break;
	case SP5K_MSG_HDMI_PLUG:
		//printf("SP5K_MSG_HDMI_PLUG=%d\n",msgParam);
		if(nDisableHdmiDetect==1)
			key = APP_KEY_ERROR;
		else
			key = APP_KEY_HDMI_IN;
		//printf("nDisableHdmiDetect1=%d\n",nDisableHdmiDetect);
		break;
	case SP5K_MSG_HDMI_UNPLUG:
		//printf("SP5K_MSG_HDMI_UNPLUG=%d\n",msgParam);
		if(nDisableHdmiDetect==1)
			key = APP_KEY_ERROR;
		else
			key = APP_KEY_HDMI_OUT;
		//printf("nDisableHdmiDetect2=%d\n",nDisableHdmiDetect);
		break;
	default:
		key = APP_KEY_ERROR;
		DBG_PRINT("Unknow message : 0x%x (0x%x)\n", msgId, msgParam);
		break;
	}
	DBG_PRINT("  -> key : %08x\n",key); 
	return 	key;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: appGpioMsg2Key                                *
 *                                                                        *
 **************************************************************************/
UINT32
appGpioMsg2Key(
	UINT32 msgId,
	UINT32 msgParam
)
{
	UINT8 key;

	if (msgParam >= GPIO_BTN_NUM/* || msgId==SP5K_MSG_GPIO_BUTTON_REPEAT*/) 
	{
 		return APP_KEY_ERROR; 
 	} 
	if(firstKeyIgnoreFlag[msgParam]) 
	{
		firstKeyIgnoreFlag[msgParam] = FALSE;
		printf("[WARN]Ignore KEY message(0x%x) .... \n\n",gpioKeyMap[msgParam] | (msgId==SP5K_MSG_GPIO_BUTTON_PRESS ? KEY_PRESS : KEY_RELEASE));
		return APP_KEY_SKIP;
	}
	if(msgId==SP5K_MSG_GPIO_BUTTON_PRESS)
	{
		key = KEY_PRESS;
	}
	else if(msgId==SP5K_MSG_GPIO_BUTTON_RELEASE)
	{
		key = KEY_RELEASE;
	}
	else
	{
		key = KEY_REPEAT;
	}	
	HLPrintf1("key msg=0x%x\n",gpioKeyMap[msgParam] | key)
	return gpioKeyMap[msgParam] | key;
}

void appGpioMsg2LongKey(UINT32 *pkey)
{
	UINT32 i = 0;
	UINT32 LongPressTimes;

	if(*pkey == APP_KEY_PRESS_OK)
	{
		LongPressTimes = 5;
		do
		{
			i ++;
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		}
		while(IO_KEY_OK_GET == TRUE && i < LongPressTimes);

		if(i >= LongPressTimes)
		{
			*pkey = APP_KEY_LONG_PRESS_OK;
			HLPrintf1("key long press msg=0x%x\n", *pkey)
		}
	}
	else if(*pkey == APP_KEY_PRESS_POWER)
	{
		LongPressTimes = 10;
		do
		{
			i ++;
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
		}
		while(IO_KEY_POWER_GET == TRUE && i < LongPressTimes);

		if(i >= LongPressTimes)
		{
			*pkey = APP_KEY_LONG_PRESS_POWER;
			HLPrintf1("key long press msg=0x%x\n", *pkey)
		}
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: appAdcMsg2Key                                 *
 *                                                                        *
 **************************************************************************/
UINT32
appAdcMsg2Key(
	UINT32 msgId,
	UINT32 msgParam
)
{
	UINT8 key;
	#if 0	/* key repeat enable */
	if (msgId==SP5K_MSG_ADC_BUTTON_REPEAT)
	{
		return APP_KEY_SKIP;
	}
	#endif
	if (msgParam < ADC_BTN_NUM) 
	{  /* ADC button decode */
		if(firstKeyIgnoreFlag[msgParam + BTN_ID_GPIO_TOTAL]) 
		{
			firstKeyIgnoreFlag[msgParam + BTN_ID_GPIO_TOTAL] = FALSE;
			printf("[WARN]Ignore KEY message(0x%x) .... \n\n",adcKeyMap[msgParam]  | (msgId==SP5K_MSG_ADC_BUTTON_PRESS ? KEY_PRESS : KEY_RELEASE));
			return APP_KEY_SKIP;
		}
		if(msgId==SP5K_MSG_ADC_BUTTON_PRESS)
		{
			key = KEY_PRESS;
		}
		else if(msgId==SP5K_MSG_ADC_BUTTON_RELEASE)
		{
			key = KEY_RELEASE;
		}
		else
		{
			key = KEY_REPEAT;
		}
		HLPrintf1("key msg=0x%x\n",adcKeyMap[msgParam] | key);
 		return (adcKeyMap[msgParam] | key);
 	}
	else 
	{
		DBG_PRINT("appAdcMsg2Key err:[0x%x %x]\n", msgId, msgParam);
		return APP_KEY_ERROR;
	}
	return APP_KEY_ERROR;
}

