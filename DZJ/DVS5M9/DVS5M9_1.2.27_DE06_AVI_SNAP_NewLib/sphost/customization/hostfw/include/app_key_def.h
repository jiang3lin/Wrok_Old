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
#ifndef _APP_KEY_DEF_H_
#define _APP_KEY_DEF_H_

#include "api/sp5k_msg_def.h"
#include "app_key_msg.h"
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
typedef enum {
	SP5K_MSGX_KEY		= SP5K_MSG_HEAD_CUSTOMER,	  /* 0xF000-0000 */
	SP5K_MSGX_STATE	= SP5K_MSG_HEAD_CUSTOMER + 0xA000000, /* 0xFA00-0000 */
	SP5K_MSGX_UI		= SP5K_MSG_HEAD_CUSTOMER + 0xB000000, /* 0xFB00-0000 */
} appMsgGroup_e;

typedef enum { 
	KEY_NONE	= 0x0,  
	KEY_RELEASE,  	/* 1 */ 
	KEY_PRESS, 		/* 2 */ 
	KEY_REPEAT,		/* 3 */  
	KEY_LONG_PRESS		/* 4 */  
} appKeyStateGroup_e;

/* define key msg is always from 0xF100-0000 */
#define APP_KEY_MSG	(SP5K_MSGX_KEY|0x01000000)

/* UI mode define */
typedef enum {	
	APP_UI_MODE_VIEW	= 0x00, 
	APP_UI_MODE_VIDEO,
	APP_UI_MODE_AUDIO,
	APP_UI_MODE_PB, 
	APP_UI_MODE_4, 
	APP_UI_MODE_5,
	APP_UI_MODE_6,
	APP_UI_MODE_7,
	APP_UI_MODE_MAX
}appUiMode_e;


/* key group define */
typedef enum {
	KEY_GROUP_S1    	= 0x0100,
	KEY_GROUP_S2    	= 0x0200,
	KEY_GROUP_WIDE	= 0x0300,
	KEY_GROUP_TELE	= 0x0400,
	KEY_GROUP_UP		= 0x0500,
	KEY_GROUP_DOWN	= 0x0600,
	KEY_GROUP_LEFT	= 0x0700,
	KEY_GROUP_RIGHT	= 0x0800,
	KEY_GROUP_MENU	= 0x0900,
	KEY_GROUP_DEL		= 0x0A00,
	KEY_GROUP_SET		= 0x0B00,
	KEY_GROUP_MODE	= 0x0C00,
	KEY_GROUP_PB		= 0x0D00,
	KEY_GROUP_DPS		= 0x0E00,
	KEY_GROUP_FLASH	= 0x0F00,
	KEY_GROUP_DEVICE	= 0xA000,
	KEY_GROUP_MODEDIAL	= 0xB000,
	KEY_GROUP_OK	= 0x1000,
	KEY_GROUP_POWER	= 0x1100,
}appKeyMsgGroup_e;


/* device group define : 0xA0 ~ 0xAF */
typedef enum {	
	KEY_GROUP_TV		= (KEY_GROUP_DEVICE |0x0100), 
	KEY_GROUP_USB		= (KEY_GROUP_DEVICE |0x0200),  
	KEY_GROUP_CARD	= (KEY_GROUP_DEVICE |0x0300), 
	KEY_GROUP_BATTERY= (KEY_GROUP_DEVICE |0x0400), 
	KEY_GROUP_HDMI= (KEY_GROUP_DEVICE |0x0500), 
}appDeviceMsgGroup_e;




