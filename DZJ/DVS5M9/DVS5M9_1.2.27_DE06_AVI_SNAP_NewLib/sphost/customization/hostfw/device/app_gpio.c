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
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_def.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "gpio_custom.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

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

/**************************************************************************
 * Function Name: gpioPinLevelGet
 **************************************************************************/
UINT32
gpioPinLevelGet(
	UINT32 ioGrp,
	UINT32 ioMask
)
{
	UINT32 ioPinLevel;
	sp5kGpioRead(ioGrp, ioMask, &ioPinLevel);
	return (!!ioPinLevel);
}

UINT32 dummyioLcdBL;
UINT32 gLCDStandby = FALSE;
UINT32 gLCDBackLight = FALSE;

/**************************************************************************
 * Function Name: gpioInitialCfgSet
 * Purpose : To configure all camera hw IOs at the camera power up time.
 **************************************************************************/
void
gpioInitialCfgSet(
	void
)
{
	#ifdef HW_DVS5M2
	
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL, SPCA_TGL_GPIO_MASK, SPCA_TGL_GPIO_CFGIO);
	sp5kGpioPullSet(SP5K_GPIO_GRP_TGL, SPCA_TGL_GPIO_MASK, SP5K_GPIO_TGL_PULL);
	sp5kGpioDrvSet(SP5K_GPIO_GRP_TGL, SPCA_TGL_GPIO_MASK, SP5K_GPIO_TGL_DS0_MIN, SP5K_GPIO_TGL_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SPCA_TGL_GPIO_MASK, SPCA_TGL_GPIO_INIT_VAL);
	
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_MASK, SPCA_LMI_GPIO_CFGIO);
	sp5kGpioPullSet(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_MASK, SP5K_GPIO_LMI_PULL);
	sp5kGpioDrvSet(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_MASK, SP5K_GPIO_LMI_DS0_MIN, SP5K_GPIO_LMI_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_MASK, SPCA_LMI_GPIO_INIT_VAL);


	sp5kGpioCfgSet(SP5K_GPIO_GRP_FML, SPCA_FML_GPIO_MASK, SPCA_FML_GPIO_CFGIO);
	sp5kGpioPullSet(SP5K_GPIO_GRP_FML, SPCA_FML_GPIO_MASK, SP5K_GPIO_FML_PULL);
	sp5kGpioDrvSet(SP5K_GPIO_GRP_FML, SPCA_FML_GPIO_MASK, SP5K_GPIO_FML_DS0_MIN, SP5K_GPIO_FML_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, SPCA_FML_GPIO_MASK, SPCA_FML_GPIO_INIT_VAL);

	sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH, SPCA_FMH_GPIO_MASK, SPCA_FMH_GPIO_CFGIO);
	sp5kGpioPullSet(SP5K_GPIO_GRP_FMH, SPCA_FMH_GPIO_MASK, SP5K_GPIO_FMH_PULL);
	sp5kGpioDrvSet(SP5K_GPIO_GRP_FMH, SPCA_FMH_GPIO_MASK, SP5K_GPIO_FMH_DS0_MIN, SP5K_GPIO_FMH_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_FMH, SPCA_FMH_GPIO_MASK, SPCA_FMH_GPIO_INIT_VAL);

	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, SPCA_GEN_GPIO_MASK, SPCA_GEN_GPIO_CFGIO);
	sp5kGpioPullSet(SP5K_GPIO_GRP_GEN, SPCA_GEN_GPIO_MASK, SP5K_GPIO_GEN_PULL);
	sp5kGpioDrvSet(SP5K_GPIO_GRP_GEN, SPCA_GEN_GPIO_MASK, SP5K_GPIO_GEN_DS0_MIN, SP5K_GPIO_GEN_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN, SPCA_GEN_GPIO_MASK, SPCA_GEN_GPIO_INIT_VAL);

	/*sp5kGpioCfgSet(SP5K_GPIO_GRP_DISPL, SPCA_DISPL_GPIO_MASK, SPCA_DISPL_GPIO_CFGIO);
	sp5kGpioPullSet(SP5K_GPIO_GRP_DISPL, SPCA_DISPL_GPIO_MASK, SP5K_GPIO_DISPL_PULL);
	sp5kGpioDrvSet(SP5K_GPIO_GRP_DISPL, SPCA_DISPL_GPIO_MASK, SP5K_GPIO_DISPL_DS0_MIN, SP5K_GPIO_DISPL_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_DISPL, SPCA_DISPL_GPIO_MASK, SPCA_DISPL_GPIO_INIT_VAL);

	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISPH, SPCA_DISPH_GPIO_MASK, SPCA_DISPH_GPIO_CFGIO);
	sp5kGpioPullSet(SP5K_GPIO_GRP_DISPH, SPCA_DISPH_GPIO_MASK, SP5K_GPIO_DISPH_PULL);
	sp5kGpioDrvSet(SP5K_GPIO_GRP_DISPH, SPCA_DISPH_GPIO_MASK, SP5K_GPIO_DISPH_DS0_MIN, SP5K_GPIO_DISPH_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_DISPH, SPCA_DISPH_GPIO_MASK, SPCA_DISPH_GPIO_INIT_VAL);*/

	sp5kGpioCfgSet(SP5K_GPIO_GRP_POCU, SPCA_POCU_GPIO_MASK, SPCA_POCU_GPIO_CFGIO);
	//sp5kGpioPullSet(SP5K_GPIO_GRP_POCU, SPCA_POCU_GPIO_MASK, SP5K_GPIO_POCU_PULL);
	//sp5kGpioDrvSet(SP5K_GPIO_GRP_POCU, SPCA_POCU_GPIO_MASK, SP5K_GPIO_POCU_DS0_MIN, SP5K_GPIO_POCU_DS1_MIN);
	sp5kGpioWrite(SP5K_GPIO_GRP_POCU, SPCA_POCU_GPIO_MASK, SPCA_POCU_GPIO_INIT_VAL);

	LED_RULES_NORMAL;
	POWER_5V_ON;
	WIFI_WAKEUP;
	BATTERY_CHARGE_ON;

	#else

	#ifdef HW_PRJ_6330
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN, SPCA_GEN_GPIO_MASK, SPCA_GEN_GPIO_CFGIO);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH, SPCA_FMH_GPIO_MASK, SPCA_FMH_GPIO_CFGIO);
    sp5kGpioPullSet(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_MASK, SPCA_LMI_GPIO_PULL);
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN, SPCA_GEN_GPIO_MASK, SPCA_GEN_GPIO_INIT_VAL);
    sp5kGpioWrite(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_MASK, SPCA_LMI_GPIO_INIT_VAL);
    sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_MASK, SPCA_LMI_GPIO_CFGIO);
    #else
	#if !ICAT_WIFI
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FML, SPCA_FML_GPIO_MASK, SPCA_FML_GPIO_CFGIO);
	#endif
	#endif
	
	#ifndef _EVB_HW_   /* EVB CAN'T DO THIS!*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,0x00000001L<<10,0x00000001L<<10);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,0x00000001L<<11,0x00000001L<<11);
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,0x00000001L<<10,0x00000001L<<10);
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI,0x00000001L<<11,0x00000001L<<11);
	#endif

	#endif
}

