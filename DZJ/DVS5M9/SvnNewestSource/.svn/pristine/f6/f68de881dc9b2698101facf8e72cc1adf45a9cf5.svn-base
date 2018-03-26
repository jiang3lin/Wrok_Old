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

#ifndef GPIO_CUSTOM_EVB_H
#define GPIO_CUSTOM_EVB_H

/*--- Note :
 * Do not include this file directly, include it from gpio_custom.h
 */

#if defined(_EVB_HW_)

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "api/sp5k_global_api.h"
#include "app_btn_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define GPIO_CFG_IN(PinNo)	(0x00000000L << (PinNo))
#define GPIO_CFG_OUT(PinNo)	(0x00000001L << (PinNo))
#define GPIO_MASK(PinNo)	(0x00000001L << (PinNo))

#define GPIO_PIN_VAL_LO(PinNo)	(0x00000000L << (PinNo))
#define GPIO_PIN_VAL_HI(PinNo)	(0x00000001L << (PinNo))
#define GPIO_PIN_PULL_DIS(PinNo)	(0x00000000L << (PinNo))
#define GPIO_PIN_PULL_EN(PinNo)	(0x00000001L << (PinNo))


#define dummyio_set			do { } while(0)
#define dummyio_get			do { } while(0)

extern UINT32 dummyioLcdBL;


/*-------------------------------------------------------------------------
 * SPCA GPIO BIT Mask Definition
 */

#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000


/*-------------------------------------------------------------------------
 * SPCA GPIO Mask Shift Definition
 */
#define PIN_0     0
#define PIN_1     1
#define PIN_2     2
#define PIN_3     3
#define PIN_4     4
#define PIN_5     5
#define PIN_6     6
#define PIN_7     7
#define PIN_8     8
#define PIN_9     9
#define PIN_10    10
#define PIN_11    11
#define PIN_12    12
#define PIN_13    13
#define PIN_14    14
#define PIN_15    15
#define PIN_16    16
#define PIN_17    17
#define PIN_18    18
#define PIN_19    19
#define PIN_20    20
#define PIN_21    21
#define PIN_22    22
#define PIN_23    23
#define PIN_24    24
#define PIN_25    25
#define PIN_26    26
#define PIN_27    27
#define PIN_28    28
#define PIN_29    29
#define PIN_30    30
#define PIN_31    31

#define PIN_32    0
#define PIN_33    1
#define PIN_34    2
#define PIN_35    3
#define PIN_36    4
#define PIN_37    5
#define PIN_38    6
#define PIN_39    7
#define PIN_40    8
#define PIN_41    9
#define PIN_42    10
#define PIN_43    11
#define PIN_44    12
#define PIN_45    13
#define PIN_46    14
#define PIN_47    15
#define PIN_48    16
#define PIN_49    17
#define PIN_50    18
#define PIN_51    19


/*-------------------------------------------------------------------------
 * SPCA GPIO PIN Number Enumeration
 */

#define PIN0		0
#define PIN1		1
#define PIN2		2
#define PIN3		3
#define PIN4		4
#define PIN5		5
#define PIN6		6
#define PIN7		7
#define PIN8		8
#define PIN9		9
#define PIN10		10
#define PIN11		11
#define PIN12		12
#define PIN13		13
#define PIN14		14
#define PIN15		15
#define PIN16		16
#define PIN17		17
#define PIN18		18
#define PIN19		19
#define PIN20		20
#define PIN21		21
#define PIN22		22
#define PIN23		23
#define PIN24		24
#define PIN25		25
#define PIN26		26
#define PIN27		27
#define PIN28		28
#define PIN29		29
#define PIN30		30
#define PIN31		31
#define PIN32		32
#define PIN33		33
#define PIN34		34
#define PIN35		35
#define PIN36		36
#define PIN37		37
#define PIN38		38
#define PIN39		39
#define PIN40		40
#define PIN41		41
#define PIN42		42
#define PIN43		43
#define PIN44		44
#define PIN45		45
#define PIN46		46
#define PIN47		47
#define PIN48		48
#define PIN49		49
#define PIN50		50
#define PIN51		51
#define PIN52		52
#define PIN53		53





#define IN_PWR_BACKLIGHT		1
#define IN_LCD_STANDBY			1

#define OUT_PWR_LCD          	dummyio_set
#define OUT_PWR_BACKLIGHT    	dummyio_set
#define OUT_PWR_BL_CTL			dummyio_set
#define OUT_LCD_RESET			dummyio_set
#define OUT_LCD_STANDBY			dummyio_set