/* app key msg define (including TV/USB/CARD) */
typedef enum {
	
	APP_KEY_RELEASE_S1     = (APP_KEY_MSG|KEY_GROUP_S1|KEY_RELEASE), /* 0xF100-0101 */
	APP_KEY_PRESS_S1         = (APP_KEY_MSG|KEY_GROUP_S1|KEY_PRESS), /* 0xF100-0102 */
	
	 
	APP_KEY_RELEASE_S2     = (APP_KEY_MSG|KEY_GROUP_S2|KEY_RELEASE),/* 0xF100-0201 */
	APP_KEY_PRESS_S2         = (APP_KEY_MSG|KEY_GROUP_S2|KEY_PRESS),/* 0xF100-0202 */
	
	
	APP_KEY_RELEASE_WIDE   = (APP_KEY_MSG|KEY_GROUP_WIDE|KEY_RELEASE), /* 0xF100-0301 */
	APP_KEY_PRESS_WIDE       = (APP_KEY_MSG|KEY_GROUP_WIDE|KEY_PRESS), /* 0xF100-0302 */
	
	
	APP_KEY_RELEASE_TELE   = (APP_KEY_MSG|KEY_GROUP_TELE|KEY_RELEASE), /* 0xF100-0401 */
	APP_KEY_PRESS_TELE       = (APP_KEY_MSG|KEY_GROUP_TELE|KEY_PRESS), /* 0xF100-0402 */

	
	APP_KEY_RELEASE_UP     = (APP_KEY_MSG|KEY_GROUP_UP|KEY_RELEASE), /* 0xF100-0501 */
	APP_KEY_PRESS_UP         = (APP_KEY_MSG|KEY_GROUP_UP|KEY_PRESS), /* 0xF100-0502 */
	APP_KEY_REPEAT_UP		 = (APP_KEY_MSG|KEY_GROUP_UP|KEY_REPEAT), /* 0xF100-0503 */
	
	
	APP_KEY_RELEASE_DOWN   = (APP_KEY_MSG|KEY_GROUP_DOWN|KEY_RELEASE), /* 0xF100-0601 */
	APP_KEY_PRESS_DOWN       = (APP_KEY_MSG|KEY_GROUP_DOWN|KEY_PRESS), /* 0xF100-0602*/
	APP_KEY_REPEAT_DOWN		 = (APP_KEY_MSG|KEY_GROUP_DOWN|KEY_REPEAT), /* 0xF100-0603*/
	
	
	APP_KEY_RELEASE_LEFT   = (APP_KEY_MSG|KEY_GROUP_LEFT|KEY_RELEASE), /* 0xF100-0701 */
	APP_KEY_PRESS_LEFT       = (APP_KEY_MSG|KEY_GROUP_LEFT|KEY_PRESS), /* 0xF100-0702 */
	APP_KEY_REPEAT_LEFT       = (APP_KEY_MSG|KEY_GROUP_LEFT|KEY_REPEAT), /* 0xF100-0703 */
	
	 
	APP_KEY_RELEASE_RIGHT  = (APP_KEY_MSG|KEY_GROUP_RIGHT|KEY_RELEASE),/* 0xF100-0801 */
	APP_KEY_PRESS_RIGHT      = (APP_KEY_MSG|KEY_GROUP_RIGHT|KEY_PRESS),/* 0xF100-0802 */
	APP_KEY_REPEAT_RIGHT       = (APP_KEY_MSG|KEY_GROUP_RIGHT|KEY_REPEAT), /* 0xF100-0803 */
	
	
	APP_KEY_RELEASE_MENU   = (APP_KEY_MSG|KEY_GROUP_MENU|KEY_RELEASE), /* 0xF100-0901 */
	APP_KEY_PRESS_MENU       = (APP_KEY_MSG|KEY_GROUP_MENU|KEY_PRESS), /* 0xF100-0902 */
	
	
	APP_KEY_RELEASE_DEL     = (APP_KEY_MSG|KEY_GROUP_DEL|KEY_RELEASE), /* 0xF100-0A01 */
	APP_KEY_PRESS_DEL         = (APP_KEY_MSG|KEY_GROUP_DEL|KEY_PRESS), /* 0xF100-0A02 */
	
	
	APP_KEY_RELEASE_SET     = (APP_KEY_MSG|KEY_GROUP_SET|KEY_RELEASE), /* 0xF100-0B01 */
	APP_KEY_PRESS_SET         = (APP_KEY_MSG|KEY_GROUP_SET|KEY_PRESS), /* 0xF100-0B02 */
	
	
	APP_KEY_RELEASE_MODE     = (APP_KEY_MSG|KEY_GROUP_MODE|KEY_RELEASE), /* 0xF100-0C01 */
	APP_KEY_PRESS_MODE         = (APP_KEY_MSG|KEY_GROUP_MODE|KEY_PRESS), /* 0xF100-0C02 */

	 
	APP_KEY_RELEASE_PB     = (APP_KEY_MSG|KEY_GROUP_PB|KEY_RELEASE),/* 0xF100-0D01 */
	APP_KEY_PRESS_PB         = (APP_KEY_MSG|KEY_GROUP_PB|KEY_PRESS),/* 0xF100-0D02 */

	 
	APP_KEY_RELEASE_DPS     = (APP_KEY_MSG|KEY_GROUP_DPS|KEY_RELEASE), /* 0xF100-0E01 */
	APP_KEY_PRESS_DPS         = (APP_KEY_MSG|KEY_GROUP_DPS|KEY_PRESS), /* 0xF100-0E02 */
	
	
	APP_KEY_RELEASE_FLASH    = (APP_KEY_MSG|KEY_GROUP_FLASH|KEY_RELEASE), /* 0xF100-0E01 */
	APP_KEY_PRESS_FLASH        = (APP_KEY_MSG|KEY_GROUP_FLASH|KEY_PRESS), /* 0xF100-0E02*/
	
	APP_KEY_RELEASE_OK    			= (APP_KEY_MSG|KEY_GROUP_OK|KEY_RELEASE), /* 0xF100-1001 */
	APP_KEY_PRESS_OK        			= (APP_KEY_MSG|KEY_GROUP_OK|KEY_PRESS), /* 0xF100-1002*/
	APP_KEY_REPEAT_OK        		= (APP_KEY_MSG|KEY_GROUP_OK|KEY_REPEAT), /* 0xF100-1003*/
	APP_KEY_LONG_PRESS_OK 		= (APP_KEY_MSG|KEY_GROUP_OK|KEY_LONG_PRESS), /* 0xF100-1004*/
	
	APP_KEY_RELEASE_POWER    		= (APP_KEY_MSG|KEY_GROUP_POWER|KEY_RELEASE), /* 0xF100-1101 */
	APP_KEY_PRESS_POWER        		= (APP_KEY_MSG|KEY_GROUP_POWER|KEY_PRESS), /* 0xF100-1102*/
	APP_KEY_REPEAT_POWER			= (APP_KEY_MSG|KEY_GROUP_POWER|KEY_REPEAT), /* 0xF100-1103*/
	APP_KEY_LONG_PRESS_POWER	= (APP_KEY_MSG|KEY_GROUP_POWER|KEY_LONG_PRESS), /* 0xF100-1104*/
	 
	APP_KEY_TV_OUT       = (APP_KEY_MSG|KEY_GROUP_TV|KEY_RELEASE),/* 0xF100-A101 */
	APP_KEY_TV_IN	        = (APP_KEY_MSG|KEY_GROUP_TV|KEY_PRESS),/* 0xF100-A102 */
	
	APP_KEY_USB_OUT     = (APP_KEY_MSG|KEY_GROUP_USB|KEY_RELEASE), /* 0xF100-A201 */
	APP_KEY_USB_IN	 = (APP_KEY_MSG|KEY_GROUP_USB|KEY_PRESS), /* 0xF100-A202 */
	
	APP_KEY_CARD_OUT   = (APP_KEY_MSG|KEY_GROUP_CARD|KEY_RELEASE), /* 0xF100-A301 */
	APP_KEY_CARD_IN	 = (APP_KEY_MSG|KEY_GROUP_CARD|KEY_PRESS), /* 0xF100-A302 */

	APP_KEY_HDMI_OUT   = (APP_KEY_MSG|KEY_GROUP_HDMI|KEY_RELEASE), /* 0xF100-A501 */
	APP_KEY_HDMI_IN	 = (APP_KEY_MSG|KEY_GROUP_HDMI|KEY_PRESS),  /* 0xF100-A502 */

	APP_KEY_SKIP		  = (APP_KEY_MSG|0xFFFE),   /* 0xF100-FFFE */
	APP_KEY_ERROR		  = (APP_KEY_MSG|0xFFFF),   /* 0xF100-FFFF */

} appKeyMsg_e;


