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

#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"
#include "app_dev_plug.h"
#include "sp5k_usb_api.h"
#include "app_errorosd.h"
#include "sp5k_rsvblk_api.h"
#include "app_tvlcd.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//static UINT32 pccamStartTimer = 0;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void 
appUsbPccamStateClose(
	void
)
{
	if (!IS_USB_IN) 
	{					
		sp5kUsbSwCon(0);
	}	
}

void
appUsbPccamStateInit(
	void
)
{
    UINT32 ret;

	if (!IS_USB_IN) 
	{
		LCD_BACKLIGHT_ON;
		return;
	}
	
//	pccamStartTimer= appTimerSet(5000, "PCCAM Start Timer");	/* dual stream mask */
	sp5kUsbModeSet(SP5K_USB_MODE_PCCAM );
//	sp5kUsbMaxPowerSet(SP5K_USB_MAXPOWER_HIGH);

	if(pUiSetting->usbMode == USB_UVCH264){
		ret =sp5kResourceFileLoad(SP5K_RES_USB_CONF_BIN, RES_USB_USBUVC_TS);
	}else{
		ret =sp5kResourceFileLoad(SP5K_RES_USB_CONF_BIN,RES_USB_USBUVC);
	}
	
	if (ret != SUCCESS) {
		printf("USB UVC Load Resource Fail !!!\n");
	}else{
		printf("USB UVC Load Resource SUCCESS!!!\n");
	}
	
	sp5kUsbSwCon(1);
}

void 
appUsbPccamState(
	UINT32 msg,
	UINT32 param
)
{	
	
	switch (msg) 
	{
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbPccamState");
		appUsbPccamStateInit();
		break;
	case APP_STATE_MSG_CLOSE:	
		appUsbPccamStateClose();	
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		appUsbPccamStateClose();		
		break;
	case SP5K_MSG_TIMER: 
		appLcdTurnOff();
//		appTimerClear(&pccamStartTimer);	/* dual stream mask */
		break;
	default:		
		break;
	}
	
} 

