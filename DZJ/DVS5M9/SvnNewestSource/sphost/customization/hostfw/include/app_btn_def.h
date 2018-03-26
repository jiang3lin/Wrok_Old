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
#ifndef APP_BTN_DEF_H
#define APP_BTN_DEF_H

#include "app_hw_def.h"

/*-- DESCRIPTIONS ------------------------------------------------------
 * The pseudo button ID for enable/disable button function is defined :
 * [31:30:29:28] for GRP mask
 * [27:0] for BTN ID/Mask
 *
 * To distinguish between GPIO and ADC (we dont support Matrix BTN at present),
 * [11:0] are used for GPIO buttons
 * [27:12] are used for ADC buttons
 */

#define BTN_SHIFT_GRP_CODE		28
#if 1
#define BTN_SHIFT_GPIO_MSK		0
#define BTN_SHIFT_ADC_MSK		12
#endif
/*-- Pseudo Button Group
 */
#define BTN_GRP_GPIO		(((UINT32)0x1)<<BTN_SHIFT_GRP_CODE)
#define BTN_GRP_ADC			(((UINT32)0x2)<<BTN_SHIFT_GRP_CODE)
#define BTN_GRP_ALL			(((UINT32)0xF)<<BTN_SHIFT_GRP_CODE)


/*-- PORTING Notes !!
 * The following configuration shall be coincident with the BTN ID
 * programmed in app_key.c, especially for ADC buttons. !!!!
 * Please see the descriptions followed.
 */

/*-- GPIO Buttons
 * Notes : The button ID defined below shall be the same as you programmed
 *         in app_key.c where appGpioBtnInit() does.
 */
typedef enum btnCtrlGpioId_s {
	BTN_ID_TELE				= 0,
	BTN_ID_WIDE,
	BTN_ID_S1,
	BTN_ID_S2,
	BTN_ID_TV,
	BTN_ID_PB,
	BTN_ID_OK,
	BTN_ID_POWER,	
	BTN_ID_GPIO_TOTAL /* <<== checkup mechanism with app_hw_def.h */
} btnCtrlGpioId_t;

typedef enum btnCtrlAdcId_s {
	BTN_ID_UP,
	BTN_ID_DOWN,
	BTN_ID_LEFT,
	BTN_ID_RIGHT,
	BTN_ID_SET,		/* OK */
	BTN_ID_DEL,		/* F1 */
	BTN_ID_MENU,	/* F2 */
	BTN_ID_MODE,	/* F3 */
	BTN_ID_ADC_TOTAL /* <<== checkup mechanism with app_hw_def.h */
} btnCtrlAdcId_t;


#define BTN_TELE	(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_TELE))
#define BTN_WIDE	(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_WIDE))
#define BTN_S1		(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_S1))
#define BTN_S2		(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_S2))
#define BTN_TV		(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_TV))
#define BTN_PB		(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_PB))
#define BTN_OK		(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_OK))
#define BTN_POWER	(BTN_GRP_GPIO + ((UINT32)1 << BTN_ID_POWER))

#define BTN_UP		(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_UP))
#define BTN_DOWN	(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_DOWN))
#define BTN_LEFT	(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_LEFT))
#define BTN_RIGHT	(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_RIGHT))
#define BTN_SET		(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_SET))
#define BTN_DEL		(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_DEL))
#define BTN_MODE	(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_MODE))
#define BTN_MENU	(BTN_GRP_ADC + ((UINT32)1 << BTN_ID_MENU))

#define BTN_ALL		(BTN_GRP_ALL + ((UINT32)0x0FFFFFFF))
#define BTN_4WAY	(BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT)
#define BTN_TW		(BTN_TELE | BTN_WIDE)

void appBtnEnable(UINT32 btnID);
void appBtnDisable(UINT32 btnID);
BOOL appBtnEnableGet(void);
UINT32 adcBtnIoLevelGet(UINT32 btnID);
#endif  /* APP_BTN_DEF_H */


