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
#ifndef APP_LENS_MSHUT_H
#define APP_LENS_MSHUT_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
//#include "api/sp5k_os_api.h"
#include "gpio_custom.h"
//#include "app_hw_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MSH_CLOSE_IRQ_ID	SP5K_GPIO_INT0 //TGGPIO2
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

#if 0
typedef struct mshutCfg_s {
	fpsp5kCallback_t pfuncSnapShutOnCBF;
	fpsp5kCallback_t pfuncSnapShutOffCBF;
} mshutCfg_t;
#endif

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void lensMShutLmiModeSet(UINT32 off);
UINT32 lensMShutOpenActuate(void);
UINT32 lensMShutCloseActuate(void);
void lensMShutIdleActuate(void);
BOOL lensMShutBusyQuery(void);
UINT32 lensMShutOpen(void);
UINT32 lensMShutClose(void);
void lensMShutLMITrig(void);


#endif  /* APP_LENS_MSHUT_H */