/* app UI mode msg define, could be dial mode or scene mode */
typedef enum {
	/* mode dial */
	APP_KEY_MODE_VIEW	= (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_VIEW),	/*0xF100-B000 */
	APP_KEY_MODE_VIDEO	= (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_VIDEO),  /*0xF100-B001 */
	APP_KEY_MODE_AUDIO	= (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_AUDIO),  /*0xF100-B002 */  
	APP_KEY_MODE_PB = (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_PB),   /*0xF100-B003 */ 
	APP_KEY_MODE5 = (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_4),   /*0xF100-B004 */ 
	APP_KEY_MODE6 = (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_5),   /*0xF100-B005 */ 
	APP_KEY_MODE7 = (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_6),   /*0xF100-B006 */ 
	APP_KEY_MODE8 = (APP_KEY_MSG|KEY_GROUP_MODEDIAL|APP_UI_MODE_7),   /*0xF100-B007 */ 	
} appUiModeMsg_e;


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define KEY_STATE(key)	((key)&0x000000ff)    /* key state and UI mode state */
#define KEY_GROUP(key)	((key)&0x0000ff00)
#define KEY_CODE(key)	((key)&0xffffff00)

#define IS_SP5K_KEY_PRESS_MSG(msg, param)	( (msg)==SP5K_MSG_GPIO_BUTTON_PRESS || \
											  (msg)==SP5K_MSG_ADC_BUTTON_PRESS || \
											  (msg)==SP5K_MSG_MATRIX_GPIO_BUTTON_PRESS || \
											 ((msg)==APP_KEY_MSG && KEY_STATE(param)==KEY_PRESS) )

