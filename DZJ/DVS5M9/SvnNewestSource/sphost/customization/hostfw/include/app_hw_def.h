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

#ifndef APP_HW_DEF_H
#define APP_HW_DEF_H
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "customization/solution_def.h"
#include "customization/dev_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ENABLE          1
#define DISABLE         0
#define ACTIVE_HIGH     1
#define ACTIVE_LOW      0
#define OUTPUT          1
#define INPUT           0

/* GPIO button realted */
#define GPIO_RPT		1 /* No Repeat key */
#define GPIO_DBNC		10 /* Debounce ~1 * polling timer */
/* ADC button realted */
#define ADC_RPT			15/* No Repeat key */
#define ADC_DBNC		10/* Debounce ~30 * polling timer */

#define ADC_BATTERY_CH 13 /*channel13 only for battery measure*/
#define ADC_BTN_A_CH	1/* channel 1 for 4 way key */
#if SPCA6330
#define ADC_BTN_B_CH	7/* channel 7 for other key */
#else
#define ADC_BTN_B_CH	2/* channel 2 for other key *//*5330 EVB*/
#endif


#if SPCA6330
	#if EVB_TYPE/**/
	/* Channel A  button */
	#define ADC_BTN_A_DEV		50
	#define ADC_BTN_A_S1          750    /* UP */
	#define ADC_BTN_A_S2          560    /* DOWN */
	#define ADC_BTN_A_S3          390    /* LEFT */
	#define ADC_BTN_A_S4          154    /* RIGHT */
	/* Channel B button*/
	#define ADC_BTN_B_DEV		50
	#define ADC_BTN_B_S1          3190    /* S1/SET Key   */
	#define ADC_BTN_B_S2          2360    /* S2/DEL Key    */
	#define ADC_BTN_B_S3          1450    /* S3/MENU Key */
	#define ADC_BTN_B_S4          635    /* S4/MODE Key */
	#else/*for RDK*/
	/* Channel A  button */
	#define ADC_BTN_A_DEV		50
	#define ADC_BTN_A_S1          750    /* UP */
	#define ADC_BTN_A_S2          560    /* DOWN */
	#define ADC_BTN_A_S3          330    /* LEFT */
	#define ADC_BTN_A_S4          144    /* RIGHT */
	/* Channel B button*/
	#define ADC_BTN_B_DEV		100
	#define ADC_BTN_B_S1          2970 		/* OK/SET Key   */
	#define ADC_BTN_B_S2          1352		/*PB KEY*/
	#define ADC_BTN_B_S3          2676    	/*MENU Key */
	#define ADC_BTN_B_S4          635   		 /* S4/MODE Key */
	#endif
#else
	/* Channel A button*/
	#define ADC_BTN_A_DEV		50
	#define ADC_BTN_A_S1          800    /* UP */
	#define ADC_BTN_A_S2          580    /* DOWN */
	#define ADC_BTN_A_S3          345    /* LEFT */
	#define ADC_BTN_A_S4          154    /* RIGHT */
	/* Channel B button*/
	#define ADC_BTN_B_DEV		50
	#define ADC_BTN_B_S1          790    /* SET/MENU Key */
	#define ADC_BTN_B_S2          590    /* F1 /DEL */
	#define ADC_BTN_B_S3          345    /* F2 /MENU */
	#define ADC_BTN_B_S4          154    /* F3 /MODE */
#endif

#define GPIO_BTN_NUM	8  /* S1/S2/T/W/TV/USB/PB */ /*OK/POWER*/
#define ADC_BTN_A_NUM	4
#define ADC_BTN_B_NUM	4
#define ADC_BTN_NUM	(ADC_BTN_A_NUM + ADC_BTN_B_NUM)
#define ADC_BTN_TOTAL	(ADC_BTN_NUM)

/* Audio Setting */
#define AUD_PLAY_SRC_FOR_LCD		SP5K_AUD_DEV_PLAY_SRC_SPEAKER
#define AUD_PLAY_SRC_FOR_TV		SP5K_AUD_DEV_PLAY_SRC_LINEOUT
#define AUD_REC_SRC				(SP5K_AUD_DEV_REC_SRC_MIC_IN | SP5K_AUD_REC_CH_LEFT)

#endif  /* APP_HW_DEF_H */