/* TV */
#if SPCA_6330M
#define IN_TV_IN				1
#else
#define IN_TV_IN				0
#endif
	
#define IN_FLASH_READY  		1 // always ready

#define OUT_FLASH_TRIG          dummyio_set
#define OUT_PWR_ZOOM 				dummyio_set
#define OUT_PWR_FOCUS 				dummyio_set

#define OUT_SHUTTER_N 			dummyio_set
#define OUT_SHUTTER_P 			dummyio_set

/* Sensor */
#define PWR_AGC(a)				dummyio_set=a

//#define PWR_CCD(a)					{OUT_PWR_CCD_VH=a; sp5kTimeDelay(10,3); OUT_PWR_CCD_VL=a;} 	// CCD Power Control
#define PWR_CCD_OFF(a)			dummyio_set=a
//#define TG_RESET(a)					{OUT_TG_RESET = a;}

/* Flash */
#define PwrFlashCharge(a)   	dummyio_set=a /* never charge */
#define FLASH_CHARGING      	0 /* never charge */
#define FLASH_TRIG_CNTL(a) 		dummyio_set=a /* never trigger */

#if !ICAT_WIFI
#if SPCA_6330M
#define IS_TV_IN				(!gpioPinLevelGet(SP5K_GPIO_GRP_FML, GPIO_MASK(29)))
#else
#define IS_TV_IN				(!gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(13)))
#endif
#define IS_USB_IN				(sp5kUsbDetectStateGet())
#else
#define IS_TV_IN				0/*(!gpioPinLevelGet(SP5K_GPIO_GRP_FML, GPIO_MASK(29)))*/
#define IS_USB_IN				0/*(sp5kUsbDetectStateGet())*/
#endif
#if (KIT_WITH_HDMI)
#define IS_HDMI_IN				(sp5kDispHdmiHotPlugDetectGet(0))
#define IS_CEC_ON				1
#endif
#define IS_LCD_ON       		IN_LCD_STANDBY  //(IO_PWR_LCD&&IO_PWR_LCD_AVDD)



#define CFG_PWR_BL_CTL		PIN_45
#define CFG_LCD_RESET     PIN_43

#define OUT_TG_VDR				dummyio_set


/*-- FLASH & STROBE */
#define CFG_FLASH_TRIG			PIN_8//PIN_32 //TG grp
#define CFG_FLASH_CHARGE 		PIN_0//PIN_7  //lmi grp
#define CFG_FLASH_READY  		PIN_1//PIN_8  //lmi grp

#define CFG_IRIS_P 	    			PIN_5//PIN_26	//lmi grp position/direction
#define CFG_IRIS_N 	    			PIN_4//PIN_27 	//lmi grp enable

#define CFG_SHUTTER_N 			PIN_6//PIN_23	//lmi grp enable
#define CFG_SHUTTER_P 			PIN_7//PIN_24	//lmi grp position/direction

/*-- Battery Charger */
/*-- Output */
#define CFG_BATCHARGER_SUSPEND		PIN_3//lmi grp Battery Charger Subspend Mode
#define CFG_BATCHARGER_ENn          PIN_12//lmi grp #Battery Charger Enable
#define CFG_BATCHARGER_CurLmtn		PIN_13//lmi grp Battery Charger Current Limiter
#define CFG_BATCHARGER_HalfChgn		PIN_14//lmi grp #Battery Helf Charger Enable
/*-- Input */
#define CFG_BATCHARGER_PGOODn		PIN_2//lmi grp #Battery Charger Power good
#define CFG_BATCHARGER_CHARGINGn	PIN_15//lmi grp #Battery Charging


/*-- BUTTON KEY */
#define CFG_BUTTON_TELE		PIN_11
#define CFG_BUTTON_WIDE		PIN_12

/*-- LED */
#define CFG_MAIN_LED		PIN_0
#define CFG_ST_LED		PIN_1

/* TV IN */
#if SPCA_6330M
#define CFG_TV_IN 		PIN_13
#else
#define CFG_TV_IN 		PIN_29
#endif

/* Note : EVB does not use the following constants.
 * 		  Keep the definition here for format compatibility.
 */
#define	SPCA_TGL_GPIO_CFGIO
#define	SPCA_TGL_GPIO_MASK
#define	SPCA_TGH_GPIO_CFGIO
#define	SPCA_TGH_GPIO_MASK
#define	SPCA_DISP_GPIO_CFGIO
#define	SPCA_DISP_GPIO_MASK

