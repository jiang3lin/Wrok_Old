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
#ifndef GPIO_CUSTOM_H
#define GPIO_CUSTOM_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

#include "customization/solution_def.h"
#include "customization/dev_init.h"

#if defined(HW_EVB_6330)
#include "gpio_custom_evb.h"
#elif defined(HW_PRJ_6330)
#include "gpio_custom_prj.h"
#elif defined(HW_DVS5M2)
#include "gpio_custom_dvs5m2.h"
#else
#error Unsupported HW Configured !
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void gpioInitialCfgSet(void);
UINT32 gpioPinLevelGet(UINT32 ioGrp, UINT32 ioMask);

#endif  /* GPIO_CUSTOM_H */


