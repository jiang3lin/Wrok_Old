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
#ifndef APP_LENS_DCU10_H
#define APP_LENS_DCU10_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "gpio_custom.h"


#ifdef LENS_MODEL_DCU10

/**************************************************************************
 *                          C O M M O N                                   *
 **************************************************************************/
#define LENS_ZOOM_START_BOOST
		/*-- special control for DCU10 to raise motor driving at the
		 * beginning of ZOOM, and reduce to normal driving after
		 * designated steps.
		 */

#define ZOOM_HOME_LED_SP5K_IRQ_ID		SP5K_GPIO_INT0
		/*-- specify the GPIO IRQ ID for ZHM LED Out */

/**************************************************************************
 *                     Z O O M   S E C T I O N                            *
 **************************************************************************/
/*----------
 * ZOOM Lens Position
 */
#define ZOOM_HOME_POS			((TYPE_ZOOM_POS)114)
#define ZOOM_TELE_POS			((TYPE_ZOOM_POS)(ZOOM_HOME_POS+1799))
#define ZOOM_WIDE_POS			((TYPE_ZOOM_POS)(ZOOM_HOME_POS+861))
#define ZOOM_CLOSE_POS			((TYPE_ZOOM_POS)(ZOOM_HOME_POS-109))
#define ZOOM_UNDETERMINED_POS	((TYPE_ZOOM_POS)(ZOOM_HOME_POS+3500))



/**************************************************************************
 *                       F O C U S   S E C T I O N                        *
 **************************************************************************/
#define FOCUS_BACKLASH_CNT				10

#define LENS_FOCUS_HOME_FHM_LED_LEVEL_HI	1
	/* specify the IO level when focus lens is in home side */

/*--
 * Focus Position
 */
#define FOCUS_HP_POS			(40)
#define FOCUS_CLOSE_POS			(FOCUS_HP_POS-40)
#define FOCUS_PARK_POS			(FOCUS_CLOSE_POS-30)
#define FOCUS_BOOT_POS			(316)

#define FOCUS_MOVE_STEPS_MAX	(1000)

#if !defined(LENS_FOCUS_HOME_FHM_LED_LEVEL_HI)
#error "LENS_FOCUS_HOME_FHM_LED_LEVEL_HI is not defined !!"
#endif
#if LENS_FOCUS_HOME_FHM_LED_LEVEL_HI
#define IS_LENS_FOCUS_HOME		IO_LENS_LED_FHM_GET
#else
#define IS_LENS_FOCUS_HOME		(!IO_LENS_LED_FHM_GET)
#endif


/**************************************************************************
 *                       I R I S     S E C T I O N                        *
 **************************************************************************/


/**************************************************************************
 *                       S H U T T E R   S E C T I O N                    *
 **************************************************************************/

#endif /* LENS_MODEL_DCU10 */

#endif  /* APP_LENS_DCU10_H */


