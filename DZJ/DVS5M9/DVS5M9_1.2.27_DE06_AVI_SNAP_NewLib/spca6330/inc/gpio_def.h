/**************************************************************************
 *
 *        Copyright (c) 2005-2008 by Sunplus mMedia Inc., Ltd.
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
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/
/**
 * \file   gpio_def.h
 * \brief  opened macros for GPIO port and pin direct access
 * \author Anmin Deng
 * \since  2006-6-27
 *   macros to direct access TGGPIO port and pin for opened sensor model
 * \date   2006-10-20
 *   porting 5000 reg to 5110
 * \date   2007-10-5 
 *   macros to direct access all GPIO port and pin
 * \date   2008-01-27
 *   treating POCU pwronint pins as GPIO
 *
 * coding style for generating more efficient MIPS instructions
 * than direct address coding
 * to direct access GPIO port and pin
 */
#ifndef _GPIO_DEF_H_
#define _GPIO_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#ifndef LOGI_ADDR_REG
#define LOGI_ADDR_REG  ((void *)0xb0000000)
#endif

#define _CAT(t,n) t##n


/**
 * \typedef  _gpioPin##_t
 * \brief  GPIO pin bit field type definitions----*/

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1;
} _gpioPin8_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1;
} _gpioPin16_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,_pin_22: 1,_pin_23: 1,
	_pin_24: 1,_pin_25: 1,_pin_26: 1,_pin_27: 1,
	_pin_28: 1,_pin_29: 1,_pin_30: 1,_pin_31: 1;
} _gpioPin32_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,_pin_22: 1,_pin_23: 1,
	_pin_24: 1,_pin_25: 1,_pin_26: 1,_pin_27: 1,
	_pin_28: 1,_pin_29: 1,_pin_30: 1,_pin_31: 1,
	_pin_32: 1,_pin_33: 1,_pin_34: 1,_pin_35: 1,
	_pin_36: 1,_pin_37: 1,_pin_38: 1,_pin_39: 1,
	_pin_40: 1,_pin_41: 1,_pin_42: 1,_pin_43: 1,
	_pin_44: 1,_pin_45: 1,_pin_46: 1,_pin_47: 1;
} _gpioPin48_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,_pin_22: 1,_pin_23: 1,
	_pin_24: 1,_pin_25: 1,_pin_26: 1,_pin_27: 1,
	_pin_28: 1,_pin_29: 1,_pin_30: 1,_pin_31: 1,
	_pin_32: 1,_pin_33: 1,_pin_34: 1,_pin_35: 1,
	_pin_36: 1,_pin_37: 1,_pin_38: 1,_pin_39: 1,
	_pin_40: 1,_pin_41: 1,_pin_42: 1,_pin_43: 1,
	_pin_44: 1,_pin_45: 1,_pin_46: 1,_pin_47: 1,
	_pin_48: 1,_pin_49: 1,_pin_50: 1,_pin_51: 1;
} _gpioPin52_t, _gpioPin_t;

typedef struct {
	UINT8  :8;
	UINT8  :8;
	UINT8  :8;
	UINT8  :8;
	UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1;
} _gpioPinH_t;

#if SPCA5330
typedef struct { UINT8
	_rsv_0 :1, _rsv_1 :1, _rsv_2 :1, _rsv_3 :1,
	_pin_0 :1, _pin_1 :1, _pin_2 :1, _pin_3 :1;
} _gpioPinMsb4_t;

typedef struct { UINT8
	_pin_4 :1, _pin_5 :1, _pin_6 :1, _pin_7 :1,
	_rsv_0 :1, _rsv_1 :1, _rsv_2 :1, _rsv_3 :1;
} _gpioPinLsb4_t;
#endif

/**
 * \typedef  _xxxGpio_t
 * \brief  GPIO register opened structure definitions
 */

#if SPCA5110
/**< ------GENGPIO0:15 (16:17 dedicated to uart)------*/
/** GPIO pwmsel[0:5] correspoding bit 1:gpio[0:5] is pwm[0:5];
 * GPIO16:17 dedicated to uart rx:tx
 * pwmsel[6] 1:gpio[16:17] is uart rx:tx
 * global [0x00b0] audsel[0:4] corresponding bit 1:gpio[8,10:13] is audio*/
typedef struct {
	UINT8 offset[0x00b0];
	volatile union { UINT8  map; _gpioPin8_t  pin; } audsel;
	UINT8 rsv00b1[0x1030-0x00b1];
	volatile union { UINT16 map; _gpioPin16_t pin; } ov;
	UINT8 rsv1032[2];
	volatile union { UINT16 map; _gpioPin16_t pin; } oe;
	UINT8 rsv1036[2];
	volatile union { UINT16 map; _gpioPin16_t pin; } iv;
	UINT8 rsv103a[2];
	volatile union { UINT8  map; _gpioPin8_t  pin; } pwmsel;
} _genGpio_t;

/**< ------FMGPIO0:51---------------------------------*/
typedef struct {
	UINT8 offset[0x4030];
	volatile union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} ov;
	volatile union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} oe;
	volatile union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} iv;
	volatile union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} sel;
} _fmGpio_t;