/* GPIO group initial H/L define*/
#define	SPCA_GEN_GPIO_INIT_MASK
#define	SPCA_TGL_GPIO_INIT_VAL
#define	SPCA_TGL_GPIO_INIT_MASK
#define	SPCA_TGH_GPIO_INIT_VAL
#define	SPCA_TGH_GPIO_INIT_MASK
#define	SPCA_DISP_GPIO_INIT_VAL
#define	SPCA_DISP_GPIO_INIT_MASK


/* GENGPIO group initial define*/
#define	SPCA_GEN_GPIO_CFGIO	( \
			GPIO_CFG_OUT(CFG_ST_LED)|\
			 GPIO_CFG_OUT(CFG_MAIN_LED))

#define	SPCA_GEN_GPIO_MASK  ( \
	            	GPIO_MASK(CFG_ST_LED)|\
	            	GPIO_MASK(CFG_MAIN_LED) )

#define	SPCA_GEN_GPIO_INIT_VAL (  \
	            	GPIO_PIN_VAL_HI(CFG_ST_LED)|\
	            	GPIO_PIN_VAL_HI(CFG_MAIN_LED) )


/* FMGPIO group initial define*/
#define	SPCA_FMH_GPIO_CFGIO	( \
			   GPIO_CFG_OUT(CFG_PWR_BL_CTL)|\
			   GPIO_CFG_OUT(CFG_LCD_RESET))

#define	SPCA_FMH_GPIO_MASK  ( \
	            GPIO_MASK(CFG_PWR_BL_CTL)|\
	            GPIO_MASK(CFG_LCD_RESET) )

#define	SPCA_FMH_GPIO_INIT_VAL (  \
				GPIO_PIN_VAL_HI(CFG_LCD_RESET))

/* LMIGPIO group initial define*/
#define	SPCA_LMI_GPIO_CFGIO	( \
			   GPIO_CFG_OUT(CFG_BATCHARGER_SUSPEND)|\
			   GPIO_CFG_OUT(CFG_BATCHARGER_ENn)|\
			   GPIO_CFG_OUT(CFG_BATCHARGER_CurLmtn)|\
			   GPIO_CFG_OUT(CFG_BATCHARGER_HalfChgn)|\
			   GPIO_CFG_IN(CFG_BATCHARGER_PGOODn)|\
			   GPIO_CFG_IN(CFG_BATCHARGER_CHARGINGn))

#define	SPCA_LMI_GPIO_MASK ( \
	            GPIO_MASK(CFG_BATCHARGER_SUSPEND)|\
	            GPIO_MASK(CFG_BATCHARGER_ENn)|\
	            GPIO_MASK(CFG_BATCHARGER_CurLmtn)|\
	            GPIO_MASK(CFG_BATCHARGER_HalfChgn)|\
	            GPIO_MASK(CFG_BATCHARGER_PGOODn)|\
	            GPIO_MASK(CFG_BATCHARGER_CHARGINGn))

#if (COMPEL_CHARGING)
#define	SPCA_LMI_GPIO_INIT_VAL ( \
				GPIO_PIN_VAL_LO(CFG_BATCHARGER_SUSPEND)|\
    			GPIO_PIN_VAL_HI(CFG_BATCHARGER_ENn)|\
    			GPIO_PIN_VAL_HI(CFG_BATCHARGER_CurLmtn)|\
				GPIO_PIN_VAL_HI(CFG_BATCHARGER_HalfChgn))
#else

#define	SPCA_LMI_GPIO_INIT_VAL ( \
				GPIO_PIN_VAL_LO(CFG_BATCHARGER_SUSPEND)|\
    			GPIO_PIN_VAL_HI(CFG_BATCHARGER_ENn)|\
				GPIO_PIN_VAL_HI(CFG_BATCHARGER_CurLmtn)|\
				GPIO_PIN_VAL_HI(CFG_BATCHARGER_HalfChgn))
#endif


#define	SPCA_LMI_GPIO_PULL ( \
                GPIO_PIN_PULL_DIS(CFG_BATCHARGER_SUSPEND)|\
                GPIO_PIN_PULL_DIS(CFG_BATCHARGER_ENn)|\
                GPIO_PIN_PULL_DIS(CFG_BATCHARGER_CurLmtn)|\
                GPIO_PIN_PULL_DIS(CFG_BATCHARGER_HalfChgn)|\
                GPIO_PIN_PULL_DIS(CFG_BATCHARGER_PGOODn)|\
                GPIO_PIN_PULL_EN(CFG_BATCHARGER_CHARGINGn))


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/*-------------------------------------------------------------------------
 * Device IO Controls
 */