#define IS_SP5K_BTN_MSG(msg)	( ((msg)&0xfffffff0)==SP5K_MSG_HEAD_BUTTON || \
									(msg)==SP5K_MSG_USB_DEVICE_VBUS_IN || \
									(msg)==SP5K_MSG_USB_DEVICE_VBUS_OFF || \
									(msg)==SP5K_MSG_HDMI_PLUG || \
									(msg)==SP5K_MSG_HDMI_UNPLUG )
									
#define IS_MODE_DIAL_MSG(msg)  ( (KEY_CODE(msg) == (APP_KEY_MSG|KEY_GROUP_MODEDIAL)) )
#define IS_STATE_CTRL_MSG(msg)  ( ((msg)&0xff000000)==SP5K_MSGX_STATE)
#define IS_APP_KEY_MSG(msg)  ( ((msg)&0xff000000)==APP_KEY_MSG)
#define IS_APP_KEY_PRESS_MSG(msg)  ( ((msg)&0xff00000f)==(APP_KEY_MSG|KEY_PRESS) )  /*junqiong.hu@20121218 add for mantis bug 0046196*/
#define IS_APP_KEY_USB_GROUP(msg)	(((msg)&0xff00ff00)==(APP_KEY_MSG |KEY_GROUP_USB) )
#define IS_APP_KEY_CARD_GROUP(msg)	(((msg)&0xff00ff00)==(APP_KEY_MSG |KEY_GROUP_CARD) )
#define IS_BATTERY_MSG(msg)  ( ((msg)&0xff000000)==(APP_KEY_MSG|KEY_GROUP_BATTERY) )

#define IS_PB_KEY_PRESS_MSG(msg)  (msg ==APP_KEY_PRESS_PB)


 /**********************************double check****************/
#define IS_KEY_TELE_PRESSED		((!IO_KEY_TELE_GET) || (!IO_KEY_TELE_GET))
#define IS_KEY_WIDE_PRESSED		((!IO_KEY_WIDE_GET) || (!IO_KEY_WIDE_GET))
#define IS_KEY_UP_PRESSED			(IO_KEY_UP_GET || IO_KEY_UP_GET)
#define IS_KEY_DOWN_PRESSED		(IO_KEY_DOWN_GET || IO_KEY_DOWN_GET)
#define IS_KEY_LEFT_PRESSED		(IO_KEY_LEFT_GET || IO_KEY_LEFT_GET)
#define IS_KEY_RIGHT_PRESSED		(IO_KEY_RIGHT_GET || IO_KEY_RIGHT_GET)
#define IS_KEY_SET_PRESSED		(IO_KEY_SET_GET || IO_KEY_SET_GET)

#define IS_KEY_OK_PRESSED			(IO_KEY_OK_GET || IO_KEY_OK_GET)
#define IS_KEY_POWER_PRESSED		(IO_KEY_POWER_GET || IO_KEY_POWER_GET)


#define PTP_KEY_MSG_PARAM				0xFFFFFFFF
#define KEY_MSG_PARAM_DIRECT_RUN		0xFFFFFFFE	//direct run, ignore lcm power save

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appKeyLookup( char * keyname );
void appBtnControl(UINT32 btnID, 	UINT32 isEnable);

#endif  /* _APP_KEY_DEF_H_ */