/**< ------TGGPIO0:15---------------------------------*/
typedef struct {
	UINT8 offset[0x9030];
	volatile union { UINT16 map; _gpioPin16_t pin; } oe;
	volatile union { UINT16 map; _gpioPin16_t pin; } ov;
	volatile union { UINT16 map; _gpioPin16_t pin; } sel;
	volatile union { UINT16 map; _gpioPin16_t pin; } opol;
	volatile union { UINT16 map; _gpioPin16_t pin; } iv;
	volatile union { UINT16 map; _gpioPin16_t pin; } ipol;
} _tgGpio_t;

/**< ------LMIGPIO0:11--------------------------------*/
typedef struct {
	UINT8 offset[0x9044];
	volatile union { UINT16 map; _gpioPin16_t pin; } oe;
	volatile union { UINT16 map; _gpioPin16_t pin; } ov;
	volatile union { UINT16 map; _gpioPin16_t pin; } sel;
	UINT8 rsv104a[2];
	volatile union { UINT16 map; _gpioPin16_t pin; } iv;
} _lmiGpio_t;

/**< ------DISPGPIO0:17-------------------------------*/
typedef struct {
	UINT8 offset[0xa108];
	volatile union { UINT32 map; _gpioPin32_t pin; } sel;
	volatile union { UINT32 map; _gpioPin32_t pin; } iv;
	volatile union { UINT32 map; _gpioPin32_t pin; } ov;
	volatile union { UINT32 map; _gpioPin32_t pin; } oe;
} _dispGpio_t;

/**< ------POCUGPIO0:2--------------------------------*/
typedef struct {
	UINT8 offset[0x00c1];
	volatile union { UINT8  map; _gpioPin8_t  pin; } iv;
} _pocuGpio_t;

#else /*SPCA5000*/

/**< ------GENGPIO0:15 (16:17 dedicated to uart)------*/
/** GPIO pwmsel[0:7] correspoding bit 1:gpio[0:7] is pwm[0:7];
 * GPIO16:17 dedicated to uart rx:tx
 * audsel[0:5] corresponding bit 1:gpio[8:13] is audio func*/
typedef struct {
	UINT8 offset[0x10e2];
	volatile union { UINT8  map; _gpioPin8_t  pin; } audsel;
	UINT8 rsv10e3[0x1100-0x10e3];
	volatile union { UINT16 map; _gpioPin16_t pin; } ov;
	UINT8 rsv1102[6];
	volatile union { UINT16 map; _gpioPin16_t pin; } oe;
	UINT8 rsv110a[6];
	volatile union { UINT16 map; _gpioPin16_t pin; } iv;
	UINT8 rsv1112[0x1184-0x1112];
	volatile union { UINT8  map; _gpioPin8_t  pin; } pwmsel;
} _genGpio_t;

/**< ------FMGPIO0:51---------------------------------*/
/** \note FM register [0x401f] used as other fm functions and occupies
 * the same address as the MSB of FMGPIO iv.map[1] in this struct */
typedef struct {
	UINT8 offset[0x4008];
	volatile union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} ov;
	volatile union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} oe;
	volatile union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} iv;
} _fmGpio_t;

/**< ------TGGPIO0:47---------------------------------*/
typedef struct {
	UINT8 offset[0x9160];
	volatile union {UINT32 map[2];_gpioPin48_t pin;_gpioPinH_t hpin;} sel;
	volatile union {UINT32 map[2];_gpioPin48_t pin;_gpioPinH_t hpin;} oe;
	volatile union {UINT32 map[2];_gpioPin48_t pin;_gpioPinH_t hpin;} ov;
	volatile union {UINT32 map[2];_gpioPin48_t pin;_gpioPinH_t hpin;} iv;
	UINT8 rsv9180[0x20];
	volatile union {UINT32 map[2];_gpioPin48_t pin;_gpioPinH_t hpin;} opol;
} _tgGpio_t;

/**< ------DISPGPIO0:27-------------------------------*/
/** \note disp gpio register address is not properly aligned,
 * we are forced to use byte access */
typedef struct {
	UINT8 offset[0xa0a1];
	volatile union { UINT8 map[4]; _gpioPin32_t pin; } ov;
	volatile union { UINT8 map[4]; _gpioPin32_t pin; } oe;
	UINT8 rsva0a9[0x20];
	volatile union { UINT8 map[4]; _gpioPin32_t pin; } iv;
} _dispGpio_t;

#endif

#if SPCA5330
/**< ------SARGPIO4:7---------------------------------*/
typedef struct {
	UINT8 offset[0x64b0];
	volatile union {UINT8 map; _gpioPinLsb4_t pin;} ie;
	volatile union {UINT8 map; _gpioPinLsb4_t pin;} pe;
	volatile union {UINT8 map; _gpioPinLsb4_t pin;} ov;
	volatile union {UINT8 map; _gpioPinLsb4_t pin;} oe;
	UINT8 rsv64bc[0x08];
	volatile union {UINT8 map; _gpioPinLsb4_t pin;} iv;
} _sarGpio_t;

/**< ------OISGPIO0:3---------------------------------*/
typedef struct {
	UINT8 offset[0x64b0];
	volatile union {UINT8 map; _gpioPinMsb4_t pin;} ie;
	volatile union {UINT8 map; _gpioPinMsb4_t pin;} pe;
	volatile union {UINT8 map; _gpioPinMsb4_t pin;} ov;
	volatile union {UINT8 map; _gpioPinMsb4_t pin;} oe;
	UINT8 rsv64bc[0x08];
	volatile union {UINT8 map; _gpioPinMsb4_t pin;} iv;
} _oisGpio_t;
#endif