#define _SP5K_IO_SET(grp, msk, val)	sp5kGpioWrite(grp, msk, (UINT32)((!!val) ? msk : 0))
#define _SP5K_IO_CFG(grp, msk, dir)	sp5kGpioCfgSet(grp, msk, (UINT32)((!!dir) ? msk : 0))

/*-- LED */
#define IO_LED_MAIN_SET(a) 		 _SP5K_IO_SET(SP5K_GPIO_GRP_GEN, GPIO_MASK(CFG_MAIN_LED), a)
#define LED_MAIN_ON				IO_LED_MAIN_SET(0)
#define LED_MAIN_OFF			IO_LED_MAIN_SET(1)

#define IO_LED_STIMER_SET(a) 	 _SP5K_IO_SET(SP5K_GPIO_GRP_GEN, GPIO_MASK(CFG_ST_LED), a)
#define LED_STIMER_ON			IO_LED_STIMER_SET(0)
#define LED_STIMER_OFF			IO_LED_STIMER_SET(1)

/*-- LCD */
#define IO_LCD_PWR_SET(a) 		dummyio_set	// LCD Power IO Control
#define LCD_PWR_ON				IO_LCD_PWR_SET(1)
#define LCD_PWR_OFF				IO_LCD_PWR_SET(0)

#if 0
#define IO_LCD_BACKLIGHT_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_FMH, GPIO_MASK(45-32), a) // LCD Backlight IO Control
#define LCD_BACKLIGHT_ON		IO_LCD_BACKLIGHT_SET(1)
#define LCD_BACKLIGHT_OFF		IO_LCD_BACKLIGHT_SET(0)
#define IS_BAKCLIGHT_ON              (0)
#else
#define IO_LCD_BACKLIGHT_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_FMH, GPIO_MASK(CFG_PWR_BL_CTL/*-32*/), a)
#define IO_LCD_BACKLIGHT_GET	gpioPinLevelGet(SP5K_GPIO_GRP_FMH, GPIO_MASK(CFG_PWR_BL_CTL/*-32*/))	// LCD Backlight IO Control
#define LCD_BACKLIGHT_ON		IO_LCD_BACKLIGHT_SET(1)
#define LCD_BACKLIGHT_OFF		IO_LCD_BACKLIGHT_SET(0)
#define IS_BACKLIGHT_ON			IO_LCD_BACKLIGHT_GET
#endif

#define IO_LCD_RESET_SET(a)		_SP5K_IO_SET(SP5K_GPIO_GRP_FMH, GPIO_MASK(43-32), a) // LCD Backlight IO Control // LCD RESET IO Control
#define LCD_RESET_ON			IO_LCD_RESET_SET(1)
#define LCD_RESET_OFF			IO_LCD_RESET_SET(0)

#define IO_LCD_STANDBY_SET(a)	{dummyioLcdBL=a;}

#define IO_LCD_STANDBY_GET		dummyioLcdBL
#define LCD_WAKEUP				IO_LCD_STANDBY_SET(1)
#define LCD_STANDBY				IO_LCD_STANDBY_SET(0)
#define IS_LCD_ACTIVE			(IO_LCD_STANDBY_GET)


/*-- SD Card */
#define IO_SD_CARD_WP_GET		gpioPinLevelGet(SP5K_GPIO_GRP_GEN,(((UINT32)0x01)<<SP5K_SD_WRP_ID))
#define IS_CARD_LOCK			(IO_SD_CARD_WP_GET) /* active hi */
#define IO_SD_CARD_DETECT_GET	gpioPinLevelGet(SP5K_SD_DETECT_GRP, (((UINT32)0x01)<<SP5K_SD_DETECT_ID))
#define IS_CARD_EXIST			(!IO_SD_CARD_DETECT_GET) /* active low */
//#define IO_SD_CARD_POWER_GET	0

#define IO_SD_CARD_WP_SET(a)	dummyio_set
#define IO_SD_CARD_WP_ON		IO_SD_CARD_WP_SET(1)
#define IO_SD_CARD_WP_OFF		IO_SD_CARD_WP_SET(0)

#define IO_SD_CARD_POWER_SET(a)		dummyio_set
#define IO_SD_CARD_POWER_ON			IO_SD_CARD_POWER_SET(1)
#define IO_SD_CARD_POWER_OFF		IO_SD_CARD_POWER_SET(0)


