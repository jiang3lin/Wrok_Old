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
#ifndef APP_LED_H
#define APP_LED_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 typedef enum {
	APP_LED_POWER  = 0,  
	APP_LED_BUSY_FRONT,
	APP_LED_BUSY_REAR,
	APP_LED_CHARGE,
	APP_LED_TOTAL,
} appLedOption_e;


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appLedIndicatorEnable(BOOL bEnable);
void appLedOn(appLedOption_e led);
void appLedOff(appLedOption_e led);
void appLedFlickerSet(appLedOption_e led,UINT32 flickerMs);
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define POWER_LED_ON     appLedOn(APP_LED_POWER);
#define POWER_LED_OFF   appLedOff(APP_LED_POWER);
#define BUSY_FRONT_LED_ON     appLedOn(APP_LED_BUSY_FRONT);
#define BUSY_FRONT_LED_OFF   appLedOff(APP_LED_BUSY_FRONT);
#define BUSY_REAR_LED_ON     appLedOn(APP_LED_BUSY_REAR);
#define BUSY_REAR_LED_OFF   appLedOff(APP_LED_BUSY_REAR);
#define CHARGE_LED_ON     appLedOn(APP_LED_CHARGE);
#define CHARGE_LED_OFF   appLedOff(APP_LED_CHARGE);

#define ALL_LED_ON      POWER_LED_ON;BUSY_FRONT_LED_ON;BUSY_REAR_LED_ON;CHARGE_LED_ON;
#define ALL_LED_OFF   POWER_LED_OFF;BUSY_FRONT_LED_OFF;BUSY_REAR_LED_OFF;CHARGE_LED_OFF;

#define POWER_LED_FLICKER(a)     appLedFlickerSet(APP_LED_POWER,a);
#define BUSY_FRONT_LED_FLICKER(a)         appLedFlickerSet(APP_LED_BUSY_FRONT,a);
#define BUSY_REAR_LED_FLICKER(a)         appLedFlickerSet(APP_LED_BUSY_REAR,a);
#define CHARGE_LED_FLICKER(a)         appLedFlickerSet(APP_LED_CHARGE,a);

//LED rules
#define LED_RULES_NORMAL						POWER_LED_ON;BUSY_FRONT_LED_OFF;BUSY_REAR_LED_OFF;
#define LED_RULES_REC							POWER_LED_OFF;BUSY_FRONT_LED_FLICKER(500);BUSY_REAR_LED_FLICKER(500);
#define LED_RULES_CAPTURE						POWER_LED_OFF;BUSY_FRONT_LED_ON;BUSY_REAR_LED_ON;
#define LED_RULES_CAPTURE_SELF_TIMER			POWER_LED_OFF;BUSY_FRONT_LED_FLICKER(500);BUSY_REAR_LED_FLICKER(500);
#define LED_RULES_CAPTURE_SELF_TIMER_FAST	POWER_LED_OFF;BUSY_FRONT_LED_FLICKER(200);BUSY_REAR_LED_FLICKER(200);
#define LED_RULES_BUSY							POWER_LED_ON;BUSY_FRONT_LED_FLICKER(250);BUSY_REAR_LED_FLICKER(250);
#define LED_RULES_CHARGE_ON					CHARGE_LED_FLICKER(500);
#define LED_RULES_CHARGE_OFF					CHARGE_LED_OFF;
#define LED_RULES_CARD_ISP						POWER_LED_ON;BUSY_FRONT_LED_OFF;BUSY_REAR_LED_FLICKER(100);




#define GREEN_LED_ON     //appLedOn(APP_LED_POWER);
#define GREEN_LED_OFF   //appLedOff(APP_LED_POWER);
#define RED_LED_ON        // appLedOn(APP_LED_CHARGE);
#define RED_LED_OFF       //appLedOff(APP_LED_CHARGE);

#define GREEN_LED_FLICKER(a)     //appLedFlickerSet(APP_LED_POWER,a);
#define RED_LED_FLICKER(a)         //appLedFlickerSet(APP_LED_CHARGE,a);

//LED rules
#define LED_NORMAL_ON				//GREEN_LED_ON;RED_LED_OFF;
#define LED_FLICKER_POWER_ON 		//GREEN_LED_FLICKER(100)
#define LED_FLICKER_LCD_OFF		//GREEN_LED_FLICKER(200)
#define LED_FLICKER_FOMATE			//GREEN_LED_FLICKER(100)
#define LED_FLICKER_STROBE_CHARGE 	//GREEN_LED_FLICKER(100)
#define LED_FLICKER_AF_ERROR			//GREEN_LED_FLICKER(100)
#define LED_FLICKER_IMAGE_SAVING		//GREEN_LED_FLICKER(100)
#define LED_FLICKER_VIDEO_RECORDING	//GREEN_LED_FLICKER(200)
#define LED_FLICKER_AUDIO_RECORDING	//GREEN_LED_FLICKER(200)
#define LED_FLICKER_DELETING			//GREEN_LED_FLICKER(200)
#define LED_FLICKER_SLIDESHOW_READING	//GREEN_LED_FLICKER(100)
#define LED_FLICKER_COPY2CARD_COPYING	//GREEN_LED_FLICKER(100)
#define LED_FLICKER_VOICE_MEMO_RECORDING		//GREEN_LED_FLICKER(200)
#define LED_FLICKER_USB_TRANSFERRING			//GREEN_LED_FLICKER(40)
#define LED_FLICKER_POWER_OFF 		//GREEN_LED_FLICKER(100)



#endif  /* APP_LED_H*/