/**
 * \def  _xxxGpioPinXXX
 * \def  _xxxGpioMapXXX
 * \brief  GPIO direct access port macros----------*/

/**< ------GENGPIO------------------------------------*/
#define _genGpioPinAS  (((_genGpio_t *)LOGI_ADDR_REG)->audsel.pin)
#define _genGpioPinPS  (((_genGpio_t *)LOGI_ADDR_REG)->pwmsel.pin)
#define _genGpioPinD   (((_genGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _genGpioPinO   (((_genGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _genGpioPinI   (((_genGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _genGpioMapAS  (((_genGpio_t *)LOGI_ADDR_REG)->audsel.map)
#define _genGpioMapPS  (((_genGpio_t *)LOGI_ADDR_REG)->pwmsel.map)
#define _genGpioMapD   (((_genGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _genGpioMapO   (((_genGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _genGpioMapI   (((_genGpio_t *)LOGI_ADDR_REG)->iv.map)

/**< ------FMGPIO-------------------------------------*/
#define _fmGpioPinD    (((_fmGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _fmGpioPinO    (((_fmGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _fmGpioPinI    (((_fmGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _fmGpioHPinD   (((_fmGpio_t *)LOGI_ADDR_REG)->oe.hpin)
#define _fmGpioHPinO   (((_fmGpio_t *)LOGI_ADDR_REG)->ov.hpin)
#define _fmGpioHPinI   (((_fmGpio_t *)LOGI_ADDR_REG)->iv.hpin)
#define _fmGpioLMapD   (((_fmGpio_t *)LOGI_ADDR_REG)->oe.map[0])
#define _fmGpioLMapO   (((_fmGpio_t *)LOGI_ADDR_REG)->ov.map[0])
#define _fmGpioLMapI   (((_fmGpio_t *)LOGI_ADDR_REG)->iv.map[0])
#define _fmGpioHMapD   (((_fmGpio_t *)LOGI_ADDR_REG)->oe.map[1])
#define _fmGpioHMapO   (((_fmGpio_t *)LOGI_ADDR_REG)->ov.map[1])
#if SPCA5110
#define _fmGpioHMapI   (((_fmGpio_t *)LOGI_ADDR_REG)->iv.map[1])
#define _fmGpioPinS    (((_fmGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _fmGpioHPinS   (((_fmGpio_t *)LOGI_ADDR_REG)->sel.hpin)
#define _fmGpioLMapS   (((_fmGpio_t *)LOGI_ADDR_REG)->sel.map[0])
#define _fmGpioHMapS   (((_fmGpio_t *)LOGI_ADDR_REG)->sel.map[1])

#else /*SPCA5000*/
/** \note FM register [0x401f] used as other fm functions and occupies
 * the same address as the MSB of FMGPIO iv.map[1] in this struct */
#define _fmGpioHMapI   (((_fmGpio_t *)LOGI_ADDR_REG)->iv.map[1] & 0x0fffff)

#endif

/**< ------TGGPIO-------------------------------------*/
#define _tgGpioPinP    (((_tgGpio_t *)LOGI_ADDR_REG)->opol.pin)
#define _tgGpioPinS    (((_tgGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _tgGpioPinD    (((_tgGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _tgGpioPinO    (((_tgGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _tgGpioPinI    (((_tgGpio_t *)LOGI_ADDR_REG)->iv.pin)

#if SPCA5110
#define _tgGpioPinIP   (((_tgGpio_t *)LOGI_ADDR_REG)->ipol.pin)
#define _tgGpioMapP    (((_tgGpio_t *)LOGI_ADDR_REG)->opol.map)
#define _tgGpioMapS    (((_tgGpio_t *)LOGI_ADDR_REG)->sel.map)
#define _tgGpioMapD    (((_tgGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _tgGpioMapO    (((_tgGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _tgGpioMapI    (((_tgGpio_t *)LOGI_ADDR_REG)->iv.map)
#define _tgGpioMapIP   (((_tgGpio_t *)LOGI_ADDR_REG)->ipol.map)

#else /*SPCA5000*/
#define _tgGpioHPinP   (((_tgGpio_t *)LOGI_ADDR_REG)->opol.hpin)
#define _tgGpioHPinS   (((_tgGpio_t *)LOGI_ADDR_REG)->sel.hpin)
#define _tgGpioHPinD   (((_tgGpio_t *)LOGI_ADDR_REG)->oe.hpin)
#define _tgGpioHPinO   (((_tgGpio_t *)LOGI_ADDR_REG)->ov.hpin)
#define _tgGpioHPinI   (((_tgGpio_t *)LOGI_ADDR_REG)->iv.hpin)
#define _tgGpioLMapP   (((_tgGpio_t *)LOGI_ADDR_REG)->opol.map[0])
#define _tgGpioLMapS   (((_tgGpio_t *)LOGI_ADDR_REG)->sel.map[0])
#define _tgGpioLMapD   (((_tgGpio_t *)LOGI_ADDR_REG)->oe.map[0])
#define _tgGpioLMapO   (((_tgGpio_t *)LOGI_ADDR_REG)->ov.map[0])
#define _tgGpioLMapI   (((_tgGpio_t *)LOGI_ADDR_REG)->iv.map[0])
#define _tgGpioHMapP   (((_tgGpio_t *)LOGI_ADDR_REG)->opol.map[1])
#define _tgGpioHMapS   (((_tgGpio_t *)LOGI_ADDR_REG)->sel.map[1])
#define _tgGpioHMapD   (((_tgGpio_t *)LOGI_ADDR_REG)->oe.map[1])
#define _tgGpioHMapO   (((_tgGpio_t *)LOGI_ADDR_REG)->ov.map[1])
#define _tgGpioHMapI   (((_tgGpio_t *)LOGI_ADDR_REG)->iv.map[1])