/*-- Camera UI Keys
 */
#define IO_KEY_UP_GET			adcBtnIoLevelGet(BTN_UP)
#define IO_KEY_DOWN_GET			adcBtnIoLevelGet(BTN_DOWN)
#define IO_KEY_LEFT_GET			adcBtnIoLevelGet(BTN_LEFT)
#define IO_KEY_RIGHT_GET		adcBtnIoLevelGet(BTN_RIGHT)
#define IO_KEY_OK_GET			adcBtnIoLevelGet(BTN_SET)
#define IO_KEY_MENU_GET			adcBtnIoLevelGet(BTN_MENU)
#define IO_KEY_DEL_GET			adcBtnIoLevelGet(BTN_DEL)
#define IO_KEY_MODE_GET			adcBtnIoLevelGet(BTN_MODE)
#define IO_KEY_TELE_GET				gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(CFG_BUTTON_TELE))
#define IO_KEY_WIDE_GET				gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(CFG_BUTTON_WIDE))
#define IO_KEY_S1_GET				gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(10))
#define IO_KEY_S2_GET				gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(8))

#ifdef EVB_CFG_COUGAR


/* Lens Zoom Home Led power */
#define IO_LENS_LED_ZHM_PWR_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(11), a)
#define IO_LENS_LED_ZHM_PWR_ON		IO_LENS_LED_ZHM_PWR_SET(1)
#define IO_LENS_LED_ZHM_PWR_OFF		IO_LENS_LED_ZHM_PWR_SET(0)

/* Lens Zoom Clk Led power */
#define IO_LENS_LED_ZCLK_PWR_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_TGL, GPIO_MASK(0), a)
#define IO_LENS_LED_ZCLK_PWR_ON		IO_LENS_LED_ZCLK_PWR_SET(1)
#define IO_LENS_LED_ZCLK_PWR_OFF	IO_LENS_LED_ZCLK_PWR_SET(0)

/* Lens Focus Home Led power */
#define IO_LENS_LED_FHM_PWR_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(11), a)
#define IO_LENS_LED_FHM_PWR_ON		IO_LENS_LED_FHM_PWR_SET(1)
#define IO_LENS_LED_FHM_PWR_OFF		IO_LENS_LED_FHM_PWR_SET(0)

/* Lens Zoom Home Led Value */
#define IO_LENS_LED_ZHM_GET			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(2))

/* Lens Zoom Clk Led Value */
#define IO_LENS_LED_ZCLK_GET		gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(13))

/* Lens Focus Home Led Value */
#define IO_LENS_LED_FHM_GET			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(3))

#endif /*-- EVB_CFG_COUGAR */


/*-- Flash
 */

/*s Add by Aries 10/11/05*/
#define IO_SPEAKER_SET(a)        dummyio_set
#define IO_SPEAKER_ON             IO_SPEAKER_SET(1)
#define IO_SPEAKER_OFF            IO_SPEAKER_SET(0)
/*e Add by Aries 10/11/05*/


#define IO_STROBE_TRIG_SET(a)		_SP5K_IO_SET(SP5K_GPIO_GRP_TGL, GPIO_MASK(CFG_FLASH_TRIG), a)
#define IO_STROBE_TRIG_ON			IO_STROBE_TRIG_SET(1)
#define IO_STROBE_TRIG_OFF			IO_STROBE_TRIG_SET(0)
#define IO_STROBE_TRIG_GET			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(CFG_FLASH_TRIG))

#define IO_STROBE_PWR_SET(a)		dummyio_set
#define IO_STROBE_PWR_ON			IO_STROBE_PWR_SET(1)
#define IO_STROBE_PWR_OFF			IO_STROBE_PWR_SET(0)
#define IO_STROBE_PWR_GET			1

#define IO_STROBE_CHARGE_SET(a)		/*dummyio_set*/_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_FLASH_CHARGE), a)
#define IO_STROBE_CHARGE_ON			IO_STROBE_CHARGE_SET(1)
#define IO_STROBE_CHARGE_OFF		IO_STROBE_CHARGE_SET(0)
#define IO_STROBE_CHARGE_GET		/*1*/gpioPinLevelGet(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_FLASH_CHARGE))

#define IO_STROBE_CHARGE_RDY_GET	gpioPinLevelGet(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_FLASH_READY))
#define IS_STROBE_RDY				(!!IO_STROBE_CHARGE_RDY_GET)

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/


#endif /* _EVB_HW_ */

#endif /* GPIO_CUSTOM_EVB_H */

