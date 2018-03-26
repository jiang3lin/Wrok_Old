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
#if SPCA5330
#include "../customization/spca5330/drivers/sensor/flow/sensor_model.h"
#else
#include "../customization/drivers/sensor/flow/sensor_model.h"

#endif
#include "customization/dev_init.h"

#include "gpio_custom.h"


void 
appSensorPowerOff(
	void
)
{
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_HDR_OFF);
	DBG_PRINT("sen rst off\n");
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_RST_OFF);
#if (SP5K_SENSOR_TG_AD9920_SONY646  ==   1)
	UINT32	id;
	UINT8 regdata[] = {
		0x00,0x30,0x00,0x00,0x00,
	};
	id = sensorCmdportIdGet(SENSOR_CMDPORTID_SEL_AFE);
	sensorSerialAsyncWrite(id, 1, regdata);
	sp5kTimeDelay( SP5K_TIME_DELAY_1MS , 10);
#endif
	DBG_PRINT("sen off\n");
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_OFF);
	#if SP5K_SENSOR_OV2715
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_OFF);
	#endif

}


void 
appAfePowerCtrl(
	UINT32 fWakeup
)
{
#if (SP5K_SENSOR_AFE_AD9920 == 1)
	UINT32 id;
	static UINT8 regdata[5]={0, 0x50, 0, 0, 0};
	id = sensorCmdportIdGet(0);
	regdata[1] = fWakeup ? 0x40 : 0x50;
	sensorSerialAsyncWrite(id, 1, regdata);
#endif
#if (SP5K_SENSOR_AFE_PANX12081A == 1)
	UINT32 id;
	static UINT8 regdata[5]={0, 0x50, 0, 0, 0};
	id = sensorCmdportIdGet(0);
	regdata[1] = fWakeup ? 0x40 : 0x50;
	sensorSerialAsyncWrite(id, 1, regdata);
#endif
#if (SP5K_SENSOR_AFE_AD9000 == 1)
	UINT32 id;
	static UINT8 regdata[4]={0x20, 0xC0, 0x7f, 0};
	id = sensorCmdportIdGet(0);
	regdata[2] = fWakeup ? 0x7F : 0x1C;
	sensorSerialAsyncWrite(id, 1, regdata);
#endif	
}


void 
appSensorPowerDownSet(
	UINT8 PWRDN
)
{		 
	if (PWRDN ==TRUE) 
	{
	    #if SP5K_SENSOR_OV2715		 
			/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL, 0x00000001L << (10), 0x00000001L << (10));  
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, 0x00000001L << (4), 0x00000000L << (4));*/
		#endif
		//IO_SENSOR_STB_ON;
		sp5kSystemCfgSet(SP5K_SENSOR_SYS_CFG,SENSOR_SYS_CFG_AUTO_PWRSAVE_TIMEOUT, 0);		//cmd: syscfg 7 0 100
	}
	else
	{
		#if SP5K_SENSOR_OV2715		 
		/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL, 0x00000001L << (10), 0x00000000L << (10));
			sp5kGpioWrite(SP5K_GPIO_GRP_TGL, 0x00000001L << (4), 0x00000001L << (4));*/
		#endif

		//IO_SENSOR_STB_OFF;
	}
}