#endif

/**< ------LMIGPIO------------------------------------*/
#if SPCA5110
#define _lmiGpioPinS   (((_lmiGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _lmiGpioPinD   (((_lmiGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _lmiGpioPinO   (((_lmiGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _lmiGpioPinI   (((_lmiGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _lmiGpioMapS   (((_lmiGpio_t *)LOGI_ADDR_REG)->sel.map)
#define _lmiGpioMapD   (((_lmiGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _lmiGpioMapO   (((_lmiGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _lmiGpioMapI   (((_lmiGpio_t *)LOGI_ADDR_REG)->iv.map)
#endif

/**< ------DISPGPIO-----------------------------------*/
#define _dispGpioPinD  (((_dispGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _dispGpioPinO  (((_dispGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _dispGpioPinI  (((_dispGpio_t *)LOGI_ADDR_REG)->iv.pin)

#if SPCA5110
#define _dispGpioPinS  (((_dispGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _dispGpioMapS  (((_dispGpio_t *)LOGI_ADDR_REG)->sel.map)
#define _dispGpioMapD  (((_dispGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _dispGpioMapO  (((_dispGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _dispGpioMapI  (((_dispGpio_t *)LOGI_ADDR_REG)->iv.map)

#else /*SPCA5000*/
/** \note disp gpio register address is not properly aligned,
 * we are forced to use byte access */
#define _dispGpioAMapD (((_dispGpio_t *)LOGI_ADDR_REG)->oe.map[0])
#define _dispGpioAMapO (((_dispGpio_t *)LOGI_ADDR_REG)->ov.map[0])
#define _dispGpioAMapI (((_dispGpio_t *)LOGI_ADDR_REG)->iv.map[0])
#define _dispGpioBMapD (((_dispGpio_t *)LOGI_ADDR_REG)->oe.map[1])
#define _dispGpioBMapO (((_dispGpio_t *)LOGI_ADDR_REG)->ov.map[1])
#define _dispGpioBMapI (((_dispGpio_t *)LOGI_ADDR_REG)->iv.map[1])
#define _dispGpioCMapD (((_dispGpio_t *)LOGI_ADDR_REG)->oe.map[2])
#define _dispGpioCMapO (((_dispGpio_t *)LOGI_ADDR_REG)->ov.map[2])
#define _dispGpioCMapI (((_dispGpio_t *)LOGI_ADDR_REG)->iv.map[2])
#define _dispGpioDMapD (((_dispGpio_t *)LOGI_ADDR_REG)->oe.map[3])
#define _dispGpioDMapO (((_dispGpio_t *)LOGI_ADDR_REG)->ov.map[3])
#define _dispGpioDMapI (((_dispGpio_t *)LOGI_ADDR_REG)->iv.map[3])

#endif

/**< ------POCUGPIO-----------------------------------*/
#if SPCA5110
#define _pocuGpioPinI  (((_pocuGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _pocuGpioMapI  (((_pocuGpio_t *)LOGI_ADDR_REG)->iv.map)
#endif

#if SPCA5330
/**< ------SARGPIO------------------------------------*/
#define _sarGpioPinR   (((_sarGpio_t *)LOGI_ADDR_REG)->ie.pin)
#define _sarGpioPinD   (((_sarGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _sarGpioPinO   (((_sarGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _sarGpioPinI   (((_sarGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _sarGpioMapR   (((_sarGpio_t *)LOGI_ADDR_REG)->ie.map)
#define _sarGpioMapD   (((_sarGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _sarGpioMapO   (((_sarGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _sarGpioMapI   (((_sarGpio_t *)LOGI_ADDR_REG)->iv.map)

/**< ------OISGPIO------------------------------------*/
#define _oisGpioPinR   (((_oisGpio_t *)LOGI_ADDR_REG)->ie.pin)
#define _oisGpioPinD   (((_oisGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _oisGpioPinO   (((_oisGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _oisGpioPinI   (((_oisGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _oisGpioMapR   (((_oisGpio_t *)LOGI_ADDR_REG)->ie.map)
#define _oisGpioMapD   (((_oisGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _oisGpioMapO   (((_oisGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _oisGpioMapI   (((_oisGpio_t *)LOGI_ADDR_REG)->iv.map)
#endif

/**
 * \def  XXXGPIO_PIN_XXX(pin)
 * \brief  GPIO direct access pin macros-----------
 *
 * \param pin  must be a decimal literal number to indicate pin index.
 * \note xxGPIOH_PIN_xxx(10...) is equivalent to xxGPIO_PIN_xxx(42...)
 * user may choose to use xxGPIO_PIN_xxx(42...) or xxGPIOH_PIN_xxx(10...) */

/**< ------GENGPIO------------------------------------*/
#define _GENGPIO_PIN_ASEL(n)  _CAT(_genGpioPinAS._pin_,n)
#define _GENGPIO_PIN_PSEL(n)  _CAT(_genGpioPinPS._pin_,n)
#define _GENGPIO_PIN_DIR(n)   _CAT(_genGpioPinD._pin_,n)
#define _GENGPIO_PIN_OUT(n)   _CAT(_genGpioPinO._pin_,n)
#define _GENGPIO_PIN_IN(n)    _CAT(_genGpioPinI._pin_,n)

