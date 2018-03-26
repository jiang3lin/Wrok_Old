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
#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define HAL_AUD_GPIO_TOTAL   6
#define HAL_DISP_GPIO_TOTAL  28
#define HAL_FM_GPIO_TOTAL    55
#define HAL_GEN_GPIO_TOTAL   18
#define HAL_TG_GPIO_TOTAL    48
#define HAL_TRAP_GPIO_TOTAL  2

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/* hal_disp_gpio.c */
void   halDispGpioDirSet(UINT32 dir, UINT32 msk);
void   halDispGpioOutSet(UINT32 val, UINT32 msk);
UINT32 halDispGpioInGet(UINT32 msk);

/* hal_fm_gpio.c */
void   halFmGpioADirSet(UINT32 dir, UINT32 msk);
void   halFmGpioBDirSet(UINT32 dir, UINT32 msk);
void   halFmGpioAOutSet(UINT32 val, UINT32 msk);
void   halFmGpioBOutSet(UINT32 val, UINT32 msk);
UINT32 halFmGpioAInGet(UINT32 msk);
UINT32 halFmGpioBInGet(UINT32 msk);

/* hal_gen_gpio.c */
void   halGenGpioDirSet(UINT32 dir, UINT32 msk);
void   halGenGpioOutSet(UINT32 val, UINT32 msk);
UINT32 halGenGpioInGet(UINT32 msk);

/* hal_tg_gpio.c */
void   halTgGpioFuncSel(UINT32 func, UINT32 msk);
void   halTgGpioDirSet(UINT32 dir, UINT32 msk);
void   halTgGpioPolarSet(UINT32 polar, UINT32 msk);
void   halTgGpioOutSet(UINT32 val, UINT32 msk);
UINT32 halTgGpioInGet(UINT32 msk);
void   halTgGpioInPolarSet(UINT32 polar, UINT32 msk);
void   halTgCtrDirSet(UINT32 dir, UINT32 msk);
void   halTgCtrPolarSet(UINT32 polar, UINT32 msk);
UINT32 halTgCtrInGet(UINT32 msk);
void   halTgCtrInPolarSet(UINT32 polar, UINT32 msk);

/* hal_lmi_gpio.c */
void   halLmiGpioDirSet(UINT32 dir, UINT32 msk);
void   halLmiGpioOutSet(UINT32 val, UINT32 msk);
UINT32 halLmiGpioInGet(UINT32 msk);

/* hal_gpio_irq.c */
UINT32 halGpioIrqReg(UINT32 id, UINT32 level, irqIsr_t  pisr);
UINT32 halGpioIrqEnable(UINT32 id, UINT32 en);

#endif  /* _HAL_GPIO_H_ */