/**< ------FMGPIO-------------------------------------*/
#define _FMGPIO_PIN_DIR(n)    _CAT(_fmGpioPinD._pin_,n)
#define _FMGPIO_PIN_OUT(n)    _CAT(_fmGpioPinO._pin_,n)
#define _FMGPIO_PIN_IN(n)     _CAT(_fmGpioPinI._pin_,n)
#define _FMGPIOH_PIN_DIR(n)   _CAT(_fmGpioHPinD._pin_,n)
#define _FMGPIOH_PIN_OUT(n)   _CAT(_fmGpioHPinO._pin_,n)
#define _FMGPIOH_PIN_IN(n)    _CAT(_fmGpioHPinI._pin_,n)
#if SPCA5110
#define _FMGPIO_PIN_SEL(n)    _CAT(_fmGpioPinS._pin_,n)
#define _FMGPIOH_PIN_SEL(n)   _CAT(_fmGpioHPinS._pin_,n)
#endif

/**< ------TGGPIO-------------------------------------*/
#define _TGGPIO_PIN_POL(n)    _CAT(_tgGpioPinP._pin_,n)
#define _TGGPIO_PIN_SEL(n)    _CAT(_tgGpioPinS._pin_,n)
#define _TGGPIO_PIN_DIR(n)    _CAT(_tgGpioPinD._pin_,n)
#define _TGGPIO_PIN_OUT(n)    _CAT(_tgGpioPinO._pin_,n)
#define _TGGPIO_PIN_IN(n)     _CAT(_tgGpioPinI._pin_,n)
#if SPCA5110
#define _TGGPIO_PIN_IPOL(n)   _CAT(_tgGpioPinIP._pin_,n)
#else
#define _TGGPIOH_PIN_POL(n)   _CAT(_tgGpioHPinP._pin_,n)
#define _TGGPIOH_PIN_SEL(n)   _CAT(_tgGpioHPinS._pin_,n)
#define _TGGPIOH_PIN_DIR(n)   _CAT(_tgGpioHPinD._pin_,n)
#define _TGGPIOH_PIN_OUT(n)   _CAT(_tgGpioHPinO._pin_,n)
#define _TGGPIOH_PIN_IN(n)    _CAT(_tgGpioHPinI._pin_,n)
#endif

/**< ------LMIGPIO------------------------------------*/
#if SPCA5110
#define _LMIGPIO_PIN_SEL(n)   _CAT(_lmiGpioPinS._pin_,n)
#define _LMIGPIO_PIN_DIR(n)   _CAT(_lmiGpioPinD._pin_,n)
#define _LMIGPIO_PIN_OUT(n)   _CAT(_lmiGpioPinO._pin_,n)
#define _LMIGPIO_PIN_IN(n)    _CAT(_lmiGpioPinI._pin_,n)
#endif

/**< ------DISPGPIO-----------------------------------*/
#define _DISPGPIO_PIN_DIR(n)  _CAT(_dispGpioPinD._pin_,n)
#define _DISPGPIO_PIN_OUT(n)  _CAT(_dispGpioPinO._pin_,n)
#define _DISPGPIO_PIN_IN(n)   _CAT(_dispGpioPinI._pin_,n)
#if SPCA5110
#define _DISPGPIO_PIN_SEL(n)  _CAT(_dispGpioPinS._pin_,n)
#endif

/**< ------POCUGPIO-----------------------------------*/
#if SPCA5110
#define _POCUGPIO_PIN_IN(n)   _CAT(_pocuGpioPinI._pin_,n)
#endif

#if SPCA5330
/**< ------SARGPIO------------------------------------*/
#define _SARGPIO_PIN_IEN(n)   _CAT(_sarGpioPinR._pin_,n)
#define _SARGPIO_PIN_OEN(n)   _CAT(_sarGpioPinD._pin_,n)
#define _SARGPIO_PIN_OUT(n)   _CAT(_sarGpioPinO._pin_,n)
#define _SARGPIO_PIN_IN(n)    _CAT(_sarGpioPinI._pin_,n)

/**< ------OISGPIO------------------------------------*/
#define _OISGPIO_PIN_IEN(n)   _CAT(_oisGpioPinR._pin_,n)
#define _OISGPIO_PIN_OEN(n)   _CAT(_oisGpioPinD._pin_,n)
#define _OISGPIO_PIN_OUT(n)   _CAT(_oisGpioPinO._pin_,n)
#define _OISGPIO_PIN_IN(n)    _CAT(_oisGpioPinI._pin_,n)
#endif

/**
 * \def  XXXGPIO_PIN_XXX(pin,...)
 * \def  XXXGPIO_MASK_XXX(mask,...)
 * \brief  GPIO config macros----------------------
 *
 * \param pin  must be a decimal literal number to indicate pin index.
 * \param mask 1 bits: corresponding bits are GPIO pins.
 * \param dir  0:GPIO in to sp5k, 1:GPIO out from sp5k.
 *
 * \note any pin config with these macros are used as GPIO.
 * function pins should config in related function modules,
 * not config with these macros. */

/**< ------GENGPIO------------------------------------*/
#define GENGPIO_PIN_WRITE(pin,val)   do { \
	_GENGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define GENGPIO_PIN_READ(pin)  _GENGPIO_PIN_IN(pin)

#define GENGPIO_MASK_WRITE(mask,valmap)   do { \
	_genGpioMapO = (_genGpioMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define GENGPIO_MASK_READ(mask)  (_genGpioMapI & (mask))

#define GENGPIO_MASK_SET(mask)   do { \
	_genGpioMapO |= (mask); \
	} while (0)

#define GENGPIO_MASK_CLR(mask)   do { \
	_genGpioMapO &= ~(mask); \
	} while (0)

#if SPCA5110
#define GENGPIO_PIN_CFG(pin,dir)   do { \
	if (pin < 6) _GENGPIO_PIN_PSEL(pin) = 0; \
	else if (pin == 8) _GENGPIO_PIN_ASEL(0) = 0; \
	else if (pin == 10) _GENGPIO_PIN_ASEL(1) = 0; \
	else if (pin == 11) _GENGPIO_PIN_ASEL(2) = 0; \
	else if (pin == 12) _GENGPIO_PIN_ASEL(3) = 0; \
	else if (pin == 13) _GENGPIO_PIN_ASEL(4) = 0; \
	_GENGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define GENGPIO_MASK_CFG(mask,dirmap)   do { \
	_genGpioMapPS &= ~((mask) & 0x3f); \
	_genGpioMapAS &= ~((((mask) >> 8) & 1) | (((mask) >> 9) & 0x1e)); \
	_genGpioMapD = (_genGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#else
#define GENGPIO_PIN_CFG(pin,dir)   do { \
	if (pin < 8) _GENGPIO_PIN_PSEL(pin) = 0; \
	else if (pin == 8) _GENGPIO_PIN_ASEL(0) = 0; \
	else if (pin == 9) _GENGPIO_PIN_ASEL(1) = 0; \
	else if (pin == 10) _GENGPIO_PIN_ASEL(2) = 0; \
	else if (pin == 11) _GENGPIO_PIN_ASEL(3) = 0; \
	else if (pin == 12) _GENGPIO_PIN_ASEL(4) = 0; \
	else if (pin == 13) _GENGPIO_PIN_ASEL(5) = 0; \
	_GENGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define GENGPIO_MASK_CFG(mask,dirmap)   do { \
	_genGpioMapPS &= ~((mask) & 0xff); \
	_genGpioMapAS &= ~(((mask) >> 8) & 0x3f); \
	_genGpioMapD = (_genGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#endif

/**< ------FMGPIO-------------------------------------*/
#define FMGPIO_PIN_WRITE(pin,val)   do { \
	_FMGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define FMGPIO_PIN_READ(pin)  _FMGPIO_PIN_IN(pin)

#define FMGPIOH_PIN_WRITE(pin,val)   do { \
	_FMGPIOH_PIN_OUT(pin) = (val); \
	} while (0)

#define FMGPIOH_PIN_READ(pin)  _FMGPIOH_PIN_IN(pin)

#define FMGPIOL_MASK_WRITE(mask,valmap)   do { \
	_fmGpioLMapO = (_fmGpioLMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define FMGPIOL_MASK_READ(mask)  (_fmGpioLMapI & (mask))

#define FMGPIOL_MASK_SET(mask)   do { \
	_fmGpioLMapO |= (mask); \
	} while (0)

#define FMGPIOL_MASK_CLR(mask)   do { \
	_fmGpioLMapO &= ~(mask); \
	} while (0)

#define FMGPIOH_MASK_WRITE(mask,valmap)   do { \
	_fmGpioHMapO = (_fmGpioHMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define FMGPIOH_MASK_READ(mask)  (_fmGpioHMapI & (mask))

#define FMGPIOH_MASK_SET(mask)   do { \
	_fmGpioHMapO |= (mask); \
	} while (0)

#define FMGPIOH_MASK_CLR(mask)   do { \
	_fmGpioHMapO &= ~(mask); \
	} while (0)

#if SPCA5110
#define FMGPIO_PIN_CFG(pin,dir)   do { \
	_FMGPIO_PIN_SEL(pin) = 1; \
	_FMGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define FMGPIOH_PIN_CFG(pin,dir)   do { \
	_FMGPIOH_PIN_SEL(pin) = 1; \
	_FMGPIOH_PIN_DIR(pin) = (dir); \
	} while (0)

#define FMGPIOL_MASK_CFG(mask,dirmap)   do { \
	_fmGpioLMapS |= (mask); \
	_fmGpioLMapD = (_fmGpioLMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define FMGPIOH_MASK_CFG(mask,dirmap)   do { \
	_fmGpioHMapS |= (mask); \
	_fmGpioHMapD = (_fmGpioHMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#else
#define FMGPIO_PIN_CFG(pin,dir)   do { \
	_FMGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define FMGPIOH_PIN_CFG(pin,dir)   do { \
	_FMGPIOH_PIN_DIR(pin) = (dir); \
	} while (0)

#define FMGPIOL_MASK_CFG(mask,dirmap)   do { \
	_fmGpioLMapD = (_fmGpioLMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define FMGPIOH_MASK_CFG(mask,dirmap)   do { \
	_fmGpioHMapD = (_fmGpioHMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#endif

/**< ------TGGPIO-------------------------------------*/
#define TGGPIO_PIN_WRITE(pin,val)   do { \
	_TGGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define TGGPIO_PIN_READ(pin)  _TGGPIO_PIN_IN(pin)

#if SPCA5110
#define TGGPIO_PIN_CFG(pin,dir)   do { \
	_TGGPIO_PIN_SEL(pin) = 1; \
	_TGGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define TGGPIO_MASK_CFG(mask,dirmap)   do { \
	_tgGpioMapS |= (mask); \
	_tgGpioMapD = (_tgGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define TGGPIO_MASK_WRITE(mask,valmap)   do { \
	_tgGpioMapO = (_tgGpioMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define TGGPIO_MASK_READ(mask)  (_tgGpioMapI & (mask))

#define TGGPIO_MASK_SET(mask)   do { \
	_tgGpioMapO |= (mask); \
	} while (0)

#define TGGPIO_MASK_CLR(mask)   do { \
	_tgGpioMapO &= ~(mask); \
	} while (0)

#else
#define TGGPIOH_PIN_WRITE(pin,val)   do { \
	_TGGPIOH_PIN_OUT(pin) = (val); \
	} while (0)

#define TGGPIOH_PIN_READ(pin)  _TGGPIOH_PIN_IN(pin)

#define TGGPIO_PIN_CFG(pin,dir)   do { \
	_TGGPIO_PIN_POL(pin) = 0; \
	_TGGPIO_PIN_SEL(pin) = 1; \
	_TGGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define TGGPIOH_PIN_CFG(pin,dir)   do { \
	_TGGPIOH_PIN_POL(pin) = 0; \
	_TGGPIOH_PIN_SEL(pin) = 1; \
	_TGGPIOH_PIN_DIR(pin) = (dir); \
	} while (0)

#define TGGPIOL_MASK_CFG(mask,dirmap)   do { \
	_tgGpioLMapP &= ~(mask); \
	_tgGpioLMapS |= (mask); \
	_tgGpioLMapD = (_tgGpioLMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define TGGPIOL_MASK_WRITE(mask,valmap)   do { \
	_tgGpioLMapO = (_tgGpioLMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define TGGPIOL_MASK_READ(mask)  (_tgGpioLMapI & (mask))

#define TGGPIOL_MASK_SET(mask)   do { \
	_tgGpioLMapO |= (mask); \
	} while (0)

#define TGGPIOL_MASK_CLR(mask)   do { \
	_tgGpioLMapO &= ~(mask); \
	} while (0)

#define TGGPIOH_MASK_CFG(mask,dirmap)   do { \
	_tgGpioHMapP &= ~(mask); \
	_tgGpioHMapS |= (mask); \
	_tgGpioHMapD = (_tgGpioHMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define TGGPIOH_MASK_WRITE(mask,valmap)   do { \
	_tgGpioHMapO = (_tgGpioHMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define TGGPIOH_MASK_READ(mask)  (_tgGpioHMapI & (mask))

#define TGGPIOH_MASK_SET(mask)   do { \
	_tgGpioHMapO |= (mask); \
	} while (0)

#define TGGPIOH_MASK_CLR(mask)   do { \
	_tgGpioHMapO &= ~(mask); \
	} while (0)

#endif

/**< ------LMIGPIO------------------------------------*/
#if SPCA5110
#define LMIGPIO_PIN_WRITE(pin,val)   do { \
	_LMIGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define LMIGPIO_PIN_READ(pin)  _LMIGPIO_PIN_IN(pin)

#define LMIGPIO_PIN_CFG(pin,dir)   do { \
	_LMIGPIO_PIN_SEL(pin) = 1; \
	_LMIGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define LMIGPIO_MASK_CFG(mask,dirmap)   do { \
	_lmiGpioMapS |= (mask); \
	_lmiGpioMapD = (_lmiGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define LMIGPIO_MASK_WRITE(mask,valmap)   do { \
	_lmiGpioMapO = (_lmiGpioMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define LMIGPIO_MASK_READ(mask)  (_lmiGpioMapI & (mask))

#define LMIGPIO_MASK_SET(mask)   do { \
	_lmiGpioMapO |= (mask); \
	} while (0)

#define LMIGPIO_MASK_CLR(mask)   do { \
	_lmiGpioMapO &= ~(mask); \
	} while (0)
#endif

/**< ------DISPGPIO-------------------------------------*/
#define DISPGPIO_PIN_WRITE(pin,val)   do { \
	_DISPGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define DISPGPIO_PIN_READ(pin)  _DISPGPIO_PIN_IN(pin)

#if SPCA5110
#define DISPGPIO_PIN_CFG(pin,dir)   do { \
	_DISPGPIO_PIN_SEL(pin) = 1; \
	_DISPGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define DISPGPIO_MASK_CFG(mask,dirmap)   do { \
	_dispGpioMapS |= (mask); \
	_dispGpioMapD = (_dispGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define DISPGPIO_MASK_WRITE(mask,valmap)   do { \
	_dispGpioMapO = (_dispGpioMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define DISPGPIO_MASK_READ(mask)  (_dispGpioMapI & (mask))

#define DISPGPIO_MASK_SET(mask)   do { \
	_dispGpioMapO |= (mask); \
	} while (0)

#define DISPGPIO_MASK_CLR(mask)   do { \
	_dispGpioMapO &= ~(mask); \
	} while (0)

#else
#define DISPGPIO_PIN_CFG(pin,dir)   do { \
	_DISPGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

/** \note disp gpio register address is not properly aligned,
 * we are forced to use byte access */

#define DISPGPIOA_MASK_CFG(mask,dirmap)   do { \
	_dispGpioAMapD = (_dispGpioAMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define DISPGPIOA_MASK_WRITE(mask,valmap)   do { \
	_dispGpioAMapO = (_dispGpioAMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define DISPGPIOA_MASK_READ(mask)  (_dispGpioAMapI & (mask))

#define DISPGPIOA_MASK_SET(mask)   do { \
	_dispGpioAMapO |= (mask); \
	} while (0)

#define DISPGPIOA_MASK_CLR(mask)   do { \
	_dispGpioAMapO &= ~(mask); \
	} while (0)

#define DISPGPIOB_MASK_CFG(mask,dirmap)   do { \
	_dispGpioBMapD = (_dispGpioBMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define DISPGPIOB_MASK_WRITE(mask,valmap)   do { \
	_dispGpioBMapO = (_dispGpioBMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define DISPGPIOB_MASK_READ(mask)  (_dispGpioBMapI & (mask))

#define DISPGPIOB_MASK_SET(mask)   do { \
	_dispGpioBMapO |= (mask); \
	} while (0)

#define DISPGPIOB_MASK_CLR(mask)   do { \
	_dispGpioBMapO &= ~(mask); \
	} while (0)

#define DISPGPIOC_MASK_CFG(mask,dirmap)   do { \
	_dispGpioCMapD = (_dispGpioCMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define DISPGPIOC_MASK_WRITE(mask,valmap)   do { \
	_dispGpioCMapO = (_dispGpioCMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define DISPGPIOC_MASK_READ(mask)  (_dispGpioCMapI & (mask))

#define DISPGPIOC_MASK_SET(mask)   do { \
	_dispGpioCMapO |= (mask); \
	} while (0)

#define DISPGPIOC_MASK_CLR(mask)   do { \
	_dispGpioCMapO &= ~(mask); \
	} while (0)

#define DISPGPIOD_MASK_CFG(mask,dirmap)   do { \
	_dispGpioDMapD = (_dispGpioDMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define DISPGPIOD_MASK_WRITE(mask,valmap)   do { \
	_dispGpioDMapO = (_dispGpioDMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define DISPGPIOD_MASK_READ(mask)  (_dispGpioDMapI & (mask))

#define DISPGPIOD_MASK_SET(mask)   do { \
	_dispGpioDMapO |= (mask); \
	} while (0)

#define DISPGPIOD_MASK_CLR(mask)   do { \
	_dispGpioDMapO &= ~(mask); \
	} while (0)

#endif

/**< ------POCUGPIO-------------------------------------*/
#if SPCA5110
#define POCUGPIO_PIN_READ(pin)  _POCUGPIO_PIN_IN(pin)
#define POCUGPIO_MASK_READ(mask)  (_pocuGpioMapI & (mask))
#endif

#if SPCA5330
/**< ------SARGPIO--------------------------------------*/
#define SARGPIO_PIN_WRITE(pin,val)   do { \
	_SARGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define SARGPIO_PIN_READ(pin)  _SARGPIO_PIN_IN(pin)

#define SARGPIO_PIN_CFG(pin,dir)   do { \
	_SARGPIO_PIN_IEN(pin) = !(dir); \
	_SARGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define SARGPIO_MASK_CFG(mask,dirmap)   do { \
	_sarGpioMapR = (_sarGpioMapR & ~((mask) >> 4)) | ((~(dirmap) & (mask)) >> 4); \
	_sarGpioMapD = (_sarGpioMapD & ~((mask) >> 4)) | (((dirmap) & (mask)) >> 4); \
	} while (0)

#define SARGPIO_MASK_WRITE(mask,valmap)   do { \
	_sarGpioMapO = (_sarGpioMapO & ~((mask) >> 4)) | (((valmap) & (mask)) >> 4); \
	} while (0)

#define SARGPIO_MASK_READ(mask)  ((_sarGpioMapI & ((mask) >> 4)) << 4)

#define SARGPIO_MASK_SET(mask)   do { \
	_sarGpioMapO |= ((mask) >> 4); \
	} while (0)

#define SARGPIO_MASK_CLR(mask)   do { \
	_sarGpioMapO &= ~((mask) >> 4); \
	} while (0)

/**< ------OISGPIO--------------------------------------*/
#define OISGPIO_PIN_WRITE(pin,val)   do { \
	_OISGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define OISGPIO_PIN_READ(pin)  _OISGPIO_PIN_IN(pin)

#define OISGPIO_PIN_CFG(pin,dir)   do { \
	_OISGPIO_PIN_IEN(pin) = !(dir); \
	_OISGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define OISGPIO_MASK_CFG(mask,dirmap)   do { \
	_oisGpioMapR = (_oisGpioMapR & ~((mask) << 4)) | ((~(dirmap) & (mask)) << 4); \
	_oisGpioMapD = (_oisGpioMapD & ~((mask) << 4)) | (((dirmap) & (mask)) << 4); \
	} while (0)

#define OISGPIO_MASK_WRITE(mask,valmap)   do { \
	_oisGpioMapO = (_oisGpioMapO & ~((mask) << 4)) | (((valmap) & (mask)) << 4); \
	} while (0)

#define OISGPIO_MASK_READ(mask)  ((_oisGpioMapI & ((mask) << 4)) >> 4)

#define OISGPIO_MASK_SET(mask)   do { \
	_oisGpioMapO |= ((mask) << 4); \
	} while (0)

#define OISGPIO_MASK_CLR(mask)   do { \
	_oisGpioMapO &= ~((mask) << 4); \
	} while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /*_GPIO_DEF_H_*/
